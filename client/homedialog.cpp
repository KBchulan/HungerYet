#include "homedialog.h"
#include "usermanager.h"
#include "ui_homedialog.h"

#include <QDebug>
#include <QPixmap>
#include <QBitmap>
#include <QPainter>
#include <QPainterPath>

HomeDialog::HomeDialog(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::HomeDialog)
{
    ui->setupUi(this);
    setupUserInterface();

    ui->orderTextLabel->SetState("normal", "hover", "hover", "selected", "selected_hover", "selected_hover");
    ui->favoriteTextLabel->SetState("normal", "hover", "hover", "selected", "selected_hover", "selected_hover");
    ui->usernameLabel->SetState("normal", "hover", "hover", "selected", "selected_hover", "selected_hover");
}

HomeDialog::~HomeDialog()
{
    qDebug() << "HomeDialog destroyed";
    delete ui;
}

void HomeDialog::setupUserInterface()
{
    if (UserManager::GetInstance()->GetHead().isEmpty())
    {
        QPixmap defaultAvatar("../resources/Application/head/anonymous.png");
        setAvatar(defaultAvatar);
        setUserInfo("游客先生", "", 355);
    }
    else
    {
        QPixmap defaultAvatar(UserManager::GetInstance()->GetHead());
        setAvatar(defaultAvatar);
        setUserInfo(UserManager::GetInstance()->GetName(), UserManager::GetInstance()->GetEmail(), UserManager::GetInstance()->GetUid());
    }
}

void HomeDialog::setUserInfo(const QString &username, const QString &email, const int &uid)
{
    // 获取用户等级
    int userLevel = static_cast<int>(UserManager::GetInstance()->GetUserLevel());
    QString levelInfo;
    switch(userLevel) {
        case 2:
            levelInfo = "VVIP会员(5折优惠)";
            break;
        case 1:
            levelInfo = "VIP会员(8折优惠)";
            break;
        default:
            levelInfo = "普通用户";
            break;
    }
    
    ui->usernameLabel->setText(QString("%1\n%2").arg(username).arg(levelInfo));

    if (email.isEmpty())
    {
        ui->emailLabel->setText("📧  未绑定邮箱");
    }
    else
    {
        ui->emailLabel->setText("📧  " + email);
    }

    ui->uidLabel->setText("🆔  #" + QString::number(uid));
}

void HomeDialog::setAvatar(const QPixmap &avatar)
{
    if (avatar.isNull())
    {
        return;
    }

    // 创建一个空的圆形图片
    QPixmap rounded(200, 200);
    rounded.fill(Qt::transparent);

    // 创建画家
    QPainter painter(&rounded);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    // 绘制圆形裁剪路径
    QPainterPath path;
    path.addEllipse(rounded.rect());
    painter.setClipPath(path);

    // 缩放原始图片并居中绘制
    QPixmap scaled = avatar.scaled(200, 200, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    int x = (200 - scaled.width()) / 2;
    int y = (200 - scaled.height()) / 2;
    painter.drawPixmap(x, y, scaled);

    // 添加白色背景
    painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
    painter.setBrush(Qt::white);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(rounded.rect());

    ui->avatarLabel->setPixmap(rounded);
}
