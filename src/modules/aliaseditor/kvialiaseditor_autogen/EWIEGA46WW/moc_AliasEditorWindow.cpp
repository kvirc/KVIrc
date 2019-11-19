/****************************************************************************
** Meta object code from reading C++ file 'AliasEditorWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../AliasEditorWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AliasEditorWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_AliasEditorTreeWidget_t {
    QByteArrayData data[1];
    char stringdata0[22];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AliasEditorTreeWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AliasEditorTreeWidget_t qt_meta_stringdata_AliasEditorTreeWidget = {
    {
QT_MOC_LITERAL(0, 0, 21) // "AliasEditorTreeWidget"

    },
    "AliasEditorTreeWidget"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AliasEditorTreeWidget[] = {

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

void AliasEditorTreeWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject AliasEditorTreeWidget::staticMetaObject = {
    { &QTreeWidget::staticMetaObject, qt_meta_stringdata_AliasEditorTreeWidget.data,
      qt_meta_data_AliasEditorTreeWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *AliasEditorTreeWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AliasEditorTreeWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AliasEditorTreeWidget.stringdata0))
        return static_cast<void*>(this);
    return QTreeWidget::qt_metacast(_clname);
}

int AliasEditorTreeWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeWidget::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_AliasEditorWidget_t {
    QByteArrayData data[28];
    char stringdata0[354];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AliasEditorWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AliasEditorWidget_t qt_meta_stringdata_AliasEditorWidget = {
    {
QT_MOC_LITERAL(0, 0, 17), // "AliasEditorWidget"
QT_MOC_LITERAL(1, 18, 18), // "currentItemChanged"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 16), // "QTreeWidgetItem*"
QT_MOC_LITERAL(4, 55, 2), // "it"
QT_MOC_LITERAL(5, 58, 10), // "renameItem"
QT_MOC_LITERAL(6, 69, 8), // "newAlias"
QT_MOC_LITERAL(7, 78, 26), // "customContextMenuRequested"
QT_MOC_LITERAL(8, 105, 3), // "pnt"
QT_MOC_LITERAL(9, 109, 12), // "newNamespace"
QT_MOC_LITERAL(10, 122, 7), // "newItem"
QT_MOC_LITERAL(11, 130, 8), // "QString&"
QT_MOC_LITERAL(12, 139, 6), // "szName"
QT_MOC_LITERAL(13, 146, 31), // "AliasEditorTreeWidgetItem::Type"
QT_MOC_LITERAL(14, 178, 5), // "eType"
QT_MOC_LITERAL(15, 184, 9), // "exportAll"
QT_MOC_LITERAL(16, 194, 22), // "exportSelectedSepFiles"
QT_MOC_LITERAL(17, 217, 14), // "exportSelected"
QT_MOC_LITERAL(18, 232, 19), // "removeSelectedItems"
QT_MOC_LITERAL(19, 252, 8), // "slotFind"
QT_MOC_LITERAL(20, 261, 22), // "slotCollapseNamespaces"
QT_MOC_LITERAL(21, 284, 12), // "slotFindWord"
QT_MOC_LITERAL(22, 297, 14), // "slotReplaceAll"
QT_MOC_LITERAL(23, 312, 6), // "before"
QT_MOC_LITERAL(24, 319, 5), // "after"
QT_MOC_LITERAL(25, 325, 11), // "itemRenamed"
QT_MOC_LITERAL(26, 337, 3), // "col"
QT_MOC_LITERAL(27, 341, 12) // "aliasRefresh"

    },
    "AliasEditorWidget\0currentItemChanged\0"
    "\0QTreeWidgetItem*\0it\0renameItem\0"
    "newAlias\0customContextMenuRequested\0"
    "pnt\0newNamespace\0newItem\0QString&\0"
    "szName\0AliasEditorTreeWidgetItem::Type\0"
    "eType\0exportAll\0exportSelectedSepFiles\0"
    "exportSelected\0removeSelectedItems\0"
    "slotFind\0slotCollapseNamespaces\0"
    "slotFindWord\0slotReplaceAll\0before\0"
    "after\0itemRenamed\0col\0aliasRefresh"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AliasEditorWidget[] = {

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
       1,    2,   94,    2, 0x09 /* Protected */,
       5,    0,   99,    2, 0x09 /* Protected */,
       6,    0,  100,    2, 0x09 /* Protected */,
       7,    1,  101,    2, 0x09 /* Protected */,
       9,    0,  104,    2, 0x09 /* Protected */,
      10,    2,  105,    2, 0x09 /* Protected */,
      15,    0,  110,    2, 0x09 /* Protected */,
      16,    0,  111,    2, 0x09 /* Protected */,
      17,    0,  112,    2, 0x09 /* Protected */,
      18,    0,  113,    2, 0x09 /* Protected */,
      19,    0,  114,    2, 0x09 /* Protected */,
      20,    0,  115,    2, 0x09 /* Protected */,
      21,    1,  116,    2, 0x09 /* Protected */,
      22,    2,  119,    2, 0x09 /* Protected */,
      25,    2,  124,    2, 0x09 /* Protected */,
      27,    1,  129,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,    8,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 11, 0x80000000 | 13,   12,   14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   23,   24,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int,    4,   26,
    QMetaType::Void, QMetaType::QString,   12,

       0        // eod
};

void AliasEditorWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AliasEditorWidget *_t = static_cast<AliasEditorWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->currentItemChanged((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QTreeWidgetItem*(*)>(_a[2]))); break;
        case 1: _t->renameItem(); break;
        case 2: _t->newAlias(); break;
        case 3: _t->customContextMenuRequested((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 4: _t->newNamespace(); break;
        case 5: _t->newItem((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< AliasEditorTreeWidgetItem::Type(*)>(_a[2]))); break;
        case 6: _t->exportAll(); break;
        case 7: _t->exportSelectedSepFiles(); break;
        case 8: _t->exportSelected(); break;
        case 9: _t->removeSelectedItems(); break;
        case 10: _t->slotFind(); break;
        case 11: _t->slotCollapseNamespaces(); break;
        case 12: _t->slotFindWord((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 13: _t->slotReplaceAll((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 14: _t->itemRenamed((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 15: _t->aliasRefresh((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject AliasEditorWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_AliasEditorWidget.data,
      qt_meta_data_AliasEditorWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *AliasEditorWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AliasEditorWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AliasEditorWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int AliasEditorWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
struct qt_meta_stringdata_AliasEditorWindow_t {
    QByteArrayData data[5];
    char stringdata0[56];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AliasEditorWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AliasEditorWindow_t qt_meta_stringdata_AliasEditorWindow = {
    {
QT_MOC_LITERAL(0, 0, 17), // "AliasEditorWindow"
QT_MOC_LITERAL(1, 18, 13), // "cancelClicked"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 9), // "okClicked"
QT_MOC_LITERAL(4, 43, 12) // "applyClicked"

    },
    "AliasEditorWindow\0cancelClicked\0\0"
    "okClicked\0applyClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AliasEditorWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x09 /* Protected */,
       3,    0,   30,    2, 0x09 /* Protected */,
       4,    0,   31,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void AliasEditorWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AliasEditorWindow *_t = static_cast<AliasEditorWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->cancelClicked(); break;
        case 1: _t->okClicked(); break;
        case 2: _t->applyClicked(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject AliasEditorWindow::staticMetaObject = {
    { &KviWindow::staticMetaObject, qt_meta_stringdata_AliasEditorWindow.data,
      qt_meta_data_AliasEditorWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *AliasEditorWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AliasEditorWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AliasEditorWindow.stringdata0))
        return static_cast<void*>(this);
    return KviWindow::qt_metacast(_clname);
}

int AliasEditorWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
