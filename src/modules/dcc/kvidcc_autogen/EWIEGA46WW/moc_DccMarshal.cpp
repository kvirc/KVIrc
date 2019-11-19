/****************************************************************************
** Meta object code from reading C++ file 'DccMarshal.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../DccMarshal.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DccMarshal.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DccMarshal_t {
    QByteArrayData data[15];
    char stringdata0[165];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DccMarshal_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DccMarshal_t qt_meta_stringdata_DccMarshal = {
    {
QT_MOC_LITERAL(0, 0, 10), // "DccMarshal"
QT_MOC_LITERAL(1, 11, 20), // "startingSSLHandshake"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 8), // "sslError"
QT_MOC_LITERAL(4, 42, 11), // "const char*"
QT_MOC_LITERAL(5, 54, 3), // "msg"
QT_MOC_LITERAL(6, 58, 9), // "connected"
QT_MOC_LITERAL(7, 68, 10), // "inProgress"
QT_MOC_LITERAL(8, 79, 5), // "error"
QT_MOC_LITERAL(9, 85, 14), // "KviError::Code"
QT_MOC_LITERAL(10, 100, 14), // "doSSLHandshake"
QT_MOC_LITERAL(11, 115, 11), // "snActivated"
QT_MOC_LITERAL(12, 127, 18), // "connectionTimedOut"
QT_MOC_LITERAL(13, 146, 8), // "doListen"
QT_MOC_LITERAL(14, 155, 9) // "doConnect"

    },
    "DccMarshal\0startingSSLHandshake\0\0"
    "sslError\0const char*\0msg\0connected\0"
    "inProgress\0error\0KviError::Code\0"
    "doSSLHandshake\0snActivated\0"
    "connectionTimedOut\0doListen\0doConnect"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DccMarshal[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x06 /* Public */,
       3,    1,   65,    2, 0x06 /* Public */,
       6,    0,   68,    2, 0x06 /* Public */,
       7,    0,   69,    2, 0x06 /* Public */,
       8,    1,   70,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    1,   73,    2, 0x08 /* Private */,
      11,    1,   76,    2, 0x08 /* Private */,
      12,    0,   79,    2, 0x08 /* Private */,
      13,    0,   80,    2, 0x08 /* Private */,
      14,    0,   81,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 9,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void DccMarshal::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DccMarshal *_t = static_cast<DccMarshal *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->startingSSLHandshake(); break;
        case 1: _t->sslError((*reinterpret_cast< const char*(*)>(_a[1]))); break;
        case 2: _t->connected(); break;
        case 3: _t->inProgress(); break;
        case 4: _t->error((*reinterpret_cast< KviError::Code(*)>(_a[1]))); break;
        case 5: _t->doSSLHandshake((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->snActivated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->connectionTimedOut(); break;
        case 8: _t->doListen(); break;
        case 9: _t->doConnect(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (DccMarshal::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DccMarshal::startingSSLHandshake)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (DccMarshal::*_t)(const char * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DccMarshal::sslError)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (DccMarshal::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DccMarshal::connected)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (DccMarshal::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DccMarshal::inProgress)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (DccMarshal::*_t)(KviError::Code );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DccMarshal::error)) {
                *result = 4;
                return;
            }
        }
    }
}

const QMetaObject DccMarshal::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_DccMarshal.data,
      qt_meta_data_DccMarshal,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *DccMarshal::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DccMarshal::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DccMarshal.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int DccMarshal::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void DccMarshal::startingSSLHandshake()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void DccMarshal::sslError(const char * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void DccMarshal::connected()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void DccMarshal::inProgress()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void DccMarshal::error(KviError::Code _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
