#include "main_window.h"
#include "ui_main_window.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    draw_settings_(std::make_shared<DrawSettings>()),
    run_tick_timer_(this)
{
    ui->setupUi(this);
    setAnimated(true);

    connect(ui->drawRoadDirCheckBox, SIGNAL(clicked(bool)), this, SLOT(on_drawRoadDirCheckBox_clicked(bool)));
    connect(ui->drawRoadIdCheckBox, SIGNAL(clicked(bool)), this, SLOT(on_drawRoadIdCheckBox_clicked(bool)));
    connect(ui->drawCarTypeCheckBox, SIGNAL(clicked(bool)), this, SLOT(on_drawCarTypeCheckBox_clicked(bool)));
    connect(ui->drawPrevCarTypeCheckBox, SIGNAL(clicked(bool)), this, SLOT(on_drawPrevCarTypeCheckBox_clicked(bool)));
    connect(ui->drawCarDataCheckBox, SIGNAL(clicked(bool)), this, SLOT(on_drawCarDataCheckBox_clicked(bool)));
    connect(ui->drawPrevCarDataCheckBox, SIGNAL(clicked(bool)), this, SLOT(on_drawPrevCarDataCheckBox_clicked(bool)));
    connect(ui->simulationSpeedSlider, SIGNAL(valueChanged(int)), this, SLOT(on_simulationSpeedSlider_valueChanged(int)));
    connect(ui->imageQualitySlider, SIGNAL(valueChanged(int)), this, SLOT(on_imageQualitySlider_valueChanged(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}

void MainWindow::on_drawRoadDirCheckBox_clicked(bool value) {
    draw_settings_->draw_road_dir = value;
}

void MainWindow::on_drawRoadIdCheckBox_clicked(bool value) {
    draw_settings_->draw_road_id = value;
}

void MainWindow::on_drawCarTypeCheckBox_clicked(bool value) {
    draw_settings_->draw_car_type = value;
}

void MainWindow::on_drawPrevCarTypeCheckBox_clicked(bool value) {
    draw_settings_->draw_prev_car_type = value;
}

void MainWindow::on_drawCarDataCheckBox_clicked(bool value) {
    draw_settings_->draw_car_data = value;
}

void MainWindow::on_drawPrevCarDataCheckBox_clicked(bool value) {
    draw_settings_->draw_prev_car_data = value;
}

void MainWindow::on_imageQualitySlider_valueChanged(int value) {
    draw_settings_->resoultion_coef = (100 - value) / 100. * 6;
    draw_settings_->resoultion_coef = std::max(1., draw_settings_->resoultion_coef);
}

void MainWindow::on_simulationSpeedSlider_valueChanged(int value) {
    run_tick_timer_.setInterval(1500 / value);
}

void MainWindow::on_playButton_clicked() {
    if (!run_tick_timer_.isActive()) {
        run_tick_timer_.start(run_tick_timer_.interval());
    }
}

void MainWindow::on_pauseButton_clicked() {
    if (run_tick_timer_.isActive()) {
        run_tick_timer_.stop();
    }
}