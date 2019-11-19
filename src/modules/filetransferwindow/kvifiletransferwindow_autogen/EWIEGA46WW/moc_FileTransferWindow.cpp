/****************************************************************************
** Meta object code from reading C++ file 'FileTransferWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../FileTransferWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FileTransferWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_FileTransferWidget_t {
    QByteArrayData data[7];
    char stringdata0[95];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FileTransferWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FileTransferWidget_t qt_meta_stringdata_FileTransferWidget = {
    {
QT_MOC_LITERAL(0, 0, 18), // "FileTransferWidget"
QT_MOC_LITERAL(1, 19, 18), // "rightButtonPressed"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 17), // "FileTransferItem*"
QT_MOC_LITERAL(4, 57, 3), // "pnt"
QT_MOC_LITERAL(5, 61, 13), // "doubleClicked"
QT_MOC_LITERAL(6, 75, 19) // "TransparencyCapable"

    },
    "FileTransferWidget\0rightButtonPressed\0"
    "\0FileTransferItem*\0pnt\0doubleClicked\0"
    "TransparencyCapable"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FileTransferWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       1,   34, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   24,    2, 0x06 /* Public */,
       5,    2,   29,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::QPoint,    2,    4,
    QMetaType::Void, 0x80000000 | 3, QMetaType::QPoint,    2,    4,

 // properties: name, type, flags
       6, QMetaType::Int, 0x00095001,

       0        // eod
};

void FileTransferWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FileTransferWidget *_t = static_cast<FileTransferWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->rightButtonPressed((*reinterpret_cast< FileTransferItem*(*)>(_a[1])),(*reinterpret_cast< QPoint(*)>(_a[2]))); break;
        case 1: _t->doubleClicked((*reinterpret_cast< FileTransferItem*(*)>(_a[1])),(*reinterpret_cast< QPoint(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (FileTransferWidget::*_t)(FileTransferItem * , QPoint );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileTransferWidget::rightButtonPressed)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (FileTransferWidget::*_t)(FileTransferItem * , QPoint );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileTransferWidget::doubleClicked)) {
                *result = 1;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        FileTransferWidget *_t = static_cast<FileTransferWidget *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->dummyRead(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject FileTransferWidget::staticMetaObject = {
    { &KviTalTableWidget::staticMetaObject, qt_meta_stringdata_FileTransferWidget.data,
      qt_meta_data_FileTransferWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *FileTransferWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FileTransferWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FileTransferWidget.stringdata0))
        return static_cast<void*>(this);
    return KviTalTableWidget::qt_metacast(_clname);
}

int FileTransferWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviTalTableWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
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
void FileTransferWidget::rightButtonPressed(FileTransferItem * _t1, QPoint _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void FileTransferWidget::doubleClicked(FileTransferItem * _t1, QPoint _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
struct qt_meta_stringdata_FileTransferItemDelegate_t {
    QByteArrayData data[1];
    char stringdata0[25];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FileTransferItemDelegate_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FileTransferItemDelegate_t qt_meta_stringdata_FileTransferItemDelegate = {
    {
QT_MOC_LITERAL(0, 0, 24) // "FileTransferItemDelegate"

    },
    "FileTransferItemDelegate"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FileTransferItemDelegate[] = {

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

void FileTransferItemDelegate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject FileTransferItemDelegate::staticMetaObject = {
    { &KviTalIconAndRichTextItemDelegate::staticMetaObject, qt_meta_stringdata_FileTransferItemDelegate.data,
      qt_meta_data_FileTransferItemDelegate,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *FileTransferItemDelegate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FileTransferItemDelegate::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FileTransferItemDelegate.stringdata0))
        return static_cast<void*>(this);
    return KviTalIconAndRichTextItemDelegate::qt_metacast(_clname);
}

int FileTransferItemDelegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviTalIconAndRichTextItemDelegate::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_FileTransferWindow_t {
    QByteArrayData data[26];
    char stringdata0[362];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FileTransferWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FileTransferWindow_t qt_meta_stringdata_FileTransferWindow = {
    {
QT_MOC_LITERAL(0, 0, 18), // "FileTransferWindow"
QT_MOC_LITERAL(1, 19, 18), // "transferRegistered"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 16), // "KviFileTransfer*"
QT_MOC_LITERAL(4, 56, 1), // "t"
QT_MOC_LITERAL(5, 58, 21), // "transferUnregistering"
QT_MOC_LITERAL(6, 80, 18), // "rightButtonPressed"
QT_MOC_LITERAL(7, 99, 17), // "FileTransferItem*"
QT_MOC_LITERAL(8, 117, 2), // "it"
QT_MOC_LITERAL(9, 120, 3), // "pnt"
QT_MOC_LITERAL(10, 124, 13), // "doubleClicked"
QT_MOC_LITERAL(11, 138, 9), // "heartbeat"
QT_MOC_LITERAL(12, 148, 15), // "clearTerminated"
QT_MOC_LITERAL(13, 164, 8), // "clearAll"
QT_MOC_LITERAL(14, 173, 10), // "tipRequest"
QT_MOC_LITERAL(15, 184, 18), // "KviDynamicToolTip*"
QT_MOC_LITERAL(16, 203, 3), // "tip"
QT_MOC_LITERAL(17, 207, 13), // "openLocalFile"
QT_MOC_LITERAL(18, 221, 17), // "openLocalFileWith"
QT_MOC_LITERAL(19, 239, 24), // "copyLocalFileToClipboard"
QT_MOC_LITERAL(20, 264, 19), // "openLocalFileFolder"
QT_MOC_LITERAL(21, 284, 22), // "openFilePopupActivated"
QT_MOC_LITERAL(22, 307, 8), // "QAction*"
QT_MOC_LITERAL(23, 316, 7), // "pAction"
QT_MOC_LITERAL(24, 324, 21), // "openLocalFileTerminal"
QT_MOC_LITERAL(25, 346, 15) // "deleteLocalFile"

    },
    "FileTransferWindow\0transferRegistered\0"
    "\0KviFileTransfer*\0t\0transferUnregistering\0"
    "rightButtonPressed\0FileTransferItem*\0"
    "it\0pnt\0doubleClicked\0heartbeat\0"
    "clearTerminated\0clearAll\0tipRequest\0"
    "KviDynamicToolTip*\0tip\0openLocalFile\0"
    "openLocalFileWith\0copyLocalFileToClipboard\0"
    "openLocalFileFolder\0openFilePopupActivated\0"
    "QAction*\0pAction\0openLocalFileTerminal\0"
    "deleteLocalFile"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FileTransferWindow[] = {

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
       1,    1,   89,    2, 0x09 /* Protected */,
       5,    1,   92,    2, 0x09 /* Protected */,
       6,    2,   95,    2, 0x09 /* Protected */,
      10,    2,  100,    2, 0x09 /* Protected */,
      11,    0,  105,    2, 0x09 /* Protected */,
      12,    0,  106,    2, 0x09 /* Protected */,
      13,    0,  107,    2, 0x09 /* Protected */,
      14,    2,  108,    2, 0x09 /* Protected */,
      17,    0,  113,    2, 0x09 /* Protected */,
      18,    0,  114,    2, 0x09 /* Protected */,
      19,    0,  115,    2, 0x09 /* Protected */,
      20,    0,  116,    2, 0x09 /* Protected */,
      21,    1,  117,    2, 0x09 /* Protected */,
      24,    0,  120,    2, 0x09 /* Protected */,
      25,    0,  121,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 7, QMetaType::QPoint,    8,    9,
    QMetaType::Void, 0x80000000 | 7, QMetaType::QPoint,    8,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 15, QMetaType::QPoint,   16,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 22,   23,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void FileTransferWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FileTransferWindow *_t = static_cast<FileTransferWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->transferRegistered((*reinterpret_cast< KviFileTransfer*(*)>(_a[1]))); break;
        case 1: _t->transferUnregistering((*reinterpret_cast< KviFileTransfer*(*)>(_a[1]))); break;
        case 2: _t->rightButtonPressed((*reinterpret_cast< FileTransferItem*(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2]))); break;
        case 3: _t->doubleClicked((*reinterpret_cast< FileTransferItem*(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2]))); break;
        case 4: _t->heartbeat(); break;
        case 5: _t->clearTerminated(); break;
        case 6: _t->clearAll(); break;
        case 7: _t->tipRequest((*reinterpret_cast< KviDynamicToolTip*(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2]))); break;
        case 8: _t->openLocalFile(); break;
        case 9: _t->openLocalFileWith(); break;
        case 10: _t->copyLocalFileToClipboard(); break;
        case 11: _t->openLocalFileFolder(); break;
        case 12: _t->openFilePopupActivated((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 13: _t->openLocalFileTerminal(); break;
        case 14: _t->deleteLocalFile(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< KviFileTransfer* >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< KviFileTransfer* >(); break;
            }
            break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< KviDynamicToolTip* >(); break;
            }
            break;
        case 12:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAction* >(); break;
            }
            break;
        }
    }
}

const QMetaObject FileTransferWindow::staticMetaObject = {
    { &KviWindow::staticMetaObject, qt_meta_stringdata_FileTransferWindow.data,
      qt_meta_data_FileTransferWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *FileTransferWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FileTransferWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FileTransferWindow.stringdata0))
        return static_cast<void*>(this);
    return KviWindow::qt_metacast(_clname);
}

int FileTransferWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviWindow::qt_metacall(_c, _id, _a);
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
