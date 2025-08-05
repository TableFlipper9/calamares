/* === This file is part of Calamares - <https://calamares.io> ===
 *
 *   SPDX-FileCopyrightText: 2014-2015 Teo Mrnjavac <teo@kde.org>
 *   SPDX-FileCopyrightText: 2015 Anke Boersma <demm@kaosx.us>
 *   SPDX-FileCopyrightText: 2017-2019 Adriaan de Groot <groot@kde.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 *   Calamares is Free Software: see the License-Identifier above.
 *
 */
#include "StageChoosePage.h"
#include "Config.h"
#include "ui_StageChoosePage.h"

#include <QComboBox>
#include <QLabel>

StageChoosePage::StageChoosePage(Config* config, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::StageChoosePage)
    , m_config(config)
{
    ui->setupUi(this);

    populateArchs();

    connect(ui->architectureComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &StageChoosePage::onArchitectureChanged);
    connect(ui->variantComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &StageChoosePage::onVariantChanged);
}

StageChoosePage::~StageChoosePage()
{
    delete ui;
}

void StageChoosePage::populateArchs()
{
    qInfo() << "[DEBUG] m_config =" << m_config;
    if (!m_config)
    {
        qWarning() << "[populateArchs] m_config is null";
        return;
    }

    const auto archList = m_config->architectures();
    if (archList.isEmpty())
    {
        qWarning() << "[populateArchs] Architecture list is empty";
        return;
    }

    for (const auto& pair : archList)
    {
        if (pair.first.isEmpty() || pair.second.isEmpty())
        {
            qWarning() << "[populateArchs] Invalid entry:" << pair.first << "=>" << pair.second;
            continue;
        }

        qInfo() << "[populateArchs] Adding:" << pair.first << "=>" << pair.second;

        // SAFE usage of QVariant to avoid crash
        ui->architectureComboBox->addItem(QString(pair.first), QVariant(QString(pair.second)));
    }

    updateSelectedTarballLabel();
}



// void StageChoosePage::populateArchs()
// {
//     if (!m_config)
//         return;

//     // Populate architecture combo box
//     const auto archList = m_config->architectures();
//     for (const auto& pair : archList) {
//         ui->architectureComboBox->addItem(pair.first, QVariant(pair.second));
//     }

//     updateSelectedTarballLabel();
// }

void StageChoosePage::onArchitectureChanged(int index)
{
    if (!m_config)
        return;

    const QString archKey = ui->architectureComboBox->itemData(index).toString();
    m_config->selectArchitecture(archKey);

    // Clear and repopulate variant combo box
    ui->variantComboBox->clear();
    const auto variants = m_config->variants();
    for (const auto& pair : variants) {
        ui->variantComboBox->addItem(pair.first, QVariant(pair.second));
    }

    updateSelectedTarballLabel();
}

void StageChoosePage::onVariantChanged(int index)
{
    if (!m_config)
        return;

    const QString variantKey = ui->variantComboBox->itemData(index).toString();
    m_config->selectVariant(variantKey);
    updateSelectedTarballLabel();
}

void StageChoosePage::updateSelectedTarballLabel()
{
    if (!m_config)
        return;

    if(m_config->selectedStage3().isEmpty())
         ui->selectedTarballLabel->setText("Selected: None");
    else
        ui->selectedTarballLabel->setText("Selected: " + m_config->selectedStage3());
}
