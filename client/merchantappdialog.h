#ifndef MERCHANTAPPDIALOG_H
#define MERCHANTAPPDIALOG_H

#include <QDialog>

namespace Ui
{
    class MerchantAppDialog;
}

class MerchantAppDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MerchantAppDialog(QWidget *parent = nullptr);
    ~MerchantAppDialog();

private:
    Ui::MerchantAppDialog *ui;
};

#endif // MERCHANTAPPDIALOG_H
