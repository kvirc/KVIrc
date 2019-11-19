/****************************************************************************
** Meta object code from reading C++ file 'KviModeEditor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ui/KviModeEditor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KviModeEditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviModeEditor_t {
    QByteArrayData data[9];
    char stringdata0[82];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviModeEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviModeEditor_t qt_meta_stringdata_KviModeEditor = {
    {
QT_MOC_LITERAL(0, 0, 13), // "KviModeEditor"
QT_MOC_LITERAL(1, 14, 7), // "setMode"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 6), // "szMode"
QT_MOC_LITERAL(4, 30, 4), // "done"
QT_MOC_LITERAL(5, 35, 15), // "checkBoxToggled"
QT_MOC_LITERAL(6, 51, 8), // "bChecked"
QT_MOC_LITERAL(7, 60, 6), // "commit"
QT_MOC_LITERAL(8, 67, 14) // "updateOpStatus"

    },
    "KviModeEditor\0setMode\0\0szMode\0done\0"
    "checkBoxToggled\0bChecked\0commit\0"
    "updateOpStatus"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviModeEditor[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       4,    0,   42,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   43,    2, 0x09 /* Protected */,
       7,    0,   46,    2, 0x09 /* Protected */,
       8,    0,   47,    2, 0x09 /* Protected */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void KviModeEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviModeEditor *_t = static_cast<KviModeEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setMode((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->done(); break;
        case 2: _t->checkBoxToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->commit(); break;
        case 4: _t->updateOpStatus(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (KviModeEditor::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviModeEditor::setMode)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (KviModeEditor::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviModeEditor::done)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject KviModeEditor::staticMetaObject = {
    { &KviWindowToolWidget::staticMetaObject, qt_meta_stringdata_KviModeEditor.data,
      qt_meta_data_KviModeEditor,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviModeEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviModeEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviModeEditor.stringdata0))
        return static_cast<void*>(this);
    return KviWindowToolWidget::qt_metacast(_clname);
}

int KviModeEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviWindowToolWidget::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void KviModeEditor::setMode(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void KviModeEditor::done()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
