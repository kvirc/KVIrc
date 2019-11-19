/****************************************************************************
** Meta object code from reading C++ file 'KvsObject_socket.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../KvsObject_socket.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KvsObject_socket.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KvsObject_socket_t {
    QByteArrayData data[13];
    char stringdata0[204];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KvsObject_socket_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KvsObject_socket_t qt_meta_stringdata_KvsObject_socket = {
    {
QT_MOC_LITERAL(0, 0, 16), // "KvsObject_socket"
QT_MOC_LITERAL(1, 17, 13), // "slotReadyRead"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 17), // "slotNewConnection"
QT_MOC_LITERAL(4, 50, 13), // "slotConnected"
QT_MOC_LITERAL(5, 64, 16), // "slotDisconnected"
QT_MOC_LITERAL(6, 81, 9), // "slotError"
QT_MOC_LITERAL(7, 91, 28), // "QAbstractSocket::SocketError"
QT_MOC_LITERAL(8, 120, 11), // "socketError"
QT_MOC_LITERAL(9, 132, 13), // "slotHostFound"
QT_MOC_LITERAL(10, 146, 16), // "slotStateChanged"
QT_MOC_LITERAL(11, 163, 28), // "QAbstractSocket::SocketState"
QT_MOC_LITERAL(12, 192, 11) // "socketState"

    },
    "KvsObject_socket\0slotReadyRead\0\0"
    "slotNewConnection\0slotConnected\0"
    "slotDisconnected\0slotError\0"
    "QAbstractSocket::SocketError\0socketError\0"
    "slotHostFound\0slotStateChanged\0"
    "QAbstractSocket::SocketState\0socketState"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KvsObject_socket[] = {

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
       3,    0,   50,    2, 0x09 /* Protected */,
       4,    0,   51,    2, 0x09 /* Protected */,
       5,    0,   52,    2, 0x09 /* Protected */,
       6,    1,   53,    2, 0x09 /* Protected */,
       9,    0,   56,    2, 0x09 /* Protected */,
      10,    1,   57,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 11,   12,

       0        // eod
};

void KvsObject_socket::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KvsObject_socket *_t = static_cast<KvsObject_socket *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->slotReadyRead(); break;
        case 1: _t->slotNewConnection(); break;
        case 2: _t->slotConnected(); break;
        case 3: _t->slotDisconnected(); break;
        case 4: _t->slotError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 5: _t->slotHostFound(); break;
        case 6: _t->slotStateChanged((*reinterpret_cast< QAbstractSocket::SocketState(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractSocket::SocketError >(); break;
            }
            break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractSocket::SocketState >(); break;
            }
            break;
        }
    }
}

const QMetaObject KvsObject_socket::staticMetaObject = {
    { &KviKvsObject::staticMetaObject, qt_meta_stringdata_KvsObject_socket.data,
      qt_meta_data_KvsObject_socket,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KvsObject_socket::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KvsObject_socket::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KvsObject_socket.stringdata0))
        return static_cast<void*>(this);
    return KviKvsObject::qt_metacast(_clname);
}

int KvsObject_socket::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviKvsObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
