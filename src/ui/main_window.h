#pragma once

#include <QMainWindow>
#include "ui_main_window.h"

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

private slots:
    void on_actionExit_triggered();

private:
    Ui::MainWindow *ui;
};