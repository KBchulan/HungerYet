#include "adminvarifydialog.h"
#include "ui_adminvarifydialog.h"

AdminVarifyDialog::AdminVarifyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdminVarifyDialog)
{
    ui->setupUi(this);
}

AdminVarifyDialog::~AdminVarifyDialog()
{
    delete ui;
}

void AdminVarifyDialog::showTip(QString str, bool b_ok)
{
    if(b_ok)
        ui->err_tip->setProperty("state", "normal");
    else
        ui->err_tip->setProperty("state", "err");

    ui->err_tip->setText(str);
    repolish(ui->err_tip);
}

void AdminVarifyDialog::AddTipErr(TipErr te, QString tips)
{
    _tip_errs[te] = tips;
    showTip(tips, false);
}

void AdminVarifyDialog::DelTipErr(TipErr te)
{
    _tip_errs.remove(te);
    if(_tip_errs.empty())
    {
        ui->err_tip->clear();
        return;
    }
    showTip(_tip_errs.first(), false);
}

bool AdminVarifyDialog::CheckName()
{
    auto name = ui->va_user_edit->text();
    if(name != QString("admin"))
    {
        AddTipErr(TipErr::TIP_USER_ERR, tr("账号错误"));
        return false;
    }
    DelTipErr(TipErr::TIP_USER_ERR);
    return true;
}

bool AdminVarifyDialog::CheckPasswd()
{
    auto passwd = ui->va_passwd_edit->text();
    if(passwd != QString("123456"))
    {
        AddTipErr(TipErr::TIP_PWD_ERR, tr("密码错误"));
        return false;
    }
    DelTipErr(TipErr::TIP_PWD_ERR);
    return true;
}

void AdminVarifyDialog::on_va_confirm_btn_clicked()
{
    if(!CheckName() || !CheckPasswd())
        return;

    emit SigSwitchAdminFromVarify();
}

void AdminVarifyDialog::on_va_cancel_btn_clicked()
{
    emit SigSwitchBeginFromVarify();
}
