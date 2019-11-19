/****************************************************************************
** Meta object code from reading C++ file 'KviSelectors.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ui/KviSelectors.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KviSelectors.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviBoolSelector_t {
    QByteArrayData data[6];
    char stringdata0[63];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviBoolSelector_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviBoolSelector_t qt_meta_stringdata_KviBoolSelector = {
    {
QT_MOC_LITERAL(0, 0, 15), // "KviBoolSelector"
QT_MOC_LITERAL(1, 16, 13), // "setNotEnabled"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 11), // "bNotEnabled"
QT_MOC_LITERAL(4, 43, 10), // "setEnabled"
QT_MOC_LITERAL(5, 54, 8) // "bEnabled"

    },
    "KviBoolSelector\0setNotEnabled\0\0"
    "bNotEnabled\0setEnabled\0bEnabled"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviBoolSelector[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x0a /* Public */,
       4,    1,   27,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    5,

       0        // eod
};

void KviBoolSelector::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviBoolSelector *_t = static_cast<KviBoolSelector *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setNotEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->setEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject KviBoolSelector::staticMetaObject = {
    { &QCheckBox::staticMetaObject, qt_meta_stringdata_KviBoolSelector.data,
      qt_meta_data_KviBoolSelector,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviBoolSelector::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviBoolSelector::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviBoolSelector.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "KviSelectorInterface"))
        return static_cast< KviSelectorInterface*>(this);
    return QCheckBox::qt_metacast(_clname);
}

int KviBoolSelector::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCheckBox::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_KviStringSelector_t {
    QByteArrayData data[4];
    char stringdata0[39];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviStringSelector_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviStringSelector_t qt_meta_stringdata_KviStringSelector = {
    {
QT_MOC_LITERAL(0, 0, 17), // "KviStringSelector"
QT_MOC_LITERAL(1, 18, 10), // "setEnabled"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 8) // "bEnabled"

    },
    "KviStringSelector\0setEnabled\0\0bEnabled"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviStringSelector[] = {

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
       1,    1,   19,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,

       0        // eod
};

void KviStringSelector::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviStringSelector *_t = static_cast<KviStringSelector *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject KviStringSelector::staticMetaObject = {
    { &KviTalHBox::staticMetaObject, qt_meta_stringdata_KviStringSelector.data,
      qt_meta_data_KviStringSelector,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviStringSelector::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviStringSelector::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviStringSelector.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "KviSelectorInterface"))
        return static_cast< KviSelectorInterface*>(this);
    return KviTalHBox::qt_metacast(_clname);
}

int KviStringSelector::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviTalHBox::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_KviPasswordSelector_t {
    QByteArrayData data[6];
    char stringdata0[60];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviPasswordSelector_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviPasswordSelector_t qt_meta_stringdata_KviPasswordSelector = {
    {
QT_MOC_LITERAL(0, 0, 19), // "KviPasswordSelector"
QT_MOC_LITERAL(1, 20, 10), // "setEnabled"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 8), // "bEnabled"
QT_MOC_LITERAL(4, 41, 12), // "checkToggled"
QT_MOC_LITERAL(5, 54, 5) // "state"

    },
    "KviPasswordSelector\0setEnabled\0\0"
    "bEnabled\0checkToggled\0state"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviPasswordSelector[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x0a /* Public */,
       4,    1,   27,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Int,    5,

       0        // eod
};

void KviPasswordSelector::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviPasswordSelector *_t = static_cast<KviPasswordSelector *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->checkToggled((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject KviPasswordSelector::staticMetaObject = {
    { &KviTalHBox::staticMetaObject, qt_meta_stringdata_KviPasswordSelector.data,
      qt_meta_data_KviPasswordSelector,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviPasswordSelector::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviPasswordSelector::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviPasswordSelector.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "KviSelectorInterface"))
        return static_cast< KviSelectorInterface*>(this);
    return KviTalHBox::qt_metacast(_clname);
}

int KviPasswordSelector::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviTalHBox::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_KviPasswordLineEdit_t {
    QByteArrayData data[6];
    char stringdata0[60];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviPasswordLineEdit_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviPasswordLineEdit_t qt_meta_stringdata_KviPasswordLineEdit = {
    {
QT_MOC_LITERAL(0, 0, 19), // "KviPasswordLineEdit"
QT_MOC_LITERAL(1, 20, 10), // "setEnabled"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 8), // "bEnabled"
QT_MOC_LITERAL(4, 41, 12), // "checkToggled"
QT_MOC_LITERAL(5, 54, 5) // "state"

    },
    "KviPasswordLineEdit\0setEnabled\0\0"
    "bEnabled\0checkToggled\0state"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviPasswordLineEdit[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x0a /* Public */,
       4,    1,   27,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Int,    5,

       0        // eod
};

void KviPasswordLineEdit::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviPasswordLineEdit *_t = static_cast<KviPasswordLineEdit *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->checkToggled((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject KviPasswordLineEdit::staticMetaObject = {
    { &KviTalHBox::staticMetaObject, qt_meta_stringdata_KviPasswordLineEdit.data,
      qt_meta_data_KviPasswordLineEdit,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviPasswordLineEdit::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviPasswordLineEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviPasswordLineEdit.stringdata0))
        return static_cast<void*>(this);
    return KviTalHBox::qt_metacast(_clname);
}

int KviPasswordLineEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviTalHBox::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_KviPixmapPreview_t {
    QByteArrayData data[1];
    char stringdata0[17];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviPixmapPreview_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviPixmapPreview_t qt_meta_stringdata_KviPixmapPreview = {
    {
QT_MOC_LITERAL(0, 0, 16) // "KviPixmapPreview"

    },
    "KviPixmapPreview"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviPixmapPreview[] = {

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

void KviPixmapPreview::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject KviPixmapPreview::staticMetaObject = {
    { &QScrollArea::staticMetaObject, qt_meta_stringdata_KviPixmapPreview.data,
      qt_meta_data_KviPixmapPreview,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviPixmapPreview::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviPixmapPreview::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviPixmapPreview.stringdata0))
        return static_cast<void*>(this);
    return QScrollArea::qt_metacast(_clname);
}

int KviPixmapPreview::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QScrollArea::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_KviPixmapSelector_t {
    QByteArrayData data[5];
    char stringdata0[57];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviPixmapSelector_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviPixmapSelector_t qt_meta_stringdata_KviPixmapSelector = {
    {
QT_MOC_LITERAL(0, 0, 17), // "KviPixmapSelector"
QT_MOC_LITERAL(1, 18, 15), // "checkBoxToggled"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 8), // "bEnabled"
QT_MOC_LITERAL(4, 44, 12) // "choosePixmap"

    },
    "KviPixmapSelector\0checkBoxToggled\0\0"
    "bEnabled\0choosePixmap"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviPixmapSelector[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x09 /* Protected */,
       4,    0,   27,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,

       0        // eod
};

void KviPixmapSelector::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviPixmapSelector *_t = static_cast<KviPixmapSelector *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->checkBoxToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->choosePixmap(); break;
        default: ;
        }
    }
}

const QMetaObject KviPixmapSelector::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_KviPixmapSelector.data,
      qt_meta_data_KviPixmapSelector,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviPixmapSelector::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviPixmapSelector::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviPixmapSelector.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "KviSelectorInterface"))
        return static_cast< KviSelectorInterface*>(this);
    return QWidget::qt_metacast(_clname);
}

int KviPixmapSelector::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_KviUIntSelector_t {
    QByteArrayData data[4];
    char stringdata0[37];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviUIntSelector_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviUIntSelector_t qt_meta_stringdata_KviUIntSelector = {
    {
QT_MOC_LITERAL(0, 0, 15), // "KviUIntSelector"
QT_MOC_LITERAL(1, 16, 10), // "setEnabled"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 8) // "bEnabled"

    },
    "KviUIntSelector\0setEnabled\0\0bEnabled"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviUIntSelector[] = {

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
       1,    1,   19,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,

       0        // eod
};

void KviUIntSelector::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviUIntSelector *_t = static_cast<KviUIntSelector *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject KviUIntSelector::staticMetaObject = {
    { &KviTalHBox::staticMetaObject, qt_meta_stringdata_KviUIntSelector.data,
      qt_meta_data_KviUIntSelector,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviUIntSelector::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviUIntSelector::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviUIntSelector.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "KviSelectorInterface"))
        return static_cast< KviSelectorInterface*>(this);
    return KviTalHBox::qt_metacast(_clname);
}

int KviUIntSelector::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviTalHBox::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_KviFileSelector_t {
    QByteArrayData data[8];
    char stringdata0[87];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviFileSelector_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviFileSelector_t qt_meta_stringdata_KviFileSelector = {
    {
QT_MOC_LITERAL(0, 0, 15), // "KviFileSelector"
QT_MOC_LITERAL(1, 16, 16), // "selectionChanged"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 10), // "szNewValue"
QT_MOC_LITERAL(4, 45, 10), // "setEnabled"
QT_MOC_LITERAL(5, 56, 8), // "bEnabled"
QT_MOC_LITERAL(6, 65, 13), // "browseClicked"
QT_MOC_LITERAL(7, 79, 7) // "tmpFile"

    },
    "KviFileSelector\0selectionChanged\0\0"
    "szNewValue\0setEnabled\0bEnabled\0"
    "browseClicked\0tmpFile"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviFileSelector[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       1,   36, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   32,    2, 0x0a /* Public */,
       6,    0,   35,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void,

 // properties: name, type, flags
       7, QMetaType::QString, 0x00095103,

       0        // eod
};

void KviFileSelector::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviFileSelector *_t = static_cast<KviFileSelector *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->selectionChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->setEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->browseClicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (KviFileSelector::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviFileSelector::selectionChanged)) {
                *result = 0;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        KviFileSelector *_t = static_cast<KviFileSelector *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->tmpFile(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        KviFileSelector *_t = static_cast<KviFileSelector *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setTmpFile(*reinterpret_cast< QString*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject KviFileSelector::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_KviFileSelector.data,
      qt_meta_data_KviFileSelector,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviFileSelector::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviFileSelector::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviFileSelector.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "KviSelectorInterface"))
        return static_cast< KviSelectorInterface*>(this);
    return QWidget::qt_metacast(_clname);
}

int KviFileSelector::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
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
void KviFileSelector::selectionChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_KviDirectorySelector_t {
    QByteArrayData data[1];
    char stringdata0[21];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviDirectorySelector_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviDirectorySelector_t qt_meta_stringdata_KviDirectorySelector = {
    {
QT_MOC_LITERAL(0, 0, 20) // "KviDirectorySelector"

    },
    "KviDirectorySelector"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviDirectorySelector[] = {

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

void KviDirectorySelector::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject KviDirectorySelector::staticMetaObject = {
    { &KviFileSelector::staticMetaObject, qt_meta_stringdata_KviDirectorySelector.data,
      qt_meta_data_KviDirectorySelector,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviDirectorySelector::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviDirectorySelector::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviDirectorySelector.stringdata0))
        return static_cast<void*>(this);
    return KviFileSelector::qt_metacast(_clname);
}

int KviDirectorySelector::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviFileSelector::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_KviColorSelector_t {
    QByteArrayData data[5];
    char stringdata0[52];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviColorSelector_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviColorSelector_t qt_meta_stringdata_KviColorSelector = {
    {
QT_MOC_LITERAL(0, 0, 16), // "KviColorSelector"
QT_MOC_LITERAL(1, 17, 13), // "changeClicked"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 10), // "setEnabled"
QT_MOC_LITERAL(4, 43, 8) // "bEnabled"

    },
    "KviColorSelector\0changeClicked\0\0"
    "setEnabled\0bEnabled"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviColorSelector[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x08 /* Private */,
       3,    1,   25,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    4,

       0        // eod
};

void KviColorSelector::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviColorSelector *_t = static_cast<KviColorSelector *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->changeClicked(); break;
        case 1: _t->setEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject KviColorSelector::staticMetaObject = {
    { &KviTalHBox::staticMetaObject, qt_meta_stringdata_KviColorSelector.data,
      qt_meta_data_KviColorSelector,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviColorSelector::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviColorSelector::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviColorSelector.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "KviSelectorInterface"))
        return static_cast< KviSelectorInterface*>(this);
    return KviTalHBox::qt_metacast(_clname);
}

int KviColorSelector::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviTalHBox::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_KviFontSelector_t {
    QByteArrayData data[5];
    char stringdata0[51];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviFontSelector_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviFontSelector_t qt_meta_stringdata_KviFontSelector = {
    {
QT_MOC_LITERAL(0, 0, 15), // "KviFontSelector"
QT_MOC_LITERAL(1, 16, 10), // "setEnabled"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 8), // "bEnabled"
QT_MOC_LITERAL(4, 37, 13) // "changeClicked"

    },
    "KviFontSelector\0setEnabled\0\0bEnabled\0"
    "changeClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviFontSelector[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x0a /* Public */,
       4,    0,   27,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,

       0        // eod
};

void KviFontSelector::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviFontSelector *_t = static_cast<KviFontSelector *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->changeClicked(); break;
        default: ;
        }
    }
}

const QMetaObject KviFontSelector::staticMetaObject = {
    { &KviTalHBox::staticMetaObject, qt_meta_stringdata_KviFontSelector.data,
      qt_meta_data_KviFontSelector,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviFontSelector::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviFontSelector::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviFontSelector.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "KviSelectorInterface"))
        return static_cast< KviSelectorInterface*>(this);
    return KviTalHBox::qt_metacast(_clname);
}

int KviFontSelector::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviTalHBox::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_KviStringListSelector_t {
    QByteArrayData data[9];
    char stringdata0[105];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviStringListSelector_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviStringListSelector_t qt_meta_stringdata_KviStringListSelector = {
    {
QT_MOC_LITERAL(0, 0, 21), // "KviStringListSelector"
QT_MOC_LITERAL(1, 22, 10), // "setEnabled"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 8), // "bEnabled"
QT_MOC_LITERAL(4, 43, 11), // "textChanged"
QT_MOC_LITERAL(5, 55, 3), // "str"
QT_MOC_LITERAL(6, 59, 20), // "itemSelectionChanged"
QT_MOC_LITERAL(7, 80, 10), // "addClicked"
QT_MOC_LITERAL(8, 91, 13) // "removeClicked"

    },
    "KviStringListSelector\0setEnabled\0\0"
    "bEnabled\0textChanged\0str\0itemSelectionChanged\0"
    "addClicked\0removeClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviStringListSelector[] = {

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
       1,    1,   39,    2, 0x0a /* Public */,
       4,    1,   42,    2, 0x08 /* Private */,
       6,    0,   45,    2, 0x08 /* Private */,
       7,    0,   46,    2, 0x08 /* Private */,
       8,    0,   47,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void KviStringListSelector::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviStringListSelector *_t = static_cast<KviStringListSelector *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->itemSelectionChanged(); break;
        case 3: _t->addClicked(); break;
        case 4: _t->removeClicked(); break;
        default: ;
        }
    }
}

const QMetaObject KviStringListSelector::staticMetaObject = {
    { &KviTalVBox::staticMetaObject, qt_meta_stringdata_KviStringListSelector.data,
      qt_meta_data_KviStringListSelector,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviStringListSelector::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviStringListSelector::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviStringListSelector.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "KviSelectorInterface"))
        return static_cast< KviSelectorInterface*>(this);
    return KviTalVBox::qt_metacast(_clname);
}

int KviStringListSelector::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviTalVBox::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_KviMircTextColorSelector_t {
    QByteArrayData data[7];
    char stringdata0[83];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviMircTextColorSelector_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviMircTextColorSelector_t qt_meta_stringdata_KviMircTextColorSelector = {
    {
QT_MOC_LITERAL(0, 0, 24), // "KviMircTextColorSelector"
QT_MOC_LITERAL(1, 25, 13), // "buttonClicked"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 12), // "foreSelected"
QT_MOC_LITERAL(4, 53, 8), // "QAction*"
QT_MOC_LITERAL(5, 62, 7), // "pAction"
QT_MOC_LITERAL(6, 70, 12) // "backSelected"

    },
    "KviMircTextColorSelector\0buttonClicked\0"
    "\0foreSelected\0QAction*\0pAction\0"
    "backSelected"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviMircTextColorSelector[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x09 /* Protected */,
       3,    1,   30,    2, 0x09 /* Protected */,
       6,    1,   33,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, 0x80000000 | 4,    5,

       0        // eod
};

void KviMircTextColorSelector::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviMircTextColorSelector *_t = static_cast<KviMircTextColorSelector *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->buttonClicked(); break;
        case 1: _t->foreSelected((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 2: _t->backSelected((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject KviMircTextColorSelector::staticMetaObject = {
    { &KviTalHBox::staticMetaObject, qt_meta_stringdata_KviMircTextColorSelector.data,
      qt_meta_data_KviMircTextColorSelector,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviMircTextColorSelector::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviMircTextColorSelector::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviMircTextColorSelector.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "KviSelectorInterface"))
        return static_cast< KviSelectorInterface*>(this);
    return KviTalHBox::qt_metacast(_clname);
}

int KviMircTextColorSelector::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviTalHBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
struct qt_meta_stringdata_KviSoundSelector_t {
    QByteArrayData data[3];
    char stringdata0[28];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviSoundSelector_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviSoundSelector_t qt_meta_stringdata_KviSoundSelector = {
    {
QT_MOC_LITERAL(0, 0, 16), // "KviSoundSelector"
QT_MOC_LITERAL(1, 17, 9), // "playSound"
QT_MOC_LITERAL(2, 27, 0) // ""

    },
    "KviSoundSelector\0playSound\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviSoundSelector[] = {

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
       1,    0,   19,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void KviSoundSelector::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviSoundSelector *_t = static_cast<KviSoundSelector *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->playSound(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject KviSoundSelector::staticMetaObject = {
    { &KviFileSelector::staticMetaObject, qt_meta_stringdata_KviSoundSelector.data,
      qt_meta_data_KviSoundSelector,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviSoundSelector::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviSoundSelector::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviSoundSelector.stringdata0))
        return static_cast<void*>(this);
    return KviFileSelector::qt_metacast(_clname);
}

int KviSoundSelector::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviFileSelector::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_KviChannelListSelector_t {
    QByteArrayData data[7];
    char stringdata0[86];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviChannelListSelector_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviChannelListSelector_t qt_meta_stringdata_KviChannelListSelector = {
    {
QT_MOC_LITERAL(0, 0, 22), // "KviChannelListSelector"
QT_MOC_LITERAL(1, 23, 11), // "textChanged"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 3), // "str"
QT_MOC_LITERAL(4, 40, 20), // "itemSelectionChanged"
QT_MOC_LITERAL(5, 61, 10), // "addClicked"
QT_MOC_LITERAL(6, 72, 13) // "removeClicked"

    },
    "KviChannelListSelector\0textChanged\0\0"
    "str\0itemSelectionChanged\0addClicked\0"
    "removeClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviChannelListSelector[] = {

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
       1,    1,   34,    2, 0x08 /* Private */,
       4,    0,   37,    2, 0x08 /* Private */,
       5,    0,   38,    2, 0x08 /* Private */,
       6,    0,   39,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void KviChannelListSelector::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviChannelListSelector *_t = static_cast<KviChannelListSelector *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->itemSelectionChanged(); break;
        case 2: _t->addClicked(); break;
        case 3: _t->removeClicked(); break;
        default: ;
        }
    }
}

const QMetaObject KviChannelListSelector::staticMetaObject = {
    { &KviTalVBox::staticMetaObject, qt_meta_stringdata_KviChannelListSelector.data,
      qt_meta_data_KviChannelListSelector,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviChannelListSelector::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviChannelListSelector::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviChannelListSelector.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "KviSelectorInterface"))
        return static_cast< KviSelectorInterface*>(this);
    return KviTalVBox::qt_metacast(_clname);
}

int KviChannelListSelector::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviTalVBox::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
