	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		nXng Software Rasterizer implementation file
		Optimised to render in 8888 RGB mode
		FPU SUB-PIXEL/SUB-TEXEL accurate!

		FPU is assumed to be in round UPPER mode!

	----------------------------------------------------------------------------*/


	#include	"nXng/nX_HeaderList.h"
	#include	"nXng/piXel.h"
	#include	"nXng/nX_piXel_Variables.h"

	extern unsigned char	mulshader[65536];


#define	PIXEL_SUB_TEXEL


long inline	f2l(float	src)
{
	long	dst;
	_asm
	{
		fld		[src]
		fistp	[dst]
	}
	return	dst;
}

float inline qceil(const float input)
{
	float	retCode;
	_asm
	{
		fld		[input]
		frndint				// upround st(0)
		fsub	[input]		// - input
		fstp	[retCode]	// qceil
	}
	return	retCode;
}


/*
	nX::nPrecapol
	polygon 32bit W-buffered preca
*/

void	nX::nPrecaPol(nX_face *pface, long *npoly, float *wbuffer, bool *trans)
{

	long		c, c2, nedge, id;

	// scanline vars

	long		xext, xint;
	float		fpentexint, fpentexext;
	float		fxint, fxdint, fyint, fyext, fxext, fxdext;
	float		fzint, fzdint, fzext, fzdext;
	long		ybrkext, ybrkint;
	long		ymax, ymin;
	long		*pscreen;
	float		*pwbuffer;
	bool		transparent;
	nX_Surface	*surface;
	nX_vertex	*pvertex;

	// polygon datas & boundaries

	nedge = pface->nedge;
	surface = pface->surf;
	pvertex = pface->vertice;
	id = pface->id;
	nX_vertex	*lbound = pvertex;
	nX_vertex	*hbound = pvertex + nedge;

	// treat transparency

	if (pface->alpha >= 1)
		return;

	if (surface->CAPS & nX_ADDITIVE)
		transparent = true;
	else if (surface->xtdCAPS & nX_MERGEDALPHA)
		transparent = true;
	else if (surface->CAPS & nX_CHROMA)
		transparent = true;
	else if (pface->alpha > 0)
		transparent = true;
	else if (surface->CAPS & nX_ALPHATEX)
		transparent = true;
	else
		transparent = false;

	// find Ymin & Ymax

	pext = pint = pvertex;
	ymin = ymax = f2l(pint->y);
	
	for (c = 0; c < (nedge - 1); c++)
	{
		pint++;
		if (ymin > f2l(pint->y))
		{	ymin = f2l(pint->y); pext=pint;	}
		if (ymax < f2l(pint->y))
			ymax = f2l(pint->y);
	}

	// begin with inits

	pint = pext;
	ybrkint = ybrkext = ymin;
	tmpcalc = ymin * screen->width;
	
	// rasterization

	for (c = ymin; c < ymax; c++)
	{
		if (c == ybrkint)
		{
			xintwasbroken:;
			fxint = pint->x;
			fyint = pint->y;
			fzint = 1. / pint->z;
			pint--;
			if (pint < lbound)
				pint = pvertex + (nedge - 1);

			if (f2l(pint->y) < c)
				return;
			if (f2l(pint->y) == ybrkint)
				goto xintwasbroken;

			fxdint = pint->x;
			ybrkint = f2l(pint->y);
			fzdint = 1. / pint->z;
			fpentexint = (fxdint - fxint) / (pint->y - fyint);
			fxint += fpentexint * qceil(fyint);					// sub-pixel X
			fpentezint = (fzdint - fzint) / (pint->y - fyint);
			fzint += fpentezint * qceil(fyint);					// sub-pixel Z
		}

		if (c == ybrkext)
		{
			xextwasbroken:;
			fxext = pext->x;
			fyext = pext->y;
			fzext = 1. / pext->z;
			pext++;
			if (pext >= hbound)
				pext = pvertex;

			if (f2l(pext->y) < c)
				return;
			if (f2l(pext->y) == ybrkext)
				goto xextwasbroken;

			fxdext = pext->x;
			ybrkext = f2l(pext->y);
			fzdext = 1. / pext->z;
			fpentexext = (fxdext - fxext) / (pext->y - fyext);
			fxext += fpentexext * qceil(fyext);					// sub-pixel X
			fpentezext = (fzdext - fzext) / (pext->y - fyext);
			fzext += fpentezext * qceil(fyext);					// sub-pixel Z
		}

		xint = f2l(fxint);
		xext = f2l(fxext);
		scansize = xext - xint;
		if (scansize < 1)
			goto noscan;

		fscansize = fxext - fxint + 1;
		fzpente = (fzext - fzint) / fscansize;
		fscanz = fzint + fzpente * qceil(fxint);				// sub-texel Z
		pscreen = npoly + tmpcalc + xint;
		pwbuffer = wbuffer + tmpcalc + xint;

		for (c2 = 0; c2 < scansize; c2++)
		{
			if (fscanz > pwbuffer[0])
				if (transparent)
				{
					trans[id] = true;
					return;
				}
				else
				{
					pwbuffer[0] = fscanz;
					pscreen[0] = id;
				}
					
			fscanz += fzpente;
			pscreen++;
			pwbuffer++;
		}

		noscan:;
	    fxint += fpentexint;
		fxext += fpentexext;
		fzint += fpentezint;
		fzext += fpentezext;
		tmpcalc += screen->width;
	}
}
