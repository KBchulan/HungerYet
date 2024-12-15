/******************************************************************************
 *
 * @file       clickedbtn.h
 * @brief      实现有图片跳转的按钮类
 *
 * @author     KBchulan
 * @date       2024/11/30
 * @history
 *****************************************************************************/
#ifndef CLICKEDBTN_H
#define CLICKEDBTN_H

#include <QPushButton>

class ClickedBtn final : public QPushButton
{
    Q_OBJECT

public:
    explicit ClickedBtn(QWidget *parent = nullptr);

    ~ClickedBtn();

    void SetState(QString normal, QString hover, QString press);

protected:
    virtual void enterEvent(QEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QString _normal;
    QString _hover;
    QString _press;
};

#endif // CLICKEDBTN_H
