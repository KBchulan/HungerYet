#include "tcpmanager.h"
#include "mainwindow.h"
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

    // create LoginDialog
    _login_dlg = new LoginDialog(this);
    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    // show current dialog(default widget is _login_dlg)
    setCentralWidget(_login_dlg);
    _login_dlg->show();

    // create and register message link
    connect(_login_dlg, &LoginDialog::switchRegister, this, &MainWindow::SlotSwitchReg);

    // reset password
    connect(_login_dlg, &LoginDialog::switchReset, this, &MainWindow::SlotSwitchReset);

    // change to applicationDialog
    connect(TcpManager::GetInstance().get(), &TcpManager::sig_switch_chatdialog, this, &MainWindow::SlotSwitchApp);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SlotSwitchReg()
{
    // create register
    _reg_dlg = new RegisterDialog(this);

    // set dialogs's style
    _reg_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    setCentralWidget(_reg_dlg);
    _login_dlg->hide();
    _reg_dlg->show();

    connect(_reg_dlg, &RegisterDialog::sigSwitchLogin, this, &MainWindow::SlotSwitchLogin);
}

void MainWindow::SlotSwitchLogin()
{
    // create LoginDialog
    _login_dlg = new LoginDialog(this);
    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    // show current dialog(default widget is _login_dlg)
    setCentralWidget(_login_dlg);

    _reg_dlg->hide();
    _login_dlg->show();

    // create and register message link
    connect(_login_dlg, &LoginDialog::switchRegister, this, &MainWindow::SlotSwitchReg);
    connect(_login_dlg, &LoginDialog::switchReset, this, &MainWindow::SlotSwitchReset);
    connect(TcpManager::GetInstance().get(), &TcpManager::sig_switch_chatdialog, this, &MainWindow::SlotSwitchApp);
}

void MainWindow::SlotSwitchLogin2()
{
    // create LoginDialog
    _login_dlg = new LoginDialog(this);
    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    // show current dialog(default widget is _login_dlg)
    setCentralWidget(_login_dlg);

    _reset_dlg->hide();
    _login_dlg->show();

    // create and register message link
    connect(_login_dlg, &LoginDialog::switchRegister, this, &MainWindow::SlotSwitchReg);
    connect(_login_dlg, &LoginDialog::switchReset, this, &MainWindow::SlotSwitchReset);
    connect(TcpManager::GetInstance().get(), &TcpManager::sig_switch_chatdialog, this, &MainWindow::SlotSwitchApp);
}

void MainWindow::SlotSwitchLogin3()
{
    // create LoginDialog
    _login_dlg = new LoginDialog(this);
    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    // show current dialog(default widget is _login_dlg)
    setCentralWidget(_login_dlg);

    _app_dlg->hide();
    _login_dlg->show();

    this->setMinimumSize(_login_dlg->size());
    this->setMaximumSize(_login_dlg->size());

    // create and register message link
    connect(_login_dlg, &LoginDialog::switchRegister, this, &MainWindow::SlotSwitchReg);
    connect(_login_dlg, &LoginDialog::switchReset, this, &MainWindow::SlotSwitchReset);
    connect(TcpManager::GetInstance().get(), &TcpManager::sig_switch_chatdialog, this, &MainWindow::SlotSwitchApp);
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
    _app_dlg = new ApplicationDialog(this);
    _app_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    setCentralWidget(_app_dlg);

    _login_dlg->hide();
    _app_dlg->show();

    this->setMinimumSize(_app_dlg->size());
    this->setMaximumSize(_app_dlg->size());

    connect(_app_dlg, &ApplicationDialog::SigSwitchLogin, this, &MainWindow::SlotSwitchLogin3);
}

void MainWindow::setupEntranceAnimation(QWidget* widget, int duration)
{
    if (!widget) return;

    // 创建动画效果
    QPropertyAnimation *animation = new QPropertyAnimation(widget, "geometry");
    animation->setDuration(duration);
    animation->setEasingCurve(QEasingCurve::OutCubic);

    // 设置动画起始和结束位置
    QRect startGeometry(0, height(), width(), height());
    QRect endGeometry(0, 0, width(), height());
    animation->setStartValue(startGeometry);
    animation->setEndValue(endGeometry);

    // 启动动画，完成后自动删除
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}
