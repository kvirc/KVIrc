/****************************************************************************
** Meta object code from reading C++ file 'KviWindowStack.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ui/KviWindowStack.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KviWindowStack.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviWindowStack_t {
    QByteArrayData data[8];
    char stringdata0[89];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviWindowStack_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviWindowStack_t qt_meta_stringdata_KviWindowStack = {
    {
QT_MOC_LITERAL(0, 0, 14), // "KviWindowStack"
QT_MOC_LITERAL(1, 15, 20), // "currentWindowChanged"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 4), // "iIdx"
QT_MOC_LITERAL(4, 42, 13), // "menuActivated"
QT_MOC_LITERAL(5, 56, 8), // "QAction*"
QT_MOC_LITERAL(6, 65, 7), // "pAction"
QT_MOC_LITERAL(7, 73, 15) // "fillWindowPopup"

    },
    "KviWindowStack\0currentWindowChanged\0"
    "\0iIdx\0menuActivated\0QAction*\0pAction\0"
    "fillWindowPopup"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviWindowStack[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x09 /* Protected */,
       4,    1,   32,    2, 0x09 /* Protected */,
       7,    0,   35,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void,

       0        // eod
};

void KviWindowStack::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviWindowStack *_t = static_cast<KviWindowStack *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->currentWindowChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->menuActivated((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 2: _t->fillWindowPopup(); break;
        default: ;
        }
    }
}

const QMetaObject KviWindowStack::staticMetaObject = {
    { &QStackedWidget::staticMetaObject, qt_meta_stringdata_KviWindowStack.data,
      qt_meta_data_KviWindowStack,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviWindowStack::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviWindowStack::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviWindowStack.stringdata0))
        return static_cast<void*>(this);
    return QStackedWidget::qt_metacast(_clname);
}

int KviWindowStack::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStackedWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
