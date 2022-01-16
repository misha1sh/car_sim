#include "main_window.h"
#include "ui_main_window.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    draw_settings_(std::make_shared<DrawSettings>())
{
    ui->setupUi(this);
    setAnimated(true);

    connect(ui->drawRoadDirCheckBox, SIGNAL(clicked(bool)), this, SLOT(on_drawRoadDirCheckBox_clicked(bool)));
    connect(ui->drawDecision1CheckBox, SIGNAL(clicked(bool)), this, SLOT(on_drawDecision1CheckBox_clicked(bool)));
    connect(ui->drawDecision2CheckBox, SIGNAL(clicked(bool)), this, SLOT(on_drawDecision2CheckBox_clicked(bool)));
    connect(ui->drawCarTypeCheckBox, SIGNAL(clicked(bool)), this, SLOT(on_drawCarTypeCheckBox_clicked(bool)));
    connect(ui->drawPrevCarTypeCheckBox, SIGNAL(clicked(bool)), this, SLOT(on_drawPrevCarTypeCheckBox_clicked(bool)));
    connect(ui->drawCarDataCheckBox, SIGNAL(clicked(bool)), this, SLOT(on_drawCarDataCheckBox_clicked(bool)));
    connect(ui->drawPrevCarDataCheckBox, SIGNAL(clicked(bool)), this, SLOT(on_drawPrevCarDataCheckBox_clicked(bool)));
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

void MainWindow::on_drawDecision1CheckBox_clicked(bool value) {
    draw_settings_->draw_decision1 = value;
}

void MainWindow::on_drawDecision2CheckBox_clicked(bool value) {
    draw_settings_->draw_decision2 = value;
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

