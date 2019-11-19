/****************************************************************************
** Meta object code from reading C++ file 'OptionsWidget_textIcons.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../OptionsWidget_textIcons.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'OptionsWidget_textIcons.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_OptionsWidget_textIcons_t {
    QByteArrayData data[14];
    char stringdata0[191];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_OptionsWidget_textIcons_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_OptionsWidget_textIcons_t qt_meta_stringdata_OptionsWidget_textIcons = {
    {
QT_MOC_LITERAL(0, 0, 23), // "OptionsWidget_textIcons"
QT_MOC_LITERAL(1, 24, 20), // "itemSelectionChanged"
QT_MOC_LITERAL(2, 45, 0), // ""
QT_MOC_LITERAL(3, 46, 18), // "currentItemChanged"
QT_MOC_LITERAL(4, 65, 17), // "QTableWidgetItem*"
QT_MOC_LITERAL(5, 83, 3), // "cur"
QT_MOC_LITERAL(6, 87, 4), // "prev"
QT_MOC_LITERAL(7, 92, 10), // "addClicked"
QT_MOC_LITERAL(8, 103, 10), // "delClicked"
QT_MOC_LITERAL(9, 114, 14), // "restoreClicked"
QT_MOC_LITERAL(10, 129, 12), // "iconSelected"
QT_MOC_LITERAL(11, 142, 25), // "KviIconManager::SmallIcon"
QT_MOC_LITERAL(12, 168, 7), // "doPopup"
QT_MOC_LITERAL(13, 176, 14) // "chooseFromFile"

    },
    "OptionsWidget_textIcons\0itemSelectionChanged\0"
    "\0currentItemChanged\0QTableWidgetItem*\0"
    "cur\0prev\0addClicked\0delClicked\0"
    "restoreClicked\0iconSelected\0"
    "KviIconManager::SmallIcon\0doPopup\0"
    "chooseFromFile"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_OptionsWidget_textIcons[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x09 /* Protected */,
       3,    2,   55,    2, 0x09 /* Protected */,
       7,    0,   60,    2, 0x09 /* Protected */,
       8,    0,   61,    2, 0x09 /* Protected */,
       9,    0,   62,    2, 0x09 /* Protected */,
      10,    1,   63,    2, 0x09 /* Protected */,
      12,    0,   66,    2, 0x09 /* Protected */,
      13,    0,   67,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4, 0x80000000 | 4,    5,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 11,    2,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void OptionsWidget_textIcons::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        OptionsWidget_textIcons *_t = static_cast<OptionsWidget_textIcons *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->itemSelectionChanged(); break;
        case 1: _t->currentItemChanged((*reinterpret_cast< QTableWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QTableWidgetItem*(*)>(_a[2]))); break;
        case 2: _t->addClicked(); break;
        case 3: _t->delClicked(); break;
        case 4: _t->restoreClicked(); break;
        case 5: _t->iconSelected((*reinterpret_cast< KviIconManager::SmallIcon(*)>(_a[1]))); break;
        case 6: _t->doPopup(); break;
        case 7: _t->chooseFromFile(); break;
        default: ;
        }
    }
}

const QMetaObject OptionsWidget_textIcons::staticMetaObject = {
    { &KviOptionsWidget::staticMetaObject, qt_meta_stringdata_OptionsWidget_textIcons.data,
      qt_meta_data_OptionsWidget_textIcons,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *OptionsWidget_textIcons::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OptionsWidget_textIcons::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_OptionsWidget_textIcons.stringdata0))
        return static_cast<void*>(this);
    return KviOptionsWidget::qt_metacast(_clname);
}

int OptionsWidget_textIcons::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviOptionsWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
