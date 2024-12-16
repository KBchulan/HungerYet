#ifndef MERCHANTDIALOG_H
#define MERCHANTDIALOG_H

#include <QDialog>

namespace Ui
{
    class MerchantDialog;
}

class MerchantDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MerchantDialog(QWidget *parent = nullptr);
    ~MerchantDialog();

    void init(int merchant_id);

private:
    Ui::MerchantDialog *ui;
};

#endif // MERCHANTDIALOG_H
