/****************************************************************************
** Meta object code from reading C++ file 'KviWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ui/KviWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KviWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviWindow_t {
    QByteArrayData data[17];
    char stringdata0[279];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviWindow_t qt_meta_stringdata_KviWindow = {
    {
QT_MOC_LITERAL(0, 0, 9), // "KviWindow"
QT_MOC_LITERAL(1, 10, 17), // "windowNameChanged"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 4), // "dock"
QT_MOC_LITERAL(4, 34, 6), // "undock"
QT_MOC_LITERAL(5, 41, 9), // "autoRaise"
QT_MOC_LITERAL(6, 51, 12), // "reloadImages"
QT_MOC_LITERAL(7, 64, 23), // "savePropertiesAsDefault"
QT_MOC_LITERAL(8, 88, 21), // "toggleCryptController"
QT_MOC_LITERAL(9, 110, 23), // "cryptControllerFinished"
QT_MOC_LITERAL(10, 134, 25), // "cryptSessionInfoDestroyed"
QT_MOC_LITERAL(11, 160, 25), // "textEncodingButtonClicked"
QT_MOC_LITERAL(12, 186, 32), // "systemTextEncodingPopupActivated"
QT_MOC_LITERAL(13, 219, 8), // "QAction*"
QT_MOC_LITERAL(14, 228, 7), // "pAction"
QT_MOC_LITERAL(15, 236, 14), // "childDestroyed"
QT_MOC_LITERAL(16, 251, 27) // "KviProperty_ChildFocusOwner"

    },
    "KviWindow\0windowNameChanged\0\0dock\0"
    "undock\0autoRaise\0reloadImages\0"
    "savePropertiesAsDefault\0toggleCryptController\0"
    "cryptControllerFinished\0"
    "cryptSessionInfoDestroyed\0"
    "textEncodingButtonClicked\0"
    "systemTextEncodingPopupActivated\0"
    "QAction*\0pAction\0childDestroyed\0"
    "KviProperty_ChildFocusOwner"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       1,   88, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   74,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   75,    2, 0x0a /* Public */,
       4,    0,   76,    2, 0x0a /* Public */,
       5,    0,   77,    2, 0x0a /* Public */,
       6,    0,   78,    2, 0x0a /* Public */,
       7,    0,   79,    2, 0x0a /* Public */,
       8,    0,   80,    2, 0x09 /* Protected */,
       9,    0,   81,    2, 0x09 /* Protected */,
      10,    0,   82,    2, 0x09 /* Protected */,
      11,    0,   83,    2, 0x09 /* Protected */,
      12,    1,   84,    2, 0x09 /* Protected */,
      15,    0,   87,    2, 0x09 /* Protected */,

 // signals: parameters
    QMetaType::Void,

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
    QMetaType::Void, 0x80000000 | 13,   14,
    QMetaType::Void,

 // properties: name, type, flags
      16, QMetaType::Int, 0x00095001,

       0        // eod
};

void KviWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviWindow *_t = static_cast<KviWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->windowNameChanged(); break;
        case 1: _t->dock(); break;
        case 2: _t->undock(); break;
        case 3: _t->autoRaise(); break;
        case 4: _t->reloadImages(); break;
        case 5: _t->savePropertiesAsDefault(); break;
        case 6: _t->toggleCryptController(); break;
        case 7: _t->cryptControllerFinished(); break;
        case 8: _t->cryptSessionInfoDestroyed(); break;
        case 9: _t->textEncodingButtonClicked(); break;
        case 10: _t->systemTextEncodingPopupActivated((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 11: _t->childDestroyed(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (KviWindow::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviWindow::windowNameChanged)) {
                *result = 0;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        KviWindow *_t = static_cast<KviWindow *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->type(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject KviWindow::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_KviWindow.data,
      qt_meta_data_KviWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviWindow.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int KviWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void KviWindow::windowNameChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
