/****************************************************************************
** Meta object code from reading C++ file 'OptionsWidget_servers.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../OptionsWidget_servers.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'OptionsWidget_servers.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_IrcNetworkDetailsWidget_t {
    QByteArrayData data[6];
    char stringdata0[104];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_IrcNetworkDetailsWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_IrcNetworkDetailsWidget_t qt_meta_stringdata_IrcNetworkDetailsWidget = {
    {
QT_MOC_LITERAL(0, 0, 23), // "IrcNetworkDetailsWidget"
QT_MOC_LITERAL(1, 24, 29), // "enableDisableNickServControls"
QT_MOC_LITERAL(2, 54, 0), // ""
QT_MOC_LITERAL(3, 55, 15), // "addNickServRule"
QT_MOC_LITERAL(4, 71, 15), // "delNickServRule"
QT_MOC_LITERAL(5, 87, 16) // "editNickServRule"

    },
    "IrcNetworkDetailsWidget\0"
    "enableDisableNickServControls\0\0"
    "addNickServRule\0delNickServRule\0"
    "editNickServRule"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_IrcNetworkDetailsWidget[] = {

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
       1,    0,   34,    2, 0x09 /* Protected */,
       3,    0,   35,    2, 0x09 /* Protected */,
       4,    0,   36,    2, 0x09 /* Protected */,
       5,    0,   37,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void IrcNetworkDetailsWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        IrcNetworkDetailsWidget *_t = static_cast<IrcNetworkDetailsWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->enableDisableNickServControls(); break;
        case 1: _t->addNickServRule(); break;
        case 2: _t->delNickServRule(); break;
        case 3: _t->editNickServRule(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject IrcNetworkDetailsWidget::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_IrcNetworkDetailsWidget.data,
      qt_meta_data_IrcNetworkDetailsWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *IrcNetworkDetailsWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *IrcNetworkDetailsWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_IrcNetworkDetailsWidget.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int IrcNetworkDetailsWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_IrcServerDetailsWidget_t {
    QByteArrayData data[7];
    char stringdata0[135];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_IrcServerDetailsWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_IrcServerDetailsWidget_t qt_meta_stringdata_IrcServerDetailsWidget = {
    {
QT_MOC_LITERAL(0, 0, 22), // "IrcServerDetailsWidget"
QT_MOC_LITERAL(1, 23, 26), // "useDefaultInitUModeToggled"
QT_MOC_LITERAL(2, 50, 0), // ""
QT_MOC_LITERAL(3, 51, 21), // "portEditorTextChanged"
QT_MOC_LITERAL(4, 73, 22), // "useCacheIpCheckToggled"
QT_MOC_LITERAL(5, 96, 19), // "useIPV6CheckToggled"
QT_MOC_LITERAL(6, 116, 18) // "useSSLCheckToggled"

    },
    "IrcServerDetailsWidget\0"
    "useDefaultInitUModeToggled\0\0"
    "portEditorTextChanged\0useCacheIpCheckToggled\0"
    "useIPV6CheckToggled\0useSSLCheckToggled"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_IrcServerDetailsWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x09 /* Protected */,
       3,    1,   42,    2, 0x09 /* Protected */,
       4,    1,   45,    2, 0x09 /* Protected */,
       5,    1,   48,    2, 0x09 /* Protected */,
       6,    1,   51,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,

       0        // eod
};

void IrcServerDetailsWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        IrcServerDetailsWidget *_t = static_cast<IrcServerDetailsWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->useDefaultInitUModeToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->portEditorTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->useCacheIpCheckToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->useIPV6CheckToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->useSSLCheckToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject IrcServerDetailsWidget::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_IrcServerDetailsWidget.data,
      qt_meta_data_IrcServerDetailsWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *IrcServerDetailsWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *IrcServerDetailsWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_IrcServerDetailsWidget.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int IrcServerDetailsWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}
struct qt_meta_stringdata_OptionsWidget_servers_t {
    QByteArrayData data[33];
    char stringdata0[499];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_OptionsWidget_servers_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_OptionsWidget_servers_t qt_meta_stringdata_OptionsWidget_servers = {
    {
QT_MOC_LITERAL(0, 0, 21), // "OptionsWidget_servers"
QT_MOC_LITERAL(1, 22, 44), // "slotShowThisDialogAtStartupSe..."
QT_MOC_LITERAL(2, 67, 0), // ""
QT_MOC_LITERAL(3, 68, 12), // "importerDead"
QT_MOC_LITERAL(4, 81, 12), // "importServer"
QT_MOC_LITERAL(5, 94, 12), // "KviIrcServer"
QT_MOC_LITERAL(6, 107, 1), // "s"
QT_MOC_LITERAL(7, 109, 7), // "network"
QT_MOC_LITERAL(8, 117, 22), // "importPopupAboutToShow"
QT_MOC_LITERAL(9, 140, 18), // "currentItemChanged"
QT_MOC_LITERAL(10, 159, 16), // "QTreeWidgetItem*"
QT_MOC_LITERAL(11, 176, 3), // "cur"
QT_MOC_LITERAL(12, 180, 4), // "prev"
QT_MOC_LITERAL(13, 185, 26), // "customContextMenuRequested"
QT_MOC_LITERAL(14, 212, 3), // "pnt"
QT_MOC_LITERAL(15, 216, 10), // "newNetwork"
QT_MOC_LITERAL(16, 227, 13), // "removeCurrent"
QT_MOC_LITERAL(17, 241, 9), // "newServer"
QT_MOC_LITERAL(18, 251, 12), // "updateFilter"
QT_MOC_LITERAL(19, 264, 14), // "favoriteServer"
QT_MOC_LITERAL(20, 279, 10), // "copyServer"
QT_MOC_LITERAL(21, 290, 11), // "pasteServer"
QT_MOC_LITERAL(22, 302, 9), // "clearList"
QT_MOC_LITERAL(23, 312, 17), // "itemDoubleClicked"
QT_MOC_LITERAL(24, 330, 14), // "detailsClicked"
QT_MOC_LITERAL(25, 345, 21), // "connectCurrentClicked"
QT_MOC_LITERAL(26, 367, 29), // "recentServersPopupAboutToShow"
QT_MOC_LITERAL(27, 397, 25), // "recentServersPopupClicked"
QT_MOC_LITERAL(28, 423, 8), // "QAction*"
QT_MOC_LITERAL(29, 432, 7), // "pAction"
QT_MOC_LITERAL(30, 440, 20), // "importPopupActivated"
QT_MOC_LITERAL(31, 461, 27), // "serverNetworkEditTextEdited"
QT_MOC_LITERAL(32, 489, 9) // "szNewText"

    },
    "OptionsWidget_servers\0"
    "slotShowThisDialogAtStartupSelectorDestroyed\0"
    "\0importerDead\0importServer\0KviIrcServer\0"
    "s\0network\0importPopupAboutToShow\0"
    "currentItemChanged\0QTreeWidgetItem*\0"
    "cur\0prev\0customContextMenuRequested\0"
    "pnt\0newNetwork\0removeCurrent\0newServer\0"
    "updateFilter\0favoriteServer\0copyServer\0"
    "pasteServer\0clearList\0itemDoubleClicked\0"
    "detailsClicked\0connectCurrentClicked\0"
    "recentServersPopupAboutToShow\0"
    "recentServersPopupClicked\0QAction*\0"
    "pAction\0importPopupActivated\0"
    "serverNetworkEditTextEdited\0szNewText"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_OptionsWidget_servers[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      21,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  119,    2, 0x09 /* Protected */,
       3,    0,  120,    2, 0x09 /* Protected */,
       4,    2,  121,    2, 0x09 /* Protected */,
       8,    0,  126,    2, 0x09 /* Protected */,
       9,    2,  127,    2, 0x09 /* Protected */,
      13,    1,  132,    2, 0x09 /* Protected */,
      15,    0,  135,    2, 0x09 /* Protected */,
      16,    0,  136,    2, 0x09 /* Protected */,
      17,    0,  137,    2, 0x09 /* Protected */,
      18,    0,  138,    2, 0x09 /* Protected */,
      19,    0,  139,    2, 0x09 /* Protected */,
      20,    0,  140,    2, 0x09 /* Protected */,
      21,    0,  141,    2, 0x09 /* Protected */,
      22,    0,  142,    2, 0x09 /* Protected */,
      23,    2,  143,    2, 0x09 /* Protected */,
      24,    0,  148,    2, 0x09 /* Protected */,
      25,    0,  149,    2, 0x09 /* Protected */,
      26,    0,  150,    2, 0x09 /* Protected */,
      27,    1,  151,    2, 0x09 /* Protected */,
      30,    1,  154,    2, 0x09 /* Protected */,
      31,    1,  157,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5, QMetaType::QString,    6,    7,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 10, 0x80000000 | 10,   11,   12,
    QMetaType::Void, QMetaType::QPoint,   14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 10, QMetaType::Int,    2,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 28,   29,
    QMetaType::Void, 0x80000000 | 28,   29,
    QMetaType::Void, QMetaType::QString,   32,

       0        // eod
};

void OptionsWidget_servers::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        OptionsWidget_servers *_t = static_cast<OptionsWidget_servers *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->slotShowThisDialogAtStartupSelectorDestroyed(); break;
        case 1: _t->importerDead(); break;
        case 2: _t->importServer((*reinterpret_cast< const KviIrcServer(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 3: _t->importPopupAboutToShow(); break;
        case 4: _t->currentItemChanged((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QTreeWidgetItem*(*)>(_a[2]))); break;
        case 5: _t->customContextMenuRequested((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 6: _t->newNetwork(); break;
        case 7: _t->removeCurrent(); break;
        case 8: _t->newServer(); break;
        case 9: _t->updateFilter(); break;
        case 10: _t->favoriteServer(); break;
        case 11: _t->copyServer(); break;
        case 12: _t->pasteServer(); break;
        case 13: _t->clearList(); break;
        case 14: _t->itemDoubleClicked((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 15: _t->detailsClicked(); break;
        case 16: _t->connectCurrentClicked(); break;
        case 17: _t->recentServersPopupAboutToShow(); break;
        case 18: _t->recentServersPopupClicked((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 19: _t->importPopupActivated((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 20: _t->serverNetworkEditTextEdited((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject OptionsWidget_servers::staticMetaObject = {
    { &KviOptionsWidget::staticMetaObject, qt_meta_stringdata_OptionsWidget_servers.data,
      qt_meta_data_OptionsWidget_servers,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *OptionsWidget_servers::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OptionsWidget_servers::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_OptionsWidget_servers.stringdata0))
        return static_cast<void*>(this);
    return KviOptionsWidget::qt_metacast(_clname);
}

int OptionsWidget_servers::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviOptionsWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 21)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 21;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 21)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 21;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
