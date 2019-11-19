/****************************************************************************
** Meta object code from reading C++ file 'KviTreeWindowList.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ui/KviTreeWindowList.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KviTreeWindowList.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviTreeWindowListTreeWidget_t {
    QByteArrayData data[4];
    char stringdata0[46];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviTreeWindowListTreeWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviTreeWindowListTreeWidget_t qt_meta_stringdata_KviTreeWindowListTreeWidget = {
    {
QT_MOC_LITERAL(0, 0, 27), // "KviTreeWindowListTreeWidget"
QT_MOC_LITERAL(1, 28, 4), // "sort"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 11) // "reverseSort"

    },
    "KviTreeWindowListTreeWidget\0sort\0\0"
    "reverseSort"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviTreeWindowListTreeWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x0a /* Public */,
       3,    0,   25,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void KviTreeWindowListTreeWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviTreeWindowListTreeWidget *_t = static_cast<KviTreeWindowListTreeWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sort(); break;
        case 1: _t->reverseSort(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject KviTreeWindowListTreeWidget::staticMetaObject = {
    { &QTreeWidget::staticMetaObject, qt_meta_stringdata_KviTreeWindowListTreeWidget.data,
      qt_meta_data_KviTreeWindowListTreeWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviTreeWindowListTreeWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviTreeWindowListTreeWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviTreeWindowListTreeWidget.stringdata0))
        return static_cast<void*>(this);
    return QTreeWidget::qt_metacast(_clname);
}

int KviTreeWindowListTreeWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}
struct qt_meta_stringdata_KviTreeWindowList_t {
    QByteArrayData data[6];
    char stringdata0[57];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviTreeWindowList_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviTreeWindowList_t qt_meta_stringdata_KviTreeWindowList = {
    {
QT_MOC_LITERAL(0, 0, 17), // "KviTreeWindowList"
QT_MOC_LITERAL(1, 18, 10), // "tipRequest"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 18), // "KviDynamicToolTip*"
QT_MOC_LITERAL(4, 49, 3), // "tip"
QT_MOC_LITERAL(5, 53, 3) // "pnt"

    },
    "KviTreeWindowList\0tipRequest\0\0"
    "KviDynamicToolTip*\0tip\0pnt"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviTreeWindowList[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   19,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::QPoint,    4,    5,

       0        // eod
};

void KviTreeWindowList::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviTreeWindowList *_t = static_cast<KviTreeWindowList *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->tipRequest((*reinterpret_cast< KviDynamicToolTip*(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject KviTreeWindowList::staticMetaObject = {
    { &KviWindowListBase::staticMetaObject, qt_meta_stringdata_KviTreeWindowList.data,
      qt_meta_data_KviTreeWindowList,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviTreeWindowList::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviTreeWindowList::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviTreeWindowList.stringdata0))
        return static_cast<void*>(this);
    return KviWindowListBase::qt_metacast(_clname);
}

int KviTreeWindowList::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviWindowListBase::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_KviTreeWindowListItemDelegate_t {
    QByteArrayData data[1];
    char stringdata0[30];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviTreeWindowListItemDelegate_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviTreeWindowListItemDelegate_t qt_meta_stringdata_KviTreeWindowListItemDelegate = {
    {
QT_MOC_LITERAL(0, 0, 29) // "KviTreeWindowListItemDelegate"

    },
    "KviTreeWindowListItemDelegate"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviTreeWindowListItemDelegate[] = {

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

void KviTreeWindowListItemDelegate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject KviTreeWindowListItemDelegate::staticMetaObject = {
    { &QStyledItemDelegate::staticMetaObject, qt_meta_stringdata_KviTreeWindowListItemDelegate.data,
      qt_meta_data_KviTreeWindowListItemDelegate,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviTreeWindowListItemDelegate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviTreeWindowListItemDelegate::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviTreeWindowListItemDelegate.stringdata0))
        return static_cast<void*>(this);
    return QStyledItemDelegate::qt_metacast(_clname);
}

int KviTreeWindowListItemDelegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStyledItemDelegate::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
