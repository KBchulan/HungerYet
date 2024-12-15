#ifndef APPLICATIONDIALOG_H
#define APPLICATIONDIALOG_H

#include <QDialog>

namespace Ui
{
    class ApplicationDialog;
}

class ApplicationDialog final : public QDialog
{
    Q_OBJECT

public:
    explicit ApplicationDialog(QWidget *parent = nullptr);
    ~ApplicationDialog();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void handleButtonClick(QPushButton* button);

private:
    Ui::ApplicationDialog *ui;
    QPushButton* currentSelectedButton = nullptr;

private slots:
    void SlotSwitchChat();

    void SlotSwitchMenu();

    void SLotSwitchHome();
};

#endif // APPLICATIONDIALOG_H
