/****************************************************************************
** Meta object code from reading C++ file 'grabberprogressdialog.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/grabberprogressdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'grabberprogressdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GrabberProgressDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      44,   22,   23,   22, 0x0a,
      54,   22,   22,   22, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_GrabberProgressDialog[] = {
    "GrabberProgressDialog\0\0std::vector<RScloud>\0"
    "perform()\0cancel()\0"
};

void GrabberProgressDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        GrabberProgressDialog *_t = static_cast<GrabberProgressDialog *>(_o);
        switch (_id) {
        case 0: { std::vector<RScloud> _r = _t->perform();
            if (_a[0]) *reinterpret_cast< std::vector<RScloud>*>(_a[0]) = _r; }  break;
        case 1: _t->cancel(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData GrabberProgressDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject GrabberProgressDialog::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_GrabberProgressDialog,
      qt_meta_data_GrabberProgressDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GrabberProgressDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GrabberProgressDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GrabberProgressDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GrabberProgressDialog))
        return static_cast<void*>(const_cast< GrabberProgressDialog*>(this));
    return QObject::qt_metacast(_clname);
}

int GrabberProgressDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
