#ifndef _PYTHONHEADERWRAPPER_H_
#define _PYTHONHEADERWRAPPER_H_

// As of Python 3, something inside <Python.h> defines a struct with a member
// called "slots" which conflicts with the builtin Qt keyword. But since we
// include stuff from KVIrc itself back into the python module, we can't just
// use QT_NO_KEYWORDS.
#undef slots

// See http://stackoverflow.com/questions/16200997/why-doesnt-include-python-h-work and http://stackoverflow.com/questions/19716859/puzzling-dependency-of-boost-python-1-54-debug-build-to-python27-lib-on-window

#if defined(_DEBUG) && defined(_MSC_VER)
# undef _DEBUG
# include <Python.h>
# define _DEBUG 1
#else
# include <Python.h>
#endif

#endif
