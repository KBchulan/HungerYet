/******************************************************************************
 *
 * @file       resetdialog.h
 * @brief      重置密码的界面
 *
 * @author     KBchulan
 * @date       2024/11/30
 * @history
 *****************************************************************************/
#ifndef RESETDIALOG_H
#define RESETDIALOG_H

#include "global.h"
#include "particleeffect.h"
#include "backgrounddialog.h"

#include <QDialog>

namespace Ui
{
    class ResetDialog;
}

class ResetDialog final : public BackgroundDialog
{
    Q_OBJECT

public:
    explicit ResetDialog(QWidget *parent = nullptr);
    ~ResetDialog();

private:
    // 检查用户名是否有效
    bool checkUserValid();

    // 检查密码是否有效
    bool checkPassValid();

    // 显示提示信息
    void showTip(QString str,bool b_ok);

    // 检查邮箱是否有效
    bool checkEmailValid();

    // 检查验证码是否有效
    bool checkVarifyValid();

    // 添加错误提示
    void AddTipErr(TipErr te,QString tips);

    // 删除错误提示
    void DelTipErr(TipErr te);
    
    // 初始化处理器
    void initHandlers();

    // 动画
    void addRandomParticle();
    void initParticleEffect();

private:
    Ui::ResetDialog *ui = nullptr;
    QMap<TipErr, QString> _tip_errs;
    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;

    // 动画元素
    QTimer *m_particleTimer = nullptr;
    ParticleEffect *m_particleEffect = nullptr;

private slots:
    // 返回按钮点击事件
    void on_return_btn_clicked();

    // 验证按钮点击事件
    void on_varify_btn_clicked();

    // 重置密码完成回调
    void slot_reset_mod_finish(ReqId id, QString res, ErrorCodes err);

    // 确认按钮点击事件
    void on_sure_btn_clicked();

signals:
    void switchLogin();
};

#endif // RESETDIALOG_H
