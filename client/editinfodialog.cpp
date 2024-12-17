#include "usermanager.h"
#include "editinfodialog.h"
#include "ui_editinfodialog.h"

#include <QDir>
#include <QFile>
#include <QLabel>
#include <QDebug>
#include <QPainter>
#include <QFileDialog>
#include <QMessageBox>


EditInfoDialog::EditInfoDialog(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::EditInfoDialog)
{
    ui->setupUi(this);
    setupUserInterface();
    setupConnections();
}

EditInfoDialog::~EditInfoDialog()
{
    delete ui;
}

void EditInfoDialog::setupUserInterface()
{
    // 设置当前用户信息
    ui->usernameEdit->setText(UserManager::GetInstance()->GetName());
    ui->emailEdit->setText(UserManager::GetInstance()->GetEmail());

    // 设置头像
    QPixmap avatar(UserManager::GetInstance()->GetHead());
    if (avatar.isNull())
    {
        avatar = QPixmap("../resources/Application/head/anonymous.png");
    }
    setAvatar(avatar);
    currentAvatar = avatar;
}

void EditInfoDialog::setupConnections()
{
    connect(ui->saveBtn, &QPushButton::clicked, this, &EditInfoDialog::onSaveClicked);
    
    ui->avatarLabel->installEventFilter(this);
    ui->changeAvatarLabel->installEventFilter(this);
}

bool EditInfoDialog::eventFilter(QObject *watched, QEvent *event)
{
    if ((watched == ui->avatarLabel || watched == ui->changeAvatarLabel) 
        && event->type() == QEvent::MouseButtonRelease) {
        onChangeAvatarClicked();
        return true;
    }
    return QDialog::eventFilter(watched, event);
}

void EditInfoDialog::setAvatar(const QPixmap &avatar)
{
    if (avatar.isNull())
        return;

    // 创建圆形头像
    QPixmap rounded(180, 180);
    rounded.fill(Qt::transparent);

    QPainter painter(&rounded);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    QPainterPath path;
    path.addEllipse(rounded.rect());
    painter.setClipPath(path);

    QPixmap scaled = avatar.scaled(180, 180, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    int x = (180 - scaled.width()) / 2;
    int y = (180 - scaled.height()) / 2;
    painter.drawPixmap(x, y, scaled);

    painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
    painter.setBrush(Qt::white);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(rounded.rect());

    ui->avatarLabel->setPixmap(rounded);
}

void EditInfoDialog::onChangeAvatarClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("选择头像"), "", tr("图片文件 (*.png *.jpg *.jpeg)"));

    if (!fileName.isEmpty())
    {
        QPixmap newAvatar(fileName);
        if (!newAvatar.isNull())
        {
            setAvatar(newAvatar);
            currentAvatar = newAvatar;
            newAvatarPath = fileName;
        }
    }
}

QString EditInfoDialog::copyAvatarToTmp(const QString &sourcePath)
{
    if (sourcePath.isEmpty())
        return QString();

    QDir dir("../resources/Application/tmp");
    if (!dir.exists())
    {
        dir.mkpath(".");
    }

    QStringList filters;
    filters << "avatar_*.png" << "avatar_*.jpg" << "avatar_*.jpeg";
    dir.setNameFilters(filters);
    int count = dir.count();

    // 生成新的文件名
    QString extension = QFileInfo(sourcePath).suffix();
    QString newFileName = QString("avatar_%1.%2").arg(count + 1).arg(extension);
    QString destPath = dir.filePath(newFileName);

    // 复制文件
    if (QFile::exists(destPath))
    {
        QFile::remove(destPath);
    }
    if (QFile::copy(sourcePath, destPath))
    {
        return destPath;
    }

    return QString();
}

void EditInfoDialog::onSaveClicked()
{
    qDebug() << "Save clicked";

    if (!newAvatarPath.isEmpty())
    {
        QString newPath = copyAvatarToTmp(newAvatarPath);
        if (!newPath.isEmpty())
        {
            qDebug() << "Avatar copied to:" << newPath;
        }
        else
        {
            qDebug() << "Failed to copy avatar";
        }
    }

    UserManager::GetInstance()->SetHead(newAvatarPath);
    UserManager::GetInstance()->SetName(ui->usernameEdit->text());
    UserManager::GetInstance()->SetEmail(ui->emailEdit->text());
    
    QMessageBox::information(
        this,
        "提示",
        "修改成功！",
        QMessageBox::Ok
    );
    
    accept();
}
