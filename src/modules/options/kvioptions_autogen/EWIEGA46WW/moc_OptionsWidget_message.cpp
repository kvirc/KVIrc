/****************************************************************************
** Meta object code from reading C++ file 'OptionsWidget_message.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../OptionsWidget_message.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'OptionsWidget_message.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_OptionsWidget_message_t {
    QByteArrayData data[1];
    char stringdata0[22];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_OptionsWidget_message_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_OptionsWidget_message_t qt_meta_stringdata_OptionsWidget_message = {
    {
QT_MOC_LITERAL(0, 0, 21) // "OptionsWidget_message"

    },
    "OptionsWidget_message"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_OptionsWidget_message[] = {

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

void OptionsWidget_message::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject OptionsWidget_message::staticMetaObject = {
    { &KviOptionsWidget::staticMetaObject, qt_meta_stringdata_OptionsWidget_message.data,
      qt_meta_data_OptionsWidget_message,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *OptionsWidget_message::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OptionsWidget_message::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_OptionsWidget_message.stringdata0))
        return static_cast<void*>(this);
    return KviOptionsWidget::qt_metacast(_clname);
}

int OptionsWidget_message::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviOptionsWidget::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_OptionsWidget_privmsg_t {
    QByteArrayData data[3];
    char stringdata0[55];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_OptionsWidget_privmsg_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_OptionsWidget_privmsg_t qt_meta_stringdata_OptionsWidget_privmsg = {
    {
QT_MOC_LITERAL(0, 0, 21), // "OptionsWidget_privmsg"
QT_MOC_LITERAL(1, 22, 31), // "enableDisableSmartColorSelector"
QT_MOC_LITERAL(2, 54, 0) // ""

    },
    "OptionsWidget_privmsg\0"
    "enableDisableSmartColorSelector\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_OptionsWidget_privmsg[] = {

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
       1,    1,   19,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    2,

       0        // eod
};

void OptionsWidget_privmsg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        OptionsWidget_privmsg *_t = static_cast<OptionsWidget_privmsg *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->enableDisableSmartColorSelector((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject OptionsWidget_privmsg::staticMetaObject = {
    { &KviOptionsWidget::staticMetaObject, qt_meta_stringdata_OptionsWidget_privmsg.data,
      qt_meta_data_OptionsWidget_privmsg,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *OptionsWidget_privmsg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OptionsWidget_privmsg::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_OptionsWidget_privmsg.stringdata0))
        return static_cast<void*>(this);
    return KviOptionsWidget::qt_metacast(_clname);
}

int OptionsWidget_privmsg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviOptionsWidget::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_OptionsWidget_timestamp_t {
    QByteArrayData data[3];
    char stringdata0[56];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_OptionsWidget_timestamp_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_OptionsWidget_timestamp_t qt_meta_stringdata_OptionsWidget_timestamp = {
    {
QT_MOC_LITERAL(0, 0, 23), // "OptionsWidget_timestamp"
QT_MOC_LITERAL(1, 24, 30), // "enableDisableTimestampSelector"
QT_MOC_LITERAL(2, 55, 0) // ""

    },
    "OptionsWidget_timestamp\0"
    "enableDisableTimestampSelector\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_OptionsWidget_timestamp[] = {

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
       1,    1,   19,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    2,

       0        // eod
};

void OptionsWidget_timestamp::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        OptionsWidget_timestamp *_t = static_cast<OptionsWidget_timestamp *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->enableDisableTimestampSelector((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject OptionsWidget_timestamp::staticMetaObject = {
    { &KviOptionsWidget::staticMetaObject, qt_meta_stringdata_OptionsWidget_timestamp.data,
      qt_meta_data_OptionsWidget_timestamp,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *OptionsWidget_timestamp::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OptionsWidget_timestamp::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_OptionsWidget_timestamp.stringdata0))
        return static_cast<void*>(this);
    return KviOptionsWidget::qt_metacast(_clname);
}

int OptionsWidget_timestamp::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviOptionsWidget::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_MessageListWidget_t {
    QByteArrayData data[1];
    char stringdata0[18];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MessageListWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MessageListWidget_t qt_meta_stringdata_MessageListWidget = {
    {
QT_MOC_LITERAL(0, 0, 17) // "MessageListWidget"

    },
    "MessageListWidget"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MessageListWidget[] = {

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

void MessageListWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject MessageListWidget::staticMetaObject = {
    { &KviTalListWidget::staticMetaObject, qt_meta_stringdata_MessageListWidget.data,
      qt_meta_data_MessageListWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *MessageListWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MessageListWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MessageListWidget.stringdata0))
        return static_cast<void*>(this);
    return KviTalListWidget::qt_metacast(_clname);
}

int MessageListWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviTalListWidget::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_MessageListWidgetItemDelegate_t {
    QByteArrayData data[1];
    char stringdata0[30];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MessageListWidgetItemDelegate_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MessageListWidgetItemDelegate_t qt_meta_stringdata_MessageListWidgetItemDelegate = {
    {
QT_MOC_LITERAL(0, 0, 29) // "MessageListWidgetItemDelegate"

    },
    "MessageListWidgetItemDelegate"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MessageListWidgetItemDelegate[] = {

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

void MessageListWidgetItemDelegate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject MessageListWidgetItemDelegate::staticMetaObject = {
    { &QItemDelegate::staticMetaObject, qt_meta_stringdata_MessageListWidgetItemDelegate.data,
      qt_meta_data_MessageListWidgetItemDelegate,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *MessageListWidgetItemDelegate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MessageListWidgetItemDelegate::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MessageListWidgetItemDelegate.stringdata0))
        return static_cast<void*>(this);
    return QItemDelegate::qt_metacast(_clname);
}

int MessageListWidgetItemDelegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QItemDelegate::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_MessageColorListWidgetItemDelegate_t {
    QByteArrayData data[1];
    char stringdata0[35];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MessageColorListWidgetItemDelegate_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MessageColorListWidgetItemDelegate_t qt_meta_stringdata_MessageColorListWidgetItemDelegate = {
    {
QT_MOC_LITERAL(0, 0, 34) // "MessageColorListWidgetItemDel..."

    },
    "MessageColorListWidgetItemDelegate"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MessageColorListWidgetItemDelegate[] = {

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

void MessageColorListWidgetItemDelegate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject MessageColorListWidgetItemDelegate::staticMetaObject = {
    { &QItemDelegate::staticMetaObject, qt_meta_stringdata_MessageColorListWidgetItemDelegate.data,
      qt_meta_data_MessageColorListWidgetItemDelegate,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *MessageColorListWidgetItemDelegate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MessageColorListWidgetItemDelegate::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MessageColorListWidgetItemDelegate.stringdata0))
        return static_cast<void*>(this);
    return QItemDelegate::qt_metacast(_clname);
}

int MessageColorListWidgetItemDelegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QItemDelegate::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_OptionsWidget_messageColors_t {
    QByteArrayData data[11];
    char stringdata0[137];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_OptionsWidget_messageColors_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_OptionsWidget_messageColors_t qt_meta_stringdata_OptionsWidget_messageColors = {
    {
QT_MOC_LITERAL(0, 0, 27), // "OptionsWidget_messageColors"
QT_MOC_LITERAL(1, 28, 11), // "itemChanged"
QT_MOC_LITERAL(2, 40, 0), // ""
QT_MOC_LITERAL(3, 41, 12), // "colorChanged"
QT_MOC_LITERAL(4, 54, 17), // "iconButtonClicked"
QT_MOC_LITERAL(5, 72, 15), // "newIconSelected"
QT_MOC_LITERAL(6, 88, 25), // "KviIconManager::SmallIcon"
QT_MOC_LITERAL(7, 114, 5), // "eIcon"
QT_MOC_LITERAL(8, 120, 6), // "commit"
QT_MOC_LITERAL(9, 127, 4), // "save"
QT_MOC_LITERAL(10, 132, 4) // "load"

    },
    "OptionsWidget_messageColors\0itemChanged\0"
    "\0colorChanged\0iconButtonClicked\0"
    "newIconSelected\0KviIconManager::SmallIcon\0"
    "eIcon\0commit\0save\0load"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_OptionsWidget_messageColors[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x09 /* Protected */,
       3,    0,   50,    2, 0x09 /* Protected */,
       4,    0,   51,    2, 0x09 /* Protected */,
       5,    1,   52,    2, 0x09 /* Protected */,
       8,    0,   55,    2, 0x09 /* Protected */,
       9,    0,   56,    2, 0x09 /* Protected */,
      10,    0,   57,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void OptionsWidget_messageColors::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        OptionsWidget_messageColors *_t = static_cast<OptionsWidget_messageColors *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->itemChanged(); break;
        case 1: _t->colorChanged(); break;
        case 2: _t->iconButtonClicked(); break;
        case 3: _t->newIconSelected((*reinterpret_cast< KviIconManager::SmallIcon(*)>(_a[1]))); break;
        case 4: _t->commit(); break;
        case 5: _t->save(); break;
        case 6: _t->load(); break;
        default: ;
        }
    }
}

const QMetaObject OptionsWidget_messageColors::staticMetaObject = {
    { &KviOptionsWidget::staticMetaObject, qt_meta_stringdata_OptionsWidget_messageColors.data,
      qt_meta_data_OptionsWidget_messageColors,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *OptionsWidget_messageColors::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OptionsWidget_messageColors::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_OptionsWidget_messageColors.stringdata0))
        return static_cast<void*>(this);
    return KviOptionsWidget::qt_metacast(_clname);
}

int OptionsWidget_messageColors::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviOptionsWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}
struct qt_meta_stringdata_OptionsWidget_standardColors_t {
    QByteArrayData data[3];
    char stringdata0[46];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_OptionsWidget_standardColors_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_OptionsWidget_standardColors_t qt_meta_stringdata_OptionsWidget_standardColors = {
    {
QT_MOC_LITERAL(0, 0, 28), // "OptionsWidget_standardColors"
QT_MOC_LITERAL(1, 29, 15), // "resetToDefaults"
QT_MOC_LITERAL(2, 45, 0) // ""

    },
    "OptionsWidget_standardColors\0"
    "resetToDefaults\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_OptionsWidget_standardColors[] = {

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
       1,    0,   19,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void OptionsWidget_standardColors::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        OptionsWidget_standardColors *_t = static_cast<OptionsWidget_standardColors *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->resetToDefaults(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject OptionsWidget_standardColors::staticMetaObject = {
    { &KviOptionsWidget::staticMetaObject, qt_meta_stringdata_OptionsWidget_standardColors.data,
      qt_meta_data_OptionsWidget_standardColors,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *OptionsWidget_standardColors::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OptionsWidget_standardColors::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_OptionsWidget_standardColors.stringdata0))
        return static_cast<void*>(this);
    return KviOptionsWidget::qt_metacast(_clname);
}

int OptionsWidget_standardColors::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviOptionsWidget::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
