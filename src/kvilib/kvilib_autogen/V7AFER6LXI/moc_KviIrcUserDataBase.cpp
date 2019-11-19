/****************************************************************************
** Meta object code from reading C++ file 'KviIrcUserDataBase.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../irc/KviIrcUserDataBase.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KviIrcUserDataBase.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviIrcUserDataBase_t {
    QByteArrayData data[6];
    char stringdata0[95];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviIrcUserDataBase_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviIrcUserDataBase_t qt_meta_stringdata_KviIrcUserDataBase = {
    {
QT_MOC_LITERAL(0, 0, 18), // "KviIrcUserDataBase"
QT_MOC_LITERAL(1, 19, 21), // "registeredUserChanged"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 6), // "szUser"
QT_MOC_LITERAL(4, 49, 19), // "registeredUserAdded"
QT_MOC_LITERAL(5, 69, 25) // "registeredDatabaseCleared"

    },
    "KviIrcUserDataBase\0registeredUserChanged\0"
    "\0szUser\0registeredUserAdded\0"
    "registeredDatabaseCleared"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviIrcUserDataBase[] = {

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
       1,    1,   29,    2, 0x09 /* Protected */,
       4,    1,   32,    2, 0x09 /* Protected */,
       5,    0,   35,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,

       0        // eod
};

void KviIrcUserDataBase::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviIrcUserDataBase *_t = static_cast<KviIrcUserDataBase *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->registeredUserChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->registeredUserAdded((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->registeredDatabaseCleared(); break;
        default: ;
        }
    }
}

const QMetaObject KviIrcUserDataBase::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_KviIrcUserDataBase.data,
      qt_meta_data_KviIrcUserDataBase,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviIrcUserDataBase::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviIrcUserDataBase::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviIrcUserDataBase.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int KviIrcUserDataBase::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
