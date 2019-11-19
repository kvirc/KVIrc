/****************************************************************************
** Meta object code from reading C++ file 'KviTopicWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ui/KviTopicWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KviTopicWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviTopicListBoxItemDelegate_t {
    QByteArrayData data[1];
    char stringdata0[28];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviTopicListBoxItemDelegate_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviTopicListBoxItemDelegate_t qt_meta_stringdata_KviTopicListBoxItemDelegate = {
    {
QT_MOC_LITERAL(0, 0, 27) // "KviTopicListBoxItemDelegate"

    },
    "KviTopicListBoxItemDelegate"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviTopicListBoxItemDelegate[] = {

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

void KviTopicListBoxItemDelegate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject KviTopicListBoxItemDelegate::staticMetaObject = {
    { &KviTalIconAndRichTextItemDelegate::staticMetaObject, qt_meta_stringdata_KviTopicListBoxItemDelegate.data,
      qt_meta_data_KviTopicListBoxItemDelegate,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviTopicListBoxItemDelegate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviTopicListBoxItemDelegate::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviTopicListBoxItemDelegate.stringdata0))
        return static_cast<void*>(this);
    return KviTalIconAndRichTextItemDelegate::qt_metacast(_clname);
}

int KviTopicListBoxItemDelegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KviTalIconAndRichTextItemDelegate::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_KviTopicWidget_t {
    QByteArrayData data[13];
    char stringdata0[166];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviTopicWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviTopicWidget_t qt_meta_stringdata_KviTopicWidget = {
    {
QT_MOC_LITERAL(0, 0, 14), // "KviTopicWidget"
QT_MOC_LITERAL(1, 15, 13), // "topicSelected"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 7), // "szTopic"
QT_MOC_LITERAL(4, 38, 13), // "acceptClicked"
QT_MOC_LITERAL(5, 52, 14), // "discardClicked"
QT_MOC_LITERAL(6, 67, 14), // "historyClicked"
QT_MOC_LITERAL(7, 82, 23), // "contextPopupAboutToShow"
QT_MOC_LITERAL(8, 106, 4), // "copy"
QT_MOC_LITERAL(9, 111, 8), // "complete"
QT_MOC_LITERAL(10, 120, 14), // "popDownListBox"
QT_MOC_LITERAL(11, 135, 10), // "switchMode"
QT_MOC_LITERAL(12, 146, 19) // "TransparencyCapable"

    },
    "KviTopicWidget\0topicSelected\0\0szTopic\0"
    "acceptClicked\0discardClicked\0"
    "historyClicked\0contextPopupAboutToShow\0"
    "copy\0complete\0popDownListBox\0switchMode\0"
    "TransparencyCapable"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviTopicWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       1,   70, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   59,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   62,    2, 0x09 /* Protected */,
       5,    0,   63,    2, 0x09 /* Protected */,
       6,    0,   64,    2, 0x09 /* Protected */,
       7,    0,   65,    2, 0x09 /* Protected */,
       8,    0,   66,    2, 0x09 /* Protected */,
       9,    0,   67,    2, 0x09 /* Protected */,
      10,    0,   68,    2, 0x09 /* Protected */,
      11,    0,   69,    2, 0x09 /* Protected */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
      12, QMetaType::Int, 0x00095001,

       0        // eod
};

void KviTopicWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviTopicWidget *_t = static_cast<KviTopicWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->topicSelected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->acceptClicked(); break;
        case 2: _t->discardClicked(); break;
        case 3: _t->historyClicked(); break;
        case 4: _t->contextPopupAboutToShow(); break;
        case 5: _t->copy(); break;
        case 6: _t->complete(); break;
        case 7: _t->popDownListBox(); break;
        case 8: _t->switchMode(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (KviTopicWidget::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviTopicWidget::topicSelected)) {
                *result = 0;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        KviTopicWidget *_t = static_cast<KviTopicWidget *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->dummyRead(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject KviTopicWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_KviTopicWidget.data,
      qt_meta_data_KviTopicWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviTopicWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviTopicWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviTopicWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int KviTopicWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
void KviTopicWidget::topicSelected(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
