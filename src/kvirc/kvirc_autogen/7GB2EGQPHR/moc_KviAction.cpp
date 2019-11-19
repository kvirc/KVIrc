/****************************************************************************
** Meta object code from reading C++ file 'KviAction.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../kernel/KviAction.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KviAction.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviAction_t {
    QByteArrayData data[11];
    char stringdata0[173];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviAction_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviAction_t qt_meta_stringdata_KviAction = {
    {
QT_MOC_LITERAL(0, 0, 9), // "KviAction"
QT_MOC_LITERAL(1, 10, 9), // "activated"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 8), // "activate"
QT_MOC_LITERAL(4, 30, 15), // "actionDestroyed"
QT_MOC_LITERAL(5, 46, 12), // "reloadImages"
QT_MOC_LITERAL(6, 59, 20), // "activeContextChanged"
QT_MOC_LITERAL(7, 80, 25), // "activeContextStateChanged"
QT_MOC_LITERAL(8, 106, 19), // "activeWindowChanged"
QT_MOC_LITERAL(9, 126, 33), // "activeWindowSelectionStateCha..."
QT_MOC_LITERAL(10, 160, 12) // "bSelectedNow"

    },
    "KviAction\0activated\0\0activate\0"
    "actionDestroyed\0reloadImages\0"
    "activeContextChanged\0activeContextStateChanged\0"
    "activeWindowChanged\0"
    "activeWindowSelectionStateChanged\0"
    "bSelectedNow"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviAction[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   55,    2, 0x0a /* Public */,
       4,    0,   56,    2, 0x09 /* Protected */,
       5,    0,   57,    2, 0x09 /* Protected */,
       6,    0,   58,    2, 0x09 /* Protected */,
       7,    0,   59,    2, 0x09 /* Protected */,
       8,    0,   60,    2, 0x09 /* Protected */,
       9,    1,   61,    2, 0x09 /* Protected */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   10,

       0        // eod
};

void KviAction::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviAction *_t = static_cast<KviAction *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->activated(); break;
        case 1: _t->activate(); break;
        case 2: _t->actionDestroyed(); break;
        case 3: _t->reloadImages(); break;
        case 4: _t->activeContextChanged(); break;
        case 5: _t->activeContextStateChanged(); break;
        case 6: _t->activeWindowChanged(); break;
        case 7: _t->activeWindowSelectionStateChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (KviAction::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviAction::activated)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject KviAction::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_KviAction.data,
      qt_meta_data_KviAction,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviAction::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviAction::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviAction.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int KviAction::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void KviAction::activated()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
