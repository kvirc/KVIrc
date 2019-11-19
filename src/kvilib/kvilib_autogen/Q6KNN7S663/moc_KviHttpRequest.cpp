/****************************************************************************
** Meta object code from reading C++ file 'KviHttpRequest.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../net/KviHttpRequest.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KviHttpRequest.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviHttpRequest_t {
    QByteArrayData data[30];
    char stringdata0[434];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviHttpRequest_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviHttpRequest_t qt_meta_stringdata_KviHttpRequest = {
    {
QT_MOC_LITERAL(0, 0, 14), // "KviHttpRequest"
QT_MOC_LITERAL(1, 15, 13), // "resolvingHost"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 8), // "hostname"
QT_MOC_LITERAL(4, 39, 14), // "contactingHost"
QT_MOC_LITERAL(5, 54, 9), // "ipandport"
QT_MOC_LITERAL(6, 64, 21), // "connectionEstablished"
QT_MOC_LITERAL(7, 86, 16), // "receivedResponse"
QT_MOC_LITERAL(8, 103, 8), // "response"
QT_MOC_LITERAL(9, 112, 10), // "terminated"
QT_MOC_LITERAL(10, 123, 8), // "bSuccess"
QT_MOC_LITERAL(11, 132, 6), // "status"
QT_MOC_LITERAL(12, 139, 7), // "message"
QT_MOC_LITERAL(13, 147, 4), // "data"
QT_MOC_LITERAL(14, 152, 10), // "KviCString"
QT_MOC_LITERAL(15, 163, 10), // "binaryData"
QT_MOC_LITERAL(16, 174, 13), // "KviDataBuffer"
QT_MOC_LITERAL(17, 188, 6), // "header"
QT_MOC_LITERAL(18, 195, 44), // "KviPointerHashTable<const cha..."
QT_MOC_LITERAL(19, 240, 3), // "hdr"
QT_MOC_LITERAL(20, 244, 11), // "requestSent"
QT_MOC_LITERAL(21, 256, 7), // "request"
QT_MOC_LITERAL(22, 264, 23), // "slotSocketReadDataReady"
QT_MOC_LITERAL(23, 288, 22), // "slotSocketDisconnected"
QT_MOC_LITERAL(24, 311, 19), // "slotSocketConnected"
QT_MOC_LITERAL(25, 331, 15), // "slotSocketError"
QT_MOC_LITERAL(26, 347, 28), // "QAbstractSocket::SocketError"
QT_MOC_LITERAL(27, 376, 11), // "socketError"
QT_MOC_LITERAL(28, 388, 22), // "slotConnectionTimedOut"
QT_MOC_LITERAL(29, 411, 22) // "slotSocketHostResolved"

    },
    "KviHttpRequest\0resolvingHost\0\0hostname\0"
    "contactingHost\0ipandport\0connectionEstablished\0"
    "receivedResponse\0response\0terminated\0"
    "bSuccess\0status\0message\0data\0KviCString\0"
    "binaryData\0KviDataBuffer\0header\0"
    "KviPointerHashTable<const char*,KviCString>*\0"
    "hdr\0requestSent\0request\0slotSocketReadDataReady\0"
    "slotSocketDisconnected\0slotSocketConnected\0"
    "slotSocketError\0QAbstractSocket::SocketError\0"
    "socketError\0slotConnectionTimedOut\0"
    "slotSocketHostResolved"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviHttpRequest[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      10,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   94,    2, 0x06 /* Public */,
       4,    1,   97,    2, 0x06 /* Public */,
       6,    0,  100,    2, 0x06 /* Public */,
       7,    1,  101,    2, 0x06 /* Public */,
       9,    1,  104,    2, 0x06 /* Public */,
      11,    1,  107,    2, 0x06 /* Public */,
      13,    1,  110,    2, 0x06 /* Public */,
      15,    1,  113,    2, 0x06 /* Public */,
      17,    1,  116,    2, 0x06 /* Public */,
      20,    1,  119,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      22,    0,  122,    2, 0x08 /* Private */,
      23,    0,  123,    2, 0x08 /* Private */,
      24,    0,  124,    2, 0x08 /* Private */,
      25,    1,  125,    2, 0x08 /* Private */,
      28,    0,  128,    2, 0x08 /* Private */,
      29,    0,  129,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, QMetaType::Bool,   10,
    QMetaType::Void, QMetaType::QString,   12,
    QMetaType::Void, 0x80000000 | 14,   13,
    QMetaType::Void, 0x80000000 | 16,   13,
    QMetaType::Void, 0x80000000 | 18,   19,
    QMetaType::Void, QMetaType::QStringList,   21,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 26,   27,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void KviHttpRequest::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviHttpRequest *_t = static_cast<KviHttpRequest *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->resolvingHost((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->contactingHost((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->connectionEstablished(); break;
        case 3: _t->receivedResponse((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->terminated((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->status((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->data((*reinterpret_cast< const KviCString(*)>(_a[1]))); break;
        case 7: _t->binaryData((*reinterpret_cast< const KviDataBuffer(*)>(_a[1]))); break;
        case 8: _t->header((*reinterpret_cast< KviPointerHashTable<const char*,KviCString>*(*)>(_a[1]))); break;
        case 9: _t->requestSent((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 10: _t->slotSocketReadDataReady(); break;
        case 11: _t->slotSocketDisconnected(); break;
        case 12: _t->slotSocketConnected(); break;
        case 13: _t->slotSocketError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 14: _t->slotConnectionTimedOut(); break;
        case 15: _t->slotSocketHostResolved(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 13:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (KviHttpRequest::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviHttpRequest::resolvingHost)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (KviHttpRequest::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviHttpRequest::contactingHost)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (KviHttpRequest::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviHttpRequest::connectionEstablished)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (KviHttpRequest::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviHttpRequest::receivedResponse)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (KviHttpRequest::*_t)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviHttpRequest::terminated)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (KviHttpRequest::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviHttpRequest::status)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (KviHttpRequest::*_t)(const KviCString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviHttpRequest::data)) {
                *result = 6;
                return;
            }
        }
        {
            typedef void (KviHttpRequest::*_t)(const KviDataBuffer & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviHttpRequest::binaryData)) {
                *result = 7;
                return;
            }
        }
        {
            typedef void (KviHttpRequest::*_t)(KviPointerHashTable<const char*,KviCString> * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviHttpRequest::header)) {
                *result = 8;
                return;
            }
        }
        {
            typedef void (KviHttpRequest::*_t)(const QStringList & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviHttpRequest::requestSent)) {
                *result = 9;
                return;
            }
        }
    }
}

const QMetaObject KviHttpRequest::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_KviHttpRequest.data,
      qt_meta_data_KviHttpRequest,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviHttpRequest::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviHttpRequest::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviHttpRequest.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "KviHeapObject"))
        return static_cast< KviHeapObject*>(this);
    return QObject::qt_metacast(_clname);
}

int KviHttpRequest::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void KviHttpRequest::resolvingHost(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void KviHttpRequest::contactingHost(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void KviHttpRequest::connectionEstablished()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void KviHttpRequest::receivedResponse(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void KviHttpRequest::terminated(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void KviHttpRequest::status(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void KviHttpRequest::data(const KviCString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void KviHttpRequest::binaryData(const KviDataBuffer & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void KviHttpRequest::header(KviPointerHashTable<const char*,KviCString> * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void KviHttpRequest::requestSent(const QStringList & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
