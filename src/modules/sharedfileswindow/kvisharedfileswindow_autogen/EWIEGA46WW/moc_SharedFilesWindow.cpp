/****************************************************************************
** Meta object code from reading C++ file 'SharedFilesWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../SharedFilesWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SharedFilesWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SharedFileEditDialog_t {
    QByteArrayData data[4];
    char stringdata0[39];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SharedFileEditDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SharedFileEditDialog_t qt_meta_stringdata_SharedFileEditDialog = {
    {
QT_MOC_LITERAL(0, 0, 20), // "SharedFileEditDialog"
QT_MOC_LITERAL(1, 21, 9), // "okClicked"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 6) // "browse"

    },
    "SharedFileEditDialog\0okClicked\0\0browse"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SharedFileEditDialog[] = {

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

void SharedFileEditDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SharedFileEditDialog *_t = static_cast<SharedFileEditDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->okClicked(); break;
        case 1: _t->browse(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject SharedFileEditDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_SharedFileEditDialog.data,
      qt_meta_data_SharedFileEditDialog,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *SharedFileEditDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SharedFileEditDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SharedFileEditDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int SharedFileEditDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_SharedFilesWindow_t {
    QByteArrayData data[11];
    char stringdata0[134];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SharedFilesWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SharedFilesWindow_t qt_meta_stringdata_SharedFilesWindow = {
    {
QT_MOC_LITERAL(0, 0, 17), // "SharedFilesWindow"
QT_MOC_LITERAL(1, 18, 12), // "fillFileView"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 15), // "sharedFileAdded"
QT_MOC_LITERAL(4, 48, 14), // "KviSharedFile*"
QT_MOC_LITERAL(5, 63, 1), // "f"
QT_MOC_LITERAL(6, 65, 17), // "sharedFileRemoved"
QT_MOC_LITERAL(7, 83, 13), // "enableButtons"
QT_MOC_LITERAL(8, 97, 13), // "removeClicked"
QT_MOC_LITERAL(9, 111, 10), // "addClicked"
QT_MOC_LITERAL(10, 122, 11) // "editClicked"

    },
    "SharedFilesWindow\0fillFileView\0\0"
    "sharedFileAdded\0KviSharedFile*\0f\0"
    "sharedFileRemoved\0enableButtons\0"
    "removeClicked\0addClicked\0editClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SharedFilesWindow[] = {

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
       3,    1,   50,    2, 0x09 /* Protected */,
       6,    1,   53,    2, 0x09 /* Protected */,
       7,    0,   56,    2, 0x09 /* Protected */,
       8,    0,   57,    2, 0x09 /* Protected */,
       9,    0,   58,    2, 0x09 /* Protected */,
      10,    0,   59,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void SharedFilesWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SharedFilesWindow *_t = static_cast<SharedFilesWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->fillFileView(); break;
        case 1: _t->sharedFileAdded((*reinterpret_cast< KviSharedFile*(*)>(_a[1]))); break;
        case 2: _t->sharedFileRemoved((*reinterpret_cast< KviSharedFile*(*)>(_a[1]))); break;
        case 3: _t->enableButtons(); break;
        case 4: _t->removeClicked(); break;
        case 5: _t->addClicked(); break;
        case 6: _t->editClicked(); break;
        default: ;
        }
    }
}

const QMetaObject SharedFilesWindow::staticMetaObject = {
    { &KviWindow::staticMetaObject, qt_meta_stringdata_SharedFilesWindow.data,
      qt_meta_data_SharedFilesWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *SharedFilesWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SharedFilesWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SharedFilesWindow.stringdata0))
        return static_cast<void*>(this);
    return KviWindow::qt_metacast(_clname);
}

int SharedFilesWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviWindow::qt_metacall(_c, _id, _a);
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
