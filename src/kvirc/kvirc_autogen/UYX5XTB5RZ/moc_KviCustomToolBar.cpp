/****************************************************************************
** Meta object code from reading C++ file 'KviCustomToolBar.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ui/KviCustomToolBar.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KviCustomToolBar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviCustomToolBar_t {
    QByteArrayData data[5];
    char stringdata0[69];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviCustomToolBar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviCustomToolBar_t qt_meta_stringdata_KviCustomToolBar = {
    {
QT_MOC_LITERAL(0, 0, 16), // "KviCustomToolBar"
QT_MOC_LITERAL(1, 17, 14), // "beginCustomize"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 12), // "endCustomize"
QT_MOC_LITERAL(4, 46, 22) // "filteredChildDestroyed"

    },
    "KviCustomToolBar\0beginCustomize\0\0"
    "endCustomize\0filteredChildDestroyed"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviCustomToolBar[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x09 /* Protected */,
       3,    0,   30,    2, 0x09 /* Protected */,
       4,    0,   31,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void KviCustomToolBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviCustomToolBar *_t = static_cast<KviCustomToolBar *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->beginCustomize(); break;
        case 1: _t->endCustomize(); break;
        case 2: _t->filteredChildDestroyed(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject KviCustomToolBar::staticMetaObject = {
    { &KviToolBar::staticMetaObject, qt_meta_stringdata_KviCustomToolBar.data,
      qt_meta_data_KviCustomToolBar,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviCustomToolBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviCustomToolBar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviCustomToolBar.stringdata0))
        return static_cast<void*>(this);
    return KviToolBar::qt_metacast(_clname);
}

int KviCustomToolBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviToolBar::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
struct qt_meta_stringdata_KviCustomToolBarSeparator_t {
    QByteArrayData data[1];
    char stringdata0[26];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviCustomToolBarSeparator_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviCustomToolBarSeparator_t qt_meta_stringdata_KviCustomToolBarSeparator = {
    {
QT_MOC_LITERAL(0, 0, 25) // "KviCustomToolBarSeparator"

    },
    "KviCustomToolBarSeparator"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviCustomToolBarSeparator[] = {

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

void KviCustomToolBarSeparator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject KviCustomToolBarSeparator::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_KviCustomToolBarSeparator.data,
      qt_meta_data_KviCustomToolBarSeparator,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviCustomToolBarSeparator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviCustomToolBarSeparator::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviCustomToolBarSeparator.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int KviCustomToolBarSeparator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
