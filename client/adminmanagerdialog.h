#ifndef ADMINMANAGERDIALOG_H
#define ADMINMANAGERDIALOG_H

#include "global.h"

#include <QDialog>
#include <QTableWidget>

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
    void on_switchBtn_clicked();

private:
    void loadLogFile();
    void filterLogs(const QString &searchText);
    void loadOrders();
    void initOrderTable();
    void updateOrderTable(ReqId id, QString res, ErrorCodes err);

private:
    QStringList m_allLogs;
    Ui::AdminManagerDialog *ui;
    bool m_isLogView;
};

#endif // ADMINMANAGERDIALOG_H
