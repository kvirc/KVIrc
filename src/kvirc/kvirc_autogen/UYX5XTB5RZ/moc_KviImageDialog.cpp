/****************************************************************************
** Meta object code from reading C++ file 'KviImageDialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ui/KviImageDialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KviImageDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviImageDialog_t {
    QByteArrayData data[13];
    char stringdata0[144];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviImageDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviImageDialog_t qt_meta_stringdata_KviImageDialog = {
    {
QT_MOC_LITERAL(0, 0, 14), // "KviImageDialog"
QT_MOC_LITERAL(1, 15, 9), // "okClicked"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 13), // "cancelClicked"
QT_MOC_LITERAL(4, 40, 9), // "heartbeat"
QT_MOC_LITERAL(5, 50, 15), // "jobTypeSelected"
QT_MOC_LITERAL(6, 66, 5), // "index"
QT_MOC_LITERAL(7, 72, 17), // "itemDoubleClicked"
QT_MOC_LITERAL(8, 90, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(9, 107, 2), // "it"
QT_MOC_LITERAL(10, 110, 10), // "tipRequest"
QT_MOC_LITERAL(11, 121, 18), // "KviDynamicToolTip*"
QT_MOC_LITERAL(12, 140, 3) // "pnt"

    },
    "KviImageDialog\0okClicked\0\0cancelClicked\0"
    "heartbeat\0jobTypeSelected\0index\0"
    "itemDoubleClicked\0QListWidgetItem*\0"
    "it\0tipRequest\0KviDynamicToolTip*\0pnt"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviImageDialog[] = {

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
       7,    1,   50,    2, 0x09 /* Protected */,
      10,    2,   53,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, 0x80000000 | 11, QMetaType::QPoint,    2,   12,

       0        // eod
};

void KviImageDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviImageDialog *_t = static_cast<KviImageDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->okClicked(); break;
        case 1: _t->cancelClicked(); break;
        case 2: _t->heartbeat(); break;
        case 3: _t->jobTypeSelected((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->itemDoubleClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 5: _t->tipRequest((*reinterpret_cast< KviDynamicToolTip*(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< KviDynamicToolTip* >(); break;
            }
            break;
        }
    }
}

const QMetaObject KviImageDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_KviImageDialog.data,
      qt_meta_data_KviImageDialog,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviImageDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviImageDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviImageDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int KviImageDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
