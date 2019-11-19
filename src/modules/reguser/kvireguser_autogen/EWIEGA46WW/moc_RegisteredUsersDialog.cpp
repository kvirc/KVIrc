/****************************************************************************
** Meta object code from reading C++ file 'RegisteredUsersDialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../RegisteredUsersDialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RegisteredUsersDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviRegisteredUsersListView_t {
    QByteArrayData data[4];
    char stringdata0[64];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviRegisteredUsersListView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviRegisteredUsersListView_t qt_meta_stringdata_KviRegisteredUsersListView = {
    {
QT_MOC_LITERAL(0, 0, 26), // "KviRegisteredUsersListView"
QT_MOC_LITERAL(1, 27, 18), // "rightButtonPressed"
QT_MOC_LITERAL(2, 46, 0), // ""
QT_MOC_LITERAL(3, 47, 16) // "QTreeWidgetItem*"

    },
    "KviRegisteredUsersListView\0"
    "rightButtonPressed\0\0QTreeWidgetItem*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviRegisteredUsersListView[] = {

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

void KviRegisteredUsersListView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviRegisteredUsersListView *_t = static_cast<KviRegisteredUsersListView *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->rightButtonPressed((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QPoint(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (KviRegisteredUsersListView::*_t)(QTreeWidgetItem * , QPoint );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviRegisteredUsersListView::rightButtonPressed)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject KviRegisteredUsersListView::staticMetaObject = {
    { &QTreeWidget::staticMetaObject, qt_meta_stringdata_KviRegisteredUsersListView.data,
      qt_meta_data_KviRegisteredUsersListView,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviRegisteredUsersListView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviRegisteredUsersListView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviRegisteredUsersListView.stringdata0))
        return static_cast<void*>(this);
    return QTreeWidget::qt_metacast(_clname);
}

int KviRegisteredUsersListView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void KviRegisteredUsersListView::rightButtonPressed(QTreeWidgetItem * _t1, QPoint _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_RegisteredUsersDialog_t {
    QByteArrayData data[22];
    char stringdata0[290];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RegisteredUsersDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RegisteredUsersDialog_t qt_meta_stringdata_RegisteredUsersDialog = {
    {
QT_MOC_LITERAL(0, 0, 21), // "RegisteredUsersDialog"
QT_MOC_LITERAL(1, 22, 13), // "cancelClicked"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 9), // "okClicked"
QT_MOC_LITERAL(4, 47, 10), // "addClicked"
QT_MOC_LITERAL(5, 58, 13), // "removeClicked"
QT_MOC_LITERAL(6, 72, 11), // "editClicked"
QT_MOC_LITERAL(7, 84, 16), // "selectionChanged"
QT_MOC_LITERAL(8, 101, 13), // "importClicked"
QT_MOC_LITERAL(9, 115, 13), // "exportClicked"
QT_MOC_LITERAL(10, 129, 16), // "addWizardClicked"
QT_MOC_LITERAL(11, 146, 15), // "addGroupClicked"
QT_MOC_LITERAL(12, 162, 16), // "selectAllClicked"
QT_MOC_LITERAL(13, 179, 11), // "itemPressed"
QT_MOC_LITERAL(14, 191, 16), // "QTreeWidgetItem*"
QT_MOC_LITERAL(15, 208, 2), // "it"
QT_MOC_LITERAL(16, 211, 1), // "c"
QT_MOC_LITERAL(17, 213, 17), // "itemDoubleClicked"
QT_MOC_LITERAL(18, 231, 18), // "rightButtonPressed"
QT_MOC_LITERAL(19, 250, 22), // "moveToGroupMenuClicked"
QT_MOC_LITERAL(20, 273, 8), // "QAction*"
QT_MOC_LITERAL(21, 282, 7) // "pAction"

    },
    "RegisteredUsersDialog\0cancelClicked\0"
    "\0okClicked\0addClicked\0removeClicked\0"
    "editClicked\0selectionChanged\0importClicked\0"
    "exportClicked\0addWizardClicked\0"
    "addGroupClicked\0selectAllClicked\0"
    "itemPressed\0QTreeWidgetItem*\0it\0c\0"
    "itemDoubleClicked\0rightButtonPressed\0"
    "moveToGroupMenuClicked\0QAction*\0pAction"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RegisteredUsersDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   89,    2, 0x09 /* Protected */,
       3,    0,   90,    2, 0x09 /* Protected */,
       4,    0,   91,    2, 0x09 /* Protected */,
       5,    0,   92,    2, 0x09 /* Protected */,
       6,    0,   93,    2, 0x09 /* Protected */,
       7,    0,   94,    2, 0x09 /* Protected */,
       8,    0,   95,    2, 0x09 /* Protected */,
       9,    0,   96,    2, 0x09 /* Protected */,
      10,    0,   97,    2, 0x09 /* Protected */,
      11,    0,   98,    2, 0x09 /* Protected */,
      12,    0,   99,    2, 0x09 /* Protected */,
      13,    2,  100,    2, 0x09 /* Protected */,
      17,    2,  105,    2, 0x09 /* Protected */,
      18,    2,  110,    2, 0x09 /* Protected */,
      19,    1,  115,    2, 0x09 /* Protected */,

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
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 14, QMetaType::Int,   15,   16,
    QMetaType::Void, 0x80000000 | 14, QMetaType::Int,   15,    2,
    QMetaType::Void, 0x80000000 | 14, QMetaType::QPoint,    2,    2,
    QMetaType::Void, 0x80000000 | 20,   21,

       0        // eod
};

void RegisteredUsersDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RegisteredUsersDialog *_t = static_cast<RegisteredUsersDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->cancelClicked(); break;
        case 1: _t->okClicked(); break;
        case 2: _t->addClicked(); break;
        case 3: _t->removeClicked(); break;
        case 4: _t->editClicked(); break;
        case 5: _t->selectionChanged(); break;
        case 6: _t->importClicked(); break;
        case 7: _t->exportClicked(); break;
        case 8: _t->addWizardClicked(); break;
        case 9: _t->addGroupClicked(); break;
        case 10: _t->selectAllClicked(); break;
        case 11: _t->itemPressed((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 12: _t->itemDoubleClicked((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 13: _t->rightButtonPressed((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QPoint(*)>(_a[2]))); break;
        case 14: _t->moveToGroupMenuClicked((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 14:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAction* >(); break;
            }
            break;
        }
    }
}

const QMetaObject RegisteredUsersDialog::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_RegisteredUsersDialog.data,
      qt_meta_data_RegisteredUsersDialog,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *RegisteredUsersDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RegisteredUsersDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RegisteredUsersDialog.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int RegisteredUsersDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
