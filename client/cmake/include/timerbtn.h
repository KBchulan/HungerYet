/******************************************************************************
 *
 * @file       timerbtn.h
 * @brief      具有定时功能的按钮
 *
 * @author     KBchulan
 * @date       2024/11/30
 * @history
 *****************************************************************************/
#ifndef TIMERBTN_H
#define TIMERBTN_H

#include <QTimer>
#include <QPushButton>

class TimerBtn final : public QPushButton
{
public:
    explicit TimerBtn(QWidget *parent = nullptr);
    ~TimerBtn();

    void mouseReleaseEvent(QMouseEvent *e) override;

private:
    int _counter;
    QTimer *_timer = nullptr;
};

#endif // TIMERBTN_H
