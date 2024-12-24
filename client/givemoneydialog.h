#ifndef GIVEMONEYDIALOG_H
#define GIVEMONEYDIALOG_H

#include <QDialog>
#include <QButtonGroup>

namespace Ui
{
    class GiveMoneyDialog;
}

class GiveMoneyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GiveMoneyDialog(QWidget *parent = nullptr);
    ~GiveMoneyDialog();

private slots:
    void onAmountButtonClicked(int id);
    void onCustomAmountChanged(const QString &text);
    void onWechatPayClicked();
    void onAlipayClicked();
    void processPayment(const QString &method);

private:
    void initUI();
    void updatePaymentButtons(bool enabled);
    double getCurrentAmount() const;

private:
    Ui::GiveMoneyDialog *ui;
    QButtonGroup *amountGroup;
    double currentAmount;
};

#endif // GIVEMONEYDIALOG_H
