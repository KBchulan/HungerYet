/******************************************************************************
 *
 * @file       clickedlabel.h
 * @brief      用于实现可点击的Label，即状态切换
 *
 * @author     KBchulan
 * @date       2024/11/30
 * @history
 *****************************************************************************/
#ifndef CLICKEDLABEL_H
#define CLICKEDLABEL_H

#include <QLabel>

#include "global.h"

class ClickedLabel final : public QLabel
{
    Q_OBJECT

public:
    explicit ClickedLabel(QWidget *parent = nullptr);

    virtual void mousePressEvent(QMouseEvent *ev) override;
    virtual void mouseReleaseEvent(QMouseEvent *ev) override;

    // widget event
    virtual void enterEvent(QEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;

    void SetState(QString normal = "", QString hover = "", QString press = "",
                  QString select = "", QString select_hover = "", QString select_press = "");

    // 获取当前状态
    ClickLbState GetCurState();

signals:
    void sig_clicked();

private:
    QString _normal;
    QString _normal_hover;
    QString _normal_press;

    QString _selected;
    QString _selected_hover;
    QString _selected_press;

    ClickLbState _cur_state;
};

#endif // CLICKEDLABEL_H
