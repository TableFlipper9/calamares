/* === This file is part of Calamares - <https://calamares.io> ===
 *
 *   SPDX-FileCopyrightText: 2019-2020 Adriaan de Groot <groot@kde.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 *   Calamares is Free Software: see the License-Identifier above.
 *
 */

#include "Config.h"

#include <QDateTime>

Config::Config(QObject* parent)
    : QObject(parent)
{

}

QStringList Config::availableArchitectures()
{
    return { "amd64", "arm", "arm64", "x86" };
}

QStringList Config::availableStagesFor(const QString& arch)
{
    m_selectedArch = arch;
    m_selectedVariant.clear();
    m_selectedTarball.clear();

    return StageFetcher::fetchVariants(arch);
}

void Config::selectVariant(const QString& variant)
{
    m_selectedVariant = variant;

    QString archSuffix = variant;
    archSuffix.replace("current-", "");

    m_selectedTarball = QString("stage3-%1.tar.xz").arg(archSuffix);
}

QString Config::selectedStage3() const
{
    return m_selectedTarball;
}

bool Config::isValid() const
{
    return !m_selectedTarball.isEmpty();
}


