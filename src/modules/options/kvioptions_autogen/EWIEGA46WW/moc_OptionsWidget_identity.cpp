/****************************************************************************
** Meta object code from reading C++ file 'OptionsWidget_identity.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../OptionsWidget_identity.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'OptionsWidget_identity.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_NickAlternativesDialog_t {
    QByteArrayData data[1];
    char stringdata0[23];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_NickAlternativesDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_NickAlternativesDialog_t qt_meta_stringdata_NickAlternativesDialog = {
    {
QT_MOC_LITERAL(0, 0, 22) // "NickAlternativesDialog"

    },
    "NickAlternativesDialog"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_NickAlternativesDialog[] = {

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

void NickAlternativesDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject NickAlternativesDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_NickAlternativesDialog.data,
      qt_meta_data_NickAlternativesDialog,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *NickAlternativesDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NickAlternativesDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_NickAlternativesDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int NickAlternativesDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_AvatarSelectionDialog_t {
    QByteArrayData data[5];
    char stringdata0[65];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AvatarSelectionDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AvatarSelectionDialog_t qt_meta_stringdata_AvatarSelectionDialog = {
    {
QT_MOC_LITERAL(0, 0, 21), // "AvatarSelectionDialog"
QT_MOC_LITERAL(1, 22, 9), // "okClicked"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 13), // "cancelClicked"
QT_MOC_LITERAL(4, 47, 17) // "chooseFileClicked"

    },
    "AvatarSelectionDialog\0okClicked\0\0"
    "cancelClicked\0chooseFileClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AvatarSelectionDialog[] = {

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
       3,    0,   30,    2, 0x09 /* Protected */,
       4,    0,   31,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void AvatarSelectionDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AvatarSelectionDialog *_t = static_cast<AvatarSelectionDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->okClicked(); break;
        case 1: _t->cancelClicked(); break;
        case 2: _t->chooseFileClicked(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject AvatarSelectionDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_AvatarSelectionDialog.data,
      qt_meta_data_AvatarSelectionDialog,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *AvatarSelectionDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AvatarSelectionDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AvatarSelectionDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int AvatarSelectionDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_AvatarDownloadDialog_t {
    QByteArrayData data[8];
    char stringdata0[100];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AvatarDownloadDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AvatarDownloadDialog_t qt_meta_stringdata_AvatarDownloadDialog = {
    {
QT_MOC_LITERAL(0, 0, 20), // "AvatarDownloadDialog"
QT_MOC_LITERAL(1, 21, 13), // "startDownload"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 13), // "cancelClicked"
QT_MOC_LITERAL(4, 50, 18), // "downloadTerminated"
QT_MOC_LITERAL(5, 69, 8), // "bSuccess"
QT_MOC_LITERAL(6, 78, 15), // "downloadMessage"
QT_MOC_LITERAL(7, 94, 5) // "szMsg"

    },
    "AvatarDownloadDialog\0startDownload\0\0"
    "cancelClicked\0downloadTerminated\0"
    "bSuccess\0downloadMessage\0szMsg"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AvatarDownloadDialog[] = {

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
       4,    1,   36,    2, 0x09 /* Protected */,
       6,    1,   39,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void, QMetaType::QString,    7,

       0        // eod
};

void AvatarDownloadDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AvatarDownloadDialog *_t = static_cast<AvatarDownloadDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->startDownload(); break;
        case 1: _t->cancelClicked(); break;
        case 2: _t->downloadTerminated((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->downloadMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject AvatarDownloadDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_AvatarDownloadDialog.data,
      qt_meta_data_AvatarDownloadDialog,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *AvatarDownloadDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AvatarDownloadDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AvatarDownloadDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int AvatarDownloadDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
struct qt_meta_stringdata_IdentityProfileEditor_t {
    QByteArrayData data[3];
    char stringdata0[36];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_IdentityProfileEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_IdentityProfileEditor_t qt_meta_stringdata_IdentityProfileEditor = {
    {
QT_MOC_LITERAL(0, 0, 21), // "IdentityProfileEditor"
QT_MOC_LITERAL(1, 22, 12), // "toggleButton"
QT_MOC_LITERAL(2, 35, 0) // ""

    },
    "IdentityProfileEditor\0toggleButton\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_IdentityProfileEditor[] = {

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

void IdentityProfileEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        IdentityProfileEditor *_t = static_cast<IdentityProfileEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->toggleButton(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject IdentityProfileEditor::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_IdentityProfileEditor.data,
      qt_meta_data_IdentityProfileEditor,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *IdentityProfileEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *IdentityProfileEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_IdentityProfileEditor.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int IdentityProfileEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
struct qt_meta_stringdata_OptionsWidget_identity_t {
    QByteArrayData data[1];
    char stringdata0[23];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_OptionsWidget_identity_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_OptionsWidget_identity_t qt_meta_stringdata_OptionsWidget_identity = {
    {
QT_MOC_LITERAL(0, 0, 22) // "OptionsWidget_identity"

    },
    "OptionsWidget_identity"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_OptionsWidget_identity[] = {

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

void OptionsWidget_identity::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject OptionsWidget_identity::staticMetaObject = {
    { &KviOptionsWidget::staticMetaObject, qt_meta_stringdata_OptionsWidget_identity.data,
      qt_meta_data_OptionsWidget_identity,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *OptionsWidget_identity::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OptionsWidget_identity::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_OptionsWidget_identity.stringdata0))
        return static_cast<void*>(this);
    return KviOptionsWidget::qt_metacast(_clname);
}

int OptionsWidget_identity::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviOptionsWidget::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_KviIdentityGeneralOptionsWidget_t {
    QByteArrayData data[3];
    char stringdata0[53];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviIdentityGeneralOptionsWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviIdentityGeneralOptionsWidget_t qt_meta_stringdata_KviIdentityGeneralOptionsWidget = {
    {
QT_MOC_LITERAL(0, 0, 31), // "KviIdentityGeneralOptionsWidget"
QT_MOC_LITERAL(1, 32, 19), // "setNickAlternatives"
QT_MOC_LITERAL(2, 52, 0) // ""

    },
    "KviIdentityGeneralOptionsWidget\0"
    "setNickAlternatives\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviIdentityGeneralOptionsWidget[] = {

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

void KviIdentityGeneralOptionsWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviIdentityGeneralOptionsWidget *_t = static_cast<KviIdentityGeneralOptionsWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setNickAlternatives(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject KviIdentityGeneralOptionsWidget::staticMetaObject = {
    { &KviOptionsWidget::staticMetaObject, qt_meta_stringdata_KviIdentityGeneralOptionsWidget.data,
      qt_meta_data_KviIdentityGeneralOptionsWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviIdentityGeneralOptionsWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviIdentityGeneralOptionsWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviIdentityGeneralOptionsWidget.stringdata0))
        return static_cast<void*>(this);
    return KviOptionsWidget::qt_metacast(_clname);
}

int KviIdentityGeneralOptionsWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviOptionsWidget::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_OptionsWidget_identityAvatar_t {
    QByteArrayData data[3];
    char stringdata0[43];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_OptionsWidget_identityAvatar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_OptionsWidget_identityAvatar_t qt_meta_stringdata_OptionsWidget_identityAvatar = {
    {
QT_MOC_LITERAL(0, 0, 28), // "OptionsWidget_identityAvatar"
QT_MOC_LITERAL(1, 29, 12), // "chooseAvatar"
QT_MOC_LITERAL(2, 42, 0) // ""

    },
    "OptionsWidget_identityAvatar\0chooseAvatar\0"
    ""
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_OptionsWidget_identityAvatar[] = {

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

void OptionsWidget_identityAvatar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        OptionsWidget_identityAvatar *_t = static_cast<OptionsWidget_identityAvatar *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->chooseAvatar(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject OptionsWidget_identityAvatar::staticMetaObject = {
    { &KviOptionsWidget::staticMetaObject, qt_meta_stringdata_OptionsWidget_identityAvatar.data,
      qt_meta_data_OptionsWidget_identityAvatar,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *OptionsWidget_identityAvatar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OptionsWidget_identityAvatar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_OptionsWidget_identityAvatar.stringdata0))
        return static_cast<void*>(this);
    return KviOptionsWidget::qt_metacast(_clname);
}

int OptionsWidget_identityAvatar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviOptionsWidget::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_OptionsWidget_identityAdvanced_t {
    QByteArrayData data[1];
    char stringdata0[31];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_OptionsWidget_identityAdvanced_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_OptionsWidget_identityAdvanced_t qt_meta_stringdata_OptionsWidget_identityAdvanced = {
    {
QT_MOC_LITERAL(0, 0, 30) // "OptionsWidget_identityAdvanced"

    },
    "OptionsWidget_identityAdvanced"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_OptionsWidget_identityAdvanced[] = {

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

void OptionsWidget_identityAdvanced::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject OptionsWidget_identityAdvanced::staticMetaObject = {
    { &KviOptionsWidget::staticMetaObject, qt_meta_stringdata_OptionsWidget_identityAdvanced.data,
      qt_meta_data_OptionsWidget_identityAdvanced,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *OptionsWidget_identityAdvanced::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OptionsWidget_identityAdvanced::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_OptionsWidget_identityAdvanced.stringdata0))
        return static_cast<void*>(this);
    return KviOptionsWidget::qt_metacast(_clname);
}

int OptionsWidget_identityAdvanced::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviOptionsWidget::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_OptionsWidget_identityProfile_t {
    QByteArrayData data[7];
    char stringdata0[116];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_OptionsWidget_identityProfile_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_OptionsWidget_identityProfile_t qt_meta_stringdata_OptionsWidget_identityProfile = {
    {
QT_MOC_LITERAL(0, 0, 29), // "OptionsWidget_identityProfile"
QT_MOC_LITERAL(1, 30, 14), // "toggleControls"
QT_MOC_LITERAL(2, 45, 0), // ""
QT_MOC_LITERAL(3, 46, 15), // "addProfileEntry"
QT_MOC_LITERAL(4, 62, 16), // "editProfileEntry"
QT_MOC_LITERAL(5, 79, 15), // "delProfileEntry"
QT_MOC_LITERAL(6, 95, 20) // "editProfileOkPressed"

    },
    "OptionsWidget_identityProfile\0"
    "toggleControls\0\0addProfileEntry\0"
    "editProfileEntry\0delProfileEntry\0"
    "editProfileOkPressed"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_OptionsWidget_identityProfile[] = {

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
       1,    0,   39,    2, 0x09 /* Protected */,
       3,    0,   40,    2, 0x09 /* Protected */,
       4,    0,   41,    2, 0x09 /* Protected */,
       5,    0,   42,    2, 0x09 /* Protected */,
       6,    0,   43,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void OptionsWidget_identityProfile::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        OptionsWidget_identityProfile *_t = static_cast<OptionsWidget_identityProfile *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->toggleControls(); break;
        case 1: _t->addProfileEntry(); break;
        case 2: _t->editProfileEntry(); break;
        case 3: _t->delProfileEntry(); break;
        case 4: _t->editProfileOkPressed(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject OptionsWidget_identityProfile::staticMetaObject = {
    { &KviOptionsWidget::staticMetaObject, qt_meta_stringdata_OptionsWidget_identityProfile.data,
      qt_meta_data_OptionsWidget_identityProfile,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *OptionsWidget_identityProfile::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OptionsWidget_identityProfile::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_OptionsWidget_identityProfile.stringdata0))
        return static_cast<void*>(this);
    return KviOptionsWidget::qt_metacast(_clname);
}

int OptionsWidget_identityProfile::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviOptionsWidget::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
