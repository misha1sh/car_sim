#pragma once

#include "ui/draw_settings.h"

// TODO: remove
#include "ui_main_window.h"

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    inline Ui::MainWindow* getUI() {
        return ui;
    }

    inline DrawSettingsPtr getDrawSettings() {
        return draw_settings_;
    }

    template <typename OBJ, typename SLOTT>
    void SetRunTickHandler(const OBJ& obj, const SLOTT& slot) {
        connect(ui->runOneTickButton, &QPushButton::clicked, &obj, slot);
        run_tick_timer_.callOnTimeout(&obj, slot);
    }

private slots:
    void on_actionExit_triggered();
    void on_drawRoadDirCheckBox_clicked(bool value);
    void on_drawRoadIdCheckBox_clicked(bool value);
    void on_drawCarTypeCheckBox_clicked(bool value);
    void on_drawPrevCarTypeCheckBox_clicked(bool value);
    void on_drawCarDataCheckBox_clicked(bool value);
    void on_drawPrevCarDataCheckBox_clicked(bool value);
    void on_simulationSpeedSlider_valueChanged(int value);
    void on_imageQualitySlider_valueChanged(int value);
    void on_playButton_clicked();
    void on_pauseButton_clicked();

private:
    Ui::MainWindow *ui;
    DrawSettingsPtr draw_settings_;
    QTimer run_tick_timer_;
};