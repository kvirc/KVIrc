/****************************************************************************
** Meta object code from reading C++ file 'RegistrationWizard.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../RegistrationWizard.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RegistrationWizard.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_RegistrationWizard_t {
    QByteArrayData data[7];
    char stringdata0[89];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RegistrationWizard_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RegistrationWizard_t qt_meta_stringdata_RegistrationWizard = {
    {
QT_MOC_LITERAL(0, 0, 18), // "RegistrationWizard"
QT_MOC_LITERAL(1, 19, 15), // "realNameChanged"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 3), // "str"
QT_MOC_LITERAL(4, 40, 11), // "maskChanged"
QT_MOC_LITERAL(5, 52, 17), // "notifyNickChanged"
QT_MOC_LITERAL(6, 70, 18) // "notifyCheckToggled"

    },
    "RegistrationWizard\0realNameChanged\0\0"
    "str\0maskChanged\0notifyNickChanged\0"
    "notifyCheckToggled"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RegistrationWizard[] = {

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
       1,    1,   34,    2, 0x09 /* Protected */,
       4,    1,   37,    2, 0x09 /* Protected */,
       5,    1,   40,    2, 0x09 /* Protected */,
       6,    1,   43,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Bool,    2,

       0        // eod
};

void RegistrationWizard::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RegistrationWizard *_t = static_cast<RegistrationWizard *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->realNameChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->maskChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->notifyNickChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->notifyCheckToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject RegistrationWizard::staticMetaObject = {
    { &KviTalWizard::staticMetaObject, qt_meta_stringdata_RegistrationWizard.data,
      qt_meta_data_RegistrationWizard,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *RegistrationWizard::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RegistrationWizard::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RegistrationWizard.stringdata0))
        return static_cast<void*>(this);
    return KviTalWizard::qt_metacast(_clname);
}

int RegistrationWizard::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviTalWizard::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
