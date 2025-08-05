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

#include <QObject>
#include <QPair>
#include <QString>
#include <QList>

class Config : public QObject
{
    Q_OBJECT

public:
    using Entry = QPair<QString, QString>; // (label, value)

    explicit Config(QObject* parent = nullptr);

    QList<Entry> architectures() const;
    QList<Entry> variants() const;

    void selectArchitecture(const QString& archKey);
    void selectVariant(const QString& variantKey);

    QString selectedStage3() const;
    bool isValid() const;

private:
    QList<Entry> m_architectures;
    QList<Entry> m_variants;

    QString m_selectedArch;
    QString m_selectedVariant;
    QString m_selectedTarball;
};

#endif // CONFIG_H

