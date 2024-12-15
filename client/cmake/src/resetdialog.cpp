#include <global.h>
#include <resetdialog.h>
#include <httpmanager.h>
#include <ui_resetdialog.h>

#include <QDebug>
#include <QtMath>
#include <QRegularExpression>

ResetDialog::ResetDialog(QWidget *parent) 
    : BackgroundDialog(parent), ui(new Ui::ResetDialog)
{
    ui->setupUi(this);

    // 设置背景图片和透明度
    setBackground(":/resources/Login/7.jpg", 0.3);

    connect(ui->user_edit, &QLineEdit::editingFinished, this, [this]()
            { checkUserValid(); });

    connect(ui->email_edit, &QLineEdit::editingFinished, this, [this]()
            { checkEmailValid(); });

    connect(ui->pwd_edit, &QLineEdit::editingFinished, this, [this]()
            { checkPassValid(); });

    connect(ui->varify_edit, &QLineEdit::editingFinished, this, [this]()
            { checkVarifyValid(); });

    // 连接reset相关信号和注册处理回调
    initHandlers();
    connect(HttpManager::GetInstance().get(), &HttpManager::sig_reset_mod_finish, this,
            &ResetDialog::slot_reset_mod_finish);
    
    // 设置密码是否可见
    ui->pwd_edit->setEchoMode(QLineEdit::Password);
    ui->pass_visible->SetState("unvisible", "unvisible_hover", "unvisible_hover", "visible", "visible_hover", "visible_hover");

    // 设置密码样式
    connect(ui->pass_visible, &ClickedLabel::sig_clicked, this, [this]
    {
        auto state = ui->pass_visible->GetCurState();
        if(state == ClickLbState::Normal)
            ui->pwd_edit->setEchoMode(QLineEdit::Password);
        else
            ui->pwd_edit->setEchoMode(QLineEdit::Normal);
    });

    initParticleEffect();
}

ResetDialog::~ResetDialog()
{
    qDebug() << "destruct reset";
    delete ui;
}

void ResetDialog::initParticleEffect()
{
    // 创建粒子效果
    m_particleEffect = new ParticleEffect(this);
    m_particleEffect->resize(size());
    m_particleEffect->lower();

    // 创建定时器以定期添加新粒子
    m_particleTimer = new QTimer(this);
    connect(m_particleTimer, &QTimer::timeout, this, &ResetDialog::addRandomParticle);
    m_particleTimer->start(1000);

    std::uint16_t particleCount = (qrand() % 10);
    
    for (std::uint16_t i = 0; i < particleCount; i++)
        addRandomParticle();

    m_particleEffect->start();
}

void ResetDialog::addRandomParticle()
{
    // 随机决定这次添加多少个粒子(2-5个)
    int particleCount = 2 + (qrand() % 4);

    for (int i = 0; i < particleCount; ++i)
    {
        int edge = qrand() % 4;
        int x = 0, y = 0;

        switch (edge)
        {
        case 0:
            x = qrand() % width();
            y = -10;
            break;
        case 1:
            x = width() + 10;
            y = qrand() % height();
            break;
        case 2:
            x = qrand() % width();
            y = height() + 10;
            break;
        case 3:
            x = -10;
            y = qrand() % height();
            break;
        }

        qreal size = 5 + (qrand() % 15);

        QColor color;
        switch (qrand() % 3)
        {
        case 0:
            color = QColor(100 + qrand() % 50, 150 + qrand() % 50, 255, 150);
            break;
        case 1:
            color = QColor(150 + qrand() % 50, 100 + qrand() % 50, 255, 150);
            break;
        case 2:
            color = QColor(100 + qrand() % 50, 255, 200 + qrand() % 55, 150);
            break;
        }

        // 随机形状
        ParticleEffect::ShapeType shape = static_cast<ParticleEffect::ShapeType>(qrand() % 5);

        // 计算向中心的速度方向
        QPointF center(width() / 2, height() / 2);
        QPointF pos(x, y);
        QPointF direction = center - pos;
        qreal length = qSqrt(direction.x() * direction.x() + direction.y() * direction.y());
        if (length > 0)
            direction /= length;

        // 添加一个随机偏移量使运动更自然
        direction.rx() += (qrand() % 100 - 50) / 100.0;
        direction.ry() += (qrand() % 100 - 50) / 100.0;

        m_particleEffect->addParticle(pos, shape, size, qrand() % 360, color, direction);
    }
}

void ResetDialog::on_return_btn_clicked()
{
    emit switchLogin();
}

void ResetDialog::on_varify_btn_clicked()
{
    auto email = ui->email_edit->text();

    if (!checkEmailValid())
        return;

    // 发送http请求获取验证码
    QJsonObject json_obj;
    json_obj["email"] = email;
    HttpManager::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/get_varifycode"),
                                            json_obj, ReqId::ID_GET_VARIFY_CODE, Modules::RESETMOD);
}

void ResetDialog::slot_reset_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    if (err != ErrorCodes::SUCCESS)
    {
        showTip(tr("网络请求错误"), false);
        return;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());

    // json解析错误
    if (jsonDoc.isNull())
    {
        showTip(tr("json解析错误"), false);
        return;
    }

    // json解析错误
    if (!jsonDoc.isObject())
    {
        showTip(tr("json解析错误"), false);
        return;
    }

    // 调用对应的逻辑,根据id回调。
    _handlers[id](jsonDoc.object());

    return;
}

bool ResetDialog::checkUserValid()
{
    if (ui->user_edit->text() == "")
    {
        AddTipErr(TipErr::TIP_USER_ERR, tr("用户名不能为空"));
        return false;
    }

    DelTipErr(TipErr::TIP_USER_ERR);
    return true;
}

bool ResetDialog::checkPassValid()
{
    auto pass = ui->pwd_edit->text();

    if (pass.length() < 6 || pass.length() > 15)
    {
        // 提示长度不准确
        AddTipErr(TipErr::TIP_PWD_ERR, tr("密码长度应为6~15"));
        return false;
    }

    // 可以是字母、数字和特定的特殊字符
    QRegularExpression regExp("^(?=.*[A-Z])(?=.*[a-z])(?=.*\\d)[A-Za-z\\d!@#$%^&*()_+\\-=\\[\\]{};':\"\\\\|,.<>\\/?]{6,15}$");

    bool match = regExp.match(pass).hasMatch();

    if (!match)
    {
        AddTipErr(TipErr::TIP_PWD_ERR, tr("不能包含非法字符"));
        return false;
    }

    DelTipErr(TipErr::TIP_PWD_ERR);

    return true;
}

bool ResetDialog::checkEmailValid()
{
    auto email = ui->email_edit->text();

    // 邮箱地址的正则表达式
    QRegularExpression regex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    bool match = regex.match(email).hasMatch();

    if (!match)
    {
        AddTipErr(TipErr::TIP_EMAIL_ERR, tr("邮箱地址不正确"));
        return false;
    }

    DelTipErr(TipErr::TIP_EMAIL_ERR);
    return true;
}

bool ResetDialog::checkVarifyValid()
{
    auto pass = ui->varify_edit->text();

    if (pass.isEmpty())
    {
        AddTipErr(TipErr::TIP_VARIFY_ERR, tr("验证码不能为空"));
        return false;
    }

    DelTipErr(TipErr::TIP_VARIFY_ERR);
    return true;
}

void ResetDialog::AddTipErr(TipErr te, QString tips)
{
    _tip_errs[te] = tips;
    showTip(tips, false);
}

void ResetDialog::DelTipErr(TipErr te)
{
    _tip_errs.remove(te);

    if (_tip_errs.empty())
    {
        ui->err_tip->clear();
        return;
    }

    showTip(_tip_errs.first(), false);
}

void ResetDialog::initHandlers()
{
    // 注册获取验证码回包逻辑
    _handlers.insert(ReqId::ID_GET_VARIFY_CODE, [this](QJsonObject jsonObj)
    {
        int error = jsonObj["error"].toInt();

        if(error != ErrorCodes::SUCCESS)
        {
            showTip(tr("参数错误"),false);
            return;
        }

        auto email = jsonObj["email"].toString();
        showTip(tr("验证码已发送到邮箱，注意查收"), true);
    });

    // 注册注册用户回包逻辑
    _handlers.insert(ReqId::ID_RESET_PWD, [this](QJsonObject jsonObj)
    {
        int error = jsonObj["error"].toInt();

        if(error != ErrorCodes::SUCCESS)
        {
            showTip(tr("参数错误"),false);
            return;
        }
        auto email = jsonObj["email"].toString();
        showTip(tr("重置成功,点击返回登录"), true);
    });
}

void ResetDialog::showTip(QString str, bool b_ok)
{
    if (b_ok)
        ui->err_tip->setProperty("state", "normal");
    else
        ui->err_tip->setProperty("state", "err");

    ui->err_tip->setText(str);

    repolish(ui->err_tip);
}

void ResetDialog::on_sure_btn_clicked()
{
    bool valid = checkUserValid();
    if (!valid)
        return;

    valid = checkEmailValid();
    if (!valid)
        return;

    valid = checkPassValid();
    if (!valid)
        return;

    valid = checkVarifyValid();
    if (!valid)
        return;

    // 发送http重置用户请求
    QJsonObject json_obj;
    json_obj["user"] = ui->user_edit->text();
    json_obj["email"] = ui->email_edit->text();
    json_obj["passwd"] = xorString(ui->pwd_edit->text());
    json_obj["varifycode"] = ui->varify_edit->text();
    
    HttpManager::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/reset_pwd"),
                                            json_obj, ReqId::ID_RESET_PWD, Modules::RESETMOD);
}
