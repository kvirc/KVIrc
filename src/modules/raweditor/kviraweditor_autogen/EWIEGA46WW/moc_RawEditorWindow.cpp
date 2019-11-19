/****************************************************************************
** Meta object code from reading C++ file 'RawEditorWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../RawEditorWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RawEditorWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_RawEditorWidget_t {
    QByteArrayData data[13];
    char stringdata0[204];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RawEditorWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RawEditorWidget_t qt_meta_stringdata_RawEditorWidget = {
    {
QT_MOC_LITERAL(0, 0, 15), // "RawEditorWidget"
QT_MOC_LITERAL(1, 16, 18), // "currentItemChanged"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 16), // "QTreeWidgetItem*"
QT_MOC_LITERAL(4, 53, 2), // "it"
QT_MOC_LITERAL(5, 56, 26), // "customContextMenuRequested"
QT_MOC_LITERAL(6, 83, 3), // "pnt"
QT_MOC_LITERAL(7, 87, 27), // "toggleCurrentHandlerEnabled"
QT_MOC_LITERAL(8, 115, 20), // "removeCurrentHandler"
QT_MOC_LITERAL(9, 136, 23), // "addHandlerForCurrentRaw"
QT_MOC_LITERAL(10, 160, 6), // "addRaw"
QT_MOC_LITERAL(11, 167, 15), // "exportAllEvents"
QT_MOC_LITERAL(12, 183, 20) // "exportCurrentHandler"

    },
    "RawEditorWidget\0currentItemChanged\0\0"
    "QTreeWidgetItem*\0it\0customContextMenuRequested\0"
    "pnt\0toggleCurrentHandlerEnabled\0"
    "removeCurrentHandler\0addHandlerForCurrentRaw\0"
    "addRaw\0exportAllEvents\0exportCurrentHandler"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RawEditorWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   54,    2, 0x09 /* Protected */,
       5,    1,   59,    2, 0x09 /* Protected */,
       7,    0,   62,    2, 0x09 /* Protected */,
       8,    0,   63,    2, 0x09 /* Protected */,
       9,    0,   64,    2, 0x09 /* Protected */,
      10,    0,   65,    2, 0x09 /* Protected */,
      11,    0,   66,    2, 0x09 /* Protected */,
      12,    0,   67,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    2,
    QMetaType::Void, QMetaType::QPoint,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void RawEditorWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RawEditorWidget *_t = static_cast<RawEditorWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->currentItemChanged((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QTreeWidgetItem*(*)>(_a[2]))); break;
        case 1: _t->customContextMenuRequested((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 2: _t->toggleCurrentHandlerEnabled(); break;
        case 3: _t->removeCurrentHandler(); break;
        case 4: _t->addHandlerForCurrentRaw(); break;
        case 5: _t->addRaw(); break;
        case 6: _t->exportAllEvents(); break;
        case 7: _t->exportCurrentHandler(); break;
        default: ;
        }
    }
}

const QMetaObject RawEditorWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_RawEditorWidget.data,
      qt_meta_data_RawEditorWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *RawEditorWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RawEditorWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RawEditorWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int RawEditorWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}
struct qt_meta_stringdata_RawEditorWindow_t {
    QByteArrayData data[5];
    char stringdata0[54];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RawEditorWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RawEditorWindow_t qt_meta_stringdata_RawEditorWindow = {
    {
QT_MOC_LITERAL(0, 0, 15), // "RawEditorWindow"
QT_MOC_LITERAL(1, 16, 13), // "cancelClicked"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 9), // "okClicked"
QT_MOC_LITERAL(4, 41, 12) // "applyClicked"

    },
    "RawEditorWindow\0cancelClicked\0\0okClicked\0"
    "applyClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RawEditorWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x09 /* Protected */,
       3,    0,   30,    2, 0x09 /* Protected */,
       4,    0,   31,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void RawEditorWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RawEditorWindow *_t = static_cast<RawEditorWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->cancelClicked(); break;
        case 1: _t->okClicked(); break;
        case 2: _t->applyClicked(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject RawEditorWindow::staticMetaObject = {
    { &KviWindow::staticMetaObject, qt_meta_stringdata_RawEditorWindow.data,
      qt_meta_data_RawEditorWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *RawEditorWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RawEditorWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RawEditorWindow.stringdata0))
        return static_cast<void*>(this);
    return KviWindow::qt_metacast(_clname);
}

int RawEditorWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
