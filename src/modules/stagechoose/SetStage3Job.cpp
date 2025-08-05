#include "SetStage3Job.h"

#include "utils/Logger.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>

SetStage3Job::SetStage3Job(const QString& tarballName)
    : m_tarballName(tarballName)
{
}

QString SetStage3Job::prettyName() const
{
    return QString("Write selected Gentoo Stage3 to config: %1").arg(m_tarballName);
}

Calamares::JobResult SetStage3Job::exec()
{
    QString configPath = "/etc/calamares.conf";
    QFile file(configPath);
    QString contents;

    if (file.exists()) {
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return Calamares::JobResult::error(
                "Failed to open Calamares config file for reading.",
                configPath);
        }
        QTextStream in(&file);
        contents = in.readAll();
        file.close();
    }

    QString stage3Line = QString("stage3 = %1").arg(m_tarballName);

    if (contents.contains(QRegularExpression(R"(stage3\s*=)"))) {
        contents.replace(QRegularExpression(R"(stage3\s*=.*)"), stage3Line);
    } else {
        contents.append("\n" + stage3Line + "\n");
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        return Calamares::JobResult::error(
            "Failed to open Calamares config file for writing.",
            configPath);
    }

    QTextStream out(&file);
    out << contents;
    file.close();

    cDebug() << "Wrote stage3 tarball to config:" << m_tarballName;
    return Calamares::JobResult::ok();
}
