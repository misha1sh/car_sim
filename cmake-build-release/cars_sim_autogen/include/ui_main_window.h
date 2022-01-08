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
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
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
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1042, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        openGLWidget = new GLMapWidget(centralwidget);
        openGLWidget->setObjectName(QString::fromUtf8("openGLWidget"));
        openGLWidget->setGeometry(QRect(10, 20, 761, 521));
        widget = new QWidget(centralwidget);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(780, 20, 201, 401));
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
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1042, 19));
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
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAIN_WINDOW_H
