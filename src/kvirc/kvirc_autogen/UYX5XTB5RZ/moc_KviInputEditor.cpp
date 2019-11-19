/****************************************************************************
** Meta object code from reading C++ file 'KviInputEditor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ui/KviInputEditor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KviInputEditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KviInputEditor_t {
    QByteArrayData data[81];
    char stringdata0[1134];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KviInputEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KviInputEditor_t qt_meta_stringdata_KviInputEditor = {
    {
QT_MOC_LITERAL(0, 0, 14), // "KviInputEditor"
QT_MOC_LITERAL(1, 15, 13), // "escapePressed"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 12), // "enterPressed"
QT_MOC_LITERAL(4, 43, 18), // "iconPopupActivated"
QT_MOC_LITERAL(5, 62, 8), // "QAction*"
QT_MOC_LITERAL(6, 71, 7), // "pAction"
QT_MOC_LITERAL(7, 79, 15), // "copyToSelection"
QT_MOC_LITERAL(8, 95, 22), // "bDonNotCopyToClipboard"
QT_MOC_LITERAL(9, 118, 15), // "copyToClipboard"
QT_MOC_LITERAL(10, 134, 14), // "removeSelected"
QT_MOC_LITERAL(11, 149, 3), // "cut"
QT_MOC_LITERAL(12, 153, 4), // "undo"
QT_MOC_LITERAL(13, 158, 4), // "redo"
QT_MOC_LITERAL(14, 163, 30), // "pasteClipboardWithConfirmation"
QT_MOC_LITERAL(15, 194, 30), // "pasteSelectionWithConfirmation"
QT_MOC_LITERAL(16, 225, 9), // "pasteSlow"
QT_MOC_LITERAL(17, 235, 13), // "stopPasteSlow"
QT_MOC_LITERAL(18, 249, 9), // "pasteFile"
QT_MOC_LITERAL(19, 259, 9), // "selectAll"
QT_MOC_LITERAL(20, 269, 5), // "clear"
QT_MOC_LITERAL(21, 275, 11), // "openHistory"
QT_MOC_LITERAL(22, 287, 21), // "toggleMultiLineEditor"
QT_MOC_LITERAL(23, 309, 12), // "previousChar"
QT_MOC_LITERAL(24, 322, 8), // "nextChar"
QT_MOC_LITERAL(25, 331, 21), // "previousCharSelection"
QT_MOC_LITERAL(26, 353, 17), // "nextCharSelection"
QT_MOC_LITERAL(27, 371, 12), // "previousWord"
QT_MOC_LITERAL(28, 384, 8), // "nextWord"
QT_MOC_LITERAL(29, 393, 21), // "previousWordSelection"
QT_MOC_LITERAL(30, 415, 17), // "nextWordSelection"
QT_MOC_LITERAL(31, 433, 10), // "insertBold"
QT_MOC_LITERAL(32, 444, 12), // "insertItalic"
QT_MOC_LITERAL(33, 457, 11), // "insertReset"
QT_MOC_LITERAL(34, 469, 15), // "insertUnderline"
QT_MOC_LITERAL(35, 485, 13), // "insertReverse"
QT_MOC_LITERAL(36, 499, 15), // "insertPlainText"
QT_MOC_LITERAL(37, 515, 10), // "insertIcon"
QT_MOC_LITERAL(38, 526, 11), // "insertColor"
QT_MOC_LITERAL(39, 538, 12), // "copyInternal"
QT_MOC_LITERAL(40, 551, 11), // "cutInternal"
QT_MOC_LITERAL(41, 563, 13), // "pasteInternal"
QT_MOC_LITERAL(42, 577, 12), // "undoInternal"
QT_MOC_LITERAL(43, 590, 12), // "redoInternal"
QT_MOC_LITERAL(44, 603, 17), // "selectAllInternal"
QT_MOC_LITERAL(45, 621, 18), // "deletePreviousWord"
QT_MOC_LITERAL(46, 640, 14), // "deleteNextWord"
QT_MOC_LITERAL(47, 655, 12), // "previousLine"
QT_MOC_LITERAL(48, 668, 8), // "nextLine"
QT_MOC_LITERAL(49, 677, 12), // "previousPage"
QT_MOC_LITERAL(50, 690, 8), // "nextPage"
QT_MOC_LITERAL(51, 699, 9), // "scrollTop"
QT_MOC_LITERAL(52, 709, 12), // "scrollBottom"
QT_MOC_LITERAL(53, 722, 6), // "search"
QT_MOC_LITERAL(54, 729, 20), // "scrollToLastReadLine"
QT_MOC_LITERAL(55, 750, 9), // "sendPlain"
QT_MOC_LITERAL(56, 760, 7), // "sendKvs"
QT_MOC_LITERAL(57, 768, 12), // "homeInternal"
QT_MOC_LITERAL(58, 781, 11), // "endInternal"
QT_MOC_LITERAL(59, 793, 21), // "homeInternalSelection"
QT_MOC_LITERAL(60, 815, 20), // "endInternalSelection"
QT_MOC_LITERAL(61, 836, 11), // "historyPrev"
QT_MOC_LITERAL(62, 848, 11), // "historyNext"
QT_MOC_LITERAL(63, 860, 9), // "returnHit"
QT_MOC_LITERAL(64, 870, 12), // "backspaceHit"
QT_MOC_LITERAL(65, 883, 9), // "deleteHit"
QT_MOC_LITERAL(66, 893, 9), // "escapeHit"
QT_MOC_LITERAL(67, 903, 17), // "toggleCommandMode"
QT_MOC_LITERAL(68, 921, 5), // "dummy"
QT_MOC_LITERAL(69, 927, 6), // "zoomIn"
QT_MOC_LITERAL(70, 934, 7), // "zoomOut"
QT_MOC_LITERAL(71, 942, 11), // "zoomDefault"
QT_MOC_LITERAL(72, 954, 19), // "popupTextIconWindow"
QT_MOC_LITERAL(73, 974, 16), // "showContextPopup"
QT_MOC_LITERAL(74, 991, 3), // "pos"
QT_MOC_LITERAL(75, 995, 20), // "showContextPopupHere"
QT_MOC_LITERAL(76, 1016, 32), // "showSpellCheckerCorrectionsPopup"
QT_MOC_LITERAL(77, 1049, 42), // "spellCheckerPopupCorrectionAc..."
QT_MOC_LITERAL(78, 1092, 12), // "addToHistory"
QT_MOC_LITERAL(79, 1105, 8), // "szString"
QT_MOC_LITERAL(80, 1114, 19) // "TransparencyCapable"

    },
    "KviInputEditor\0escapePressed\0\0"
    "enterPressed\0iconPopupActivated\0"
    "QAction*\0pAction\0copyToSelection\0"
    "bDonNotCopyToClipboard\0copyToClipboard\0"
    "removeSelected\0cut\0undo\0redo\0"
    "pasteClipboardWithConfirmation\0"
    "pasteSelectionWithConfirmation\0pasteSlow\0"
    "stopPasteSlow\0pasteFile\0selectAll\0"
    "clear\0openHistory\0toggleMultiLineEditor\0"
    "previousChar\0nextChar\0previousCharSelection\0"
    "nextCharSelection\0previousWord\0nextWord\0"
    "previousWordSelection\0nextWordSelection\0"
    "insertBold\0insertItalic\0insertReset\0"
    "insertUnderline\0insertReverse\0"
    "insertPlainText\0insertIcon\0insertColor\0"
    "copyInternal\0cutInternal\0pasteInternal\0"
    "undoInternal\0redoInternal\0selectAllInternal\0"
    "deletePreviousWord\0deleteNextWord\0"
    "previousLine\0nextLine\0previousPage\0"
    "nextPage\0scrollTop\0scrollBottom\0search\0"
    "scrollToLastReadLine\0sendPlain\0sendKvs\0"
    "homeInternal\0endInternal\0homeInternalSelection\0"
    "endInternalSelection\0historyPrev\0"
    "historyNext\0returnHit\0backspaceHit\0"
    "deleteHit\0escapeHit\0toggleCommandMode\0"
    "dummy\0zoomIn\0zoomOut\0zoomDefault\0"
    "popupTextIconWindow\0showContextPopup\0"
    "pos\0showContextPopupHere\0"
    "showSpellCheckerCorrectionsPopup\0"
    "spellCheckerPopupCorrectionActionTriggered\0"
    "addToHistory\0szString\0TransparencyCapable"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KviInputEditor[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      74,   14, // methods
       1,  466, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,  384,    2, 0x06 /* Public */,
       3,    0,  385,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,  386,    2, 0x0a /* Public */,
       7,    1,  389,    2, 0x0a /* Public */,
       7,    0,  392,    2, 0x2a /* Public | MethodCloned */,
       9,    0,  393,    2, 0x0a /* Public */,
      10,    0,  394,    2, 0x0a /* Public */,
      11,    0,  395,    2, 0x0a /* Public */,
      12,    0,  396,    2, 0x0a /* Public */,
      13,    0,  397,    2, 0x0a /* Public */,
      14,    0,  398,    2, 0x0a /* Public */,
      15,    0,  399,    2, 0x0a /* Public */,
      16,    0,  400,    2, 0x0a /* Public */,
      17,    0,  401,    2, 0x0a /* Public */,
      18,    0,  402,    2, 0x0a /* Public */,
      19,    0,  403,    2, 0x0a /* Public */,
      20,    0,  404,    2, 0x0a /* Public */,
      21,    0,  405,    2, 0x08 /* Private */,
      22,    0,  406,    2, 0x08 /* Private */,
      23,    0,  407,    2, 0x08 /* Private */,
      24,    0,  408,    2, 0x08 /* Private */,
      25,    0,  409,    2, 0x08 /* Private */,
      26,    0,  410,    2, 0x08 /* Private */,
      27,    0,  411,    2, 0x08 /* Private */,
      28,    0,  412,    2, 0x08 /* Private */,
      29,    0,  413,    2, 0x08 /* Private */,
      30,    0,  414,    2, 0x08 /* Private */,
      31,    0,  415,    2, 0x08 /* Private */,
      32,    0,  416,    2, 0x08 /* Private */,
      33,    0,  417,    2, 0x08 /* Private */,
      34,    0,  418,    2, 0x08 /* Private */,
      35,    0,  419,    2, 0x08 /* Private */,
      36,    0,  420,    2, 0x08 /* Private */,
      37,    0,  421,    2, 0x08 /* Private */,
      38,    0,  422,    2, 0x08 /* Private */,
      39,    0,  423,    2, 0x08 /* Private */,
      40,    0,  424,    2, 0x08 /* Private */,
      41,    0,  425,    2, 0x08 /* Private */,
      42,    0,  426,    2, 0x08 /* Private */,
      43,    0,  427,    2, 0x08 /* Private */,
      44,    0,  428,    2, 0x08 /* Private */,
      45,    0,  429,    2, 0x08 /* Private */,
      46,    0,  430,    2, 0x08 /* Private */,
      47,    0,  431,    2, 0x08 /* Private */,
      48,    0,  432,    2, 0x08 /* Private */,
      49,    0,  433,    2, 0x08 /* Private */,
      50,    0,  434,    2, 0x08 /* Private */,
      51,    0,  435,    2, 0x08 /* Private */,
      52,    0,  436,    2, 0x08 /* Private */,
      53,    0,  437,    2, 0x08 /* Private */,
      54,    0,  438,    2, 0x08 /* Private */,
      55,    0,  439,    2, 0x08 /* Private */,
      56,    0,  440,    2, 0x08 /* Private */,
      57,    0,  441,    2, 0x08 /* Private */,
      58,    0,  442,    2, 0x08 /* Private */,
      59,    0,  443,    2, 0x08 /* Private */,
      60,    0,  444,    2, 0x08 /* Private */,
      61,    0,  445,    2, 0x08 /* Private */,
      62,    0,  446,    2, 0x08 /* Private */,
      63,    0,  447,    2, 0x08 /* Private */,
      64,    0,  448,    2, 0x08 /* Private */,
      65,    0,  449,    2, 0x08 /* Private */,
      66,    0,  450,    2, 0x08 /* Private */,
      67,    0,  451,    2, 0x08 /* Private */,
      68,    0,  452,    2, 0x08 /* Private */,
      69,    0,  453,    2, 0x08 /* Private */,
      70,    0,  454,    2, 0x08 /* Private */,
      71,    0,  455,    2, 0x08 /* Private */,
      72,    0,  456,    2, 0x08 /* Private */,
      73,    1,  457,    2, 0x08 /* Private */,
      75,    0,  460,    2, 0x08 /* Private */,
      76,    0,  461,    2, 0x08 /* Private */,
      77,    0,  462,    2, 0x08 /* Private */,
      78,    1,  463,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,   74,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   79,

 // properties: name, type, flags
      80, QMetaType::Int, 0x00095001,

       0        // eod
};

void KviInputEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KviInputEditor *_t = static_cast<KviInputEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->escapePressed(); break;
        case 1: _t->enterPressed(); break;
        case 2: _t->iconPopupActivated((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 3: _t->copyToSelection((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->copyToSelection(); break;
        case 5: _t->copyToClipboard(); break;
        case 6: _t->removeSelected(); break;
        case 7: _t->cut(); break;
        case 8: _t->undo(); break;
        case 9: _t->redo(); break;
        case 10: _t->pasteClipboardWithConfirmation(); break;
        case 11: _t->pasteSelectionWithConfirmation(); break;
        case 12: _t->pasteSlow(); break;
        case 13: _t->stopPasteSlow(); break;
        case 14: _t->pasteFile(); break;
        case 15: _t->selectAll(); break;
        case 16: _t->clear(); break;
        case 17: _t->openHistory(); break;
        case 18: _t->toggleMultiLineEditor(); break;
        case 19: _t->previousChar(); break;
        case 20: _t->nextChar(); break;
        case 21: _t->previousCharSelection(); break;
        case 22: _t->nextCharSelection(); break;
        case 23: _t->previousWord(); break;
        case 24: _t->nextWord(); break;
        case 25: _t->previousWordSelection(); break;
        case 26: _t->nextWordSelection(); break;
        case 27: _t->insertBold(); break;
        case 28: _t->insertItalic(); break;
        case 29: _t->insertReset(); break;
        case 30: _t->insertUnderline(); break;
        case 31: _t->insertReverse(); break;
        case 32: _t->insertPlainText(); break;
        case 33: _t->insertIcon(); break;
        case 34: _t->insertColor(); break;
        case 35: _t->copyInternal(); break;
        case 36: _t->cutInternal(); break;
        case 37: _t->pasteInternal(); break;
        case 38: _t->undoInternal(); break;
        case 39: _t->redoInternal(); break;
        case 40: _t->selectAllInternal(); break;
        case 41: _t->deletePreviousWord(); break;
        case 42: _t->deleteNextWord(); break;
        case 43: _t->previousLine(); break;
        case 44: _t->nextLine(); break;
        case 45: _t->previousPage(); break;
        case 46: _t->nextPage(); break;
        case 47: _t->scrollTop(); break;
        case 48: _t->scrollBottom(); break;
        case 49: _t->search(); break;
        case 50: _t->scrollToLastReadLine(); break;
        case 51: _t->sendPlain(); break;
        case 52: _t->sendKvs(); break;
        case 53: _t->homeInternal(); break;
        case 54: _t->endInternal(); break;
        case 55: _t->homeInternalSelection(); break;
        case 56: _t->endInternalSelection(); break;
        case 57: _t->historyPrev(); break;
        case 58: _t->historyNext(); break;
        case 59: _t->returnHit(); break;
        case 60: _t->backspaceHit(); break;
        case 61: _t->deleteHit(); break;
        case 62: _t->escapeHit(); break;
        case 63: _t->toggleCommandMode(); break;
        case 64: _t->dummy(); break;
        case 65: _t->zoomIn(); break;
        case 66: _t->zoomOut(); break;
        case 67: _t->zoomDefault(); break;
        case 68: _t->popupTextIconWindow(); break;
        case 69: _t->showContextPopup((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 70: _t->showContextPopupHere(); break;
        case 71: _t->showSpellCheckerCorrectionsPopup(); break;
        case 72: _t->spellCheckerPopupCorrectionActionTriggered(); break;
        case 73: _t->addToHistory((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAction* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (KviInputEditor::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviInputEditor::escapePressed)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (KviInputEditor::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KviInputEditor::enterPressed)) {
                *result = 1;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        KviInputEditor *_t = static_cast<KviInputEditor *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->heightHint(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject KviInputEditor::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_KviInputEditor.data,
      qt_meta_data_KviInputEditor,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KviInputEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KviInputEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KviInputEditor.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int KviInputEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 74)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 74;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 74)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 74;
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
void KviInputEditor::escapePressed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void KviInputEditor::enterPressed()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
