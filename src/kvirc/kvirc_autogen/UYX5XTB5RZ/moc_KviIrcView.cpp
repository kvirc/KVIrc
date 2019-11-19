/****************************************************************************
** Meta object code from reading C++ file 'KviIrcView.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ui/KviIrcView.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KviIrcView.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviIrcView_t {
    QByteArrayData data[23];
    char stringdata0[320];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviIrcView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviIrcView_t qt_meta_stringdata_KviIrcView = {
    {
QT_MOC_LITERAL(0, 0, 10), // "KviIrcView"
QT_MOC_LITERAL(1, 11, 12), // "rightClicked"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 10), // "dndEntered"
QT_MOC_LITERAL(4, 36, 11), // "fileDropped"
QT_MOC_LITERAL(5, 48, 8), // "flushLog"
QT_MOC_LITERAL(6, 57, 14), // "showToolsPopup"
QT_MOC_LITERAL(7, 72, 11), // "clearBuffer"
QT_MOC_LITERAL(8, 84, 16), // "toggleToolWidget"
QT_MOC_LITERAL(9, 101, 16), // "increaseFontSize"
QT_MOC_LITERAL(10, 118, 16), // "decreaseFontSize"
QT_MOC_LITERAL(11, 135, 16), // "resetDefaultFont"
QT_MOC_LITERAL(12, 152, 10), // "chooseFont"
QT_MOC_LITERAL(13, 163, 16), // "chooseBackground"
QT_MOC_LITERAL(14, 180, 15), // "resetBackground"
QT_MOC_LITERAL(15, 196, 24), // "scrollBarPositionChanged"
QT_MOC_LITERAL(16, 221, 8), // "newValue"
QT_MOC_LITERAL(17, 230, 13), // "screenChanged"
QT_MOC_LITERAL(18, 244, 8), // "QScreen*"
QT_MOC_LITERAL(19, 253, 10), // "masterDead"
QT_MOC_LITERAL(20, 264, 18), // "animatedIconChange"
QT_MOC_LITERAL(21, 283, 19), // "TransparencyCapable"
QT_MOC_LITERAL(22, 303, 16) // "usePaintOnScreen"

    },
    "KviIrcView\0rightClicked\0\0dndEntered\0"
    "fileDropped\0flushLog\0showToolsPopup\0"
    "clearBuffer\0toggleToolWidget\0"
    "increaseFontSize\0decreaseFontSize\0"
    "resetDefaultFont\0chooseFont\0"
    "chooseBackground\0resetBackground\0"
    "scrollBarPositionChanged\0newValue\0"
    "screenChanged\0QScreen*\0masterDead\0"
    "animatedIconChange\0TransparencyCapable\0"
    "usePaintOnScreen"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviIrcView[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       2,  122, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   99,    2, 0x06 /* Public */,
       3,    0,  100,    2, 0x06 /* Public */,
       4,    1,  101,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,  104,    2, 0x0a /* Public */,
       6,    0,  105,    2, 0x0a /* Public */,
       7,    0,  106,    2, 0x0a /* Public */,
       8,    0,  107,    2, 0x0a /* Public */,
       9,    0,  108,    2, 0x0a /* Public */,
      10,    0,  109,    2, 0x0a /* Public */,
      11,    0,  110,    2, 0x0a /* Public */,
      12,    0,  111,    2, 0x0a /* Public */,
      13,    0,  112,    2, 0x0a /* Public */,
      14,    0,  113,    2, 0x0a /* Public */,
      15,    1,  114,    2, 0x09 /* Protected */,
      17,    1,  117,    2, 0x09 /* Protected */,
      19,    0,  120,    2, 0x09 /* Protected */,
      20,    0,  121,    2, 0x09 /* Protected */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,

 // slots: parameters
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
    QMetaType::Void, QMetaType::Int,   16,
    QMetaType::Void, 0x80000000 | 18,    2,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
      21, QMetaType::Int, 0x00095001,
      22, QMetaType::Bool, 0x00095003,

       0        // eod
};

void KviIrcView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviIrcView *_t = static_cast<KviIrcView *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->rightClicked(); break;
        case 1: _t->dndEntered(); break;
        case 2: _t->fileDropped((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->flushLog(); break;
        case 4: _t->showToolsPopup(); break;
        case 5: _t->clearBuffer(); break;
        case 6: _t->toggleToolWidget(); break;
        case 7: _t->increaseFontSize(); break;
        case 8: _t->decreaseFontSize(); break;
        case 9: _t->resetDefaultFont(); break;
        case 10: _t->chooseFont(); break;
        case 11: _t->chooseBackground(); break;
        case 12: _t->resetBackground(); break;
        case 13: _t->scrollBarPositionChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: _t->screenChanged((*reinterpret_cast< QScreen*(*)>(_a[1]))); break;
        case 15: _t->masterDead(); break;
        case 16: _t->animatedIconChange(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (KviIrcView::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviIrcView::rightClicked)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (KviIrcView::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviIrcView::dndEntered)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (KviIrcView::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviIrcView::fileDropped)) {
                *result = 2;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        KviIrcView *_t = static_cast<KviIrcView *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->dummyRead(); break;
        case 1: *reinterpret_cast< bool*>(_v) = _t->getPaintOnScreen(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        KviIrcView *_t = static_cast<KviIrcView *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 1: _t->setPaintOnScreen(*reinterpret_cast< bool*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject KviIrcView::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_KviIrcView.data,
      qt_meta_data_KviIrcView,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviIrcView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviIrcView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviIrcView.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int KviIrcView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 17)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 17;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void KviIrcView::rightClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void KviIrcView::dndEntered()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void KviIrcView::fileDropped(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
