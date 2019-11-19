/****************************************************************************
** Meta object code from reading C++ file 'libkvidialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../libkvidialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'libkvidialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviKvsCallbackMessageBox_t {
    QByteArrayData data[4];
    char stringdata0[36];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviKvsCallbackMessageBox_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviKvsCallbackMessageBox_t qt_meta_stringdata_KviKvsCallbackMessageBox = {
    {
QT_MOC_LITERAL(0, 0, 24), // "KviKvsCallbackMessageBox"
QT_MOC_LITERAL(1, 25, 4), // "done"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 4) // "code"

    },
    "KviKvsCallbackMessageBox\0done\0\0code"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviKvsCallbackMessageBox[] = {

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
    QMetaType::Void, QMetaType::Int,    3,

       0        // eod
};

void KviKvsCallbackMessageBox::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviKvsCallbackMessageBox *_t = static_cast<KviKvsCallbackMessageBox *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->done((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject KviKvsCallbackMessageBox::staticMetaObject = {
    { &QMessageBox::staticMetaObject, qt_meta_stringdata_KviKvsCallbackMessageBox.data,
      qt_meta_data_KviKvsCallbackMessageBox,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviKvsCallbackMessageBox::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviKvsCallbackMessageBox::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviKvsCallbackMessageBox.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "KviKvsCallbackObject"))
        return static_cast< KviKvsCallbackObject*>(this);
    return QMessageBox::qt_metacast(_clname);
}

int KviKvsCallbackMessageBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMessageBox::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_KviKvsCallbackTextInput_t {
    QByteArrayData data[7];
    char stringdata0[65];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviKvsCallbackTextInput_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviKvsCallbackTextInput_t qt_meta_stringdata_KviKvsCallbackTextInput = {
    {
QT_MOC_LITERAL(0, 0, 23), // "KviKvsCallbackTextInput"
QT_MOC_LITERAL(1, 24, 9), // "b0Clicked"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 9), // "b1Clicked"
QT_MOC_LITERAL(4, 45, 9), // "b2Clicked"
QT_MOC_LITERAL(5, 55, 4), // "done"
QT_MOC_LITERAL(6, 60, 4) // "code"

    },
    "KviKvsCallbackTextInput\0b0Clicked\0\0"
    "b1Clicked\0b2Clicked\0done\0code"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviKvsCallbackTextInput[] = {

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
       5,    1,   37,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    6,

       0        // eod
};

void KviKvsCallbackTextInput::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviKvsCallbackTextInput *_t = static_cast<KviKvsCallbackTextInput *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->b0Clicked(); break;
        case 1: _t->b1Clicked(); break;
        case 2: _t->b2Clicked(); break;
        case 3: _t->done((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject KviKvsCallbackTextInput::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_KviKvsCallbackTextInput.data,
      qt_meta_data_KviKvsCallbackTextInput,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviKvsCallbackTextInput::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviKvsCallbackTextInput::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviKvsCallbackTextInput.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "KviKvsCallbackObject"))
        return static_cast< KviKvsCallbackObject*>(this);
    return QDialog::qt_metacast(_clname);
}

int KviKvsCallbackTextInput::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
struct qt_meta_stringdata_KviKvsCallbackFileDialog_t {
    QByteArrayData data[1];
    char stringdata0[25];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviKvsCallbackFileDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviKvsCallbackFileDialog_t qt_meta_stringdata_KviKvsCallbackFileDialog = {
    {
QT_MOC_LITERAL(0, 0, 24) // "KviKvsCallbackFileDialog"

    },
    "KviKvsCallbackFileDialog"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviKvsCallbackFileDialog[] = {

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

void KviKvsCallbackFileDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject KviKvsCallbackFileDialog::staticMetaObject = {
    { &KviFileDialog::staticMetaObject, qt_meta_stringdata_KviKvsCallbackFileDialog.data,
      qt_meta_data_KviKvsCallbackFileDialog,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviKvsCallbackFileDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviKvsCallbackFileDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviKvsCallbackFileDialog.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "KviKvsCallbackObject"))
        return static_cast< KviKvsCallbackObject*>(this);
    return KviFileDialog::qt_metacast(_clname);
}

int KviKvsCallbackFileDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviFileDialog::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_KviKvsCallbackImageDialog_t {
    QByteArrayData data[1];
    char stringdata0[26];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviKvsCallbackImageDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviKvsCallbackImageDialog_t qt_meta_stringdata_KviKvsCallbackImageDialog = {
    {
QT_MOC_LITERAL(0, 0, 25) // "KviKvsCallbackImageDialog"

    },
    "KviKvsCallbackImageDialog"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviKvsCallbackImageDialog[] = {

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

void KviKvsCallbackImageDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject KviKvsCallbackImageDialog::staticMetaObject = {
    { &KviImageDialog::staticMetaObject, qt_meta_stringdata_KviKvsCallbackImageDialog.data,
      qt_meta_data_KviKvsCallbackImageDialog,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviKvsCallbackImageDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviKvsCallbackImageDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviKvsCallbackImageDialog.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "KviKvsCallbackObject"))
        return static_cast< KviKvsCallbackObject*>(this);
    return KviImageDialog::qt_metacast(_clname);
}

int KviKvsCallbackImageDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviImageDialog::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
