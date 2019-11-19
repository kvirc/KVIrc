/****************************************************************************
** Meta object code from reading C++ file 'LogViewWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../LogViewWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LogViewWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_LogViewListView_t {
    QByteArrayData data[4];
    char stringdata0[53];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LogViewListView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LogViewListView_t qt_meta_stringdata_LogViewListView = {
    {
QT_MOC_LITERAL(0, 0, 15), // "LogViewListView"
QT_MOC_LITERAL(1, 16, 18), // "rightButtonPressed"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 16) // "QTreeWidgetItem*"

    },
    "LogViewListView\0rightButtonPressed\0\0"
    "QTreeWidgetItem*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LogViewListView[] = {

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

void LogViewListView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LogViewListView *_t = static_cast<LogViewListView *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->rightButtonPressed((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QPoint(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (LogViewListView::*_t)(QTreeWidgetItem * , QPoint );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&LogViewListView::rightButtonPressed)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject LogViewListView::staticMetaObject = {
    { &QTreeWidget::staticMetaObject, qt_meta_stringdata_LogViewListView.data,
      qt_meta_data_LogViewListView,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *LogViewListView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LogViewListView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_LogViewListView.stringdata0))
        return static_cast<void*>(this);
    return QTreeWidget::qt_metacast(_clname);
}

int LogViewListView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeWidget::qt_metacall(_c, _id, _a);
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
void LogViewListView::rightButtonPressed(QTreeWidgetItem * _t1, QPoint _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_LogViewWindow_t {
    QByteArrayData data[14];
    char stringdata0[160];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LogViewWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LogViewWindow_t qt_meta_stringdata_LogViewWindow = {
    {
QT_MOC_LITERAL(0, 0, 13), // "LogViewWindow"
QT_MOC_LITERAL(1, 14, 18), // "rightButtonClicked"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 16), // "QTreeWidgetItem*"
QT_MOC_LITERAL(4, 51, 12), // "itemSelected"
QT_MOC_LITERAL(5, 64, 5), // "pItem"
QT_MOC_LITERAL(6, 70, 13), // "deleteCurrent"
QT_MOC_LITERAL(7, 84, 11), // "applyFilter"
QT_MOC_LITERAL(8, 96, 11), // "abortFilter"
QT_MOC_LITERAL(9, 108, 13), // "cacheFileList"
QT_MOC_LITERAL(10, 122, 10), // "filterNext"
QT_MOC_LITERAL(11, 133, 9), // "exportLog"
QT_MOC_LITERAL(12, 143, 8), // "QAction*"
QT_MOC_LITERAL(13, 152, 7) // "pAction"

    },
    "LogViewWindow\0rightButtonClicked\0\0"
    "QTreeWidgetItem*\0itemSelected\0pItem\0"
    "deleteCurrent\0applyFilter\0abortFilter\0"
    "cacheFileList\0filterNext\0exportLog\0"
    "QAction*\0pAction"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LogViewWindow[] = {

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
       1,    2,   54,    2, 0x09 /* Protected */,
       4,    2,   59,    2, 0x09 /* Protected */,
       6,    0,   64,    2, 0x09 /* Protected */,
       7,    0,   65,    2, 0x09 /* Protected */,
       8,    0,   66,    2, 0x09 /* Protected */,
       9,    0,   67,    2, 0x09 /* Protected */,
      10,    0,   68,    2, 0x09 /* Protected */,
      11,    1,   69,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::QPoint,    2,    2,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    5,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 12,   13,

       0        // eod
};

void LogViewWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LogViewWindow *_t = static_cast<LogViewWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->rightButtonClicked((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2]))); break;
        case 1: _t->itemSelected((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QTreeWidgetItem*(*)>(_a[2]))); break;
        case 2: _t->deleteCurrent(); break;
        case 3: _t->applyFilter(); break;
        case 4: _t->abortFilter(); break;
        case 5: _t->cacheFileList(); break;
        case 6: _t->filterNext(); break;
        case 7: _t->exportLog((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject LogViewWindow::staticMetaObject = {
    { &KviWindow::staticMetaObject, qt_meta_stringdata_LogViewWindow.data,
      qt_meta_data_LogViewWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *LogViewWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LogViewWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_LogViewWindow.stringdata0))
        return static_cast<void*>(this);
    return KviWindow::qt_metacast(_clname);
}

int LogViewWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviWindow::qt_metacall(_c, _id, _a);
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
