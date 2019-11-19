/****************************************************************************
** Meta object code from reading C++ file 'DccBroker.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../DccBroker.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DccBroker.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DccBroker_t {
    QByteArrayData data[17];
    char stringdata0[229];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DccBroker_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DccBroker_t qt_meta_stringdata_DccBroker = {
    {
QT_MOC_LITERAL(0, 0, 9), // "DccBroker"
QT_MOC_LITERAL(1, 10, 12), // "rsendExecute"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 10), // "DccDialog*"
QT_MOC_LITERAL(4, 35, 3), // "box"
QT_MOC_LITERAL(5, 39, 14), // "DccDescriptor*"
QT_MOC_LITERAL(6, 54, 3), // "dcc"
QT_MOC_LITERAL(7, 58, 11), // "executeChat"
QT_MOC_LITERAL(8, 70, 19), // "activeCanvasExecute"
QT_MOC_LITERAL(9, 90, 18), // "activeVoiceExecute"
QT_MOC_LITERAL(10, 109, 18), // "activeVideoExecute"
QT_MOC_LITERAL(11, 128, 15), // "sendFileExecute"
QT_MOC_LITERAL(12, 144, 15), // "recvFileExecute"
QT_MOC_LITERAL(13, 160, 18), // "chooseSaveFileName"
QT_MOC_LITERAL(14, 179, 21), // "renameOverwriteResume"
QT_MOC_LITERAL(15, 201, 17), // "renameDccSendFile"
QT_MOC_LITERAL(16, 219, 9) // "cancelDcc"

    },
    "DccBroker\0rsendExecute\0\0DccDialog*\0"
    "box\0DccDescriptor*\0dcc\0executeChat\0"
    "activeCanvasExecute\0activeVoiceExecute\0"
    "activeVideoExecute\0sendFileExecute\0"
    "recvFileExecute\0chooseSaveFileName\0"
    "renameOverwriteResume\0renameDccSendFile\0"
    "cancelDcc"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DccBroker[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   79,    2, 0x0a /* Public */,
       1,    1,   84,    2, 0x0a /* Public */,
       7,    2,   87,    2, 0x0a /* Public */,
       8,    2,   92,    2, 0x0a /* Public */,
       9,    2,   97,    2, 0x0a /* Public */,
      10,    2,  102,    2, 0x0a /* Public */,
      11,    2,  107,    2, 0x0a /* Public */,
      12,    2,  112,    2, 0x0a /* Public */,
      13,    2,  117,    2, 0x0a /* Public */,
      14,    2,  122,    2, 0x0a /* Public */,
      15,    2,  127,    2, 0x0a /* Public */,
      16,    2,  132,    2, 0x0a /* Public */,
      16,    1,  137,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, 0x80000000 | 5,    6,

       0        // eod
};

void DccBroker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DccBroker *_t = static_cast<DccBroker *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->rsendExecute((*reinterpret_cast< DccDialog*(*)>(_a[1])),(*reinterpret_cast< DccDescriptor*(*)>(_a[2]))); break;
        case 1: _t->rsendExecute((*reinterpret_cast< DccDescriptor*(*)>(_a[1]))); break;
        case 2: _t->executeChat((*reinterpret_cast< DccDialog*(*)>(_a[1])),(*reinterpret_cast< DccDescriptor*(*)>(_a[2]))); break;
        case 3: _t->activeCanvasExecute((*reinterpret_cast< DccDialog*(*)>(_a[1])),(*reinterpret_cast< DccDescriptor*(*)>(_a[2]))); break;
        case 4: _t->activeVoiceExecute((*reinterpret_cast< DccDialog*(*)>(_a[1])),(*reinterpret_cast< DccDescriptor*(*)>(_a[2]))); break;
        case 5: _t->activeVideoExecute((*reinterpret_cast< DccDialog*(*)>(_a[1])),(*reinterpret_cast< DccDescriptor*(*)>(_a[2]))); break;
        case 6: _t->sendFileExecute((*reinterpret_cast< DccDialog*(*)>(_a[1])),(*reinterpret_cast< DccDescriptor*(*)>(_a[2]))); break;
        case 7: _t->recvFileExecute((*reinterpret_cast< DccDialog*(*)>(_a[1])),(*reinterpret_cast< DccDescriptor*(*)>(_a[2]))); break;
        case 8: _t->chooseSaveFileName((*reinterpret_cast< DccDialog*(*)>(_a[1])),(*reinterpret_cast< DccDescriptor*(*)>(_a[2]))); break;
        case 9: _t->renameOverwriteResume((*reinterpret_cast< DccDialog*(*)>(_a[1])),(*reinterpret_cast< DccDescriptor*(*)>(_a[2]))); break;
        case 10: _t->renameDccSendFile((*reinterpret_cast< DccDialog*(*)>(_a[1])),(*reinterpret_cast< DccDescriptor*(*)>(_a[2]))); break;
        case 11: _t->cancelDcc((*reinterpret_cast< DccDialog*(*)>(_a[1])),(*reinterpret_cast< DccDescriptor*(*)>(_a[2]))); break;
        case 12: _t->cancelDcc((*reinterpret_cast< DccDescriptor*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject DccBroker::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_DccBroker.data,
      qt_meta_data_DccBroker,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *DccBroker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DccBroker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DccBroker.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int DccBroker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
