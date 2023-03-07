	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		nXng Software Rasterizer (Wide) implementation file
		Optimised to render in 8888 RGB mode
		FPU SUB-PIXEL/SUB-TEXEL accurate!

		FPU is assumed to be in round UPPER mode!

	----------------------------------------------------------------------------*/


	#pragma warning( disable : 4244) 

	#include	"nXng/nX_HeaderList.h"
	#include	"nXng/piXel.h"
	#include	"nXng/nX_piXel_Variables.h"

	extern unsigned char	mulshader[65536];


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


void nX::WIDE_nFlatAlpTex(nX_face *pface, nXcolor *color, nX_TexMap *alpmap, ScreenMN *screen)
{

	long	c, c2, nedge;

	// scanline vars

	long	xext, xint;
	float	fpentexint, fpentexext;
	float	fxint, fxdint, fyint, fyext, fxext, fxdext;
	long	ybrkext, ybrkint;
	long	ymax, ymin;

	// alpha shader

	prshade = &mulshader[color->r * 256];
	pgshade = &mulshader[color->g * 256];
	pbshade = &mulshader[color->b * 256];

	// polygon datas & boundaries

	uchar	*pscreen;
	uchar	*alpadr = alpmap->alpha;
	uchar	*ialpadr = alpmap->ialpha;
	ylkup = alpmap->widey;

	nedge = pface->nedge;
	nX_vertex	*pvertex = pface->vertice;
	nX_vertex	*lbound = pvertex;
	nX_vertex	*hbound = pvertex + nedge;

	long	mapoff;

	// find Ymin & Ymax
	
	pext = pint = pvertex;
	ymin = ymax = f2l(pint->y);
	
	for (c = 0; c < (nedge - 1); c++)
	{
		pint++;
		long	y = f2l(pint->y);
		if (ymin > y)
		{	ymin = y; pext = pint;	}
		if (ymax < y)
			ymax = y;
	}

	// rasterization

	pint = pext;
	ybrkint = ybrkext = ymin;
	tmpcalc = ymin * screen->pitch;

	for (c = ymin; c < ymax; c++)
	{
		if (c == ybrkint)
		{
			xintwasbroken:;
			fxint = pint->x;
			fyint = pint->y;
			fuint = pint->u[3] * (alpmap->width-0.1);
			fvint = pint->v[3] * (alpmap->height-0.1);
			pint--;
			if (pint < lbound)
				pint = pvertex + (nedge - 1);

			if (f2l(pint->y) < c)
				return;
			if (f2l(pint->y) == ybrkint)
				goto xintwasbroken;

			fxdint = pint->x;
			ybrkint = f2l(pint->y);
			fudint = pint->u[3] * (alpmap->width-0.1);
			fvdint = pint->v[3] * (alpmap->height-0.1);

			fpentexint=(fxdint-fxint)/(pint->y-fyint);
			fxint+=fpentexint*qceil(fyint);		// sub-pixel X
			fpenteuint=(fudint - fuint)/(pint->y-fyint);
			fuint+=fpenteuint*qceil(fyint);	// sub-pixel U
			fpentevint=(fvdint-fvint)/(pint->y-fyint);
			fvint+=fpentevint*qceil(fyint);	// sub-pixel V
		}

		if (c==ybrkext)
		{
			xextwasbroken:;
			fxext = pext->x;
			fyext = pext->y;
			fuext = pext->u[3] * (alpmap->width-0.1);
			fvext = pext->v[3] * (alpmap->height-0.1);

			pext++;
			if (pext >= hbound)
				pext = pvertex;

			if (f2l(pext->y) < c)
				return;
			if (f2l(pext->y) == ybrkext)
				goto xextwasbroken;

			fxdext = pext->x;
			ybrkext = f2l(pext->y);
			fudext = pext->u[3] * (alpmap->width-0.1);
			fvdext = pext->v[3] * (alpmap->height-0.1);

			fpentexext = (fxdext - fxext) / (pext->y - fyext);
			fxext += fpentexext * qceil(fyext);		// sub-pixel X
			fpenteuext = (fudext - fuext) / (pext->y - fyext);
			fuext += fpenteuext * qceil(fyext);		// sub-pixel U
			fpentevext = (fvdext - fvext) / (pext->y - fyext);
			fvext += fpentevext * qceil(fyext);		// sub-pixel V
		}

		xint = f2l(fxint);
		xext = f2l(fxext);
		scansize = xext - xint;
		if (scansize <= 0)
			goto noscan;

		fscansize = fxext - fxint + 1;
		fupente = (fuext - fuint) / fscansize;
		uint = f2l((fuint + fupente * qceil(fxint)) * 65536);		// sub-texel U
		upente = f2l(fupente * 65536);
		fvpente = (fvext - fvint) / fscansize;
		vint = f2l((fvint + fvpente * qceil(fxint)) * 65536);		// sub-texel V
		vpente = f2l(fvpente * 65536);

		pscreen = (uchar *)screen->adress + (tmpcalc + xint) * 4;

		for (c2 = 0 ; c2 < scansize; c2++)
		{
			mapoff = ylkup[vint >> 16] + (uint >> 16);
			pscreen[0] = prshade[ialpadr[mapoff + 0]] + mulshader[pscreen[0] * 256 + alpadr[mapoff + 0]];
			pscreen[1] = pgshade[ialpadr[mapoff + 1]] + mulshader[pscreen[1] * 256 + alpadr[mapoff + 1]];
			pscreen[2] = pbshade[ialpadr[mapoff + 2]] + mulshader[pscreen[2] * 256 + alpadr[mapoff + 2]];
			pscreen += 4;
			uint += upente;
			vint += vpente;
		}
		noscan:;
	    fxint+=fpentexint;
		fxext+=fpentexext;
		fuint+=fpenteuint;
		fuext+=fpenteuext;
		fvint+=fpentevint;
		fvext+=fpentevext;
		tmpcalc+=screen->pitch;
	}
}


/*

	WIDE Perspective mapping: polyadr, cmap, screen

*/


void nX::WIDE_nPmap(nX_face *pface, nX_TexMap *map, ScreenMN *screen, long texlevel)
{
	long	c, c2, nedge;

	// scanline vars

	long	xext, xint;
	long	block;
	float	fpentexint, fpentexext;
	float	fxint, fxdint, fyint, fyext, fxext, fxdext;
	float	fzint, fzdint, fzext, fzdext;
	long	ybrkext, ybrkint;
	long	ymax, ymin;

	// polygon datas & boundaries

	mapadr = map->adress;
	nedge = pface->nedge;
	ylkup = map->widey;
	nX_vertex	*pvertex = pface->vertice;
	nX_vertex	*lbound = pvertex;
	nX_vertex	*hbound = pvertex + nedge;

	// find Ymin & Ymax

	pext = pint = pvertex;
	ymin = ymax = f2l(pint->y);
	
	for (c = 0; c < (nedge - 1); c++)
	{
		pint++;
		long y = f2l(pint->y);
		if (ymin > y)
		{	ymin = y; pext = pint;	}
		if (ymax < y)
			ymax = y;
	}

	// inits

	pint = pext;
	ybrkint = ybrkext = ymin;
	tmpcalc = ymin * screen->pitch;
	
	// rasterization

	for (c = ymin; c < ymax; c++)
	{
		if (c == ybrkint)
		{
			xintwasbroken:;
			fxint = pint->x;
			fyint = pint->y;
			fzint = 1 / pint->z;
			fuint = pint->u[texlevel] * (map->width-0.1) * fzint;
			fvint = pint->v[texlevel] * (map->height-0.1) * fzint;

			pint--;
			if (pint < lbound)
				pint=pvertex+(nedge-1);

			if (f2l(pint->y)<c)
				return;
			if (f2l(pint->y) == ybrkint)
				goto xintwasbroken;

			fxdint=pint->x;
			ybrkint=f2l(pint->y);
			fzdint=1 / pint->z;
			fudint=pint->u[texlevel] * (map->width-0.1) * fzdint;
			fvdint=pint->v[texlevel] * (map->height-0.1) * fzdint;

			fpentexint=(fxdint-fxint)/(pint->y-fyint);
			fxint+=fpentexint*qceil(fyint);	// sub-pixel X
			fpentezint=(fzdint-fzint)/(pint->y-fyint);
			fzint+=fpentezint*qceil(fyint);	// sub-pixel Z
			fpenteuint=(fudint-fuint)/(pint->y-fyint);
			fuint+=fpenteuint*qceil(fyint);	// sub-pixel U
			fpentevint=(fvdint-fvint)/(pint->y-fyint);
			fvint+=fpentevint*qceil(fyint);	// sub-pixel V
		}

		if (c==ybrkext)
		{
			xextwasbroken:;
			fxext=pext->x;
			fyext=pext->y;
			fzext=1 / pext->z;
			fuext=pext->u[texlevel] * (map->width-0.1) * fzext;
			fvext=pext->v[texlevel] * (map->height-0.1) * fzext;

			pext++;
			if (pext>=hbound)
				pext=pvertex;

			if (f2l(pext->y)<c)
				return;
			if (f2l(pext->y) == ybrkext)
				goto xextwasbroken;

			fxdext=pext->x;
			ybrkext=f2l(pext->y);
			fzdext=1 / pext->z;
			fudext=pext->u[texlevel] * (map->width-0.1) * fzdext;
			fvdext=pext->v[texlevel] * (map->height-0.1) * fzdext;

			fpentexext=(fxdext-fxext)/(pext->y-fyext);
			fxext+=fpentexext*qceil(fyext);	// sub-pixel X
			fpentezext=(fzdext-fzext)/(pext->y-fyext);
			fzext+=fpentezext*qceil(fyext);	// sub-pixel Z
			fpenteuext=(fudext-fuext)/(pext->y-fyext);
			fuext+=fpenteuext*qceil(fyext);	// sub-pixel U
			fpentevext=(fvdext-fvext)/(pext->y-fyext);
			fvext+=fpentevext*qceil(fyext);	// sub-pixel V
		}

		xint = f2l(fxint);
		xext = f2l(fxext);
		scansize = xext-xint;
		if (scansize < 1)
			goto noscan;

		fscansize = fxext - fxint + 1;
		fzpente = (fzext - fzint) / fscansize;
		fscanz = fzint + fzpente * qceil(fxint);	// sub-texel Z
		fupente = (fuext - fuint) / fscansize;
		fscanu = fuint + fupente * qceil(fxint);	// sub-texel U
		fvpente = (fvext - fvint) / fscansize;
		fscanv = fvint + fvpente * qceil(fxint);	// sub-texel V
		pscreen = (ulong *)screen->adress + tmpcalc + xint;


		block = scansize & 15;

		if (block > 0)
		{

			if (block < 4)
				tcoef = 1;

			tuint = fscanu;
			tvint = fscanv;
			tcoef = 1 / fscanz;

			fscanz += fzpente * block;
			fscanu += fupente * block;
			fscanv += fvpente * block;
			coef = 1 / fscanz;

			tupente = fscanu * coef; tvpente = fscanv * coef;
			tupente -= tuint * tcoef; tvpente -= tvint * tcoef;

			tupente /= block; tvpente /= block;

			uint = (ulong)(tuint * tcoef * 65536);
			vint = (ulong)(tvint * tcoef * 65536);
			upente = f2l(tupente * 65536);
			vpente = f2l(tvpente * 65536);

			for (c2 = 0; c2 < block; c2++)
			{
				*pscreen++ = mapadr[ylkup[vint >> 16] + (uint >> 16)];
				uint += upente;
				vint += vpente;
			}

		}

		// perform 16 pixels blittings...

		block = scansize / 16;

		if (block > 0)
		{

			fupente *= 16;
			fvpente *= 16;
			fzpente *= 16;						// correction 8 pixel...

			for (c2 = 0; c2 < block; c2++)
			{

				tuint = fscanu;
				tvint = fscanv;
				tcoef = 1 / fscanz;

				fscanz += fzpente;
				fscanu += fupente;
				fscanv += fvpente;
				coef = 1 / fscanz;

				tupente = fscanu * coef; tvpente = fscanv * coef;
				tupente -= tuint * tcoef; tvpente -= tvint * tcoef;
				upente = f2l(tupente * 65536 / 16);
				vpente = f2l(tvpente * 65536 / 16);
				uint = (ulong)(tuint * tcoef * 65536);
				vint = (ulong)(tvint * tcoef * 65536);

				_asm
				{

					push	ebp
					mov		eax,uint
					mov		ebp,vint
					mov		esi,mapadr
					mov		edi,pscreen
					mov		ebx,ylkup

					// 16 texels to screen

					mov		ecx,ebp
					add		ebp,vpente
					shr		ecx,16
					mov		edx,eax
					mov		ecx,[ebx + ecx * 4]		// y * width
					add		eax,upente
					shr		edx,16
					add		ecx,edx					// 1 texel
					mov		edx,[esi + ecx * 4]
					mov		[edi],edx

					mov		ecx,ebp
					add		ebp,vpente
					shr		ecx,16
					mov		edx,eax
					mov		ecx,[ebx + ecx * 4]		// y * width
					add		eax,upente
					shr		edx,16
					add		ecx,edx					// 2 texel
					mov		edx,[esi + ecx * 4]
					mov		[edi + 4],edx

					mov		ecx,ebp
					add		ebp,vpente
					shr		ecx,16
					mov		edx,eax
					mov		ecx,[ebx + ecx * 4]		// y * width
					add		eax,upente
					shr		edx,16
					add		ecx,edx					// 3 texel
					mov		edx,[esi + ecx * 4]
					mov		[edi + 8],edx

					mov		ecx,ebp
					add		ebp,vpente
					shr		ecx,16
					mov		edx,eax
					mov		ecx,[ebx + ecx * 4]		// y * width
					add		eax,upente
					shr		edx,16
					add		ecx,edx					// 4 texel
					mov		edx,[esi + ecx * 4]
					mov		[edi + 12],edx

					mov		ecx,ebp
					add		ebp,vpente
					shr		ecx,16
					mov		edx,eax
					mov		ecx,[ebx + ecx * 4]		// y * width
					add		eax,upente
					shr		edx,16
					add		ecx,edx					// 5 texel
					mov		edx,[esi + ecx * 4]
					mov		[edi + 16],edx

					mov		ecx,ebp
					add		ebp,vpente
					shr		ecx,16
					mov		edx,eax
					mov		ecx,[ebx + ecx * 4]		// y * width
					add		eax,upente
					shr		edx,16
					add		ecx,edx					// 6 texel
					mov		edx,[esi + ecx * 4]
					mov		[edi + 20],edx

					mov		ecx,ebp
					add		ebp,vpente
					shr		ecx,16
					mov		edx,eax
					mov		ecx,[ebx + ecx * 4]		// y * width
					add		eax,upente
					shr		edx,16
					add		ecx,edx					// 7 texel
					mov		edx,[esi + ecx * 4]
					mov		[edi + 24],edx

					mov		ecx,ebp
					add		ebp,vpente
					shr		ecx,16
					mov		edx,eax
					mov		ecx,[ebx + ecx * 4]		// y * width
					add		eax,upente
					shr		edx,16
					add		ecx,edx					// 8 texel
					mov		edx,[esi + ecx * 4]
					mov		[edi + 28],edx

					mov		ecx,ebp
					add		ebp,vpente
					shr		ecx,16
					mov		edx,eax
					mov		ecx,[ebx + ecx * 4]		// y * width
					add		eax,upente
					shr		edx,16
					add		ecx,edx					// 9 texel
					mov		edx,[esi + ecx * 4]
					mov		[edi + 32],edx

					mov		ecx,ebp
					add		ebp,vpente
					shr		ecx,16
					mov		edx,eax
					mov		ecx,[ebx + ecx * 4]		// y * width
					add		eax,upente
					shr		edx,16
					add		ecx,edx					// 10 texel
					mov		edx,[esi + ecx * 4]
					mov		[edi + 36],edx

					mov		ecx,ebp
					add		ebp,vpente
					shr		ecx,16
					mov		edx,eax
					mov		ecx,[ebx + ecx * 4]		// y * width
					add		eax,upente
					shr		edx,16
					add		ecx,edx					// 11 texel
					mov		edx,[esi + ecx * 4]
					mov		[edi + 40],edx

					mov		ecx,ebp
					add		ebp,vpente
					shr		ecx,16
					mov		edx,eax
					mov		ecx,[ebx + ecx * 4]		// y * width
					add		eax,upente
					shr		edx,16
					add		ecx,edx					// 12 texel
					mov		edx,[esi + ecx * 4]
					mov		[edi + 44],edx

					mov		ecx,ebp
					add		ebp,vpente
					shr		ecx,16
					mov		edx,eax
					mov		ecx,[ebx + ecx * 4]		// y * width
					add		eax,upente
					shr		edx,16
					add		ecx,edx					// 13 texel
					mov		edx,[esi + ecx * 4]
					mov		[edi + 48],edx

					mov		ecx,ebp
					add		ebp,vpente
					shr		ecx,16
					mov		edx,eax
					mov		ecx,[ebx + ecx * 4]		// y * width
					add		eax,upente
					shr		edx,16
					add		ecx,edx					// 14 texel
					mov		edx,[esi + ecx * 4]
					mov		[edi + 52],edx

					mov		ecx,ebp
					add		ebp,vpente
					shr		ecx,16
					mov		edx,eax
					mov		ecx,[ebx + ecx * 4]		// y * width
					add		eax,upente
					shr		edx,16
					add		ecx,edx					// 15 texel
					mov		edx,[esi + ecx * 4]
					mov		[edi + 56],edx

					mov		ecx,ebp
					add		ebp,vpente
					shr		ecx,16
					mov		edx,eax
					mov		ecx,[ebx + ecx * 4]		// y * width
					add		eax,upente
					shr		edx,16
					add		ecx,edx					// 16 texel
					mov		edx,[esi + ecx * 4]
					mov		[edi + 60],edx

					pop		ebp
				}

				pscreen+=16;

			}

		}

		noscan:;
	    fxint+=fpentexint;
		fxext+=fpentexext;
		fzint+=fpentezint;
		fzext+=fpentezext;
		fuint+=fpenteuint;
		fuext+=fpenteuext;
		fvint+=fpentevint;
		fvext+=fpentevext;
		tmpcalc+=screen->pitch;
	}

}


/*

	Wide linear mapping: polyadr, map, screen, scene.
	16:16 x, y sub-pixel/sub-texel
	support textures upto 65535 x 65535

*/


void nX::WIDE_nLmap(nX_face *pface, nX_TexMap *map, ScreenMN *screen, long texlevel)
{

	long	c, nedge;

	// scanline vars

	long	xext, xint;
	float	fpentexint, fpentexext;
	float	fxint, fxdint, fyint, fyext, fxext, fxdext;
	long	ybrkext, ybrkint;
	long	ymax, ymin;

	// polygon datas & boundaries

	mapadr = map->adress;
	nedge = pface->nedge;
	nX_vertex	*pvertex = pface->vertice;
	nX_vertex	*lbound = pvertex;
	nX_vertex	*hbound = pvertex + nedge;
	ylkup = map->widey;

	// find Ymin & Ymax
	
	pext = pint = pvertex;
	ymin = ymax = f2l(pint->y);
	
	for (c = 0; c < (nedge - 1); c++)
	{
		pint++;
		long	y = f2l(pint->y);
		if (ymin > y)
		{	ymin = y; pext = pint;	}
		if (ymax < y)
			ymax = y;
	}

	// rasterization

	pint = pext;
	ybrkint = ybrkext = ymin;
	tmpcalc = ymin * screen->pitch;

	for (c=ymin; c<ymax; c++)
	{
		if (c==ybrkint)
		{
			xintwasbroken:;
			fxint=pint->x;
			fyint=pint->y;
			fuint=pint->u[texlevel] * (map->width-0.1);
			fvint=pint->v[texlevel] * (map->height-0.1);
			pint--;
			if (pint < lbound)
				pint=pvertex + (nedge-1);

			if (f2l(pint->y) < c)
				return;
			if (f2l(pint->y) == ybrkint)
				goto xintwasbroken;

			fxdint=pint->x;
			ybrkint=f2l(pint->y);
			fudint=pint->u[texlevel] * (map->width-0.1);
			fvdint=pint->v[texlevel] * (map->height-0.1);

			fpentexint=(fxdint-fxint)/(pint->y-fyint);
			fxint+=fpentexint*qceil(fyint);	// sub-pixel X
			fpenteuint=(fudint-fuint)/(pint->y-fyint);
			fuint+=fpenteuint*qceil(fyint);	// sub-pixel U
			fpentevint=(fvdint-fvint)/(pint->y-fyint);
			fvint+=fpentevint*qceil(fyint);	// sub-pixel V
		}

		if (c==ybrkext)
		{
			xextwasbroken:;
			fxext=pext->x;
			fyext=pext->y;
			fuext=pext->u[texlevel] * (map->width-0.1);
			fvext=pext->v[texlevel] * (map->height-0.1);

			pext++;
			if (pext >= hbound)
				pext=pvertex;

			if (f2l(pext->y) < c)
				return;
			if (f2l(pext->y) == ybrkext)
				goto xextwasbroken;

			fxdext=pext->x;
			ybrkext=f2l(pext->y);
			fudext=pext->u[texlevel] * (map->width-0.1);
			fvdext=pext->v[texlevel] * (map->height-0.1);

			fpentexext=(fxdext-fxext)/(pext->y-fyext);
			fxext+=fpentexext*qceil(fyext);	// sub-pixel X
			fpenteuext=(fudext-fuext)/(pext->y-fyext);
			fuext+=fpenteuext*qceil(fyext);	// sub-pixel U
			fpentevext=(fvdext-fvext)/(pext->y-fyext);
			fvext+=fpentevext*qceil(fyext);	// sub-pixel V
		}

		xint=f2l(fxint);
		xext=f2l(fxext);
		scansize=xext-xint;
		if (scansize<=0)
			goto noscan;

		fscansize=fxext-fxint+1;
		fupente=(fuext-fuint)/fscansize;
		uint=(fuint+fupente*qceil(fxint))*65536;		// sub-texel U
		upente=f2l(fupente*65536);
		fvpente=(fvext-fvint)/fscansize;
		vint=(fvint+fvpente*qceil(fxint))*65536;		// sub-texel V
		vpente=f2l(fvpente*65536);

		pscreen=(ulong *)screen->adress+tmpcalc+xint;
		peoscan=pscreen+scansize;
		if (scansize == 0)
			goto noscan;

		__asm
		{
			push	ebp
			mov		esi,mapadr
			mov		edi,pscreen
			mov		edx,ylkup
			mov		ecx,uint
			mov		ebp,vint

algnloop:	mov		ebx,ecx
			mov		eax,ebp
			shr		ebx,16
			add		ebp,vpente
			shr		eax,16
			add		ebx,[edx + eax * 4]			// y * width
			add		ecx,upente
			mov		eax,[esi+ebx*4]
			add		edi,4
			mov		[edi-4],eax

			cmp		edi,peoscan
			jne	short	algnloop

			pop		ebp
		}

		noscan:;
	    fxint += fpentexint;
		fxext += fpentexext;
		fuint += fpenteuint;
		fuext += fpenteuext;
		fvint += fpentevint;
		fvext += fpentevext;
		tmpcalc += screen->pitch;
	}
}



/*

	Wide ADDITIVE linear mapping: polyadr, map, screen, scene.
	16:16 x, y sub-pixel/sub-texel
	support textures upto 65535 x 65535

*/


void nX::WIDE_nLmapAdd(nX_face *pface, nX_TexMap *map, ScreenMN *screen, long texlevel)
{

	long	c, nedge;

	// scanline vars

	long	xext, xint;
	float	fpentexint, fpentexext;
	float	fxint, fxdint, fyint, fyext, fxext, fxdext;
	long	ybrkext, ybrkint;
	long	ymax, ymin;

	// polygon datas & boundaries

	mapadr = map->adress;
	nedge = pface->nedge;
	nX_vertex	*pvertex = pface->vertice;
	nX_vertex	*lbound = pvertex;
	nX_vertex	*hbound = pvertex + nedge;
	ylkup = map->widey;

	// find Ymin & Ymax
	
	pext = pint = pvertex;
	ymin = ymax = f2l(pint->y);
	
	for (c = 0; c < (nedge - 1); c++)
	{
		pint++;
		long	y = f2l(pint->y);
		if (ymin > y)
		{	ymin = y; pext = pint;	}
		if (ymax < y)
			ymax = y;
	}

	// rasterization

	pint = pext;
	ybrkint = ybrkext = ymin;
	tmpcalc = ymin * screen->pitch;

	for (c=ymin; c<ymax; c++)
	{
		if (c==ybrkint)
		{
			xintwasbroken:;
			fxint=pint->x;
			fyint=pint->y;
			fuint=pint->u[texlevel] * (map->width-0.1);
			fvint=pint->v[texlevel] * (map->height-0.1);
			pint--;
			if (pint < lbound)
				pint=pvertex + (nedge-1);

			if (f2l(pint->y) < c)
				return;
			if (f2l(pint->y) == ybrkint)
				goto xintwasbroken;

			fxdint=pint->x;
			ybrkint=f2l(pint->y);
			fudint=pint->u[texlevel] * (map->width-0.1);
			fvdint=pint->v[texlevel] * (map->height-0.1);

			fpentexint=(fxdint-fxint)/(pint->y-fyint);
			fxint+=fpentexint*qceil(fyint);	// sub-pixel X
			fpenteuint=(fudint-fuint)/(pint->y-fyint);
			fuint+=fpenteuint*qceil(fyint);	// sub-pixel U
			fpentevint=(fvdint-fvint)/(pint->y-fyint);
			fvint+=fpentevint*qceil(fyint);	// sub-pixel V
		}

		if (c==ybrkext)
		{
			xextwasbroken:;
			fxext=pext->x;
			fyext=pext->y;
			fuext=pext->u[texlevel] * (map->width-0.1);
			fvext=pext->v[texlevel] * (map->height-0.1);

			pext++;
			if (pext >= hbound)
				pext=pvertex;

			if (f2l(pext->y) < c)
				return;
			if (f2l(pext->y) == ybrkext)
				goto xextwasbroken;

			fxdext=pext->x;
			ybrkext=f2l(pext->y);
			fudext=pext->u[texlevel] * (map->width-0.1);
			fvdext=pext->v[texlevel] * (map->height-0.1);

			fpentexext=(fxdext-fxext)/(pext->y-fyext);
			fxext+=fpentexext*qceil(fyext);	// sub-pixel X
			fpenteuext=(fudext-fuext)/(pext->y-fyext);
			fuext+=fpenteuext*qceil(fyext);	// sub-pixel U
			fpentevext=(fvdext-fvext)/(pext->y-fyext);
			fvext+=fpentevext*qceil(fyext);	// sub-pixel V
		}

		xint=f2l(fxint);
		xext=f2l(fxext);
		scansize=xext-xint;
		if (scansize<=0)
			goto noscan;

		fscansize=fxext-fxint+1;
		fupente=(fuext-fuint)/fscansize;
		uint=(fuint+fupente*qceil(fxint))*65536;		// sub-texel U
		upente=f2l(fupente*65536);
		fvpente=(fvext-fvint)/fscansize;
		vint=(fvint+fvpente*qceil(fxint))*65536;		// sub-texel V
		vpente=f2l(fvpente*65536);

		pscreen=(ulong *)screen->adress+tmpcalc+xint;
		peoscan=pscreen+scansize;
		if (scansize == 0)
			goto noscan;

		__asm
		{
			push	ebp
			mov		esi,mapadr
			mov		edi,pscreen
			mov		ecx,uint
			mov		ebp,vint

algnloop:	mov		ebx,ecx
			mov		edx,ylkup
			mov		eax,ebp
			shr		ebx,16
			add		ebp,vpente
			shr		eax,16
			add		ebx,[edx + eax * 4]			// y * width
			add		ecx,upente

			mov		al,[esi+ebx*4+2]		// texel R
			mov		dl,[esi+ebx*4+1]		// texel G
			add		al,[edi+2]
			jnc	short nclpr
			mov		eax,255

nclpr:		mov		[edi+2],al

			mov		al,[esi+ebx*4]			// texel B
			add		dl,[edi+1]
			jnc	short nclpg
			mov		edx,255

nclpg:		mov		[edi+1],dl

			add		al,[edi]
			jnc	short nclpb
			mov		eax,255

nclpb:		mov		[edi],al
			add		edi,4

			cmp		edi,peoscan
			jne	short	algnloop

			pop		ebp
		}

		noscan:;
	    fxint+=fpentexint;
		fxext+=fpentexext;
		fuint+=fpenteuint;
		fuext+=fpenteuext;
		fvint+=fpentevint;
		fvext+=fpentevext;
		tmpcalc+=screen->pitch;
	}
}
