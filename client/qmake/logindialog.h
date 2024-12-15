/******************************************************************************
 *
 * @file       logindialog.h
 * @brief      LoginWindow
 *
 * @author     KBchulan
 * @date       2024/11/19
 * @history
 *****************************************************************************/
#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include "global.h"
#include "particleeffect.h"
#include "backgrounddialog.h"

#include <QDialog>

namespace Ui
{
    class LoginDialog;
}

class LoginDialog final : public BackgroundDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

private:
    // 初始化头像
    void initHead();

    // email text
    bool checkUserValid();

    // passwd text
    bool checkPwdVaild();

    // show tip in err_tip
    void showTip(QString str, bool b_ok);

    // add to tip map
    void AddTipErr(TipErr te, QString tips);

    // remove from tip map
    void DelTipErr(TipErr te);

    // 设置按钮无效
    bool enableBtn(bool enabled);

    // 网络请求
    void initHttpHandlers();

    // 动画
    void addRandomParticle();
    void initParticleEffect();

private:
    Ui::LoginDialog *ui = nullptr;
    QMap<TipErr, QString> _tip_errs;
    QMap<ReqId, std::function<void(const QJsonObject &)>> _handlers;

    // 后续统一管理
    int _uid;
    QString _token;

    // 动画元素
    QTimer *m_particleTimer = nullptr;
    ParticleEffect *m_particleEffect = nullptr;

public slots:
    // 处理忘记密码的槽函数
    void slot_forget_pwd();

    // tcp success
    void slot_tcp_con_finished(bool success);

    // tcp failed
    void slot_login_failed(int);

signals:
    // change to register
    void switchRegister();

    // change to resetPwd
    void switchReset();

    // when receive info from http, it will send to tcp
    void sig_connect_tcp(ServerInfo);

private slots:
    // 登录按钮
    void on_login_btn_clicked();

    // 接到登录回包后的逻辑
    void slot_login_mod_finish(ReqId id, QString res, ErrorCodes err);
    void on_visitor_btn_clicked();
};

#endif // LOGINDIALOG_H
