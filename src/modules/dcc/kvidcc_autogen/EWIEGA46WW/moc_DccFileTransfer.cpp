/****************************************************************************
** Meta object code from reading C++ file 'DccFileTransfer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../DccFileTransfer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DccFileTransfer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DccFileTransferBandwidthDialog_t {
    QByteArrayData data[4];
    char stringdata0[56];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DccFileTransferBandwidthDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DccFileTransferBandwidthDialog_t qt_meta_stringdata_DccFileTransferBandwidthDialog = {
    {
QT_MOC_LITERAL(0, 0, 30), // "DccFileTransferBandwidthDialog"
QT_MOC_LITERAL(1, 31, 9), // "okClicked"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 13) // "cancelClicked"

    },
    "DccFileTransferBandwidthDialog\0okClicked\0"
    "\0cancelClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DccFileTransferBandwidthDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x09 /* Protected */,
       3,    0,   25,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void DccFileTransferBandwidthDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DccFileTransferBandwidthDialog *_t = static_cast<DccFileTransferBandwidthDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->okClicked(); break;
        case 1: _t->cancelClicked(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject DccFileTransferBandwidthDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_DccFileTransferBandwidthDialog.data,
      qt_meta_data_DccFileTransferBandwidthDialog,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *DccFileTransferBandwidthDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DccFileTransferBandwidthDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DccFileTransferBandwidthDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int DccFileTransferBandwidthDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}
struct qt_meta_stringdata_DccFileTransfer_t {
    QByteArrayData data[17];
    char stringdata0[224];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DccFileTransfer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DccFileTransfer_t qt_meta_stringdata_DccFileTransfer = {
    {
QT_MOC_LITERAL(0, 0, 15), // "DccFileTransfer"
QT_MOC_LITERAL(1, 16, 20), // "connectionInProgress"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 8), // "sslError"
QT_MOC_LITERAL(4, 47, 11), // "const char*"
QT_MOC_LITERAL(5, 59, 3), // "msg"
QT_MOC_LITERAL(6, 63, 20), // "startingSSLHandshake"
QT_MOC_LITERAL(7, 84, 18), // "handleMarshalError"
QT_MOC_LITERAL(8, 103, 14), // "KviError::Code"
QT_MOC_LITERAL(9, 118, 9), // "connected"
QT_MOC_LITERAL(10, 128, 24), // "bandwidthDialogDestroyed"
QT_MOC_LITERAL(11, 153, 18), // "configureBandwidth"
QT_MOC_LITERAL(12, 172, 14), // "resumeTimedOut"
QT_MOC_LITERAL(13, 187, 5), // "abort"
QT_MOC_LITERAL(14, 193, 8), // "retryDCC"
QT_MOC_LITERAL(15, 202, 9), // "retryTDCC"
QT_MOC_LITERAL(16, 212, 11) // "retryRevDCC"

    },
    "DccFileTransfer\0connectionInProgress\0"
    "\0sslError\0const char*\0msg\0"
    "startingSSLHandshake\0handleMarshalError\0"
    "KviError::Code\0connected\0"
    "bandwidthDialogDestroyed\0configureBandwidth\0"
    "resumeTimedOut\0abort\0retryDCC\0retryTDCC\0"
    "retryRevDCC"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DccFileTransfer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   74,    2, 0x09 /* Protected */,
       3,    1,   75,    2, 0x09 /* Protected */,
       6,    0,   78,    2, 0x09 /* Protected */,
       7,    1,   79,    2, 0x09 /* Protected */,
       9,    0,   82,    2, 0x09 /* Protected */,
      10,    0,   83,    2, 0x09 /* Protected */,
      11,    0,   84,    2, 0x09 /* Protected */,
      12,    0,   85,    2, 0x09 /* Protected */,
      13,    0,   86,    2, 0x0a /* Public */,
      14,    0,   87,    2, 0x0a /* Public */,
      15,    0,   88,    2, 0x0a /* Public */,
      16,    0,   89,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void DccFileTransfer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DccFileTransfer *_t = static_cast<DccFileTransfer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->connectionInProgress(); break;
        case 1: _t->sslError((*reinterpret_cast< const char*(*)>(_a[1]))); break;
        case 2: _t->startingSSLHandshake(); break;
        case 3: _t->handleMarshalError((*reinterpret_cast< KviError::Code(*)>(_a[1]))); break;
        case 4: _t->connected(); break;
        case 5: _t->bandwidthDialogDestroyed(); break;
        case 6: _t->configureBandwidth(); break;
        case 7: _t->resumeTimedOut(); break;
        case 8: _t->abort(); break;
        case 9: _t->retryDCC(); break;
        case 10: _t->retryTDCC(); break;
        case 11: _t->retryRevDCC(); break;
        default: ;
        }
    }
}

const QMetaObject DccFileTransfer::staticMetaObject = {
    { &KviFileTransfer::staticMetaObject, qt_meta_stringdata_DccFileTransfer.data,
      qt_meta_data_DccFileTransfer,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *DccFileTransfer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DccFileTransfer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DccFileTransfer.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "DccMarshalOutputContext"))
        return static_cast< DccMarshalOutputContext*>(this);
    return KviFileTransfer::qt_metacast(_clname);
}

int DccFileTransfer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviFileTransfer::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
