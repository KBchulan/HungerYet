#include "homedialog.h"
#include "ui_homedialog.h"
#include <QPixmap>
#include <QPainter>
#include <QBitmap>
#include <QDebug>

HomeDialog::HomeDialog(QWidget *parent) : QDialog(parent),
                                          ui(new Ui::HomeDialog)
{
    ui->setupUi(this);
    setupUserInterface();
}

HomeDialog::~HomeDialog()
{
    qDebug() << "HomeDialog destroyed";
    delete ui;
}

void HomeDialog::setupUserInterface()
{
    // 设置默认头像
    QPixmap defaultAvatar("../resources/Application/head/anonymous.png");
    setAvatar(defaultAvatar);
    // 设置默认用户信息
    setUserInfo("游客先生", "", 355);
}

void HomeDialog::setUserInfo(const QString &username, const QString &email, const int &uid)
{
    ui->usernameLabel->setText(username+", 您好!");
    ui->emailLabel->setText("email: "+email);
    ui->uidLabel->setText("UID: "+QString::number(uid));
}

void HomeDialog::setAvatar(const QPixmap &avatar)
{
    if (avatar.isNull())
    {
        return;
    }

    // 将头像处理成圆形
    QPixmap rounded = avatar.scaled(240, 240, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    QPixmap mask(240, 240);
    mask.fill(Qt::transparent);

    QPainter painter(&mask);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::white);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(mask.rect());

    rounded.setMask(mask.createMaskFromColor(Qt::transparent));
    ui->avatarLabel->setPixmap(rounded);
}