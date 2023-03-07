	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		nXng Software Rasterizer (Generic) implementation file
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


void	nX::GENERIC_rasterizer(nX_face *pface, nX_Surface *surface, ScreenMN *screen)
{

	long	c, c2, nedge;
	uchar	*pgenscan, *wpgenscan, level;
	long	*lpgenscan;
	long	rc, gc, bc;
	uchar	*pmulshader, *ipmulshader;
	long	color;

	// retrieve fast CAPS

	long	FLAT = surface->CAPS & nX_FLAT;
	long	ADDITIVE = surface->CAPS & nX_ADDITIVE;
	long	MAPPED = surface->CAPS & nX_MAPPED;
	long	EVMAP = surface->CAPS & nX_EVMAP;
	long	BUMP = surface->CAPS & nX_BUMP;
	long	LIGHTSOURCE = surface->CAPS & nX_LIGHTSOURCE;
	long	GOURAUD = surface->CAPS & nX_GOURAUD;
	long	ALPHATEX = surface->CAPS & nX_ALPHATEX;
	long	PERSPECTIVE = surface->CAPS & nX_PERSPECTIVE;

	// scanline vars

	long	xext, xint;
	float	fpentexint, fpentexext;
	float	fxint, fxdint, fyint, fyext, fxext, fxdext;
	long	ybrkext, ybrkint;
	long	ymax, ymin;
	float	fzint, fzdint, fzext, fzdext;

	// polygon data sources & boundaries

	nX_TexMap	*map, *evmap, *bumpmap, *alpmap;
	uchar		*mapadr, *evmapadr, *alpadr, *ialpadr;
	short		*bumpadr, bumpoff;

	if (MAPPED)
	{
		map = surface->map[0];
		mapadr = (uchar *)map->adress;
	}
	if (EVMAP)
	{
		evmap = surface->map[1];
		evmapadr = (uchar *)evmap->adress;
	}
	if (BUMP)
	{
		bumpmap = surface->map[2];
		bumpadr = bumpmap->bump;
	}
	if (ALPHATEX)
	{
		alpmap = surface->map[3];
		alpadr = alpmap->alpha;
		ialpadr = alpmap->ialpha;
	}

	uchar	*pscreen;
	long	*lpscreen;
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

			if (MAPPED)
			{
				fuint = pint->u[0] * (map->width-0.1);
				fvint = pint->v[0] * (map->height-0.1);
			}
			if (EVMAP)
			{
				fu2int = pint->u[1] * (evmap->width-0.1);
				fv2int = pint->v[1] * (evmap->height-0.1);
			}
			if (BUMP)
			{
				fu3int = pint->u[2] * (bumpmap->width-0.1);
				fv3int = pint->v[2] * (bumpmap->height-0.1);
			}
			if (ALPHATEX)
			{
				fu4int = pint->u[3] * (alpmap->width-0.1);
				fv4int = pint->v[3] * (alpmap->height-0.1);
			}
			if (GOURAUD)
			{
				frint = pint->diff.r;
				fgint = pint->diff.g;
				fbint = pint->diff.b;
			}
			if (PERSPECTIVE)
				fzint = pint->z;

			pint--;
			if (pint < lbound)
				pint = pvertex + (nedge - 1);

			if (f2l(pint->y) < c)
				return;
			if (f2l(pint->y) == ybrkint)
				goto xintwasbroken;

			fxdint = pint->x;
			ybrkint = f2l(pint->y);

			if (MAPPED)
			{
				fudint = pint->u[0] * (map->width-0.1);
				fvdint = pint->v[0] * (map->height-0.1);
				fpenteuint = (fudint - fuint) / (pint->y - fyint);
				fuint += fpenteuint * qceil(fyint);					// sub-pixel U
				fpentevint = (fvdint - fvint) / (pint->y - fyint);
				fvint += fpentevint * qceil(fyint);					// sub-pixel V
			}
			if (EVMAP)
			{
				fu2dint = pint->u[1] * (evmap->width-0.1);
				fv2dint = pint->v[1] * (evmap->height-0.1);
				fpenteu2int = (fu2dint - fu2int) / (pint->y - fyint);
				fu2int += fpenteu2int * qceil(fyint);				// sub-pixel U2
				fpentev2int = (fv2dint - fv2int) / (pint->y - fyint);
				fv2int += fpentev2int * qceil(fyint);				// sub-pixel V2
			}
			if (BUMP)
			{
				fu3dint = pint->u[2] * (bumpmap->width-0.1);
				fv3dint = pint->v[2] * (bumpmap->height-0.1);
				fpenteu3int = (fu3dint - fu3int) / (pint->y - fyint);
				fu3int += fpenteu3int * qceil(fyint);				// sub-pixel U3
				fpentev3int = (fv3dint - fv3int) / (pint->y - fyint);
				fv3int += fpentev3int * qceil(fyint);				// sub-pixel V3
			}
			if (ALPHATEX)
			{
				fu4dint = pint->u[3] * (alpmap->width-0.1);
				fv4dint = pint->v[3] * (alpmap->height-0.1);
				fpenteu4int = (fu4dint - fu4int) / (pint->y - fyint);
				fu4int += fpenteu4int * qceil(fyint);				// sub-pixel U4
				fpentev4int = (fv4dint - fv4int) / (pint->y - fyint);
				fv4int += fpentev4int * qceil(fyint);				// sub-pixel V4
			}
			if (GOURAUD)
			{
				frdint = pint->diff.r;
				fgdint = pint->diff.g;
				fbdint = pint->diff.b;
				fpenterint = (frdint - frint) / (pint->y - fyint);
				frint += fpenterint * qceil(fyint);					// sub-pixel R
				fpentegint = (fgdint - fgint) / (pint->y - fyint);
				fgint += fpentegint * qceil(fyint);					// sub-pixel G
				fpentebint = (fbdint - fbint) / (pint->y - fyint);
				fbint += fpentebint * qceil(fyint);					// sub-pixel B
			}
			if (PERSPECTIVE)
			{
				fzdint = pint->z;
				fpentezint = (fzdint - fzint) / (pint->y - fyint);
				fzint += fpentezint * qceil(fyint);					// sub-pixel Z
			}

			fpentexint=(fxdint-fxint)/(pint->y-fyint);
			fxint+=fpentexint*qceil(fyint);		// sub-pixel X
		}

		if (c==ybrkext)
		{
			xextwasbroken:;

			fxext = pext->x;
			fyext = pext->y;

			if (MAPPED)
			{
				fuext = pext->u[0] * (map->width-0.1);
				fvext = pext->v[0] * (map->height-0.1);
			}
			if (EVMAP)
			{
				fu2ext = pext->u[1] * (evmap->width-0.1);
				fv2ext = pext->v[1] * (evmap->height-0.1);
			}
			if (BUMP)
			{
				fu3ext = pext->u[2] * (bumpmap->width-0.1);
				fv3ext = pext->v[2] * (bumpmap->height-0.1);
			}
			if (ALPHATEX)
			{
				fu4ext = pext->u[3] * (alpmap->width-0.1);
				fv4ext = pext->v[3] * (alpmap->height-0.1);
			}
			if (GOURAUD)
			{
				frext = pext->diff.r;
				fgext = pext->diff.g;
				fbext = pext->diff.b;
			}
			if (PERSPECTIVE)
				fzext = pext->z;

			pext++;
			if (pext >= hbound)
				pext = pvertex;

			if (f2l(pext->y) < c)
				return;
			if (f2l(pext->y) == ybrkext)
				goto xextwasbroken;

			fxdext = pext->x;
			ybrkext = f2l(pext->y);

			if (MAPPED)
			{
				fudext = pext->u[0] * (map->width-0.1);
				fvdext = pext->v[0] * (map->height-0.1);
				fpenteuext = (fudext - fuext) / (pext->y - fyext);
				fuext += fpenteuext * qceil(fyext);					// sub-pixel U
				fpentevext = (fvdext - fvext) / (pext->y - fyext);
				fvext += fpentevext * qceil(fyext);					// sub-pixel V
			}
			if (EVMAP)
			{
				fu2dext = pext->u[1] * (evmap->width-0.1);
				fv2dext = pext->v[1] * (evmap->height-0.1);
				fpenteu2ext = (fu2dext - fu2ext) / (pext->y - fyext);
				fu2ext += fpenteu2ext * qceil(fyext);				// sub-pixel U2
				fpentev2ext = (fv2dext - fv2ext) / (pext->y - fyext);
				fv2ext += fpentev2ext * qceil(fyext);				// sub-pixel V2
			}
			if (BUMP)
			{
				fu3dext = pext->u[2] * (bumpmap->width-0.1);
				fv3dext = pext->v[2] * (bumpmap->height-0.1);
				fpenteu3ext = (fu3dext - fu3ext) / (pext->y - fyext);
				fu3ext += fpenteu3ext * qceil(fyext);				// sub-pixel U3
				fpentev3ext = (fv3dext - fv3ext) / (pext->y - fyext);
				fv3ext += fpentev3ext * qceil(fyext);				// sub-pixel V3
			}
			if (ALPHATEX)
			{
				fu4dext = pext->u[3] * (alpmap->width-0.1);
				fv4dext = pext->v[3] * (alpmap->height-0.1);
				fpenteu4ext = (fu4dext - fu4ext) / (pext->y - fyext);
				fu4ext += fpenteu4ext * qceil(fyext);				// sub-pixel U4
				fpentev4ext = (fv4dext - fv4ext) / (pext->y - fyext);
				fv4ext += fpentev4ext * qceil(fyext);				// sub-pixel V4
			}
			if (GOURAUD)
			{
				frdext = pext->diff.r;
				fgdext = pext->diff.g;
				fbdext = pext->diff.b;
				fpenterext = (frdext - frext) / (pext->y - fyext);
				frext += fpenterext * qceil(fyext);					// sub-pixel R
				fpentegext = (fgdext - fgext) / (pext->y - fyext);
				fgext += fpentegext * qceil(fyext);					// sub-pixel G
				fpentebext = (fbdext - fbext) / (pext->y - fyext);
				fbext += fpentebext * qceil(fyext);					// sub-pixel B
			}
			if (PERSPECTIVE)
			{
				fzdext = pext->z;
				fpentezext = (fzdext - fzext) / (pint->y - fyint);
				fzext += fpentezext * qceil(fyint);					// sub-pixel Z
			}

			fpentexext = (fxdext - fxext) / (pext->y - fyext);
			fxext += fpentexext * qceil(fyext);		// sub-pixel X
		}

		xint = f2l(fxint);
		xext = f2l(fxext);
		scansize = xext - xint;

		if (scansize <= 0)
			goto noscan;

		fscansize = fxext - fxint + 1;

		if (MAPPED)
		{
			fupente = (fuext - fuint) / fscansize;
			uint = (fuint + fupente * qceil(fxint)) * 65536;		// sub-texel U
			upente = fupente * 65536;
			fvpente = (fvext - fvint) / fscansize;
			vint = (fvint + fvpente * qceil(fxint)) * 65536;		// sub-texel V
			vpente = fvpente * 65536;
		}
		if (PERSPECTIVE)
		{
			fzpente = (fzext - fzint) / fscansize;
			zint = (fzint + fzpente * qceil(fxint)) * 65536;		// sub-texel Z
			zpente = fzpente * 65536;
		}
		if (EVMAP)
		{
			fu2pente = (fu2ext - fu2int) / fscansize;
			u2int = (fu2int + fu2pente * qceil(fxint)) * 65536;	// sub-texel U2
			u2pente = fu2pente * 65536;
			fv2pente = (fv2ext - fv2int) / fscansize;
			v2int = (fv2int + fv2pente * qceil(fxint)) * 65536;	// sub-texel V2
			v2pente = fv2pente * 65536;
		}
		if (BUMP)
		{
			fu3pente = (fu3ext - fu3int) / fscansize;
			u3int = (fu3int + fu3pente * qceil(fxint)) * 65536;	// sub-texel U3
			u3pente = fu3pente * 65536;
			fv3pente = (fv3ext - fv3int) / fscansize;
			v3int = (fv3int + fv3pente * qceil(fxint)) * 65536;	// sub-texel V3
			v3pente = fv3pente * 65536;
		}
		if (ALPHATEX)
		{
			fu4pente = (fu4ext - fu4int) / fscansize;
			u4int = (fu4int + fu4pente * qceil(fxint)) * 65536;	// sub-texel U4
			u4pente = fu4pente * 65536;
			fv4pente = (fv4ext - fv4int) / fscansize;
			v4int = (fv4int + fv4pente * qceil(fxint)) * 65536;	// sub-texel V4
			v4pente = fv4pente * 65536;
		}
		if (GOURAUD)
		{
			frpente = (frext - frint) / fscansize;
			rint = (frint + frpente * qceil(fxint)) * 256;		// sub-texel R
			rpente = frpente * 256;
			fgpente = (fgext - fgint) / fscansize;
			gint = (fgint + fgpente * qceil(fxint)) * 256;		// sub-texel G
			gpente = fgpente * 256;
			fbpente = (fbext - fbint) / fscansize;
			bint = (fbint + fbpente * qceil(fxint)) * 256;		// sub-texel B
			bpente = fbpente * 256;
		}

		pscreen = (uchar *)screen->adress + (tmpcalc + xint) * 4;

		if (!ALPHATEX && !ADDITIVE && pfacelist->alpha == 0)
			pgenscan = pscreen;
		else
			pgenscan = (uchar *)screen->genericscanline + xint * 4;

		// SHADING PROCESS -----------------------------------------------------------

		wpgenscan = pgenscan;
		level = 0;

		if (GOURAUD)
		{
			for (c2 = 0; c2 < scansize; c2++)
			{
				wpgenscan[2] = (uchar)(rint >> 8);
				wpgenscan[1] = (uchar)(gint >> 8);
				wpgenscan[0] = (uchar)(bint >> 8);

				wpgenscan += 4;
				rint += rpente;
				gint += gpente;
				bint += bpente;
			}
			level++;
		}
		else if (LIGHTSOURCE)
		{
			for (c2 = 0; c2 < scansize; c2++)
			{
				wpgenscan[2] = pfacelist->diff.r;
				wpgenscan[1] = pfacelist->diff.g;
				wpgenscan[0] = pfacelist->diff.b;

				wpgenscan += 4;
			}
			level++;			
		}
		else
		{
			color = (surface->color.a << 24) + (surface->color.r << 16) + (surface->color.g << 8) + surface->color.b;

			for (c2 = 0; c2 < scansize; c2++)
				*(long *)(wpgenscan + c2 * 4) = color;
		}

		wpgenscan = pgenscan;

		// TEXTURE PROCESS -----------------------------------------------------------

		if (MAPPED)
		{
//			if (!PERSPECTIVE)
//			{
			if (level)
			{
				for (c2 = 0; c2 < scansize; c2++)
				{
					mapoff = ( ((vint >> 8) & 0x0ff00) + ((uint >> 16) & 255) ) * 4 ;
					wpgenscan[0] = mulshader[ wpgenscan[0] * 256 + mapadr[mapoff + 0]];
					wpgenscan[1] = mulshader[ wpgenscan[1] * 256 + mapadr[mapoff + 1]];
					wpgenscan[2] = mulshader[ wpgenscan[2] * 256 + mapadr[mapoff + 2]];

					wpgenscan += 4;
					uint += upente;
					vint += vpente;
				}
			}
			else
			{
				for (c2 = 0; c2 < scansize; c2++)
				{
					mapoff = ( ((vint >> 8) & 0x0ff00) + ((uint >> 16) & 255) ) * 4;
					*(ulong *)wpgenscan = *(ulong *)(mapadr + mapoff);

					wpgenscan += 4;
					uint += upente;
					vint += vpente;
				}
			}
//			}
/*
			else
			{
				if (level)
				{
				}
				else
				{
					block = scansize & 7;

					if (block > 0)
					{
						tuint = fscanu;
						tvint = fscanv;
						tcoef = 1 / fscanz;

						fscanz += fzpente * block;
						fscanu += fupente * block;
						fscanv += fvpente * block;
						coef = 1 / fscanz;

						tupente = fscanu * coef; tvpente = fscanv*coef;
						tupente -= tuint * tcoef; tvpente -= tvint * tcoef;

						tupente /= block; tvpente /= block;

						uint = (ulong)(tuint * tcoef * 65536);
						vint = (ulong)(tvint * tcoef * 65536);
						upente = long(tupente * 65536);
						vpente = long(tvpente * 65536);

						for (c2 = 0; c2 < block; c2++)
						{
							*(ulong *)wpgenscan = map->adress[((vint >> 8) & 0xff00) + (uint >> 16)];
							wpgenscan += 4;
							uint += upente;
							vint += vpente;
						}
					}

					// perform 8 pixels blittings...

					block = scansize / 8;
			
					if (block > 0)
					{
						fupente *= 8;
						fvpente *= 8;
						fzpente *= 8;					// correction 8 pixel...
			
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
							upente = long(tupente * 65536 / 8);
							vpente = long(tvpente * 65536 / 8);
							uint = (ulong)(tuint * tcoef * 65536);
							vint = (ulong)(tvint * tcoef * 65536);

							for (c3 = 0; c3 < 8; c3++)
							{
								mapoff = ( ((vint >> 8) & 0x0ff00) + ((uint >> 16) & 255) ) * 4;
								*(ulong *)wpgenscan = *(ulong *)(mapadr + mapoff);
								wpgenscan += 4;
								uint += upente;
								vint += vpente;
							}
						}
					}
				}
			}
*/
			level++;
			wpgenscan = pgenscan;
		}

		// envirronment and bump

		if (EVMAP)
		{
			if (BUMP)
			{
				if (level)
				{
					for (c2 = 0; c2 < scansize; c2++)
					{
						bumpoff = bumpadr[ ((v3int >> 8) & 0xff00) + ((u3int >> 16) & 255) ];
						mapoff = ( ((v2int >> 8) & 0x0ff00) + ((u2int >> 16) & 255) );
						mapoff += bumpoff;
						mapoff &= 65535;
						mapoff *= 4;

						wpgenscan[0] = mulshader[ wpgenscan[0] * 256 + evmapadr[mapoff + 0]];
						wpgenscan[1] = mulshader[ wpgenscan[1] * 256 + evmapadr[mapoff + 1]];
						wpgenscan[2] = mulshader[ wpgenscan[2] * 256 + evmapadr[mapoff + 2]];

						wpgenscan += 4;
						u2int += u2pente;
						v2int += v2pente;
						u3int+=u3pente;
						v3int+=v3pente;
					}
				}
				else
				{
					for (c2 = 0; c2 < scansize; c2++)
					{
						bumpoff = bumpadr[ ((v3int >> 8) & 0xff00) + ((u3int >> 16) & 255) ];
						mapoff = ( ((v2int >> 8) & 0x0ff00) + ((u2int >> 16) & 255) );
						mapoff += bumpoff;
						mapoff &= 65535;

						*(ulong *)wpgenscan = *(ulong *)(evmapadr + mapoff * 4);

						wpgenscan += 4;
						u2int += u2pente;
						v2int += v2pente;
						u3int+=u3pente;
						v3int+=v3pente;
					}
				}
			}
			else		// evmap only
			{
				if (level)
				{
					for (c2 = 0; c2 < scansize; c2++)
					{
						mapoff = ( ((v2int >> 8) & 0x0ff00) + ((u2int >> 16) & 255) ) * 4 ;

						wpgenscan[0] = mulshader[ wpgenscan[0] * 256 + evmapadr[mapoff + 0]];
						wpgenscan[1] = mulshader[ wpgenscan[1] * 256 + evmapadr[mapoff + 1]];
						wpgenscan[2] = mulshader[ wpgenscan[2] * 256 + evmapadr[mapoff + 2]];

						wpgenscan += 4;
						u2int += u2pente;
						v2int += v2pente;
					}
				}
				else
				{
					for (c2 = 0; c2 < scansize; c2++)
					{
						mapoff = ( ((v2int >> 8) & 0x0ff00) + ((u2int >> 16) & 255) ) * 4;
						*(ulong *)wpgenscan = *(ulong *)(evmapadr + mapoff);

						wpgenscan += 4;
						u2int += u2pente;
						v2int += v2pente;
					}
				}
			}

			level++;
			wpgenscan = pgenscan;
		}

		// ALPHA PROCESS ------------------------------------------------------------

		if (ADDITIVE)
		{
			if (ALPHATEX)
			{
				for (c2 = 0; c2 < scansize; c2++)
				{
					mapoff = ( ((v4int >> 8) & 0x0ff00) + ((u4int >> 16) & 255) );
					bc = mulshader[ wpgenscan[0] * 256 + ialpadr[mapoff] ];
					gc = mulshader[ wpgenscan[1] * 256 + ialpadr[mapoff] ];
					rc = mulshader[ wpgenscan[2] * 256 + ialpadr[mapoff] ];

					if (pscreen[0] + bc > 255)	pscreen[0] = (uchar)255; else pscreen[0] += (uchar)bc;
					if (pscreen[1] + gc > 255)	pscreen[1] = (uchar)255; else pscreen[1] += (uchar)gc;
					if (pscreen[2] + rc > 255)	pscreen[2] = (uchar)255; else pscreen[2] += (uchar)rc;

					pscreen += 4;
					wpgenscan += 4;
					u4int += u4pente;
					v4int += v4pente;
				}
			}
			else if (pfacelist->alpha != 0)
			{
				pmulshader = &mulshader[ long((1. - pfacelist->alpha) * 255) * 256];

				for (c2 = 0; c2 < scansize; c2++)
				{
					bc = pmulshader[ wpgenscan[0] ];
					gc = pmulshader[ wpgenscan[1] ];
					rc = pmulshader[ wpgenscan[2] ];

					if (pscreen[0] + bc > 255)	pscreen[0] = (uchar)255; else pscreen[0] += (uchar)bc;
					if (pscreen[1] + gc > 255)	pscreen[1] = (uchar)255; else pscreen[1] += (uchar)gc;
					if (pscreen[2] + rc > 255)	pscreen[2] = (uchar)255; else pscreen[2] += (uchar)rc;

					pscreen += 4;
					wpgenscan += 4;
				}
			}
			else
			{
				for (c2 = 0; c2 < scansize; c2++)
				{
					if (pscreen[0] + wpgenscan[0] > 255) pscreen[0] = 255; else pscreen[0] += wpgenscan[0];
					if (pscreen[1] + wpgenscan[1] > 255) pscreen[1] = 255; else pscreen[1] += wpgenscan[1];
					if (pscreen[2] + wpgenscan[2] > 255) pscreen[2] = 255; else pscreen[2] += wpgenscan[2];

					pscreen += 4;
					wpgenscan += 4;
				}
			}
		}
		else if (ALPHATEX)
		{
			for (c2 = 0; c2 < scansize; c2++)
			{
				mapoff = ( ((v4int >> 8) & 0x0ff00) + ((u4int >> 16) & 255) );
				bc = mulshader[ wpgenscan[0] * 256 + ialpadr[mapoff] ];
				gc = mulshader[ wpgenscan[1] * 256 + ialpadr[mapoff] ];
				rc = mulshader[ wpgenscan[2] * 256 + ialpadr[mapoff] ];

				pscreen[0] = (uchar)(mulshader[pscreen[0] * 256 + alpadr[mapoff]] + bc);
				pscreen[1] = (uchar)(mulshader[pscreen[1] * 256 + alpadr[mapoff]] + gc);
				pscreen[2] = (uchar)(mulshader[pscreen[2] * 256 + alpadr[mapoff]] + rc);

				pscreen += 4;
				wpgenscan += 4;
				u4int += u4pente;
				v4int += v4pente;
			}
		}
		else if (pfacelist->alpha != 0)
		{
			pmulshader = &mulshader[ long((1. - pfacelist->alpha) * 255) * 256];
			ipmulshader = &mulshader[ long(pfacelist->alpha * 255) * 256];

			for (c2 = 0; c2 < scansize; c2++)
			{
				pscreen[0] = ipmulshader[pscreen[0]] + pmulshader[wpgenscan[0]];
				pscreen[1] = ipmulshader[pscreen[1]] + pmulshader[wpgenscan[1]];
				pscreen[2] = ipmulshader[pscreen[2]] + pmulshader[wpgenscan[2]];

				pscreen += 4;
				wpgenscan += 4;
			}
		}
		else
		{
			lpscreen = (long *)pscreen;
			lpgenscan = (long *)pgenscan;

			for (c2 = 0; c2 < scansize; c2++)
				lpscreen[c2] = lpgenscan[c2];
		}

		// EO generic renders
		/////////////////////

		noscan:;
	    fxint += fpentexint;
		fxext += fpentexext;

		if (PERSPECTIVE)
		{
		    fzint += fpentezint;
			fzext += fpentezext;
		}
		if (MAPPED)
		{
			fuint += fpenteuint;
			fuext += fpenteuext;
			fvint += fpentevint;
			fvext += fpentevext;
		}
		if (EVMAP)
		{
			fu2int += fpenteu2int;
			fu2ext += fpenteu2ext;
			fv2int += fpentev2int;
			fv2ext += fpentev2ext;
		}
		if (BUMP)
		{
			fu3int += fpenteu3int;
			fu3ext += fpenteu3ext;
			fv3int += fpentev3int;
			fv3ext += fpentev3ext;
		}
		if (ALPHATEX)
		{
			fu4int += fpenteu4int;
			fu4ext += fpenteu4ext;
			fv4int += fpentev4int;
			fv4ext += fpentev4ext;
		}
		if (GOURAUD)
		{
			frint += fpenterint;
			frext += fpenterext;
			fgint += fpentegint;
			fgext += fpentegext;
			fbint += fpentebint;
			fbext += fpentebext;
		}

		tmpcalc+=screen->pitch;
	}
}
