/****************************************************************************
** Meta object code from reading C++ file 'KvsObject_tableWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../KvsObject_tableWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KvsObject_tableWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KvsObject_tableWidget_t {
    QByteArrayData data[8];
    char stringdata0[99];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KvsObject_tableWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KvsObject_tableWidget_t qt_meta_stringdata_KvsObject_tableWidget = {
    {
QT_MOC_LITERAL(0, 0, 21), // "KvsObject_tableWidget"
QT_MOC_LITERAL(1, 22, 15), // "slotItemEntered"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 17), // "QTableWidgetItem*"
QT_MOC_LITERAL(4, 57, 13), // "cellActivated"
QT_MOC_LITERAL(5, 71, 4), // "iRow"
QT_MOC_LITERAL(6, 76, 4), // "iCol"
QT_MOC_LITERAL(7, 81, 17) // "cellDoubleClicked"

    },
    "KvsObject_tableWidget\0slotItemEntered\0"
    "\0QTableWidgetItem*\0cellActivated\0iRow\0"
    "iCol\0cellDoubleClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KvsObject_tableWidget[] = {

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
       4,    2,   32,    2, 0x09 /* Protected */,
       7,    2,   37,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    5,    6,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    5,    6,

       0        // eod
};

void KvsObject_tableWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KvsObject_tableWidget *_t = static_cast<KvsObject_tableWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->slotItemEntered((*reinterpret_cast< QTableWidgetItem*(*)>(_a[1]))); break;
        case 1: _t->cellActivated((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->cellDoubleClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject KvsObject_tableWidget::staticMetaObject = {
    { &KvsObject_widget::staticMetaObject, qt_meta_stringdata_KvsObject_tableWidget.data,
      qt_meta_data_KvsObject_tableWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KvsObject_tableWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KvsObject_tableWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KvsObject_tableWidget.stringdata0))
        return static_cast<void*>(this);
    return KvsObject_widget::qt_metacast(_clname);
}

int KvsObject_tableWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KvsObject_widget::qt_metacall(_c, _id, _a);
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
