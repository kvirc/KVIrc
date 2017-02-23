//=============================================================================
//
//   File : libkvimath.cpp
//   Creation date : Sat Jan 13 14:00:12 2006 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2006-2010 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your option) any later version.
//
//   This program is distributed in the HOPE that it will be USEFUL,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program. If not, write to the Free Software Foundation,
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "kvi_settings.h"
#include "KviModule.h"
#include "KviCString.h"

#include <cmath>

// Workaround for lost isnan and isinf definitions after
// inclusion of iostream.h on some macOS systems
#ifdef COMPILE_ON_MAC
#ifndef isnan
extern "C" int isnan(double);
#endif
#ifndef isinf
extern "C" int isinf(double);
#endif
#endif

/* Solaris doesn't have isinf? */
#if defined(__SVR4) && defined(__sun)
#ifndef isinf
#include <ieeefp.h>
#define isinf(x) (!finite((x)) && (x) == (x))
#endif
#endif

#define MATH_KVS_1PARAM_FUNCTION(__fncname, __paramname, __mathcallname) \
	static bool __fncname(KviKvsModuleFunctionCall * c)                  \
	{                                                                    \
		kvs_real_t dReal;                                                \
		KVSM_PARAMETERS_BEGIN(c)                                         \
		KVSM_PARAMETER(__paramname, KVS_PT_REAL, 0, dReal)               \
		KVSM_PARAMETERS_END(c)                                           \
		c->returnValue()->setReal(__mathcallname(dReal));                \
		return true;                                                     \
	}

/*
	@doc: math.sin
	@type:
		function
	@title:
		$math.sin
	@short:
		Returns the sinus of the specified angle in radiants
	@syntax:
		<real> $math.sin(<angle:real>)
	@description:
		Returns the sinus of the specified angle in radiants.
*/

MATH_KVS_1PARAM_FUNCTION(math_kvs_fnc_sin, "angle", sin)

/*
	@doc: math.cos
	@type:
		function
	@title:
		$math.cos
	@short:
		Returns the cosinus of the specified angle in radiants
	@syntax:
		<real> $math.cos(<angle:real>)
	@description:
		Returns the cosinus of the specified angle in radiants.
*/

MATH_KVS_1PARAM_FUNCTION(math_kvs_fnc_cos, "angle", cos)

/*
	@doc: math.tan
	@type:
		function
	@title:
		$math.tan
	@short:
		Returns the tangent of the specified angle in radiants
	@syntax:
		<real> $math.tan(<angle:real>)
	@description:
		Returns the tangent of the specified angle in radiants.
*/

MATH_KVS_1PARAM_FUNCTION(math_kvs_fnc_tan, "angle", tan)

/*
	@doc: math.asin
	@type:
		function
	@title:
		$math.asin
	@short:
		Returns the angle in radiants that has the specified sinus value.
	@syntax:
		<real> $math.asin(<sinus:real>)
	@description:
		Returns the angle in radiants that has the specified sinus value.
*/

MATH_KVS_1PARAM_FUNCTION(math_kvs_fnc_asin, "sinus", asin)

/*
	@doc: math.acos
	@type:
		function
	@title:
		$math.acos
	@short:
		Returns the angle in radiants that has the specified cosinus value.
	@syntax:
		<real> $math.acos(<cosinus:real>)
	@description:
		Returns the angle in radiants that has the specified cosinus value.
*/

MATH_KVS_1PARAM_FUNCTION(math_kvs_fnc_acos, "cosinus", acos)

/*
	@doc: math.atan
	@type:
		function
	@title:
		$math.atan
	@short:
		Returns the angle in radiants that has the specified tangent value.
	@syntax:
		<real> $math.atan(<tangent:real>)
	@description:
		Returns the angle in radiants that has the specified tangent value.
*/

MATH_KVS_1PARAM_FUNCTION(math_kvs_fnc_atan, "tangent", atan)

/*
	@doc: math.sqrt
	@type:
		function
	@title:
		$math.sqrt
	@short:
		Returns the square root of a number
	@syntax:
		<real> $math.sqrt(<number:real>)
	@description:
		Returns the square root of the specified number.
*/

MATH_KVS_1PARAM_FUNCTION(math_kvs_fnc_sqrt, "number", sqrt)

/*
	@doc: math.cbrt
	@type:
		function
	@title:
		$math.cbrt
	@short:
		Returns the cube root of a number
	@syntax:
		<real> $math.cbrt(<number:real>)
	@description:
		Returns the cube root of the specified number.
*/

MATH_KVS_1PARAM_FUNCTION(math_kvs_fnc_cbrt, "number", cbrt)

/*
	@doc: math.abs
	@type:
		function
	@title:
		$math.abs
	@short:
		Returns the absolute value of a number
	@syntax:
		<real> $math.abs(<number:real>)
	@description:
		Returns the absolute value of the specified number.
*/

MATH_KVS_1PARAM_FUNCTION(math_kvs_fnc_abs, "number", fabs)

/*
	@doc: math.floor
	@type:
		function
	@title:
		$math.floor
	@short:
		Rounds down to the nearest integer
	@syntax:
		<real> $math.floor(<number:real>)
	@description:
		Rounds down the number to the nearest integer.
*/

MATH_KVS_1PARAM_FUNCTION(math_kvs_fnc_floor, "number", floor)

/*
	@doc: math.ceil
	@type:
		function
	@title:
		$math.ceil
	@short:
		Rounds up to the nearest integer
	@syntax:
		<real> $math.ceil(<number:real>)
	@description:
		Rounds up the number to the nearest integer.
*/

MATH_KVS_1PARAM_FUNCTION(math_kvs_fnc_ceil, "number", ceil)

/*
	@doc: math.exp
	@type:
		function
	@title:
		$math.exp
	@short:
		Returns the exponential of the specified number
	@syntax:
		<real> $math.exp(<number:real>)
	@description:
		Returns the exponential of the specified number.
*/

MATH_KVS_1PARAM_FUNCTION(math_kvs_fnc_exp, "number", exp)

/*
	@doc: math.log
	@type:
		function
	@title:
		$math.log
	@short:
		Returns the natural logarithm of the specified number
	@syntax:
		<real> $math.log(<number:real>)
	@description:
		Returns the natural (base e) logarithm of the specified number.
*/

MATH_KVS_1PARAM_FUNCTION(math_kvs_fnc_log, "number", log)

/*
	@doc: math.log10
	@type:
		function
	@title:
		$math.log10
	@short:
		Returns the base 10 logarithm of the specified number
	@syntax:
		<real> $math.log10(<number:real>)
	@description:
		Returns the base 10 logarithm of the specified number.
*/

MATH_KVS_1PARAM_FUNCTION(math_kvs_fnc_log10, "number", log10)

/*
	@doc: math.isnan
	@type:
		function
	@title:
		$math.isnan
	@short:
		Checks if the value is not a number
	@syntax:
		<boolean> $math.isnan(<value:real>)
	@description:
		Checks if the specified value is a valid number
		or NaN ([b]N[/b]ot [b]a[/b] [b]N[/b]umber). NaN is returned by several
		math functions when the result would be undefined.
*/

static bool math_kvs_fnc_isnan(KviKvsModuleFunctionCall * c)
{
	kvs_real_t dReal;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("value", KVS_PT_REAL, 0, dReal)
	KVSM_PARAMETERS_END(c)
	c->returnValue()->setBoolean(std::isnan(dReal));
	return true;
}

/*
	@doc: math.isinf
	@type:
		function
	@title:
		$math.isinf
	@short:
		Checks if the value is the infinity
	@syntax:
		<boolean> $math.isinf(<value:real>)
	@description:
		Checks if the specified value is the infinity.
*/

static bool math_kvs_fnc_isinf(KviKvsModuleFunctionCall * c)
{
	kvs_real_t dReal;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("value", KVS_PT_REAL, 0, dReal)
	KVSM_PARAMETERS_END(c)
	c->returnValue()->setBoolean(std::isinf(dReal));
	return true;
}

/*
	@doc: math.pow
	@type:
		function
	@title:
		$math.pow
	@short:
		Returns a to the power of b
	@syntax:
		<real> $math.pow(<a:real>,<b:real>)
	@description:
		Returns a to the power of b
*/

static bool math_kvs_fnc_pow(KviKvsModuleFunctionCall * c)
{
	kvs_real_t dA, dB;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("a", KVS_PT_REAL, 0, dA)
	KVSM_PARAMETER("b", KVS_PT_REAL, 0, dB)
	KVSM_PARAMETERS_END(c)
	c->returnValue()->setReal(pow(dA, dB));
	return true;
}

/*
	@doc: math.pi
	@type:
		function
	@title:
		$math.pi
	@short:
		Returns the value of pi
	@syntax:
		<real> $math.pi()
	@description:
		Returns the value of pi.
*/

#ifdef COMPILE_ON_WINDOWS
static bool math_kvs_fnc_pi(KviKvsModuleFunctionCall * c)
{
	c->returnValue()->setReal(3.141592653589793238462643383279502884197169399375);
	return true;
}
#else
static bool math_kvs_fnc_pi(KviKvsModuleFunctionCall * c)
{
	c->returnValue()->setReal(M_PI);
	return true;
}
#endif

/*
	@doc: math.e
	@type:
		function
	@title:
		$math.e
	@short:
		Returns the value of e
	@syntax:
		<real> $math.e()
	@description:
		Returns the value of the Euler constant e.
*/

#ifdef COMPILE_ON_WINDOWS
static bool math_kvs_fnc_e(KviKvsModuleFunctionCall * c)
{
	c->returnValue()->setReal(1.56903485);
	return true;
}
#else
static bool math_kvs_fnc_e(KviKvsModuleFunctionCall * c)
{
	c->returnValue()->setReal(M_E);
	return true;
}
#endif

static bool math_module_init(KviModule * m)
{
	KVSM_REGISTER_FUNCTION(m, "sin", math_kvs_fnc_sin);
	KVSM_REGISTER_FUNCTION(m, "cos", math_kvs_fnc_cos);
	KVSM_REGISTER_FUNCTION(m, "tan", math_kvs_fnc_tan);
	KVSM_REGISTER_FUNCTION(m, "asin", math_kvs_fnc_asin);
	KVSM_REGISTER_FUNCTION(m, "acos", math_kvs_fnc_acos);
	KVSM_REGISTER_FUNCTION(m, "atan", math_kvs_fnc_atan);
	KVSM_REGISTER_FUNCTION(m, "sqrt", math_kvs_fnc_sqrt);
	KVSM_REGISTER_FUNCTION(m, "cbrt", math_kvs_fnc_cbrt);
	KVSM_REGISTER_FUNCTION(m, "abs", math_kvs_fnc_abs);
	KVSM_REGISTER_FUNCTION(m, "floor", math_kvs_fnc_floor);
	KVSM_REGISTER_FUNCTION(m, "ceil", math_kvs_fnc_ceil);
	KVSM_REGISTER_FUNCTION(m, "pow", math_kvs_fnc_pow);
	KVSM_REGISTER_FUNCTION(m, "exp", math_kvs_fnc_exp);
	KVSM_REGISTER_FUNCTION(m, "log", math_kvs_fnc_log);
	KVSM_REGISTER_FUNCTION(m, "log10", math_kvs_fnc_log10);
	KVSM_REGISTER_FUNCTION(m, "pi", math_kvs_fnc_pi);
	KVSM_REGISTER_FUNCTION(m, "e", math_kvs_fnc_e);
	KVSM_REGISTER_FUNCTION(m, "isnan", math_kvs_fnc_isnan);
	KVSM_REGISTER_FUNCTION(m, "isinf", math_kvs_fnc_isinf);
	return true;
}

static bool math_module_cleanup(KviModule *)
{
	return true;
}

KVIRC_MODULE(
    "Math",  // module name
    "4.0.0", // module version
    "Copyright (C) 2006 Szymon Stefanek (pragma at kvirc dot net),"
    "Tonino Imbesi (grifisx at barmes dot org),"
    "Alessandro Carbone (elfonol at gmail dot com)",
    "Mathematical function module",
    math_module_init,
    0,
    0,
    math_module_cleanup,
    0)
