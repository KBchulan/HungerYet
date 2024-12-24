#include "global.h"
#include "mainwindow.h"

#include <QFile>
#include <QDateTime>
#include <QApplication> 

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // 获取当前时间
    QFile logFile("../resources/Application/tmp/log.txt");
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    logFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream stream(&logFile);
    // 根据消息类型设置前缀
    QString prefix;
    switch (type) {
        case QtDebugMsg:
            prefix = "Debug";
            break;
        case QtWarningMsg:
            prefix = "Warning";
            break;
        case QtCriticalMsg:
            prefix = "Critical";
            break;
        case QtFatalMsg:
            prefix = "Fatal";
            break;
        case QtInfoMsg:
            prefix = "Info";
            break;
    }
    // 写入日志
    stream << time << " " << prefix << " " << msg << "\n";
    stream << flush;
    logFile.close();
}

int main(int argc, char *argv[])
{
    QDir temp("../resources/Application/tmp");
    if(!temp.exists())
        temp.mkpath(".");
    qInstallMessageHandler(customMessageHandler);
    QApplication a(argc, argv);

    // 加载样式表并打印调试信息
    QFile qss(":/style/stylesheet.qss");
    if(qss.open(QFile::ReadOnly))
    {
        qDebug() << "Style sheet loaded successfully";
        QString style = QString::fromUtf8(qss.readAll());
        a.setStyleSheet(style);
        qss.close();
    }
    else
    {
        qDebug() << "Failed to load style sheet";
    }
    // read config
    QString config_path = ":/resources/config.ini";
    QSettings settings(config_path, QSettings::IniFormat);
    QString gate_host = settings.value("GateServer/host").toString();
    QString gate_port = settings.value("GateServer/port").toString();
    gate_url_prefix = "http://" + gate_host + ":" + gate_port;

    MainWindow w;
    w.show();
    return a.exec();
}
