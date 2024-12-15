/******************************************************************************
 *
 * @file       backgrounddialog.h
 * @brief      注册登录和重置密码三个界面的个性化
 *
 * @author     KBchulan
 * @date       2024/12/01
 * @history
 *****************************************************************************/
#ifndef BACKGROUNDDIALOG_H
#define BACKGROUNDDIALOG_H

#include <QDialog>
#include <QPainter>
#include <QPixmap>

class BackgroundDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BackgroundDialog(QWidget *parent = nullptr);
    
    // 设置背景图片路径和透明度
    void setBackground(const QString& imagePath, qreal opacity = 1.0);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    qreal m_opacity;
    QString m_imagePath;
    QPixmap m_backgroundImage;
};

#endif // BACKGROUNDDIALOG_H 
