/****************************************************************************
** Meta object code from reading C++ file 'KviIrcView_tools.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ui/KviIrcView_tools.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KviIrcView_tools.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviIrcViewToolWidget_t {
    QByteArrayData data[11];
    char stringdata0[131];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviIrcViewToolWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviIrcViewToolWidget_t qt_meta_stringdata_KviIrcViewToolWidget = {
    {
QT_MOC_LITERAL(0, 0, 20), // "KviIrcViewToolWidget"
QT_MOC_LITERAL(1, 21, 8), // "findPrev"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 8), // "findNext"
QT_MOC_LITERAL(4, 40, 14), // "findNextHelper"
QT_MOC_LITERAL(5, 55, 6), // "unused"
QT_MOC_LITERAL(6, 62, 15), // "filterEnableAll"
QT_MOC_LITERAL(7, 78, 16), // "filterEnableNone"
QT_MOC_LITERAL(8, 95, 10), // "filterSave"
QT_MOC_LITERAL(9, 106, 10), // "filterLoad"
QT_MOC_LITERAL(10, 117, 13) // "toggleOptions"

    },
    "KviIrcViewToolWidget\0findPrev\0\0findNext\0"
    "findNextHelper\0unused\0filterEnableAll\0"
    "filterEnableNone\0filterSave\0filterLoad\0"
    "toggleOptions"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviIrcViewToolWidget[] = {

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
       3,    0,   55,    2, 0x09 /* Protected */,
       4,    1,   56,    2, 0x09 /* Protected */,
       6,    0,   59,    2, 0x09 /* Protected */,
       7,    0,   60,    2, 0x09 /* Protected */,
       8,    0,   61,    2, 0x09 /* Protected */,
       9,    0,   62,    2, 0x09 /* Protected */,
      10,    0,   63,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void KviIrcViewToolWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviIrcViewToolWidget *_t = static_cast<KviIrcViewToolWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->findPrev(); break;
        case 1: _t->findNext(); break;
        case 2: _t->findNextHelper((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->filterEnableAll(); break;
        case 4: _t->filterEnableNone(); break;
        case 5: _t->filterSave(); break;
        case 6: _t->filterLoad(); break;
        case 7: _t->toggleOptions(); break;
        default: ;
        }
    }
}

const QMetaObject KviIrcViewToolWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_KviIrcViewToolWidget.data,
      qt_meta_data_KviIrcViewToolWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviIrcViewToolWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviIrcViewToolWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviIrcViewToolWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int KviIrcViewToolWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
