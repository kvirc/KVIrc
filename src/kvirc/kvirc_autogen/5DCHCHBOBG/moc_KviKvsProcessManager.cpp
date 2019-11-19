/****************************************************************************
** Meta object code from reading C++ file 'KviKvsProcessManager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../kvs/KviKvsProcessManager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KviKvsProcessManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviKvsProcessAsyncOperation_t {
    QByteArrayData data[10];
    char stringdata0[123];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviKvsProcessAsyncOperation_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviKvsProcessAsyncOperation_t qt_meta_stringdata_KviKvsProcessAsyncOperation = {
    {
QT_MOC_LITERAL(0, 0, 27), // "KviKvsProcessAsyncOperation"
QT_MOC_LITERAL(1, 28, 17), // "maxRunTimeExpired"
QT_MOC_LITERAL(2, 46, 0), // ""
QT_MOC_LITERAL(3, 47, 4), // "ping"
QT_MOC_LITERAL(4, 52, 10), // "readStdout"
QT_MOC_LITERAL(5, 63, 10), // "readStderr"
QT_MOC_LITERAL(6, 74, 13), // "processExited"
QT_MOC_LITERAL(7, 88, 8), // "exitCode"
QT_MOC_LITERAL(8, 97, 14), // "processStarted"
QT_MOC_LITERAL(9, 112, 10) // "selfDelete"

    },
    "KviKvsProcessAsyncOperation\0"
    "maxRunTimeExpired\0\0ping\0readStdout\0"
    "readStderr\0processExited\0exitCode\0"
    "processStarted\0selfDelete"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviKvsProcessAsyncOperation[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x08 /* Private */,
       3,    0,   50,    2, 0x08 /* Private */,
       4,    0,   51,    2, 0x08 /* Private */,
       5,    0,   52,    2, 0x08 /* Private */,
       6,    1,   53,    2, 0x08 /* Private */,
       8,    0,   56,    2, 0x08 /* Private */,
       9,    0,   57,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void KviKvsProcessAsyncOperation::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviKvsProcessAsyncOperation *_t = static_cast<KviKvsProcessAsyncOperation *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->maxRunTimeExpired(); break;
        case 1: _t->ping(); break;
        case 2: _t->readStdout(); break;
        case 3: _t->readStderr(); break;
        case 4: _t->processExited((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->processStarted(); break;
        case 6: _t->selfDelete(); break;
        default: ;
        }
    }
}

const QMetaObject KviKvsProcessAsyncOperation::staticMetaObject = {
    { &KviKvsAsyncOperation::staticMetaObject, qt_meta_stringdata_KviKvsProcessAsyncOperation.data,
      qt_meta_data_KviKvsProcessAsyncOperation,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviKvsProcessAsyncOperation::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviKvsProcessAsyncOperation::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviKvsProcessAsyncOperation.stringdata0))
        return static_cast<void*>(this);
    return KviKvsAsyncOperation::qt_metacast(_clname);
}

int KviKvsProcessAsyncOperation::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviKvsAsyncOperation::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
