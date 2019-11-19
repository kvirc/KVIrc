/****************************************************************************
** Meta object code from reading C++ file 'OptionsWidget_proxy.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../OptionsWidget_proxy.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'OptionsWidget_proxy.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_OptionsWidget_proxy_t {
    QByteArrayData data[12];
    char stringdata0[145];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_OptionsWidget_proxy_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_OptionsWidget_proxy_t qt_meta_stringdata_OptionsWidget_proxy = {
    {
QT_MOC_LITERAL(0, 0, 19), // "OptionsWidget_proxy"
QT_MOC_LITERAL(1, 20, 18), // "currentItemChanged"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 16), // "QTreeWidgetItem*"
QT_MOC_LITERAL(4, 57, 2), // "it"
QT_MOC_LITERAL(5, 60, 4), // "prev"
QT_MOC_LITERAL(6, 65, 26), // "customContextMenuRequested"
QT_MOC_LITERAL(7, 92, 3), // "pnt"
QT_MOC_LITERAL(8, 96, 8), // "newProxy"
QT_MOC_LITERAL(9, 105, 13), // "removeCurrent"
QT_MOC_LITERAL(10, 119, 16), // "ipV6CheckToggled"
QT_MOC_LITERAL(11, 136, 8) // "bEnabled"

    },
    "OptionsWidget_proxy\0currentItemChanged\0"
    "\0QTreeWidgetItem*\0it\0prev\0"
    "customContextMenuRequested\0pnt\0newProxy\0"
    "removeCurrent\0ipV6CheckToggled\0bEnabled"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_OptionsWidget_proxy[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   39,    2, 0x09 /* Protected */,
       6,    1,   44,    2, 0x09 /* Protected */,
       8,    0,   47,    2, 0x09 /* Protected */,
       9,    0,   48,    2, 0x09 /* Protected */,
      10,    1,   49,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    5,
    QMetaType::Void, QMetaType::QPoint,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   11,

       0        // eod
};

void OptionsWidget_proxy::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        OptionsWidget_proxy *_t = static_cast<OptionsWidget_proxy *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->currentItemChanged((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QTreeWidgetItem*(*)>(_a[2]))); break;
        case 1: _t->customContextMenuRequested((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 2: _t->newProxy(); break;
        case 3: _t->removeCurrent(); break;
        case 4: _t->ipV6CheckToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject OptionsWidget_proxy::staticMetaObject = {
    { &KviOptionsWidget::staticMetaObject, qt_meta_stringdata_OptionsWidget_proxy.data,
      qt_meta_data_OptionsWidget_proxy,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *OptionsWidget_proxy::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OptionsWidget_proxy::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_OptionsWidget_proxy.stringdata0))
        return static_cast<void*>(this);
    return KviOptionsWidget::qt_metacast(_clname);
}

int OptionsWidget_proxy::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviOptionsWidget::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
