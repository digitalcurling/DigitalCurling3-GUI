#include "preference_dialog.hpp"
#include "ui_preference_dialog.h"

#include <QSettings>

#include "settings.hpp"

namespace digitalcurling3_gui {

PreferenceDialog::PreferenceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferenceDialog)
{
    ui->setupUi(this);

    {
        // 設定を読み込む
        Settings settings;
        settings.load();
        ui->animationSpeedSpinBox->setValue(settings.animation_speed);
        ui->showGameResultCheckBox->setChecked(settings.show_game_result);
    }

    connect(this, &QDialog::accepted, this, [this] {
        // 設定を書き込む
        Settings settings;
        settings.animation_speed = ui->animationSpeedSpinBox->value();
        settings.show_game_result = ui->showGameResultCheckBox->isChecked();
        settings.save();
    });
}


PreferenceDialog::~PreferenceDialog()
{
    delete ui;
}

} // namespace digitalcurling3_gui
