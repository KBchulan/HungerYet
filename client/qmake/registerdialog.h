/******************************************************************************
 *
 * @file       registerdialog.h
 * @brief      RegisterDialog
 *
 * @author     KBchulan
 * @date       2024/11/19
 * @history
 *****************************************************************************/
#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include "global.h"
#include "particleeffect.h"
#include "backgrounddialog.h"

#include <QDialog>

namespace Ui
{
    class RegisterDialog;
}

class RegisterDialog final : public BackgroundDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

signals:
    void sigSwitchLogin();

private slots:
    // get_code button clicked's callback
    void on_get_code_clicked();

    // when httpManager's register module finished, then the registerDialog will working
    void slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err);

    //  when push the sure btn, it will send a http request
    void on_sure_btn_clicked();

    // change two page
    void ChangeTipPage();

    // page2
    void on_return_btn_clicked();

    // cancel btn
    void on_cancel_btn_clicked();

private:
    // show some message(err is red, else green)
    void showTip(QString str, bool b_ok);

    // handle httpRequest, in other words, it will init the _handlers
    void initHttpHandlers();

    // check all edit isValid
    bool CheckUserValid();
    bool CheckEmailValid();
    bool CheckPasswordValid();
    bool CheckConfWordValid();
    bool CheckVarifyCodeValid();

    // add to tip map
    void AddTipErr(TipErr te, QString tips);

    // remove from tip map
    void DelTipErr(TipErr te);

    // 动画
    void addRandomParticle();
    void initParticleEffect();

private:
    int _countdown{5};
    Ui::RegisterDialog *ui = nullptr;
    QTimer *_countdown_timer = nullptr;
    QMap<TipErr, QString> _tip_errs;
    QMap<ReqId, std::function<void(const QJsonObject &)>> _handlers;

    // 动画元素
    QTimer *m_particleTimer = nullptr;
    ParticleEffect *m_particleEffect = nullptr;
};

#endif // REGISTERDIALOG_H
