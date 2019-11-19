/****************************************************************************
** Meta object code from reading C++ file 'KviInput.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ui/KviInput.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KviInput.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviInput_t {
    QByteArrayData data[9];
    char stringdata0[140];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviInput_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviInput_t qt_meta_stringdata_KviInput = {
    {
QT_MOC_LITERAL(0, 0, 8), // "KviInput"
QT_MOC_LITERAL(1, 9, 28), // "multiLineEditorButtonToggled"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 3), // "bOn"
QT_MOC_LITERAL(4, 43, 20), // "historyButtonClicked"
QT_MOC_LITERAL(5, 64, 17), // "iconButtonClicked"
QT_MOC_LITERAL(6, 82, 23), // "inputEditorEnterPressed"
QT_MOC_LITERAL(7, 106, 17), // "toggleToolButtons"
QT_MOC_LITERAL(8, 124, 15) // "toggleMultiLine"

    },
    "KviInput\0multiLineEditorButtonToggled\0"
    "\0bOn\0historyButtonClicked\0iconButtonClicked\0"
    "inputEditorEnterPressed\0toggleToolButtons\0"
    "toggleMultiLine"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviInput[] = {

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
       1,    1,   44,    2, 0x0a /* Public */,
       4,    0,   47,    2, 0x0a /* Public */,
       5,    0,   48,    2, 0x0a /* Public */,
       6,    0,   49,    2, 0x0a /* Public */,
       7,    0,   50,    2, 0x0a /* Public */,
       8,    0,   51,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void KviInput::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviInput *_t = static_cast<KviInput *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->multiLineEditorButtonToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->historyButtonClicked(); break;
        case 2: _t->iconButtonClicked(); break;
        case 3: _t->inputEditorEnterPressed(); break;
        case 4: _t->toggleToolButtons(); break;
        case 5: _t->toggleMultiLine(); break;
        default: ;
        }
    }
}

const QMetaObject KviInput::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_KviInput.data,
      qt_meta_data_KviInput,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviInput::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviInput::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviInput.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int KviInput::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
