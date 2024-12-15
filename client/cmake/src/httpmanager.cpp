#include <httpmanager.h>

HttpManager::~HttpManager()
{
    std::cout << R"(HttpManager has been destructed!)" << '\n';
}

void HttpManager::slot_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod)
{
    if (mod == Modules::REGISTERMOD)
    {
        // Sends a signal to notify the specified module that the http response has ended
        emit sig_reg_mod_finish(id, res, err);
    }
    if(mod == Modules::RESETMOD)
    {
        // Sends a signal to notify the specified module that the http response has ended
        emit sig_reset_mod_finish(id, res, err);
    }
    if(mod == Modules::LOGINMOD)
    {
        emit sig_login_mod_finish(id, res, err);
    }
}

HttpManager::HttpManager()
{
    connect(this, &HttpManager::sig_http_finished, this, &HttpManager::slot_http_finish);
}

void HttpManager::PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod)
{
    QByteArray data = QJsonDocument(json).toJson();
    QNetworkRequest request(url);

    // http: when receive the response, it will closed
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(data.length()));

    auto self = shared_from_this();
    QNetworkReply *reply = _manager.post(request, data);

    QObject::connect(reply, &QNetworkReply::finished, [self, reply, req_id, mod]()
    {
        // if error
        if(reply->error() != QNetworkReply::NoError)
        {
            qDebug() << reply->errorString();
            // tell other modules this thing finished;
            emit self->sig_http_finished(req_id, "", ErrorCodes::ERR_NETWORK, mod);
            reply->deleteLater();
            return;
        }

        QString res = reply->readAll();
        emit self->sig_http_finished(req_id, res, ErrorCodes::SUCCESS, mod);
        reply->deleteLater();
        return;
    });
}
