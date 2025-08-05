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
    // Hardcoded architectures (label, key)
    m_architectures = {
        { "AMD64 (x86_64)", "amd64" },
        { "ARM64 (aarch64)", "arm64" }
    };

    // qInfo() << "[Config] Architectures initialized:";
    // for (const auto& a : m_architectures)
    //     qInfo() << "  -" << a.first << "=>" << a.second;

}

QList<Config::Entry> Config::architectures() const
{
    // qDebug() << "[DEBUG] architectures() called, this=" << this;
    // qDebug() << "[DEBUG] m_architectures size=" << m_architectures.size();
    // for (const auto& entry : m_architectures)
    // {
    //     qDebug() << "  Label:" << entry.first << ", Value:" << entry.second;
    // }
    return m_architectures;
}


QList<Config::Entry> Config::variants() const
{
    return m_variants;
}

void Config::selectArchitecture(const QString& archKey)
{
    m_selectedArch = archKey;
    m_selectedVariant.clear();
    m_selectedTarball.clear();

    // Hardcoded variants for each arch
    if (archKey == "amd64") {
        m_variants = {
            { "Desktop Systemd", "current-stage3-amd64-desktop-systemd" },
            { "OpenRC Minimal", "current-stage3-amd64-openrc" }
        };
    } else if (archKey == "arm64") {
        m_variants = {
            { "Default", "current-stage3-arm64" },
            { "OpenRC", "current-stage3-arm64-openrc" }
        };
    } else {
        m_variants.clear();
    }
}

void Config::selectVariant(const QString& variantKey)
{
    m_selectedVariant = variantKey;

    // Simulate tarball name as: stage3-<variant>-<timestamp>.tar.xz
    QString now = QDateTime::currentDateTimeUtc().toString("yyyyMMdd'T'hhmmss'Z'");
    QString archSuffix = variantKey;
    archSuffix.replace("current-", "");

    m_selectedTarball = QString("stage3-%1-%2.tar.xz").arg(archSuffix, now);
}

QString Config::selectedStage3() const
{
    return m_selectedTarball;
}

bool Config::isValid() const
{
    return !m_selectedTarball.isEmpty();
}


