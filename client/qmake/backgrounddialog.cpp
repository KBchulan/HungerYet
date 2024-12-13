#include "backgrounddialog.h"

BackgroundDialog::BackgroundDialog(QWidget *parent)
    : QDialog(parent)
    , m_opacity(1.0)
{
}

void BackgroundDialog::setBackground(const QString& imagePath, qreal opacity)
{
    m_imagePath = imagePath;
    m_opacity = qBound(0.0, opacity, 1.0);
    
    if (!m_imagePath.isEmpty())
    {
        m_backgroundImage.load(m_imagePath);
    }
    
    // 触发重绘
    update();
}

void BackgroundDialog::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    if (!m_backgroundImage.isNull())
    {
        painter.setOpacity(m_opacity);
        
        QPixmap scaled = m_backgroundImage.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        
        QPoint pos((width() - scaled.width()) / 2, (height() - scaled.height()) / 2);
        
        painter.drawPixmap(pos, scaled);
    }
    
    QDialog::paintEvent(event);
} 
