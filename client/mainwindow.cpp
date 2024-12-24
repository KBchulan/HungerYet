#include "tcpmanager.h"
#include "mainwindow.h"
#include "usermanager.h"
#include "ui_mainwindow.h"

#include <QTimer>
#include <QPropertyAnimation>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // design ico
    setWindowIcon(QIcon(":/resources/icos/MainWindow.ico"));

    _begin_dlg = new BeginDialog(this);
    _begin_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    setCentralWidget(_begin_dlg);
    _begin_dlg->show();

    connect(_begin_dlg, &BeginDialog::SigSwitchToLogin, this, &MainWindow::SlotSwitchLogin0);
    connect(_begin_dlg, &BeginDialog::SigSwitchToAdmin, this, &MainWindow::SlotSwitchAdmin);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SlotSwitchReg()
{
    _reg_dlg = new RegisterDialog(this);

    _reg_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    setCentralWidget(_reg_dlg);
    _login_dlg->hide();
    _reg_dlg->show();

    connect(_reg_dlg, &RegisterDialog::sigSwitchLogin, this, &MainWindow::SlotSwitchLogin1);
}

void MainWindow::SlotSwitchLogin0()
{
    _login_dlg = new LoginDialog(this);
    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    setCentralWidget(_login_dlg);
    _begin_dlg->hide();
    _login_dlg->show();

    connect(_login_dlg, &LoginDialog::switchRegister, this, &MainWindow::SlotSwitchReg);
    connect(_login_dlg, &LoginDialog::switchReset, this, &MainWindow::SlotSwitchReset);
    connect(TcpManager::GetInstance().get(), &TcpManager::sig_switch_chatdialog, this, &MainWindow::SlotSwitchApp);
}

void MainWindow::SlotSwitchLogin1()
{
    _login_dlg = new LoginDialog(this);
    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    setCentralWidget(_login_dlg);

    _reg_dlg->hide();
    _login_dlg->show();

    connect(_login_dlg, &LoginDialog::switchRegister, this, &MainWindow::SlotSwitchReg);
    connect(_login_dlg, &LoginDialog::switchReset, this, &MainWindow::SlotSwitchReset);
    connect(TcpManager::GetInstance().get(), &TcpManager::sig_switch_chatdialog, this, &MainWindow::SlotSwitchApp);
}

void MainWindow::SlotSwitchLogin2()
{
    _login_dlg = new LoginDialog(this);
    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    setCentralWidget(_login_dlg);

    _reset_dlg->hide();
    _login_dlg->show();

    connect(_login_dlg, &LoginDialog::switchRegister, this, &MainWindow::SlotSwitchReg);
    connect(_login_dlg, &LoginDialog::switchReset, this, &MainWindow::SlotSwitchReset);
    connect(TcpManager::GetInstance().get(), &TcpManager::sig_switch_chatdialog, this, &MainWindow::SlotSwitchApp);
}

void MainWindow::SlotSwitchLogin3()
{
    // 退出应用
    exit(0);

    _login_dlg = new LoginDialog(this);
    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    setCentralWidget(_login_dlg);

    _app_dlg->hide();
    _login_dlg->show();

    this->setMinimumSize(_login_dlg->size());
    this->setMaximumSize(_login_dlg->size());

    connect(_login_dlg, &LoginDialog::switchRegister, this, &MainWindow::SlotSwitchReg);
    connect(_login_dlg, &LoginDialog::switchReset, this, &MainWindow::SlotSwitchReset);
    connect(TcpManager::GetInstance().get(), &TcpManager::sig_switch_chatdialog, this, &MainWindow::SlotSwitchApp);
}

void MainWindow::SlotSwitchBegin()
{
    _begin_dlg = new BeginDialog(this);
    _begin_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    setCentralWidget(_begin_dlg);

    _admin_dlg->hide();
    _begin_dlg->show();

    this->setMinimumSize(_begin_dlg->size());
    this->setMaximumSize(_begin_dlg->size());

    connect(_begin_dlg, &BeginDialog::SigSwitchToLogin, this, &MainWindow::SlotSwitchLogin0);
    connect(_begin_dlg, &BeginDialog::SigSwitchToAdmin, this, &MainWindow::SlotSwitchAdmin);
}

void MainWindow::SlotSwitchReset()
{
    _reset_dlg = new ResetDialog(this);
    _reset_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    setCentralWidget(_reset_dlg);

    _login_dlg->hide();
    _reset_dlg->show();

    connect(_reset_dlg, &ResetDialog::switchLogin, this, &MainWindow::SlotSwitchLogin2);
}

void MainWindow::SlotSwitchApp()
{
    if(UserManager::GetInstance()->GetUserType() == UserType::User)
    {
        _app_dlg = new ApplicationDialog(this);
        _app_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

        setCentralWidget(_app_dlg);

        _login_dlg->hide();
        _app_dlg->show();

        this->setMinimumSize(_app_dlg->size());
        this->setMaximumSize(_app_dlg->size());

        connect(_app_dlg, &ApplicationDialog::SigSwitchLogin, this, &MainWindow::SlotSwitchLogin3);
    }
    else if(UserManager::GetInstance()->GetUserType() == UserType::Merchant)
    {
        _mer_app_dlg = new MerchantAppDialog(this);
        _mer_app_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

        setCentralWidget(_mer_app_dlg);

        _login_dlg->hide();
        _mer_app_dlg->show();

        this->setMinimumSize(_mer_app_dlg->size());
        this->setMaximumSize(_mer_app_dlg->size());
    }
}

void MainWindow::SlotSwitchAdmin()
{
    _admin_dlg = new AdminManagerDialog(this);
    _admin_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    setCentralWidget(_admin_dlg);

    _begin_dlg->hide();
    _admin_dlg->show();

    this->setMinimumSize(_admin_dlg->size());
    this->setMaximumSize(_admin_dlg->size());

    // 连接从管理界面回来的槽函数
    connect(_admin_dlg, &AdminManagerDialog::SigReturnLogin, this, &MainWindow::SlotSwitchBegin);
}

void MainWindow::setupEntranceAnimation(QWidget* widget, int duration)
{
    if (!widget) return;

    QPropertyAnimation *animation = new QPropertyAnimation(widget, "geometry");
    animation->setDuration(duration);
    animation->setEasingCurve(QEasingCurve::OutCubic);

    QRect startGeometry(0, height(), width(), height());
    QRect endGeometry(0, 0, width(), height());
    animation->setStartValue(startGeometry);
    animation->setEndValue(endGeometry);

    animation->start(QAbstractAnimation::DeleteWhenStopped);
}
