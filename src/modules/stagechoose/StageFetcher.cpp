#include "StageFetcher.h"

#include <QNetworkAccessManager>

StageFetcher::StageFetcher(QObject* parent)
    : QObject(parent)
{
}

QString StageFetcher::fetchHtml(const QUrl& url)
{
    QNetworkAccessManager manager;
    QNetworkRequest request(url);

    QEventLoop loop;
    QNetworkReply* reply = manager.get(request);
    QObject::connect(reply,&QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec(); 

    QString html;
    if(reply->error() == QNetworkReply::NoError){
        html = reply->readAll();
    }
    else{
        qWarning() << "StageFetcher: Couldn't fetch" << url.toString() << reply->errorString();
    }

    reply->deleteLater();
    return html;
}

QStringList StageFetcher::fetchVariants(const QString& arch)
{
    QstringList variants;

    QString urlStr = QString("http://distfiles.gentoo.org/releases/%1/autobuilds/").arg(arch);
    QUrl url(urlStr);
    QString html = fetchHtml(url);

    if(html.isEmpty())
        return variants;

    QRegularExpression re(R"((current-stage3-[^"/]+)[/])");
    QRegularExpressionMatchIterator iterator = re.globalMatch(html);

    QStringList seen;
    while(iterator.hasNext()){
        QRegularExpressionMatch match = iterator.next();
        QString variant = match.captured(1);
        if(!seen.contains(variant)){
            variants.append(variant);
            seen.append(variant);
        }
    }

    return variants;
}