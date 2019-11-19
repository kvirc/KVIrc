/****************************************************************************
** Meta object code from reading C++ file 'HelpWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../HelpWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'HelpWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_HelpWindow_t {
    QByteArrayData data[15];
    char stringdata0[175];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_HelpWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_HelpWindow_t qt_meta_stringdata_HelpWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "HelpWindow"
QT_MOC_LITERAL(1, 11, 13), // "indexSelected"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(4, 43, 13), // "searchInIndex"
QT_MOC_LITERAL(5, 57, 1), // "s"
QT_MOC_LITERAL(6, 59, 14), // "showIndexTopic"
QT_MOC_LITERAL(7, 74, 11), // "startSearch"
QT_MOC_LITERAL(8, 86, 14), // "searchSelected"
QT_MOC_LITERAL(9, 101, 12), // "refreshIndex"
QT_MOC_LITERAL(10, 114, 12), // "initialSetup"
QT_MOC_LITERAL(11, 127, 13), // "indexingStart"
QT_MOC_LITERAL(12, 141, 4), // "iNum"
QT_MOC_LITERAL(13, 146, 16), // "indexingProgress"
QT_MOC_LITERAL(14, 163, 11) // "indexingEnd"

    },
    "HelpWindow\0indexSelected\0\0QListWidgetItem*\0"
    "searchInIndex\0s\0showIndexTopic\0"
    "startSearch\0searchSelected\0refreshIndex\0"
    "initialSetup\0indexingStart\0iNum\0"
    "indexingProgress\0indexingEnd"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_HelpWindow[] = {

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
       1,    1,   64,    2, 0x0a /* Public */,
       4,    1,   67,    2, 0x0a /* Public */,
       6,    0,   70,    2, 0x0a /* Public */,
       7,    0,   71,    2, 0x0a /* Public */,
       8,    1,   72,    2, 0x0a /* Public */,
       9,    0,   75,    2, 0x0a /* Public */,
      10,    0,   76,    2, 0x0a /* Public */,
      11,    1,   77,    2, 0x0a /* Public */,
      13,    1,   80,    2, 0x0a /* Public */,
      14,    0,   83,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void,

       0        // eod
};

void HelpWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        HelpWindow *_t = static_cast<HelpWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->indexSelected((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 1: _t->searchInIndex((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->showIndexTopic(); break;
        case 3: _t->startSearch(); break;
        case 4: _t->searchSelected((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 5: _t->refreshIndex(); break;
        case 6: _t->initialSetup(); break;
        case 7: _t->indexingStart((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->indexingProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->indexingEnd(); break;
        default: ;
        }
    }
}

const QMetaObject HelpWindow::staticMetaObject = {
    { &KviWindow::staticMetaObject, qt_meta_stringdata_HelpWindow.data,
      qt_meta_data_HelpWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *HelpWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HelpWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_HelpWindow.stringdata0))
        return static_cast<void*>(this);
    return KviWindow::qt_metacast(_clname);
}

int HelpWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviWindow::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
