/****************************************************************************
** Meta object code from reading C++ file 'SetupWizard.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../SetupWizard.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SetupWizard.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SetupPage_t {
    QByteArrayData data[1];
    char stringdata0[10];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SetupPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SetupPage_t qt_meta_stringdata_SetupPage = {
    {
QT_MOC_LITERAL(0, 0, 9) // "SetupPage"

    },
    "SetupPage"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SetupPage[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void SetupPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject SetupPage::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_SetupPage.data,
      qt_meta_data_SetupPage,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *SetupPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SetupPage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SetupPage.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int SetupPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_SetupWizard_t {
    QByteArrayData data[14];
    char stringdata0[208];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SetupWizard_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SetupWizard_t qt_meta_stringdata_SetupWizard = {
    {
QT_MOC_LITERAL(0, 0, 11), // "SetupWizard"
QT_MOC_LITERAL(1, 12, 17), // "chooseOldDataPath"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 14), // "chooseDataPath"
QT_MOC_LITERAL(4, 46, 18), // "chooseIncomingPath"
QT_MOC_LITERAL(5, 65, 6), // "accept"
QT_MOC_LITERAL(6, 72, 6), // "reject"
QT_MOC_LITERAL(7, 79, 13), // "oldDirClicked"
QT_MOC_LITERAL(8, 93, 13), // "newDirClicked"
QT_MOC_LITERAL(9, 107, 15), // "portableClicked"
QT_MOC_LITERAL(10, 123, 23), // "nickSelectorTextChanged"
QT_MOC_LITERAL(11, 147, 18), // "oldDataTextChanged"
QT_MOC_LITERAL(12, 166, 18), // "newDataTextChanged"
QT_MOC_LITERAL(13, 185, 22) // "newIncomingTextChanged"

    },
    "SetupWizard\0chooseOldDataPath\0\0"
    "chooseDataPath\0chooseIncomingPath\0"
    "accept\0reject\0oldDirClicked\0newDirClicked\0"
    "portableClicked\0nickSelectorTextChanged\0"
    "oldDataTextChanged\0newDataTextChanged\0"
    "newIncomingTextChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SetupWizard[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   74,    2, 0x0a /* Public */,
       3,    0,   75,    2, 0x0a /* Public */,
       4,    0,   76,    2, 0x0a /* Public */,
       5,    0,   77,    2, 0x0a /* Public */,
       6,    0,   78,    2, 0x0a /* Public */,
       7,    0,   79,    2, 0x0a /* Public */,
       8,    0,   80,    2, 0x0a /* Public */,
       9,    0,   81,    2, 0x0a /* Public */,
      10,    1,   82,    2, 0x0a /* Public */,
      11,    1,   85,    2, 0x0a /* Public */,
      12,    1,   88,    2, 0x0a /* Public */,
      13,    1,   91,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,

       0        // eod
};

void SetupWizard::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SetupWizard *_t = static_cast<SetupWizard *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->chooseOldDataPath(); break;
        case 1: _t->chooseDataPath(); break;
        case 2: _t->chooseIncomingPath(); break;
        case 3: _t->accept(); break;
        case 4: _t->reject(); break;
        case 5: _t->oldDirClicked(); break;
        case 6: _t->newDirClicked(); break;
        case 7: _t->portableClicked(); break;
        case 8: _t->nickSelectorTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->oldDataTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: _t->newDataTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 11: _t->newIncomingTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject SetupWizard::staticMetaObject = {
    { &KviTalWizard::staticMetaObject, qt_meta_stringdata_SetupWizard.data,
      qt_meta_data_SetupWizard,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *SetupWizard::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SetupWizard::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SetupWizard.stringdata0))
        return static_cast<void*>(this);
    return KviTalWizard::qt_metacast(_clname);
}

int SetupWizard::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviTalWizard::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
