#ifndef MERCHANTAPPDIALOG_H
#define MERCHANTAPPDIALOG_H

#include <QDialog>

namespace Ui
{
    class MerchantAppDialog;
}

class MerchantAppDialog final : public QDialog
{
    Q_OBJECT

public:
    explicit MerchantAppDialog(QWidget *parent = nullptr);
    ~MerchantAppDialog();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void handleButtonClick(QPushButton* button);

private:
    Ui::MerchantAppDialog *ui;
    QPushButton* currentSelectedButton = nullptr;

signals:
    void SigSwitchLogin();

private slots:
    void SlotSwitchOrder();
    void SlotSwitchHome();
    void SlotSwitchLogin();
    void SlotSwitchEdit();
    void SlotSwitchSetting();
    void SlotSwitchMenuManager();
};

#endif // MERCHANTAPPDIALOG_H
