#ifndef STAGEFETCHER_H
#define STAGEFETCHER_H

#include <QObject>
#include <QStringList>

class StageFetcher : public QObject
{
    Q_OBJECT

public: 
    explicit StageFetcher(QObject* parent = nullptr);

    static QStringList fetchVariants(const QString& arch);

private:
    static QString fetchHtml(const QUrl& url);
};

#endif //STAGEFETCHER_H