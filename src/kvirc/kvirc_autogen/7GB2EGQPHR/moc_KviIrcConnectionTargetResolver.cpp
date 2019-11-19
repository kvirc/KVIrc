/****************************************************************************
** Meta object code from reading C++ file 'KviIrcConnectionTargetResolver.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../kernel/KviIrcConnectionTargetResolver.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KviIrcConnectionTargetResolver.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviIrcConnectionTargetResolver_t {
    QByteArrayData data[7];
    char stringdata0[122];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviIrcConnectionTargetResolver_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviIrcConnectionTargetResolver_t qt_meta_stringdata_KviIrcConnectionTargetResolver = {
    {
QT_MOC_LITERAL(0, 0, 30), // "KviIrcConnectionTargetResolver"
QT_MOC_LITERAL(1, 31, 10), // "terminated"
QT_MOC_LITERAL(2, 42, 0), // ""
QT_MOC_LITERAL(3, 43, 17), // "asyncStartResolve"
QT_MOC_LITERAL(4, 61, 22), // "serverLookupTerminated"
QT_MOC_LITERAL(5, 84, 15), // "KviDnsResolver*"
QT_MOC_LITERAL(6, 100, 21) // "proxyLookupTerminated"

    },
    "KviIrcConnectionTargetResolver\0"
    "terminated\0\0asyncStartResolve\0"
    "serverLookupTerminated\0KviDnsResolver*\0"
    "proxyLookupTerminated"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviIrcConnectionTargetResolver[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   35,    2, 0x09 /* Protected */,
       4,    1,   36,    2, 0x09 /* Protected */,
       6,    1,   39,    2, 0x09 /* Protected */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    2,
    QMetaType::Void, 0x80000000 | 5,    2,

       0        // eod
};

void KviIrcConnectionTargetResolver::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviIrcConnectionTargetResolver *_t = static_cast<KviIrcConnectionTargetResolver *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->terminated(); break;
        case 1: _t->asyncStartResolve(); break;
        case 2: _t->serverLookupTerminated((*reinterpret_cast< KviDnsResolver*(*)>(_a[1]))); break;
        case 3: _t->proxyLookupTerminated((*reinterpret_cast< KviDnsResolver*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (KviIrcConnectionTargetResolver::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviIrcConnectionTargetResolver::terminated)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject KviIrcConnectionTargetResolver::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_KviIrcConnectionTargetResolver.data,
      qt_meta_data_KviIrcConnectionTargetResolver,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviIrcConnectionTargetResolver::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviIrcConnectionTargetResolver::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviIrcConnectionTargetResolver.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int KviIrcConnectionTargetResolver::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void KviIrcConnectionTargetResolver::terminated()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
