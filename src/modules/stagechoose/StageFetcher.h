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
};

#endif //STAGEFETCHER_H