/****************************************************************************
** Meta object code from reading C++ file 'KviStatusBarApplet.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ui/KviStatusBarApplet.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KviStatusBarApplet.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviStatusBarApplet_t {
    QByteArrayData data[1];
    char stringdata0[19];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviStatusBarApplet_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviStatusBarApplet_t qt_meta_stringdata_KviStatusBarApplet = {
    {
QT_MOC_LITERAL(0, 0, 18) // "KviStatusBarApplet"

    },
    "KviStatusBarApplet"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviStatusBarApplet[] = {

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

void KviStatusBarApplet::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject KviStatusBarApplet::staticMetaObject = {
    { &QLabel::staticMetaObject, qt_meta_stringdata_KviStatusBarApplet.data,
      qt_meta_data_KviStatusBarApplet,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviStatusBarApplet::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviStatusBarApplet::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviStatusBarApplet.stringdata0))
        return static_cast<void*>(this);
    return QLabel::qt_metacast(_clname);
}

int KviStatusBarApplet::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLabel::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_KviStatusBarClock_t {
    QByteArrayData data[6];
    char stringdata0[61];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviStatusBarClock_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviStatusBarClock_t qt_meta_stringdata_KviStatusBarClock = {
    {
QT_MOC_LITERAL(0, 0, 17), // "KviStatusBarClock"
QT_MOC_LITERAL(1, 18, 9), // "toggleUtc"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 9), // "toggle24h"
QT_MOC_LITERAL(4, 39, 12), // "changeFormat"
QT_MOC_LITERAL(5, 52, 8) // "QAction*"

    },
    "KviStatusBarClock\0toggleUtc\0\0toggle24h\0"
    "changeFormat\0QAction*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviStatusBarClock[] = {

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
       1,    0,   29,    2, 0x09 /* Protected */,
       3,    0,   30,    2, 0x09 /* Protected */,
       4,    1,   31,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    2,

       0        // eod
};

void KviStatusBarClock::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviStatusBarClock *_t = static_cast<KviStatusBarClock *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->toggleUtc(); break;
        case 1: _t->toggle24h(); break;
        case 2: _t->changeFormat((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject KviStatusBarClock::staticMetaObject = {
    { &KviStatusBarApplet::staticMetaObject, qt_meta_stringdata_KviStatusBarClock.data,
      qt_meta_data_KviStatusBarClock,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviStatusBarClock::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviStatusBarClock::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviStatusBarClock.stringdata0))
        return static_cast<void*>(this);
    return KviStatusBarApplet::qt_metacast(_clname);
}

int KviStatusBarClock::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviStatusBarApplet::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_KviStatusBarConnectionTimer_t {
    QByteArrayData data[3];
    char stringdata0[41];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviStatusBarConnectionTimer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviStatusBarConnectionTimer_t qt_meta_stringdata_KviStatusBarConnectionTimer = {
    {
QT_MOC_LITERAL(0, 0, 27), // "KviStatusBarConnectionTimer"
QT_MOC_LITERAL(1, 28, 11), // "toggleTotal"
QT_MOC_LITERAL(2, 40, 0) // ""

    },
    "KviStatusBarConnectionTimer\0toggleTotal\0"
    ""
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviStatusBarConnectionTimer[] = {

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
       1,    0,   19,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void KviStatusBarConnectionTimer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviStatusBarConnectionTimer *_t = static_cast<KviStatusBarConnectionTimer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->toggleTotal(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject KviStatusBarConnectionTimer::staticMetaObject = {
    { &KviStatusBarApplet::staticMetaObject, qt_meta_stringdata_KviStatusBarConnectionTimer.data,
      qt_meta_data_KviStatusBarConnectionTimer,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviStatusBarConnectionTimer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviStatusBarConnectionTimer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviStatusBarConnectionTimer.stringdata0))
        return static_cast<void*>(this);
    return KviStatusBarApplet::qt_metacast(_clname);
}

int KviStatusBarConnectionTimer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviStatusBarApplet::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_KviStatusBarSeparator_t {
    QByteArrayData data[1];
    char stringdata0[22];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviStatusBarSeparator_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviStatusBarSeparator_t qt_meta_stringdata_KviStatusBarSeparator = {
    {
QT_MOC_LITERAL(0, 0, 21) // "KviStatusBarSeparator"

    },
    "KviStatusBarSeparator"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviStatusBarSeparator[] = {

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

void KviStatusBarSeparator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject KviStatusBarSeparator::staticMetaObject = {
    { &KviStatusBarApplet::staticMetaObject, qt_meta_stringdata_KviStatusBarSeparator.data,
      qt_meta_data_KviStatusBarSeparator,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviStatusBarSeparator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviStatusBarSeparator::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviStatusBarSeparator.stringdata0))
        return static_cast<void*>(this);
    return KviStatusBarApplet::qt_metacast(_clname);
}

int KviStatusBarSeparator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviStatusBarApplet::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_KviStatusBarAwayIndicator_t {
    QByteArrayData data[4];
    char stringdata0[55];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviStatusBarAwayIndicator_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviStatusBarAwayIndicator_t qt_meta_stringdata_KviStatusBarAwayIndicator = {
    {
QT_MOC_LITERAL(0, 0, 25), // "KviStatusBarAwayIndicator"
QT_MOC_LITERAL(1, 26, 13), // "updateDisplay"
QT_MOC_LITERAL(2, 40, 0), // ""
QT_MOC_LITERAL(3, 41, 13) // "toggleContext"

    },
    "KviStatusBarAwayIndicator\0updateDisplay\0"
    "\0toggleContext"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviStatusBarAwayIndicator[] = {

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
       3,    0,   25,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void KviStatusBarAwayIndicator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviStatusBarAwayIndicator *_t = static_cast<KviStatusBarAwayIndicator *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateDisplay(); break;
        case 1: _t->toggleContext(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject KviStatusBarAwayIndicator::staticMetaObject = {
    { &KviStatusBarApplet::staticMetaObject, qt_meta_stringdata_KviStatusBarAwayIndicator.data,
      qt_meta_data_KviStatusBarAwayIndicator,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviStatusBarAwayIndicator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviStatusBarAwayIndicator::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviStatusBarAwayIndicator.stringdata0))
        return static_cast<void*>(this);
    return KviStatusBarApplet::qt_metacast(_clname);
}

int KviStatusBarAwayIndicator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviStatusBarApplet::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_KviStatusBarLagIndicator_t {
    QByteArrayData data[3];
    char stringdata0[40];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviStatusBarLagIndicator_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviStatusBarLagIndicator_t qt_meta_stringdata_KviStatusBarLagIndicator = {
    {
QT_MOC_LITERAL(0, 0, 24), // "KviStatusBarLagIndicator"
QT_MOC_LITERAL(1, 25, 13), // "updateDisplay"
QT_MOC_LITERAL(2, 39, 0) // ""

    },
    "KviStatusBarLagIndicator\0updateDisplay\0"
    ""
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviStatusBarLagIndicator[] = {

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
       1,    0,   19,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void KviStatusBarLagIndicator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviStatusBarLagIndicator *_t = static_cast<KviStatusBarLagIndicator *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateDisplay(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject KviStatusBarLagIndicator::staticMetaObject = {
    { &KviStatusBarApplet::staticMetaObject, qt_meta_stringdata_KviStatusBarLagIndicator.data,
      qt_meta_data_KviStatusBarLagIndicator,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviStatusBarLagIndicator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviStatusBarLagIndicator::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviStatusBarLagIndicator.stringdata0))
        return static_cast<void*>(this);
    return KviStatusBarApplet::qt_metacast(_clname);
}

int KviStatusBarLagIndicator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviStatusBarApplet::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_KviStatusBarUpdateIndicator_t {
    QByteArrayData data[10];
    char stringdata0[146];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviStatusBarUpdateIndicator_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviStatusBarUpdateIndicator_t qt_meta_stringdata_KviStatusBarUpdateIndicator = {
    {
QT_MOC_LITERAL(0, 0, 27), // "KviStatusBarUpdateIndicator"
QT_MOC_LITERAL(1, 28, 13), // "updateDisplay"
QT_MOC_LITERAL(2, 42, 0), // ""
QT_MOC_LITERAL(3, 43, 13), // "toggleStartup"
QT_MOC_LITERAL(4, 57, 14), // "toggleRevision"
QT_MOC_LITERAL(5, 72, 16), // "responseReceived"
QT_MOC_LITERAL(6, 89, 16), // "requestCompleted"
QT_MOC_LITERAL(7, 106, 6), // "status"
QT_MOC_LITERAL(8, 113, 18), // "binaryDataReceived"
QT_MOC_LITERAL(9, 132, 13) // "KviDataBuffer"

    },
    "KviStatusBarUpdateIndicator\0updateDisplay\0"
    "\0toggleStartup\0toggleRevision\0"
    "responseReceived\0requestCompleted\0"
    "status\0binaryDataReceived\0KviDataBuffer"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviStatusBarUpdateIndicator[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x09 /* Protected */,
       3,    0,   45,    2, 0x09 /* Protected */,
       4,    0,   46,    2, 0x09 /* Protected */,
       5,    1,   47,    2, 0x09 /* Protected */,
       6,    1,   50,    2, 0x09 /* Protected */,
       8,    1,   53,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, 0x80000000 | 9,    2,

       0        // eod
};

void KviStatusBarUpdateIndicator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviStatusBarUpdateIndicator *_t = static_cast<KviStatusBarUpdateIndicator *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateDisplay(); break;
        case 1: _t->toggleStartup(); break;
        case 2: _t->toggleRevision(); break;
        case 3: _t->responseReceived((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->requestCompleted((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->binaryDataReceived((*reinterpret_cast< const KviDataBuffer(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject KviStatusBarUpdateIndicator::staticMetaObject = {
    { &KviStatusBarApplet::staticMetaObject, qt_meta_stringdata_KviStatusBarUpdateIndicator.data,
      qt_meta_data_KviStatusBarUpdateIndicator,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviStatusBarUpdateIndicator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviStatusBarUpdateIndicator::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviStatusBarUpdateIndicator.stringdata0))
        return static_cast<void*>(this);
    return KviStatusBarApplet::qt_metacast(_clname);
}

int KviStatusBarUpdateIndicator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviStatusBarApplet::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
