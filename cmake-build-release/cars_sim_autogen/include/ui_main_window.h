/********************************************************************************
** Form generated from reading UI file 'main_window.ui'
**
** Created by: Qt User Interface Compiler version 6.2.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAIN_WINDOW_H
#define UI_MAIN_WINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>
#include <ui/map/glmapwidget.h>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    GLMapWidget *openGLWidget;
    QWidget *widget;
    QCheckBox *drawRoadDirCheckBox;
    QCheckBox *drawDecision1CheckBox;
    QCheckBox *drawDecision2CheckBox;
    QCheckBox *drawCarTypeCheckBox;
    QCheckBox *drawPrevCarTypeCheckBox;
    QCheckBox *drawCarDataCheckBox;
    QCheckBox *drawPrevCarDataCheckBox;
    QPushButton *runOneTickButton;
    QSlider *simulationSpeedSlider;
    QLabel *label;
    QPushButton *playButton;
    QPushButton *pauseButton;
    QSlider *imageQualitySlider;
    QLabel *label_2;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1573, 897);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        openGLWidget = new GLMapWidget(centralwidget);
        openGLWidget->setObjectName(QString::fromUtf8("openGLWidget"));
        openGLWidget->setGeometry(QRect(210, 0, 1341, 861));
        widget = new QWidget(centralwidget);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(0, 0, 201, 601));
        drawRoadDirCheckBox = new QCheckBox(widget);
        drawRoadDirCheckBox->setObjectName(QString::fromUtf8("drawRoadDirCheckBox"));
        drawRoadDirCheckBox->setGeometry(QRect(10, 10, 85, 20));
        drawRoadDirCheckBox->setChecked(true);
        drawDecision1CheckBox = new QCheckBox(widget);
        drawDecision1CheckBox->setObjectName(QString::fromUtf8("drawDecision1CheckBox"));
        drawDecision1CheckBox->setGeometry(QRect(10, 30, 85, 20));
        drawDecision1CheckBox->setChecked(true);
        drawDecision2CheckBox = new QCheckBox(widget);
        drawDecision2CheckBox->setObjectName(QString::fromUtf8("drawDecision2CheckBox"));
        drawDecision2CheckBox->setGeometry(QRect(10, 50, 85, 20));
        drawDecision2CheckBox->setChecked(true);
        drawCarTypeCheckBox = new QCheckBox(widget);
        drawCarTypeCheckBox->setObjectName(QString::fromUtf8("drawCarTypeCheckBox"));
        drawCarTypeCheckBox->setGeometry(QRect(10, 110, 85, 20));
        drawCarTypeCheckBox->setChecked(true);
        drawPrevCarTypeCheckBox = new QCheckBox(widget);
        drawPrevCarTypeCheckBox->setObjectName(QString::fromUtf8("drawPrevCarTypeCheckBox"));
        drawPrevCarTypeCheckBox->setGeometry(QRect(10, 130, 121, 20));
        drawPrevCarTypeCheckBox->setChecked(false);
        drawCarDataCheckBox = new QCheckBox(widget);
        drawCarDataCheckBox->setObjectName(QString::fromUtf8("drawCarDataCheckBox"));
        drawCarDataCheckBox->setGeometry(QRect(10, 70, 121, 20));
        drawCarDataCheckBox->setChecked(true);
        drawPrevCarDataCheckBox = new QCheckBox(widget);
        drawPrevCarDataCheckBox->setObjectName(QString::fromUtf8("drawPrevCarDataCheckBox"));
        drawPrevCarDataCheckBox->setGeometry(QRect(10, 90, 121, 20));
        drawPrevCarDataCheckBox->setChecked(false);
        runOneTickButton = new QPushButton(widget);
        runOneTickButton->setObjectName(QString::fromUtf8("runOneTickButton"));
        runOneTickButton->setGeometry(QRect(10, 260, 80, 22));
        simulationSpeedSlider = new QSlider(widget);
        simulationSpeedSlider->setObjectName(QString::fromUtf8("simulationSpeedSlider"));
        simulationSpeedSlider->setGeometry(QRect(10, 310, 160, 21));
        simulationSpeedSlider->setMinimum(1);
        simulationSpeedSlider->setMaximum(150);
        simulationSpeedSlider->setValue(50);
        simulationSpeedSlider->setOrientation(Qt::Horizontal);
        simulationSpeedSlider->setTickPosition(QSlider::TicksBelow);
        label = new QLabel(widget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 290, 111, 16));
        playButton = new QPushButton(widget);
        playButton->setObjectName(QString::fromUtf8("playButton"));
        playButton->setGeometry(QRect(10, 340, 61, 22));
        pauseButton = new QPushButton(widget);
        pauseButton->setObjectName(QString::fromUtf8("pauseButton"));
        pauseButton->setGeometry(QRect(90, 340, 61, 22));
        imageQualitySlider = new QSlider(widget);
        imageQualitySlider->setObjectName(QString::fromUtf8("imageQualitySlider"));
        imageQualitySlider->setGeometry(QRect(10, 410, 160, 16));
        imageQualitySlider->setMinimum(1);
        imageQualitySlider->setMaximum(100);
        imageQualitySlider->setValue(50);
        imageQualitySlider->setOrientation(Qt::Horizontal);
        imageQualitySlider->setTickPosition(QSlider::TicksBelow);
        label_2 = new QLabel(widget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 390, 91, 16));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1573, 19));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "CarsSim", nullptr));
        drawRoadDirCheckBox->setText(QCoreApplication::translate("MainWindow", "road dir", nullptr));
        drawDecision1CheckBox->setText(QCoreApplication::translate("MainWindow", "decision 1", nullptr));
        drawDecision2CheckBox->setText(QCoreApplication::translate("MainWindow", "decision 2", nullptr));
        drawCarTypeCheckBox->setText(QCoreApplication::translate("MainWindow", "car_type", nullptr));
        drawPrevCarTypeCheckBox->setText(QCoreApplication::translate("MainWindow", "prev_car_type", nullptr));
        drawCarDataCheckBox->setText(QCoreApplication::translate("MainWindow", "car_data", nullptr));
        drawPrevCarDataCheckBox->setText(QCoreApplication::translate("MainWindow", "prev_car_data", nullptr));
        runOneTickButton->setText(QCoreApplication::translate("MainWindow", "Run one tick", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "simulation speed", nullptr));
        playButton->setText(QCoreApplication::translate("MainWindow", "|> Play", nullptr));
        pauseButton->setText(QCoreApplication::translate("MainWindow", "|| Pause", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "image quality:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAIN_WINDOW_H
