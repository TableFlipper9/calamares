#ifndef STAGEFETCHER_H
#define STAGEFETCHER_H

#include <QNetworkAccessManager>
#include <QObject>
#include <QNetworkReply>
#include <QPointer>
#include <QString>
#include <QStringList>
#include <QUrl>

class StageFetcher : public QObject
{
    Q_OBJECT

public: 
    explicit StageFetcher(QObject* parent =nullptr);

    void fetchVariants(const QString& arch);
    QString extractvariantBase(const QString& varaint);
    void fetchLatestTarball(const QString& arch, const QString& variant);

    void setMirrorBase(const QString& mirror);
    void cancelOngoingRequest();

signals:
    void fetchStatusChanged(const QString& status);
    void fetchError(const QString& error);
    void variantsFetched(const QStringList& variants);
    void tarballFetched(const QString& tarballs);

private slots:
    void onVariantsReplyFinished(QNetworkReply* reply);
    void onTarballReplyFinished(QNetworkReply* reply, const QString& variant);

private:
    QString m_mirrorBase {QStringLiteral("http://distfiles.gentoo.org/releases")};
    QNetworkAccessManager m_nam;
    QPointer<QNetworkReply> m_currentReply;
};

#endif //STAGEFETCHER_H