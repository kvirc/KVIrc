/****************************************************************************
** Meta object code from reading C++ file 'ActionEditor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ActionEditor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ActionEditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ActionEditorTreeView_t {
    QByteArrayData data[1];
    char stringdata0[21];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ActionEditorTreeView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ActionEditorTreeView_t qt_meta_stringdata_ActionEditorTreeView = {
    {
QT_MOC_LITERAL(0, 0, 20) // "ActionEditorTreeView"

    },
    "ActionEditorTreeView"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ActionEditorTreeView[] = {

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

void ActionEditorTreeView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject ActionEditorTreeView::staticMetaObject = {
    { &QTreeWidget::staticMetaObject, qt_meta_stringdata_ActionEditorTreeView.data,
      qt_meta_data_ActionEditorTreeView,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ActionEditorTreeView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ActionEditorTreeView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ActionEditorTreeView.stringdata0))
        return static_cast<void*>(this);
    return QTreeWidget::qt_metacast(_clname);
}

int ActionEditorTreeView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeWidget::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_SingleActionEditor_t {
    QByteArrayData data[13];
    char stringdata0[251];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SingleActionEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SingleActionEditor_t qt_meta_stringdata_SingleActionEditor = {
    {
QT_MOC_LITERAL(0, 0, 18), // "SingleActionEditor"
QT_MOC_LITERAL(1, 19, 15), // "chooseSmallIcon"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 13), // "chooseBigIcon"
QT_MOC_LITERAL(4, 50, 16), // "displaySmallIcon"
QT_MOC_LITERAL(5, 67, 8), // "szIconId"
QT_MOC_LITERAL(6, 76, 14), // "displayBigIcon"
QT_MOC_LITERAL(7, 91, 24), // "needsContextCheckToggled"
QT_MOC_LITERAL(8, 116, 27), // "needsConnectionCheckToggled"
QT_MOC_LITERAL(9, 144, 27), // "specificWindowsCheckToggled"
QT_MOC_LITERAL(10, 172, 26), // "channelWindowsCheckToggled"
QT_MOC_LITERAL(11, 199, 26), // "consoleWindowsCheckToggled"
QT_MOC_LITERAL(12, 226, 24) // "queryWindowsCheckToggled"

    },
    "SingleActionEditor\0chooseSmallIcon\0\0"
    "chooseBigIcon\0displaySmallIcon\0szIconId\0"
    "displayBigIcon\0needsContextCheckToggled\0"
    "needsConnectionCheckToggled\0"
    "specificWindowsCheckToggled\0"
    "channelWindowsCheckToggled\0"
    "consoleWindowsCheckToggled\0"
    "queryWindowsCheckToggled"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SingleActionEditor[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x09 /* Protected */,
       3,    0,   65,    2, 0x09 /* Protected */,
       4,    1,   66,    2, 0x09 /* Protected */,
       6,    1,   69,    2, 0x09 /* Protected */,
       7,    1,   72,    2, 0x09 /* Protected */,
       8,    1,   75,    2, 0x09 /* Protected */,
       9,    1,   78,    2, 0x09 /* Protected */,
      10,    1,   81,    2, 0x09 /* Protected */,
      11,    1,   84,    2, 0x09 /* Protected */,
      12,    1,   87,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,

       0        // eod
};

void SingleActionEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SingleActionEditor *_t = static_cast<SingleActionEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->chooseSmallIcon(); break;
        case 1: _t->chooseBigIcon(); break;
        case 2: _t->displaySmallIcon((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->displayBigIcon((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->needsContextCheckToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->needsConnectionCheckToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->specificWindowsCheckToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->channelWindowsCheckToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->consoleWindowsCheckToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->queryWindowsCheckToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject SingleActionEditor::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_SingleActionEditor.data,
      qt_meta_data_SingleActionEditor,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *SingleActionEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SingleActionEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SingleActionEditor.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int SingleActionEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}
struct qt_meta_stringdata_ActionEditor_t {
    QByteArrayData data[9];
    char stringdata0[96];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ActionEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ActionEditor_t qt_meta_stringdata_ActionEditor = {
    {
QT_MOC_LITERAL(0, 0, 12), // "ActionEditor"
QT_MOC_LITERAL(1, 13, 18), // "currentItemChanged"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 16), // "QTreeWidgetItem*"
QT_MOC_LITERAL(4, 50, 2), // "it"
QT_MOC_LITERAL(5, 53, 4), // "prev"
QT_MOC_LITERAL(6, 58, 9), // "newAction"
QT_MOC_LITERAL(7, 68, 13), // "deleteActions"
QT_MOC_LITERAL(8, 82, 13) // "exportActions"

    },
    "ActionEditor\0currentItemChanged\0\0"
    "QTreeWidgetItem*\0it\0prev\0newAction\0"
    "deleteActions\0exportActions"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ActionEditor[] = {

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
       6,    0,   39,    2, 0x09 /* Protected */,
       7,    0,   40,    2, 0x09 /* Protected */,
       8,    0,   41,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ActionEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ActionEditor *_t = static_cast<ActionEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->currentItemChanged((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QTreeWidgetItem*(*)>(_a[2]))); break;
        case 1: _t->newAction(); break;
        case 2: _t->deleteActions(); break;
        case 3: _t->exportActions(); break;
        default: ;
        }
    }
}

const QMetaObject ActionEditor::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ActionEditor.data,
      qt_meta_data_ActionEditor,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ActionEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ActionEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ActionEditor.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ActionEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
struct qt_meta_stringdata_ActionEditorWindow_t {
    QByteArrayData data[5];
    char stringdata0[57];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ActionEditorWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ActionEditorWindow_t qt_meta_stringdata_ActionEditorWindow = {
    {
QT_MOC_LITERAL(0, 0, 18), // "ActionEditorWindow"
QT_MOC_LITERAL(1, 19, 13), // "cancelClicked"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 9), // "okClicked"
QT_MOC_LITERAL(4, 44, 12) // "applyClicked"

    },
    "ActionEditorWindow\0cancelClicked\0\0"
    "okClicked\0applyClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ActionEditorWindow[] = {

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

void ActionEditorWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ActionEditorWindow *_t = static_cast<ActionEditorWindow *>(_o);
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

const QMetaObject ActionEditorWindow::staticMetaObject = {
    { &KviWindow::staticMetaObject, qt_meta_stringdata_ActionEditorWindow.data,
      qt_meta_data_ActionEditorWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ActionEditorWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ActionEditorWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ActionEditorWindow.stringdata0))
        return static_cast<void*>(this);
    return KviWindow::qt_metacast(_clname);
}

int ActionEditorWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
