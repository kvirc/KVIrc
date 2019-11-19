/****************************************************************************
** Meta object code from reading C++ file 'KvsObject_ftp.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../KvsObject_ftp.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KvsObject_ftp.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KvsObject_ftp_t {
    QByteArrayData data[19];
    char stringdata0[206];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KvsObject_ftp_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KvsObject_ftp_t qt_meta_stringdata_KvsObject_ftp = {
    {
QT_MOC_LITERAL(0, 0, 13), // "KvsObject_ftp"
QT_MOC_LITERAL(1, 14, 19), // "slotCommandFinished"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 2), // "id"
QT_MOC_LITERAL(4, 38, 5), // "error"
QT_MOC_LITERAL(5, 44, 18), // "slotCommandStarted"
QT_MOC_LITERAL(6, 63, 24), // "slotDataTransferProgress"
QT_MOC_LITERAL(7, 88, 4), // "done"
QT_MOC_LITERAL(8, 93, 5), // "total"
QT_MOC_LITERAL(9, 99, 8), // "slotDone"
QT_MOC_LITERAL(10, 108, 12), // "slotListInfo"
QT_MOC_LITERAL(11, 121, 8), // "QUrlInfo"
QT_MOC_LITERAL(12, 130, 1), // "i"
QT_MOC_LITERAL(13, 132, 19), // "slotRawCommandReply"
QT_MOC_LITERAL(14, 152, 9), // "replyCode"
QT_MOC_LITERAL(15, 162, 6), // "detail"
QT_MOC_LITERAL(16, 169, 13), // "slotReadyRead"
QT_MOC_LITERAL(17, 183, 16), // "slotStateChanged"
QT_MOC_LITERAL(18, 200, 5) // "state"

    },
    "KvsObject_ftp\0slotCommandFinished\0\0"
    "id\0error\0slotCommandStarted\0"
    "slotDataTransferProgress\0done\0total\0"
    "slotDone\0slotListInfo\0QUrlInfo\0i\0"
    "slotRawCommandReply\0replyCode\0detail\0"
    "slotReadyRead\0slotStateChanged\0state"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KvsObject_ftp[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   54,    2, 0x09 /* Protected */,
       5,    1,   59,    2, 0x09 /* Protected */,
       6,    2,   62,    2, 0x09 /* Protected */,
       9,    1,   67,    2, 0x09 /* Protected */,
      10,    1,   70,    2, 0x09 /* Protected */,
      13,    2,   73,    2, 0x09 /* Protected */,
      16,    0,   78,    2, 0x09 /* Protected */,
      17,    1,   79,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,    3,    4,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::LongLong, QMetaType::LongLong,    7,    8,
    QMetaType::Void, QMetaType::Bool,    4,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   14,   15,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   18,

       0        // eod
};

void KvsObject_ftp::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KvsObject_ftp *_t = static_cast<KvsObject_ftp *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->slotCommandFinished((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 1: _t->slotCommandStarted((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->slotDataTransferProgress((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< qint64(*)>(_a[2]))); break;
        case 3: _t->slotDone((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->slotListInfo((*reinterpret_cast< const QUrlInfo(*)>(_a[1]))); break;
        case 5: _t->slotRawCommandReply((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 6: _t->slotReadyRead(); break;
        case 7: _t->slotStateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject KvsObject_ftp::staticMetaObject = {
    { &KviKvsObject::staticMetaObject, qt_meta_stringdata_KvsObject_ftp.data,
      qt_meta_data_KvsObject_ftp,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KvsObject_ftp::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KvsObject_ftp::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KvsObject_ftp.stringdata0))
        return static_cast<void*>(this);
    return KviKvsObject::qt_metacast(_clname);
}

int KvsObject_ftp::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviKvsObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
