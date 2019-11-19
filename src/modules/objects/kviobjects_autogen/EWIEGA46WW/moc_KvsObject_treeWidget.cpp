/****************************************************************************
** Meta object code from reading C++ file 'KvsObject_treeWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../KvsObject_treeWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KvsObject_treeWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KvsObject_treeWidget_t {
    QByteArrayData data[12];
    char stringdata0[182];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KvsObject_treeWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KvsObject_treeWidget_t qt_meta_stringdata_KvsObject_treeWidget = {
    {
QT_MOC_LITERAL(0, 0, 20), // "KvsObject_treeWidget"
QT_MOC_LITERAL(1, 21, 11), // "slotClicked"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 16), // "QTreeWidgetItem*"
QT_MOC_LITERAL(4, 51, 20), // "slotSelectionChanged"
QT_MOC_LITERAL(5, 72, 18), // "slotCurrentChanged"
QT_MOC_LITERAL(6, 91, 17), // "slotItemActivated"
QT_MOC_LITERAL(7, 109, 3), // "col"
QT_MOC_LITERAL(8, 113, 17), // "slotOnItemEntered"
QT_MOC_LITERAL(9, 131, 16), // "slotItemExpanded"
QT_MOC_LITERAL(10, 148, 17), // "slotItemCollapsed"
QT_MOC_LITERAL(11, 166, 15) // "slotItemChanged"

    },
    "KvsObject_treeWidget\0slotClicked\0\0"
    "QTreeWidgetItem*\0slotSelectionChanged\0"
    "slotCurrentChanged\0slotItemActivated\0"
    "col\0slotOnItemEntered\0slotItemExpanded\0"
    "slotItemCollapsed\0slotItemChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KvsObject_treeWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   54,    2, 0x09 /* Protected */,
       4,    0,   59,    2, 0x09 /* Protected */,
       5,    2,   60,    2, 0x09 /* Protected */,
       6,    2,   65,    2, 0x09 /* Protected */,
       8,    2,   70,    2, 0x09 /* Protected */,
       9,    1,   75,    2, 0x09 /* Protected */,
      10,    1,   78,    2, 0x09 /* Protected */,
      11,    2,   81,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int,    2,    2,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    2,    2,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int,    2,    7,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int,    2,    7,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int,    2,    2,

       0        // eod
};

void KvsObject_treeWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KvsObject_treeWidget *_t = static_cast<KvsObject_treeWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->slotClicked((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->slotSelectionChanged(); break;
        case 2: _t->slotCurrentChanged((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QTreeWidgetItem*(*)>(_a[2]))); break;
        case 3: _t->slotItemActivated((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->slotOnItemEntered((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: _t->slotItemExpanded((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1]))); break;
        case 6: _t->slotItemCollapsed((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1]))); break;
        case 7: _t->slotItemChanged((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject KvsObject_treeWidget::staticMetaObject = {
    { &KvsObject_widget::staticMetaObject, qt_meta_stringdata_KvsObject_treeWidget.data,
      qt_meta_data_KvsObject_treeWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KvsObject_treeWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KvsObject_treeWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KvsObject_treeWidget.stringdata0))
        return static_cast<void*>(this);
    return KvsObject_widget::qt_metacast(_clname);
}

int KvsObject_treeWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KvsObject_widget::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_KviKvsTreeWidget_t {
    QByteArrayData data[1];
    char stringdata0[17];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviKvsTreeWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviKvsTreeWidget_t qt_meta_stringdata_KviKvsTreeWidget = {
    {
QT_MOC_LITERAL(0, 0, 16) // "KviKvsTreeWidget"

    },
    "KviKvsTreeWidget"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviKvsTreeWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void KviKvsTreeWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject KviKvsTreeWidget::staticMetaObject = {
    { &QTreeWidget::staticMetaObject, qt_meta_stringdata_KviKvsTreeWidget.data,
      qt_meta_data_KviKvsTreeWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviKvsTreeWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviKvsTreeWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviKvsTreeWidget.stringdata0))
        return static_cast<void*>(this);
    return QTreeWidget::qt_metacast(_clname);
}

int KviKvsTreeWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeWidget::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
