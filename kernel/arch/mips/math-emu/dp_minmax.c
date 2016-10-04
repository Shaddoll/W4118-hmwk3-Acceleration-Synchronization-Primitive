/* IEEE754 floating point arithmetic
 * double precision: MIN/MAX/MINA/MAXA
 */
/*
 * MIPS floating point support
 * Copyright (C) 2015 Imagination technologies, Ltd.
 *
 * ########################################################################
 *
 *  This program is free software; you can distribute it and/or modify it
 *  under the terms of the GNU General Public License (Version 2) as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 * ########################################################################
 *
 */


#include "ieee754dp.h"

ieee754dp ieee754dp_fmin(ieee754dp x, ieee754dp y)
{
	COMPXDP;
	COMPYDP;

	EXPLODEXDP;
	EXPLODEYDP;

	CLEARCX;

	FLUSHXDP;
	FLUSHYDP;

	switch (CLPAIR(xc, yc)) {
	case CLPAIR(IEEE754_CLASS_SNAN, IEEE754_CLASS_QNAN):
	case CLPAIR(IEEE754_CLASS_QNAN, IEEE754_CLASS_SNAN):
	case CLPAIR(IEEE754_CLASS_SNAN, IEEE754_CLASS_SNAN):
	case CLPAIR(IEEE754_CLASS_ZERO, IEEE754_CLASS_SNAN):
	case CLPAIR(IEEE754_CLASS_NORM, IEEE754_CLASS_SNAN):
	case CLPAIR(IEEE754_CLASS_DNORM, IEEE754_CLASS_SNAN):
	case CLPAIR(IEEE754_CLASS_INF, IEEE754_CLASS_SNAN):
	case CLPAIR(IEEE754_CLASS_SNAN, IEEE754_CLASS_ZERO):
	case CLPAIR(IEEE754_CLASS_SNAN, IEEE754_CLASS_NORM):
	case CLPAIR(IEEE754_CLASS_SNAN, IEEE754_CLASS_DNORM):
	case CLPAIR(IEEE754_CLASS_SNAN, IEEE754_CLASS_INF):
		SETCX(IEEE754_INVALID_OPERATION);
		return ieee754dp_nanxcpt(ieee754dp_indef(), "min", x, y);

	case CLPAIR(IEEE754_CLASS_QNAN, IEEE754_CLASS_QNAN):
	case CLPAIR(IEEE754_CLASS_ZERO, IEEE754_CLASS_QNAN):
	case CLPAIR(IEEE754_CLASS_NORM, IEEE754_CLASS_QNAN):
	case CLPAIR(IEEE754_CLASS_DNORM, IEEE754_CLASS_QNAN):
	case CLPAIR(IEEE754_CLASS_INF, IEEE754_CLASS_QNAN):
		return x;

	case CLPAIR(IEEE754_CLASS_QNAN, IEEE754_CLASS_ZERO):
	case CLPAIR(IEEE754_CLASS_QNAN, IEEE754_CLASS_NORM):
	case CLPAIR(IEEE754_CLASS_QNAN, IEEE754_CLASS_DNORM):
	case CLPAIR(IEEE754_CLASS_QNAN, IEEE754_CLASS_INF):
		return y;

		/* Infinity and zero handling
		 */

	case CLPAIR(IEEE754_CLASS_INF, IEEE754_CLASS_INF):
	case CLPAIR(IEEE754_CLASS_INF, IEEE754_CLASS_ZERO):
	case CLPAIR(IEEE754_CLASS_INF, IEEE754_CLASS_NORM):
	case CLPAIR(IEEE754_CLASS_INF, IEEE754_CLASS_DNORM):
	case CLPAIR(IEEE754_CLASS_NORM, IEEE754_CLASS_ZERO):
	case CLPAIR(IEEE754_CLASS_DNORM, IEEE754_CLASS_ZERO):
		if (xs)
			return x;
		return y;

	case CLPAIR(IEEE754_CLASS_NORM, IEEE754_CLASS_INF):
	case CLPAIR(IEEE754_CLASS_ZERO, IEEE754_CLASS_INF):
	case CLPAIR(IEEE754_CLASS_DNORM, IEEE754_CLASS_INF):
	case CLPAIR(IEEE754_CLASS_ZERO, IEEE754_CLASS_NORM):
	case CLPAIR(IEEE754_CLASS_ZERO, IEEE754_CLASS_DNORM):
		if (ys)
			return y;
		return x;

	case CLPAIR(IEEE754_CLASS_ZERO, IEEE754_CLASS_ZERO):
		if (xs == ys)
			return x;
		return ieee754dp_zero(1);

	case CLPAIR(IEEE754_CLASS_DNORM, IEEE754_CLASS_DNORM):
		DPDNORMX;

		/* FALL THROUGH */

	case CLPAIR(IEEE754_CLASS_NORM, IEEE754_CLASS_DNORM):
		DPDNORMY;
		break;

	case CLPAIR(IEEE754_CLASS_DNORM, IEEE754_CLASS_NORM):
		DPDNORMX;
		break;

	case CLPAIR(IEEE754_CLASS_NORM, IEEE754_CLASS_NORM):
		break;
	}
	assert(xm & DP_HIDDEN_BIT);
	assert(ym & DP_HIDDEN_BIT);

	if (xs ^ ys) {
		if (xs)
			return x;
		return y;
	}

	if (xe > ye)
		return y;
	if (ye > xe)
		return x;

	assert(xe == ye);
	assert(xe <= DP_EMAX);

	if (xm <= ym)
		return x;
	return y;
}

ieee754dp ieee754dp_fmax(ieee754dp x, ieee754dp y)
{
	COMPXDP;
	COMPYDP;

	EXPLODEXDP;
	EXPLODEYDP;

	CLEARCX;

	FLUSHXDP;
	FLUSHYDP;

	switch (CLPAIR(xc, yc)) {
	case CLPAIR(IEEE754_CLASS_SNAN, IEEE754_CLASS_QNAN):
	case CLPAIR(IEEE754_CLASS_QNAN, IEEE754_CLASS_SNAN):
	case CLPAIR(IEEE754_CLASS_SNAN, IEEE754_CLASS_SNAN):
	case CLPAIR(IEEE754_CLASS_ZERO, IEEE754_CLASS_SNAN):
	case CLPAIR(IEEE754_CLASS_NORM, IEEE754_CLASS_SNAN):
	case CLPAIR(IEEE754_CLASS_DNORM, IEEE754_CLASS_SNAN):
	case CLPAIR(IEEE754_CLASS_INF, IEEE754_CLASS_SNAN):
	case CLPAIR(IEEE754_CLASS_SNAN, IEEE754_CLASS_ZERO):
	case CLPAIR(IEEE754_CLASS_SNAN, IEEE754_CLASS_NORM):
	case CLPAIR(IEEE754_CLASS_SNAN, IEEE754_CLASS_DNORM):
	case CLPAIR(IEEE754_CLASS_SNAN, IEEE754_CLASS_INF):
		SETCX(IEEE754_INVALID_OPERATION);
		return ieee754dp_nanxcpt(ieee754dp_indef(), "max", x, y);

	case CLPAIR(IEEE754_CLASS_QNAN, IEEE754_CLASS_QNAN):
	case CLPAIR(IEEE754_CLASS_ZERO, IEEE754_CLASS_QNAN):
	case CLPAIR(IEEE754_CLASS_NORM, IEEE754_CLASS_QNAN):
	case CLPAIR(IEEE754_CLASS_DNORM, IEEE754_CLASS_QNAN):
	case CLPAIR(IEEE754_CLASS_INF, IEEE754_CLASS_QNAN):
		return x;

	case CLPAIR(IEEE754_CLASS_QNAN, IEEE754_CLASS_ZERO):
	case CLPAIR(IEEE754_CLASS_QNAN, IEEE754_CLASS_NORM):
	case CLPAIR(IEEE754_CLASS_QNAN, IEEE754_CLASS_DNORM):
	case CLPAIR(IEEE754_CLASS_QNAN, IEEE754_CLASS_INF):
		return y;

		/* Infinity and zero handling
		 */

	case CLPAIR(IEEE754_CLASS_INF, IEEE754_CLASS_INF):
	case CLPAIR(IEEE754_CLASS_INF, IEEE754_CLASS_ZERO):
	case CLPAIR(IEEE754_CLASS_INF, IEEE754_CLASS_NORM):
	case CLPAIR(IEEE754_CLASS_INF, IEEE754_CLASS_DNORM):
	case CLPAIR(IEEE754_CLASS_NORM, IEEE754_CLASS_ZERO):
	case CLPAIR(IEEE754_CLASS_DNORM, IEEE754_CLASS_ZERO):
		if (!xs)
			return x;
		return y;

	case CLPAIR(IEEE754_CLASS_NORM, IEEE754_CLASS_INF):
	case CLPAIR(IEEE754_CLASS_ZERO, IEEE754_CLASS_INF):
	case CLPAIR(IEEE754_CLASS_DNORM, IEEE754_CLASS_INF):
	case CLPAIR(IEEE754_CLASS_ZERO, IEEE754_CLASS_NORM):
	case CLPAIR(IEEE754_CLASS_ZERO, IEEE754_CLASS_DNORM):
		if (!ys)
			return y;
		return x;

	case CLPAIR(IEEE754_CLASS_ZERO, IEEE754_CLASS_ZERO):
		if (xs == ys)
			return x;
		return ieee754dp_zero(0);

	case CLPAIR(IEEE754_CLASS_DNORM, IEEE754_CLASS_DNORM):
		DPDNORMX;

		/* FALL THROUGH */

	case CLPAIR(IEEE754_CLASS_NORM, IEEE754_CLASS_DNORM):
		DPDNORMY;
		break;

	case CLPAIR(IEEE754_CLASS_DNORM, IEEE754_CLASS_NORM):
		DPDNORMX;
		break;

	case CLPAIR(IEEE754_CLASS_NORM, IEEE754_CLASS_NORM):
		break;
	}
	assert(xm & DP_HIDDEN_BIT);
	assert(ym & DP_HIDDEN_BIT);

	if (xs ^ ys) {
		if (!xs)
			return x;
		return y;
	}

	if (xe > ye)
		return x;
	if (ye > xe)
		return y;

	assert(xe == ye);
	assert(xe <= DP_EMAX);

	if (xm < ym)
		return y;
	return x;
}

ieee754dp ieee754dp_fmina(ieee754dp x, ieee754dp y)
{
	COMPXDP;
	COMPYDP;

	EXPLODEXDP;
	EXPLODEYDP;

	CLEARCX;

	FLUSHXDP;
	FLUSHYDP;

	switch (CLPAIR(xc, yc)) {
	case CLPAIR(IEEE754_CLASS_SNAN, IEEE754_CLASS_QNAN):
	case CLPAIR(IEEE754_CLASS_QNAN, IEEE754_CLASS_SNAN):
	case CLPAIR(IEEE754_CLASS_SNAN, IEEE754_CLASS_SNAN):
	case CLPAIR(IEEE754_CLASS_ZERO, IEEE754_CLASS_SNAN):
	case CLPAIR(IEEE754_CLASS_NORM, IEEE754_CLASS_SNAN):
	case CLPAIR(IEEE754_CLASS_DNORM, IEEE754_CLASS_SNAN):
	case CLPAIR(IEEE754_CLASS_INF, IEEE754_CLASS_SNAN):
	case CLPAIR(IEEE754_CLASS_SNAN, IEEE754_CLASS_ZERO):
	case CLPAIR(IEEE754_CLASS_SNAN, IEEE754_CLASS_NORM):
	case CLPAIR(IEEE754_CLASS_SNAN, IEEE754_CLASS_DNORM):
	case CLPAIR(IEEE754_CLASS_SNAN, IEEE754_CLASS_INF):
		SETCX(IEEE754_INVALID_OPERATION);
		return ieee754dp_nanxcpt(ieee754dp_indef(), "mina", x, y);

	case CLPAIR(IEEE754_CLASS_QNAN, IEEE754_CLASS_QNAN):
	case CLPAIR(IEEE754_CLASS_ZERO, IEEE754_CLASS_QNAN):
	case CLPAIR(IEEE754_CLASS_NORM, IEEE754_CLASS_QNAN):
	case CLPAIR(IEEE754_CLASS_DNORM, IEEE754_CLASS_QNAN):
	case CLPAIR(IEEE754_CLASS_INF, IEEE754_CLASS_QNAN):
		return x;

	case CLPAIR(IEEE754_CLASS_QNAN, IEEE754_CLASS_ZERO):
	case CLPAIR(IEEE754_CLASS_QNAN, IEEE754_CLASS_NORM):
	case CLPAIR(IEEE754_CLASS_QNAN, IEEE754_CLASS_DNORM):
	case CLPAIR(IEEE754_CLASS_QNAN, IEEE754_CLASS_INF):
		return y;

		/* Infinity and zero handling
		 */

	case CLPAIR(IEEE754_CLASS_INF, IEEE754_CLASS_INF):
	case CLPAIR(IEEE754_CLASS_INF, IEEE754_CLASS_ZERO):
	case CLPAIR(IEEE754_CLASS_INF, IEEE754_CLASS_NORM):
	case CLPAIR(IEEE754_CLASS_INF, IEEE754_CLASS_DNORM):
	case CLPAIR(IEEE754_CLASS_NORM, IEEE754_CLASS_ZERO):
	case CLPAIR(IEEE754_CLASS_DNORM, IEEE754_CLASS_ZERO):
		return y;

	case CLPAIR(IEEE754_CLASS_NORM, IEEE754_CLASS_INF):
	case CLPAIR(IEEE754_CLASS_ZERO, IEEE754_CLASS_INF):
	case CLPAIR(IEEE754_CLASS_DNORM, IEEE754_CLASS_INF):
	case CLPAIR(IEEE754_CLASS_ZERO, IEEE754_CLASS_NORM):
	case CLPAIR(IEEE754_CLASS_ZERO, IEEE754_CLASS_DNORM):
		return x;

	case CLPAIR(IEEE754_CLASS_ZERO, IEEE754_CLASS_ZERO):
		if (xs == ys)
			return x;
		return ieee754dp_zero(1);

	case CLPAIR(IEEE754_CLASS_DNORM, IEEE754_CLASS_DNORM):
		DPDNORMX;

		/* FALL THROUGH */

	case CLPAIR(IEEE754_CLASS_NORM, IEEE754_CLASS_DNORM):
		DPDNORMY;
		break;

	case CLPAIR(IEEE754_CLASS_DNORM, IEEE754_CLASS_NORM):
		DPDNORMX;
		break;

	case CLPAIR(IEEE754_CLASS_NORM, IEEE754_CLASS_NORM):
		break;
	}
	assert(xm & DP_HIDDEN_BIT);
	assert(ym & DP_HIDDEN_BIT);

	if (xe > ye)
		return y;
	if (ye > xe)
		return x;

	assert(xe == ye);
	assert(xe <= DP_EMAX);

	if (xm <= ym)
		return x;
	return y;
}

ieee754dp ieee754dp_fmaxa(ieee754dp x, ieee754dp y)
{
	COMPXDP;
	COMPYDP;

	EXPLODEXDP;
	EXPLODEYDP;

	CLEARCX;

	FLUSHXDP;
	FLUSHYDP;

	switch (CLPAIR(xc, yc)) {
	case CLPAIR(IEEE754_CLASS_SNAN, IEEE754_CLASS_QNAN):
	case CLPAIR(IEEE754_CLASS_QNAN, IEEE754_CLASS_SNAN):
	case CLPAIR(IEEE754_CLASS_SNAN, IEEE754_CLASS_SNAN):
	case CLPAIR(IEEE754_CLASS_ZERO, IEEE754_CLASS_SNAN):
	case CLPAIR(IEEE754_CLASS_NORM, IEEE754_CLASS_SNAN):
	case CLPAIR(IEEE754_CLASS_DNORM, IEEE754_CLASS_SNAN):
	case CLPAIR(IEEE754_CLASS_INF, IEEE754_CLASS_SNAN):
	case CLPAIR(IEEE754_CLASS_SNAN, IEEE754_CLASS_ZERO):
	case CLPAIR(IEEE754_CLASS_SNAN, IEEE754_CLASS_NORM):
	case CLPAIR(IEEE754_CLASS_SNAN, IEEE754_CLASS_DNORM):
	case CLPAIR(IEEE754_CLASS_SNAN, IEEE754_CLASS_INF):
		SETCX(IEEE754_INVALID_OPERATION);
		return ieee754dp_nanxcpt(ieee754dp_indef(), "maxa", x, y);

	case CLPAIR(IEEE754_CLASS_QNAN, IEEE754_CLASS_QNAN):
	case CLPAIR(IEEE754_CLASS_ZERO, IEEE754_CLASS_QNAN):
	case CLPAIR(IEEE754_CLASS_NORM, IEEE754_CLASS_QNAN):
	case CLPAIR(IEEE754_CLASS_DNORM, IEEE754_CLASS_QNAN):
	case CLPAIR(IEEE754_CLASS_INF, IEEE754_CLASS_QNAN):
		return x;

	case CLPAIR(IEEE754_CLASS_QNAN, IEEE754_CLASS_ZERO):
	case CLPAIR(IEEE754_CLASS_QNAN, IEEE754_CLASS_NORM):
	case CLPAIR(IEEE754_CLASS_QNAN, IEEE754_CLASS_DNORM):
	case CLPAIR(IEEE754_CLASS_QNAN, IEEE754_CLASS_INF):
		return y;

		/* Infinity and zero handling
		 */

	case CLPAIR(IEEE754_CLASS_INF, IEEE754_CLASS_INF):
	case CLPAIR(IEEE754_CLASS_INF, IEEE754_CLASS_ZERO):
	case CLPAIR(IEEE754_CLASS_INF, IEEE754_CLASS_NORM):
	case CLPAIR(IEEE754_CLASS_INF, IEEE754_CLASS_DNORM):
	case CLPAIR(IEEE754_CLASS_NORM, IEEE754_CLASS_ZERO):
	case CLPAIR(IEEE754_CLASS_DNORM, IEEE754_CLASS_ZERO):
		return x;

	case CLPAIR(IEEE754_CLASS_NORM, IEEE754_CLASS_INF):
	case CLPAIR(IEEE754_CLASS_ZERO, IEEE754_CLASS_INF):
	case CLPAIR(IEEE754_CLASS_DNORM, IEEE754_CLASS_INF):
	case CLPAIR(IEEE754_CLASS_ZERO, IEEE754_CLASS_NORM):
	case CLPAIR(IEEE754_CLASS_ZERO, IEEE754_CLASS_DNORM):
		return y;

	case CLPAIR(IEEE754_CLASS_ZERO, IEEE754_CLASS_ZERO):
		if (xs == ys)
			return x;
		return ieee754dp_zero(0);

	case CLPAIR(IEEE754_CLASS_DNORM, IEEE754_CLASS_DNORM):
		DPDNORMX;

		/* FALL THROUGH */

	case CLPAIR(IEEE754_CLASS_NORM, IEEE754_CLASS_DNORM):
		DPDNORMY;
		break;

	case CLPAIR(IEEE754_CLASS_DNORM, IEEE754_CLASS_NORM):
		DPDNORMX;
		break;

	case CLPAIR(IEEE754_CLASS_NORM, IEEE754_CLASS_NORM):
		break;
	}
	assert(xm & DP_HIDDEN_BIT);
	assert(ym & DP_HIDDEN_BIT);

	if (xe > ye)
		return x;
	if (ye > xe)
		return y;

	assert(xe == ye);
	assert(xe <= DP_EMAX);

	if (xm < ym)
		return y;
	return x;
}
