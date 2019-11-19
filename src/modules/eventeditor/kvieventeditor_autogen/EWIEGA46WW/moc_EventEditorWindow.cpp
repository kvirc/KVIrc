/****************************************************************************
** Meta object code from reading C++ file 'EventEditorWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../EventEditorWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EventEditorWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_EventEditorTreeWidget_t {
    QByteArrayData data[4];
    char stringdata0[59];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_EventEditorTreeWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_EventEditorTreeWidget_t qt_meta_stringdata_EventEditorTreeWidget = {
    {
QT_MOC_LITERAL(0, 0, 21), // "EventEditorTreeWidget"
QT_MOC_LITERAL(1, 22, 18), // "rightButtonPressed"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 16) // "QTreeWidgetItem*"

    },
    "EventEditorTreeWidget\0rightButtonPressed\0"
    "\0QTreeWidgetItem*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_EventEditorTreeWidget[] = {

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

void EventEditorTreeWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        EventEditorTreeWidget *_t = static_cast<EventEditorTreeWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->rightButtonPressed((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QPoint(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (EventEditorTreeWidget::*_t)(QTreeWidgetItem * , QPoint );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&EventEditorTreeWidget::rightButtonPressed)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject EventEditorTreeWidget::staticMetaObject = {
    { &QTreeWidget::staticMetaObject, qt_meta_stringdata_EventEditorTreeWidget.data,
      qt_meta_data_EventEditorTreeWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *EventEditorTreeWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *EventEditorTreeWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_EventEditorTreeWidget.stringdata0))
        return static_cast<void*>(this);
    return QTreeWidget::qt_metacast(_clname);
}

int EventEditorTreeWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void EventEditorTreeWidget::rightButtonPressed(QTreeWidgetItem * _t1, QPoint _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_EventEditor_t {
    QByteArrayData data[14];
    char stringdata0[208];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_EventEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_EventEditor_t qt_meta_stringdata_EventEditor = {
    {
QT_MOC_LITERAL(0, 0, 11), // "EventEditor"
QT_MOC_LITERAL(1, 12, 18), // "currentItemChanged"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 16), // "QTreeWidgetItem*"
QT_MOC_LITERAL(4, 49, 2), // "it"
QT_MOC_LITERAL(5, 52, 11), // "itemPressed"
QT_MOC_LITERAL(6, 64, 3), // "pnt"
QT_MOC_LITERAL(7, 68, 27), // "toggleCurrentHandlerEnabled"
QT_MOC_LITERAL(8, 96, 20), // "removeCurrentHandler"
QT_MOC_LITERAL(9, 117, 25), // "addHandlerForCurrentEvent"
QT_MOC_LITERAL(10, 143, 15), // "exportAllEvents"
QT_MOC_LITERAL(11, 159, 20), // "exportCurrentHandler"
QT_MOC_LITERAL(12, 180, 20), // "eventHandlerDisabled"
QT_MOC_LITERAL(13, 201, 6) // "szName"

    },
    "EventEditor\0currentItemChanged\0\0"
    "QTreeWidgetItem*\0it\0itemPressed\0pnt\0"
    "toggleCurrentHandlerEnabled\0"
    "removeCurrentHandler\0addHandlerForCurrentEvent\0"
    "exportAllEvents\0exportCurrentHandler\0"
    "eventHandlerDisabled\0szName"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_EventEditor[] = {

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
       5,    2,   59,    2, 0x09 /* Protected */,
       7,    0,   64,    2, 0x09 /* Protected */,
       8,    0,   65,    2, 0x09 /* Protected */,
       9,    0,   66,    2, 0x09 /* Protected */,
      10,    0,   67,    2, 0x09 /* Protected */,
      11,    0,   68,    2, 0x09 /* Protected */,
      12,    1,   69,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    2,
    QMetaType::Void, 0x80000000 | 3, QMetaType::QPoint,    4,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   13,

       0        // eod
};

void EventEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        EventEditor *_t = static_cast<EventEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->currentItemChanged((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QTreeWidgetItem*(*)>(_a[2]))); break;
        case 1: _t->itemPressed((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2]))); break;
        case 2: _t->toggleCurrentHandlerEnabled(); break;
        case 3: _t->removeCurrentHandler(); break;
        case 4: _t->addHandlerForCurrentEvent(); break;
        case 5: _t->exportAllEvents(); break;
        case 6: _t->exportCurrentHandler(); break;
        case 7: _t->eventHandlerDisabled((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject EventEditor::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_EventEditor.data,
      qt_meta_data_EventEditor,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *EventEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *EventEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_EventEditor.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int EventEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
struct qt_meta_stringdata_EventEditorWindow_t {
    QByteArrayData data[5];
    char stringdata0[56];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_EventEditorWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_EventEditorWindow_t qt_meta_stringdata_EventEditorWindow = {
    {
QT_MOC_LITERAL(0, 0, 17), // "EventEditorWindow"
QT_MOC_LITERAL(1, 18, 13), // "cancelClicked"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 9), // "okClicked"
QT_MOC_LITERAL(4, 43, 12) // "applyClicked"

    },
    "EventEditorWindow\0cancelClicked\0\0"
    "okClicked\0applyClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_EventEditorWindow[] = {

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

void EventEditorWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        EventEditorWindow *_t = static_cast<EventEditorWindow *>(_o);
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

const QMetaObject EventEditorWindow::staticMetaObject = {
    { &KviWindow::staticMetaObject, qt_meta_stringdata_EventEditorWindow.data,
      qt_meta_data_EventEditorWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *EventEditorWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *EventEditorWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_EventEditorWindow.stringdata0))
        return static_cast<void*>(this);
    return KviWindow::qt_metacast(_clname);
}

int EventEditorWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
