/* === This file is part of Calamares - <https://calamares.io> ===
 *
 *   SPDX-FileCopyrightText: 2014 Teo Mrnjavac <teo@kde.org>
 *   SPDX-FileCopyrightText: 2019 Adriaan de Groot <groot@kde.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 *   Calamares is Free Software: see the License-Identifier above.
 *
 */

#ifndef STAGECHOOSEPAGE_H
#define STAGECHOOSEPAGE_H

#include <QWidget>

class QComboBox;
class QLabel;
class Config;

namespace Ui {
class StageChoosePage;
}

class StageChoosePage : public QWidget
{
    Q_OBJECT

public:
    explicit StageChoosePage( Config* config, QWidget* parent = nullptr);
    ~StageChoosePage() override;

    void populateArchs();
    void setFetcherStatus(const QString& status);
    void showRestartFetcherButton(bool visible);
    void onRestartFetcherClicked();
    void whenVariantsReady(const QStringList &stages);

private slots:
    void onArchitectureChanged(int index);
    void onVariantChanged(int index);
    void updateSelectedTarballLabel();

private:
    Ui::StageChoosePage* ui;
    Config* m_config;
};

#endif // STAGECHOOSEPAGE_H

