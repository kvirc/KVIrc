/****************************************************************************
** Meta object code from reading C++ file 'KviIrcToolBar.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ui/KviIrcToolBar.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KviIrcToolBar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviToolBarGraphicalApplet_t {
    QByteArrayData data[1];
    char stringdata0[26];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviToolBarGraphicalApplet_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviToolBarGraphicalApplet_t qt_meta_stringdata_KviToolBarGraphicalApplet = {
    {
QT_MOC_LITERAL(0, 0, 25) // "KviToolBarGraphicalApplet"

    },
    "KviToolBarGraphicalApplet"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviToolBarGraphicalApplet[] = {

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

void KviToolBarGraphicalApplet::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject KviToolBarGraphicalApplet::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_KviToolBarGraphicalApplet.data,
      qt_meta_data_KviToolBarGraphicalApplet,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviToolBarGraphicalApplet::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviToolBarGraphicalApplet::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviToolBarGraphicalApplet.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int KviToolBarGraphicalApplet::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_KviIrcContextDisplay_t {
    QByteArrayData data[5];
    char stringdata0[56];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviIrcContextDisplay_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviIrcContextDisplay_t qt_meta_stringdata_KviIrcContextDisplay = {
    {
QT_MOC_LITERAL(0, 0, 20), // "KviIrcContextDisplay"
QT_MOC_LITERAL(1, 21, 10), // "tipRequest"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 18), // "KviDynamicToolTip*"
QT_MOC_LITERAL(4, 52, 3) // "tip"

    },
    "KviIrcContextDisplay\0tipRequest\0\0"
    "KviDynamicToolTip*\0tip"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviIrcContextDisplay[] = {

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
       1,    2,   19,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::QPoint,    4,    2,

       0        // eod
};

void KviIrcContextDisplay::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviIrcContextDisplay *_t = static_cast<KviIrcContextDisplay *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->tipRequest((*reinterpret_cast< KviDynamicToolTip*(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject KviIrcContextDisplay::staticMetaObject = {
    { &KviToolBarGraphicalApplet::staticMetaObject, qt_meta_stringdata_KviIrcContextDisplay.data,
      qt_meta_data_KviIrcContextDisplay,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviIrcContextDisplay::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviIrcContextDisplay::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviIrcContextDisplay.stringdata0))
        return static_cast<void*>(this);
    return KviToolBarGraphicalApplet::qt_metacast(_clname);
}

int KviIrcContextDisplay::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviToolBarGraphicalApplet::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
