/* === This file is part of Calamares - <https://calamares.io> ===
 *
 *   SPDX-FileCopyrightText: 2019-2020 Adriaan de Groot <groot@kde.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 *   Calamares is Free Software: see the License-Identifier above.
 *
 */

#ifndef CONFIG_H
#define CONFIG_H

#include "StageFetcher.h"
#include <QObject>
#include <QPair>
#include <QString>
#include <QList>

class Config : public QObject
{
    Q_OBJECT

public:
    explicit Config(QObject* parent = nullptr);

    QStringList availableArchitectures();
    void availableStagesFor(const QString& architecture);
    void selectVariant(const QString& variantKey);

    QString selectedStage3() const;
    bool isValid() const;

    void updateGlobalStorage();
    void updateTarball(const QString &tarball);

signals:
    void variantsReady(const QStringList& variants);
    void tarballReady(const QString& tarball);
    void fetchStatusChanged(const QString& status);
    void fetchError(const QString& error);

private:
    StageFetcher* m_fetcher;
    QString m_selectedArch;
    QString m_selectedVariant;
    QString m_selectedTarball;
};

#endif // CONFIG_H

