/****************************************************************************
** Meta object code from reading C++ file 'libkvitrayicon.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../libkvitrayicon.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'libkvitrayicon.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviTrayIconWidget_t {
    QByteArrayData data[11];
    char stringdata0[173];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviTrayIconWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviTrayIconWidget_t qt_meta_stringdata_KviTrayIconWidget = {
    {
QT_MOC_LITERAL(0, 0, 17), // "KviTrayIconWidget"
QT_MOC_LITERAL(1, 18, 16), // "fillContextPopup"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 17), // "toggleParentFrame"
QT_MOC_LITERAL(4, 54, 6), // "doAway"
QT_MOC_LITERAL(5, 61, 17), // "flashingTimerShot"
QT_MOC_LITERAL(6, 79, 13), // "activatedSlot"
QT_MOC_LITERAL(7, 93, 33), // "QSystemTrayIcon::ActivationRe..."
QT_MOC_LITERAL(8, 127, 6), // "reason"
QT_MOC_LITERAL(9, 134, 22), // "executeInternalCommand"
QT_MOC_LITERAL(10, 157, 15) // "disableTrayIcon"

    },
    "KviTrayIconWidget\0fillContextPopup\0\0"
    "toggleParentFrame\0doAway\0flashingTimerShot\0"
    "activatedSlot\0QSystemTrayIcon::ActivationReason\0"
    "reason\0executeInternalCommand\0"
    "disableTrayIcon"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviTrayIconWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x08 /* Private */,
       3,    0,   50,    2, 0x08 /* Private */,
       4,    1,   51,    2, 0x08 /* Private */,
       5,    0,   54,    2, 0x08 /* Private */,
       6,    1,   55,    2, 0x08 /* Private */,
       9,    1,   58,    2, 0x08 /* Private */,
      10,    0,   61,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,

       0        // eod
};

void KviTrayIconWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviTrayIconWidget *_t = static_cast<KviTrayIconWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->fillContextPopup(); break;
        case 1: _t->toggleParentFrame(); break;
        case 2: _t->doAway((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->flashingTimerShot(); break;
        case 4: _t->activatedSlot((*reinterpret_cast< QSystemTrayIcon::ActivationReason(*)>(_a[1]))); break;
        case 5: _t->executeInternalCommand((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->disableTrayIcon(); break;
        default: ;
        }
    }
}

const QMetaObject KviTrayIconWidget::staticMetaObject = {
    { &QSystemTrayIcon::staticMetaObject, qt_meta_stringdata_KviTrayIconWidget.data,
      qt_meta_data_KviTrayIconWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviTrayIconWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviTrayIconWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviTrayIconWidget.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "KviTrayIcon"))
        return static_cast< KviTrayIcon*>(this);
    return QSystemTrayIcon::qt_metacast(_clname);
}

int KviTrayIconWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSystemTrayIcon::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
