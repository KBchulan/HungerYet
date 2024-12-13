#include "tcpmanager.h"
#include "logindialog.h"
#include "httpmanager.h"
#include "ui_logindialog.h"

#include <QDebug>
#include <QtMath>
#include <QPainter>
#include <QParallelAnimationGroup>
#include <QGraphicsDropShadowEffect>

LoginDialog::LoginDialog(QWidget *parent)
    : BackgroundDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    
    // 设置背景图片和透明度
    setBackground(":/resources/Login/8.jpg", 0.3);
    
    // 注册按钮
    connect(ui->reg_btn, &QPushButton::clicked, this, &LoginDialog::switchRegister);

    // 忘记密码的样式
    ui->forget_label->SetState("normal", "hover", "hover", "selected", "selected_hover", "selected_hover");

    // 跳转到reset界面
    connect(ui->forget_label, &ClickedLabel::sig_clicked, this, &LoginDialog::slot_forget_pwd);

    // 设置头像
    initHead();

    // 初始化httphandler
    initHttpHandlers();

    // 设置密码是否可见
    ui->pass_edit->setEchoMode(QLineEdit::Password);
    ui->pass_visible->SetState("unvisible", "unvisible_hover", "unvisible_hover", "visible", "visible_hover", "visible_hover");

    // 连接登录回包的信号
    connect(HttpManager::GetInstance().get(), &HttpManager::sig_login_mod_finish, this,
            &LoginDialog::slot_login_mod_finish);
    
    // 密码是否可见的点击事件
    connect(ui->pass_visible, &ClickedLabel::sig_clicked, this, [this]
    {
        auto state = ui->pass_visible->GetCurState();
        if(state == ClickLbState::Normal)
            ui->pass_edit->setEchoMode(QLineEdit::Password);
        else
            ui->pass_edit->setEchoMode(QLineEdit::Normal);
    });

    // tcp请求的信号和槽函数
    connect(this, &LoginDialog::sig_connect_tcp, TcpManager::GetInstance().get(), &TcpManager::slot_tcp_connect);
    // tcp管理者发出的成功信号
    connect(TcpManager::GetInstance().get(), &TcpManager::sig_con_success, this, &LoginDialog::slot_tcp_con_finished);
    // tcp管理者发出的信号
    connect(TcpManager::GetInstance().get(), &TcpManager::sig_login_failed, this, &LoginDialog::slot_login_failed);

    initParticleEffect();
}

LoginDialog::~LoginDialog()
{
    qDebug() << "login destruce";
    delete ui;
    m_particleTimer->stop();
}

void LoginDialog::initParticleEffect()
{
    // 创建粒��效果
    m_particleEffect = new ParticleEffect(this);
    m_particleEffect->resize(size());
    m_particleEffect->lower();

    // 创建定时器以定期添加新粒子
    m_particleTimer = new QTimer(this);
    connect(m_particleTimer, &QTimer::timeout, this, &LoginDialog::addRandomParticle);
    m_particleTimer->start(1000);

    std::uint16_t particleCount = (qrand() % 4);
    
    for (std::uint16_t i = 0; i < particleCount; i++)
        addRandomParticle();

    m_particleEffect->start();
}

void LoginDialog::addRandomParticle()
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

void LoginDialog::initHead()
{
    QPixmap pixmap(":/resources/Login/3.jpg");

    pixmap = pixmap.scaled(ui->head_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QSize targetSize = ui->head_label->size();
    QPixmap rounded(targetSize);
    rounded.fill(Qt::transparent);

    QPainter painter(&rounded);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect imageRect(125, 0, pixmap.width(), pixmap.height());

    QPainterPath path;
    std::uint32_t radius = 20;
    path.addRoundedRect(imageRect, radius, radius);

    painter.setClipPath(path);

    painter.drawPixmap(imageRect.x(), imageRect.y(), pixmap);

    QPen pen(QColor(255, 192, 203, 115));
    pen.setWidth(4);
    painter.setPen(pen);
    painter.drawRoundedRect(imageRect, radius, radius);

    ui->head_label->setPixmap(rounded);
}

void LoginDialog::showTip(QString str, bool b_ok)
{
    if(b_ok)
        ui->err_tip->setProperty("state", "normal");
    else
        ui->err_tip->setProperty("state", "err");
        
    ui->err_tip->setText(str);
    repolish(ui->err_tip);
}

void LoginDialog::AddTipErr(TipErr te, QString tips)
{
    _tip_errs[te] = tips;
    showTip(tips, false);
}

void LoginDialog::DelTipErr(TipErr te)
{
    _tip_errs.remove(te);
    if(_tip_errs.empty())
    {
        ui->err_tip->clear();
        return;
    }
    showTip(_tip_errs.first(), false);
}

bool LoginDialog::enableBtn(bool enabled)
{
    ui->login_btn->setEnabled(enabled);
    ui->reg_btn->setEnabled(enabled);
    return true;
}

void LoginDialog::initHttpHandlers()
{
    _handlers.insert(ReqId::ID_LOGIN_USER, [this](QJsonObject jsonObj)
    {
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS)
        {
            if(error == 1009)
            {
                showTip("密码错误", false);
                enableBtn(true);
                return;
            }
            showTip("参数错误", false);
            enableBtn(true);
            return;
        }

        auto email = jsonObj["email"].toString();

        ServerInfo si;
        si.Uid = jsonObj["uid"].toInt();
        si.Host = jsonObj["host"].toString();
        si.Port = jsonObj["port"].toString();
        si.Token = jsonObj["token"].toString();

        _uid = si.Uid;
        _token = si.Token;

        qDebug() << "email: " << email << " uid: " << si.Uid << " token: " << si.Token;

        emit sig_connect_tcp(si);
    });
}

bool LoginDialog::checkUserValid()
{
    auto email = ui->email_edit->text();

    // （nums + letters + specials) + '@' + （nums + letters + specials) + '.' + (at least two letter)
    QRegularExpression regex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    bool match = regex.match(email).hasMatch();

    if(!match)
    {
        AddTipErr(TipErr::TIP_EMAIL_ERR, tr("邮箱地址不正确"));
        return false;
    }

    DelTipErr(TipErr::TIP_EMAIL_ERR);
    return true;
}

bool LoginDialog::checkPwdVaild()
{
    auto password = ui->pass_edit->text();

    // 先检测长度
    if(password.length() < 6 || password.length() > 15)
    {
        AddTipErr(TipErr::TIP_PWD_ERR, tr("密码长度不准确,应在6~15位"));
        return false;
    }

    QRegularExpression regex("^(?=.*[A-Z])(?=.*[a-z])(?=.*\\d)[A-Za-z\\d!@#$%^&*()_+\\-=\\[\\]{};':\"\\\\|,.<>\\/?]{6,15}$");
    bool match = regex.match(password).hasMatch();

    if(!match)
    {
        AddTipErr(TipErr::TIP_PWD_ERR, tr("密码必须包含大小写字母和数字"));
        return false;
    }

    DelTipErr(TipErr::TIP_PWD_ERR);
    return true;
}

void LoginDialog::slot_forget_pwd()
{
    emit switchReset();
}

void LoginDialog::slot_tcp_con_finished(bool success)
{
    if(success)
    {
        showTip(tr("聊天服务器连接成功，正在登录"), true);
        QJsonObject jsonObj;
        jsonObj["uid"] = _uid;
        jsonObj["token"] = _token;

        QJsonDocument doc(jsonObj);
        QString jsonString = doc.toJson(QJsonDocument::Indented);

        // 发给ChatServer
        emit TcpManager::GetInstance()->sig_send_data(ReqId::ID_CHAT_LOGIN, jsonString);
    }
    else
    {
        showTip(tr("网络异常"), false);
        enableBtn(true);
    }
}

void LoginDialog::slot_login_failed(int err)
{
    QString result = QString("登录失败,err is %1").arg(err);
    showTip(result, false);
    enableBtn(true);
}

void LoginDialog::on_login_btn_clicked()
{
    if(!checkUserValid())
        return;

    if(!checkPwdVaild())
        return;

    enableBtn(false);
    auto email = ui->email_edit->text();
    auto passwd = ui->pass_edit->text();

    QJsonObject json_obj;
    json_obj["email"] = email;
    json_obj["passwd"] = xorString(passwd);
    HttpManager::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/user_login"),
                                            json_obj, ReqId::ID_LOGIN_USER, Modules::LOGINMOD);
}

void LoginDialog::slot_login_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    if(err != ErrorCodes::SUCCESS)
    {
        showTip(tr("网络请求错误"), false);
        return;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());

    if(jsonDoc.isNull())
    {
        showTip(tr("json解析错误"), false);
        return;
    }

    _handlers[id](jsonDoc.object());
    return;
}
