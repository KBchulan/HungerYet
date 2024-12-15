#include <global.h>

QString gate_url_prefix = "";

std::function<void(QWidget *)> repolish = [](QWidget *w)
{
    w->style()->unpolish(w);
    w->style()->polish(w);
};

std::function<QString(QString)> xorString = [](QString input)
{
    QString result = input;
    int length = input.length();
    
    // 使用固定密钥进行异或加密
    const QString key = "WhxChat";
    int keyLength = key.length();
    
    // 对每个字符进行异或操作
    for(int i = 0; i < length; i++) 
        result[i] = QChar(input[i].unicode() ^ key[i % keyLength].unicode());
    
    return result;
};
