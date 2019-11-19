/****************************************************************************
** Meta object code from reading C++ file 'ClassEditorWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ClassEditorWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ClassEditorWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ClassEditorTreeWidget_t {
    QByteArrayData data[6];
    char stringdata0[69];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ClassEditorTreeWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ClassEditorTreeWidget_t qt_meta_stringdata_ClassEditorTreeWidget = {
    {
QT_MOC_LITERAL(0, 0, 21), // "ClassEditorTreeWidget"
QT_MOC_LITERAL(1, 22, 18), // "rightButtonPressed"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 16), // "QTreeWidgetItem*"
QT_MOC_LITERAL(4, 59, 5), // "pItem"
QT_MOC_LITERAL(5, 65, 3) // "pos"

    },
    "ClassEditorTreeWidget\0rightButtonPressed\0"
    "\0QTreeWidgetItem*\0pItem\0pos"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ClassEditorTreeWidget[] = {

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
    QMetaType::Void, 0x80000000 | 3, QMetaType::QPoint,    4,    5,

       0        // eod
};

void ClassEditorTreeWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ClassEditorTreeWidget *_t = static_cast<ClassEditorTreeWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->rightButtonPressed((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QPoint(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (ClassEditorTreeWidget::*_t)(QTreeWidgetItem * , QPoint );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ClassEditorTreeWidget::rightButtonPressed)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject ClassEditorTreeWidget::staticMetaObject = {
    { &QTreeWidget::staticMetaObject, qt_meta_stringdata_ClassEditorTreeWidget.data,
      qt_meta_data_ClassEditorTreeWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ClassEditorTreeWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ClassEditorTreeWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ClassEditorTreeWidget.stringdata0))
        return static_cast<void*>(this);
    return QTreeWidget::qt_metacast(_clname);
}

int ClassEditorTreeWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void ClassEditorTreeWidget::rightButtonPressed(QTreeWidgetItem * _t1, QPoint _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_ClassEditorWidget_t {
    QByteArrayData data[32];
    char stringdata0[448];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ClassEditorWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ClassEditorWidget_t qt_meta_stringdata_ClassEditorWidget = {
    {
QT_MOC_LITERAL(0, 0, 17), // "ClassEditorWidget"
QT_MOC_LITERAL(1, 18, 18), // "currentItemChanged"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 16), // "QTreeWidgetItem*"
QT_MOC_LITERAL(4, 55, 5), // "pItem"
QT_MOC_LITERAL(5, 61, 26), // "customContextMenuRequested"
QT_MOC_LITERAL(6, 88, 8), // "newClass"
QT_MOC_LITERAL(7, 97, 12), // "newNamespace"
QT_MOC_LITERAL(8, 110, 17), // "newMemberFunction"
QT_MOC_LITERAL(9, 128, 14), // "renameFunction"
QT_MOC_LITERAL(10, 143, 7), // "newItem"
QT_MOC_LITERAL(11, 151, 26), // "ClassEditorTreeWidgetItem*"
QT_MOC_LITERAL(12, 178, 8), // "QString&"
QT_MOC_LITERAL(13, 187, 6), // "szName"
QT_MOC_LITERAL(14, 194, 31), // "ClassEditorTreeWidgetItem::Type"
QT_MOC_LITERAL(15, 226, 5), // "eType"
QT_MOC_LITERAL(16, 232, 10), // "renameItem"
QT_MOC_LITERAL(17, 243, 9), // "exportAll"
QT_MOC_LITERAL(18, 253, 22), // "exportSelectedSepFiles"
QT_MOC_LITERAL(19, 276, 14), // "exportSelected"
QT_MOC_LITERAL(20, 291, 8), // "slotFind"
QT_MOC_LITERAL(21, 300, 12), // "slotFindWord"
QT_MOC_LITERAL(22, 313, 13), // "searchReplace"
QT_MOC_LITERAL(23, 327, 8), // "szSearch"
QT_MOC_LITERAL(24, 336, 8), // "bReplace"
QT_MOC_LITERAL(25, 345, 9), // "szReplace"
QT_MOC_LITERAL(26, 355, 17), // "slotCollapseItems"
QT_MOC_LITERAL(27, 373, 22), // "recursiveCollapseItems"
QT_MOC_LITERAL(28, 396, 19), // "removeSelectedItems"
QT_MOC_LITERAL(29, 416, 14), // "slotReplaceAll"
QT_MOC_LITERAL(30, 431, 8), // "szBefore"
QT_MOC_LITERAL(31, 440, 7) // "szAfter"

    },
    "ClassEditorWidget\0currentItemChanged\0"
    "\0QTreeWidgetItem*\0pItem\0"
    "customContextMenuRequested\0newClass\0"
    "newNamespace\0newMemberFunction\0"
    "renameFunction\0newItem\0"
    "ClassEditorTreeWidgetItem*\0QString&\0"
    "szName\0ClassEditorTreeWidgetItem::Type\0"
    "eType\0renameItem\0exportAll\0"
    "exportSelectedSepFiles\0exportSelected\0"
    "slotFind\0slotFindWord\0searchReplace\0"
    "szSearch\0bReplace\0szReplace\0"
    "slotCollapseItems\0recursiveCollapseItems\0"
    "removeSelectedItems\0slotReplaceAll\0"
    "szBefore\0szAfter"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ClassEditorWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,  114,    2, 0x09 /* Protected */,
       5,    1,  119,    2, 0x09 /* Protected */,
       6,    0,  122,    2, 0x09 /* Protected */,
       7,    0,  123,    2, 0x09 /* Protected */,
       8,    0,  124,    2, 0x09 /* Protected */,
       9,    0,  125,    2, 0x09 /* Protected */,
      10,    2,  126,    2, 0x09 /* Protected */,
      16,    0,  131,    2, 0x09 /* Protected */,
      17,    0,  132,    2, 0x09 /* Protected */,
      18,    0,  133,    2, 0x09 /* Protected */,
      19,    0,  134,    2, 0x09 /* Protected */,
      20,    0,  135,    2, 0x09 /* Protected */,
      21,    1,  136,    2, 0x09 /* Protected */,
      22,    3,  139,    2, 0x09 /* Protected */,
      22,    2,  146,    2, 0x29 /* Protected | MethodCloned */,
      22,    1,  151,    2, 0x29 /* Protected | MethodCloned */,
      26,    0,  154,    2, 0x09 /* Protected */,
      27,    1,  155,    2, 0x09 /* Protected */,
      28,    0,  158,    2, 0x09 /* Protected */,
      29,    2,  159,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    2,
    QMetaType::Void, QMetaType::QPoint,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    0x80000000 | 11, 0x80000000 | 12, 0x80000000 | 14,   13,   15,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool, QMetaType::QString,   23,   24,   25,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,   23,   24,
    QMetaType::Void, QMetaType::QString,   23,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 11,    4,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   30,   31,

       0        // eod
};

void ClassEditorWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ClassEditorWidget *_t = static_cast<ClassEditorWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->currentItemChanged((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QTreeWidgetItem*(*)>(_a[2]))); break;
        case 1: _t->customContextMenuRequested((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 2: _t->newClass(); break;
        case 3: _t->newNamespace(); break;
        case 4: _t->newMemberFunction(); break;
        case 5: _t->renameFunction(); break;
        case 6: { ClassEditorTreeWidgetItem* _r = _t->newItem((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< ClassEditorTreeWidgetItem::Type(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< ClassEditorTreeWidgetItem**>(_a[0]) = std::move(_r); }  break;
        case 7: _t->renameItem(); break;
        case 8: _t->exportAll(); break;
        case 9: _t->exportSelectedSepFiles(); break;
        case 10: _t->exportSelected(); break;
        case 11: _t->slotFind(); break;
        case 12: _t->slotFindWord((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 13: _t->searchReplace((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 14: _t->searchReplace((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 15: _t->searchReplace((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 16: _t->slotCollapseItems(); break;
        case 17: _t->recursiveCollapseItems((*reinterpret_cast< ClassEditorTreeWidgetItem*(*)>(_a[1]))); break;
        case 18: _t->removeSelectedItems(); break;
        case 19: _t->slotReplaceAll((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject ClassEditorWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ClassEditorWidget.data,
      qt_meta_data_ClassEditorWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ClassEditorWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ClassEditorWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ClassEditorWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ClassEditorWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 20)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 20;
    }
    return _id;
}
struct qt_meta_stringdata_ClassEditorWindow_t {
    QByteArrayData data[5];
    char stringdata0[58];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ClassEditorWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ClassEditorWindow_t qt_meta_stringdata_ClassEditorWindow = {
    {
QT_MOC_LITERAL(0, 0, 17), // "ClassEditorWindow"
QT_MOC_LITERAL(1, 18, 13), // "cancelClicked"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 12), // "buildClicked"
QT_MOC_LITERAL(4, 46, 11) // "saveClicked"

    },
    "ClassEditorWindow\0cancelClicked\0\0"
    "buildClicked\0saveClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ClassEditorWindow[] = {

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

void ClassEditorWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ClassEditorWindow *_t = static_cast<ClassEditorWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->cancelClicked(); break;
        case 1: _t->buildClicked(); break;
        case 2: _t->saveClicked(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject ClassEditorWindow::staticMetaObject = {
    { &KviWindow::staticMetaObject, qt_meta_stringdata_ClassEditorWindow.data,
      qt_meta_data_ClassEditorWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ClassEditorWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ClassEditorWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ClassEditorWindow.stringdata0))
        return static_cast<void*>(this);
    return KviWindow::qt_metacast(_clname);
}

int ClassEditorWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
struct qt_meta_stringdata_KviClassEditorDialog_t {
    QByteArrayData data[3];
    char stringdata0[34];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviClassEditorDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviClassEditorDialog_t qt_meta_stringdata_KviClassEditorDialog = {
    {
QT_MOC_LITERAL(0, 0, 20), // "KviClassEditorDialog"
QT_MOC_LITERAL(1, 21, 11), // "textChanged"
QT_MOC_LITERAL(2, 33, 0) // ""

    },
    "KviClassEditorDialog\0textChanged\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviClassEditorDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   19,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    2,

       0        // eod
};

void KviClassEditorDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviClassEditorDialog *_t = static_cast<KviClassEditorDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject KviClassEditorDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_KviClassEditorDialog.data,
      qt_meta_data_KviClassEditorDialog,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviClassEditorDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviClassEditorDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviClassEditorDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int KviClassEditorDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_KviClassEditorFunctionDialog_t {
    QByteArrayData data[3];
    char stringdata0[42];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviClassEditorFunctionDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviClassEditorFunctionDialog_t qt_meta_stringdata_KviClassEditorFunctionDialog = {
    {
QT_MOC_LITERAL(0, 0, 28), // "KviClassEditorFunctionDialog"
QT_MOC_LITERAL(1, 29, 11), // "textChanged"
QT_MOC_LITERAL(2, 41, 0) // ""

    },
    "KviClassEditorFunctionDialog\0textChanged\0"
    ""
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviClassEditorFunctionDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   19,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    2,

       0        // eod
};

void KviClassEditorFunctionDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviClassEditorFunctionDialog *_t = static_cast<KviClassEditorFunctionDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject KviClassEditorFunctionDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_KviClassEditorFunctionDialog.data,
      qt_meta_data_KviClassEditorFunctionDialog,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviClassEditorFunctionDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviClassEditorFunctionDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviClassEditorFunctionDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int KviClassEditorFunctionDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
