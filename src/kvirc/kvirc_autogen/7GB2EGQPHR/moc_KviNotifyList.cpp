/****************************************************************************
** Meta object code from reading C++ file 'KviNotifyList.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../kernel/KviNotifyList.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KviNotifyList.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviNotifyListManager_t {
    QByteArrayData data[1];
    char stringdata0[21];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviNotifyListManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviNotifyListManager_t qt_meta_stringdata_KviNotifyListManager = {
    {
QT_MOC_LITERAL(0, 0, 20) // "KviNotifyListManager"

    },
    "KviNotifyListManager"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviNotifyListManager[] = {

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

void KviNotifyListManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject KviNotifyListManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_KviNotifyListManager.data,
      qt_meta_data_KviNotifyListManager,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviNotifyListManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviNotifyListManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviNotifyListManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int KviNotifyListManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_KviIsOnNotifyListManager_t {
    QByteArrayData data[15];
    char stringdata0[205];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviIsOnNotifyListManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviIsOnNotifyListManager_t qt_meta_stringdata_KviIsOnNotifyListManager = {
    {
QT_MOC_LITERAL(0, 0, 24), // "KviIsOnNotifyListManager"
QT_MOC_LITERAL(1, 25, 16), // "newNotifySession"
QT_MOC_LITERAL(2, 42, 0), // ""
QT_MOC_LITERAL(3, 43, 14), // "newIsOnSession"
QT_MOC_LITERAL(4, 58, 18), // "newUserhostSession"
QT_MOC_LITERAL(5, 77, 16), // "buildRegUserDict"
QT_MOC_LITERAL(6, 94, 13), // "buildIsOnList"
QT_MOC_LITERAL(7, 108, 17), // "buildUserhostList"
QT_MOC_LITERAL(8, 126, 15), // "buildNotifyList"
QT_MOC_LITERAL(9, 142, 8), // "sendIsOn"
QT_MOC_LITERAL(10, 151, 12), // "sendUserhost"
QT_MOC_LITERAL(11, 164, 11), // "doMatchUser"
QT_MOC_LITERAL(12, 176, 12), // "notifyString"
QT_MOC_LITERAL(13, 189, 10), // "KviIrcMask"
QT_MOC_LITERAL(14, 200, 4) // "mask"

    },
    "KviIsOnNotifyListManager\0newNotifySession\0"
    "\0newIsOnSession\0newUserhostSession\0"
    "buildRegUserDict\0buildIsOnList\0"
    "buildUserhostList\0buildNotifyList\0"
    "sendIsOn\0sendUserhost\0doMatchUser\0"
    "notifyString\0KviIrcMask\0mask"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviIsOnNotifyListManager[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x08 /* Private */,
       3,    0,   65,    2, 0x08 /* Private */,
       4,    0,   66,    2, 0x08 /* Private */,
       5,    0,   67,    2, 0x08 /* Private */,
       6,    0,   68,    2, 0x08 /* Private */,
       7,    0,   69,    2, 0x08 /* Private */,
       8,    0,   70,    2, 0x08 /* Private */,
       9,    0,   71,    2, 0x08 /* Private */,
      10,    0,   72,    2, 0x08 /* Private */,
      11,    2,   73,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Bool, QMetaType::QString, 0x80000000 | 13,   12,   14,

       0        // eod
};

void KviIsOnNotifyListManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviIsOnNotifyListManager *_t = static_cast<KviIsOnNotifyListManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->newNotifySession(); break;
        case 1: _t->newIsOnSession(); break;
        case 2: _t->newUserhostSession(); break;
        case 3: _t->buildRegUserDict(); break;
        case 4: _t->buildIsOnList(); break;
        case 5: _t->buildUserhostList(); break;
        case 6: _t->buildNotifyList(); break;
        case 7: _t->sendIsOn(); break;
        case 8: _t->sendUserhost(); break;
        case 9: { bool _r = _t->doMatchUser((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const KviIrcMask(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
}

const QMetaObject KviIsOnNotifyListManager::staticMetaObject = {
    { &KviNotifyListManager::staticMetaObject, qt_meta_stringdata_KviIsOnNotifyListManager.data,
      qt_meta_data_KviIsOnNotifyListManager,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviIsOnNotifyListManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviIsOnNotifyListManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviIsOnNotifyListManager.stringdata0))
        return static_cast<void*>(this);
    return KviNotifyListManager::qt_metacast(_clname);
}

int KviIsOnNotifyListManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviNotifyListManager::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}
struct qt_meta_stringdata_KviStupidNotifyListManager_t {
    QByteArrayData data[1];
    char stringdata0[27];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviStupidNotifyListManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviStupidNotifyListManager_t qt_meta_stringdata_KviStupidNotifyListManager = {
    {
QT_MOC_LITERAL(0, 0, 26) // "KviStupidNotifyListManager"

    },
    "KviStupidNotifyListManager"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviStupidNotifyListManager[] = {

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

void KviStupidNotifyListManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject KviStupidNotifyListManager::staticMetaObject = {
    { &KviNotifyListManager::staticMetaObject, qt_meta_stringdata_KviStupidNotifyListManager.data,
      qt_meta_data_KviStupidNotifyListManager,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviStupidNotifyListManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviStupidNotifyListManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviStupidNotifyListManager.stringdata0))
        return static_cast<void*>(this);
    return KviNotifyListManager::qt_metacast(_clname);
}

int KviStupidNotifyListManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviNotifyListManager::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_KviWatchNotifyListManager_t {
    QByteArrayData data[1];
    char stringdata0[26];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviWatchNotifyListManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviWatchNotifyListManager_t qt_meta_stringdata_KviWatchNotifyListManager = {
    {
QT_MOC_LITERAL(0, 0, 25) // "KviWatchNotifyListManager"

    },
    "KviWatchNotifyListManager"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviWatchNotifyListManager[] = {

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

void KviWatchNotifyListManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject KviWatchNotifyListManager::staticMetaObject = {
    { &KviNotifyListManager::staticMetaObject, qt_meta_stringdata_KviWatchNotifyListManager.data,
      qt_meta_data_KviWatchNotifyListManager,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviWatchNotifyListManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviWatchNotifyListManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviWatchNotifyListManager.stringdata0))
        return static_cast<void*>(this);
    return KviNotifyListManager::qt_metacast(_clname);
}

int KviWatchNotifyListManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviNotifyListManager::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
