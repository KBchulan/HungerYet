#include "particleeffect.h"

#include <QtMath>

ParticleEffect::ParticleEffect(QWidget *parent)
    : QWidget(parent), m_timer(new QTimer(this)), m_isActive(false)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint);

    connect(m_timer, &QTimer::timeout, this, &ParticleEffect::updateParticles);
    m_timer->setInterval(UPDATE_INTERVAL);
}

ParticleEffect::~ParticleEffect()
{
    stop();
    clear();
}

void ParticleEffect::start()
{
    if (!m_isActive)
    {
        m_isActive = true;
        m_timer->start();
    }
}

void ParticleEffect::stop()
{
    if (m_isActive)
    {
        m_isActive = false;
        m_timer->stop();
    }
}

void ParticleEffect::clear()
{
    m_particles.clear();
    update();
}

void ParticleEffect::setEmitterPos(const QPointF &pos)
{
    m_emitterPos = pos;
}

void ParticleEffect::addParticle(const QPointF& pos,
                                ShapeType shape,
                                qreal size,
                                qreal angle,
                                const QColor& color,
                                const QPointF& direction)
{
    if (m_particles.size() >= MAX_PARTICLES)
    {
        return;
    }

    Particle p;
    p.pos = pos;
    
    // 使用传入的方向和随机速度
    qreal speed = (qrand() % 100) / 100.0 * 2.0;
    p.velocity = direction * speed;
    
    p.angle = angle;
    p.angleSpeed = (qrand() % 100 - 50) / 10.0;
    p.size = size;
    p.alpha = 1.0;
    p.color = color;
    p.shapeType = shape;
    p.alive = true;
    p.maxLife = 5.0; // 增加生命周期让粒子有更多时间到达目标
    p.life = p.maxLife;

    m_particles.append(p);
}

void ParticleEffect::drawStar(QPainter *painter, const QPointF &center, qreal size, qreal angle)
{
    const int points = 5;
    const qreal PI2 = M_PI * 2;
    const qreal rotAngle = angle * M_PI / 180;

    QPolygonF star;
    for (int i = 0; i < points * 2; ++i)
    {
        qreal r = (i & 1) ? size / 2 : size;
        qreal a = (i * PI2) / (points * 2) + rotAngle;
        star << QPointF(center.x() + cos(a) * r,
                        center.y() + sin(a) * r);
    }

    painter->drawPolygon(star);
}

void ParticleEffect::drawTriangle(QPainter *painter, const QPointF &center, qreal size, qreal angle)
{
    const qreal rotAngle = angle * M_PI / 180;
    QPolygonF triangle;

    for (int i = 0; i < 3; ++i)
    {
        qreal a = (i * M_PI * 2 / 3) + rotAngle;
        triangle << QPointF(center.x() + cos(a) * size,
                            center.y() + sin(a) * size);
    }

    painter->drawPolygon(triangle);
}

void ParticleEffect::drawHeart(QPainter *painter, const QPointF &center, qreal size, qreal angle)
{
    QPainterPath path;

    // 绘制心形的贝塞尔曲线路径
    path.moveTo(center.x(), center.y() + size * 0.3);
    path.cubicTo(center.x() + size * 0.3, center.y(),
                 center.x() + size * 0.5, center.y() - size * 0.3,
                 center.x(), center.y() - size * 0.5);
    path.cubicTo(center.x() - size * 0.5, center.y() - size * 0.3,
                 center.x() - size * 0.3, center.y(),
                 center.x(), center.y() + size * 0.3);

    // 应用旋转
    QTransform transform;
    transform.translate(center.x(), center.y());
    transform.rotate(angle);
    transform.translate(-center.x(), -center.y());

    painter->drawPath(transform.map(path));
}

void ParticleEffect::updateParticles()
{
    for (auto &particle : m_particles)
    {
        if (!particle.alive)
            continue;

        // 更新位置
        particle.pos += particle.velocity;

        // 更新角度
        particle.angle += particle.angleSpeed;

        // 更新生命值
        particle.life -= UPDATE_INTERVAL / 1000.0;
        if (particle.life <= 0)
        {
            particle.alive = false;
        }

        // 更新透明度
        particle.alpha = particle.life / particle.maxLife;
    }

    // 移除死亡粒子
    m_particles.erase(
        std::remove_if(m_particles.begin(), m_particles.end(),
                       [](const Particle &p)
                       { return !p.alive; }),
        m_particles.end());

    update(); // 触发重绘
}

void ParticleEffect::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    for (const auto &particle : m_particles)
    {
        if (!particle.alive)
            continue;

        painter.save();

        // 设置颜色和透明度
        QColor color = particle.color;
        color.setAlphaF(particle.alpha);
        painter.setPen(Qt::NoPen);
        painter.setBrush(color);

        // 根据形状类型绘制不同的图形
        switch (particle.shapeType)
        {
        case Star:
            drawStar(&painter, particle.pos, particle.size, particle.angle);
            break;
        case Triangle:
            drawTriangle(&painter, particle.pos, particle.size, particle.angle);
            break;
        case Circle:
            painter.drawEllipse(particle.pos, particle.size, particle.size);
            break;
        case Heart:
            drawHeart(&painter, particle.pos, particle.size, particle.angle);
            break;
        default:
            painter.drawRect(QRectF(particle.pos.x() - particle.size / 2,
                                    particle.pos.y() - particle.size / 2,
                                    particle.size, particle.size));
        }

        painter.restore();
    }
}