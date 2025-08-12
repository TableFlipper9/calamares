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

    QStringList fetchVariants(const QString& arch);
    QString extractvariantBase(const QString& varaint);
    QString fetchLatestTarball(const QString& arch, const QString& variant);

signals:
    void fetchStatusChanged(const QString& status);
    void fetchError(const QString& error);
    void variantsFetched(const QStringList& variants);
    void tarballFetched(const QString& tarballs);

private slots:
    void onVariantsReplyFinished();
    void onTarballReplyFinished(const QString& variant);

private:
    void cancelOngoingRequest();
    QNetworkAccessManager m_nam;
    QPointer<QNetworkReply> m_currentReply;
};

#endif //STAGEFETCHER_H