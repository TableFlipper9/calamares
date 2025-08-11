#include "StageFetcher.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QStringList>


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
    QStringList variants;

    QString urlStr = QString("https://distfiles.gentoo.org/releases/%1/autobuilds/").arg(arch);
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

QString StageFetcher::extractvariantBase(const QString& variant){
    if(variant.startsWith("current-"))
        return variant.mid(8);
    return variant;
}

QString StageFetcher::fetchLatestTarball(const QString& arch, const QString& variant)
{
    QString latest;
    latest = QString("No tar fetched");

    const QString baseUrl = QString("https://distfiles.gentoo.org/releases/%1/autobuilds/%2/").arg(arch,variant);
    QUrl url(baseUrl);
    QString html = fetchHtml(url);

    if(html.isEmpty())
        return latest;

    QRegularExpression re(QString("(%1-[\\dTZ]+\\.tar\\.xz)").arg(StageFetcher::extractvariantBase(variant)));
    QRegularExpressionMatchIterator iterator = re.globalMatch(html);

    while(iterator.hasNext()){
        QRegularExpressionMatch match = iterator.next();
        QString filename = match.captured(1);
        if(filename > latest){
            latest = filename;
        }
    }

    return latest;
}