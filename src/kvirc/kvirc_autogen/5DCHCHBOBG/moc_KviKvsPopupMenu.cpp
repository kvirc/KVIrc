/****************************************************************************
** Meta object code from reading C++ file 'KviKvsPopupMenu.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../kvs/KviKvsPopupMenu.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KviKvsPopupMenu.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviKvsPopupMenuItemLabelHelper_t {
    QByteArrayData data[3];
    char stringdata0[47];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviKvsPopupMenuItemLabelHelper_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviKvsPopupMenuItemLabelHelper_t qt_meta_stringdata_KviKvsPopupMenuItemLabelHelper = {
    {
QT_MOC_LITERAL(0, 0, 30), // "KviKvsPopupMenuItemLabelHelper"
QT_MOC_LITERAL(1, 31, 14), // "labelDestroyed"
QT_MOC_LITERAL(2, 46, 0) // ""

    },
    "KviKvsPopupMenuItemLabelHelper\0"
    "labelDestroyed\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviKvsPopupMenuItemLabelHelper[] = {

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

void KviKvsPopupMenuItemLabelHelper::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviKvsPopupMenuItemLabelHelper *_t = static_cast<KviKvsPopupMenuItemLabelHelper *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->labelDestroyed(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject KviKvsPopupMenuItemLabelHelper::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_KviKvsPopupMenuItemLabelHelper.data,
      qt_meta_data_KviKvsPopupMenuItemLabelHelper,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviKvsPopupMenuItemLabelHelper::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviKvsPopupMenuItemLabelHelper::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviKvsPopupMenuItemLabelHelper.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int KviKvsPopupMenuItemLabelHelper::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_KviKvsPopupMenu_t {
    QByteArrayData data[9];
    char stringdata0[108];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviKvsPopupMenu_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviKvsPopupMenu_t qt_meta_stringdata_KviKvsPopupMenu = {
    {
QT_MOC_LITERAL(0, 0, 15), // "KviKvsPopupMenu"
QT_MOC_LITERAL(1, 16, 19), // "testModeItemClicked"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 20), // "KviKvsPopupMenuItem*"
QT_MOC_LITERAL(4, 58, 2), // "it"
QT_MOC_LITERAL(5, 61, 17), // "setupMenuContents"
QT_MOC_LITERAL(6, 79, 11), // "itemClicked"
QT_MOC_LITERAL(7, 91, 8), // "QAction*"
QT_MOC_LITERAL(8, 100, 7) // "pAction"

    },
    "KviKvsPopupMenu\0testModeItemClicked\0"
    "\0KviKvsPopupMenuItem*\0it\0setupMenuContents\0"
    "itemClicked\0QAction*\0pAction"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviKvsPopupMenu[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   32,    2, 0x08 /* Private */,
       6,    1,   33,    2, 0x09 /* Protected */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 7,    8,

       0        // eod
};

void KviKvsPopupMenu::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviKvsPopupMenu *_t = static_cast<KviKvsPopupMenu *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->testModeItemClicked((*reinterpret_cast< KviKvsPopupMenuItem*(*)>(_a[1]))); break;
        case 1: _t->setupMenuContents(); break;
        case 2: _t->itemClicked((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAction* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (KviKvsPopupMenu::*_t)(KviKvsPopupMenuItem * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviKvsPopupMenu::testModeItemClicked)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject KviKvsPopupMenu::staticMetaObject = {
    { &QMenu::staticMetaObject, qt_meta_stringdata_KviKvsPopupMenu.data,
      qt_meta_data_KviKvsPopupMenu,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviKvsPopupMenu::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviKvsPopupMenu::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviKvsPopupMenu.stringdata0))
        return static_cast<void*>(this);
    return QMenu::qt_metacast(_clname);
}

int KviKvsPopupMenu::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMenu::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void KviKvsPopupMenu::testModeItemClicked(KviKvsPopupMenuItem * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
