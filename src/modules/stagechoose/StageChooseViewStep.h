/* === This file is part of Calamares - <https://calamares.io> ===
 *
 *   SPDX-FileCopyrightText: 2014-2015 Teo Mrnjavac <teo@kde.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 *   Calamares is Free Software: see the License-Identifier above.
 *
 */

#ifndef STAGECHOOSEVIEWSTEP_H
#define STAGECHOOSEVIEWSTEP_H

#include <QObject>
#include <QWidget>
#include <QString>

#include <utils/PluginFactory.h>
#include <viewpages/ViewStep.h>

class StageChoosePage;
class Config;

class StageChooseViewStep : public Calamares::ViewStep
{
    Q_OBJECT

public:
    explicit StageChooseViewStep(QObject* parent = nullptr);
    ~StageChooseViewStep() override;

    QString prettyName() const override;

    QWidget* widget() override;

    void next() override;
    void back() override;

    bool isNextEnabled() const override;
    bool isBackEnabled() const override;
    bool isAtBeginning() const override;
    bool isAtEnd() const override;

    Calamares::JobList jobs() const override;

private:
    StageChoosePage* m_widget;
    Config* m_config;
};

#endif // STAGECHOOSEVIEWSTEP_H

