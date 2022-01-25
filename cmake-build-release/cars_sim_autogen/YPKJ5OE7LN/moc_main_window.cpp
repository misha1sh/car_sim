/****************************************************************************
** Meta object code from reading C++ file 'main_window.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/ui/main_window.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'main_window.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    const uint offsetsAndSize[30];
    char stringdata0[389];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 10), // "MainWindow"
QT_MOC_LITERAL(11, 23), // "on_actionExit_triggered"
QT_MOC_LITERAL(35, 0), // ""
QT_MOC_LITERAL(36, 30), // "on_drawRoadDirCheckBox_clicked"
QT_MOC_LITERAL(67, 5), // "value"
QT_MOC_LITERAL(73, 32), // "on_drawDecision1CheckBox_clicked"
QT_MOC_LITERAL(106, 32), // "on_drawDecision2CheckBox_clicked"
QT_MOC_LITERAL(139, 30), // "on_drawCarTypeCheckBox_clicked"
QT_MOC_LITERAL(170, 34), // "on_drawPrevCarTypeCheckBox_cl..."
QT_MOC_LITERAL(205, 30), // "on_drawCarDataCheckBox_clicked"
QT_MOC_LITERAL(236, 34), // "on_drawPrevCarDataCheckBox_cl..."
QT_MOC_LITERAL(271, 37), // "on_simulationSpeedSlider_valu..."
QT_MOC_LITERAL(309, 34), // "on_imageQualitySlider_valueCh..."
QT_MOC_LITERAL(344, 21), // "on_playButton_clicked"
QT_MOC_LITERAL(366, 22) // "on_pauseButton_clicked"

    },
    "MainWindow\0on_actionExit_triggered\0\0"
    "on_drawRoadDirCheckBox_clicked\0value\0"
    "on_drawDecision1CheckBox_clicked\0"
    "on_drawDecision2CheckBox_clicked\0"
    "on_drawCarTypeCheckBox_clicked\0"
    "on_drawPrevCarTypeCheckBox_clicked\0"
    "on_drawCarDataCheckBox_clicked\0"
    "on_drawPrevCarDataCheckBox_clicked\0"
    "on_simulationSpeedSlider_valueChanged\0"
    "on_imageQualitySlider_valueChanged\0"
    "on_playButton_clicked\0on_pauseButton_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   86,    2, 0x08,    1 /* Private */,
       3,    1,   87,    2, 0x08,    2 /* Private */,
       5,    1,   90,    2, 0x08,    4 /* Private */,
       6,    1,   93,    2, 0x08,    6 /* Private */,
       7,    1,   96,    2, 0x08,    8 /* Private */,
       8,    1,   99,    2, 0x08,   10 /* Private */,
       9,    1,  102,    2, 0x08,   12 /* Private */,
      10,    1,  105,    2, 0x08,   14 /* Private */,
      11,    1,  108,    2, 0x08,   16 /* Private */,
      12,    1,  111,    2, 0x08,   18 /* Private */,
      13,    0,  114,    2, 0x08,   20 /* Private */,
      14,    0,  115,    2, 0x08,   21 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    4,
    QMetaType::Void, QMetaType::Bool,    4,
    QMetaType::Void, QMetaType::Bool,    4,
    QMetaType::Void, QMetaType::Bool,    4,
    QMetaType::Void, QMetaType::Bool,    4,
    QMetaType::Void, QMetaType::Bool,    4,
    QMetaType::Void, QMetaType::Bool,    4,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->on_actionExit_triggered(); break;
        case 1: _t->on_drawRoadDirCheckBox_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->on_drawDecision1CheckBox_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->on_drawDecision2CheckBox_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->on_drawCarTypeCheckBox_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->on_drawPrevCarTypeCheckBox_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->on_drawCarDataCheckBox_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->on_drawPrevCarDataCheckBox_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->on_simulationSpeedSlider_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->on_imageQualitySlider_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->on_playButton_clicked(); break;
        case 11: _t->on_pauseButton_clicked(); break;
        default: ;
        }
    }
}

const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.offsetsAndSize,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_MainWindow_t
, QtPrivate::TypeAndForceComplete<MainWindow, std::true_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>


>,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 12;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
