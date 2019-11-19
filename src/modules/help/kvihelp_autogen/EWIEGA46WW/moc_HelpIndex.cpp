/****************************************************************************
** Meta object code from reading C++ file 'HelpIndex.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../HelpIndex.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'HelpIndex.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_HelpIndex_t {
    QByteArrayData data[7];
    char stringdata0[82];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_HelpIndex_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_HelpIndex_t qt_meta_stringdata_HelpIndex = {
    {
QT_MOC_LITERAL(0, 0, 9), // "HelpIndex"
QT_MOC_LITERAL(1, 10, 13), // "indexingStart"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 16), // "indexingProgress"
QT_MOC_LITERAL(4, 42, 11), // "indexingEnd"
QT_MOC_LITERAL(5, 54, 16), // "setLastWinClosed"
QT_MOC_LITERAL(6, 71, 10) // "filterNext"

    },
    "HelpIndex\0indexingStart\0\0indexingProgress\0"
    "indexingEnd\0setLastWinClosed\0filterNext"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_HelpIndex[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       3,    1,   42,    2, 0x06 /* Public */,
       4,    0,   45,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   46,    2, 0x08 /* Private */,
       6,    0,   47,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void HelpIndex::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        HelpIndex *_t = static_cast<HelpIndex *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->indexingStart((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->indexingProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->indexingEnd(); break;
        case 3: _t->setLastWinClosed(); break;
        case 4: _t->filterNext(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (HelpIndex::*_t)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HelpIndex::indexingStart)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (HelpIndex::*_t)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HelpIndex::indexingProgress)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (HelpIndex::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HelpIndex::indexingEnd)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject HelpIndex::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_HelpIndex.data,
      qt_meta_data_HelpIndex,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *HelpIndex::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HelpIndex::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_HelpIndex.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int HelpIndex::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void HelpIndex::indexingStart(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void HelpIndex::indexingProgress(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void HelpIndex::indexingEnd()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
