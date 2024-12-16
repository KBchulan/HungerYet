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

#include <QTimer>
#include <QDialog>
#include <opencv2/opencv.hpp>
#include <QPropertyAnimation>

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

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private slots:
    void updateFrame();
    void processQrCode(const cv::Mat& frame);

private:
    void initCamera();
    void startScanning();
    void stopScanning();
    QImage cvMatToQImage(const cv::Mat& mat);
    void initScanAnimation();

private:
    bool isScanning;
    Ui::ScanCodeDialog *ui;
    cv::VideoCapture cap;
    QTimer *frameTimer;
    QPropertyAnimation *scanAnimation;
};

#endif // SCANCODEDIALOG_H
