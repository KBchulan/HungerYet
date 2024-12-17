/******************************************************************************
 *
 * @file       settingdialog.h
 * @brief      设置页面
 *
 * @author     KBchulan
 * @date       2024/12/17
 * @history
 *****************************************************************************/
#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>

namespace Ui
{
    class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(QWidget *parent = nullptr);
    ~SettingDialog();

private slots:
    void onThemeChanged(int index);

    void onFontSizeChanged(int value);

    void onNotificationToggled(bool checked);

    void onSoundToggled(bool checked);
    
    void onAnimationToggled(bool checked);
    
    void onLanguageChanged(int index);
    
    void onAutoStartToggled(bool checked);
    
    void onClearCacheClicked();
    
    void onAboutClicked();

private:
    Ui::SettingDialog *ui;
    void setupUserInterface();
    void setupConnections();
    void loadSettings();
    void saveSettings();
    
    void applyTheme(int themeIndex);
    void applyFontSize(int size);
    void updateStyleSheet();
    
    static QString loadStyleSheet(const QString &filename);
    QFont defaultFont;
};

#endif // SETTINGDIALOG_H
