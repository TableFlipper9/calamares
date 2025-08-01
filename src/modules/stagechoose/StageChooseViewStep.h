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

#include "DllMacro.h"
#include "utils/PluginFactory.h"
#include "viewpages/ViewStep.h"

class StageChoosePage;
class Config;

class PLUGINDLLEXPORT StageChooseViewStep : public Calamares::ViewStep
{
    Q_OBJECT

public:
    explicit StageChooseViewStep(QObject* parent = nullptr);
    ~StageChooseViewStep() override;

    QString prettyName() const override;

    QWidget* widget() override;

    bool isNextEnabled() const override;
    bool isBackEnabled() const override;
    bool isAtBeginning() const override;
    bool isAtEnd() const override;

    Calamares::JobList jobs() const override;

    void onLeave() override;

private:
    Config* m_config;
    StageChoosePage* m_widget;
};

CALAMARES_PLUGIN_FACTORY_DECLARATION( StageChooseViewStepFactory )

#endif // STAGECHOOSEVIEWSTEP_H

