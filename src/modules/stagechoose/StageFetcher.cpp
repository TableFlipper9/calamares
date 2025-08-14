#include "StageFetcher.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QStringList>

StageFetcher :: StageFetcher(QObject* parent):QObject(parent)
{
}

QString StageFetcher::extractvariantBase(const QString& variant){
    if(variant.startsWith("current-"))
        return variant.mid(8);
    return variant;
}

void StageFetcher::cancelOngoingRequest()
{
    if(m_currentReply){
        disconnect(m_currentReply,nullptr,this,nullptr);
        if(m_currentReply->isRunning())
            m_currentReply->abort();
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
    }
}

// QString StageFetcher::fetchHtml(const QUrl& url)
// {
//     QNetworkAccessManager manager;
//     QNetworkRequest request(url);
//     QEventLoop loop;
//     QNetworkReply* reply = manager.get(request);
//     QObject::connect(reply,&QNetworkReply::finished, &loop, &QEventLoop::quit);
//     loop.exec(); 
//     QString html;
//     if(reply->error() == QNetworkReply::NoError){
//         html = reply->readAll();
//     }
//     else{
//         qWarning() << "StageFetcher: Couldn't fetch" << url.toString() << reply->errorString();
//     }
//     reply->deleteLater();
//     return html;
// }

void StageFetcher::fetchVariants(const QString& arch)
{
    cancelOngoingRequest(); 
    emit fetchStatusChanged("Fetching variants for " + arch + "...");

    QString urlStr = QString("http://distfiles.gentoo.org/releases/%1/autobuilds/").arg(arch);
    QUrl url(urlStr);
    QNetworkRequest request(url);

    QNetworkReply* reply = m_nam.get(request);
    m_currentReply = reply;
    connect(reply, &QNetworkReply::finished, this,[this, reply](){onVariantsReplyFinished(reply);});
}

void StageFetcher::onVariantsReplyFinished(QNetworkReply* reply)
{
    if(!reply)
        return;

    if(reply != m_currentReply){
        reply->deleteLater();
        return;
    }

    QStringList variants;
    if(reply->error() != QNetworkReply::NoError){
        emit fetchError(reply->errorString());
        reply->deleteLater();
        if(m_currentReply == reply) m_currentReply = nullptr;
        return;
    }

    QString html = reply->readAll();
     if(html.isEmpty())
        emit variantsFetched(variants);

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

    emit variantsFetched(variants);
    emit fetchStatusChanged("Idle");
    reply->deleteLater();
    if(reply == m_currentReply) m_currentReply = nullptr;
}

void StageFetcher::fetchLatestTarball(const QString& arch, const QString& variant)
{
    cancelOngoingRequest();
    emit fetchStatusChanged("Fetching Tarball for "+ variant +"...");
    const QString baseUrl = QString("http://distfiles.gentoo.org/releases/%1/autobuilds/%2/").arg(arch,variant);
    QUrl url(baseUrl);
    QNetworkRequest request(url);

    QNetworkReply* reply = m_nam.get(request);
    m_currentReply = reply;
    connect(reply, &QNetworkReply::finished, this, [this, reply, variant](){onTarballReplyFinished(reply, variant);});
}

void StageFetcher::onTarballReplyFinished(QNetworkReply* reply, const QString& variant)
{
    if(!reply)
        return;

    if(reply != m_currentReply){
        reply->deleteLater();
        return;
    }

    QString latest;
    if(reply->error() != QNetworkReply::NoError){
        emit fetchError(reply->errorString());
        reply->deleteLater();
        if(m_currentReply == reply) m_currentReply = nullptr;
        return;
    }

    QString html = reply->readAll();
    if(html.isEmpty())
        emit tarballFetched(latest);

    QRegularExpression re(QString("(%1-[\\dTZ]+\\.tar\\.xz)").arg(StageFetcher::extractvariantBase(variant)));
    QRegularExpressionMatchIterator iterator = re.globalMatch(html);

    while(iterator.hasNext()){
        QRegularExpressionMatch match = iterator.next();
        QString filename = match.captured(1);
        if(filename > latest){
            latest = filename;
        }
    }

    emit tarballFetched(latest);
    emit fetchStatusChanged("Idle");
    reply->deleteLater();
    if(reply == m_currentReply) m_currentReply = nullptr;
}