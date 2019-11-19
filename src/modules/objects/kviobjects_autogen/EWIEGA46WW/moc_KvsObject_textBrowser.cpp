/****************************************************************************
** Meta object code from reading C++ file 'KvsObject_textBrowser.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../KvsObject_textBrowser.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KvsObject_textBrowser.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KvsObject_textBrowser_t {
    QByteArrayData data[3];
    char stringdata0[37];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KvsObject_textBrowser_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KvsObject_textBrowser_t qt_meta_stringdata_KvsObject_textBrowser = {
    {
QT_MOC_LITERAL(0, 0, 21), // "KvsObject_textBrowser"
QT_MOC_LITERAL(1, 22, 13), // "anchorClicked"
QT_MOC_LITERAL(2, 36, 0) // ""

    },
    "KvsObject_textBrowser\0anchorClicked\0"
    ""
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KvsObject_textBrowser[] = {

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
    QMetaType::Void, QMetaType::QUrl,    2,

       0        // eod
};

void KvsObject_textBrowser::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KvsObject_textBrowser *_t = static_cast<KvsObject_textBrowser *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->anchorClicked((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject KvsObject_textBrowser::staticMetaObject = {
    { &KvsObject_textedit::staticMetaObject, qt_meta_stringdata_KvsObject_textBrowser.data,
      qt_meta_data_KvsObject_textBrowser,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KvsObject_textBrowser::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KvsObject_textBrowser::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KvsObject_textBrowser.stringdata0))
        return static_cast<void*>(this);
    return KvsObject_textedit::qt_metacast(_clname);
}

int KvsObject_textBrowser::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KvsObject_textedit::qt_metacall(_c, _id, _a);
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
