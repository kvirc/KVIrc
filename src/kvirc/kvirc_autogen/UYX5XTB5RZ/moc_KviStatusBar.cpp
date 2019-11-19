/****************************************************************************
** Meta object code from reading C++ file 'KviStatusBar.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ui/KviStatusBar.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KviStatusBar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviStatusBar_t {
    QByteArrayData data[12];
    char stringdata0[184];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviStatusBar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviStatusBar_t qt_meta_stringdata_KviStatusBar = {
    {
QT_MOC_LITERAL(0, 0, 12), // "KviStatusBar"
QT_MOC_LITERAL(1, 13, 17), // "messageTimerFired"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 20), // "contextMenuRequested"
QT_MOC_LITERAL(4, 53, 3), // "pos"
QT_MOC_LITERAL(5, 57, 23), // "contextPopupAboutToShow"
QT_MOC_LITERAL(6, 81, 23), // "appletsPopupAboutToShow"
QT_MOC_LITERAL(7, 105, 21), // "appletsPopupActivated"
QT_MOC_LITERAL(8, 127, 8), // "QAction*"
QT_MOC_LITERAL(9, 136, 7), // "pAction"
QT_MOC_LITERAL(10, 144, 19), // "removeClickedApplet"
QT_MOC_LITERAL(11, 164, 19) // "setPermanentMessage"

    },
    "KviStatusBar\0messageTimerFired\0\0"
    "contextMenuRequested\0pos\0"
    "contextPopupAboutToShow\0appletsPopupAboutToShow\0"
    "appletsPopupActivated\0QAction*\0pAction\0"
    "removeClickedApplet\0setPermanentMessage"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviStatusBar[] = {

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
       1,    0,   49,    2, 0x09 /* Protected */,
       3,    1,   50,    2, 0x09 /* Protected */,
       5,    0,   53,    2, 0x09 /* Protected */,
       6,    0,   54,    2, 0x09 /* Protected */,
       7,    1,   55,    2, 0x09 /* Protected */,
      10,    0,   58,    2, 0x09 /* Protected */,
      11,    0,   59,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void KviStatusBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviStatusBar *_t = static_cast<KviStatusBar *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->messageTimerFired(); break;
        case 1: _t->contextMenuRequested((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 2: _t->contextPopupAboutToShow(); break;
        case 3: _t->appletsPopupAboutToShow(); break;
        case 4: _t->appletsPopupActivated((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 5: _t->removeClickedApplet(); break;
        case 6: _t->setPermanentMessage(); break;
        default: ;
        }
    }
}

const QMetaObject KviStatusBar::staticMetaObject = {
    { &QStatusBar::staticMetaObject, qt_meta_stringdata_KviStatusBar.data,
      qt_meta_data_KviStatusBar,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviStatusBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviStatusBar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviStatusBar.stringdata0))
        return static_cast<void*>(this);
    return QStatusBar::qt_metacast(_clname);
}

int KviStatusBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStatusBar::qt_metacall(_c, _id, _a);
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
