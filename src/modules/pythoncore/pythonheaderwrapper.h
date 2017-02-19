#ifndef _PYTHONHEADERWRAPPER_H_
#define _PYTHONHEADERWRAPPER_H_

// See http://stackoverflow.com/questions/16200997/why-doesnt-include-python-h-work and http://stackoverflow.com/questions/19716859/puzzling-dependency-of-boost-python-1-54-debug-build-to-python27-lib-on-window

#if defined(_DEBUG) && defined(_MSC_VER)
# undef _DEBUG
# include <Python.h>
# define _DEBUG 1
#else
# include <Python.h>
#endif

#endif
