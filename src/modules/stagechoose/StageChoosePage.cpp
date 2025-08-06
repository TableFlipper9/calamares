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
#include <QTimer>

StageChoosePage::StageChoosePage(Config* config, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::StageChoosePage)
    , m_config(config)
{
    ui->setupUi(this);

    connect(ui->architectureComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &StageChoosePage::onArchitectureChanged);
    connect(ui->variantComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &StageChoosePage::onVariantChanged);

    populateArchs();
}

void StageChoosePage::populateArchs()
{
    if (!m_config)
        return;

    // // Populate architecture combo box
    // const auto archList = m_config->architectures();
    // for (const auto& pair : archList) {
    //     ui->architectureComboBox->addItem(pair.first, QVariant(pair.second));
    // }

    QStringList archs = m_config->availableArchitectures();
    for(const QString& arch : archs){
        ui->architectureComboBox->addItem(arch,arch);
    }

    updateSelectedTarballLabel();
}

void StageChoosePage::onArchitectureChanged(int index)
{
    if (!m_config)
        return;

    // const QString archKey = ui->architectureComboBox->itemData(index).toString();
    // m_config->selectArchitecture(archKey);

    // // Clear and repopulate variant combo box
    // ui->variantComboBox->clear();
    // const auto variants = m_config->variants();
    // for (const auto& pair : variants) {
    //     ui->variantComboBox->addItem(pair.first, QVariant(pair.second));
    // }

    const QString archKey = ui->architectureComboBox->itemData(index).toString();
    m_config->selectArchitecture(archKey);

    ui->variantComboBox->clear();

    QStringList stages = m_config->availableStagesFor(archKey);
    for(const QString& stage : stages){
        ui->variantComboBox->addItem( stage, stage);
    }

    if(!stages.isEmpty()){
        ui->variantComboBox->setCurrentIndex(0);
        onVariantChanged(0);
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

StageChoosePage::~StageChoosePage()
{
    delete ui;
}
