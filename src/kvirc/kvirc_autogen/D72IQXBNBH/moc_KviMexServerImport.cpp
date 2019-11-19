/****************************************************************************
** Meta object code from reading C++ file 'KviMexServerImport.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../module/KviMexServerImport.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KviMexServerImport.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviMexServerImport_t {
    QByteArrayData data[6];
    char stringdata0[50];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviMexServerImport_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviMexServerImport_t qt_meta_stringdata_KviMexServerImport = {
    {
QT_MOC_LITERAL(0, 0, 18), // "KviMexServerImport"
QT_MOC_LITERAL(1, 19, 6), // "server"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 12), // "KviIrcServer"
QT_MOC_LITERAL(4, 40, 1), // "s"
QT_MOC_LITERAL(5, 42, 7) // "network"

    },
    "KviMexServerImport\0server\0\0KviIrcServer\0"
    "s\0network"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviMexServerImport[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::QString,    4,    5,

       0        // eod
};

void KviMexServerImport::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviMexServerImport *_t = static_cast<KviMexServerImport *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->server((*reinterpret_cast< const KviIrcServer(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (KviMexServerImport::*_t)(const KviIrcServer & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviMexServerImport::server)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject KviMexServerImport::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_KviMexServerImport.data,
      qt_meta_data_KviMexServerImport,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviMexServerImport::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviMexServerImport::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviMexServerImport.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "KviModuleExtension"))
        return static_cast< KviModuleExtension*>(this);
    return QObject::qt_metacast(_clname);
}

int KviMexServerImport::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void KviMexServerImport::server(const KviIrcServer & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
