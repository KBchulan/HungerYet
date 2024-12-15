#include <global.h>
#include <mainwindow.h>

#include <QFile>
#include <QApplication> 

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // load and open qss
    QFile qss(":/style/stylesheet.qss");
    if(qss.open(QFile::ReadOnly))
    {
        qDebug("Open success!");
        QString style = QString::fromUtf8(qss.readAll());
        a.setStyleSheet(style);
        qss.close();
    }
    else
    {
        qDebug("Open failed!");
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
