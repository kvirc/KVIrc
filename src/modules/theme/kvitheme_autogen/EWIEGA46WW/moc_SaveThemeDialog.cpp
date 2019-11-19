/****************************************************************************
** Meta object code from reading C++ file 'SaveThemeDialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../SaveThemeDialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SaveThemeDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SaveThemeDialog_t {
    QByteArrayData data[5];
    char stringdata0[66];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SaveThemeDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SaveThemeDialog_t qt_meta_stringdata_SaveThemeDialog = {
    {
QT_MOC_LITERAL(0, 0, 15), // "SaveThemeDialog"
QT_MOC_LITERAL(1, 16, 14), // "makeScreenshot"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 21), // "imageSelectionChanged"
QT_MOC_LITERAL(4, 54, 11) // "szImagePath"

    },
    "SaveThemeDialog\0makeScreenshot\0\0"
    "imageSelectionChanged\0szImagePath"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SaveThemeDialog[] = {

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
       3,    1,   25,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    4,

       0        // eod
};

void SaveThemeDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SaveThemeDialog *_t = static_cast<SaveThemeDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->makeScreenshot(); break;
        case 1: _t->imageSelectionChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject SaveThemeDialog::staticMetaObject = {
    { &KviTalWizard::staticMetaObject, qt_meta_stringdata_SaveThemeDialog.data,
      qt_meta_data_SaveThemeDialog,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *SaveThemeDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SaveThemeDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SaveThemeDialog.stringdata0))
        return static_cast<void*>(this);
    return KviTalWizard::qt_metacast(_clname);
}

int SaveThemeDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviTalWizard::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
