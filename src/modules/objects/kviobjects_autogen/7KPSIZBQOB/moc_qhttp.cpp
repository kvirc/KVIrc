/****************************************************************************
** Meta object code from reading C++ file 'qhttp.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qthttp/qhttp.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qhttp.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_QHttp_t {
    QByteArrayData data[24];
    char stringdata0[305];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QHttp_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QHttp_t qt_meta_stringdata_QHttp = {
    {
QT_MOC_LITERAL(0, 0, 5), // "QHttp"
QT_MOC_LITERAL(1, 6, 12), // "stateChanged"
QT_MOC_LITERAL(2, 19, 0), // ""
QT_MOC_LITERAL(3, 20, 22), // "responseHeaderReceived"
QT_MOC_LITERAL(4, 43, 19), // "QHttpResponseHeader"
QT_MOC_LITERAL(5, 63, 4), // "resp"
QT_MOC_LITERAL(6, 68, 9), // "readyRead"
QT_MOC_LITERAL(7, 78, 16), // "dataSendProgress"
QT_MOC_LITERAL(8, 95, 16), // "dataReadProgress"
QT_MOC_LITERAL(9, 112, 14), // "requestStarted"
QT_MOC_LITERAL(10, 127, 15), // "requestFinished"
QT_MOC_LITERAL(11, 143, 4), // "done"
QT_MOC_LITERAL(12, 148, 27), // "proxyAuthenticationRequired"
QT_MOC_LITERAL(13, 176, 13), // "QNetworkProxy"
QT_MOC_LITERAL(14, 190, 5), // "proxy"
QT_MOC_LITERAL(15, 196, 15), // "QAuthenticator*"
QT_MOC_LITERAL(16, 212, 22), // "authenticationRequired"
QT_MOC_LITERAL(17, 235, 8), // "hostname"
QT_MOC_LITERAL(18, 244, 4), // "port"
QT_MOC_LITERAL(19, 249, 9), // "sslErrors"
QT_MOC_LITERAL(20, 259, 16), // "QList<QSslError>"
QT_MOC_LITERAL(21, 276, 6), // "errors"
QT_MOC_LITERAL(22, 283, 5), // "abort"
QT_MOC_LITERAL(23, 289, 15) // "ignoreSslErrors"

    },
    "QHttp\0stateChanged\0\0responseHeaderReceived\0"
    "QHttpResponseHeader\0resp\0readyRead\0"
    "dataSendProgress\0dataReadProgress\0"
    "requestStarted\0requestFinished\0done\0"
    "proxyAuthenticationRequired\0QNetworkProxy\0"
    "proxy\0QAuthenticator*\0authenticationRequired\0"
    "hostname\0port\0sslErrors\0QList<QSslError>\0"
    "errors\0abort\0ignoreSslErrors"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QHttp[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      11,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   79,    2, 0x06 /* Public */,
       3,    1,   82,    2, 0x06 /* Public */,
       6,    1,   85,    2, 0x06 /* Public */,
       7,    2,   88,    2, 0x06 /* Public */,
       8,    2,   93,    2, 0x06 /* Public */,
       9,    1,   98,    2, 0x06 /* Public */,
      10,    2,  101,    2, 0x06 /* Public */,
      11,    1,  106,    2, 0x06 /* Public */,
      12,    2,  109,    2, 0x06 /* Public */,
      16,    3,  114,    2, 0x06 /* Public */,
      19,    1,  121,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      22,    0,  124,    2, 0x0a /* Public */,
      23,    0,  125,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,    2,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, 0x80000000 | 13, 0x80000000 | 15,   14,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::UShort, 0x80000000 | 15,   17,   18,    2,
    QMetaType::Void, 0x80000000 | 20,   21,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void QHttp::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QHttp *_t = static_cast<QHttp *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->stateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->responseHeaderReceived((*reinterpret_cast< const QHttpResponseHeader(*)>(_a[1]))); break;
        case 2: _t->readyRead((*reinterpret_cast< const QHttpResponseHeader(*)>(_a[1]))); break;
        case 3: _t->dataSendProgress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->dataReadProgress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: _t->requestStarted((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->requestFinished((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 7: _t->done((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->proxyAuthenticationRequired((*reinterpret_cast< const QNetworkProxy(*)>(_a[1])),(*reinterpret_cast< QAuthenticator*(*)>(_a[2]))); break;
        case 9: _t->authenticationRequired((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< quint16(*)>(_a[2])),(*reinterpret_cast< QAuthenticator*(*)>(_a[3]))); break;
        case 10: _t->sslErrors((*reinterpret_cast< const QList<QSslError>(*)>(_a[1]))); break;
        case 11: _t->abort(); break;
        case 12: _t->ignoreSslErrors(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (QHttp::*_t)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QHttp::stateChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (QHttp::*_t)(const QHttpResponseHeader & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QHttp::responseHeaderReceived)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (QHttp::*_t)(const QHttpResponseHeader & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QHttp::readyRead)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (QHttp::*_t)(int , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QHttp::dataSendProgress)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (QHttp::*_t)(int , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QHttp::dataReadProgress)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (QHttp::*_t)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QHttp::requestStarted)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (QHttp::*_t)(int , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QHttp::requestFinished)) {
                *result = 6;
                return;
            }
        }
        {
            typedef void (QHttp::*_t)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QHttp::done)) {
                *result = 7;
                return;
            }
        }
        {
            typedef void (QHttp::*_t)(const QNetworkProxy & , QAuthenticator * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QHttp::proxyAuthenticationRequired)) {
                *result = 8;
                return;
            }
        }
        {
            typedef void (QHttp::*_t)(const QString & , quint16 , QAuthenticator * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QHttp::authenticationRequired)) {
                *result = 9;
                return;
            }
        }
        {
            typedef void (QHttp::*_t)(const QList<QSslError> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QHttp::sslErrors)) {
                *result = 10;
                return;
            }
        }
    }
}

const QMetaObject QHttp::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QHttp.data,
      qt_meta_data_QHttp,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *QHttp::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QHttp::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_QHttp.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int QHttp::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void QHttp::stateChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QHttp::responseHeaderReceived(const QHttpResponseHeader & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QHttp::readyRead(const QHttpResponseHeader & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QHttp::dataSendProgress(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QHttp::dataReadProgress(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QHttp::requestStarted(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void QHttp::requestFinished(int _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void QHttp::done(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void QHttp::proxyAuthenticationRequired(const QNetworkProxy & _t1, QAuthenticator * _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void QHttp::authenticationRequired(const QString & _t1, quint16 _t2, QAuthenticator * _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void QHttp::sslErrors(const QList<QSslError> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
