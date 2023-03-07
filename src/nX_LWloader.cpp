	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		nXng Scene Lightwave Loader implementation file

	----------------------------------------------------------------------------*/


	#include	"nX_HeaderList.h"
	#include	"IO/cqString.h"
	#include	"IO/MCIO.h"

	static		qString			cStr;
	static		char			str[256];
	static		MCIO			lMC;
	static		char			objfile[256];
	static		LoadLWOConfig	lLWOcfg;
	static		xLinker			xPK;
	extern		unsigned char	mulshader[65536];


///////////////////////////////////////////////////////////////////////////////
void inline	ValidateTexLevel(long texlevel, nX_Entity *entity, nX_Surface *surface)
///////////////////////////////////////////////////////////////////////////////
{

	surface->texlevel[texlevel] = true;
	entity->texlevel[texlevel] = true;

	if (texlevel == nXML_COLOR)
		surface->CAPS |= nX_MAPPED;
	else if (texlevel == nXML_EVMAP)
		surface->CAPS |= nX_EVMAP;
	else if (texlevel == nXML_TRANSPARENCY)
		surface->CAPS |= nX_ALPHATEX;

}


///////////////////////////////////////////////////////////////////////////////
static	uchar	GetMappingType(long texlevel, nX_Entity *entity, nX_Surface *surface, char *LWO)
///////////////////////////////////////////////////////////////////////////////
{

	if (!cStr.qstrncmp(LWO, "Planar Image Map", 16))
		surface->mappingtype[texlevel] = 0;

	else if (!cStr.qstrncmp(LWO, "Cubic Image Map", 15))
	{
		surface->mappingtype[texlevel] = 1;
		entity->requirement |= nX_USECUBICMAP;
	}

	else if (!cStr.qstrncmp(LWO, "Cylindrical Image Map", 21))
		surface->mappingtype[texlevel] = 2;

	else if (!cStr.qstrncmp(LWO, "Spherical Image Map", 19))
		surface->mappingtype[texlevel] = 3;

	// arithmetic textures
	else if (!cStr.qstrncmp(LWO, "Fractal Noise", 13) || !cStr.qstrncmp(LWO, "Fractal Bumps", 13))
	{
		surface->mappingtype[texlevel] = 1;
		if (texlevel == 0)
			surface->xtdCAPS |= nX_PROCEDURALMAPPING;
		entity->requirement |= nX_USECUBICMAP;
		ValidateTexLevel(texlevel, entity, surface);
		return	1;		// still have to set map path
	}
	else if (!cStr.qstrncmp(LWO, "Crumple", 7))
	{
		surface->mappingtype[texlevel] = 1;
		if (texlevel == 0)
			surface->xtdCAPS |= nX_PROCEDURALMAPPING;
		entity->requirement |= nX_USECUBICMAP;
		ValidateTexLevel(texlevel, entity, surface);
		return	2;		// still have to set map path
	}

	// we're done
	return	0;			// map path will be set from another sub-chunk

}


///////////////////////////////////////////////////////////////////////////////
nX_TexMap	*nX_Scene::LoadAndRegisterTexMapSurface(char *file, long editindex, nX_Surface *surface, xLinker *xPK)
//	Try to find an already loaded version of the requested file, if it can't
//	load the map register it into nXLL and nX_SceneLL then returns a pointer
//	to the map. The map is also attached to a surface.
///////////////////////////////////////////////////////////////////////////////
{

	bool	bRes;

	// get caps from surface
	nXTMC	nCAPS = nXTMC_NONE;
	if (surface->xtdCAPS & nX_WIDEMAP)
		nCAPS |= nXTMC_WIDE;
	if (surface->negative[editindex])
		nCAPS |= nXTMC_NEGATIVE;

	// check for already known map name in nX
	nX_TexMap *map = pnX->FindTexMap(file, nCAPS);

	// If we didn't match, we have to load it...
	if (!map)
	{
		// create & load new map
		map = new nX_TexMap(pnX);
		bRes = map->Load(file, xPK);
		if (!bRes)
		{	delete map;
			return NULL;	}

		// Keep track of it in nX map list
		pnX->RegisterTexMap(map);
		// Apply Surface FXs to RGB (like negative or flip)
		map->ApplySurfaceFXs(surface, editindex);
		// Conform the map to the renderer
		map->ConformToD3D();
		// That was it (:
		nbtexmap++;
	}

	surface->map[editindex] = map;
	AddMapToLLTexMap(map);
	map->AddRef();
	return map;

}


///////////////////////////////////////////////////////////////////////////////
nX_TexMap	*nX_Scene::LoadAndRegisterTexMap(char *file, xLinker *xPK)
//	Try to find an already loaded version of the requested texmap, if it can't
//	load the map register it into nXLL and nX_SceneLL then returns a pointer
//	to the map.
//
//	NOTE: The TexMap has still no references when returned!
//	This function is xLinker aware...
///////////////////////////////////////////////////////////////////////////////
{

	// check for already known map name in nX (WE WANT THE ORIGINAL HERE)
	bool			bRes;
	nX_TexMap		*map = pnX->FindTexMap(file, nXTMC_NONE);

	if (!map)
	{
		// create & load new map
		map = new nX_TexMap(pnX);
		bRes = map->Load(file, xPK);
		if (!bRes)
		{	delete map;
			return NULL;	}

		// Keep track of it in nX map list
		pnX->RegisterTexMap(map);
		// Conform the map to the renderer
		map->ConformToD3D();
		nbtexmap++;
	}

	AddMapToLLTexMap(map);
	map->AddRef();
	return map;

}


///////////////////////////////////////////////////////////////////////////////
void	nX_Scene::ProcessSURF(nX_Entity *entity, char *eochunk, nX *pnX)
///////////////////////////////////////////////////////////////////////////////
{

	nX_Surface	*surface;
	char		*eoschunk, *psurf;
	long		enttsurfno = 0, found;
	long		edittexmapindex = nXML_NONE;
	char		*mapname[nX_MAXTEXLEVEL];
	float		v;

	// get the surface index as scene surface adress
	while (strcmp(entsurfacelkp[enttsurfno]->name, (char *)pLWO))
		enttsurfno++;
	surface = entsurfacelkp[enttsurfno];

	while (*pLWO++);
	while (!pLWO[0])
		pLWO++;

	// process surface
	surface->maxsmoothingangle = (float)cos(deg2rad(nX_DefaultMaxSmoothingAngle));
	surface->CAPS = pnX->SurfDefaultShading;

	// check [TRAIL] keyword
	if (cStr.qstrsearch(surface->name, "[TRAIL]") != -1)
		surface->xtdCAPS |= nX_TRAIL;

	// check [NOLIGHT] keyword
	bool disallowlightning = false;
	if (cStr.qstrsearch(surface->name, "[NOLIGHT]") != -1)
		disallowlightning = true;

	// check [CHROMA] keyword
	if (cStr.qstrsearch(surface->name, "CHROMA") != -1)
		surface->CAPS |= nX_CHROMA;

	// check [SUB] key for substractive blending
//	if (cStr.qstrsearch(surface->name, "[SUB]") != -1)
//		surface->CAPS |= nX_SUBSTRACT;

	// check [ADDEVMAP] keyword
	if (cStr.qstrsearch(surface->name, "[ADDEVMAP]") != -1)
		surface->xtdCAPS |= nX_ADDEVMAP;

	// check [DISTO] keyword
	if (cStr.qstrsearch(surface->name, "[DISTO]") != -1)
		_2D_Background = true;

	// check for lightning model restrictions...
	found = cStr.qstrsearch(surface->name, "LGT_");
	psurf = surface->name + found + 4;

	if (found != -1)
	{
		surface->lightspec |= nX_RESTRICTLIGHT;

		// process light list
		while (psurf[0] != 0 &&
			   psurf[0] != ' ')
		{
			if (psurf[0]-'0' >= 0 && psurf[0] - '0' < 10)
				surface->lightallowed[psurf[0] - '0' - 1] = true;
			psurf++;
		}
	}

	// PROCESS WHOLE SUB-CHUNK
	//////////////////////////

	bool	skiptex = false, procedural = false;
	long	c;

	while (pLWO < eochunk)
	{
		// COLR sub-chunk
		if (lMC.GetLong(pLWO) == 'COLR')
		{
			eoschunk = pLWO + lMC.GetWord(pLWO + 4) + 6;

			pLWO += 6;
			surface->color.r = pLWO[0];
			surface->color.g = pLWO[1];
			surface->color.b = pLWO[2];

			pLWO = eoschunk;
		}

		// GLOW sub-chunk --> glow percentage
		else if (lMC.GetLong(pLWO) == 'GLOW')
		{
			eoschunk = pLWO + lMC.GetWord(pLWO + 4) + 6;
			surface->glow = lMC.LongAsFloat(lMC.GetLong(pLWO + 6));
			pLWO = eoschunk;
		}

		// SMAN sub-chunk --> max smoothing angle
		else if (lMC.GetLong(pLWO) == 'SMAN')
		{
			eoschunk = pLWO + lMC.GetWord(pLWO + 4) + 6;

			v = lMC.LongAsFloat(lMC.GetLong(pLWO + 6));
			if ( fabs(v - deg2rad(180.0) ) > 0.001 )
				entity->stdenormal = false;
			surface->maxsmoothingangle = (float)cos(v);
			pLWO = eoschunk;
		}

		// VLUM sub-chunk --> surface's luminosity
		else if (lMC.GetLong(pLWO) == 'VLUM')
		{
			eoschunk = pLWO + lMC.GetWord(pLWO + 4) + 6;
			surface->luminosity = uchar(lMC.LongAsFloat(lMC.GetLong(pLWO + 6)) * 255);
			pLWO = eoschunk;
		}					

		// VDIF sub-chunk --> diffusion
		else if (lMC.GetLong(pLWO) == 'VDIF')
		{
			eoschunk = pLWO + lMC.GetWord(pLWO + 4) + 6;
			surface->diffuse = lMC.LongAsFloat(lMC.GetLong(pLWO + 6));
			pLWO = eoschunk;
		}					

		// VSPC sub-chunk --> specular
		else if (lMC.GetLong(pLWO) == 'VSPC')
		{
			eoschunk = pLWO + lMC.GetWord(pLWO + 4) + 6;
			surface->specular = lMC.LongAsFloat(lMC.GetLong(pLWO + 6));

			pLWO = eoschunk;
		}					

		// GLOS sub-chunk --> glossiness
		else if (lMC.GetLong(pLWO) == 'GLOS')
		{
			eoschunk = pLWO + lMC.GetWord(pLWO + 4) + 6;
			surface->glossiness = float(lMC.GetWord(pLWO + 6));
			pLWO = eoschunk;
		}

		// VTRN sub-chunk --> transparency
		else if (lMC.GetLong(pLWO) == 'VTRN')
		{
			eoschunk = pLWO + lMC.GetWord(pLWO + 4) + 6;
			surface->transparency = lMC.LongAsFloat(lMC.GetLong(pLWO + 6));
			pLWO = eoschunk;
		}

		// CTEX sub-chunk --> colormap infos next
		else if (lMC.GetLong(pLWO) == 'CTEX')
		{
			eoschunk = pLWO + lMC.GetWord(pLWO + 4) + 6;
			pLWO += 6;

			edittexmapindex = nXML_COLOR;
			uchar	type = GetMappingType( edittexmapindex, entity, surface, (char *)pLWO );

			if (type) procedural = true; else procedural = false;
			if (type == 1)	mapname[edittexmapindex] = nX_FRACTALBUMPMAPPATH;
			else if (type == 2) mapname[edittexmapindex] = nX_CRUMPLEMAPPATH;

			pLWO = eoschunk;
		}

		// RTEX sub-chunk --> reflectionmap infos next
		else if (lMC.GetLong(pLWO) == 'RTEX')
		{
			eoschunk = pLWO + lMC.GetWord(pLWO + 4) + 6;
			pLWO += 6;

			edittexmapindex = nXML_EVMAP;
			uchar	type = GetMappingType( edittexmapindex, entity, surface, (char *)pLWO );

			if (type) procedural = true; else procedural = false;
			if (type == 1)	mapname[edittexmapindex] = nX_FRACTALBUMPMAPPATH;
			else if (type == 2) mapname[edittexmapindex] = nX_CRUMPLEMAPPATH;

			pLWO = eoschunk;
		}

		// BTEX sub-chunk --> bumpmap infos next
		else if (lMC.GetLong(pLWO) == 'BTEX')
		{
			eoschunk = pLWO + lMC.GetWord(pLWO + 4) + 6;
/*			pLWO += 6;

			edittexmapindex = nXML_BUMP;
			uchar	type = GetMappingType( edittexmapindex, entity, surface, (char *)pLWO );

			if (type) procedural = true; else procedural = false;
			if (type == 1)	mapname[edittexmapindex] = nX_FRACTALBUMPMAPPATH;
			else if (type == 2) mapname[edittexmapindex] = nX_CRUMPLEMAPPATH;
*/
			pLWO = eoschunk;
		}

		// TTEX sub-chunk --> transparencemap infos next
		else if (lMC.GetLong(pLWO) == 'TTEX')
		{
			eoschunk = pLWO + lMC.GetWord(pLWO + 4) + 6;
			pLWO += 6;

			edittexmapindex = nXML_TRANSPARENCY;
			uchar	type = GetMappingType( edittexmapindex, entity, surface, (char *)pLWO );

			if (type) procedural = true; else procedural = false;
			if (type == 1)	mapname[edittexmapindex] = nX_FRACTALBUMPMAPPATH;
			else if (type == 2) mapname[edittexmapindex] = nX_CRUMPLEMAPPATH;

			pLWO = eoschunk;
		}

		// DTEX sub-chunk --> diffusemap infos next (UNSUPPORTED!)
		else if (lMC.GetLong(pLWO) == 'DTEX')
		{
			edittexmapindex = -1;
			procedural = false;
			pLWO += lMC.GetWord(pLWO + 4) + 6;
		}

		// STEX sub-chunk --> diffusemap infos next (UNSUPPORTED!)
		else if (lMC.GetLong(pLWO) == 'STEX')
		{
			edittexmapindex = -1;
			procedural = false;
			pLWO += lMC.GetWord(pLWO + 4) + 6;
		}

		// TIMG sub-chunk --> the file to map
		else if (lMC.GetLong(pLWO) == 'TIMG' && edittexmapindex != -1)
		{
			eoschunk = pLWO + lMC.GetWord(pLWO + 4) + 6;
			pLWO += 6;

			// (none) are discarded
			if (cStr.qstrcmp((char*)pLWO, "(none)"))
			{
				mapname[edittexmapindex] = (char *)pLWO;
				surface->texlevel[edittexmapindex] = true;
				entity->texlevel[edittexmapindex] = true;

					 if (edittexmapindex == nXML_COLOR)
					surface->CAPS |= nX_MAPPED;
				else if (edittexmapindex == nXML_EVMAP)
					surface->CAPS |= nX_EVMAP;
				else if (edittexmapindex == nXML_TRANSPARENCY)
					surface->CAPS |= nX_ALPHATEX;
			}
			else
				edittexmapindex = nXML_NONE;

			pLWO = eoschunk;
		}

		// RIMG sub-chunk --> the file to use as spherical evmap
		else if (lMC.GetLong(pLWO) == 'RIMG')
		{
			eoschunk = pLWO + lMC.GetWord(pLWO + 4) + 6;
			pLWO += 6;

			// (none) are discarded
			if (cStr.qstrcmp((char*)pLWO, "(none)"))
			{
				mapname[1] = (char *)pLWO;
				surface->texlevel[1] = true;
				entity->texlevel[1] = true;
				surface->CAPS |= nX_EVMAP;
			}
			else
				edittexmapindex = nXML_NONE;

			pLWO = eoschunk;
		}

		// TSIZ sub-chunk --> texture size
		else if (lMC.GetLong(pLWO) == 'TSIZ')
		{
			eoschunk = pLWO + lMC.GetWord(pLWO + 4) + 6;
			pLWO += 6;

			if (edittexmapindex != -1)
			{
				surface->size[edittexmapindex][0]=
					lMC.LongAsFloat(lMC.GetLong(pLWO));
				surface->size[edittexmapindex][1]=
					-lMC.LongAsFloat(lMC.GetLong(pLWO+4));
				surface->size[edittexmapindex][2]=
					lMC.LongAsFloat(lMC.GetLong(pLWO+8));

				if (procedural)
				{
					surface->size[edittexmapindex][0] *= 20.;
					surface->size[edittexmapindex][1] *= 20.;
					surface->size[edittexmapindex][2] *= 20.;
				}
			}

			pLWO = eoschunk;
		}

		// TWRP sub-chunk --> indicate how the texture U,V wrap
		else if (lMC.GetLong(pLWO) == 'TWRP')
		{
			eoschunk = pLWO + lMC.GetWord(pLWO + 4) + 6;
			pLWO += 6;

			if (edittexmapindex != -1)
			{
				surface->mapuwrap[edittexmapindex] = lMC.GetWord(pLWO);
				surface->mapvwrap[edittexmapindex] = lMC.GetWord(pLWO + 2);
			}
			pLWO = eoschunk;
		}

		// TFLG sub-chunk --> add. infos on texture
		else if (lMC.GetLong(pLWO) == 'TFLG')
		{
			eoschunk = pLWO + lMC.GetWord(pLWO + 4) + 6;
			pLWO += 7;

			if (edittexmapindex != -1)
			{
				surface->mappingaxis[edittexmapindex] = pLWO[0] & 7;
				if ((pLWO[0] & 16))
					surface->negative[edittexmapindex] = true;
				if ((pLWO[0] & 32))
					surface->pblend[edittexmapindex] = true;
				if ((pLWO[0] & 64) && edittexmapindex == 0)
					surface->CAPS |= nX_PERSPECTIVE;
			}

			pLWO=eoschunk;
		}

		// TCTR sub-chunk --> texture center
		else if (lMC.GetLong(pLWO) == 'TCTR')
		{
			eoschunk = pLWO + lMC.GetWord(pLWO + 4) + 6;
			pLWO += 6;

			if (edittexmapindex != -1)
			{
				surface->center[edittexmapindex][0]=
					lMC.LongAsFloat(lMC.GetLong(pLWO));
				surface->center[edittexmapindex][1]=
					-lMC.LongAsFloat(lMC.GetLong(pLWO+4));
				surface->center[edittexmapindex][2]=
					lMC.LongAsFloat(lMC.GetLong(pLWO+8));
			}
			pLWO = eoschunk;
		}

		// TVEL sub-chunk --> texture velocity
		else if (lMC.GetLong(pLWO) == 'TVEL')
		{
			eoschunk = pLWO + lMC.GetWord(pLWO + 4) + 6;
			pLWO += 6;

			if (edittexmapindex != -1)
			{
				surface->velocity[edittexmapindex][0] = lMC.LongAsFloat(lMC.GetLong(pLWO));
				surface->velocity[edittexmapindex][1] = lMC.LongAsFloat(lMC.GetLong(pLWO + 4));
				surface->velocity[edittexmapindex][2] = lMC.LongAsFloat(lMC.GetLong(pLWO + 8));
			}
			pLWO = eoschunk;
		}

		// FLAG sub-chunk --> most of surface CAPS
		else if (lMC.GetLong(pLWO) == 'FLAG')
		{
			eoschunk = pLWO + lMC.GetWord(pLWO + 4) + 6;
			pLWO += 6;

			// bit 1: outline
			if ((pLWO[1] & 2))
				surface->CAPS |= nX_WIREFRAME;

			// bit 2: smoothing
			if ((pLWO[1] & 4))
				surface->CAPS |= nX_GOURAUD;

			// bit 7: sharp terminator (interpreted as lightsource)
			if ((pLWO[1] & 256))
				surface->CAPS |= nX_LIGHTSOURCE;

			// bit 8: double sided
			if ((pLWO[0] & 1))
				surface->CAPS |= nX_DOUBLESIDED;

			// bit 9: additive
			if ((pLWO[0] & 2))
				surface->CAPS |= nX_ADDITIVE;

			pLWO = eoschunk;
		}

		// we don't know the chunk so skip it
		else
		{
			if (pLWO < eochunk)
			{
				pLWO = pLWO + lMC.GetWord(pLWO + 4) + 6;
				if (pLWO >= eochunk)
					break;
			}
			else
				break;
		}
	}
				

	// AUTOMATIC SURFACE SETTINGS ----------------------------------------------
	////////////////////////////////////////////////////////////////////////////

	// remove all lightning for [NOLIGHT] surfaces

	if (disallowlightning)
	{
		surface->CAPS &= -(nX_LIGHTSOURCE + 1);
		surface->CAPS &= -(nX_GOURAUD + 1);
	}

	// the case that needs manual set

	if (surface->CAPS == nX_NULL)
		surface->CAPS = nX_FLAT;
	if (surface->CAPS == nX_LIGHTSOURCE)
		surface->CAPS = nX_FLAT + nX_LIGHTSOURCE;

	// catch uncorrected surface CAPS

	if (surface->CAPS & nX_LIGHTSOURCE && surface->CAPS & nX_GOURAUD)
		surface->CAPS &= -(nX_LIGHTSOURCE + 1);

/*
	if ((surface->CAPS & nX_BUMP) && !(surface->CAPS & nX_EVMAP))
	{
		surface->map[1] = pnX->DefaultEVMAP;
		surface->CAPS |= nX_EVMAP;
		surface->texlevel[1] = true;
		entity->texlevel[1] = true;
	}
*/
/*
	if (surface->CAPS & nX_BUMP)	// no D3D bump for now ):
		surface->CAPS &= -(nX_BUMP + 1);
*/
	// this is a total non-sense... but handle it anyway...

	if (surface->CAPS == nX_MAPPED + nX_ADDITIVE + nX_CHROMA)
		surface->CAPS &= -(nX_CHROMA + 1);

	// ENSURE WE HAVE GOURAUD OR FLAT LIGHTSOURCE BEFORE VALIDATING SPECULAR

	if (surface->specular > 0 && ( (surface->CAPS & nX_LIGHTSOURCE) || (surface->CAPS & nX_GOURAUD) ) )
		surface->xtdCAPS |= nX_SPECULAR;

	// discard unsupported WIDE routines

	if (surface->xtdCAPS & nX_WIDEMAP)
		if (!(
			surface->CAPS == nX_MAPPED ||
			surface->CAPS == nX_MAPPED + nX_PERSPECTIVE ||
			surface->CAPS == nX_MAPPED + nX_ADDITIVE ||
			surface->CAPS == nX_ALPHATEX ||
			surface->CAPS == nX_ALPHATEX + nX_LIGHTSOURCE
			))
			surface->xtdCAPS &= -(nX_WIDEMAP + 1);

	if (surface->CAPS & nX_EVMAP ||
		surface->CAPS & nX_GOURAUD)
	{
		surface->requirement |= nX_USEEDGENORMAL;
		entity->requirement |= nX_USEEDGENORMAL;
	}

	if (surface->CAPS & nX_LIGHTSOURCE)
	{
		surface->requirement |= nX_USEFACENORMAL;
		entity->requirement |= nX_USEFACENORMAL;
	}

	// LOAD TEXTURE STAGES -----------------------------------------------------
	////////////////////////////////////////////////////////////////////////////


	for (c = 0; c < nX_MAXTEXLEVEL; c++)
	{
		// ensure we wont try to load an FX map
		if (surface->texlevel[c] && !surface->map[c])
		{

			if (!LoadAndRegisterTexMapSurface(mapname[c], c, surface, NULL))
			{
				surface->texlevel[c] = false;		// remove texlevel if no texture was created/found

				switch (c)
				{
					case nXML_COLOR:
						surface->CAPS &= -(nX_MAPPED + 1);
						break;
					case nXML_EVMAP:
						surface->CAPS &= -(nX_EVMAP + 1);
						break;
					case nXML_TRANSPARENCY:
						surface->CAPS &= -(nX_ALPHATEX + 1);
						break;
				}
			}

		}
	}


	// EXTRA TEXTURE COMPUTING -------------------------------------------------
	////////////////////////////////////////////////////////////////////////////

	// test if d3d can merge alpha and color texture components
	if (surface->texlevel[nXML_COLOR] &&
		surface->texlevel[nXML_TRANSPARENCY])
	{
		for (c = 0; c < 3; c++)
			if (surface->velocity[nXML_COLOR][c] != surface->velocity[nXML_TRANSPARENCY][c])
				goto nooptimd3dalphatex;

		surface->map[nXML_COLOR]->SetD3DMergeAlpha(surface->map[nXML_TRANSPARENCY]);
		surface->CAPS &= -(nX_ALPHATEX + 1);
		surface->xtdCAPS |= nX_MERGEDALPHA;
		surface->map[nXML_TRANSPARENCY]->Stretch(surface->map[nXML_COLOR]->width, surface->map[nXML_COLOR]->height);
		surface->map[nXML_TRANSPARENCY]->GetAlpha();
	}

nooptimd3dalphatex:;

	if (surface->CAPS & nX_ALPHATEX && surface->map[nXML_TRANSPARENCY])
		surface->map[nXML_TRANSPARENCY]->GetAlpha();

	if (surface->CAPS & nX_CHROMA && surface->map[nXML_COLOR])
	{
		surface->map[nXML_COLOR]->GetChroma();
		surface->xtdCAPS |= nX_MERGEDALPHA;
	}

}


///////////////////////////////////////////////////////////////////////////////
void	nX_Scene::LoadLWO(char *Filename, nX_Entity *entity, LoadLWOConfig *cfg)
///////////////////////////////////////////////////////////////////////////////
{

	nX_Surface		*surface;
	nX_reliure		*preliure;
	unsigned long	*pindex;
	unsigned short	*unresolved, *punresolved;

	char			*oLWO, *eoLWO;
	unsigned long	c, soLWO, nbface = 0;
	unsigned long	nedge;
	unsigned long	sochunk;

	// load object file (we are sure it exist!)
	entity->requirement = 0;
	MakeComply2Path(Filename);
	unsigned char	*buf;
	unsigned long	sze;
	pnX->Load(Filename, &buf, &sze, NULL);
	oLWO = pLWO = (char *)buf;

	// low-level chunk info
	pLWO += 4;
	soLWO = lMC.GetLong(pLWO);
	eoLWO = pLWO + soLWO + 4;
	pLWO += 8;
	long			edittexmapindex = -1;
	char			*eochunk;
	char			*TMP;
	bool			match;

	// default is to perform classic gouraud shading...
	// per vertice normal, per face/per vertice normal
	entity->stdenormal = true;

	// PROCESS CHUNKS
	/////////////////

	while (pLWO < eoLWO)
	{

		// IFF 'SURF' header matched
		////////////////////////////
		if (lMC.GetLong(pLWO) == 'SURF' && cfg->surface)
		{
			eochunk = pLWO + lMC.GetLong(pLWO + 4) + 8;

			if (cfg->surface)
			{
				pLWO += 8;
				ProcessSURF(entity, eochunk, pnX);
			}
			pLWO = eochunk;
		}

		// IFF 'SRFS' header matched
		////////////////////////////
		else if (lMC.GetLong(pLWO) == 'SRFS')
		{
			eochunk = pLWO + lMC.GetLong(pLWO + 4) + 8;

			if (cfg->surface)
			{
				// to the start of entity surfaces
				pLWO += 8;
				entity->nbsurf = 0;

				while (pLWO < eochunk)
				{
					//do we already know this surface in scene surface table?
					match = false;
					surface = LLsurface;	

					for (c = 0; c < nbsurface; c++)
					{
						match = true;
						if (!cStr.qstrcmp(&surface->name[0], pLWO))
							break;
						else
							match = false;
						surface = surface->LLnext;
					}

					// if needed copy surface name into scene surface table
					if (!match)
					{
						surface = new nX_Surface(pnX);
						surface->LLnext = LLsurface;
						LLsurface = surface;
						nbsurface++;

						surface->name = new char[strlen(pLWO) + 1];
						strcpy(surface->name, pLWO);
					}
					// surface is already known
					else
					{
						for (c = 0; c < nX_MAXTEXLEVEL; c++)
							if (surface->texlevel[c])
								entity->texlevel[c];
						entity->requirement |= surface->requirement;
					}

					entsurfacelkp[entity->nbsurf] = surface;
					entity->nbsurf++;

					while (pLWO[0])
						pLWO++;
					while (!pLWO[0])
						pLWO++;
				}
			}
			pLWO = eochunk;
		}

		// IFF 'PNTS' header matched. create bounding box!
		//////////////////////////////////////////////////
		else if (lMC.GetLong(pLWO) == 'PNTS')
			ProcessPNTS(entity);

		// IFF 'POLS' header matched
		////////////////////////////
		else if (lMC.GetLong(pLWO) == 'POLS')
		{
			sochunk = lMC.GetLong(pLWO + 4);
			eochunk = pLWO + sochunk + 8;
			pLWO += 8;

			// perform a quick go trough to find # of (face/'independent triangle') in object
			long	dc, unresolv, dnedge, dnpol;
			TMP = pLWO;

			while (pLWO != eochunk)			
			{
				nedge = lMC.GetWord(pLWO);
				pLWO += 2 + 2 * nedge;

				// get surface
				unresolv = lMC.GetWord(pLWO);
				pLWO += 2;

				// jump over detail polygons
				if (unresolv > 32767)
				{
					dnpol = lMC.GetWord(pLWO);
					pLWO += 2;
					for (dc = 0; dc < dnpol; dc++)
					{
						dnedge = lMC.GetWord(pLWO);
						pLWO += 4 + 2 * dnedge;
					}
				}

				// add current face stats... LOOK ABOVE ITS HERE !
				if (nedge >= nX_MINEDGEPOLY && nedge < nX_MAXEDGEPOLY)
				{
					entity->totalglobaledgenumber += nedge;
					entity->nbface++;
					if (nedge > 3)
							entity->nbtri += nedge - 2;
					else	entity->nbtri++;
				}
			}
			pLWO = TMP;

			// allocate reliure for object
			entity->reliure_adress = new nX_reliure[entity->nbface];

			// array to convert LWO surface's to SCENE surfaces
			unresolved = new unsigned short[entity->nbface];
			punresolved = unresolved;

			entity->indexlist = new ulong[entity->totalglobaledgenumber];
			preliure = entity->reliure_adress;

			// convert LW reliure to local one
			pindex = entity->indexlist;

			while (pLWO != eochunk)
			{
				nedge = lMC.GetWord(pLWO);

				// when removing the minimum # vertex/poly limit! LOOK ABOVE
				if (nedge >= nX_MINEDGEPOLY && nedge < nX_MAXEDGEPOLY)
				{
					preliure->nedge = (unsigned char)nedge;
					preliure->index = pindex;
					pLWO += 2;

					for (c = 0; c < nedge; c++)
					{
						*pindex++ = lMC.GetWord(pLWO) * 3;
						pLWO += 2;
					}

					// get surface in its scene
					unresolv = lMC.GetWord(pLWO);
					pLWO += 2;

					// SDK says detail polygons should be ignored from now on...
					if (unresolv > 0x7fff)
					{
						punresolved[0] = (unsigned short)(-unresolv);
						dnpol = lMC.GetWord(pLWO);
						pLWO += 2;
						for (dc = 0; dc < dnpol; dc++)
						{
							dnedge = lMC.GetWord(pLWO);
							pLWO += 4 + 2 * dnedge;
						}
					}
					else
						punresolved[0] = unsigned short (unresolv - 1);

					preliure++;
					punresolved++;
				}
				else
					pLWO += 4 + 2 * nedge;
			}
		}

		// UNKNOWN CHUNK
		////////////////
		else
		{
			if (pLWO < eoLWO)
			{
				pLWO = pLWO + lMC.GetWord(pLWO + 4) + 8;
				if (pLWO >= eoLWO)
					break;
			}
			else
				break;
		}
	}

	// SOME CASES TO HANDLE... ------------------------------------------------
	///////////////////////////////////////////////////////////////////////////

	if (pnX->ForceAllNormals)
	{
		entity->requirement |= nX_USEFACENORMAL;
		entity->requirement |= nX_USEEDGENORMAL;
	}
	if (entity->requirement & nX_USECUBICMAP)
		entity->requirement |= nX_USEFACENORMAL;
	if (entity->requirement & nX_USEEDGENORMAL)
		entity->requirement |= nX_USEFACENORMAL;

	// CONVERT SURFACE INDEX TO SCENE' SURFACES AND GET ENTITY REQUIREMENT ----
	///////////////////////////////////////////////////////////////////////////

	for (c = 0; c < entity->nbface; c++)
		entity->reliure_adress[c].surf = entsurfacelkp[unresolved[c]];

	delete (unresolved);
	delete oLWO;
}


///////////////////////////////////////////////////////////////////////////////
void	nX_Scene::AddIKChain(nX_Entity *src)
///////////////////////////////////////////////////////////////////////////////
{
	IK_Node	*node = new IK_Node;
	node->IKsrc = src;
	node->LLnext = LLIK;
	LLIK = node;
}


///////////////////////////////////////////////////////////////////////////////
bool	nX_Scene::ReadCommonParameters(nX_Entity *entity)
///////////////////////////////////////////////////////////////////////////////
{
	if (!cStr.qstrncmp(pLWS, "ObjectMotion", 12))
	{
		pLWS = entity->LWmotion.LoadLWSMotion(pLWS);
		return true;
	}
	else if (!cStr.qstrncmp(pLWS, "PivotPoint", 10))
	{
		pLWS+=11;

		entity->pivot[0] = cStr.a2f(pLWS);
		while (*pLWS++ != ' ');
		entity->pivot[1] = cStr.a2f(pLWS);
		while (*pLWS++ != ' ');
		entity->pivot[2] = cStr.a2f(pLWS);

		while (pLWS[0]!=0xd && pLWS[1]!=0xa)
			pLWS++;
		pLWS+=2;
		return true;
	}
	else if (!cStr.qstrncmp(pLWS, "ParentObject", 12))
	{
		pLWS += 13;
		entity->iparent = (ushort)cStr.a2f(pLWS);
		return true;
	}
	else if (!cStr.qstrncmp(pLWS, "GoalObject", 10))
	{
		pLWS += 11;
		entity->igoal = (ushort)cStr.a2f(pLWS);
		return true;
	}
	else if (!cStr.qstrncmp(pLWS, "IKAnchor", 8))
	{
		pLWS += 9;
		if (cStr.a2f(pLWS) == 1)
			entity->IKAnchor = true;
		return true;
	}
	else if (!cStr.qstrncmp(pLWS, "HLimits", 7))
	{
		pLWS += 8;
		entity->IKminHeading = (float)deg2rad(cStr.a2f(pLWS));
		while (*pLWS++ != ' ');
		entity->IKmaxHeading = (float)deg2rad(cStr.a2f(pLWS));
		entity->IKHLimits = true;
		return true;
	}
	else if (!cStr.qstrncmp(pLWS, "PLimits", 7))
	{
		pLWS += 8;
		entity->IKminPitch = (float)-deg2rad(cStr.a2f(pLWS));
		while (*pLWS++ != ' ');
		entity->IKmaxPitch = (float)-deg2rad(cStr.a2f(pLWS));
		entity->IKPLimits = true;
		return true;
	}
	else if (!cStr.qstrncmp(pLWS, "FullTimeIK", 10))
	{
		pLWS += 11;
		if (cStr.a2f(pLWS) == 1)
			AddIKChain(entity);
		return true;
	}
	else if (!cStr.qstrncmp(pLWS, "UnaffectedByFog", 15))
	{
		pLWS += 16;
		if (cStr.a2f(pLWS))
			entity->unaffectedbyfog = true;
		return true;
	}	

	return false;
}


///////////////////////////////////////////////////////////////////////////////
static bool	FindObjectFile(char *pLWS, nX_Scene *scn)
///////////////////////////////////////////////////////////////////////////////
{
	char	*pname = objfile;

	pLWS += 11;
	while (pLWS[0] != 0xd && pLWS[1] != 0xa)
		*pname++ = *pLWS++;
	pname[0] = 0;

	scn->MakeComply2Path(objfile);
	if (scn->pnX->FileExist(objfile, NULL))
		return true;
	return false;
}


///////////////////////////////////////////////////////////////////////////////
void	nX_Scene::AddObjectFromLWS(short centity)
///////////////////////////////////////////////////////////////////////////////
{

	char		name[256];
	char		*pname = name;

	// get filename
	pLWS += 11;
	while (pLWS[0] != 0xd && pLWS[1] != 0xa)
		*pname++ = *pLWS++;
	pname[0] = 0;

	// create new entity
	nX_Entity	*entity = new nX_Entity(pnX);
	entity->scene = this;
	entityadress[centity] = entity;
	entity->type = nXETYPE_OBJECT;

	// Raytracing settings
	if (cStr.qstrsearch(name, "[FORCERAY]") != -1)
		entity->forceray = true;
	if (cStr.qstrsearch(name, "[UNSEENRAY]") != -1)
		entity->unseenray = true;
	if (cStr.qstrsearch(name, "[NOLMAP]") != -1)
		entity->nolmap = true;
	if (cStr.qstrsearch(name, "[DOLMAP]") != -1)
		entity->dolmap = true;

	// Decode LWO
	lLWOcfg.surface = true;
	lLWOcfg.forcefacenorm = false;
	lLWOcfg.forceedgenorm = false;
	entity->file = new char[strlen(name) + 1];
	strcpy(entity->file, name);
	LoadLWO(name, entity, &lLWOcfg);

	// additional LWO parameters
	while (cStr.qstrncmp(pLWS, "ShadowOptions", 13))
	{

		if (!cStr.qstrncmp(pLWS, "PolygonSize", 11))
		{
			pLWS += 12;
			if (pLWS[0] == '(')
				pLWS = entity->PolySize->LoadLWSMotion(pLWS);
			else
				entity->PolySize->values[0] = cStr.a2f(pLWS);
		}
		else if (!cStr.qstrncmp(pLWS, "Plugin", 6))
		{
			pLWS += 7;

			if (!cStr.qstrncmp(pLWS, "DisplacementHandler", 19))
			{
				pLWS += 20;
				while (*pLWS++ != ' ');

				if (!cStr.qstrncmp(pLWS, "MD_Plug", 7))
				{
					pLWS += 19;
					pname = str;					
					while (pLWS[0] != 0xd && pLWS[1] != 0xa)
						*pname++ = *pLWS++;
					pname[0] = 0;

					if (entity->LoadMDD(str))
						entity->usemdd = true;
				}

				// get MDD loop flag
				pLWS += 10;
				str[0] = pLWS[0];
				str[1] = 0;
				if (atof(str) == 2)
					entity->MDloop = true;

				// end of plug
				while (cStr.qstrncmp(pLWS++, "EndPlugin", 9));
				pLWS += 10;
			}
		}
		else if (!cStr.qstrncmp(pLWS, "ObjDissolve", 11))
		{
			pLWS += 12;
			entity->Dissolve = new nX_Envelop();

			if (pLWS[0] == '(')
				pLWS = entity->Dissolve->LoadLWSMotion(pLWS);
			else
				entity->Dissolve->values[0] = cStr.a2f(pLWS);
		}
		else if (!cStr.qstrncmp(pLWS, "Metamorph", 9))
		{
			pLWS += 10;
			entity->Morph = new nX_Envelop();

			if (pLWS[0] == '(')
				pLWS = entity->Morph->LoadLWSMotion(pLWS);
			else
				entity->Morph->values[0] = cStr.a2f(pLWS);

			while (cStr.qstrncmp(pLWS, "MorphTarget", 11))
				pLWS++;
			pLWS += 12;
			entity->morphtarget = (unsigned short)cStr.a2f(pLWS);
		}
		else if (!ReadCommonParameters(entity))
		{
			while (pLWS[0] != 0xd && pLWS[1] != 0xa)
				pLWS++;
			pLWS += 2;
		}
	}

	// Get Shadow Options
	pLWS += 14;
	long	so = long(cStr.a2f(pLWS));
	if (!(so & 2))					// cast shadows
		entity->nocastshadow = true;
	if (!(so & 4))					// receive shadows
		entity->nolmap = true;

	// CATCH FX OBJECTS
	///////////////////

	if (cStr.qstrsearch(name, "[SPARK_") != -1)
	{
		entity->SetupSparkEmit(0);
		entity->type = nXETYPE_SPARKEMITER;
	}

	// worm effector
	if (cStr.qstrsearch(name, "[WORM_") != -1)
		entity->useworm = true;

	// wave effector
	if (cStr.qstrsearch(name, "[WAVE]") != -1)
	{
		entity->perframenormalcalc = true;
		entity->wave = true;
	}

	// metaball object
	if (cStr.qstrsearch(name, "[MBALL_") != -1)
	{
		entity->type = nXETYPE_METABALL;			// we should not draw it has standard object!
		entity->metaball = new nX_metaball;
		entity->metaball->radius = 1.0;
		entity->metaball->pEnt = entity;
		hasmetaobjects = true;
	}

	// COMPUTE FACE NORMALS ---------------------------------------------------
	///////////////////////////////////////////////////////////////////////////

	if ( entity->requirement & nX_USEFACENORMAL )
	{
		entity->facenormals_adress = new float[entity->nbface * 3];
		entity->GetFaceNormals();
	}
	entity->ofacenormals = entity->facenormals_adress;

	// COMPUTE EDGE NORMALS (max smoothing angle supported) -------------------
	///////////////////////////////////////////////////////////////////////////

	if ( entity->requirement & nX_USEEDGENORMAL )
	{
		if (entity->stdenormal)
				entity->edgenormals_adress = new float[entity->nbpoints * 3];
		else	entity->edgenormals_adress = new float[entity->totalglobaledgenumber * 3];
		entity->GetEdgeNormals();
	}
	entity->oedgenormals = entity->edgenormals_adress;

}


///////////////////////////////////////////////////////////////////////////////
void	nX_Scene::AddLightFromLWS(short centity)
///////////////////////////////////////////////////////////////////////////////
{

	if (firstlightno == 0)
		firstlightno = centity;

	char	flare[256];
	char	*tmp;
	long	pos, eol;
	nX_Entity	*entity = new nX_Entity(pnX);
	entityadress[centity] = entity;
	entity->type = nXETYPE_LIGHT;
	entity->scene = this;

	// virtual point for light

	entity->nbpoints = 2;
	entity->edge_adress = new float[2 * 3];
	entity->edge_adress[0] = 0;
	entity->edge_adress[1] = 0;
	entity->edge_adress[2] = 0;
	entity->edge_adress[3] = 0;
	entity->edge_adress[4] = 0;
	entity->edge_adress[5] = 1;

	// decode light

	while (cStr.qstrncmp(pLWS, "ShadowType", 10))
	{
		if (!cStr.qstrncmp(pLWS, "LightMotion", 11))
			pLWS = entity->LWmotion.LoadLWSMotion(pLWS);

		else if (!cStr.qstrncmp(pLWS, "LightColor", 10))
		{
			pLWS+=11;

			entity->lightcolor.r= (uchar) ( cStr.a2f(pLWS) );
			while (*pLWS++!=32);
			entity->lightcolor.g= (uchar) ( cStr.a2f(pLWS) );
			while (*pLWS++!=32);
			entity->lightcolor.b= (uchar) ( cStr.a2f(pLWS) );

		}
		else if (!cStr.qstrncmp(pLWS, "LgtIntensity", 12))
		{
			pLWS+=13;
			entity->LightInt = new nX_Envelop();

			if (pLWS[0] == '(')
				pLWS = entity->LightInt->LoadLWSMotion(pLWS);
			else
			{
				entity->LightInt->values[0] = cStr.a2f(pLWS);
				entity->ilightcolor.r = (uchar)(entity->lightcolor.r * entity->LightInt->values[0]);
				entity->ilightcolor.g = (uchar)(entity->lightcolor.g * entity->LightInt->values[0]);
				entity->ilightcolor.b = (uchar)(entity->lightcolor.b * entity->LightInt->values[0]);
			}
		}
		else if (!cStr.qstrncmp(pLWS, "LightName", 9))
		{
			pLWS += 10;

			// Is there a user flare file?

			tmp = pLWS;
			eol = 0;
			while (tmp[0] != 0xd && tmp[1] != 0xa)
			{
				tmp++;
				eol++;
			}
			pos = cStr.qstrsearch(pLWS, "[flare=");

			if (pos != -1 && pos < eol)
			{
				tmp = pLWS + 7 + pos;
				pos = 0;

				while (tmp[0] != ']')
					flare[pos++] = *tmp++;
				flare[pos] = 0;

				// despite we create an Fx surface we Add the map manually to attach it to the scene

				entity->flaresurface = new nX_Surface(pnX);
				entity->flaresurface->QuickSetup(NULL, nX_ADDITIVE + nX_MAPPED, NULL);
				if ( !LoadAndRegisterTexMapSurface(flare, 0, entity->flaresurface, NULL) )
				{
					wsprintf(str, "Could not access '%s' :: LoadLensFlare() failed!", flare);
					pnX->Verbose(str);
					delete entity->flaresurface;
					entity->flaresurface = pnX->DefaultFlareSurface;
				}
			}

			while (pLWS[0] != 0xd && pLWS[1] != 0xa)
				pLWS++;
		}
		else if (!cStr.qstrncmp(pLWS, "LightRange", 10))
		{
			pLWS += 11;
			entity->LightRange = new nX_Envelop();

			if (pLWS[0] == '(')
				pLWS = entity->LightRange->LoadLWSMotion(pLWS);
			else
				entity->LightRange->values[0] = cStr.a2f(pLWS);
		}
		else if (!cStr.qstrncmp(pLWS, "EdgeAngle", 9))
		{
			pLWS += 10;
			entity->EdgeAngle = new nX_Envelop();

			if (pLWS[0] == '(')
				pLWS = entity->EdgeAngle->LoadLWSMotion(pLWS);
			else
				entity->EdgeAngle->values[0] = 1.0f - (float)cos(deg2rad(cStr.a2f(pLWS)));
		}
		else if (!cStr.qstrncmp(pLWS, "ConeAngle", 9))
		{
			pLWS += 10;
			entity->ConeAngle = new nX_Envelop();

			if (pLWS[0] == '(')
				pLWS = entity->ConeAngle->LoadLWSMotion(pLWS);
			else
				entity->ConeAngle->values[0] = (float)cos(deg2rad(cStr.a2f(pLWS)));
		}
		else if (!cStr.qstrncmp(pLWS, "ParentObject", 12))
		{
			pLWS+=13;
			entity->iparent=(ushort)cStr.a2f(pLWS);
		}
		else if (!cStr.qstrncmp(pLWS, "LightType", 9))
		{
			pLWS += 10;
			entity->lighttype = (long)cStr.a2f(pLWS);
		}
		else if (!cStr.qstrncmp(pLWS, "LensFlare", 9))
		{
			pLWS += 10;
			if (cStr.a2f(pLWS))
				entity->lensflare = true;
			if (!entity->flaresurface)
				entity->flaresurface = pnX->DefaultFlareSurface;
		}
		else if (!cStr.qstrncmp(pLWS, "FlareIntensity", 14))
		{
			pLWS += 15;
			entity->FlareInt = new nX_Envelop();

			if (pLWS[0] == '(')
				pLWS = entity->FlareInt->LoadLWSMotion(pLWS);
			else
				entity->FlareInt->values[0] = cStr.a2f(pLWS);
		}
		else
		{
			while (pLWS[0] != 0xd && pLWS[1] != 0xa)
				pLWS++;
			pLWS += 2;
		}
	}

	// we're done

	if (entity->ConeAngle)
		entity->ConeAngle->values[0] += entity->EdgeAngle->values[0];

}


///////////////////////////////////////////////////////////////////////////////
void	nX_Scene::AddNullObjectFromLWS(short centity)
///////////////////////////////////////////////////////////////////////////////
{

	nX_Entity	*entity = new nX_Entity(pnX);
	entityadress[centity] = entity;
	entity->type = nXETYPE_OBJECT;
	entity->scene = this;
	pLWS += 14;

	while (cStr.qstrncmp(pLWS, "ShadowOption", 12))
	{
		if (!ReadCommonParameters(entity))
		{
			while (pLWS[0] != 0xd && pLWS[1] != 0xa)
				pLWS++;
			pLWS += 2;
		}
	}

}


///////////////////////////////////////////////////////////////////////////////
void	nX_Scene::AddCameraFromLWS()
///////////////////////////////////////////////////////////////////////////////
{

	nX_Entity	*entity = new nX_Entity(pnX);
	entity->type = nXETYPE_CAMERA;
	camEntity = entity;
	entity->scene = this;

	// Fill in the LW move structure for camera

	pLWS = entity->LWmotion.LoadLWSMotion(pLWS);

	// Convert extra LW fields

	while (cStr.qstrncmp(pLWS, "DepthOfField", 12))
	{
		if (!cStr.qstrncmp(pLWS, "TargetObject", 12))
		{
			pLWS += 13;
			entity->itarget = (ushort)cStr.a2f(pLWS);
		}
		else if (!cStr.qstrncmp(pLWS, "ParentObject", 12))
		{
			pLWS+=13;
			entity->iparent = (ushort)cStr.a2f(pLWS);
		}
		else if (!cStr.qstrncmp(pLWS, "ZoomFactor", 10))
		{ 
			pLWS += 11;
			Focal = new nX_Envelop();

			if (pLWS[0] == '(')
				pLWS = Focal->LoadLWSMotion(pLWS);
			else
				LWfocal = cStr.a2f(pLWS);
		}
		else if (!cStr.qstrncmp(pLWS, "CustomSize", 10))
		{
			pLWS += 11;
			renderResW = (long)(cStr.a2f(pLWS));

			while (pLWS[0] != ' ') pLWS++;
			while (pLWS[0] == ' ') pLWS++;

			renderResH = (long)(cStr.a2f(pLWS));

			// Resize Custom Size so it fits fullscreen
			if (renderResW < pnX->screen->referencewidth)
			{	renderResH *= (unsigned long)(pnX->screen->referencewidth / renderResW);
				renderResW = pnX->screen->referencewidth;		}
			if (renderResW > pnX->screen->referencewidth)
			{	renderResH *= (unsigned long)(pnX->screen->referencewidth / renderResW);
				renderResW = pnX->screen->referencewidth;		}
			if (renderResH > pnX->screen->referenceheight)
			{	renderResW *= (unsigned long)(pnX->screen->referenceheight / renderResH);
				renderResH = pnX->screen->referenceheight;		}
		}
/*
		else if(!qstrncmp(pLWS, "MotionBlur", 10))
		{
			pLWS += 11;
			if (cStr.a2f((const char *)pLWS) != 0)
				mblur = true;			// scene'll use motion blur
		}
		else if(!qstrncmp(pLWS, "BlurLength", 10))
		{
			pLWS += 11;
			mblurlen = cStr.a2f((const char *)pLWS);
		}
*/
		else
		{
			while (pLWS[0] != 0xd && pLWS[1] != 0xa)
				pLWS++;
			pLWS += 2;
		}
	}

	entityadress[0] = entity;

}


///////////////////////////////////////////////////////////////////////////////
bool	nX_Scene::LoadLWS(char *filename)
///////////////////////////////////////////////////////////////////////////////
{

	wsprintf(str, "nX_Scene::LWSLoad(\"%s\");:", filename);
	pnX->Verbose(str);

	// Perform LOAD and various SETUPS ----------------------------------------
	///////////////////////////////////////////////////////////////////////////

	pnX->cscene = this;
	loadtime = GetTickCount();
	if (!pnX->Load(filename, (unsigned char **)&pLWS, (unsigned long *)&filesize, NULL))
	{
		pnX->Verbose("    ERROR: File not found!");
		return false;
	}

	char	*nX_peoLWS = pLWS + filesize;
	char	*nX_opLWS;
	lwsadress = nX_opLWS = pLWS;

	if (pLWS[4] == 0xa)
	{
		pnX->Verbose("    ERROR: AMIGA carriage return detected!");
		pnX->Verbose("    Please load and save the scene using a PC version of LW.");
		delete pLWS;
		return false;
	}
	if (pLWS[4] == 0xd && pLWS[5] != 0xa)
	{
		pnX->Verbose("    ERROR: Unsupported carriage return detected!");
		pnX->Verbose("    Please load and save the scene using a PC version of LW.");
		delete pLWS;
		return false;
	}
	if (pLWS[6] >= '3')
	{
		pnX->Verbose("    ERROR: Sorry but nX compatibility stops with Lightwave 5.6c ):");
		pnX->Verbose("    This scene is from a newer and unsupported version.");
		delete pLWS;
		return false;
	}

	// store scene's path
	char *ppath = path;
	strcpy(ppath, filename);
	ppath += strlen(filename);

	while (ppath[0] != '\\' && ppath[0] != '/')
		ppath--;
	ppath[1] = 0;

	strcpy(fullfile, filename);		// store scene full filename

	// Reset world boundaries -------------------------------------------------
	///////////////////////////////////////////////////////////////////////////

	minBound.x = FLT_MAX; minBound.y = FLT_MAX; minBound.z = FLT_MAX;
	maxBound.x = FLT_MIN; maxBound.y = FLT_MIN; maxBound.z = FLT_MIN;

	// Let's check for an eventual LightMap info file -------------------------
	///////////////////////////////////////////////////////////////////////////

	unsigned char	*UVset = NULL, *pUV;
	xLinker			xLkLmap;

	if (!bComputeLM)
	{
		strcpy(str, fullfile);
		strcpy(str + strlen(str) - 3, "nxl");
		if (xLkLmap.OpenxLink(str) == XPACK_OK)
			useslightmap = true;

		if (useslightmap)
			UVset = (unsigned char *)xLkLmap.LoadFromxLink("UVset");
		pUV = UVset;

		if (UVset)
		{
			if (*(unsigned long *)pUV != 'LMP2')
			{
				useslightmap = false;
				pnX->Verbose("    ERROR: INCORRECT LIGHTMAP, lightmaps disabled.");
			}
			else
			{
				pUV += 4;
				pnX->Verbose("    Found LIGHTMAPS informations:");
				wsprintf(str, "      Min. lightmap size: %d", pUV[0]);
				pnX->Verbose(str);
				wsprintf(str, "      Max. lightmap size: %d", pUV[1]);
				pnX->Verbose(str);
				pUV += 2;
			}
		}
	}

	// Create the 'Default' Surface -------------------------------------------
	///////////////////////////////////////////////////////////////////////////

	nX_Surface *surface = new nX_Surface(pnX);
	surface->LLnext = LLsurface;
	LLsurface = surface;
	nbsurface++;

	surface->name = new char[8];
	strcpy(surface->name, "Default");

	surface->CAPS = nX_FLAT;
	surface->color.r = 127;
	surface->color.g = 127;
	surface->color.b = 127;

	// LWS ASCII Scene decoding root ------------------------------------------
	///////////////////////////////////////////////////////////////////////////

	short	centity = 0;
	float	rfirstframe = -1, rlastframe = -1, rspeedcoef = -1;
	float	pfirstframe = -1, plastframe = -1, pspeedcoef = -1;

	// default...
	zcut = nX_ZCUTFRONTIER;
	mulfactor = 1.0;

	while (pLWS < nX_peoLWS)
	{
		if (!cStr.qstrncmp(pLWS, "AmbientColor", 12))
		{
			pLWS += 13;

			ambientcolor.r = (uchar) ( cStr.a2f(pLWS) );
			while (*pLWS++!=32);
			ambientcolor.g = (uchar) ( cStr.a2f(pLWS) );
			while (*pLWS++!=32);
			ambientcolor.b = (uchar) ( cStr.a2f(pLWS) );
		}
		else if (!cStr.qstrncmp(pLWS, "AmbIntensity", 12))
		{ 
			pLWS += 13;
			AmbientInt = new nX_Envelop();

			if (pLWS[0]=='(')
				pLWS = AmbientInt->LoadLWSMotion(pLWS);
			else
			{
				AmbientInt->values[0] = cStr.a2f(pLWS);
				iambientcolor.r = (uchar)(ambientcolor.r * AmbientInt->values[0]);
				iambientcolor.g = (uchar)(ambientcolor.g * AmbientInt->values[0]);
				iambientcolor.b = (uchar)(ambientcolor.b * AmbientInt->values[0]);
			}
		}
		else if (!cStr.qstrncmp(pLWS, "AddLight", 8))
		{ 
			centity++;
			nblight++;			
			AddLightFromLWS(centity);
		}
		else if (!cStr.qstrncmp(pLWS, "PreviewFirstFrame", 17))
		{ 
			pLWS += 18;
			pfirstframe = cStr.a2f(pLWS);
		}
		else if (!cStr.qstrncmp(pLWS, "PreviewLastFrame", 16))
		{ 
			pLWS += 17;
			plastframe = cStr.a2f(pLWS);
		}
		else if (!cStr.qstrncmp(pLWS, "PreviewFrameStep", 16))
		{ 
			pLWS += 17;
			pspeedcoef = cStr.a2f(pLWS);
		}
		else if (!cStr.qstrncmp(pLWS, "FirstFrame", 10))
		{ 
			pLWS += 11;
			rfirstframe = cStr.a2f(pLWS);
		}
		else if (!cStr.qstrncmp(pLWS, "LastFrame", 9))
		{ 
			pLWS += 10;
			rlastframe = cStr.a2f(pLWS);
		}
		else if (!cStr.qstrncmp(pLWS, "FrameStep", 9))
		{ 
			pLWS += 10;
			rspeedcoef = cStr.a2f(pLWS);
		}
		else if (!cStr.qstrncmp(pLWS, "FramesPerSecond", 15))
		{ 
			pLWS += 16;
			fps = (unsigned char)cStr.a2f(pLWS);
		}
		else if (!cStr.qstrncmp(pLWS, "LoadObject", 10))
		{ 
			centity++;		// camera is entry 0
			nbobj++;

			// perform loading of the entity
			if (FindObjectFile(pLWS, this))
			{
				AddObjectFromLWS(centity);

				//	Any lightmap for this object?
				//	Lightmaps are calculated only for objects
				//	that require shading.
				if ( useslightmap && *pUV++ )
				{
					pUV = entityadress[centity]->GetUVLightMap(centity, str, pUV, &xLkLmap);
					if (pUV == NULL)
					{
						pnX->Verbose("    ERROR: LIGHTMAP LOAD SEQUENCE CAUGHT EXCEPTION!, lightmaps disabled...");
						useslightmap = false;
					}
				}
			}
			else
			{
				wsprintf(str, "    ERROR: Could not load '%s'!, nX_Entity::LoadLWO(); failed!", objfile);
				pnX->Verbose(str);
				if (useslightmap)
				{
					pnX->Verbose("    ERROR: LIGHTMAP LOAD SEQUENCE CAUGHT EXCEPTION!, lightmaps disabled...");
					useslightmap = false;
				}
				AddNullObjectFromLWS(centity);
			}
		}
		else if (!cStr.qstrncmp(pLWS, "AddNullObject", 13))
		{ 
			pUV++;
			centity++;			// camera is entry 0
			nbobj++;
			AddNullObjectFromLWS(centity);
		}
		else if (!cStr.qstrncmp(pLWS, "CameraMotion", 12))

			AddCameraFromLWS();

		else if (!cStr.qstrncmp(pLWS, "BackdropColor", 13))
		{
			pLWS += 14;

			backdropcolor.r = (uchar) ( cStr.a2f(pLWS) );
			while (*pLWS++ != 32);
			backdropcolor.g = (uchar) ( cStr.a2f(pLWS) );
			while (*pLWS++ != 32);
			backdropcolor.b = (uchar) ( cStr.a2f(pLWS) );
		}

		// BG image

		else if (!cStr.qstrncmp(pLWS, "BGImage", 7))
		{
			pLWS += 8;

			char	name[256], *pname = name;
			while (pLWS[0] != 0x0d && pLWS[1] != 0x0a)
				*pname++ = *pLWS++;
			pname[0] = 0;
			MakeComply2Path(name);

			// create the surface and load the map
			BGsurf = new nX_Surface(pnX);

			if ( BGsurf->QuickSetup(name, nX_MAPPED, nX_WIDEMAP | nX_NOZWRITE | nX_BACKGROUND) == nX_OK )
			{
				BGsurf->name = new char[7];
				strcpy(BGsurf->name, "BGsurf");
				BGmap = true;
			}
			else
			{
				pnX->Verbose("    ERROR: Creating/Loading BG Surface!");
				delete BGsurf;
				BGsurf = NULL;
			}
		}

		// glow FX

		else if (!cStr.qstrncmp(pLWS, "GlowEffect", 10))
		{
			pLWS += 11;
			glow = true;
		}
		else if (!cStr.qstrncmp(pLWS, "GlowIntensity", 13))
		{
			pLWS += 14;
//			iglow = cStr.a2f((const char *)pLWS);
		}
		else if (!cStr.qstrncmp(pLWS, "GlowRadius", 10))
		{
			pLWS += 11;
//			glowradius = cStr.a2f((const char *)pLWS);
		}

		// handle scene' fog

		else if (!cStr.qstrncmp(pLWS, "FogType", 7))
		{
			pLWS += 8;
			fogtype = (uchar)cStr.a2f(pLWS);

			if (fogtype)
				fog = true;
		}
		else if (!cStr.qstrncmp(pLWS, "FogColor", 8))
		{
			pLWS += 9;
			uchar	R = (uchar) (cStr.a2f(pLWS));
			while (*pLWS++ != 32);
			uchar	G = (uchar) (cStr.a2f(pLWS));
			while (*pLWS++ != 32);
			uchar	B = (uchar) (cStr.a2f(pLWS));
			fogcolorRGB = (R << 16) + (G << 8) + B;
			fogcolorR = R; fogcolorG = G; fogcolorB = B;
		}
		else if (!cStr.qstrncmp(pLWS, "FogMinDist", 10))
		{
			pLWS += 11;

			FogMinDist = new nX_Envelop;

			if (pLWS[0]=='(')
				pLWS = FogMinDist->LoadLWSMotion(pLWS);
			else
				FogMinDist->values[0] = cStr.a2f(pLWS);
		}
		else if (!cStr.qstrncmp(pLWS, "FogMaxDist", 10))
		{
			pLWS += 11;
			FogMaxDist = new nX_Envelop;

			if (pLWS[0]=='(')
				pLWS = FogMaxDist->LoadLWSMotion(pLWS);
			else
				FogMaxDist->values[0] = cStr.a2f(pLWS);
		}
		else if (!cStr.qstrncmp(pLWS, "GridSize", 8))
		{
			pLWS += 9;
			mulfactor = 30.0f / cStr.a2f(pLWS);
			wsprintf(str, "    Basing MULFACTOR on GridSize = %d(x100000)", (long)(mulfactor * 100000.0));
			pnX->Verbose(str);
		}
		else
		{
			while (pLWS[0] != 0xd && pLWS[1] != 0xa)
				pLWS++;
			pLWS += 2;
		}

		// Update progress percent
		pnX->SetProgress( char( float (pLWS - this->lwsadress) / filesize * 100.0 ) );
	}

	// We've done that...
	pnX->SetProgress(-1);

	nbentity = centity;		// centity was pre-incremented so zero-index it

	if (pfirstframe == -1)
		firstframe = rfirstframe;
	else
		firstframe = pfirstframe;
	if (plastframe == -1)
		lastframe = rlastframe;
	else
		lastframe = plastframe;

	// MAPPING Coordinate computation -----------------------------------------
	///////////////////////////////////////////////////////////////////////////

	long		c;
	nX_Entity	*entity, *entity2;

	for	(c = 1; c <= nbobj; c++)
		entityadress[c]->GetUV();
	
	if (useslightmap)
		xLkLmap.ClosexLink();
	if (UVset)
		delete UVset;

	// RESIZE WORLD -----------------------------------------------------------
	///////////////////////////////////////////////////////////////////////////

	Resize(mulfactor);

	// METAMORPHING coefs precalculation --------------------------------------
	///////////////////////////////////////////////////////////////////////////

	for (c = 1; c <= nbobj; c++)
	{
		entity = entityadress[c];

		if (entity->wave)
			entity->GetWaveGradients();

		if (entity->Morph)
		{

			if (!entity->morphtarget)
			{
				wsprintf(str, "    WARNING: Morph with no target! Object %s.", entity->file);
				pnX->Verbose(str);
				delete	entity->Morph;
				entity->Morph = NULL;
			}
			else if (entity->stdenormal != entityadress[entity->morphtarget]->stdenormal)
			{
				wsprintf(str, "    WARNING: Morphing object with different normal types (STD-SMOOTH)! Object %s.", entity->file);
				pnX->Verbose(str);
				delete	entity->Morph;
				entity->Morph = NULL;
			}
			else
			{
				entity2 = entityadress[entity->morphtarget];
				entity->GetMorphGradient(entity2, &entity->xyzmorphdeltas, &entity->nedmorphdeltas, &entity->nfamorphdeltas);
			}
		}
	}

	// ALLOCATE METAGRID if any -----------------------------------------------
	///////////////////////////////////////////////////////////////////////////

	if (hasmetaobjects)
	{
		metagrid = new nX_metagrid;

		// and setup
		metagrid->Setup(40, 40, 40, 1.0);			// 20x20x20 grid (unit 1)
		metagrid->Reset();							// Reset metagrid force field
	}

	// SURFACES Review --------------------------------------------------------
	///////////////////////////////////////////////////////////////////////////

	surface = LLsurface;
	bool		multitex = false;

	for (c = 0; c < nbsurface; c++)
	{

		// compute the number of textures the surface use

		if (surface->CAPS & nX_MAPPED)
			surface->texcount++;
		if (surface->CAPS & nX_EVMAP)
			surface->texcount++;
		if (surface->CAPS & nX_ALPHATEX)
			surface->texcount++;

		surface = surface->LLnext;

	}

	// RESOLVE PARENT/TARGET/GOAL DIRECT LINK ---------------------------------
	///////////////////////////////////////////////////////////////////////////


	nX_Entity	*pEnt, *pPar;

	for (c = 0; c <= nbentity; c++)
	{
		pEnt = entityadress[c];
		pEnt->staticinscene = true;
		if (pEnt->iparent > 0)
			pEnt->dparent = entityadress[pEnt->iparent];
		if (pEnt->itarget > 0)
			pEnt->dtarget = entityadress[pEnt->itarget];
		if (pEnt->igoal > 0)
			pEnt->dgoal = entityadress[pEnt->igoal];
	}

	// check staticinscene flag/allocate previous buffer in case of LM

	for (c = 0; c <= nbentity; c++)
	{
		pEnt = entityadress[c];
		pPar = pEnt;

		while (pPar && pEnt->staticinscene)
		{
			if (pPar->LWmotion.nbKF > 1)
			{
				pEnt->staticinscene = false;
				pPar->staticinscene = false;
				break;
			}
			pPar = pPar->dparent;
		}
		if ( (pEnt->Morph) && (pEnt->Morph->useenvelop) )
			pEnt->staticinscene = false;
		if (    pEnt->wave 
			 || pEnt->usemdd)
			pEnt->staticinscene = false;

		if (pEnt->staticinscene || bComputeLM)		// LIGHTMAPS calculus require that previous buffer are allocated!
		{
			// If object is static then we'll optimize that... (:
			nbstatic++;
			pEnt->previous_edge = new float[pEnt->nbpoints * 3];
			pEnt->previous_edgenormals = new float[pEnt->totalglobaledgenumber * 3];
			pEnt->previous_facenormals = new float[pEnt->nbface * 3];
		}
	}

	if (pnX->ForceAllNormals)
	{
		if (!pEnt->previous_edge)
			pEnt->previous_edge = new float[pEnt->nbpoints * 3];
		if (!pEnt->previous_edgenormals)
			pEnt->previous_edgenormals = new float[pEnt->totalglobaledgenumber * 3];
		if (!pEnt->previous_facenormals)
			pEnt->previous_facenormals = new float[pEnt->nbface * 3];
	}


	// MARK ENTITIES BELONGING TO AN IK CHAIN ---------------------------------
	///////////////////////////////////////////////////////////////////////////
/*
	IK_Node	*IKNode;
	IKNode = LLIK;

	while (IKNode)
	{
		pEnt = IKNode->IKsrc;
		while (pEnt && !pEnt->IKAnchor)
		{
			pEnt->inIKchain = true;
			IKNode->top = pEnt;
			pEnt = pEnt->dparent;
		}
		IKNode = IKNode->LLnext;
	}
*/
	// SCENE DYNAMIC MEMORY ALLOCATION ----------------------------------------
	///////////////////////////////////////////////////////////////////////////

	unsigned short	biggestone = 0;
	unsigned long	biggestglobaledge = 0;
	totalface = 0;
	totaledge = 0;

	for (c = 1; c <= nbobj; c++)
	{
		entity = entityadress[c];

		// reserve trail polygons...
#ifdef	nX_ENABLETRAIL
		entity->ComputeTrail();
#endif

		if (entity->totalglobaledgenumber > biggestglobaledge)
			biggestglobaledge = entity->totalglobaledgenumber;

		if (entity->nbpoints > biggestone)
			biggestone = entity->nbpoints;

		totalface += entity->nbface;
		totaltri += entity->nbtri;
		totaledge += (short)entity->nbpoints;
		ngvtxface += entity->totalglobaledgenumber;
	}

	nedgemaxobj = biggestone;
	nglobedgemaxobj = biggestglobaledge;
	loadtime = GetTickCount() - loadtime;
	loaded = true;

	// output stats to console
	wsprintf(str, "    %d vertice/%d face(s)/%d object(s)/%d light(s)", totaledge, totalface, nbobj, nblight);
	pnX->Verbose(str);
	wsprintf(str, "    %d texture(s)/%d sequence(s)/%d surface(s)/%d static(s)", nbtexmap, nbsequence, nbsurface, nbstatic);
	pnX->Verbose(str);

	Reset(true, NULL);

	// we're done!
	delete nX_opLWS;
	return true;

}


///////////////////////////////////////////////////////////////////////////////
void	nX_Scene::MakeComply2Path(char *filename)
///////////////////////////////////////////////////////////////////////////////
{
	char	correct[256];
	long	pos;

	if (!pnX->LWpath)
		return;
	if (filename[1] != ':')
	{
		strcpy(correct, pnX->LWpath);
		pos = strlen(correct)-1;
		strcat(&correct[pos], filename);
		strcpy(filename, correct);
	}
}


///////////////////////////////////////////////////////////////////////////////
void	nX_Scene::ProcessPNTS(nX_Entity *entity)
///////////////////////////////////////////////////////////////////////////////
{

	char	*XWO;

	// size of chunk
	long	sochunk = lMC.GetLong(pLWO + 4);
	char	*eochunk = pLWO + lMC.GetLong(pLWO + 4) + 8;
	pLWO += 8;

	// fill entity structure
	XWO = new char[sochunk + 8 * 4 * 3];
	entity->edge_adress = (float *)XWO;
	entity->nbpoints = sochunk / 12;

	// find bounding box...
	float	bbminx, bbminy, bbminz;
	float	bbmaxx, bbmaxy, bbmaxz;
	bbminx = bbminy = bbminz = FLT_MAX;
	bbmaxx = bbmaxy = bbmaxz = FLT_MIN;

	// now, convert the chunk
	while (pLWO != eochunk)
	{
		*(long *)XWO = lMC.GetLong(pLWO);
		if (*(float *)XWO > bbmaxx)
			bbmaxx = *(float *)XWO;
		if (*(float *)XWO < bbminx)
			bbminx = *(float *)XWO;
		*(long *)(XWO + 4) = lMC.GetLong(pLWO + 4);
		*(float *)(XWO + 4) *= -1;
		if (*(float *)(XWO + 4) > bbmaxy)
			bbmaxy = *(float *)(XWO + 4);
		if (*(float *)(XWO + 4) < bbminy)
			bbminy = *(float *)(XWO + 4);
		*(long *)(XWO + 8) = lMC.GetLong(pLWO + 8);
		if (*(float *)(XWO + 8) > bbmaxz)
			bbmaxz = *(float *)(XWO + 8);
		if (*(float *)(XWO + 8) < bbminz)
			bbminz = *(float *)(XWO + 8);
		pLWO += 12;
		XWO += 12;
	}

	// get bbox
	entity->BBmin.x = bbminx;
	entity->BBmin.y = bbminy;
	entity->BBmin.z = bbminz;
	entity->BBmax.x = bbmaxx;
	entity->BBmax.y = bbmaxy;
	entity->BBmax.z = bbmaxz;

	// add bounding box to entity
	entity->nbpoints += 8;
	*(float *)(XWO) = bbminx; *(float *)(XWO + 4) = bbminy; *(float *)(XWO + 8) = bbminz; XWO += 12;
	*(float *)(XWO) = bbmaxx; *(float *)(XWO + 4) = bbminy; *(float *)(XWO + 8) = bbminz; XWO += 12;
	*(float *)(XWO) = bbminx; *(float *)(XWO + 4) = bbmaxy; *(float *)(XWO + 8) = bbminz; XWO += 12;
	*(float *)(XWO) = bbmaxx; *(float *)(XWO + 4) = bbmaxy; *(float *)(XWO + 8) = bbminz; XWO += 12;
	*(float *)(XWO) = bbminx; *(float *)(XWO + 4) = bbminy; *(float *)(XWO + 8) = bbmaxz; XWO += 12;
	*(float *)(XWO) = bbmaxx; *(float *)(XWO + 4) = bbminy; *(float *)(XWO + 8) = bbmaxz; XWO += 12;
	*(float *)(XWO) = bbminx; *(float *)(XWO + 4) = bbmaxy; *(float *)(XWO + 8) = bbmaxz; XWO += 12;
	*(float *)(XWO) = bbmaxx; *(float *)(XWO + 4) = bbmaxy; *(float *)(XWO + 8) = bbmaxz; 
	// ensure we'll not waste time
	pLWO = eochunk;

}