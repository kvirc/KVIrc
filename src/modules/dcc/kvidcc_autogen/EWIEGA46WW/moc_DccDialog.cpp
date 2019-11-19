/****************************************************************************
** Meta object code from reading C++ file 'DccDialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../DccDialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DccDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DccAcceptDialog_t {
    QByteArrayData data[8];
    char stringdata0[89];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DccAcceptDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DccAcceptDialog_t qt_meta_stringdata_DccAcceptDialog = {
    {
QT_MOC_LITERAL(0, 0, 15), // "DccAcceptDialog"
QT_MOC_LITERAL(1, 16, 8), // "accepted"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 10), // "DccDialog*"
QT_MOC_LITERAL(4, 37, 14), // "DccDescriptor*"
QT_MOC_LITERAL(5, 52, 8), // "rejected"
QT_MOC_LITERAL(6, 61, 13), // "acceptClicked"
QT_MOC_LITERAL(7, 75, 13) // "rejectClicked"

    },
    "DccAcceptDialog\0accepted\0\0DccDialog*\0"
    "DccDescriptor*\0rejected\0acceptClicked\0"
    "rejectClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DccAcceptDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   34,    2, 0x06 /* Public */,
       5,    2,   39,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   44,    2, 0x08 /* Private */,
       7,    0,   45,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 4,    2,    2,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 4,    2,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void DccAcceptDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DccAcceptDialog *_t = static_cast<DccAcceptDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->accepted((*reinterpret_cast< DccDialog*(*)>(_a[1])),(*reinterpret_cast< DccDescriptor*(*)>(_a[2]))); break;
        case 1: _t->rejected((*reinterpret_cast< DccDialog*(*)>(_a[1])),(*reinterpret_cast< DccDescriptor*(*)>(_a[2]))); break;
        case 2: _t->acceptClicked(); break;
        case 3: _t->rejectClicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (DccAcceptDialog::*_t)(DccDialog * , DccDescriptor * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DccAcceptDialog::accepted)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (DccAcceptDialog::*_t)(DccDialog * , DccDescriptor * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DccAcceptDialog::rejected)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject DccAcceptDialog::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_DccAcceptDialog.data,
      qt_meta_data_DccAcceptDialog,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *DccAcceptDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DccAcceptDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DccAcceptDialog.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "DccDialog"))
        return static_cast< DccDialog*>(this);
    return QWidget::qt_metacast(_clname);
}

int DccAcceptDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void DccAcceptDialog::accepted(DccDialog * _t1, DccDescriptor * _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DccAcceptDialog::rejected(DccDialog * _t1, DccDescriptor * _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
struct qt_meta_stringdata_DccRenameDialog_t {
    QByteArrayData data[11];
    char stringdata0[150];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DccRenameDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DccRenameDialog_t qt_meta_stringdata_DccRenameDialog = {
    {
QT_MOC_LITERAL(0, 0, 15), // "DccRenameDialog"
QT_MOC_LITERAL(1, 16, 17), // "overwriteSelected"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 10), // "DccDialog*"
QT_MOC_LITERAL(4, 46, 14), // "DccDescriptor*"
QT_MOC_LITERAL(5, 61, 14), // "renameSelected"
QT_MOC_LITERAL(6, 76, 14), // "cancelSelected"
QT_MOC_LITERAL(7, 91, 13), // "renameClicked"
QT_MOC_LITERAL(8, 105, 16), // "overwriteClicked"
QT_MOC_LITERAL(9, 122, 13), // "resumeClicked"
QT_MOC_LITERAL(10, 136, 13) // "cancelClicked"

    },
    "DccRenameDialog\0overwriteSelected\0\0"
    "DccDialog*\0DccDescriptor*\0renameSelected\0"
    "cancelSelected\0renameClicked\0"
    "overwriteClicked\0resumeClicked\0"
    "cancelClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DccRenameDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   49,    2, 0x06 /* Public */,
       5,    2,   54,    2, 0x06 /* Public */,
       6,    2,   59,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,   64,    2, 0x08 /* Private */,
       8,    0,   65,    2, 0x08 /* Private */,
       9,    0,   66,    2, 0x08 /* Private */,
      10,    0,   67,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 4,    2,    2,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 4,    2,    2,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 4,    2,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void DccRenameDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DccRenameDialog *_t = static_cast<DccRenameDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->overwriteSelected((*reinterpret_cast< DccDialog*(*)>(_a[1])),(*reinterpret_cast< DccDescriptor*(*)>(_a[2]))); break;
        case 1: _t->renameSelected((*reinterpret_cast< DccDialog*(*)>(_a[1])),(*reinterpret_cast< DccDescriptor*(*)>(_a[2]))); break;
        case 2: _t->cancelSelected((*reinterpret_cast< DccDialog*(*)>(_a[1])),(*reinterpret_cast< DccDescriptor*(*)>(_a[2]))); break;
        case 3: _t->renameClicked(); break;
        case 4: _t->overwriteClicked(); break;
        case 5: _t->resumeClicked(); break;
        case 6: _t->cancelClicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (DccRenameDialog::*_t)(DccDialog * , DccDescriptor * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DccRenameDialog::overwriteSelected)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (DccRenameDialog::*_t)(DccDialog * , DccDescriptor * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DccRenameDialog::renameSelected)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (DccRenameDialog::*_t)(DccDialog * , DccDescriptor * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DccRenameDialog::cancelSelected)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject DccRenameDialog::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_DccRenameDialog.data,
      qt_meta_data_DccRenameDialog,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *DccRenameDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DccRenameDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DccRenameDialog.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "DccDialog"))
        return static_cast< DccDialog*>(this);
    return QWidget::qt_metacast(_clname);
}

int DccRenameDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void DccRenameDialog::overwriteSelected(DccDialog * _t1, DccDescriptor * _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DccRenameDialog::renameSelected(DccDialog * _t1, DccDescriptor * _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void DccRenameDialog::cancelSelected(DccDialog * _t1, DccDescriptor * _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
