/****************************************************************************
** Meta object code from reading C++ file 'KviTalWizard.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../tal/KviTalWizard.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KviTalWizard.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviTalWizard_t {
    QByteArrayData data[10];
    char stringdata0[138];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviTalWizard_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviTalWizard_t qt_meta_stringdata_KviTalWizard = {
    {
QT_MOC_LITERAL(0, 0, 12), // "KviTalWizard"
QT_MOC_LITERAL(1, 13, 11), // "helpClicked"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 11), // "pageChanged"
QT_MOC_LITERAL(4, 38, 5), // "title"
QT_MOC_LITERAL(5, 44, 17), // "backButtonClicked"
QT_MOC_LITERAL(6, 62, 17), // "nextButtonClicked"
QT_MOC_LITERAL(7, 80, 17), // "helpButtonClicked"
QT_MOC_LITERAL(8, 98, 19), // "cancelButtonClicked"
QT_MOC_LITERAL(9, 118, 19) // "finishButtonClicked"

    },
    "KviTalWizard\0helpClicked\0\0pageChanged\0"
    "title\0backButtonClicked\0nextButtonClicked\0"
    "helpButtonClicked\0cancelButtonClicked\0"
    "finishButtonClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviTalWizard[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x06 /* Public */,
       3,    1,   50,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   53,    2, 0x09 /* Protected */,
       6,    0,   54,    2, 0x09 /* Protected */,
       7,    0,   55,    2, 0x09 /* Protected */,
       8,    0,   56,    2, 0x09 /* Protected */,
       9,    0,   57,    2, 0x09 /* Protected */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void KviTalWizard::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviTalWizard *_t = static_cast<KviTalWizard *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->helpClicked(); break;
        case 1: _t->pageChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->backButtonClicked(); break;
        case 3: _t->nextButtonClicked(); break;
        case 4: _t->helpButtonClicked(); break;
        case 5: _t->cancelButtonClicked(); break;
        case 6: _t->finishButtonClicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (KviTalWizard::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviTalWizard::helpClicked)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (KviTalWizard::*_t)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviTalWizard::pageChanged)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject KviTalWizard::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_KviTalWizard.data,
      qt_meta_data_KviTalWizard,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviTalWizard::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviTalWizard::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviTalWizard.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int KviTalWizard::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void KviTalWizard::helpClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void KviTalWizard::pageChanged(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
