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

struct ArchitectureInfo
{
    QString name;
    QString description;

    ArchitectureInfo() = default;
    ArchitectureInfo(const QString& n, const QString& d):
    name(n),description(d){}
};

class Config : public QObject
{
    Q_OBJECT

public:
    explicit Config(QObject* parent = nullptr);

    QList<ArchitectureInfo> availableArchitecturesInfo();
    QStringList availableArchitectures();
    void availableStagesFor(const QString& architecture);
    void selectVariant(const QString& variantKey);

    QString selectedStage3() const;
    bool isValid() const;

    void updateGlobalStorage();
    void updateTarball(const QString &tarball);
    void setMirrorBase(const QString& mirror);
    QString mirrorBase();

signals:
    void variantsReady(const QStringList& variants);
    void tarballReady(const QString& tarball);
    void fetchStatusChanged(const QString& status);
    void fetchError(const QString& error);
    void validityChanged(bool validity);

private:
    StageFetcher* m_fetcher;
    QString m_mirrorBase {QStringLiteral("http://distfiles.gentoo.org/releases")};
    QString m_selectedArch;
    QString m_selectedVariant;
    QString m_selectedTarball;
};

#endif // CONFIG_H

