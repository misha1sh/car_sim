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

private slots:
    void on_actionExit_triggered();
    void on_drawRoadDirCheckBox_clicked(bool value);
    void on_drawDecision1CheckBox_clicked(bool value);
    void on_drawDecision2CheckBox_clicked(bool value);
    void on_drawCarTypeCheckBox_clicked(bool value);
    void on_drawPrevCarTypeCheckBox_clicked(bool value);
    void on_drawCarDataCheckBox_clicked(bool value);
    void on_drawPrevCarDataCheckBox_clicked(bool value);

private:
    Ui::MainWindow *ui;
    DrawSettingsPtr draw_settings_;
};