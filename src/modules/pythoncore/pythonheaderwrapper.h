#ifndef _PYTHONHEADERWRAPPER_H_
#define _PYTHONHEADERWRAPPER_H_

// As of Python 3, something inside <Python.h> defines a struct with a member
// called "slots" which conflicts with the builtin Qt keyword. But since we
// include stuff from KVIrc itself back into the python module, we can't just
// use QT_NO_KEYWORDS.
#undef slots

#include <Python.h>

#endif
