#include "scancodedialog.h"
#include "ui_scancodedialog.h"

#include <QDebug>
#include <QTimer>
#include <QShowEvent>
#include <QHideEvent>

ScanCodeDialog::ScanCodeDialog(QWidget *parent) :
    QDialog(parent),
    isScanning(false),
    ui(new Ui::ScanCodeDialog)
{
    ui->setupUi(this);
    
    // 初始化定时器
    frameTimer = new QTimer(this);
    connect(frameTimer, &QTimer::timeout, this, &ScanCodeDialog::updateFrame);
    
    // 初始化扫描动画
    initScanAnimation();
    
    // 设置扫描线样式
    ui->scan_line->setStyleSheet(
        "QLabel "
        "{"
        "    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,"
        "        stop:0 rgba(0,255,0,0),"
        "        stop:0.5 rgba(0,255,0,255),"
        "        stop:1 rgba(0,255,0,0));"
        "}"
    );
}

ScanCodeDialog::~ScanCodeDialog()
{
    stopScanning();
    delete ui;
}

void ScanCodeDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    startScanning();
}

void ScanCodeDialog::hideEvent(QHideEvent *event)
{
    stopScanning();
    QDialog::hideEvent(event);
}

void ScanCodeDialog::initCamera()
{
    if (!cap.open(0))
    {
        qDebug() << "无法打开摄像头";
        return;
    }
    
    // 设置摄像头分辨率
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
}

void ScanCodeDialog::startScanning()
{
    if (!isScanning)
    {
        initCamera();
        frameTimer->start(33);
        scanAnimation->start();
        isScanning = true;
    }
}

void ScanCodeDialog::stopScanning()
{
    if (isScanning)
    {
        frameTimer->stop();
        scanAnimation->stop();
        if (cap.isOpened())
            cap.release();
        isScanning = false;
    }
}

void ScanCodeDialog::updateFrame()
{
    cv::Mat frame;
    if (!cap.read(frame))
        return;

    // 处理二维码
    processQrCode(frame);
    
    // 转换并显示图像
    QImage image = cvMatToQImage(frame);
    ui->preview_label->setPixmap(QPixmap::fromImage(image));
}

void ScanCodeDialog::processQrCode(const cv::Mat& frame)
{
    cv::QRCodeDetector qrDecoder;
    std::string decodedText;
    std::vector<cv::Point> points;
    
    // 尝试检测和解码二维码
    decodedText = qrDecoder.detectAndDecode(frame, points);
    
    if (!decodedText.empty())
    {
        qDebug() << "扫描到二维码:" << QString::fromStdString(decodedText);
        // TODO: 处理扫描结果
        stopScanning();
        accept();
    }
}

QImage ScanCodeDialog::cvMatToQImage(const cv::Mat& mat)
{
    cv::Mat rgb;
    if (mat.channels() == 3)
    {
        cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
    }
    else
    {
        rgb = mat;
    }
    
    return QImage((uchar*)rgb.data, rgb.cols, rgb.rows,
                 rgb.step, QImage::Format_RGB888).copy();
}

void ScanCodeDialog::initScanAnimation()
{
    scanAnimation = new QPropertyAnimation(ui->scan_line, "geometry", this);
    scanAnimation->setDuration(2000);
    scanAnimation->setStartValue(QRect(48, 0, 400, 2));
    scanAnimation->setEndValue(QRect(48, 691, 400, 2));
    scanAnimation->setLoopCount(-1);
}