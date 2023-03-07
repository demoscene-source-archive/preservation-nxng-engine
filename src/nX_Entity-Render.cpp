//	------------------------------------------------------------------------------
//
//		nXv9 by xBaRr/Syndrome 2000
//			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
//			All rights reserved 2000.
//
//		Part of nXng.
//		mailto://xbarr@pingoo.com
//		http://xbarr.planet-d.net
//
//		nXng Entity Polygon Creation control implementation file
//
//		Entities-> Transformations from LWS or User Motions
//		Entities-> belonging to scenes add their polygon to viewport (being clipped by nX)
//		nX-> Sort final polygon table
//		nX-> Rasterize polygon table
//
//	------------------------------------------------------------------------------


	#include	"nX_HeaderList.h"
	static		char			str[256];
	extern		unsigned char	mulshader[65536];
	#include	"nXng/nXCol_FASTinlines.cpp"


	#define	SKIPFACE	{if (!stdenormal) pnX->penorm2 += 3 * nedge; mapcoordoffset += 2 * nedge; goto	skipface;}


//-----------------------------------------------------------------------------
void	nX_Entity::Freeze()
//		Create polygons set.
//-----------------------------------------------------------------------------
{

	if (!nbface || (scene->dissolve == 1) )
		return;

	nX					*pnX = scene->pnX;
	nX_Surface			*surf;

	// transformed geometry
	float				*c2d = pnX->p3dpswap2;

	// mapping index
	long				mapcoordoffset = 0;

	// infos to build polygons
	unsigned long		*pindex;
	long				nedge, c2;
	nX_vertex			*pworklocalvertex;
	nX_reliure			*preliure = reliure_adress;
	nX_LightMapInfo		*plmapinfo = lmaplist;
	float				farthest, coef;

	// RENDERSTATES shortcuts
	long				MAPPED, EVMAP, LIGHTSOURCE, GOURAUD, LIGHTMAP;
	long				SPECULAR, ALPHATEX, ADDITIVE, FOREMOST, PROCMAP;
	bool				zout;

	// BUILD ENTITY POLYGONS
	//-------------------------------------------------------------------------


	for (unsigned long c = 0; c < nbface; c++)
	{

		nedge = preliure->nedge;
		surf = preliure->surf;
		pindex = preliure->index;


		// VISIBILITY (FACE LEVEL)
		//---------------------------------------------------------------------


		// moa for entity
		if (scene->usepreca)
		{
			if (scene->pprecabuffer[0] == scene->precafacecounter)
			{
				scene->pprecabuffer++;
				scene->facewastodraw++;
			}
			else
				SKIPFACE
		}

		// perform behind-viewport faces removal
		zout = true;

		for (c2 = 0; c2 < nedge; c2++)
			if (c2d[pindex[c2] + 2] > 1.0)
				zout = false;
		// behind the camera
		if (zout)
			SKIPFACE

		// 3d back face cull
		if ( !(surf->CAPS & nX_DOUBLESIDED) )
		{
			if ( 
				 (
				  (nedge > 2) &&
			      (c2d[pindex[2]] * (c2d[pindex[0] + 2] * c2d[pindex[1] + 1] - c2d[pindex[0] + 1] * c2d[pindex[1] + 2]) +
				   c2d[pindex[2] + 1] * (c2d[pindex[0]] * c2d[pindex[1] + 2] - c2d[pindex[0] + 2] * c2d[pindex[1]]) +
				   c2d[pindex[2] + 2] * (c2d[pindex[0] + 1] * c2d[pindex[1]] - c2d[pindex[0]] * c2d[pindex[1] + 1]) >= 0)
				 )
				)
			SKIPFACE
		}

		// setup renderstates
		//---------------------------------------------------------------------

		MAPPED = surf->CAPS & nX_MAPPED;
		EVMAP = surf->CAPS & nX_EVMAP;
		LIGHTSOURCE = surf->CAPS & nX_LIGHTSOURCE;
		GOURAUD = surf->CAPS & nX_GOURAUD;
		ALPHATEX = surf->CAPS & nX_ALPHATEX;
		ADDITIVE = surf->CAPS & nX_ADDITIVE;
		SPECULAR = surf->xtdCAPS & nX_SPECULAR;
		FOREMOST = surf->xtdCAPS & nX_FOREMOST;
		PROCMAP = surf->xtdCAPS & nX_PROCEDURALMAPPING;
		if (plmapinfo)
				LIGHTMAP = plmapinfo->lightmapped;
		else	LIGHTMAP = 0;

		// GEOMETRY
		pnX->pfacelist->nedge = (unsigned char)nedge;	// the face # vertice
		pnX->pfacelist->caster = this;					// the face caster
		pnX->pfacelist->surf = surf;					// the face surface

		// ALPHA BLEND
		if (Dissolve)
				pnX->pfacelist->alpha = float (surf->transparency + (1 - surf->transparency) * Dissolve->values[0]);
		else	pnX->pfacelist->alpha = surf->transparency;

		// WIREFRAME [Adaptative Sampling]
		if (surf->CAPS & nX_WIREFRAME)
			pnX->pfacelist->alpha = 0.8f + pnX->pfacelist->alpha / 5.0f;

		// WARNING! MOA has no way to predict realtime playback scene dissolve!
		if (pnX->pfacelist->alpha >= 1)
			SKIPFACE

		pnX->pfacelist->alpha += scene->dissolve;
		if (pnX->pfacelist->alpha > 1)
			pnX->pfacelist->alpha = 1;
		else if (pnX->pfacelist->alpha < 0)
			pnX->pfacelist->alpha = 0;

		// LIGTHSOURCE (excluded by fullblown LIGHTMAPs!)
		if ( (LIGHTMAP != 1) && LIGHTSOURCE)
		{
			pnX->ModelLight(this, surf, c2d + pindex[0], pnX->pfnorm2, SPECULAR);

			if (pnX->diff.r + surf->luminosity < 255) pnX->diff.r += surf->luminosity;
			else pnX->diff.r = 255;
			if (pnX->diff.g + surf->luminosity < 255) pnX->diff.g += surf->luminosity;
			else pnX->diff.g = 255;
			if (pnX->diff.b + surf->luminosity < 255) pnX->diff.b += surf->luminosity;
			else pnX->diff.b = 255;

			if ( (!MAPPED && !EVMAP) || PROCMAP )
 			{
				// compute exact physic color model for face
 				pnX->pfacelist->diff.r = mulshader[(pnX->diff.r << 8) + surf->color.r];
				pnX->pfacelist->diff.g = mulshader[(pnX->diff.g << 8) + surf->color.g];
				pnX->pfacelist->diff.b = mulshader[(pnX->diff.b << 8) + surf->color.b];
			}
			else
				nXcolcpy(&pnX->pfacelist->diff, &pnX->diff);
		}

		// build it without clipping if possible

		farthest = 0;
		pnX->pfacelist->vertice = pnX->pvertexlist;			// get next free vertex
		pworklocalvertex = pnX->pvertexlist;


		// CONSTRUCT/CLIP POLYGONS & VALID IF OK ------------------------------
		///////////////////////////////////////////////////////////////////////

		// if BoundingBox reported full 'OnScreen' object then do not clip faces
		if (visibility == 2)
		{

			for (c2 = 0; c2 < nedge; c2++)
			{

				coef = (float)(1.0f / (c2d[pindex[0]+2] * (scene->Focal->values[0]) + scene->offocal) );

				pnX->pvertexlist->x = float (c2d[pindex[0]+0] * coef * scene->viewcx);
				pnX->pvertexlist->x += scene->viewportMidX;
				pnX->pvertexlist->x *= pnX->screen->wcoef;

				pnX->pvertexlist->y = float (c2d[pindex[0]+1] * coef * scene->viewcy);
				pnX->pvertexlist->y += scene->viewportMidY;
				pnX->pvertexlist->y *= pnX->screen->hcoef;

				pnX->pvertexlist->z = c2d[pindex[0]+2];

				if (pnX->pvertexlist->y < 0)
					pnX->pvertexlist->y = 0;

				// sorting is done via farthest Z

				if ( c2d[pindex[0] + 2] * scene->radixprec > farthest )
					farthest = float (c2d[pindex[0]+2] * scene->radixprec);

				// fill CAPS' fields

				if (MAPPED)
				{
					pnX->pvertexlist->u[nXML_COLOR] = UVmap[nXML_COLOR][mapcoordoffset+0] + surf->offsetx[nXML_COLOR];
					pnX->pvertexlist->v[nXML_COLOR] = UVmap[nXML_COLOR][mapcoordoffset+1] + surf->offsety[nXML_COLOR];
				}
				if (EVMAP)
				{
					if (stdenormal)
					{
						pnX->pvertexlist->u[nXML_EVMAP] = float (pnX->penorm2[pindex[0] + 0] / 2 + 0.5 + surf->offsetx[nXML_EVMAP]);
						pnX->pvertexlist->v[nXML_EVMAP] = float (pnX->penorm2[pindex[0] + 1] / 2 + 0.5 + surf->offsety[nXML_EVMAP]);
					}
					else
					{
						pnX->pvertexlist->u[nXML_EVMAP] = float (pnX->penorm2[0] / 2 + 0.5 + surf->offsetx[nXML_EVMAP]);
						pnX->pvertexlist->v[nXML_EVMAP] = float (pnX->penorm2[1] / 2 + 0.5 + surf->offsety[nXML_EVMAP]);
					}
				}
				if (ALPHATEX)
				{
					pnX->pvertexlist->u[nXML_TRANSPARENCY] = UVmap[nXML_TRANSPARENCY][mapcoordoffset+0] + surf->offsetx[nXML_TRANSPARENCY];
					pnX->pvertexlist->v[nXML_TRANSPARENCY] = UVmap[nXML_TRANSPARENCY][mapcoordoffset+1] + surf->offsety[nXML_TRANSPARENCY];
				}

				if (LIGHTMAP)
				{
					pnX->pvertexlist->u[nXML_LIGHTMAP] = UVmap[nXML_LIGHTMAP][mapcoordoffset+0];
					pnX->pvertexlist->v[nXML_LIGHTMAP] = UVmap[nXML_LIGHTMAP][mapcoordoffset+1];
				}

				if (GOURAUD && (LIGHTMAP != 1))
				{
					if (stdenormal)
							pnX->ModelLight(this, surf, c2d + pindex[0], pnX->penorm2 + pindex[0], SPECULAR);
					else	pnX->ModelLight(this, surf, c2d + pindex[0], pnX->penorm2, SPECULAR);

					if (pnX->diff.r + surf->luminosity < 255) pnX->diff.r += surf->luminosity;
					else pnX->diff.r = 255;
					if (pnX->diff.g + surf->luminosity < 255) pnX->diff.g += surf->luminosity;
					else pnX->diff.g = 255;
					if (pnX->diff.b + surf->luminosity < 255) pnX->diff.b += surf->luminosity;
					else pnX->diff.b = 255;

					if ( (!MAPPED && !EVMAP) || PROCMAP )
					{
						// compute exact physic color model for vertex
						pnX->pvertexlist->diff.r = mulshader[(pnX->diff.r << 8) + surf->color.r];
						pnX->pvertexlist->diff.g = mulshader[(pnX->diff.g << 8) + surf->color.g];
						pnX->pvertexlist->diff.b = mulshader[(pnX->diff.b << 8) + surf->color.b];
					}
					else
						nXcolcpy(&pnX->pvertexlist->diff, &pnX->diff);
				}

				if (SPECULAR)	// 'spec' is valid because we cannot have specular without flat or gouraud and both did prior call to ModelLight()
				{
					if (Dissolve && (Dissolve->values[0] != 0.0) )
					{
						pnX->pvertexlist->spec.r = unsigned char( float(pnX->spec.r) * (1 - Dissolve->values[0]) );
						pnX->pvertexlist->spec.g = pnX->pvertexlist->spec.r;
						pnX->pvertexlist->spec.b = pnX->pvertexlist->spec.r;
					}
					else
						nXcolcpy(&pnX->pvertexlist->spec, &pnX->spec);
				}

				// onto next vertex

				pnX->pvertexlist++;
				mapcoordoffset += 2;
				if (!stdenormal)
					pnX->penorm2 += 3;
				pindex++;
			}

			// this face do not require any clipping, MOVE ON!
			if (LIGHTMAP)
					pnX->pfacelist->lmap = plmapinfo;	// the face lmapinfo node
			else	pnX->pfacelist->lmap = NULL;

			if (ADDITIVE || ALPHATEX || (surf->xtdCAPS & nX_MERGEDALPHA) || pnX->pfacelist->alpha > 0)
					pnX->ValidFace( ulong(farthest), pnX->nX_TRANSPARENTRADIXCONST );
			else	pnX->ValidFace( ulong(farthest), pnX->nX_OPAQUERADIXCONST );
			goto	skipface;
		}
		else
		{

			for (c2 = 0; c2 < nedge; c2++)
			{
				pworklocalvertex->x = c2d[pindex[0]+0];
				pworklocalvertex->y = c2d[pindex[0]+1];
				pworklocalvertex->z = c2d[pindex[0]+2];

				// sorting is done via farthest Z

				if ( c2d[pindex[0]+2] * scene->radixprec > farthest )
					farthest = float (c2d[pindex[0]+2] * scene->radixprec);

				// fill CAPS' fields

				if (MAPPED)
				{
					pworklocalvertex->u[nXML_COLOR] = UVmap[nXML_COLOR][mapcoordoffset+0] + surf->offsetx[nXML_COLOR];
					pworklocalvertex->v[nXML_COLOR] = UVmap[nXML_COLOR][mapcoordoffset+1] + surf->offsety[nXML_COLOR];
				}
				if (EVMAP)
				{
					if (stdenormal)
					{
						pworklocalvertex->u[nXML_EVMAP] = float (pnX->penorm2[pindex[0] + 0] / 2 + 0.5 + surf->offsetx[nXML_EVMAP]);
						pworklocalvertex->v[nXML_EVMAP] = float (pnX->penorm2[pindex[0] + 1] / 2 + 0.5 + surf->offsety[nXML_EVMAP]);
					}
					else
					{
						pworklocalvertex->u[nXML_EVMAP] = float (pnX->penorm2[0] / 2 + 0.5 + surf->offsetx[nXML_EVMAP]);
						pworklocalvertex->v[nXML_EVMAP] = float (pnX->penorm2[1] / 2 + 0.5 + surf->offsety[nXML_EVMAP]);
					}
				}
				if (ALPHATEX)
				{
					pworklocalvertex->u[nXML_TRANSPARENCY] = UVmap[nXML_TRANSPARENCY][mapcoordoffset+0] + surf->offsetx[nXML_TRANSPARENCY] + surf->offsetx[nXML_TRANSPARENCY];
					pworklocalvertex->v[nXML_TRANSPARENCY] = UVmap[nXML_TRANSPARENCY][mapcoordoffset+1] + surf->offsety[nXML_TRANSPARENCY] + surf->offsetx[nXML_TRANSPARENCY];
				}

				if (LIGHTMAP)
				{
					pworklocalvertex->u[nXML_LIGHTMAP] = UVmap[nXML_LIGHTMAP][mapcoordoffset + 0];
					pworklocalvertex->v[nXML_LIGHTMAP] = UVmap[nXML_LIGHTMAP][mapcoordoffset + 1];
				}

				if (GOURAUD && (LIGHTMAP != 1) )
				{
					if (stdenormal)
							pnX->ModelLight(this, surf, c2d + pindex[0], pnX->penorm2 + pindex[0], SPECULAR);
					else	pnX->ModelLight(this, surf, c2d + pindex[0], pnX->penorm2, SPECULAR);

					if (pnX->diff.r + surf->luminosity < 255) pnX->diff.r += surf->luminosity;
					else pnX->diff.r = 255;
					if (pnX->diff.g + surf->luminosity < 255) pnX->diff.g += surf->luminosity;
					else pnX->diff.g = 255;
					if (pnX->diff.b + surf->luminosity < 255) pnX->diff.b += surf->luminosity;
					else pnX->diff.b = 255;

					if ( (!MAPPED && !EVMAP) || PROCMAP )
					{
						// compute exact physic color model for vertex
						pworklocalvertex->diff.r = mulshader[(pnX->diff.r << 8) + surf->color.r];
						pworklocalvertex->diff.g = mulshader[(pnX->diff.g << 8) + surf->color.g];
						pworklocalvertex->diff.b = mulshader[(pnX->diff.b << 8) + surf->color.b];
					}
					else
						nXcolcpy(&pworklocalvertex->diff, &pnX->diff);
				}

				if (SPECULAR)	// 'spec' is valid because we cannot have specular without flat or gouraud and both did prior call to ModelLight()
				{
					if (Dissolve && (Dissolve->values[0] != 0.0) )
					{
						pworklocalvertex->spec.r = unsigned char( float(pnX->spec.r) * (1 - Dissolve->values[0]) );
						pworklocalvertex->spec.g = pworklocalvertex->spec.r;
						pworklocalvertex->spec.b = pworklocalvertex->spec.r;
					}
					else
						nXcolcpy(&pworklocalvertex->spec, &pnX->spec);
				}

				// onto next vertex

				pworklocalvertex++;
				mapcoordoffset += 2;
				if (!stdenormal)
					pnX->penorm2 += 3;
				pindex++;
			}

			// face clipping

			if (pnX->ClipFace())
			{
				if (LIGHTMAP)
						pnX->pfacelist->lmap = plmapinfo;
				else	pnX->pfacelist->lmap = NULL;

				if (ADDITIVE || ALPHATEX || (surf->xtdCAPS & nX_MERGEDALPHA) || pnX->pfacelist->alpha > 0)
						pnX->ValidFace( ulong(farthest), pnX->nX_TRANSPARENTRADIXCONST );
				else	pnX->ValidFace( ulong(farthest), pnX->nX_OPAQUERADIXCONST );
			}
		}


		// keep indexes up-to-date
skipface:;
		preliure++;
		pnX->pfnorm2 += 3;

		if (plmapinfo)
			plmapinfo++;
		scene->precafacecounter++;
	}

}
