/* === This file is part of Calamares - <https://calamares.io> ===
 *
 *   SPDX-FileCopyrightText: 2019-2020 Adriaan de Groot <groot@kde.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 *   Calamares is Free Software: see the License-Identifier above.
 *
 */

#include "Config.h"
#include "locale/Global.h"
#include "JobQueue.h"
#include "GlobalStorage.h"
#include "StageFetcher.h"

#include <QDateTime>

Config::Config(QObject* parent)
    : QObject(parent)
{

}

QStringList Config::availableArchitectures()
{
    return { "amd64", "arm", "arm64", "x86", "livecd"};
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

    m_selectedTarball = StageFetcher::fetchLatestTarball(m_selectedArch,variant);
}

QString Config::selectedStage3() const
{
    return m_selectedTarball;
}

bool Config::isValid() const
{
    return !m_selectedTarball.isEmpty();
}

void Config::updateGlobalStorage()
{
    Calamares::GlobalStorage* gs = Calamares::JobQueue::instance()->globalStorage();

    gs->insert( "BASE_DOWNLOAD_URL",  QString("https://distfiles.gentoo.org/releases/%1/autobuilds/%2/").arg(m_selectedArch,m_selectedVariant));
    gs->insert( "FINAL_DOWNLOAD_URL",  QString("https://distfiles.gentoo.org/releases/%1/autobuilds/%2/%3").arg(m_selectedArch,m_selectedVariant,m_selectedTarball));
    gs->insert( "STAGE_NAME_TAR", m_selectedTarball );

    if(m_selectedArch == "livecd")
        gs->insert("GENTOO_LIVECD","yes");
    else
        gs->insert("GENTOO_LIVECD","no");
}


