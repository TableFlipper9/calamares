#ifndef STAGEFETCHER_H
#define STAGEFETCHER_H

#include <QObject>
#include <QStringList>
#include <QUrl>

class StageFetcher : public QObject
{
    Q_OBJECT

public: 
    static QStringList fetchVariants(const QString& arch);

    static QString fetchHtml(const QUrl& url);

    static QString extractvariantBase(const QString& varaint);

    static QString fetchLatestTarball(const QString& arch, const QString& variant);
};

#endif //STAGEFETCHER_H