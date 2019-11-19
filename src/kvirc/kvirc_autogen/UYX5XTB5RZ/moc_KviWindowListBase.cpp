/****************************************************************************
** Meta object code from reading C++ file 'KviWindowListBase.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ui/KviWindowListBase.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KviWindowListBase.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviWindowListBase_t {
    QByteArrayData data[6];
    char stringdata0[85];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviWindowListBase_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviWindowListBase_t qt_meta_stringdata_KviWindowListBase = {
    {
QT_MOC_LITERAL(0, 0, 17), // "KviWindowListBase"
QT_MOC_LITERAL(1, 18, 19), // "updateActivityMeter"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 18), // "updateDockLocation"
QT_MOC_LITERAL(4, 58, 18), // "Qt::DockWidgetArea"
QT_MOC_LITERAL(5, 77, 7) // "newArea"

    },
    "KviWindowListBase\0updateActivityMeter\0"
    "\0updateDockLocation\0Qt::DockWidgetArea\0"
    "newArea"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviWindowListBase[] = {

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
       1,    0,   24,    2, 0x09 /* Protected */,
       3,    1,   25,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,

       0        // eod
};

void KviWindowListBase::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviWindowListBase *_t = static_cast<KviWindowListBase *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateActivityMeter(); break;
        case 1: _t->updateDockLocation((*reinterpret_cast< Qt::DockWidgetArea(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject KviWindowListBase::staticMetaObject = {
    { &QDockWidget::staticMetaObject, qt_meta_stringdata_KviWindowListBase.data,
      qt_meta_data_KviWindowListBase,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviWindowListBase::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviWindowListBase::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviWindowListBase.stringdata0))
        return static_cast<void*>(this);
    return QDockWidget::qt_metacast(_clname);
}

int KviWindowListBase::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_KviWindowListButton_t {
    QByteArrayData data[6];
    char stringdata0[59];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviWindowListButton_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviWindowListButton_t qt_meta_stringdata_KviWindowListButton = {
    {
QT_MOC_LITERAL(0, 0, 19), // "KviWindowListButton"
QT_MOC_LITERAL(1, 20, 10), // "tipRequest"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 18), // "KviDynamicToolTip*"
QT_MOC_LITERAL(4, 51, 3), // "tip"
QT_MOC_LITERAL(5, 55, 3) // "pnt"

    },
    "KviWindowListButton\0tipRequest\0\0"
    "KviDynamicToolTip*\0tip\0pnt"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviWindowListButton[] = {

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

void KviWindowListButton::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviWindowListButton *_t = static_cast<KviWindowListButton *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->tipRequest((*reinterpret_cast< KviDynamicToolTip*(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject KviWindowListButton::staticMetaObject = {
    { &QPushButton::staticMetaObject, qt_meta_stringdata_KviWindowListButton.data,
      qt_meta_data_KviWindowListButton,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviWindowListButton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviWindowListButton::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviWindowListButton.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "KviWindowListItem"))
        return static_cast< KviWindowListItem*>(this);
    return QPushButton::qt_metacast(_clname);
}

int KviWindowListButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QPushButton::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_KviClassicWindowListToolButton_t {
    QByteArrayData data[1];
    char stringdata0[31];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviClassicWindowListToolButton_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviClassicWindowListToolButton_t qt_meta_stringdata_KviClassicWindowListToolButton = {
    {
QT_MOC_LITERAL(0, 0, 30) // "KviClassicWindowListToolButton"

    },
    "KviClassicWindowListToolButton"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviClassicWindowListToolButton[] = {

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

void KviClassicWindowListToolButton::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject KviClassicWindowListToolButton::staticMetaObject = {
    { &QToolButton::staticMetaObject, qt_meta_stringdata_KviClassicWindowListToolButton.data,
      qt_meta_data_KviClassicWindowListToolButton,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviClassicWindowListToolButton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviClassicWindowListToolButton::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviClassicWindowListToolButton.stringdata0))
        return static_cast<void*>(this);
    return QToolButton::qt_metacast(_clname);
}

int KviClassicWindowListToolButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QToolButton::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_KviClassicWindowList_t {
    QByteArrayData data[6];
    char stringdata0[72];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviClassicWindowList_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviClassicWindowList_t qt_meta_stringdata_KviClassicWindowList = {
    {
QT_MOC_LITERAL(0, 0, 20), // "KviClassicWindowList"
QT_MOC_LITERAL(1, 21, 22), // "orientationChangedSlot"
QT_MOC_LITERAL(2, 44, 0), // ""
QT_MOC_LITERAL(3, 45, 15), // "Qt::Orientation"
QT_MOC_LITERAL(4, 61, 1), // "o"
QT_MOC_LITERAL(5, 63, 8) // "doLayout"

    },
    "KviClassicWindowList\0orientationChangedSlot\0"
    "\0Qt::Orientation\0o\0doLayout"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviClassicWindowList[] = {

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
       1,    1,   24,    2, 0x09 /* Protected */,
       5,    0,   27,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,

       0        // eod
};

void KviClassicWindowList::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviClassicWindowList *_t = static_cast<KviClassicWindowList *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->orientationChangedSlot((*reinterpret_cast< Qt::Orientation(*)>(_a[1]))); break;
        case 1: _t->doLayout(); break;
        default: ;
        }
    }
}

const QMetaObject KviClassicWindowList::staticMetaObject = {
    { &KviWindowListBase::staticMetaObject, qt_meta_stringdata_KviClassicWindowList.data,
      qt_meta_data_KviClassicWindowList,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviClassicWindowList::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviClassicWindowList::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviClassicWindowList.stringdata0))
        return static_cast<void*>(this);
    return KviWindowListBase::qt_metacast(_clname);
}

int KviClassicWindowList::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviWindowListBase::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_KviWindowListTitleWidget_t {
    QByteArrayData data[1];
    char stringdata0[25];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviWindowListTitleWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviWindowListTitleWidget_t qt_meta_stringdata_KviWindowListTitleWidget = {
    {
QT_MOC_LITERAL(0, 0, 24) // "KviWindowListTitleWidget"

    },
    "KviWindowListTitleWidget"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviWindowListTitleWidget[] = {

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

void KviWindowListTitleWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject KviWindowListTitleWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_KviWindowListTitleWidget.data,
      qt_meta_data_KviWindowListTitleWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviWindowListTitleWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviWindowListTitleWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviWindowListTitleWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int KviWindowListTitleWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
