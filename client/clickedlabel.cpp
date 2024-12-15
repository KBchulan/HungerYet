#include "clickedlabel.h"

#include <QMouseEvent>

ClickedLabel::ClickedLabel(QWidget *parent) : QLabel(parent), _cur_state(ClickLbState::Normal)
{
    this->setCursor(Qt::PointingHandCursor);
}

void ClickedLabel::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        if(_cur_state == ClickLbState::Normal)
        {
            _cur_state = ClickLbState::Selected;
            setProperty("state", _selected_press);
            repolish(this);
            update();
        }
        else
        {
            _cur_state = ClickLbState::Normal;
            setProperty("state", _normal_press);
            repolish(this);
            update();
        }
        return;
    }
    // 调用父类的方法，否则无法触发父类的事件   
    QLabel::mousePressEvent(event);
}

void ClickedLabel::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (_cur_state == ClickLbState::Normal)
        {
            setProperty("state", _normal_hover);
            repolish(this);
            update();
        }
        else
        {
            setProperty("state", _selected_hover);
            repolish(this);
            update();
        }
        emit sig_clicked();
        return;
    }
    
    // 调用基类的mousePressEvent以保证正常的事件处理
    QLabel::mousePressEvent(event);
}

void ClickedLabel::enterEvent(QEvent *event)
{
    // 处理鼠标悬停进入的逻辑
    if(_cur_state == ClickLbState::Normal)
    {
        setProperty("state", _normal_hover);
        repolish(this);
        update();
    }
    else
    {
        setProperty("state", _selected_hover);
        repolish(this);
        update();
    }
    QLabel::enterEvent(event);
}

void ClickedLabel::leaveEvent(QEvent *event)
{
    if(_cur_state == ClickLbState::Normal)
    {
        setProperty("state", _normal);
        repolish(this);
        update();
    }
    else
    {
        setProperty("state", _selected);
        repolish(this);
        update();
    }
    QLabel::leaveEvent(event);
}

void ClickedLabel::SetState(QString normal, QString hover, QString press, QString select, QString select_hover, QString select_press)
{
    _normal = normal;
    _normal_hover = hover;
    _normal_press = press;

    _selected = select;
    _selected_hover = select_hover;
    _selected_press = select_press;

    setProperty("state", _normal);
    repolish(this);
}

ClickLbState ClickedLabel::GetCurState()
{
    return _cur_state;
}
