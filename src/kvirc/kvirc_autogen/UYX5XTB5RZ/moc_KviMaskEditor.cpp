/****************************************************************************
** Meta object code from reading C++ file 'KviMaskEditor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ui/KviMaskEditor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KviMaskEditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviMaskInputDialog_t {
    QByteArrayData data[3];
    char stringdata0[27];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviMaskInputDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviMaskInputDialog_t qt_meta_stringdata_KviMaskInputDialog = {
    {
QT_MOC_LITERAL(0, 0, 18), // "KviMaskInputDialog"
QT_MOC_LITERAL(1, 19, 6), // "accept"
QT_MOC_LITERAL(2, 26, 0) // ""

    },
    "KviMaskInputDialog\0accept\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviMaskInputDialog[] = {

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
       1,    0,   19,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void KviMaskInputDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviMaskInputDialog *_t = static_cast<KviMaskInputDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->accept(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject KviMaskInputDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_KviMaskInputDialog.data,
      qt_meta_data_KviMaskInputDialog,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviMaskInputDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviMaskInputDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviMaskInputDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int KviMaskInputDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_KviMaskEditor_t {
    QByteArrayData data[11];
    char stringdata0[162];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviMaskEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviMaskEditor_t qt_meta_stringdata_KviMaskEditor = {
    {
QT_MOC_LITERAL(0, 0, 13), // "KviMaskEditor"
QT_MOC_LITERAL(1, 14, 11), // "removeMasks"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 14), // "KviMaskEditor*"
QT_MOC_LITERAL(4, 42, 26), // "std::vector<KviMaskEntry*>"
QT_MOC_LITERAL(5, 69, 13), // "removeClicked"
QT_MOC_LITERAL(6, 83, 10), // "addClicked"
QT_MOC_LITERAL(7, 94, 17), // "itemDoubleClicked"
QT_MOC_LITERAL(8, 112, 16), // "QTreeWidgetItem*"
QT_MOC_LITERAL(9, 129, 17), // "searchTextChanged"
QT_MOC_LITERAL(10, 147, 14) // "updateOpStatus"

    },
    "KviMaskEditor\0removeMasks\0\0KviMaskEditor*\0"
    "std::vector<KviMaskEntry*>\0removeClicked\0"
    "addClicked\0itemDoubleClicked\0"
    "QTreeWidgetItem*\0searchTextChanged\0"
    "updateOpStatus"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviMaskEditor[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   44,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   49,    2, 0x09 /* Protected */,
       6,    0,   50,    2, 0x09 /* Protected */,
       7,    2,   51,    2, 0x09 /* Protected */,
       9,    1,   56,    2, 0x09 /* Protected */,
      10,    0,   59,    2, 0x09 /* Protected */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 4,    2,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,

       0        // eod
};

void KviMaskEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviMaskEditor *_t = static_cast<KviMaskEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->removeMasks((*reinterpret_cast< KviMaskEditor*(*)>(_a[1])),(*reinterpret_cast< std::vector<KviMaskEntry*>(*)>(_a[2]))); break;
        case 1: _t->removeClicked(); break;
        case 2: _t->addClicked(); break;
        case 3: _t->itemDoubleClicked((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->searchTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->updateOpStatus(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< KviMaskEditor* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (KviMaskEditor::*_t)(KviMaskEditor * , std::vector<KviMaskEntry*> );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviMaskEditor::removeMasks)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject KviMaskEditor::staticMetaObject = {
    { &KviWindowToolWidget::staticMetaObject, qt_meta_stringdata_KviMaskEditor.data,
      qt_meta_data_KviMaskEditor,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviMaskEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviMaskEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviMaskEditor.stringdata0))
        return static_cast<void*>(this);
    return KviWindowToolWidget::qt_metacast(_clname);
}

int KviMaskEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviWindowToolWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void KviMaskEditor::removeMasks(KviMaskEditor * _t1, std::vector<KviMaskEntry*> _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
