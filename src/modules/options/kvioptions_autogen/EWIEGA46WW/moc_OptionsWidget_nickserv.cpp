/****************************************************************************
** Meta object code from reading C++ file 'OptionsWidget_nickserv.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../OptionsWidget_nickserv.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'OptionsWidget_nickserv.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_NickServRuleEditor_t {
    QByteArrayData data[3];
    char stringdata0[30];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_NickServRuleEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_NickServRuleEditor_t qt_meta_stringdata_NickServRuleEditor = {
    {
QT_MOC_LITERAL(0, 0, 18), // "NickServRuleEditor"
QT_MOC_LITERAL(1, 19, 9), // "okPressed"
QT_MOC_LITERAL(2, 29, 0) // ""

    },
    "NickServRuleEditor\0okPressed\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_NickServRuleEditor[] = {

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

void NickServRuleEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        NickServRuleEditor *_t = static_cast<NickServRuleEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->okPressed(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject NickServRuleEditor::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_NickServRuleEditor.data,
      qt_meta_data_NickServRuleEditor,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *NickServRuleEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NickServRuleEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_NickServRuleEditor.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int NickServRuleEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
struct qt_meta_stringdata_OptionsWidget_nickServ_t {
    QByteArrayData data[6];
    char stringdata0[103];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_OptionsWidget_nickServ_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_OptionsWidget_nickServ_t qt_meta_stringdata_OptionsWidget_nickServ = {
    {
QT_MOC_LITERAL(0, 0, 22), // "OptionsWidget_nickServ"
QT_MOC_LITERAL(1, 23, 29), // "enableDisableNickServControls"
QT_MOC_LITERAL(2, 53, 0), // ""
QT_MOC_LITERAL(3, 54, 15), // "addNickServRule"
QT_MOC_LITERAL(4, 70, 15), // "delNickServRule"
QT_MOC_LITERAL(5, 86, 16) // "editNickServRule"

    },
    "OptionsWidget_nickServ\0"
    "enableDisableNickServControls\0\0"
    "addNickServRule\0delNickServRule\0"
    "editNickServRule"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_OptionsWidget_nickServ[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x09 /* Protected */,
       3,    0,   35,    2, 0x09 /* Protected */,
       4,    0,   36,    2, 0x09 /* Protected */,
       5,    0,   37,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void OptionsWidget_nickServ::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        OptionsWidget_nickServ *_t = static_cast<OptionsWidget_nickServ *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->enableDisableNickServControls(); break;
        case 1: _t->addNickServRule(); break;
        case 2: _t->delNickServRule(); break;
        case 3: _t->editNickServRule(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject OptionsWidget_nickServ::staticMetaObject = {
    { &KviOptionsWidget::staticMetaObject, qt_meta_stringdata_OptionsWidget_nickServ.data,
      qt_meta_data_OptionsWidget_nickServ,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *OptionsWidget_nickServ::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OptionsWidget_nickServ::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_OptionsWidget_nickServ.stringdata0))
        return static_cast<void*>(this);
    return KviOptionsWidget::qt_metacast(_clname);
}

int OptionsWidget_nickServ::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviOptionsWidget::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
