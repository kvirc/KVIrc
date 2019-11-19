/****************************************************************************
** Meta object code from reading C++ file 'HttpFileTransfer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../HttpFileTransfer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'HttpFileTransfer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_HttpFileTransfer_t {
    QByteArrayData data[20];
    char stringdata0[254];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_HttpFileTransfer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_HttpFileTransfer_t qt_meta_stringdata_HttpFileTransfer = {
    {
QT_MOC_LITERAL(0, 0, 16), // "HttpFileTransfer"
QT_MOC_LITERAL(1, 17, 13), // "statusMessage"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 3), // "txt"
QT_MOC_LITERAL(4, 36, 18), // "transferTerminated"
QT_MOC_LITERAL(5, 55, 8), // "bSuccess"
QT_MOC_LITERAL(6, 64, 15), // "headersReceived"
QT_MOC_LITERAL(7, 80, 44), // "KviPointerHashTable<const cha..."
QT_MOC_LITERAL(8, 125, 1), // "h"
QT_MOC_LITERAL(9, 127, 11), // "requestSent"
QT_MOC_LITERAL(10, 139, 2), // "sl"
QT_MOC_LITERAL(11, 142, 13), // "resolvingHost"
QT_MOC_LITERAL(12, 156, 8), // "hostname"
QT_MOC_LITERAL(13, 165, 14), // "contactingHost"
QT_MOC_LITERAL(14, 180, 9), // "ipandport"
QT_MOC_LITERAL(15, 190, 16), // "receivedResponse"
QT_MOC_LITERAL(16, 207, 8), // "response"
QT_MOC_LITERAL(17, 216, 21), // "connectionEstablished"
QT_MOC_LITERAL(18, 238, 5), // "abort"
QT_MOC_LITERAL(19, 244, 9) // "autoClean"

    },
    "HttpFileTransfer\0statusMessage\0\0txt\0"
    "transferTerminated\0bSuccess\0headersReceived\0"
    "KviPointerHashTable<const char*,KviCString>*\0"
    "h\0requestSent\0sl\0resolvingHost\0hostname\0"
    "contactingHost\0ipandport\0receivedResponse\0"
    "response\0connectionEstablished\0abort\0"
    "autoClean"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_HttpFileTransfer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   64,    2, 0x09 /* Protected */,
       4,    1,   67,    2, 0x09 /* Protected */,
       6,    1,   70,    2, 0x09 /* Protected */,
       9,    1,   73,    2, 0x09 /* Protected */,
      11,    1,   76,    2, 0x09 /* Protected */,
      13,    1,   79,    2, 0x09 /* Protected */,
      15,    1,   82,    2, 0x09 /* Protected */,
      17,    0,   85,    2, 0x09 /* Protected */,
      18,    0,   86,    2, 0x09 /* Protected */,
      19,    0,   87,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, QMetaType::QStringList,   10,
    QMetaType::Void, QMetaType::QString,   12,
    QMetaType::Void, QMetaType::QString,   14,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void HttpFileTransfer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        HttpFileTransfer *_t = static_cast<HttpFileTransfer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->statusMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->transferTerminated((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->headersReceived((*reinterpret_cast< KviPointerHashTable<const char*,KviCString>*(*)>(_a[1]))); break;
        case 3: _t->requestSent((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 4: _t->resolvingHost((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->contactingHost((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->receivedResponse((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->connectionEstablished(); break;
        case 8: _t->abort(); break;
        case 9: _t->autoClean(); break;
        default: ;
        }
    }
}

const QMetaObject HttpFileTransfer::staticMetaObject = {
    { &KviFileTransfer::staticMetaObject, qt_meta_stringdata_HttpFileTransfer.data,
      qt_meta_data_HttpFileTransfer,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *HttpFileTransfer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HttpFileTransfer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_HttpFileTransfer.stringdata0))
        return static_cast<void*>(this);
    return KviFileTransfer::qt_metacast(_clname);
}

int HttpFileTransfer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviFileTransfer::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
