/******************************************************************************
 *
 * @file       scancodedialog.h
 * @brief      扫描二维码
 *
 * @author     KBchulan
 * @date       2024/12/16
 * @history
 *****************************************************************************/
#ifndef SCANCODEDIALOG_H
#define SCANCODEDIALOG_H

#include <QUrl>
#include <QTimer>
#include <QDialog>
#include <opencv2/opencv.hpp>
#include <QPropertyAnimation>
#include <QDesktopServices>

namespace Ui
{
    class ScanCodeDialog;
}

class ScanCodeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScanCodeDialog(QWidget *parent = nullptr);
    ~ScanCodeDialog();

signals:
    // 添加扫描结果信号
    void qrCodeScanned(const QString &result);

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private slots:
    void updateFrame();
    bool processQrCode(const cv::Mat& frame);
    void handleQRContent(const QString &content);

private:
    void initCamera();
    void startScanning();
    void stopScanning();
    QImage cvMatToQImage(const cv::Mat& mat);
    void initScanAnimation();
    void handleLocalImage();

private:
    bool isScanning;
    Ui::ScanCodeDialog *ui;
    cv::VideoCapture cap;
    QTimer *frameTimer;
    QPropertyAnimation *scanAnimation;
};

#endif // SCANCODEDIALOG_H
