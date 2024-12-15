#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>
#include <QStackedWidget>

namespace Ui {
class MainDialog;
}

class MainDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MainDialog(QWidget *parent = nullptr);
    ~MainDialog();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    Ui::MainDialog *ui;
    QPushButton* currentSelectedButton = nullptr;  // 记录当前选中的按钮
    void handleButtonClick(QPushButton* button);  // 处理按钮点击
    QStackedWidget *stackedWidget;
};

#endif // MAINDIALOG_H
