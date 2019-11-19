/****************************************************************************
** Meta object code from reading C++ file 'KviIrcSocket.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../kernel/KviIrcSocket.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KviIrcSocket.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviIrcSocket_t {
    QByteArrayData data[9];
    char stringdata0[128];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviIrcSocket_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviIrcSocket_t qt_meta_stringdata_KviIrcSocket = {
    {
QT_MOC_LITERAL(0, 0, 12), // "KviIrcSocket"
QT_MOC_LITERAL(1, 13, 18), // "connectionTimedOut"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 18), // "writeNotifierFired"
QT_MOC_LITERAL(4, 52, 8), // "readData"
QT_MOC_LITERAL(5, 61, 13), // "readProxyData"
QT_MOC_LITERAL(6, 75, 22), // "readHttpProxyErrorData"
QT_MOC_LITERAL(7, 98, 14), // "flushSendQueue"
QT_MOC_LITERAL(8, 113, 14) // "doSSLHandshake"

    },
    "KviIrcSocket\0connectionTimedOut\0\0"
    "writeNotifierFired\0readData\0readProxyData\0"
    "readHttpProxyErrorData\0flushSendQueue\0"
    "doSSLHandshake"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviIrcSocket[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x09 /* Protected */,
       3,    1,   50,    2, 0x09 /* Protected */,
       4,    1,   53,    2, 0x09 /* Protected */,
       5,    1,   56,    2, 0x09 /* Protected */,
       6,    1,   59,    2, 0x09 /* Protected */,
       7,    0,   62,    2, 0x09 /* Protected */,
       8,    1,   63,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void KviIrcSocket::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviIrcSocket *_t = static_cast<KviIrcSocket *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->connectionTimedOut(); break;
        case 1: _t->writeNotifierFired((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->readData((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->readProxyData((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->readHttpProxyErrorData((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->flushSendQueue(); break;
        case 6: _t->doSSLHandshake((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject KviIrcSocket::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_KviIrcSocket.data,
      qt_meta_data_KviIrcSocket,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviIrcSocket::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviIrcSocket::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviIrcSocket.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int KviIrcSocket::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
