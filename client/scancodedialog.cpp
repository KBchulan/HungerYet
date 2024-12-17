#include "scancodedialog.h"
#include "ui_scancodedialog.h"

#include <QDebug>
#include <QTimer>
#include <QBoxLayout>
#include <QShowEvent>
#include <QHideEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QPushButton>

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
    isScanning = false;
    QDialog::showEvent(event);
    startScanning();
}

void ScanCodeDialog::hideEvent(QHideEvent *event)
{
    stopScanning();
    isScanning = false;
    QDialog::hideEvent(event);
}

void ScanCodeDialog::initCamera()
{
    qDebug() << "initCamera";
    if (!cap.open(0))
    {
        // 下面前两行禁用了能进入界面看看
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("无法打开摄像头，请检查设备是否正确连接");
        msgBox.setInformativeText("您可以选择上传本地二维码图片或取消扫码");
        
        // 添加两个按钮：上传图片 和 返回
        QPushButton *uploadButton = msgBox.addButton("上传本地图片", QMessageBox::ActionRole);
        QPushButton *cancelButton = msgBox.addButton("取消扫码", QMessageBox::RejectRole);
        
        msgBox.exec();
        
        // 根据用户选择执行相应操作
        if (msgBox.clickedButton() == uploadButton) 
        {
            handleLocalImage();
        }
        else if (msgBox.clickedButton() == cancelButton)
        {
            stopScanning();
            emit ui->return_menu_btn->clicked();
        }
        return;
    }
    
    // 获取摄像头支持的最大分辨率
    double maxWidth = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    double maxHeight = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    
    int targetWidth = ui->preview_label->width();
    int targetHeight = ui->preview_label->height();
    
    double aspectRatio = static_cast<double>(targetWidth) / targetHeight;
    int width = std::min(static_cast<int>(maxWidth), targetWidth);
    int height = static_cast<int>(width / aspectRatio);
    
    if (height > maxHeight)
    {
        height = static_cast<int>(maxHeight);
        width = static_cast<int>(height * aspectRatio);
    }
    
    // 设置分辨率
    cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
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

    // TODO: 处理二维码
    
    if (processQrCode(frame)){
        // 转换并显示图像
        QImage image = cvMatToQImage(frame);
        ui->preview_label->setPixmap(QPixmap::fromImage(image));
        stopScanning();
        emit ui->return_menu_btn->clicked();
    }
}

bool ScanCodeDialog::processQrCode(const cv::Mat& frame)
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
        return true;
    }else{
        return false;
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
    scanAnimation->setStartValue(QRect(15, 60, 400, 2));
    scanAnimation->setEndValue(QRect(15, 691, 400, 2));
    scanAnimation->setLoopCount(-1);
}

void ScanCodeDialog::handleLocalImage()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("选择图片"),
        "",
        tr("图片文件 (*.png *.jpg *.jpeg *.bmp)"));
        
    if (fileName.isEmpty()) {
        // 如果用户取消选择，则返回主菜单
        stopScanning();
        emit ui->return_menu_btn->clicked();
        return;
    }
        
    // 读取图片
    cv::Mat image = cv::imread(fileName.toStdString());
    if (image.empty()) {
        QMessageBox::warning(this, "错误", "无法读取所选图片");
        stopScanning();
        emit ui->return_menu_btn->clicked();
        return;
    }
    qDebug() << "handleLocalImage success";
    // 处理二维码
    if (processQrCode(image)){
        qDebug() << "processQrCode success";
    }else{
        QMessageBox::warning(this, "错误", "无法读取所选图片");
    }
    stopScanning();
    emit ui->return_menu_btn->clicked();
}
