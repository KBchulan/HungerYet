#include "settingdialog.h"
#include "ui_settingdialog.h"

#include <QDir>
#include <QFile>
#include <QFont>
#include <QDebug>
#include <QScreen>
#include <QProcess>
#include <QSettings>
#include <QMessageBox>
#include <QApplication>
#include <QStandardPaths>
#include <QSystemTrayIcon>
#include <QDesktopServices>

SettingDialog::SettingDialog(QWidget *parent)
    : QDialog(parent),
    ui(new Ui::SettingDialog),
    defaultFont(qApp->font())
{
    ui->setupUi(this);
    setupUserInterface();
    setupConnections();
    loadSettings();
}

SettingDialog::~SettingDialog()
{
    saveSettings();
    delete ui;
}

void SettingDialog::setupUserInterface()
{
    // 设置字体大小滑块范围
    ui->fontSizeSlider->setMinimum(12);
    ui->fontSizeSlider->setMaximum(20);
    ui->fontSizeSlider->setValue(14);

    // 设置分组标题样式
    ui->appearanceLabel->setProperty("groupTitle", true);
    ui->notificationLabel->setProperty("groupTitle", true);
    ui->generalLabel->setProperty("groupTitle", true);
    ui->aboutLabel->setProperty("groupTitle", true);

    // 设置设置项样式
    ui->themeWidget->setProperty("settingItem", true);
    ui->fontSizeWidget->setProperty("settingItem", true);
    ui->animationWidget->setProperty("settingItem", true);
    ui->notificationWidget->setProperty("settingItem", true);
    ui->soundWidget->setProperty("settingItem", true);
    ui->autoStartWidget->setProperty("settingItem", true);
    ui->languageWidget->setProperty("settingItem", true);
}

void SettingDialog::setupConnections()
{
    connect(ui->themeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingDialog::onThemeChanged);
    connect(ui->fontSizeSlider, &QSlider::valueChanged,
            this, &SettingDialog::onFontSizeChanged);
    connect(ui->notificationCheckBox, &QCheckBox::toggled,
            this, &SettingDialog::onNotificationToggled);
    connect(ui->autoStartCheckBox, &QCheckBox::toggled,
            this, &SettingDialog::onAutoStartToggled);
    connect(ui->clearCacheBtn, &QPushButton::clicked,
            this, &SettingDialog::onClearCacheClicked);
    connect(ui->aboutBtn, &QPushButton::clicked,
            this, &SettingDialog::onAboutClicked);

    connect(ui->soundCheckBox, &QCheckBox::toggled,
            this, &SettingDialog::onSoundToggled);
    connect(ui->animationCheckBox, &QCheckBox::toggled,
            this, &SettingDialog::onAnimationToggled);
    connect(ui->languageComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingDialog::onLanguageChanged);
}

void SettingDialog::loadSettings()
{
    QSettings settings("HungerYet", "Client");
    
    // 加载原有设置
    ui->themeComboBox->setCurrentIndex(settings.value("theme", 0).toInt());
    ui->fontSizeSlider->setValue(settings.value("fontSize", 14).toInt());
    ui->notificationCheckBox->setChecked(settings.value("notification", true).toBool());
    ui->autoStartCheckBox->setChecked(settings.value("autoStart", false).toBool());

    // 加载新增设置
    ui->soundCheckBox->setChecked(settings.value("sound", true).toBool());
    ui->animationCheckBox->setChecked(settings.value("animation", true).toBool());
    ui->languageComboBox->setCurrentIndex(settings.value("language", 0).toInt());
}

void SettingDialog::saveSettings()
{
    QSettings settings("HungerYet", "Client");
    
    // 保存原有设置
    settings.setValue("theme", ui->themeComboBox->currentIndex());
    settings.setValue("fontSize", ui->fontSizeSlider->value());
    settings.setValue("notification", ui->notificationCheckBox->isChecked());
    settings.setValue("autoStart", ui->autoStartCheckBox->isChecked());

    // 保存新增设置
    settings.setValue("sound", ui->soundCheckBox->isChecked());
    settings.setValue("animation", ui->animationCheckBox->isChecked());
    settings.setValue("language", ui->languageComboBox->currentIndex());
}

void SettingDialog::onThemeChanged(int index)
{
    applyTheme(index);
    qDebug() << "Theme changed to:" << index;
}

void SettingDialog::applyTheme(int themeIndex)
{
    QString baseStyle = loadStyleSheet(":/style/stylesheet.qss");
    
    if (themeIndex == 1)
    {
        QString darkStyle = QString(
            "QWidget { background-color: rgb(45, 45, 45); color: white; }"
            "QLineEdit { background-color: rgb(60, 60, 60); color: white; border: 1px solid rgb(80, 80, 80); }"
            "QPushButton { background-color: rgb(60, 60, 60); color: white; }"
            "QLabel { color: white; }"
            "QComboBox { background-color: rgb(60, 60, 60); color: white; }"
            "QCheckBox { color: white; }"
        );
        baseStyle += darkStyle;
    }
    
    qApp->setStyleSheet(baseStyle);
}

void SettingDialog::onFontSizeChanged(int value)
{
    applyFontSize(value);
    
    QFont previewFont = ui->fontPreviewLabel->font();
    previewFont.setPointSize(value);
    ui->fontPreviewLabel->setFont(previewFont);
    
    qDebug() << "Font size changed to:" << value;
}

void SettingDialog::applyFontSize(int size)
{
    QFont newFont = defaultFont;
    newFont.setPointSize(size);
    qApp->setFont(newFont);
}

void SettingDialog::onNotificationToggled(bool checked)
{
    QSettings settings("HungerYet", "Client");
    settings.setValue("notification", checked);
    
    if (checked)
    {
        // 创建系统托盘图标来显示通知
        static QSystemTrayIcon *trayIcon = nullptr;
        if (!trayIcon)
        {
            trayIcon = new QSystemTrayIcon(QIcon(":/resources/Application/icon.png"), this);
            trayIcon->show();
        }
        trayIcon->showMessage("通知已启用", "您将收到订单和消息通知", QSystemTrayIcon::Information, 3000);
    }
}

void SettingDialog::onAutoStartToggled(bool checked)
{
#ifdef Q_OS_WIN
    QSettings bootUpSettings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                           QSettings::NativeFormat);
    QString appName = QApplication::applicationName();
    QString appPath = QApplication::applicationFilePath();
    
    if (checked)
    {
        bootUpSettings.setValue(appName, appPath.replace("/", "\\"));
    }
    else
    {
        bootUpSettings.remove(appName);
    }
#endif

#ifdef Q_OS_LINUX
    QString autoStartPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) 
                          + "/autostart/";
    QString desktopFile = autoStartPath + "hungeryet.desktop";
    
    if (checked)
    {
        QDir dir(autoStartPath);
        if (!dir.exists())
        {
            dir.mkpath(".");
        }
        
        QFile file(desktopFile);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream stream(&file);
            stream << "[Desktop Entry]\n"
                  << "Type=Application\n"
                  << "Name=HungerYet\n"
                  << "Exec=" << QApplication::applicationFilePath() << "\n"
                  << "Terminal=false\n"
                  << "Hidden=false\n";
            file.close();
        }
    }
    else
    {
        QFile::remove(desktopFile);
    }
#endif
}

void SettingDialog::onClearCacheClicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "清除缓存",
        "确定要清除所有缓存吗？",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes)
    {
        QDir cacheDir("../resources/Application/tmp");
        if (cacheDir.exists())
        {
            cacheDir.removeRecursively();
            QMessageBox::information(this, "提示", "缓存已清除");
        }
    }
}

void SettingDialog::onAboutClicked()
{
    QString aboutText = QString(
        "HungerYet v1.0.0\n\n"
        "一个简洁、高效的点餐应用\n\n"
        "功能特点：\n"
        "• 简单直观的操作界面\n"
        "• 智能的订单管理系统\n"
        "• 实时的通知提醒\n"
        "• 个性化的设置选项\n\n"
        "技术支持:18737519552@qq.com\n"
        "© 2024 KBchulan. All rights reserved.\n"
        "© 2024 fgloris. All rights reserved."
    );

    QMessageBox::about(this, "关于 HungerYet", aboutText);
}

void SettingDialog::onSoundToggled(bool checked)
{
    QSettings settings("HungerYet", "Client");
    settings.setValue("sound", checked);
}

void SettingDialog::onAnimationToggled(bool checked)
{
    QSettings settings("HungerYet", "Client");
    settings.setValue("animation", checked);
}

void SettingDialog::onLanguageChanged(int index)
{
    QSettings settings("HungerYet", "Client");
    settings.setValue("language", index);
    
    QMessageBox::information(this, "提示",
        "语言设置将在重启应用后生效",
        QMessageBox::Ok);
}

QString SettingDialog::loadStyleSheet(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return QString();
    }
    return QString::fromUtf8(file.readAll());
}
