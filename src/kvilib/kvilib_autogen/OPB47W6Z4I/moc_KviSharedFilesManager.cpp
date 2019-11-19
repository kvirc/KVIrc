/****************************************************************************
** Meta object code from reading C++ file 'KviSharedFilesManager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ext/KviSharedFilesManager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KviSharedFilesManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviSharedFilesManager_t {
    QByteArrayData data[8];
    char stringdata0[101];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviSharedFilesManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviSharedFilesManager_t qt_meta_stringdata_KviSharedFilesManager = {
    {
QT_MOC_LITERAL(0, 0, 21), // "KviSharedFilesManager"
QT_MOC_LITERAL(1, 22, 18), // "sharedFilesChanged"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 15), // "sharedFileAdded"
QT_MOC_LITERAL(4, 58, 14), // "KviSharedFile*"
QT_MOC_LITERAL(5, 73, 1), // "f"
QT_MOC_LITERAL(6, 75, 17), // "sharedFileRemoved"
QT_MOC_LITERAL(7, 93, 7) // "cleanup"

    },
    "KviSharedFilesManager\0sharedFilesChanged\0"
    "\0sharedFileAdded\0KviSharedFile*\0f\0"
    "sharedFileRemoved\0cleanup"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviSharedFilesManager[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x06 /* Public */,
       3,    1,   35,    2, 0x06 /* Public */,
       6,    1,   38,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,   41,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, 0x80000000 | 4,    5,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void KviSharedFilesManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviSharedFilesManager *_t = static_cast<KviSharedFilesManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sharedFilesChanged(); break;
        case 1: _t->sharedFileAdded((*reinterpret_cast< KviSharedFile*(*)>(_a[1]))); break;
        case 2: _t->sharedFileRemoved((*reinterpret_cast< KviSharedFile*(*)>(_a[1]))); break;
        case 3: _t->cleanup(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (KviSharedFilesManager::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviSharedFilesManager::sharedFilesChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (KviSharedFilesManager::*_t)(KviSharedFile * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviSharedFilesManager::sharedFileAdded)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (KviSharedFilesManager::*_t)(KviSharedFile * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviSharedFilesManager::sharedFileRemoved)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject KviSharedFilesManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_KviSharedFilesManager.data,
      qt_meta_data_KviSharedFilesManager,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviSharedFilesManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviSharedFilesManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviSharedFilesManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int KviSharedFilesManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void KviSharedFilesManager::sharedFilesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void KviSharedFilesManager::sharedFileAdded(KviSharedFile * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void KviSharedFilesManager::sharedFileRemoved(KviSharedFile * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
