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
    , m_fetcher(new StageFetcher(this))
{
    connect(m_fetcher, &StageFetcher::variantsFetched, this, [this](const QStringList &variants) {
        emit variantsReady(variants);
    });

    connect(m_fetcher, &StageFetcher::tarballFetched, this, [this](const QString &tarball) {
        updateTarball(tarball);
    });

    connect(m_fetcher, &StageFetcher::fetchStatusChanged,this,&Config::fetchStatusChanged);
    connect(m_fetcher, &StageFetcher::fetchError,this,&Config::fetchError);
    /// change Config into function handles the fetcher signals
}

QStringList Config::availableArchitectures()
{
    return { "livecd", "amd64", "arm", "arm64", "x86" };
}

QList<ArchitectureInfo> Config::availableArchitecturesInfo()
{
    QList<ArchitectureInfo> list;
    list << ArchitectureInfo{ QStringLiteral("livecd"), QStringLiteral("Live CD (unsafe)") }
         << ArchitectureInfo{ QStringLiteral("alpha"),   QStringLiteral("Digital Alpha (alpha)") }
         << ArchitectureInfo{ QStringLiteral("amd64"),   QStringLiteral("64-bit Intel/AMD (amd64)") }
         << ArchitectureInfo{ QStringLiteral("x86"),     QStringLiteral("32-bit Intel/AMD (x86)") }
         << ArchitectureInfo{ QStringLiteral("arm"),     QStringLiteral("ARM 32-bit (arm)") }
         << ArchitectureInfo{ QStringLiteral("arm64"),   QStringLiteral("ARM 64-bit (arm64)") }
         << ArchitectureInfo{ QStringLiteral("hppa"),    QStringLiteral("HPPA (hppa)") }
         << ArchitectureInfo{ QStringLiteral("ia64"),    QStringLiteral("Intel Itanium (ia64)") }
         << ArchitectureInfo{ QStringLiteral("loong"),   QStringLiteral("Loongson MIPS-based (loong)") }
         << ArchitectureInfo{ QStringLiteral("m68k"),    QStringLiteral("Motorola 68k (m68k)") }
         << ArchitectureInfo{ QStringLiteral("mips"),    QStringLiteral("MIPS 32/64-bit (mips)") }
         << ArchitectureInfo{ QStringLiteral("ppc"),     QStringLiteral("PowerPC (ppc)") }
         << ArchitectureInfo{ QStringLiteral("riscv"),   QStringLiteral("RISC-V 32/64-bit (riscv)") }
         << ArchitectureInfo{ QStringLiteral("s390"),    QStringLiteral("IBM System z (s390)") }
         << ArchitectureInfo{ QStringLiteral("sh"),      QStringLiteral("SuperH legacy (sh)") }
         << ArchitectureInfo{ QStringLiteral("sparc"),   QStringLiteral("SPARC 64-bit (sparc)") };
    return list;
}

void Config::availableStagesFor(const QString& arch)
{
    m_selectedArch = arch;
    m_selectedVariant.clear();
    if(arch == "livecd"){
        m_fetcher->cancelOngoingRequest();
        m_selectedTarball = "livecd";
        emit tarballReady(m_selectedTarball);
        emit fetchStatusChanged("LiveCD mode");
        return;
    }
    else{
        m_selectedTarball.clear();
        m_fetcher->fetchVariants(arch);
    }
}

void Config::selectVariant(const QString& variant)
{
    m_selectedVariant = variant;

    m_fetcher->fetchLatestTarball(m_selectedArch,variant);
}

QString Config::selectedStage3() const
{
    if(!m_selectedTarball.isEmpty())
        return m_selectedTarball;

    return "No tar fetched";
}

bool Config::isValid() const
{
    return (!m_selectedTarball.isEmpty()) ;
}

void Config::updateTarball(const QString &tarball){
    m_selectedTarball = tarball;
    emit tarballReady(tarball);
    emit validityChanged(isValid());
}

void Config::updateGlobalStorage()
{
    Calamares::GlobalStorage* gs = Calamares::JobQueue::instance()->globalStorage();

    if(m_selectedArch == "livecd")
        gs->insert("GENTOO_LIVECD","yes");
    else{
        gs->insert("GENTOO_LIVECD","no");
        gs->insert( "BASE_DOWNLOAD_URL",  QString("https://distfiles.gentoo.org/releases/%1/autobuilds/%2/").arg(m_selectedArch,m_selectedVariant));
        gs->insert( "FINAL_DOWNLOAD_URL",  QString("https://distfiles.gentoo.org/releases/%1/autobuilds/%2/%3").arg(m_selectedArch,m_selectedVariant,m_selectedTarball));
        gs->insert( "STAGE_NAME_TAR", m_selectedTarball );
    }
}


