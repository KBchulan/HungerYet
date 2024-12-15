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

private:
    Ui::ApplicationDialog *ui;

private slots:
    void SlotSwitchChat();

    void SlotSwitchMenu();
};

#endif // APPLICATIONDIALOG_H
