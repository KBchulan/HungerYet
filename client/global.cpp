#include "global.h"

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
    
    const QString key = "WhxChat";
    int keyLength = key.length();
    
    for(int i = 0; i < length; i++) 
        result[i] = QChar(input[i].unicode() ^ key[i % keyLength].unicode());
    
    return result;
};
