/****************************************************************************
** Meta object code from reading C++ file 'KviChannelWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ui/KviChannelWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KviChannelWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviChannelWindow_t {
    QByteArrayData data[19];
    char stringdata0[261];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviChannelWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviChannelWindow_t qt_meta_stringdata_KviChannelWindow = {
    {
QT_MOC_LITERAL(0, 0, 16), // "KviChannelWindow"
QT_MOC_LITERAL(1, 17, 15), // "opStatusChanged"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 16), // "toggleDoubleView"
QT_MOC_LITERAL(4, 51, 14), // "toggleListView"
QT_MOC_LITERAL(5, 66, 20), // "toggleListModeEditor"
QT_MOC_LITERAL(6, 87, 16), // "toggleModeEditor"
QT_MOC_LITERAL(7, 104, 16), // "modeSelectorDone"
QT_MOC_LITERAL(8, 121, 13), // "topicSelected"
QT_MOC_LITERAL(9, 135, 7), // "szTopic"
QT_MOC_LITERAL(10, 143, 7), // "setMode"
QT_MOC_LITERAL(11, 151, 6), // "szMode"
QT_MOC_LITERAL(12, 158, 20), // "textViewRightClicked"
QT_MOC_LITERAL(13, 179, 11), // "removeMasks"
QT_MOC_LITERAL(14, 191, 14), // "KviMaskEditor*"
QT_MOC_LITERAL(15, 206, 3), // "pEd"
QT_MOC_LITERAL(16, 210, 26), // "std::vector<KviMaskEntry*>"
QT_MOC_LITERAL(17, 237, 5), // "pList"
QT_MOC_LITERAL(18, 243, 17) // "toggleToolButtons"

    },
    "KviChannelWindow\0opStatusChanged\0\0"
    "toggleDoubleView\0toggleListView\0"
    "toggleListModeEditor\0toggleModeEditor\0"
    "modeSelectorDone\0topicSelected\0szTopic\0"
    "setMode\0szMode\0textViewRightClicked\0"
    "removeMasks\0KviMaskEditor*\0pEd\0"
    "std::vector<KviMaskEntry*>\0pList\0"
    "toggleToolButtons"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviChannelWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   70,    2, 0x0a /* Public */,
       4,    0,   71,    2, 0x08 /* Private */,
       5,    0,   72,    2, 0x08 /* Private */,
       6,    0,   73,    2, 0x08 /* Private */,
       7,    0,   74,    2, 0x08 /* Private */,
       8,    1,   75,    2, 0x08 /* Private */,
      10,    1,   78,    2, 0x08 /* Private */,
      12,    0,   81,    2, 0x08 /* Private */,
      13,    2,   82,    2, 0x08 /* Private */,
      18,    0,   87,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 14, 0x80000000 | 16,   15,   17,
    QMetaType::Void,

       0        // eod
};

void KviChannelWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviChannelWindow *_t = static_cast<KviChannelWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->opStatusChanged(); break;
        case 1: _t->toggleDoubleView(); break;
        case 2: _t->toggleListView(); break;
        case 3: _t->toggleListModeEditor(); break;
        case 4: _t->toggleModeEditor(); break;
        case 5: _t->modeSelectorDone(); break;
        case 6: _t->topicSelected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->setMode((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->textViewRightClicked(); break;
        case 9: _t->removeMasks((*reinterpret_cast< KviMaskEditor*(*)>(_a[1])),(*reinterpret_cast< const std::vector<KviMaskEntry*>(*)>(_a[2]))); break;
        case 10: _t->toggleToolButtons(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (KviChannelWindow::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviChannelWindow::opStatusChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject KviChannelWindow::staticMetaObject = {
    { &KviWindow::staticMetaObject, qt_meta_stringdata_KviChannelWindow.data,
      qt_meta_data_KviChannelWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviChannelWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviChannelWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviChannelWindow.stringdata0))
        return static_cast<void*>(this);
    return KviWindow::qt_metacast(_clname);
}

int KviChannelWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void KviChannelWindow::opStatusChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
