/****************************************************************************
** Meta object code from reading C++ file 'KviIrcConnection.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../kernel/KviIrcConnection.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KviIrcConnection.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviIrcConnection_t {
    QByteArrayData data[16];
    char stringdata0[254];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviIrcConnection_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviIrcConnection_t qt_meta_stringdata_KviIrcConnection = {
    {
QT_MOC_LITERAL(0, 0, 16), // "KviIrcConnection"
QT_MOC_LITERAL(1, 17, 16), // "awayStateChanged"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 15), // "userModeChanged"
QT_MOC_LITERAL(4, 51, 15), // "nickNameChanged"
QT_MOC_LITERAL(5, 67, 17), // "channelRegistered"
QT_MOC_LITERAL(6, 85, 17), // "KviChannelWindow*"
QT_MOC_LITERAL(7, 103, 5), // "pChan"
QT_MOC_LITERAL(8, 109, 19), // "channelUnregistered"
QT_MOC_LITERAL(9, 129, 15), // "chanListChanged"
QT_MOC_LITERAL(10, 145, 22), // "unhighlightAllChannels"
QT_MOC_LITERAL(11, 168, 21), // "unhighlightAllQueries"
QT_MOC_LITERAL(12, 190, 17), // "restartNotifyList"
QT_MOC_LITERAL(13, 208, 24), // "hostNameLookupTerminated"
QT_MOC_LITERAL(14, 233, 15), // "KviDnsResolver*"
QT_MOC_LITERAL(15, 249, 4) // "pDns"

    },
    "KviIrcConnection\0awayStateChanged\0\0"
    "userModeChanged\0nickNameChanged\0"
    "channelRegistered\0KviChannelWindow*\0"
    "pChan\0channelUnregistered\0chanListChanged\0"
    "unhighlightAllChannels\0unhighlightAllQueries\0"
    "restartNotifyList\0hostNameLookupTerminated\0"
    "KviDnsResolver*\0pDns"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviIrcConnection[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x06 /* Public */,
       3,    0,   65,    2, 0x06 /* Public */,
       4,    0,   66,    2, 0x06 /* Public */,
       5,    1,   67,    2, 0x06 /* Public */,
       8,    1,   70,    2, 0x06 /* Public */,
       9,    0,   73,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    0,   74,    2, 0x0a /* Public */,
      11,    0,   75,    2, 0x0a /* Public */,
      12,    0,   76,    2, 0x0a /* Public */,
      13,    1,   77,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 14,   15,

       0        // eod
};

void KviIrcConnection::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviIrcConnection *_t = static_cast<KviIrcConnection *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->awayStateChanged(); break;
        case 1: _t->userModeChanged(); break;
        case 2: _t->nickNameChanged(); break;
        case 3: _t->channelRegistered((*reinterpret_cast< KviChannelWindow*(*)>(_a[1]))); break;
        case 4: _t->channelUnregistered((*reinterpret_cast< KviChannelWindow*(*)>(_a[1]))); break;
        case 5: _t->chanListChanged(); break;
        case 6: _t->unhighlightAllChannels(); break;
        case 7: _t->unhighlightAllQueries(); break;
        case 8: _t->restartNotifyList(); break;
        case 9: _t->hostNameLookupTerminated((*reinterpret_cast< KviDnsResolver*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (KviIrcConnection::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviIrcConnection::awayStateChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (KviIrcConnection::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviIrcConnection::userModeChanged)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (KviIrcConnection::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviIrcConnection::nickNameChanged)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (KviIrcConnection::*_t)(KviChannelWindow * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviIrcConnection::channelRegistered)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (KviIrcConnection::*_t)(KviChannelWindow * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviIrcConnection::channelUnregistered)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (KviIrcConnection::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviIrcConnection::chanListChanged)) {
                *result = 5;
                return;
            }
        }
    }
}

const QMetaObject KviIrcConnection::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_KviIrcConnection.data,
      qt_meta_data_KviIrcConnection,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviIrcConnection::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviIrcConnection::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviIrcConnection.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int KviIrcConnection::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void KviIrcConnection::awayStateChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void KviIrcConnection::userModeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void KviIrcConnection::nickNameChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void KviIrcConnection::channelRegistered(KviChannelWindow * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void KviIrcConnection::channelUnregistered(KviChannelWindow * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void KviIrcConnection::chanListChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
