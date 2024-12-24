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

signals:
    void SigReturnLogin();

private slots:
    void on_return_btn_clicked();
    void on_searchEdit_textChanged(const QString &text);
    void on_refreshBtn_clicked();

private:
    void loadLogFile();
    void filterLogs(const QString &searchText);
    QStringList m_allLogs;  // 存储所有日志
    Ui::AdminManagerDialog *ui;
};

#endif // ADMINMANAGERDIALOG_H
