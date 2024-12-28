#ifndef ADMINVARIFYDIALOG_H
#define ADMINVARIFYDIALOG_H

#include "global.h"

#include <QMap>
#include <QDialog>

namespace Ui
{
    class AdminVarifyDialog;
}

class AdminVarifyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AdminVarifyDialog(QWidget *parent = nullptr);
    ~AdminVarifyDialog();

signals:
    void SigSwitchAdminFromVarify();
    void SigSwitchBeginFromVarify();

private slots:
    void on_va_confirm_btn_clicked();

    void on_va_cancel_btn_clicked();

private:
    // show tip in err_tip
    void showTip(QString str, bool b_ok);

    // add to tip map
    void AddTipErr(TipErr te, QString tips);

    // remove from tip map
    void DelTipErr(TipErr te);

    // check name
    bool CheckName();

    // check password
    bool CheckPasswd();

private:
    Ui::AdminVarifyDialog *ui;
    QMap<TipErr, QString> _tip_errs;
};

#endif // ADMINVARIFYDIALOG_H
