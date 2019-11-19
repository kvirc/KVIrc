/****************************************************************************
** Meta object code from reading C++ file 'LinksWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../LinksWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LinksWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_LinksListView_t {
    QByteArrayData data[4];
    char stringdata0[51];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LinksListView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LinksListView_t qt_meta_stringdata_LinksListView = {
    {
QT_MOC_LITERAL(0, 0, 13), // "LinksListView"
QT_MOC_LITERAL(1, 14, 18), // "rightButtonPressed"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 16) // "QTreeWidgetItem*"

    },
    "LinksListView\0rightButtonPressed\0\0"
    "QTreeWidgetItem*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LinksListView[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::QPoint,    2,    2,

       0        // eod
};

void LinksListView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LinksListView *_t = static_cast<LinksListView *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->rightButtonPressed((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QPoint(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (LinksListView::*_t)(QTreeWidgetItem * , QPoint );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&LinksListView::rightButtonPressed)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject LinksListView::staticMetaObject = {
    { &KviThemedTreeWidget::staticMetaObject, qt_meta_stringdata_LinksListView.data,
      qt_meta_data_LinksListView,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *LinksListView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LinksListView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_LinksListView.stringdata0))
        return static_cast<void*>(this);
    return KviThemedTreeWidget::qt_metacast(_clname);
}

int LinksListView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviThemedTreeWidget::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void LinksListView::rightButtonPressed(QTreeWidgetItem * _t1, QPoint _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_LinksWindow_t {
    QByteArrayData data[11];
    char stringdata0[117];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LinksWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LinksWindow_t qt_meta_stringdata_LinksWindow = {
    {
QT_MOC_LITERAL(0, 0, 11), // "LinksWindow"
QT_MOC_LITERAL(1, 12, 13), // "showHostPopup"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 16), // "QTreeWidgetItem*"
QT_MOC_LITERAL(4, 44, 1), // "i"
QT_MOC_LITERAL(5, 46, 1), // "p"
QT_MOC_LITERAL(6, 48, 16), // "hostPopupClicked"
QT_MOC_LITERAL(7, 65, 8), // "QAction*"
QT_MOC_LITERAL(8, 74, 7), // "pAction"
QT_MOC_LITERAL(9, 82, 12), // "requestLinks"
QT_MOC_LITERAL(10, 95, 21) // "connectionStateChange"

    },
    "LinksWindow\0showHostPopup\0\0QTreeWidgetItem*\0"
    "i\0p\0hostPopupClicked\0QAction*\0pAction\0"
    "requestLinks\0connectionStateChange"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LinksWindow[] = {

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
       1,    2,   34,    2, 0x09 /* Protected */,
       6,    1,   39,    2, 0x09 /* Protected */,
       9,    0,   42,    2, 0x09 /* Protected */,
      10,    0,   43,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::QPoint,    4,    5,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void LinksWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LinksWindow *_t = static_cast<LinksWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->showHostPopup((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2]))); break;
        case 1: _t->hostPopupClicked((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 2: _t->requestLinks(); break;
        case 3: _t->connectionStateChange(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAction* >(); break;
            }
            break;
        }
    }
}

const QMetaObject LinksWindow::staticMetaObject = {
    { &KviWindow::staticMetaObject, qt_meta_stringdata_LinksWindow.data,
      qt_meta_data_LinksWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *LinksWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LinksWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_LinksWindow.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "KviExternalServerDataParser"))
        return static_cast< KviExternalServerDataParser*>(this);
    return KviWindow::qt_metacast(_clname);
}

int LinksWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
