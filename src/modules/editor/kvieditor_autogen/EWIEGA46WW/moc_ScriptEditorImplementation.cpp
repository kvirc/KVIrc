/****************************************************************************
** Meta object code from reading C++ file 'ScriptEditorImplementation.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ScriptEditorImplementation.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ScriptEditorImplementation.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ScriptEditorWidget_t {
    QByteArrayData data[11];
    char stringdata0[155];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScriptEditorWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScriptEditorWidget_t qt_meta_stringdata_ScriptEditorWidget = {
    {
QT_MOC_LITERAL(0, 0, 18), // "ScriptEditorWidget"
QT_MOC_LITERAL(1, 19, 10), // "keyPressed"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 19), // "checkReadyCompleter"
QT_MOC_LITERAL(4, 51, 16), // "insertCompletion"
QT_MOC_LITERAL(5, 68, 12), // "szCompletion"
QT_MOC_LITERAL(6, 81, 8), // "slotFind"
QT_MOC_LITERAL(7, 90, 8), // "slotHelp"
QT_MOC_LITERAL(8, 99, 11), // "slotReplace"
QT_MOC_LITERAL(9, 111, 22), // "asyncCompleterCreation"
QT_MOC_LITERAL(10, 134, 20) // "contextSensitiveHelp"

    },
    "ScriptEditorWidget\0keyPressed\0\0"
    "checkReadyCompleter\0insertCompletion\0"
    "szCompletion\0slotFind\0slotHelp\0"
    "slotReplace\0asyncCompleterCreation\0"
    "contextSensitiveHelp"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScriptEditorWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       1,   58, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   50,    2, 0x0a /* Public */,
       4,    1,   51,    2, 0x0a /* Public */,
       6,    0,   54,    2, 0x0a /* Public */,
       7,    0,   55,    2, 0x0a /* Public */,
       8,    0,   56,    2, 0x0a /* Public */,
       9,    0,   57,    2, 0x09 /* Protected */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
      10, QMetaType::Bool, 0x00095001,

       0        // eod
};

void ScriptEditorWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ScriptEditorWidget *_t = static_cast<ScriptEditorWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->keyPressed(); break;
        case 1: _t->checkReadyCompleter(); break;
        case 2: _t->insertCompletion((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->slotFind(); break;
        case 4: _t->slotHelp(); break;
        case 5: _t->slotReplace(); break;
        case 6: _t->asyncCompleterCreation(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (ScriptEditorWidget::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ScriptEditorWidget::keyPressed)) {
                *result = 0;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        ScriptEditorWidget *_t = static_cast<ScriptEditorWidget *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->contextSensitiveHelp(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject ScriptEditorWidget::staticMetaObject = {
    { &QTextEdit::staticMetaObject, qt_meta_stringdata_ScriptEditorWidget.data,
      qt_meta_data_ScriptEditorWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ScriptEditorWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScriptEditorWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ScriptEditorWidget.stringdata0))
        return static_cast<void*>(this);
    return QTextEdit::qt_metacast(_clname);
}

int ScriptEditorWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTextEdit::qt_metacall(_c, _id, _a);
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
void ScriptEditorWidget::keyPressed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
struct qt_meta_stringdata_ScriptEditorWidgetColorOptions_t {
    QByteArrayData data[3];
    char stringdata0[42];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScriptEditorWidgetColorOptions_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScriptEditorWidgetColorOptions_t qt_meta_stringdata_ScriptEditorWidgetColorOptions = {
    {
QT_MOC_LITERAL(0, 0, 30), // "ScriptEditorWidgetColorOptions"
QT_MOC_LITERAL(1, 31, 9), // "okClicked"
QT_MOC_LITERAL(2, 41, 0) // ""

    },
    "ScriptEditorWidgetColorOptions\0okClicked\0"
    ""
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScriptEditorWidgetColorOptions[] = {

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

void ScriptEditorWidgetColorOptions::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ScriptEditorWidgetColorOptions *_t = static_cast<ScriptEditorWidgetColorOptions *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->okClicked(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject ScriptEditorWidgetColorOptions::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ScriptEditorWidgetColorOptions.data,
      qt_meta_data_ScriptEditorWidgetColorOptions,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ScriptEditorWidgetColorOptions::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScriptEditorWidgetColorOptions::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ScriptEditorWidgetColorOptions.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int ScriptEditorWidgetColorOptions::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
struct qt_meta_stringdata_ScriptEditorImplementation_t {
    QByteArrayData data[21];
    char stringdata0[250];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScriptEditorImplementation_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScriptEditorImplementation_t qt_meta_stringdata_ScriptEditorImplementation = {
    {
QT_MOC_LITERAL(0, 0, 26), // "ScriptEditorImplementation"
QT_MOC_LITERAL(1, 27, 4), // "find"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 10), // "replaceAll"
QT_MOC_LITERAL(4, 44, 11), // "szToReplace"
QT_MOC_LITERAL(5, 56, 13), // "szReplaceWith"
QT_MOC_LITERAL(6, 70, 8), // "initFind"
QT_MOC_LITERAL(7, 79, 8), // "nextFind"
QT_MOC_LITERAL(8, 88, 6), // "szText"
QT_MOC_LITERAL(9, 95, 7), // "newFile"
QT_MOC_LITERAL(10, 103, 8), // "saveFile"
QT_MOC_LITERAL(11, 112, 10), // "saveFileAs"
QT_MOC_LITERAL(12, 123, 12), // "loadFromFile"
QT_MOC_LITERAL(13, 136, 15), // "configureColors"
QT_MOC_LITERAL(14, 152, 17), // "updateRowColLabel"
QT_MOC_LITERAL(15, 170, 8), // "slotFind"
QT_MOC_LITERAL(16, 179, 14), // "slotReplaceAll"
QT_MOC_LITERAL(17, 194, 12), // "slotInitFind"
QT_MOC_LITERAL(18, 207, 12), // "slotNextFind"
QT_MOC_LITERAL(19, 220, 21), // "optionsDialogFinished"
QT_MOC_LITERAL(20, 242, 7) // "iResult"

    },
    "ScriptEditorImplementation\0find\0\0"
    "replaceAll\0szToReplace\0szReplaceWith\0"
    "initFind\0nextFind\0szText\0newFile\0"
    "saveFile\0saveFileAs\0loadFromFile\0"
    "configureColors\0updateRowColLabel\0"
    "slotFind\0slotReplaceAll\0slotInitFind\0"
    "slotNextFind\0optionsDialogFinished\0"
    "iResult"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScriptEditorImplementation[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   89,    2, 0x06 /* Public */,
       3,    2,   92,    2, 0x06 /* Public */,
       6,    0,   97,    2, 0x06 /* Public */,
       7,    1,   98,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    0,  101,    2, 0x09 /* Protected */,
      10,    0,  102,    2, 0x09 /* Protected */,
      11,    0,  103,    2, 0x09 /* Protected */,
      12,    0,  104,    2, 0x09 /* Protected */,
      13,    0,  105,    2, 0x09 /* Protected */,
      14,    0,  106,    2, 0x09 /* Protected */,
      15,    0,  107,    2, 0x09 /* Protected */,
      16,    2,  108,    2, 0x09 /* Protected */,
      17,    0,  113,    2, 0x09 /* Protected */,
      18,    1,  114,    2, 0x09 /* Protected */,
      19,    1,  117,    2, 0x09 /* Protected */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    4,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    8,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    4,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Int,   20,

       0        // eod
};

void ScriptEditorImplementation::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ScriptEditorImplementation *_t = static_cast<ScriptEditorImplementation *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->find((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->replaceAll((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 2: _t->initFind(); break;
        case 3: _t->nextFind((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->newFile(); break;
        case 5: _t->saveFile(); break;
        case 6: _t->saveFileAs(); break;
        case 7: _t->loadFromFile(); break;
        case 8: _t->configureColors(); break;
        case 9: _t->updateRowColLabel(); break;
        case 10: _t->slotFind(); break;
        case 11: _t->slotReplaceAll((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 12: _t->slotInitFind(); break;
        case 13: _t->slotNextFind((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 14: _t->optionsDialogFinished((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (ScriptEditorImplementation::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ScriptEditorImplementation::find)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (ScriptEditorImplementation::*_t)(const QString & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ScriptEditorImplementation::replaceAll)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (ScriptEditorImplementation::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ScriptEditorImplementation::initFind)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (ScriptEditorImplementation::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ScriptEditorImplementation::nextFind)) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject ScriptEditorImplementation::staticMetaObject = {
    { &KviScriptEditor::staticMetaObject, qt_meta_stringdata_ScriptEditorImplementation.data,
      qt_meta_data_ScriptEditorImplementation,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ScriptEditorImplementation::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScriptEditorImplementation::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ScriptEditorImplementation.stringdata0))
        return static_cast<void*>(this);
    return KviScriptEditor::qt_metacast(_clname);
}

int ScriptEditorImplementation::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviScriptEditor::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void ScriptEditorImplementation::find(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ScriptEditorImplementation::replaceAll(const QString & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ScriptEditorImplementation::initFind()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void ScriptEditorImplementation::nextFind(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
struct qt_meta_stringdata_ScriptEditorReplaceDialog_t {
    QByteArrayData data[8];
    char stringdata0[93];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScriptEditorReplaceDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScriptEditorReplaceDialog_t qt_meta_stringdata_ScriptEditorReplaceDialog = {
    {
QT_MOC_LITERAL(0, 0, 25), // "ScriptEditorReplaceDialog"
QT_MOC_LITERAL(1, 26, 10), // "replaceAll"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 8), // "initFind"
QT_MOC_LITERAL(4, 47, 8), // "nextFind"
QT_MOC_LITERAL(5, 56, 11), // "textChanged"
QT_MOC_LITERAL(6, 68, 11), // "slotReplace"
QT_MOC_LITERAL(7, 80, 12) // "slotNextFind"

    },
    "ScriptEditorReplaceDialog\0replaceAll\0"
    "\0initFind\0nextFind\0textChanged\0"
    "slotReplace\0slotNextFind"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScriptEditorReplaceDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   44,    2, 0x06 /* Public */,
       3,    0,   49,    2, 0x06 /* Public */,
       4,    1,   50,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   53,    2, 0x08 /* Private */,
       6,    0,   56,    2, 0x08 /* Private */,
       7,    0,   57,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    2,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ScriptEditorReplaceDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ScriptEditorReplaceDialog *_t = static_cast<ScriptEditorReplaceDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->replaceAll((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 1: _t->initFind(); break;
        case 2: _t->nextFind((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->slotReplace(); break;
        case 5: _t->slotNextFind(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (ScriptEditorReplaceDialog::*_t)(const QString & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ScriptEditorReplaceDialog::replaceAll)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (ScriptEditorReplaceDialog::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ScriptEditorReplaceDialog::initFind)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (ScriptEditorReplaceDialog::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ScriptEditorReplaceDialog::nextFind)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject ScriptEditorReplaceDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ScriptEditorReplaceDialog.data,
      qt_meta_data_ScriptEditorReplaceDialog,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ScriptEditorReplaceDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScriptEditorReplaceDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ScriptEditorReplaceDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int ScriptEditorReplaceDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void ScriptEditorReplaceDialog::replaceAll(const QString & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ScriptEditorReplaceDialog::initFind()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ScriptEditorReplaceDialog::nextFind(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
