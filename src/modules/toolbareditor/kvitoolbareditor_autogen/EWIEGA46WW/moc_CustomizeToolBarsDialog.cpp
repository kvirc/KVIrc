/****************************************************************************
** Meta object code from reading C++ file 'CustomizeToolBarsDialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../CustomizeToolBarsDialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CustomizeToolBarsDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CustomizeToolBarsDialog_t {
    QByteArrayData data[10];
    char stringdata0[134];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CustomizeToolBarsDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CustomizeToolBarsDialog_t qt_meta_stringdata_CustomizeToolBarsDialog = {
    {
QT_MOC_LITERAL(0, 0, 23), // "CustomizeToolBarsDialog"
QT_MOC_LITERAL(1, 24, 12), // "closeClicked"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 10), // "newToolBar"
QT_MOC_LITERAL(4, 49, 13), // "deleteToolBar"
QT_MOC_LITERAL(5, 63, 13), // "renameToolBar"
QT_MOC_LITERAL(6, 77, 13), // "exportToolBar"
QT_MOC_LITERAL(7, 91, 13), // "importToolBar"
QT_MOC_LITERAL(8, 105, 21), // "currentToolBarChanged"
QT_MOC_LITERAL(9, 127, 6) // "reject"

    },
    "CustomizeToolBarsDialog\0closeClicked\0"
    "\0newToolBar\0deleteToolBar\0renameToolBar\0"
    "exportToolBar\0importToolBar\0"
    "currentToolBarChanged\0reject"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CustomizeToolBarsDialog[] = {

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
       1,    0,   54,    2, 0x09 /* Protected */,
       3,    0,   55,    2, 0x09 /* Protected */,
       4,    0,   56,    2, 0x09 /* Protected */,
       5,    0,   57,    2, 0x09 /* Protected */,
       6,    0,   58,    2, 0x09 /* Protected */,
       7,    0,   59,    2, 0x09 /* Protected */,
       8,    0,   60,    2, 0x09 /* Protected */,
       9,    0,   61,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CustomizeToolBarsDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CustomizeToolBarsDialog *_t = static_cast<CustomizeToolBarsDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->closeClicked(); break;
        case 1: _t->newToolBar(); break;
        case 2: _t->deleteToolBar(); break;
        case 3: _t->renameToolBar(); break;
        case 4: _t->exportToolBar(); break;
        case 5: _t->importToolBar(); break;
        case 6: _t->currentToolBarChanged(); break;
        case 7: _t->reject(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject CustomizeToolBarsDialog::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CustomizeToolBarsDialog.data,
      qt_meta_data_CustomizeToolBarsDialog,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *CustomizeToolBarsDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CustomizeToolBarsDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CustomizeToolBarsDialog.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int CustomizeToolBarsDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
struct qt_meta_stringdata_CustomToolBarPropertiesDialog_t {
    QByteArrayData data[7];
    char stringdata0[94];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CustomToolBarPropertiesDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CustomToolBarPropertiesDialog_t qt_meta_stringdata_CustomToolBarPropertiesDialog = {
    {
QT_MOC_LITERAL(0, 0, 29), // "CustomToolBarPropertiesDialog"
QT_MOC_LITERAL(1, 30, 9), // "okClicked"
QT_MOC_LITERAL(2, 40, 0), // ""
QT_MOC_LITERAL(3, 41, 15), // "advancedClicked"
QT_MOC_LITERAL(4, 57, 17), // "iconButtonClicked"
QT_MOC_LITERAL(5, 75, 16), // "labelTextChanged"
QT_MOC_LITERAL(6, 92, 1) // "s"

    },
    "CustomToolBarPropertiesDialog\0okClicked\0"
    "\0advancedClicked\0iconButtonClicked\0"
    "labelTextChanged\0s"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CustomToolBarPropertiesDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x09 /* Protected */,
       3,    0,   35,    2, 0x09 /* Protected */,
       4,    0,   36,    2, 0x09 /* Protected */,
       5,    1,   37,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    6,

       0        // eod
};

void CustomToolBarPropertiesDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CustomToolBarPropertiesDialog *_t = static_cast<CustomToolBarPropertiesDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->okClicked(); break;
        case 1: _t->advancedClicked(); break;
        case 2: _t->iconButtonClicked(); break;
        case 3: _t->labelTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject CustomToolBarPropertiesDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_CustomToolBarPropertiesDialog.data,
      qt_meta_data_CustomToolBarPropertiesDialog,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *CustomToolBarPropertiesDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CustomToolBarPropertiesDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CustomToolBarPropertiesDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int CustomToolBarPropertiesDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
struct qt_meta_stringdata_TrashcanLabel_t {
    QByteArrayData data[4];
    char stringdata0[31];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TrashcanLabel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TrashcanLabel_t qt_meta_stringdata_TrashcanLabel = {
    {
QT_MOC_LITERAL(0, 0, 13), // "TrashcanLabel"
QT_MOC_LITERAL(1, 14, 5), // "flash"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 9) // "heartbeat"

    },
    "TrashcanLabel\0flash\0\0heartbeat"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TrashcanLabel[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x0a /* Public */,
       3,    0,   25,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void TrashcanLabel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TrashcanLabel *_t = static_cast<TrashcanLabel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->flash(); break;
        case 1: _t->heartbeat(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject TrashcanLabel::staticMetaObject = {
    { &QLabel::staticMetaObject, qt_meta_stringdata_TrashcanLabel.data,
      qt_meta_data_TrashcanLabel,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *TrashcanLabel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TrashcanLabel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TrashcanLabel.stringdata0))
        return static_cast<void*>(this);
    return QLabel::qt_metacast(_clname);
}

int TrashcanLabel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLabel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
