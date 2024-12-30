#include "../include/MysqlDao.h"
#include "../include/LogManager.h"
#include "../include/ConfigManager.h"

SqlConnection::SqlConnection(sql::Connection *con, std::uint64_t lastTime)
    : _con(con), _last_oper_time(lastTime)
{
}

MysqlPool::MysqlPool(const std::string &url, const std::string &user, const std::string &passwd, const std::string &schema, std::uint32_t poolSize)
    : _url(url), _user(user), _passwd(passwd), _schema(schema), _poolSize(poolSize), _b_stop(false)
{
    try
    {
        for (uint32_t i = 0; i < _poolSize; i++)
        {
            sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
            auto *con = driver->connect(_url, _user, _passwd);
            con->setSchema(_schema);

            // 获取当前时间戳
            auto currentTime = std::chrono::system_clock::now().time_since_epoch();

            long long timeStamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();

            _pool.push(std::make_unique<SqlConnection>(con, timeStamp));
        }

        _check_thread = std::thread([this]
        {
            while (!_b_stop)
            {
                CheckConnection();
                std::this_thread::sleep_for(std::chrono::seconds(60));
            } 
        });
    }
    catch (const std::exception &e)
    {
        LOG_SQL->error(R"({} : {})", __FILE__, "Mysql pool init failed!");
    }
}

void MysqlPool::CheckConnection()
{
    std::lock_guard<std::mutex> guard(_mutex);
    int poolSize = _pool.size();

    auto currentTime = std::chrono::system_clock::now().time_since_epoch();
    long long timeStamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();

    for (uint32_t i = 0; i < poolSize; i++)
    {
        auto con = std::move(_pool.front());
        _pool.pop();

        Defer defer([this, &con]
                    { _pool.push(std::move(con)); });

        if (timeStamp - con->_last_oper_time < 600)
            continue;

        try
        {
            std::unique_ptr<sql::Statement> stmt(con->_con->createStatement());
            stmt->executeQuery("SELECT 1");
            con->_last_oper_time = timeStamp;
            LOG_SQL->info("execute timer alive query, cur is {}", con.get()->_last_oper_time);
        }
        catch (const std::exception &e)
        {
            // 检测不到心跳就重新生成一个连接
            LOG_SQL->error("execute timer alive query, cur is {}", e.what());
            sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
            auto *newCon = driver->connect(_url, _user, _passwd);
            newCon->setSchema(_schema);
            con->_con.reset(newCon);
            con->_last_oper_time = timeStamp;
        }
    }
}

std::unique_ptr<SqlConnection> MysqlPool::GetConnection()
{
    std::unique_lock<std::mutex> lock(_mutex);

    // false阻塞
    _cond.wait(lock, [this]
               {
        if(_b_stop)
            return true;
        return !_pool.empty(); });

    if (_b_stop)
        return nullptr;

    std::unique_ptr<SqlConnection> con = std::move(_pool.front());
    _pool.pop();
    return con;
}

void MysqlPool::ReturnConnection(std::unique_ptr<SqlConnection> con)
{
    std::lock_guard<std::mutex> guard(_mutex);
    if (_b_stop)
        return;

    _pool.push(std::move(con));
    _cond.notify_one();
}

void MysqlPool::Close()
{
    _b_stop = true;
    _cond.notify_all();
}

MysqlPool::~MysqlPool()
{
    if (_check_thread.joinable())
        _check_thread.join();

    std::lock_guard<std::mutex> guard(_mutex);
    while (!_pool.empty())
        _pool.pop();
}

MysqlDao::MysqlDao()
{
    auto &gCfgMgr = ConfigManager::GetInstance();
    std::string host = gCfgMgr["Mysql"]["host"];
    std::string port = gCfgMgr["Mysql"]["port"];
    std::string user = gCfgMgr["Mysql"]["user"];
    std::string passwd = gCfgMgr["Mysql"]["password"];
    std::string schema = gCfgMgr["Mysql"]["schema"];
    _pool.reset(new MysqlPool(host + ":" + port, user, passwd, schema, std::thread::hardware_concurrency()));
}

MysqlDao::~MysqlDao()
{
    _pool->Close();
}

int MysqlDao::RegUser(const std::string &name, const std::string &email, const std::string &passwd)
{
    auto con = _pool->GetConnection();

    Defer defer([this, &con]
                { _pool->ReturnConnection(std::move(con)); });

    try
    {
        if (con == nullptr)
            return false;

        // 此处使用存储过程
        std::unique_ptr<sql::PreparedStatement> stmt(con->_con->prepareStatement("CALL reg_user(?,?,?,@result)"));
        stmt->setString(1, name);
        stmt->setString(2, email);
        stmt->setString(3, passwd);

        stmt->execute();

        // 执行存储过程
        std::unique_ptr<sql::Statement> stmtResult(con->_con->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmtResult->executeQuery("SELECT @result AS result"));

        if (res->next())
        {
            int result = res->getInt("result");
            LOG_SQL->info("reg user result is {}", result);
            return result;
        }
        return -1;
    }
    catch (sql::SQLException& e) 
    {
		LOG_SQL->error("SQLException: {} (MySQL error code: {}, SQLState: {})", e.what(), e.getErrorCode(), e.getSQLState());
		return -1;
	}
}

bool MysqlDao::CheckEmail(const std::string &name, const std::string &email)
{
    auto con = _pool->GetConnection();

    Defer defer([this, &con]
                { _pool->ReturnConnection(std::move(con)); });

    try
    {
        if(con == nullptr)
            return false;

        std::unique_ptr<sql::PreparedStatement> stmt(con->_con->prepareStatement("SELECT email FROM user WHERE name = ?"));
        stmt->setString(1, name);

        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());

        while(res->next())
        {
            std::string tmpemail = res->getString("email");
            LOG_SQL->info("check email result is {}", tmpemail);
            if(tmpemail == email)
                return true;
        }
        return false;
    }
    catch(sql::SQLException& e)
    {
        LOG_SQL->error("SQLException: {} (MySQL error code: {}, SQLState: {})", e.what(), e.getErrorCode(), e.getSQLState());
        return false;
    }
}

bool MysqlDao::UpdatePasswd(const std::string &name, const std::string &newpasswd)
{
    auto con = _pool->GetConnection();

    Defer defer([this, &con]
                { _pool->ReturnConnection(std::move(con)); });

    try
    {
        if(con == nullptr)
            return false;

        std::unique_ptr<sql::PreparedStatement> pstmt(con->_con->prepareStatement("UPDATE user SET passwd = ? WHERE name = ?"));
        pstmt->setString(1, newpasswd);
        pstmt->setString(2, name);
        LOG_SQL->info("update passwd, name is {}, newpasswd is {}", name, newpasswd);

        pstmt->executeUpdate();
        return true;
    }
    catch(sql::SQLException& e)
    {
        LOG_SQL->error("SQLException: {} (MySQL error code: {}, SQLState: {})", e.what(), e.getErrorCode(), e.getSQLState());
        return false;
    }
}

bool MysqlDao::CheckPasswd(const std::string &email, const std::string &passwd, UserInfo &userInfo)
{
    auto con = _pool->GetConnection();

    Defer defer([this, &con]
                { _pool->ReturnConnection(std::move(con)); });

    if(con == nullptr)
        return false;

    try
    {
        std::unique_ptr<sql::PreparedStatement> pstmt(con->_con->prepareStatement("SELECT * FROM user WHERE email = ?"));
        pstmt->setString(1, email);

        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        std::string origin_pwd = "";
        while(res->next())
        {
            origin_pwd = res->getString("passwd");
            LOG_SQL->info("check passwd, origin_pwd is {}", origin_pwd);
            break;
        }

        if(origin_pwd != passwd)
            return false;

        userInfo._passwd = origin_pwd;
        userInfo._uid = res->getInt("uid");
        userInfo._name = res->getString("name");
        userInfo._email = res->getString("email");
        return true;
    }
    catch(sql::SQLException& e)
    {
        LOG_SQL->error("SQLException: {} (MySQL error code: {}, SQLState: {})", e.what(), e.getErrorCode(), e.getSQLState());
        return false;
    }
}


std::shared_ptr<UserInfo> MysqlDao::GetUser(int uid)
{
	auto con = _pool->GetConnection();
	if (con == nullptr) {
		return nullptr;
	}

	Defer defer([this, &con]
    {
		_pool->ReturnConnection(std::move(con));
	});

	try 
    {
		std::unique_ptr<sql::PreparedStatement> pstmt(con->_con->prepareStatement("SELECT * FROM user WHERE uid = ?"));
		pstmt->setInt(1, uid);

		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		std::shared_ptr<UserInfo> user_ptr = nullptr;

		while (res->next())
        {
			user_ptr.reset(new UserInfo);
			user_ptr->_passwd = res->getString("passwd");
			user_ptr->_email = res->getString("email");
			user_ptr->_name = res->getString("name");
			user_ptr->_uid = res->getInt("uid");
			break;
		}
		return user_ptr;
	}
	catch (sql::SQLException& e) 
    {
		LOG_SQL->error("SQLException: {} (MySQL error code: {}, SQLState: {})", e.what(), e.getErrorCode(), e.getSQLState());
		return nullptr;
	}
}

std::shared_ptr<UserInfo> MysqlDao::GetUser(std::string name)
{
	auto con = _pool->GetConnection();
	if (con == nullptr) {
		return nullptr;
	}

	Defer defer([this, &con]
    {
		_pool->ReturnConnection(std::move(con));
	});

	try 
    {
		std::unique_ptr<sql::PreparedStatement> pstmt(con->_con->prepareStatement("SELECT * FROM user WHERE name = ?"));
		pstmt->setString(1, name);

		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		std::shared_ptr<UserInfo> user_ptr = nullptr;

		while (res->next()) 
        {
			user_ptr.reset(new UserInfo);
			user_ptr->_passwd = res->getString("passwd");
			user_ptr->_email = res->getString("email");
			user_ptr->_name = res->getString("name");
			user_ptr->_uid = res->getInt("uid");
			break;
		}
		return user_ptr;
	}
	catch (sql::SQLException& e) 
    {
		LOG_SQL->error("SQLException: {} (MySQL error code: {}, SQLState: {})", e.what(), e.getErrorCode(), e.getSQLState());
		return nullptr;
	}
}

bool MysqlDao::AddOrder(const std::string& order_id, int merchant_id, const std::string& order_items, 
                       const std::string& time, double total, const std::string& user_name, std::uint32_t status)
{
    auto con = _pool->GetConnection();
    
    Defer defer([this, &con]
                { _pool->ReturnConnection(std::move(con)); });

    try
    {
        if(con == nullptr)
            return false;

        std::unique_ptr<sql::PreparedStatement> pstmt(con->_con->prepareStatement(
            "INSERT INTO `orders` (order_id, merchant_id, order_items, time, total, user_name, status) "
            "VALUES (?, ?, ?, ?, ?, ?, ?)"));
            
        pstmt->setString(1, order_id);
        pstmt->setInt(2, merchant_id);
        pstmt->setString(3, order_items);
        pstmt->setString(4, time);
        pstmt->setDouble(5, total);
        pstmt->setString(6, user_name);
        pstmt->setInt(7, status);

        pstmt->executeUpdate();
        LOG_SQL->info("Add order success, order_id: {}, merchant_id: {}", order_id, merchant_id);
        return true;
    }
    catch(sql::SQLException& e)
    {
        LOG_SQL->error("SQLException: {} (MySQL error code: {}, SQLState: {})", 
                      e.what(), e.getErrorCode(), e.getSQLState());
        return false;
    }
}

std::vector<OrderInfo> MysqlDao::GetAllOrders()
{
    auto con = _pool->GetConnection();
    std::vector<OrderInfo> orders;

    Defer defer([this, &con]
                { _pool->ReturnConnection(std::move(con)); });

    try
    {
        if(con == nullptr)
            return orders;

        std::unique_ptr<sql::PreparedStatement> pstmt(con->_con->prepareStatement(
            "SELECT * FROM `orders` ORDER BY time DESC"));

        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        while(res->next())
        {
            OrderInfo order;
            order.order_id = res->getString("order_id");
            order.merchant_id = res->getInt("merchant_id");
            order.order_items = res->getString("order_items");
            order.time = res->getString("time");
            order.total = res->getDouble("total");
            order.user_name = res->getString("user_name");
            order.status = res->getInt("status");
            orders.push_back(order);
        }
        LOG_SQL->info("Get all orders, count: {}", orders.size());
        return orders;
    }
    catch(sql::SQLException& e)
    {
        LOG_SQL->error("SQLException: {} (MySQL error code: {}, SQLState: {})", 
                      e.what(), e.getErrorCode(), e.getSQLState());
        return orders;
    }
}

std::vector<OrderInfo> MysqlDao::GetOrders(int merchant_id)
{
    auto con = _pool->GetConnection();
    std::vector<OrderInfo> orders;

    Defer defer([this, &con]
                { _pool->ReturnConnection(std::move(con)); });

    try
    {
        if(con == nullptr)
            return orders;

        std::unique_ptr<sql::PreparedStatement> pstmt(con->_con->prepareStatement(
            "SELECT * FROM `orders` WHERE merchant_id = ? ORDER BY time DESC"));

        pstmt->setInt(1, merchant_id);

        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        while(res->next())
        {
            OrderInfo order;
            order.order_id = res->getString("order_id");
            order.merchant_id = res->getInt("merchant_id");
            order.order_items = res->getString("order_items");
            order.time = res->getString("time");
            order.total = res->getDouble("total");
            order.user_name = res->getString("user_name");
            order.status = res->getInt("status");
            orders.push_back(order);
        }
        LOG_SQL->info("Get orders, count: {}", orders.size());
        return orders;
    }
    catch(sql::SQLException& e)
    {
        LOG_SQL->error("SQLException: {} (MySQL error code: {}, SQLState: {})", 
                      e.what(), e.getErrorCode(), e.getSQLState());
        return orders;
    }
}

bool MysqlDao::UpdateOrderStatus(const std::string& order_id, std::uint32_t status)
{
    auto con = _pool->GetConnection();

    Defer defer([this, &con]
                { _pool->ReturnConnection(std::move(con)); });

    try
    {
        if(con == nullptr)
            return false;

        std::unique_ptr<sql::PreparedStatement> pstmt(con->_con->prepareStatement(
            "UPDATE `orders` SET status = ? WHERE order_id = ?"));

        pstmt->setInt(1, status);
        pstmt->setString(2, order_id);

        pstmt->executeUpdate();
        LOG_SQL->info("Update order status, order_id: {}, status: {}", order_id, status);
        return true;
    }
    catch(sql::SQLException& e)
    {
        LOG_SQL->error("SQLException: {} (MySQL error code: {}, SQLState: {})", e.what(), e.getErrorCode(), e.getSQLState());
        return false;
    }
}   