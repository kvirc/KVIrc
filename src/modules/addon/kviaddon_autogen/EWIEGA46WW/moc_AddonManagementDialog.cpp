/****************************************************************************
** Meta object code from reading C++ file 'AddonManagementDialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../AddonManagementDialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AddonManagementDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_AddonManagementDialog_t {
    QByteArrayData data[13];
    char stringdata0[164];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AddonManagementDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AddonManagementDialog_t qt_meta_stringdata_AddonManagementDialog = {
    {
QT_MOC_LITERAL(0, 0, 21), // "AddonManagementDialog"
QT_MOC_LITERAL(1, 22, 14), // "currentChanged"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(4, 55, 1), // "i"
QT_MOC_LITERAL(5, 57, 12), // "closeClicked"
QT_MOC_LITERAL(6, 70, 15), // "configureScript"
QT_MOC_LITERAL(7, 86, 14), // "showScriptHelp"
QT_MOC_LITERAL(8, 101, 10), // "packScript"
QT_MOC_LITERAL(9, 112, 15), // "uninstallScript"
QT_MOC_LITERAL(10, 128, 14), // "getMoreScripts"
QT_MOC_LITERAL(11, 143, 13), // "installScript"
QT_MOC_LITERAL(12, 157, 6) // "reject"

    },
    "AddonManagementDialog\0currentChanged\0"
    "\0QListWidgetItem*\0i\0closeClicked\0"
    "configureScript\0showScriptHelp\0"
    "packScript\0uninstallScript\0getMoreScripts\0"
    "installScript\0reject"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AddonManagementDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   59,    2, 0x09 /* Protected */,
       5,    0,   64,    2, 0x09 /* Protected */,
       6,    0,   65,    2, 0x09 /* Protected */,
       7,    0,   66,    2, 0x09 /* Protected */,
       8,    0,   67,    2, 0x09 /* Protected */,
       9,    0,   68,    2, 0x09 /* Protected */,
      10,    0,   69,    2, 0x09 /* Protected */,
      11,    0,   70,    2, 0x09 /* Protected */,
      12,    0,   71,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    2,
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

void AddonManagementDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AddonManagementDialog *_t = static_cast<AddonManagementDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->currentChanged((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        case 1: _t->closeClicked(); break;
        case 2: _t->configureScript(); break;
        case 3: _t->showScriptHelp(); break;
        case 4: _t->packScript(); break;
        case 5: _t->uninstallScript(); break;
        case 6: _t->getMoreScripts(); break;
        case 7: _t->installScript(); break;
        case 8: _t->reject(); break;
        default: ;
        }
    }
}

const QMetaObject AddonManagementDialog::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_AddonManagementDialog.data,
      qt_meta_data_AddonManagementDialog,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *AddonManagementDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AddonManagementDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AddonManagementDialog.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int AddonManagementDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
