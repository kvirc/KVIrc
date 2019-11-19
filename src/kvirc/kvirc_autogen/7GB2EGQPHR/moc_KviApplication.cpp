/****************************************************************************
** Meta object code from reading C++ file 'KviApplication.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../kernel/KviApplication.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KviApplication.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviApplication_t {
    QByteArrayData data[12];
    char stringdata0[170];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviApplication_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviApplication_t qt_meta_stringdata_KviApplication = {
    {
QT_MOC_LITERAL(0, 0, 14), // "KviApplication"
QT_MOC_LITERAL(1, 15, 12), // "reloadImages"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 14), // "updateNotifier"
QT_MOC_LITERAL(4, 44, 17), // "recentUrlsChanged"
QT_MOC_LITERAL(5, 62, 17), // "saveConfiguration"
QT_MOC_LITERAL(6, 80, 9), // "updateGui"
QT_MOC_LITERAL(7, 90, 24), // "updatePseudoTransparency"
QT_MOC_LITERAL(8, 115, 20), // "restoreDefaultScript"
QT_MOC_LITERAL(9, 136, 12), // "addRecentUrl"
QT_MOC_LITERAL(10, 149, 4), // "text"
QT_MOC_LITERAL(11, 154, 15) // "showParentFrame"

    },
    "KviApplication\0reloadImages\0\0"
    "updateNotifier\0recentUrlsChanged\0"
    "saveConfiguration\0updateGui\0"
    "updatePseudoTransparency\0restoreDefaultScript\0"
    "addRecentUrl\0text\0showParentFrame"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviApplication[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x06 /* Public */,
       3,    0,   60,    2, 0x06 /* Public */,
       4,    0,   61,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   62,    2, 0x0a /* Public */,
       6,    0,   63,    2, 0x0a /* Public */,
       7,    0,   64,    2, 0x0a /* Public */,
       8,    0,   65,    2, 0x0a /* Public */,
       9,    1,   66,    2, 0x0a /* Public */,
      11,    0,   69,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   10,
    QMetaType::Void,

       0        // eod
};

void KviApplication::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviApplication *_t = static_cast<KviApplication *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->reloadImages(); break;
        case 1: _t->updateNotifier(); break;
        case 2: _t->recentUrlsChanged(); break;
        case 3: _t->saveConfiguration(); break;
        case 4: _t->updateGui(); break;
        case 5: _t->updatePseudoTransparency(); break;
        case 6: _t->restoreDefaultScript(); break;
        case 7: _t->addRecentUrl((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->showParentFrame(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (KviApplication::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviApplication::reloadImages)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (KviApplication::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviApplication::updateNotifier)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (KviApplication::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviApplication::recentUrlsChanged)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject KviApplication::staticMetaObject = {
    { &KviTalApplication::staticMetaObject, qt_meta_stringdata_KviApplication.data,
      qt_meta_data_KviApplication,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviApplication::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviApplication::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviApplication.stringdata0))
        return static_cast<void*>(this);
    return KviTalApplication::qt_metacast(_clname);
}

int KviApplication::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviTalApplication::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void KviApplication::reloadImages()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void KviApplication::updateNotifier()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void KviApplication::recentUrlsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
