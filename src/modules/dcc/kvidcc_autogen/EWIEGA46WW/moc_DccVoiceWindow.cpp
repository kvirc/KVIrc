/****************************************************************************
** Meta object code from reading C++ file 'DccVoiceWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../DccVoiceWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DccVoiceWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DccVoiceWindow_t {
    QByteArrayData data[11];
    char stringdata0[140];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DccVoiceWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DccVoiceWindow_t qt_meta_stringdata_DccVoiceWindow = {
    {
QT_MOC_LITERAL(0, 0, 14), // "DccVoiceWindow"
QT_MOC_LITERAL(1, 15, 18), // "handleMarshalError"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 14), // "KviError::Code"
QT_MOC_LITERAL(4, 50, 6), // "eError"
QT_MOC_LITERAL(5, 57, 9), // "connected"
QT_MOC_LITERAL(6, 67, 10), // "updateInfo"
QT_MOC_LITERAL(7, 78, 18), // "startOrStopTalking"
QT_MOC_LITERAL(8, 97, 6), // "bStart"
QT_MOC_LITERAL(9, 104, 14), // "setMixerVolume"
QT_MOC_LITERAL(10, 119, 20) // "connectionInProgress"

    },
    "DccVoiceWindow\0handleMarshalError\0\0"
    "KviError::Code\0eError\0connected\0"
    "updateInfo\0startOrStopTalking\0bStart\0"
    "setMixerVolume\0connectionInProgress"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DccVoiceWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x09 /* Protected */,
       5,    0,   47,    2, 0x09 /* Protected */,
       6,    0,   48,    2, 0x09 /* Protected */,
       7,    1,   49,    2, 0x09 /* Protected */,
       9,    1,   52,    2, 0x09 /* Protected */,
      10,    0,   55,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,

       0        // eod
};

void DccVoiceWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DccVoiceWindow *_t = static_cast<DccVoiceWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->handleMarshalError((*reinterpret_cast< KviError::Code(*)>(_a[1]))); break;
        case 1: _t->connected(); break;
        case 2: _t->updateInfo(); break;
        case 3: _t->startOrStopTalking((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->setMixerVolume((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->connectionInProgress(); break;
        default: ;
        }
    }
}

const QMetaObject DccVoiceWindow::staticMetaObject = {
    { &DccWindow::staticMetaObject, qt_meta_stringdata_DccVoiceWindow.data,
      qt_meta_data_DccVoiceWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *DccVoiceWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DccVoiceWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DccVoiceWindow.stringdata0))
        return static_cast<void*>(this);
    return DccWindow::qt_metacast(_clname);
}

int DccVoiceWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = DccWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
