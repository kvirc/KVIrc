/****************************************************************************
** Meta object code from reading C++ file 'ListWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ListWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ListWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ChannelTreeWidget_t {
    QByteArrayData data[1];
    char stringdata0[18];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ChannelTreeWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ChannelTreeWidget_t qt_meta_stringdata_ChannelTreeWidget = {
    {
QT_MOC_LITERAL(0, 0, 17) // "ChannelTreeWidget"

    },
    "ChannelTreeWidget"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ChannelTreeWidget[] = {

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

void ChannelTreeWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject ChannelTreeWidget::staticMetaObject = {
    { &KviThemedTreeWidget::staticMetaObject, qt_meta_stringdata_ChannelTreeWidget.data,
      qt_meta_data_ChannelTreeWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ChannelTreeWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ChannelTreeWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ChannelTreeWidget.stringdata0))
        return static_cast<void*>(this);
    return KviThemedTreeWidget::qt_metacast(_clname);
}

int ChannelTreeWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviThemedTreeWidget::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_ListWindow_t {
    QByteArrayData data[13];
    char stringdata0[147];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ListWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ListWindow_t qt_meta_stringdata_ListWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "ListWindow"
QT_MOC_LITERAL(1, 11, 5), // "flush"
QT_MOC_LITERAL(2, 17, 0), // ""
QT_MOC_LITERAL(3, 18, 17), // "itemDoubleClicked"
QT_MOC_LITERAL(4, 36, 16), // "QTreeWidgetItem*"
QT_MOC_LITERAL(5, 53, 2), // "it"
QT_MOC_LITERAL(6, 56, 11), // "requestList"
QT_MOC_LITERAL(7, 68, 16), // "stoplistdownload"
QT_MOC_LITERAL(8, 85, 21), // "connectionStateChange"
QT_MOC_LITERAL(9, 107, 10), // "exportList"
QT_MOC_LITERAL(10, 118, 10), // "importList"
QT_MOC_LITERAL(11, 129, 10), // "liveSearch"
QT_MOC_LITERAL(12, 140, 6) // "szText"

    },
    "ListWindow\0flush\0\0itemDoubleClicked\0"
    "QTreeWidgetItem*\0it\0requestList\0"
    "stoplistdownload\0connectionStateChange\0"
    "exportList\0importList\0liveSearch\0"
    "szText"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ListWindow[] = {

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
       3,    2,   55,    2, 0x09 /* Protected */,
       6,    0,   60,    2, 0x09 /* Protected */,
       7,    0,   61,    2, 0x09 /* Protected */,
       8,    0,   62,    2, 0x09 /* Protected */,
       9,    0,   63,    2, 0x09 /* Protected */,
      10,    0,   64,    2, 0x09 /* Protected */,
      11,    1,   65,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4, QMetaType::Int,    5,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   12,

       0        // eod
};

void ListWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ListWindow *_t = static_cast<ListWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->flush(); break;
        case 1: _t->itemDoubleClicked((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->requestList(); break;
        case 3: _t->stoplistdownload(); break;
        case 4: _t->connectionStateChange(); break;
        case 5: _t->exportList(); break;
        case 6: _t->importList(); break;
        case 7: _t->liveSearch((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject ListWindow::staticMetaObject = {
    { &KviWindow::staticMetaObject, qt_meta_stringdata_ListWindow.data,
      qt_meta_data_ListWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ListWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ListWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ListWindow.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "KviExternalServerDataParser"))
        return static_cast< KviExternalServerDataParser*>(this);
    return KviWindow::qt_metacast(_clname);
}

int ListWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
