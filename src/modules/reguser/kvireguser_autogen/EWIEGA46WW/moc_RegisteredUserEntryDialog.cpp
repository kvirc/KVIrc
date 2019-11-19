/****************************************************************************
** Meta object code from reading C++ file 'RegisteredUserEntryDialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../RegisteredUserEntryDialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RegisteredUserEntryDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_RegisteredUserPropertiesDialog_t {
    QByteArrayData data[5];
    char stringdata0[64];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RegisteredUserPropertiesDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RegisteredUserPropertiesDialog_t qt_meta_stringdata_RegisteredUserPropertiesDialog = {
    {
QT_MOC_LITERAL(0, 0, 30), // "RegisteredUserPropertiesDialog"
QT_MOC_LITERAL(1, 31, 9), // "okClicked"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 10), // "addClicked"
QT_MOC_LITERAL(4, 53, 10) // "delClicked"

    },
    "RegisteredUserPropertiesDialog\0okClicked\0"
    "\0addClicked\0delClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RegisteredUserPropertiesDialog[] = {

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
       4,    0,   31,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void RegisteredUserPropertiesDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RegisteredUserPropertiesDialog *_t = static_cast<RegisteredUserPropertiesDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->okClicked(); break;
        case 1: _t->addClicked(); break;
        case 2: _t->delClicked(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject RegisteredUserPropertiesDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_RegisteredUserPropertiesDialog.data,
      qt_meta_data_RegisteredUserPropertiesDialog,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *RegisteredUserPropertiesDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RegisteredUserPropertiesDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RegisteredUserPropertiesDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int RegisteredUserPropertiesDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_RegisteredUserMaskDialog_t {
    QByteArrayData data[3];
    char stringdata0[36];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RegisteredUserMaskDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RegisteredUserMaskDialog_t qt_meta_stringdata_RegisteredUserMaskDialog = {
    {
QT_MOC_LITERAL(0, 0, 24), // "RegisteredUserMaskDialog"
QT_MOC_LITERAL(1, 25, 9), // "okClicked"
QT_MOC_LITERAL(2, 35, 0) // ""

    },
    "RegisteredUserMaskDialog\0okClicked\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RegisteredUserMaskDialog[] = {

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

void RegisteredUserMaskDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RegisteredUserMaskDialog *_t = static_cast<RegisteredUserMaskDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->okClicked(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject RegisteredUserMaskDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_RegisteredUserMaskDialog.data,
      qt_meta_data_RegisteredUserMaskDialog,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *RegisteredUserMaskDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RegisteredUserMaskDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RegisteredUserMaskDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int RegisteredUserMaskDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_RegisteredUserEntryDialog_t {
    QByteArrayData data[10];
    char stringdata0[155];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RegisteredUserEntryDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RegisteredUserEntryDialog_t qt_meta_stringdata_RegisteredUserEntryDialog = {
    {
QT_MOC_LITERAL(0, 0, 25), // "RegisteredUserEntryDialog"
QT_MOC_LITERAL(1, 26, 9), // "okClicked"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 14), // "addMaskClicked"
QT_MOC_LITERAL(4, 52, 14), // "delMaskClicked"
QT_MOC_LITERAL(5, 67, 15), // "editMaskClicked"
QT_MOC_LITERAL(6, 83, 24), // "editAllPropertiesClicked"
QT_MOC_LITERAL(7, 108, 18), // "maskCurrentChanged"
QT_MOC_LITERAL(8, 127, 18), // "notifyCheckClicked"
QT_MOC_LITERAL(9, 146, 8) // "bChecked"

    },
    "RegisteredUserEntryDialog\0okClicked\0"
    "\0addMaskClicked\0delMaskClicked\0"
    "editMaskClicked\0editAllPropertiesClicked\0"
    "maskCurrentChanged\0notifyCheckClicked\0"
    "bChecked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RegisteredUserEntryDialog[] = {

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
       5,    0,   52,    2, 0x09 /* Protected */,
       6,    0,   53,    2, 0x09 /* Protected */,
       7,    0,   54,    2, 0x09 /* Protected */,
       8,    1,   55,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    9,

       0        // eod
};

void RegisteredUserEntryDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RegisteredUserEntryDialog *_t = static_cast<RegisteredUserEntryDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->okClicked(); break;
        case 1: _t->addMaskClicked(); break;
        case 2: _t->delMaskClicked(); break;
        case 3: _t->editMaskClicked(); break;
        case 4: _t->editAllPropertiesClicked(); break;
        case 5: _t->maskCurrentChanged(); break;
        case 6: _t->notifyCheckClicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject RegisteredUserEntryDialog::staticMetaObject = {
    { &KviTalTabDialog::staticMetaObject, qt_meta_stringdata_RegisteredUserEntryDialog.data,
      qt_meta_data_RegisteredUserEntryDialog,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *RegisteredUserEntryDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RegisteredUserEntryDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RegisteredUserEntryDialog.stringdata0))
        return static_cast<void*>(this);
    return KviTalTabDialog::qt_metacast(_clname);
}

int RegisteredUserEntryDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviTalTabDialog::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
