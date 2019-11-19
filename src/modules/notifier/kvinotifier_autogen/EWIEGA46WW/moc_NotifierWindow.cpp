/****************************************************************************
** Meta object code from reading C++ file 'NotifierWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../NotifierWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NotifierWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_NotifierWindow_t {
    QByteArrayData data[19];
    char stringdata0[282];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_NotifierWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_NotifierWindow_t qt_meta_stringdata_NotifierWindow = {
    {
QT_MOC_LITERAL(0, 0, 14), // "NotifierWindow"
QT_MOC_LITERAL(1, 15, 7), // "hideNow"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 14), // "toggleLineEdit"
QT_MOC_LITERAL(4, 39, 21), // "slotTabCloseRequested"
QT_MOC_LITERAL(5, 61, 5), // "index"
QT_MOC_LITERAL(6, 67, 5), // "blink"
QT_MOC_LITERAL(7, 73, 9), // "heartbeat"
QT_MOC_LITERAL(8, 83, 13), // "returnPressed"
QT_MOC_LITERAL(9, 97, 9), // "updateGui"
QT_MOC_LITERAL(10, 107, 16), // "fillContextPopup"
QT_MOC_LITERAL(11, 124, 17), // "disableFor1Minute"
QT_MOC_LITERAL(12, 142, 18), // "disableFor5Minutes"
QT_MOC_LITERAL(13, 161, 19), // "disableFor15Minutes"
QT_MOC_LITERAL(14, 181, 19), // "disableFor30Minutes"
QT_MOC_LITERAL(15, 201, 19), // "disableFor60Minutes"
QT_MOC_LITERAL(16, 221, 26), // "disableUntilKVIrcRestarted"
QT_MOC_LITERAL(17, 248, 18), // "disablePermanently"
QT_MOC_LITERAL(18, 267, 14) // "progressUpdate"

    },
    "NotifierWindow\0hideNow\0\0toggleLineEdit\0"
    "slotTabCloseRequested\0index\0blink\0"
    "heartbeat\0returnPressed\0updateGui\0"
    "fillContextPopup\0disableFor1Minute\0"
    "disableFor5Minutes\0disableFor15Minutes\0"
    "disableFor30Minutes\0disableFor60Minutes\0"
    "disableUntilKVIrcRestarted\0"
    "disablePermanently\0progressUpdate"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_NotifierWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   94,    2, 0x0a /* Public */,
       3,    0,   95,    2, 0x0a /* Public */,
       4,    1,   96,    2, 0x0a /* Public */,
       6,    0,   99,    2, 0x09 /* Protected */,
       7,    0,  100,    2, 0x09 /* Protected */,
       8,    0,  101,    2, 0x09 /* Protected */,
       9,    0,  102,    2, 0x09 /* Protected */,
      10,    0,  103,    2, 0x09 /* Protected */,
      11,    0,  104,    2, 0x09 /* Protected */,
      12,    0,  105,    2, 0x09 /* Protected */,
      13,    0,  106,    2, 0x09 /* Protected */,
      14,    0,  107,    2, 0x09 /* Protected */,
      15,    0,  108,    2, 0x09 /* Protected */,
      16,    0,  109,    2, 0x09 /* Protected */,
      17,    0,  110,    2, 0x09 /* Protected */,
      18,    0,  111,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void NotifierWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        NotifierWindow *_t = static_cast<NotifierWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->hideNow(); break;
        case 1: _t->toggleLineEdit(); break;
        case 2: _t->slotTabCloseRequested((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->blink(); break;
        case 4: _t->heartbeat(); break;
        case 5: _t->returnPressed(); break;
        case 6: _t->updateGui(); break;
        case 7: _t->fillContextPopup(); break;
        case 8: _t->disableFor1Minute(); break;
        case 9: _t->disableFor5Minutes(); break;
        case 10: _t->disableFor15Minutes(); break;
        case 11: _t->disableFor30Minutes(); break;
        case 12: _t->disableFor60Minutes(); break;
        case 13: _t->disableUntilKVIrcRestarted(); break;
        case 14: _t->disablePermanently(); break;
        case 15: _t->progressUpdate(); break;
        default: ;
        }
    }
}

const QMetaObject NotifierWindow::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_NotifierWindow.data,
      qt_meta_data_NotifierWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *NotifierWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NotifierWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_NotifierWindow.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int NotifierWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 16;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
