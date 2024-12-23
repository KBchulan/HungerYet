#ifndef ADMINMANAGERDIALOG_H
#define ADMINMANAGERDIALOG_H

#include <QDialog>

namespace Ui
{
    class AdminManagerDialog;
}

class AdminManagerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AdminManagerDialog(QWidget *parent = nullptr);
    ~AdminManagerDialog();

private:
    void loadLogFile();
    Ui::AdminManagerDialog *ui;
};

#endif // ADMINMANAGERDIALOG_H
