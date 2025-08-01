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
#include <QPushButton>

StageChoosePage::StageChoosePage(Config* config, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::StageChoosePage)
    , m_config(config)
{
    ui->setupUi(this);

    connect(ui->architectureComboBox, QOverload<int>::of(&QComboBox::activated),
            this, &StageChoosePage::onArchitectureChanged);
    connect(ui->variantComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &StageChoosePage::onVariantChanged);

    connect(ui->mirrorLineEdit, &QLineEdit::editingFinished, this, &StageChoosePage::onMirrorChanged);
    connect(ui->restartFetcherButton, &QPushButton::clicked, this, &StageChoosePage::onRestartFetcherClicked);

    if(m_config){
        connect(m_config, &Config::fetchStatusChanged,this,&StageChoosePage::setFetcherStatus);
        connect(m_config, &Config::fetchError,this,[this](const QString& error){setFetcherStatus("Error" + error);showRestartFetcherButton(true);});
        connect(m_config, &Config::variantsReady, this, &StageChoosePage::whenVariantsReady);
        connect(m_config, &Config::tarballReady, this, [this](const QString&){updateSelectedTarballLabel();});
    }

    setFetcherStatus("Idle");
    updateSelectedTarballLabel();
    showRestartFetcherButton(false);

    populateArchs();
}

void StageChoosePage::onMirrorChanged()
{
    if(!m_config) return;
    QString mirror = ui->mirrorLineEdit->text().trimmed();
    m_config->setMirrorBase(mirror);
}

void StageChoosePage::setFetcherStatus(const QString& status)
{
    ui->fetcherStatusLabel->setText("Status: " + status);
}

void StageChoosePage::showRestartFetcherButton(bool visible)
{
    ui->restartFetcherButton->setVisible(false);
    // To implement
}

void StageChoosePage::onRestartFetcherClicked(){
    // Logic here
    setFetcherStatus("Restarting...");
    showRestartFetcherButton(false);
}

void StageChoosePage::populateArchs()
{
    if (!m_config)
        return;

    const auto archs = m_config->availableArchitecturesInfo();
    ui->architectureComboBox->clear();
    for(const auto& arch : archs){
        ui->architectureComboBox->addItem(arch.description,arch.name);
    }
    ui->architectureComboBox->setCurrentIndex(-1);
}

void StageChoosePage::onArchitectureChanged(int index)
{
    if (!m_config)
        return;

    const QString archKey = ui->architectureComboBox->itemData(index).toString();
    ui->variantComboBox->clear();

    m_config->availableStagesFor(archKey);

    if(archKey == "livecd"){
        ui->variantComboBox->setVisible(false);
        ui->variantLabel->setVisible(false);

        // setFetcherStatus("LiveCD mode");
        // m_config->updateTarball("livecd");
        showRestartFetcherButton(false);
        return;
    }
    else{
        ui->variantComboBox->setVisible(true);
        ui->variantLabel->setVisible(true);
    }
}

void StageChoosePage::onVariantChanged(int index)
{
    if (!m_config)
        return;

    const QString variantKey = ui->variantComboBox->itemData(index).toString();
    m_config->selectVariant(variantKey);
}

void StageChoosePage::whenVariantsReady(const QStringList &stages)
{
    ui->variantComboBox->clear();

    for(const QString& stage : stages){
        ui->variantComboBox->addItem(stage, stage);
    }

    if(!stages.isEmpty()){
        ui->variantComboBox->setCurrentIndex(0);
        onVariantChanged(0);
    }
}

void StageChoosePage::updateSelectedTarballLabel()
{
    if (!m_config)
        return;

    ui->selectedTarballLabel->setText("Selected: " + m_config->selectedStage3());
}

StageChoosePage::~StageChoosePage()
{
    delete ui;
}
