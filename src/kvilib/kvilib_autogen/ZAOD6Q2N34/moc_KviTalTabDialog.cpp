/****************************************************************************
** Meta object code from reading C++ file 'KviTalTabDialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../tal/KviTalTabDialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KviTalTabDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviTalTabDialog_t {
    QByteArrayData data[6];
    char stringdata0[80];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviTalTabDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviTalTabDialog_t qt_meta_stringdata_KviTalTabDialog = {
    {
QT_MOC_LITERAL(0, 0, 15), // "KviTalTabDialog"
QT_MOC_LITERAL(1, 16, 18), // "applyButtonPressed"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 19), // "cancelButtonPressed"
QT_MOC_LITERAL(4, 56, 9), // "okPressed"
QT_MOC_LITERAL(5, 66, 13) // "cancelPressed"

    },
    "KviTalTabDialog\0applyButtonPressed\0\0"
    "cancelButtonPressed\0okPressed\0"
    "cancelPressed"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviTalTabDialog[] = {

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
       1,    0,   34,    2, 0x06 /* Public */,
       3,    0,   35,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   36,    2, 0x09 /* Protected */,
       5,    0,   37,    2, 0x09 /* Protected */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void KviTalTabDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviTalTabDialog *_t = static_cast<KviTalTabDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->applyButtonPressed(); break;
        case 1: _t->cancelButtonPressed(); break;
        case 2: _t->okPressed(); break;
        case 3: _t->cancelPressed(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (KviTalTabDialog::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviTalTabDialog::applyButtonPressed)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (KviTalTabDialog::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviTalTabDialog::cancelButtonPressed)) {
                *result = 1;
                return;
            }
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject KviTalTabDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_KviTalTabDialog.data,
      qt_meta_data_KviTalTabDialog,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviTalTabDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviTalTabDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviTalTabDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int KviTalTabDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

// SIGNAL 0
void KviTalTabDialog::applyButtonPressed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void KviTalTabDialog::cancelButtonPressed()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
