/****************************************************************************
** Meta object code from reading C++ file 'KvsObject_pixmap.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../KvsObject_pixmap.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KvsObject_pixmap.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KvsObject_pixmap_t {
    QByteArrayData data[4];
    char stringdata0[42];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KvsObject_pixmap_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KvsObject_pixmap_t qt_meta_stringdata_KvsObject_pixmap = {
    {
QT_MOC_LITERAL(0, 0, 16), // "KvsObject_pixmap"
QT_MOC_LITERAL(1, 17, 10), // "aboutToDie"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 12) // "frameChanged"

    },
    "KvsObject_pixmap\0aboutToDie\0\0frameChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KvsObject_pixmap[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   25,    2, 0x09 /* Protected */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void KvsObject_pixmap::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KvsObject_pixmap *_t = static_cast<KvsObject_pixmap *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->aboutToDie(); break;
        case 1: _t->frameChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (KvsObject_pixmap::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KvsObject_pixmap::aboutToDie)) {
                *result = 0;
                return;
            }
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject KvsObject_pixmap::staticMetaObject = {
    { &KviKvsObject::staticMetaObject, qt_meta_stringdata_KvsObject_pixmap.data,
      qt_meta_data_KvsObject_pixmap,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KvsObject_pixmap::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KvsObject_pixmap::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KvsObject_pixmap.stringdata0))
        return static_cast<void*>(this);
    return KviKvsObject::qt_metacast(_clname);
}

int KvsObject_pixmap::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviKvsObject::qt_metacall(_c, _id, _a);
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
    return _id;
}

// SIGNAL 0
void KvsObject_pixmap::aboutToDie()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
