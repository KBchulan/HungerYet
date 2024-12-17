/******************************************************************************
 *
 * @file       particleeffect.h
 * @brief      粒子系统
 *
 * @author     KBchulan
 * @date       2024/11/30
 * @history
 *****************************************************************************/
#ifndef PARTICLEEFFECT_H
#define PARTICLEEFFECT_H

#include <QColor>
#include <QTimer>
#include <QPointF>
#include <QWidget>
#include <QVector>
#include <QPainter>
#include <algorithm>

// 单个粒子的属性结构体
struct Particle
{
    QPointF pos;         // 位置
    QPointF velocity;    // 速度
    qreal angle;         // 旋转角度
    qreal angleSpeed;    // 旋转速度
    qreal size;         // 大小
    qreal alpha;        // 透明度
    QColor color;       // 颜色
    int shapeType;      // 形状类型
    bool alive;         // 是否存活
    
    qreal life;         // 当前生命值
    qreal maxLife;      // 最大生命值
};

class ParticleEffect final : public QWidget {
    Q_OBJECT
    
public:
    enum ShapeType 
    {
        Star = 0,
        Triangle,
        Circle,
        Rectangle,
        Heart
    };
    
    explicit ParticleEffect(QWidget *parent = nullptr);
    ~ParticleEffect();
    
    // 添加一个粒子
    void addParticle(const QPointF& pos, 
                    ShapeType shape,
                    qreal size = 10,
                    qreal angle = 0,
                    const QColor& color = Qt::white,
                    const QPointF& direction = QPointF(0, 0));
                    
    // 设置发射器位置
    void setEmitterPos(const QPointF& pos);
    
    // 开始/停止动画
    void start();
    void stop();

    // 清除所有粒子
    void clear();
    
protected:
    void paintEvent(QPaintEvent* event) override;
    
private:
    // 绘制各种形状的函数
    void drawStar(QPainter* painter, const QPointF& center, qreal size, qreal angle);
    void drawTriangle(QPainter* painter, const QPointF& center, qreal size, qreal angle);
    void drawHeart(QPainter* painter, const QPointF& center, qreal size, qreal angle);
    
    // 更新粒子状态
    void updateParticles();
    
private:
    QTimer* m_timer = nullptr;        // 动画计时器
    QVector<Particle> m_particles;    // 粒子容器
    QPointF m_emitterPos;             // 发射器位置
    bool m_isActive;                  // 是否激活
    
    // 配置参数
    static constexpr int MAX_PARTICLES = 1000;  // 最大粒子数
    static constexpr int EMIT_RATE = 10;        // 每次发射的粒子数
    static constexpr int UPDATE_INTERVAL = 16;  // 更新间隔(ms)
};

#endif // PARTICLEEFFECT_H
