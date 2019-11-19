/****************************************************************************
** Meta object code from reading C++ file 'KvsObject_http.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../KvsObject_http.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KvsObject_http.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KvsObject_http_t {
    QByteArrayData data[19];
    char stringdata0[243];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KvsObject_http_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KvsObject_http_t qt_meta_stringdata_KvsObject_http = {
    {
QT_MOC_LITERAL(0, 0, 14), // "KvsObject_http"
QT_MOC_LITERAL(1, 15, 19), // "slotRequestFinished"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 2), // "id"
QT_MOC_LITERAL(4, 39, 5), // "error"
QT_MOC_LITERAL(5, 45, 8), // "slotDone"
QT_MOC_LITERAL(6, 54, 18), // "slotRequestStarted"
QT_MOC_LITERAL(7, 73, 20), // "slotDataSendProgress"
QT_MOC_LITERAL(8, 94, 4), // "done"
QT_MOC_LITERAL(9, 99, 5), // "total"
QT_MOC_LITERAL(10, 105, 20), // "slotDataReadProgress"
QT_MOC_LITERAL(11, 126, 13), // "slotSslErrors"
QT_MOC_LITERAL(12, 140, 16), // "QList<QSslError>"
QT_MOC_LITERAL(13, 157, 26), // "slotResponseHeaderReceived"
QT_MOC_LITERAL(14, 184, 19), // "QHttpResponseHeader"
QT_MOC_LITERAL(15, 204, 1), // "r"
QT_MOC_LITERAL(16, 206, 13), // "slotReadyRead"
QT_MOC_LITERAL(17, 220, 16), // "slotStateChanged"
QT_MOC_LITERAL(18, 237, 5) // "state"

    },
    "KvsObject_http\0slotRequestFinished\0\0"
    "id\0error\0slotDone\0slotRequestStarted\0"
    "slotDataSendProgress\0done\0total\0"
    "slotDataReadProgress\0slotSslErrors\0"
    "QList<QSslError>\0slotResponseHeaderReceived\0"
    "QHttpResponseHeader\0r\0slotReadyRead\0"
    "slotStateChanged\0state"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KvsObject_http[] = {

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
       5,    1,   64,    2, 0x09 /* Protected */,
       6,    1,   67,    2, 0x09 /* Protected */,
       7,    2,   70,    2, 0x09 /* Protected */,
      10,    2,   75,    2, 0x09 /* Protected */,
      11,    1,   80,    2, 0x09 /* Protected */,
      13,    1,   83,    2, 0x09 /* Protected */,
      16,    1,   86,    2, 0x09 /* Protected */,
      17,    1,   89,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,    3,    4,
    QMetaType::Void, QMetaType::Bool,    4,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    8,    9,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    8,    9,
    QMetaType::Void, 0x80000000 | 12,    2,
    QMetaType::Void, 0x80000000 | 14,   15,
    QMetaType::Void, 0x80000000 | 14,   15,
    QMetaType::Void, QMetaType::Int,   18,

       0        // eod
};

void KvsObject_http::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KvsObject_http *_t = static_cast<KvsObject_http *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->slotRequestFinished((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 1: _t->slotDone((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->slotRequestStarted((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->slotDataSendProgress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->slotDataReadProgress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: _t->slotSslErrors((*reinterpret_cast< QList<QSslError>(*)>(_a[1]))); break;
        case 6: _t->slotResponseHeaderReceived((*reinterpret_cast< const QHttpResponseHeader(*)>(_a[1]))); break;
        case 7: _t->slotReadyRead((*reinterpret_cast< const QHttpResponseHeader(*)>(_a[1]))); break;
        case 8: _t->slotStateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<QSslError> >(); break;
            }
            break;
        }
    }
}

const QMetaObject KvsObject_http::staticMetaObject = {
    { &KviKvsObject::staticMetaObject, qt_meta_stringdata_KvsObject_http.data,
      qt_meta_data_KvsObject_http,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KvsObject_http::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KvsObject_http::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KvsObject_http.stringdata0))
        return static_cast<void*>(this);
    return KviKvsObject::qt_metacast(_clname);
}

int KvsObject_http::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviKvsObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
