/****************************************************************************
** Meta object code from reading C++ file 'OptionsWidget_away.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../OptionsWidget_away.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'OptionsWidget_away.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_OptionsWidget_away_t {
    QByteArrayData data[4];
    char stringdata0[59];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_OptionsWidget_away_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_OptionsWidget_away_t qt_meta_stringdata_OptionsWidget_away = {
    {
QT_MOC_LITERAL(0, 0, 18), // "OptionsWidget_away"
QT_MOC_LITERAL(1, 19, 20), // "enableCustomAwayText"
QT_MOC_LITERAL(2, 40, 0), // ""
QT_MOC_LITERAL(3, 41, 17) // "enableAwayMessage"

    },
    "OptionsWidget_away\0enableCustomAwayText\0"
    "\0enableAwayMessage"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_OptionsWidget_away[] = {

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
       1,    1,   24,    2, 0x0a /* Public */,
       3,    1,   27,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,

       0        // eod
};

void OptionsWidget_away::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        OptionsWidget_away *_t = static_cast<OptionsWidget_away *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->enableCustomAwayText((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->enableAwayMessage((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject OptionsWidget_away::staticMetaObject = {
    { &KviOptionsWidget::staticMetaObject, qt_meta_stringdata_OptionsWidget_away.data,
      qt_meta_data_OptionsWidget_away,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *OptionsWidget_away::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OptionsWidget_away::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_OptionsWidget_away.stringdata0))
        return static_cast<void*>(this);
    return KviOptionsWidget::qt_metacast(_clname);
}

int OptionsWidget_away::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviOptionsWidget::qt_metacall(_c, _id, _a);
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
