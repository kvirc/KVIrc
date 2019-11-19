/****************************************************************************
** Meta object code from reading C++ file 'ThemeManagementDialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ThemeManagementDialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ThemeManagementDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ThemeManagementDialog_t {
    QByteArrayData data[19];
    char stringdata0[264];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ThemeManagementDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ThemeManagementDialog_t qt_meta_stringdata_ThemeManagementDialog = {
    {
QT_MOC_LITERAL(0, 0, 21), // "ThemeManagementDialog"
QT_MOC_LITERAL(1, 22, 16), // "saveCurrentTheme"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 13), // "getMoreThemes"
QT_MOC_LITERAL(4, 54, 15), // "installFromFile"
QT_MOC_LITERAL(5, 70, 12), // "fillThemeBox"
QT_MOC_LITERAL(6, 83, 11), // "deleteTheme"
QT_MOC_LITERAL(7, 95, 12), // "closeClicked"
QT_MOC_LITERAL(8, 108, 9), // "packTheme"
QT_MOC_LITERAL(9, 118, 10), // "applyTheme"
QT_MOC_LITERAL(10, 129, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(11, 146, 17), // "applyCurrentTheme"
QT_MOC_LITERAL(12, 164, 20), // "enableDisableButtons"
QT_MOC_LITERAL(13, 185, 20), // "contextMenuRequested"
QT_MOC_LITERAL(14, 206, 3), // "pos"
QT_MOC_LITERAL(15, 210, 10), // "tipRequest"
QT_MOC_LITERAL(16, 221, 5), // "pItem"
QT_MOC_LITERAL(17, 227, 3), // "pnt"
QT_MOC_LITERAL(18, 231, 32) // "webThemeInterfaceDialogDestroyed"

    },
    "ThemeManagementDialog\0saveCurrentTheme\0"
    "\0getMoreThemes\0installFromFile\0"
    "fillThemeBox\0deleteTheme\0closeClicked\0"
    "packTheme\0applyTheme\0QListWidgetItem*\0"
    "applyCurrentTheme\0enableDisableButtons\0"
    "contextMenuRequested\0pos\0tipRequest\0"
    "pItem\0pnt\0webThemeInterfaceDialogDestroyed"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ThemeManagementDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   79,    2, 0x09 /* Protected */,
       3,    0,   80,    2, 0x09 /* Protected */,
       4,    0,   81,    2, 0x09 /* Protected */,
       5,    0,   82,    2, 0x09 /* Protected */,
       6,    0,   83,    2, 0x09 /* Protected */,
       7,    0,   84,    2, 0x09 /* Protected */,
       8,    0,   85,    2, 0x09 /* Protected */,
       9,    1,   86,    2, 0x09 /* Protected */,
      11,    0,   89,    2, 0x09 /* Protected */,
      12,    0,   90,    2, 0x09 /* Protected */,
      13,    1,   91,    2, 0x09 /* Protected */,
      15,    2,   94,    2, 0x09 /* Protected */,
      18,    0,   99,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 10,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,   14,
    QMetaType::Void, 0x80000000 | 10, QMetaType::QPoint,   16,   17,
    QMetaType::Void,

       0        // eod
};

void ThemeManagementDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ThemeManagementDialog *_t = static_cast<ThemeManagementDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->saveCurrentTheme(); break;
        case 1: _t->getMoreThemes(); break;
        case 2: _t->installFromFile(); break;
        case 3: _t->fillThemeBox(); break;
        case 4: _t->deleteTheme(); break;
        case 5: _t->closeClicked(); break;
        case 6: _t->packTheme(); break;
        case 7: _t->applyTheme((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 8: _t->applyCurrentTheme(); break;
        case 9: _t->enableDisableButtons(); break;
        case 10: _t->contextMenuRequested((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 11: _t->tipRequest((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2]))); break;
        case 12: _t->webThemeInterfaceDialogDestroyed(); break;
        default: ;
        }
    }
}

const QMetaObject ThemeManagementDialog::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ThemeManagementDialog.data,
      qt_meta_data_ThemeManagementDialog,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ThemeManagementDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ThemeManagementDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ThemeManagementDialog.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ThemeManagementDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
