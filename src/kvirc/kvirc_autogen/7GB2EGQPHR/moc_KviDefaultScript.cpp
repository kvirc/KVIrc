/****************************************************************************
** Meta object code from reading C++ file 'KviDefaultScript.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../kernel/KviDefaultScript.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KviDefaultScript.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviDefaultScriptManager_t {
    QByteArrayData data[1];
    char stringdata0[24];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviDefaultScriptManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviDefaultScriptManager_t qt_meta_stringdata_KviDefaultScriptManager = {
    {
QT_MOC_LITERAL(0, 0, 23) // "KviDefaultScriptManager"

    },
    "KviDefaultScriptManager"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviDefaultScriptManager[] = {

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

void KviDefaultScriptManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject KviDefaultScriptManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_KviDefaultScriptManager.data,
      qt_meta_data_KviDefaultScriptManager,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviDefaultScriptManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviDefaultScriptManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviDefaultScriptManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int KviDefaultScriptManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_KviDefaultScriptDialog_t {
    QByteArrayData data[6];
    char stringdata0[57];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviDefaultScriptDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviDefaultScriptDialog_t qt_meta_stringdata_KviDefaultScriptDialog = {
    {
QT_MOC_LITERAL(0, 0, 22), // "KviDefaultScriptDialog"
QT_MOC_LITERAL(1, 23, 9), // "toggleAll"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 8), // "advanced"
QT_MOC_LITERAL(4, 43, 6), // "accept"
QT_MOC_LITERAL(5, 50, 6) // "reject"

    },
    "KviDefaultScriptDialog\0toggleAll\0\0"
    "advanced\0accept\0reject"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviDefaultScriptDialog[] = {

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
       1,    1,   34,    2, 0x09 /* Protected */,
       3,    0,   37,    2, 0x09 /* Protected */,
       4,    0,   38,    2, 0x09 /* Protected */,
       5,    0,   39,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void KviDefaultScriptDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviDefaultScriptDialog *_t = static_cast<KviDefaultScriptDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->toggleAll((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->advanced(); break;
        case 2: _t->accept(); break;
        case 3: _t->reject(); break;
        default: ;
        }
    }
}

const QMetaObject KviDefaultScriptDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_KviDefaultScriptDialog.data,
      qt_meta_data_KviDefaultScriptDialog,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviDefaultScriptDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviDefaultScriptDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviDefaultScriptDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int KviDefaultScriptDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
