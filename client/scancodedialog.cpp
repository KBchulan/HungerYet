#include "scancodedialog.h"
#include "ui_scancodedialog.h"

#include <QUrl>
#include <QDebug>
#include <QTimer>
#include <QClipboard>
#include <QBoxLayout>
#include <QShowEvent>
#include <QHideEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QPushButton>
#include <QApplication>
#include <QDesktopServices>

ScanCodeDialog::ScanCodeDialog(QWidget *parent) : QDialog(parent),
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
        "}");
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
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("无法打开摄像头，请检查设备是否正确连接");
        msgBox.setInformativeText("您可以选择上传本地二维码图片或取消扫码");

        QPushButton *uploadButton = msgBox.addButton("上传本地图片", QMessageBox::ActionRole);
        QPushButton *cancelButton = msgBox.addButton("取消扫码", QMessageBox::RejectRole);

        msgBox.exec();

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

    QImage image = cvMatToQImage(frame);
    ui->preview_label->setPixmap(QPixmap::fromImage(image.scaled(ui->preview_label->size(), Qt::KeepAspectRatio)));

    if (processQrCode(frame))
    {
        stopScanning();
        emit ui->return_menu_btn->clicked();
    }
}

bool ScanCodeDialog::processQrCode(const cv::Mat &frame)
{
    cv::QRCodeDetector qrDecoder;
    std::string decodedText;
    std::vector<cv::Point> points;

    // 尝试检测和解码二维码
    decodedText = qrDecoder.detectAndDecode(frame, points);

    if (!decodedText.empty())
    {
        QString result = QString::fromStdString(decodedText);
        qDebug() << "扫描到二维码:" << result;

        // 发出扫描结果信号
        emit qrCodeScanned(result);

        // 处理二维码内容
        handleQRContent(result);

        return true;
    }
    return false;
}

QImage ScanCodeDialog::cvMatToQImage(const cv::Mat &mat)
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

    return QImage((uchar *)rgb.data, rgb.cols, rgb.rows,
                  rgb.step, QImage::Format_RGB888)
        .copy();
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

    if (fileName.isEmpty())
    {
        // 如果用户取消选择，则返回主菜单
        stopScanning();
        emit ui->return_menu_btn->clicked();
        return;
    }

    // 读取图片
    cv::Mat image = cv::imread(fileName.toStdString());
    if (image.empty())
    {
        QMessageBox::warning(this, "错误", "无法读取所选图片");
        stopScanning();
        emit ui->return_menu_btn->clicked();
        return;
    }

    // 处理二维码
    if (processQrCode(image))
    {
        qDebug() << "二维码处理成功";
    }
    else
    {
        QMessageBox::warning(this, "错误", "未能在图片中识别到二维码");
    }
    stopScanning();
    emit ui->return_menu_btn->clicked();
}

void ScanCodeDialog::handleQRContent(const QString &content)
{
    if (content.contains("wxp://") || 
        content.contains("weixin://") || 
        content.contains("wx.tenpay.com") ||
        content.startsWith("https://weixin.qq.com") ||
        content.contains("wechat.com"))
    {
        QMessageBox msgBox;
        msgBox.setText("检测到微信二维码");
        msgBox.setInformativeText("是否打开微信处理？\n注意：在Linux系统下可能需要手动打开微信扫一扫");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        
        if (msgBox.exec() == QMessageBox::Yes)
        {
            #ifdef Q_OS_LINUX
                QGuiApplication::clipboard()->setText(content);
                QMessageBox::information(this, "微信二维码",
                    "二维码内容已复制到剪贴板。\n"
                    "请手动打开微信，使用扫一扫，选择\"从相册选取二维码\"");
            #else
                QDesktopServices::openUrl(QUrl(content));
            #endif
        }
    }
    else if (content.contains("ALIPAY.COM", Qt::CaseInsensitive) || 
             content.contains("alipay.com") ||
             content.startsWith("alipays://"))
    {
        QMessageBox msgBox;
        msgBox.setText("检测到支付宝付款码");
        msgBox.setInformativeText("是否打开支付宝进行支付？");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        
        if (msgBox.exec() == QMessageBox::Yes)
        {
            QDesktopServices::openUrl(QUrl(content));
        }
    }
    else if (content.startsWith("http://") || content.startsWith("https://"))
    {
        QMessageBox msgBox;
        msgBox.setText("检测到网页链接");
        msgBox.setInformativeText("是否打开该链接？\n" + content);
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        
        if (msgBox.exec() == QMessageBox::Yes)
        {
            QDesktopServices::openUrl(QUrl(content));
        }
    }
    else
    {
        QMessageBox::information(this, "扫描结果",
                               "扫描内容：\n" + content + "\n\n内容已复制到剪贴板");
        QGuiApplication::clipboard()->setText(content);
    }
}
