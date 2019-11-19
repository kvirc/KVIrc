/****************************************************************************
** Meta object code from reading C++ file 'KvsObject_listWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../KvsObject_listWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KvsObject_listWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KvsObject_listWidget_t {
    QByteArrayData data[8];
    char stringdata0[112];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KvsObject_listWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KvsObject_listWidget_t qt_meta_stringdata_KvsObject_listWidget = {
    {
QT_MOC_LITERAL(0, 0, 20), // "KvsObject_listWidget"
QT_MOC_LITERAL(1, 21, 16), // "selectionChanged"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 18), // "currentItemChanged"
QT_MOC_LITERAL(4, 58, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(5, 75, 15), // "slotItemEntered"
QT_MOC_LITERAL(6, 91, 4), // "item"
QT_MOC_LITERAL(7, 96, 15) // "slotItemChanged"

    },
    "KvsObject_listWidget\0selectionChanged\0"
    "\0currentItemChanged\0QListWidgetItem*\0"
    "slotItemEntered\0item\0slotItemChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KvsObject_listWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x09 /* Protected */,
       3,    2,   35,    2, 0x09 /* Protected */,
       5,    1,   40,    2, 0x09 /* Protected */,
       7,    1,   43,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4, 0x80000000 | 4,    2,    2,
    QMetaType::Void, 0x80000000 | 4,    6,
    QMetaType::Void, 0x80000000 | 4,    6,

       0        // eod
};

void KvsObject_listWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KvsObject_listWidget *_t = static_cast<KvsObject_listWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->selectionChanged(); break;
        case 1: _t->currentItemChanged((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        case 2: _t->slotItemEntered((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 3: _t->slotItemChanged((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject KvsObject_listWidget::staticMetaObject = {
    { &KvsObject_widget::staticMetaObject, qt_meta_stringdata_KvsObject_listWidget.data,
      qt_meta_data_KvsObject_listWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KvsObject_listWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KvsObject_listWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KvsObject_listWidget.stringdata0))
        return static_cast<void*>(this);
    return KvsObject_widget::qt_metacast(_clname);
}

int KvsObject_listWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KvsObject_widget::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
