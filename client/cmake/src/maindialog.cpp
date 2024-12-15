#include <maindialog.h>
#include <ui_maindialog.h>

#include <QDebug>
#include <QStyle>

MainDialog::MainDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MainDialog)
{
    ui->setupUi(this);
    ui->chat_tool->setProperty("state", "normal");
    ui->order_tool->setProperty("state", "selected");
    currentSelectedButton = ui->order_tool;
    ui->order_tool->installEventFilter(this);
    ui->chat_tool->installEventFilter(this);
}

MainDialog::~MainDialog()
{
    qDebug() << "MainDialog destructed!";
    delete ui;
}

void MainDialog::handleButtonClick(QPushButton* clickedButton)
{
    qDebug() << "handleButtonClick clickedButton:" << clickedButton;
    if (currentSelectedButton != clickedButton) {
        // 如果点击的是其他按钮
        if (currentSelectedButton) {
            // 如果有其他按钮被选中，先取消它的选中状态
            currentSelectedButton->setProperty("state", "normal");
            currentSelectedButton->style()->polish(currentSelectedButton);
        }
        // 选中当前点击的按钮
        clickedButton->setProperty("state", "selected");
        currentSelectedButton = clickedButton;
    }
    clickedButton->style()->polish(clickedButton);
}

bool MainDialog::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->chat_tool || watched == ui->order_tool) {
        QPushButton *button = qobject_cast<QPushButton*>(watched);
        QString currentState = button->property("state").toString();
        
        if (event->type() == QEvent::Enter) {
            if (currentState != "selected") {
                button->setProperty("state", "hover");
                button->style()->polish(button);
            }
        } 
        else if (event->type() == QEvent::Leave) {
            if (currentState != "selected") {
                button->setProperty("state", "normal");
                button->style()->polish(button);
            }
        }
        else if (event->type() == QEvent::MouseButtonPress) {
            if (currentState != "selected") {
                button->setProperty("state", "selected");
                button->style()->polish(button);
            }
        }
        else if (event->type() == QEvent::MouseButtonRelease) {
            handleButtonClick(button);
        }
    }
    return QDialog::eventFilter(watched, event);
}
