/****************************************************************************
** Meta object code from reading C++ file 'qftp.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qtftp/qftp.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qftp.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_QFtp_t {
    QByteArrayData data[12];
    char stringdata0[126];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QFtp_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QFtp_t qt_meta_stringdata_QFtp = {
    {
QT_MOC_LITERAL(0, 0, 4), // "QFtp"
QT_MOC_LITERAL(1, 5, 12), // "stateChanged"
QT_MOC_LITERAL(2, 18, 0), // ""
QT_MOC_LITERAL(3, 19, 8), // "listInfo"
QT_MOC_LITERAL(4, 28, 8), // "QUrlInfo"
QT_MOC_LITERAL(5, 37, 9), // "readyRead"
QT_MOC_LITERAL(6, 47, 20), // "dataTransferProgress"
QT_MOC_LITERAL(7, 68, 15), // "rawCommandReply"
QT_MOC_LITERAL(8, 84, 14), // "commandStarted"
QT_MOC_LITERAL(9, 99, 15), // "commandFinished"
QT_MOC_LITERAL(10, 115, 4), // "done"
QT_MOC_LITERAL(11, 120, 5) // "abort"

    },
    "QFtp\0stateChanged\0\0listInfo\0QUrlInfo\0"
    "readyRead\0dataTransferProgress\0"
    "rawCommandReply\0commandStarted\0"
    "commandFinished\0done\0abort"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QFtp[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   59,    2, 0x06 /* Public */,
       3,    1,   62,    2, 0x06 /* Public */,
       5,    0,   65,    2, 0x06 /* Public */,
       6,    2,   66,    2, 0x06 /* Public */,
       7,    2,   71,    2, 0x06 /* Public */,
       8,    1,   76,    2, 0x06 /* Public */,
       9,    2,   79,    2, 0x06 /* Public */,
      10,    1,   84,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      11,    0,   87,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, 0x80000000 | 4,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::LongLong, QMetaType::LongLong,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    2,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,    2,    2,
    QMetaType::Void, QMetaType::Bool,    2,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void QFtp::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QFtp *_t = static_cast<QFtp *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->stateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->listInfo((*reinterpret_cast< const QUrlInfo(*)>(_a[1]))); break;
        case 2: _t->readyRead(); break;
        case 3: _t->dataTransferProgress((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< qint64(*)>(_a[2]))); break;
        case 4: _t->rawCommandReply((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 5: _t->commandStarted((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->commandFinished((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 7: _t->done((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->abort(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (QFtp::*_t)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QFtp::stateChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (QFtp::*_t)(const QUrlInfo & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QFtp::listInfo)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (QFtp::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QFtp::readyRead)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (QFtp::*_t)(qint64 , qint64 );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QFtp::dataTransferProgress)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (QFtp::*_t)(int , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QFtp::rawCommandReply)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (QFtp::*_t)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QFtp::commandStarted)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (QFtp::*_t)(int , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QFtp::commandFinished)) {
                *result = 6;
                return;
            }
        }
        {
            typedef void (QFtp::*_t)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QFtp::done)) {
                *result = 7;
                return;
            }
        }
    }
}

const QMetaObject QFtp::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QFtp.data,
      qt_meta_data_QFtp,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *QFtp::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QFtp::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_QFtp.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int QFtp::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void QFtp::stateChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QFtp::listInfo(const QUrlInfo & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QFtp::readyRead()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void QFtp::dataTransferProgress(qint64 _t1, qint64 _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QFtp::rawCommandReply(int _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QFtp::commandStarted(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void QFtp::commandFinished(int _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void QFtp::done(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
struct qt_meta_stringdata_QFtpDTP_t {
    QByteArrayData data[15];
    char stringdata0[212];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QFtpDTP_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QFtpDTP_t qt_meta_stringdata_QFtpDTP = {
    {
QT_MOC_LITERAL(0, 0, 7), // "QFtpDTP"
QT_MOC_LITERAL(1, 8, 8), // "listInfo"
QT_MOC_LITERAL(2, 17, 0), // ""
QT_MOC_LITERAL(3, 18, 8), // "QUrlInfo"
QT_MOC_LITERAL(4, 27, 9), // "readyRead"
QT_MOC_LITERAL(5, 37, 20), // "dataTransferProgress"
QT_MOC_LITERAL(6, 58, 12), // "connectState"
QT_MOC_LITERAL(7, 71, 15), // "socketConnected"
QT_MOC_LITERAL(8, 87, 15), // "socketReadyRead"
QT_MOC_LITERAL(9, 103, 11), // "socketError"
QT_MOC_LITERAL(10, 115, 28), // "QAbstractSocket::SocketError"
QT_MOC_LITERAL(11, 144, 22), // "socketConnectionClosed"
QT_MOC_LITERAL(12, 167, 18), // "socketBytesWritten"
QT_MOC_LITERAL(13, 186, 11), // "setupSocket"
QT_MOC_LITERAL(14, 198, 13) // "dataReadyRead"

    },
    "QFtpDTP\0listInfo\0\0QUrlInfo\0readyRead\0"
    "dataTransferProgress\0connectState\0"
    "socketConnected\0socketReadyRead\0"
    "socketError\0QAbstractSocket::SocketError\0"
    "socketConnectionClosed\0socketBytesWritten\0"
    "setupSocket\0dataReadyRead"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QFtpDTP[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   69,    2, 0x06 /* Public */,
       4,    0,   72,    2, 0x06 /* Public */,
       5,    2,   73,    2, 0x06 /* Public */,
       6,    1,   78,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,   81,    2, 0x08 /* Private */,
       8,    0,   82,    2, 0x08 /* Private */,
       9,    1,   83,    2, 0x08 /* Private */,
      11,    0,   86,    2, 0x08 /* Private */,
      12,    1,   87,    2, 0x08 /* Private */,
      13,    0,   90,    2, 0x08 /* Private */,
      14,    0,   91,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::LongLong, QMetaType::LongLong,    2,    2,
    QMetaType::Void, QMetaType::Int,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 10,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::LongLong,    2,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void QFtpDTP::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QFtpDTP *_t = static_cast<QFtpDTP *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->listInfo((*reinterpret_cast< const QUrlInfo(*)>(_a[1]))); break;
        case 1: _t->readyRead(); break;
        case 2: _t->dataTransferProgress((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< qint64(*)>(_a[2]))); break;
        case 3: _t->connectState((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->socketConnected(); break;
        case 5: _t->socketReadyRead(); break;
        case 6: _t->socketError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 7: _t->socketConnectionClosed(); break;
        case 8: _t->socketBytesWritten((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 9: _t->setupSocket(); break;
        case 10: _t->dataReadyRead(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 6:
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
            typedef void (QFtpDTP::*_t)(const QUrlInfo & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QFtpDTP::listInfo)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (QFtpDTP::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QFtpDTP::readyRead)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (QFtpDTP::*_t)(qint64 , qint64 );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QFtpDTP::dataTransferProgress)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (QFtpDTP::*_t)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QFtpDTP::connectState)) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject QFtpDTP::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QFtpDTP.data,
      qt_meta_data_QFtpDTP,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *QFtpDTP::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QFtpDTP::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_QFtpDTP.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int QFtpDTP::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void QFtpDTP::listInfo(const QUrlInfo & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QFtpDTP::readyRead()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void QFtpDTP::dataTransferProgress(qint64 _t1, qint64 _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QFtpDTP::connectState(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
struct qt_meta_stringdata_QFtpPI_t {
    QByteArrayData data[13];
    char stringdata0[161];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QFtpPI_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QFtpPI_t qt_meta_stringdata_QFtpPI = {
    {
QT_MOC_LITERAL(0, 0, 6), // "QFtpPI"
QT_MOC_LITERAL(1, 7, 12), // "connectState"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 8), // "finished"
QT_MOC_LITERAL(4, 30, 5), // "error"
QT_MOC_LITERAL(5, 36, 11), // "rawFtpReply"
QT_MOC_LITERAL(6, 48, 9), // "hostFound"
QT_MOC_LITERAL(7, 58, 9), // "connected"
QT_MOC_LITERAL(8, 68, 16), // "connectionClosed"
QT_MOC_LITERAL(9, 85, 20), // "delayedCloseFinished"
QT_MOC_LITERAL(10, 106, 9), // "readyRead"
QT_MOC_LITERAL(11, 116, 28), // "QAbstractSocket::SocketError"
QT_MOC_LITERAL(12, 145, 15) // "dtpConnectState"

    },
    "QFtpPI\0connectState\0\0finished\0error\0"
    "rawFtpReply\0hostFound\0connected\0"
    "connectionClosed\0delayedCloseFinished\0"
    "readyRead\0QAbstractSocket::SocketError\0"
    "dtpConnectState"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QFtpPI[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   69,    2, 0x06 /* Public */,
       3,    1,   72,    2, 0x06 /* Public */,
       4,    2,   75,    2, 0x06 /* Public */,
       5,    2,   80,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   85,    2, 0x08 /* Private */,
       7,    0,   86,    2, 0x08 /* Private */,
       8,    0,   87,    2, 0x08 /* Private */,
       9,    0,   88,    2, 0x08 /* Private */,
      10,    0,   89,    2, 0x08 /* Private */,
       4,    1,   90,    2, 0x08 /* Private */,
      12,    1,   93,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    2,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 11,    2,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void QFtpPI::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QFtpPI *_t = static_cast<QFtpPI *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->connectState((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->finished((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->error((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 3: _t->rawFtpReply((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 4: _t->hostFound(); break;
        case 5: _t->connected(); break;
        case 6: _t->connectionClosed(); break;
        case 7: _t->delayedCloseFinished(); break;
        case 8: _t->readyRead(); break;
        case 9: _t->error((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 10: _t->dtpConnectState((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 9:
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
            typedef void (QFtpPI::*_t)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QFtpPI::connectState)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (QFtpPI::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QFtpPI::finished)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (QFtpPI::*_t)(int , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QFtpPI::error)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (QFtpPI::*_t)(int , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QFtpPI::rawFtpReply)) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject QFtpPI::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QFtpPI.data,
      qt_meta_data_QFtpPI,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *QFtpPI::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QFtpPI::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_QFtpPI.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int QFtpPI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void QFtpPI::connectState(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QFtpPI::finished(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QFtpPI::error(int _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QFtpPI::rawFtpReply(int _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
