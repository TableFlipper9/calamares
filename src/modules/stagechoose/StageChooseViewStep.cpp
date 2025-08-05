/* === This file is part of Calamares - <https://calamares.io> ===
 *
 *   SPDX-FileCopyrightText: 2014-2015 Teo Mrnjavac <teo@kde.org>
 *   SPDX-FileCopyrightText: 2018 Adriaan de Groot <groot@kde.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 *   Calamares is Free Software: see the License-Identifier above.
 *
 */

#include "StageChooseViewStep.h"

#include "Config.h"
#include "StageChoosePage.h"
#include "SetStage3Job.h"

#include "utils/Logger.h"

CALAMARES_PLUGIN_FACTORY_DEFINITION(StageChooserViewStepFactory, registerPlugin<StageChooserViewStep>();)

StageChooseViewStep::StageChooseViewStep(QObject* parent)
    : Calamares::ViewStep(parent)
    , m_config(new Config(this))
    , m_widget(new StageChoosePage(m_config))
{
    //m_widget->setConfig(m_config);
}

StageChooseViewStep::~StageChooseViewStep()
{
    delete m_widget;
}

QString StageChooseViewStep::prettyName() const
{
    return tr("Select Gentoo Stage3");
}

QWidget* StageChooseViewStep::widget()
{
    return m_widget;
}

bool StageChooseViewStep::isNextEnabled() const
{
    return m_config->isValid();
}

bool StageChooseViewStep::isBackEnabled() const
{
    return true;
}

bool StageChooseViewStep::isAtBeginning() const
{
    return true;
}

bool StageChooseViewStep::isAtEnd() const
{
    return true;
}

Calamares::JobList StageChooseViewStep::jobs() const
{
    Calamares::JobList list;
    if (m_config->isValid())
    {
        list.append(QSharedPointer<SetStage3Job>::create(m_config->selectedStage3()));
    }
    return list;
}
