/****************************************************************************
** Meta object code from reading C++ file 'KviRegisteredUserDataBase.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ext/KviRegisteredUserDataBase.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KviRegisteredUserDataBase.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviRegisteredUserDataBase_t {
    QByteArrayData data[6];
    char stringdata0[77];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviRegisteredUserDataBase_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviRegisteredUserDataBase_t qt_meta_stringdata_KviRegisteredUserDataBase = {
    {
QT_MOC_LITERAL(0, 0, 25), // "KviRegisteredUserDataBase"
QT_MOC_LITERAL(1, 26, 11), // "userRemoved"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 11), // "userChanged"
QT_MOC_LITERAL(4, 51, 9), // "userAdded"
QT_MOC_LITERAL(5, 61, 15) // "databaseCleared"

    },
    "KviRegisteredUserDataBase\0userRemoved\0"
    "\0userChanged\0userAdded\0databaseCleared"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviRegisteredUserDataBase[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       3,    1,   37,    2, 0x06 /* Public */,
       4,    1,   40,    2, 0x06 /* Public */,
       5,    0,   43,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,

       0        // eod
};

void KviRegisteredUserDataBase::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviRegisteredUserDataBase *_t = static_cast<KviRegisteredUserDataBase *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->userRemoved((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->userChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->userAdded((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->databaseCleared(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (KviRegisteredUserDataBase::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviRegisteredUserDataBase::userRemoved)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (KviRegisteredUserDataBase::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviRegisteredUserDataBase::userChanged)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (KviRegisteredUserDataBase::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviRegisteredUserDataBase::userAdded)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (KviRegisteredUserDataBase::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviRegisteredUserDataBase::databaseCleared)) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject KviRegisteredUserDataBase::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_KviRegisteredUserDataBase.data,
      qt_meta_data_KviRegisteredUserDataBase,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviRegisteredUserDataBase::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviRegisteredUserDataBase::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviRegisteredUserDataBase.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int KviRegisteredUserDataBase::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void KviRegisteredUserDataBase::userRemoved(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void KviRegisteredUserDataBase::userChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void KviRegisteredUserDataBase::userAdded(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void KviRegisteredUserDataBase::databaseCleared()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
