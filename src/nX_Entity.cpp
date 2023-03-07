	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		nXng Entity implementation file

	----------------------------------------------------------------------------*/


	#include	<math.h>
	#include	"nX_HeaderList.h"

	#define		M_PI			3.1415926597
	#define		HALFPI			(M_PI * 2)
	static		char			str[256];
	static		xLinker			xPK;
	extern		unsigned char	mulshader[65536];


///////////////////////////////////////////////////////////////////////////////
nX_Entity::~nX_Entity()
///////////////////////////////////////////////////////////////////////////////
{
	
	long		c;

	// filename
	if (file)
		delete file;

	// free extra envelops
	if (Dissolve)
		delete Dissolve;
	if (LightRange)
		delete LightRange;
	if (LightInt)
		delete LightInt;
	if (ConeAngle)
		delete ConeAngle;
	if (EdgeAngle)
		delete EdgeAngle;
	if (PolySize)
		delete PolySize;
	if (Morph)
	{
		delete Morph;
		delete xyzmorphdeltas;
		if ((requirement & nX_USEEDGENORMAL))
			delete nedmorphdeltas;
		if ((requirement & nX_USEFACENORMAL))
			delete nfamorphdeltas;
	}

	// free polygon areas
	if (polydeltas)
		delete(polydeltas);
	if (faceplanes)
		delete(faceplanes);

	// free edge buffers
	if (edge_adress && !usemdd)
		delete(edge_adress);
	if (previous_edge)
		delete(previous_edge);

	// free normal buffers
	if (ofacenormals)
		delete(ofacenormals);
	if (oedgenormals)
		delete(oedgenormals);
	if (previous_facenormals)
		delete(previous_facenormals);
	if (previous_edgenormals)
		delete(previous_edgenormals);

	// free UV mapping coordinates
	for (c = 0; c < nX_MAXTEXLEVEL; c++)
		if (UVmap[c])
			delete(UVmap[c]);
	if (lmaplist)
		delete lmaplist;

	// eventual custom flare surface
	if (flaresurface && (flaresurface != pnX->DefaultFlareSurface) )
	{
		delete flaresurface;
		flaresurface = NULL;
	}

	// destroy the spark emiter
	if (sparkemiter)
	{
		delete	sparkemiter->sparklist;
		delete	sparkemiter;
	}

	// per frame buffer
	if (perframenormalcalc)
		delete (qfacefromedge);

	// destroy metaobjects
	if (metaball)
		delete metaball;

	// erase [WAVE] buffers
	if (pLong)
		delete pLong;
	if (pLat)
		delete pLat;
	if (oMesh)
		delete oMesh;

	// erase MDPlug buffers
	if (usemdd)
	{
		for (c = 0; c < nMDstep; c++)
		{
			if (MD[c].vertice)
				delete (MD[c].vertice);
			if (MD[c].enorm)
				delete (MD[c].enorm);
			if (MD[c].fnorm)
				delete (MD[c].fnorm);
			if (MD[c].blend)
				delete (MD[c].blend);
			if (MD[c].benorm)
				delete (MD[c].benorm);
			if (MD[c].bfnorm)
				delete (MD[c].bfnorm);
		}
		delete (MD);
	}

	// erase trails
	#ifdef	nX_ENABLETRAIL
		FreeTrail();
	#endif

	// free TSP
	if (TSP)
		pnX->FreeTSPNode(TSP);

	// last few bytes left to trash...
	delete reliure_adress;
	delete indexlist;

}


///////////////////////////////////////////////////////////////////////////////
void	nX_Entity::GetLWMotion(float frame)
///////////////////////////////////////////////////////////////////////////////
{

		// get motion for current frame
		LWmotion.GetValues(frame);

		// entity motion
		offset[0] = LWmotion.values[0];
		offset[1] = LWmotion.values[1];
		offset[2] = LWmotion.values[2];
		angle[0] = -LWmotion.values[4] * (float)(digitPI / 180.0f);
		angle[1] = LWmotion.values[3] * (float)(digitPI / 180.0f);
		angle[2] = -LWmotion.values[5] * (float)(digitPI / 180.0f);
		scale[0] = LWmotion.values[6];
		scale[1] = LWmotion.values[7];
		scale[2] = LWmotion.values[8];

}


///////////////////////////////////////////////////////////////////////////////
void	nX_Entity::GetExtraLWMotions(float frame)
///////////////////////////////////////////////////////////////////////////////
{	

	// DISSOLVE
	if (Dissolve)
	{
		Dissolve->GetValues(frame);
		if (Dissolve->values[0] <= 0)
			Dissolve->values[0] = 0;
		else if (Dissolve->values[0] >= 1)
			Dissolve->values[0] = 1;
	}

	// MORPH
	if (Morph)
		Morph->GetValues(frame);

	// POLYGON SIZE
	if (PolySize)
		PolySize->GetValues(frame);

	// LIGHT SPECIFIC
	if (type == nXETYPE_LIGHT)
	{
		if (lighttype == 2)
		{
			if (ConeAngle && ConeAngle->useenvelop)
			{
				ConeAngle->GetValues(frame);
				if (ConeAngle->values[0] < 0) ConeAngle->values[0] = 0;
//				ConeAngle->values[0] *= digitPI / 180.0;
//				ConeAngle->values[0] = cos(ConeAngle->values[0]);
				ConeAngle->values[0] = pnX->qCos[degsincos(ConeAngle->values[0])];
			}
			if (EdgeAngle && EdgeAngle->useenvelop)
			{
				EdgeAngle->GetValues(frame);
	
				if (EdgeAngle->values[0] > ConeAngle->values[0])
					EdgeAngle->values[0] = ConeAngle->values[0] - 0.1f;
				if (EdgeAngle->values[0] < 0)
					EdgeAngle->values[0] = 0;
//				EdgeAngle->values[0] *= digitPI / 180.0;
				EdgeAngle->values[0] = 1 - pnX->qCos[degsincos(EdgeAngle->values[0])];
			}
		}

		LightInt->GetValues(frame);
		if (LightInt->useenvelop)
		{
			if (LightInt->values[0] < 0)
				LightInt->values[0] = 0;
			ilightcolor.r = mulshader[lightcolor.r * 256 + char(LightInt->values[0] * 255)];
			ilightcolor.g = mulshader[lightcolor.g * 256 + char(LightInt->values[0] * 255)];
			ilightcolor.b = mulshader[lightcolor.b * 256 + char(LightInt->values[0] * 255)];
		}
		if (LightRange)
		{
			LightRange->GetValues(frame);
			if (LightRange->values[0] < 0)
				LightRange->values[0] = 0;
		}
		if (FlareInt)
			FlareInt->GetValues(frame);
	}

}


///////////////////////////////////////////////////////////////////////////////
void	nX_Entity::GetFaceNormals()
///////////////////////////////////////////////////////////////////////////////
{

	if (!facenormals_adress)
		facenormals_adress = new float[nbface * 3];

	float	normx, normy, normz, normsize;
	float	*c2d = edge_adress;
	float	*pfacenorm = facenormals_adress;
	nX_reliure	*preliure = reliure_adress;

	for (ulong c = 0; c < nbface; c++)
	{
		ulong	nedge = preliure->nedge - 1;
		ulong	*pindex = preliure->index;

		if (nedge > 1)
		{
			normx=( c2d[pindex[1] + 1] - c2d[pindex[0] + 1] ) * ( c2d[pindex[2] + 2] - c2d[pindex[0] + 2] ) -
				  ( c2d[pindex[1] + 2] - c2d[pindex[0] + 2] ) * ( c2d[pindex[2] + 1] - c2d[pindex[0] + 1] );
			normy=( c2d[pindex[1] + 2] - c2d[pindex[0] + 2] ) * ( c2d[pindex[2] + 0] - c2d[pindex[0] + 0] ) -
				  ( c2d[pindex[1] + 0] - c2d[pindex[0] + 0] ) * ( c2d[pindex[2] + 2] - c2d[pindex[0] + 2] );
			normz=( c2d[pindex[1] + 0] - c2d[pindex[0] + 0] ) * ( c2d[pindex[2] + 1] - c2d[pindex[0] + 1] ) -
				  ( c2d[pindex[1] + 1] - c2d[pindex[0] + 1] ) * ( c2d[pindex[2] + 0] - c2d[pindex[0] + 0] );
			normsize = (float)sqrt( normx * normx + normy * normy + normz * normz );
		}
		else
			// Dum normal that will always produce fully lit lightnings...
		{	normx = 1; normy = 1; normz = 1; normsize = 1;	}

		if (normsize)
		{
			normx /= normsize;
			pfacenorm[0] = normx;
			normy /= normsize;
			pfacenorm[1] = normy;
			normz /= normsize;
			pfacenorm[2] = normz;
		}
		else
		{
			pfacenorm[0] = 0;
			pfacenorm[1] = 0;
			pfacenorm[2] = 0;
		}

		pfacenorm += 3;
		preliure++;
	}

}


///////////////////////////////////////////////////////////////////////////////
void	nX_Entity::GetEdgeNormals()
//	ARRAY 'edgenormal_adress' MUST HAVE BEEN ALLOCATED!!!
//	Call this to compute edge normal of entity.
//	The normals are calculated using a
//	MaxSmoothingAngle but a trick is used so
//	it take less time than 'classic' edge normal
//	calculations (; ! it's true yep...
///////////////////////////////////////////////////////////////////////////////
{

	nX_reliure		*preliure;
	ulong			*pindex;
	long			c;
	unsigned long	c2;

	long	index, nindex, nedge;
	float	nbnorm;

	// build the lookup table. cannot retrieve more than 8 faces from an edge!
	long	*pfacefromedge;
	long	*facefromedge;

	if (!ffeuptodate)
	{
		qfacefromedge = new long[nbpoints * 8];
		facefromedge = qfacefromedge;
		memset(facefromedge, 0, nbpoints * 8 * 4);
		preliure = reliure_adress;
		ffeuptodate = true;

		for (c2 = 0; c2 < nbface; c2++)
		{
			nedge = preliure->nedge;
			pindex = preliure->index;

			for (c = 0; c < nedge; c++)
			{
				index = 0;
				pfacefromedge = facefromedge + (*pindex++ / 3) * 8;

				while (index < 8)
				{
					if (pfacefromedge[0] == 0)
					{
						pfacefromedge[0] = c2 + 1;
						break;		// store index of face...
					}
					index++;
					pfacefromedge++;
				}
			}
			preliure++;
		}
	}
	facefromedge = qfacefromedge;

	// now to the real computing
	nX_Surface	*surface;
	float		*pfacenorm = facenormals_adress;
	float		*pedgenorm = edgenormals_adress;
	float		normx, normy, normz;
	preliure = reliure_adress;

	if (stdenormal)

	// Standard version
	{
		for (c = 0; c < nbpoints; c++)
		{
			normx = 0; normy = 0; normz = 0; nbnorm = 0; index = 0;
			pfacefromedge = facefromedge + c * 8;

			while (index < 8)
			{
				// check if we processed every face coming trough this edge
				if (!pfacefromedge[0])
					break;

				// add face normals to edge's one... if match smoothing angle...
				nindex = (pfacefromedge[0] - 1) * 3;

				normx += facenormals_adress[nindex + 0];
				normy += facenormals_adress[nindex + 1];
				normz += facenormals_adress[nindex + 2];
				nbnorm++;
				
				index++;
				pfacefromedge++;
			}

			// make it one!
			if (nbnorm > 1)
			{
				pedgenorm[0] = normx / nbnorm;
				pedgenorm[1] = normy / nbnorm;
				pedgenorm[2] = normz / nbnorm;
			}
			else
			{
				pedgenorm[0] = normx;
				pedgenorm[1] = normy;
				pedgenorm[2] = normz;
			}
			pedgenorm += 3;
		}
	}
	else

	// Smoothing angle version
	{
		for (c2 = 0; c2 < nbface; c2++)
		{
			nedge = preliure->nedge;
			surface = preliure->surf;
			pindex = preliure->index;

			if (surface->requirement & nX_USEEDGENORMAL)
			{
				for (c = 0; c < nedge; c++)
				{
					normx = 0; normy = 0; normz = 0; nbnorm = 0; index = 0;
					pfacefromedge = facefromedge + (pindex[c] / 3) * 8;

					while (index < 8)
					{
						// check if we processed every face coming trough this edge
						if (!pfacefromedge[0])
							break;

						// add face normals to edge's one... if match smoothing angle...
						nindex = (pfacefromedge[0] - 1) * 3;

						if ((facenormals_adress[nindex + 0] * pfacenorm[0] +
							 facenormals_adress[nindex + 1] * pfacenorm[1] +
							 facenormals_adress[nindex + 2] * pfacenorm[2]) >
							 surface->maxsmoothingangle)
						{
							normx += facenormals_adress[nindex + 0];
							normy += facenormals_adress[nindex + 1];
							normz += facenormals_adress[nindex + 2];
							nbnorm++;
						}
					
						index++;
						pfacefromedge++;
					}

					// make it one!
					if (nbnorm > 1)
					{
						pedgenorm[0] = normx / nbnorm;
						pedgenorm[1] = normy / nbnorm;
						pedgenorm[2] = normz / nbnorm;
					}
					else
					{
						pedgenorm[0] = normx;
						pedgenorm[1] = normy;
						pedgenorm[2] = normz;
					}
					pedgenorm += 3;
				}
			}
			else
				pedgenorm += nedge * 3;

			// next face
			preliure++;
			pfacenorm += 3;
		}
	}

	if (!perframenormalcalc)
	{
		delete (qfacefromedge);
		qfacefromedge = NULL;
		ffeuptodate = false;
	}

}


///////////////////////////////////////////////////////////////////////////////
nX_Entity::nX_Entity(nX *creator)
///////////////////////////////////////////////////////////////////////////////
{

	memset(this, 0, sizeof(nX_Entity));

	// i know my god... my creator
	pnX = creator;

	// set FXs default values
	PolySize = new nX_Envelop();
	PolySize->values[0] = 1.0;

}


///////////////////////////////////////////////////////////////////////////////
void	nX_Entity::GetMorphGradient(nX_Entity *entity2, float **pcd, float **pne, float **pnf)
///////////////////////////////////////////////////////////////////////////////
{
	unsigned long	c;
	float			*poobj, *pdobj, *poned, *pdned, *ponfa, *pdnfa;
	float			*pxyzdeltas, *pneddeltas, *pnfadeltas;

	// allocate memory to store xyz morphing coefficients
	pxyzdeltas = new float[nbpoints * 3];
	*pcd = pxyzdeltas;

	if (requirement & nX_USEEDGENORMAL)
	{
		pneddeltas = new float[totalglobaledgenumber * 3];
		*pne = pneddeltas;

		if (!entity2->edgenormals_adress)
		{
			if (!entity2->facenormals_adress)
			{
				entity2->GetFaceNormals();
				entity2->ofacenormals = entity2->facenormals_adress;
			}
			entity2->stdenormal = stdenormal;
			if (entity2->stdenormal)
					entity2->oedgenormals = entity2->edgenormals_adress = new float[entity2->nbpoints * 3];
			else	entity2->oedgenormals = entity2->edgenormals_adress = new float[entity2->totalglobaledgenumber * 3];
			entity2->GetEdgeNormals();
		}
	}

	if (requirement & nX_USEFACENORMAL)
	{
		if (!entity2->facenormals_adress)
		{
			entity2->GetFaceNormals();
			entity2->ofacenormals = entity2->facenormals_adress;
		}

		pnfadeltas = new float[nbface * 3];
		*pnf = pnfadeltas;
		ponfa = facenormals_adress;
		pdnfa = entity2->facenormals_adress;

		// compute gradients face normals

		for (c = 0; c < nbface * 3; c++)
			*pnfadeltas++ = *pdnfa++ - *ponfa++;
	}

	// we have as many xyz/normaledge deltas as edges in object
	poobj = edge_adress;
	pdobj = entity2->edge_adress;
	poned = edgenormals_adress;
	pdned = entity2->edgenormals_adress;

	for (c = 0; c < unsigned long(nbpoints * 3); c++)
		*pxyzdeltas++ = *pdobj++ - *poobj++;

	if (requirement & nX_USEEDGENORMAL)
		for (c = 0; c < totalglobaledgenumber * 3; c++)
			*pneddeltas++ = *pdned++ - *poned++;
}


///////////////////////////////////////////////////////////////////////////////
float inline xyztoh(float x, float y, float z)
///////////////////////////////////////////////////////////////////////////////
{
	float h;

	if (x==0.0 && z==0.0)
		h=0.0;
	else
	{
		if (z==0.0)
			h=(float)((x<0.0) ? HALFPI:-HALFPI);
		else
			if (z<0.0)
			h=(float)(-atan(x/z)+M_PI);
		else
			h=(float)(-atan(x/z));
	}

	return h;
}


///////////////////////////////////////////////////////////////////////////////
void inline xyztohp(float x, float y, float z, float *h, float *p)
///////////////////////////////////////////////////////////////////////////////
{
	if (x==0.0 && z==0.0)
	{
		h[0]=0.0;
		if (y!=0.0)
			p[0]=(float)((y < 0.0) ? -HALFPI:HALFPI);
		else
			p[0]=(float)0.0;
	}
	else
	{
		if (z==0.0)
			h[0]=(float)((x<0.0) ? HALFPI:-HALFPI);
		else
			if (z<0.0)
				h[0]=(float)(-atan(x/z) + M_PI);
		else
			h[0]=(float)(-atan(x/z));

		x=(float)(sqrt(x*x+z*z));
		if (x==0.0)
			p[0]=(float)((y<0.0) ? -HALFPI:HALFPI);
		else
			p[0]=(float)(atan(y/x));
	}
}


///////////////////////////////////////////////////////////////////////////////
unsigned char *nX_Entity::GetUVLightMap(long index, char *lmapfile, unsigned char *pUV, xLinker *xPK)
///////////////////////////////////////////////////////////////////////////////
{

	// Variables
	unsigned long	c, c2, nedge;
	float			*pmapc;

	// Perform UV reading
	nX_reliure		*preliure = reliure_adress;
	nX_Surface		*surface;
	ulong			*pindex;

	// now compute UV maps for LightMaps
	pmapc = new float[totalglobaledgenumber * 2];
	UVmap[nXML_LIGHTMAP] = pmapc;
	lmaplist = new nX_LightMapInfo[nbface];
	nX_LightMapInfo	*plmaplist = lmaplist;
	unsigned long vLMAP, cLMAP;
	char	fname[256];
	unsigned short	tmpuc;

	// Do not update texlevel infos!
	//texlevel[nXML_LIGHTMAP] = true;

	// go trough all entity' faces
	for (c = 0; c < nbface; c++)
	{
		vLMAP = *pUV++;							// Lightmap for this face?
		nedge = preliure->nedge;
		surface = preliure->surf;
		pindex = preliure->index;

		switch (vLMAP)
		{

			case 0:			// NO LIGHTMAPPING INFORMATIONS

				plmaplist->lightmapped = 0;
				break;

			case 1:			// TEXTURED LIGHTMAP

				plmaplist->lightmapped = 1;
				cLMAP = *(unsigned short *)pUV;
				pUV += 2;

				// Get lightmap texture name and load it
				wsprintf(fname, "c:\\lm%d-%d.jpg", index, cLMAP);
				plmaplist->lmap = scene->LoadAndRegisterTexMap(fname, xPK);
				if (plmaplist->lmap == NULL)
					return NULL;
				plmaplist->lmap->AddRef();

				// Quads are mapped as {0,0;1,0;1,1;0,1} whatever happens
				for (c2 = 0; c2 < nedge; c2++)
				{
					tmpuc = *(unsigned short *)pUV;
					pUV += 2;
					pmapc[c2 * 2 + 0] = float(tmpuc) / 64000.0f;
					tmpuc = *(unsigned short *)pUV;
					pUV += 2;
					pmapc[c2 * 2 + 1] = float(tmpuc) / 64000.0f;
				}
				break;

			case 2:			// TEXTURED SHADOW

				plmaplist->lightmapped = 2;
				cLMAP = *(unsigned short *)pUV;
				pUV += 2;

				// Get lightmap texture name and load it

				wsprintf(fname, "c:\\lm%d-%d.jpg", index, cLMAP);
				plmaplist->lmap = scene->LoadAndRegisterTexMap(fname, xPK);
				if (plmaplist->lmap == NULL)
					return NULL;
				plmaplist->lmap->AddRef();

				// Quads are mapped as {0,0;1,0;1,1;0,1} whatever happens

				for (c2 = 0; c2 < nedge; c2++)
				{
					tmpuc = *(unsigned short *)pUV;
					pUV += 2;
					pmapc[c2 * 2 + 0] = float(tmpuc) / 64000.0f;
					tmpuc = *(unsigned short *)pUV;
					pUV += 2;
					pmapc[c2 * 2 + 1] = float(tmpuc) / 64000.0f;
				}
				break;

			case 3:			// LIGHT SHADED

				plmaplist->lightmapped = 3;
				plmaplist->shade.r = pUV[0];
				plmaplist->shade.g = pUV[1];
				plmaplist->shade.b = pUV[2];
				pUV += 3;
				break;

		}
		preliure++;
		pmapc += nedge * 2;
		plmaplist++;
	}

	// We've done it!
	return pUV;
}


///////////////////////////////////////////////////////////////////////////////
void	nX_Entity::GetUV()
///////////////////////////////////////////////////////////////////////////////
{

	unsigned long	c, c2, c3, axis, nedge;
	float			*pedge, *pmapc, *ponfa;
	float			nx, ny, nz, s, t, lon, lat;

	nX_reliure		*preliure;
	nX_Surface		*surface;
	ulong			*pindex;

	// surface optimizer table
	long		CAPStowatch[nX_MAXTEXLEVEL];
	CAPStowatch[nXML_COLOR] = nX_MAPPED;
	CAPStowatch[nXML_TRANSPARENCY] = nX_ALPHATEX;

	// now compute UV maps for all texture levels

	for (c3 = 0; c3 < nX_MAXTEXLEVEL; c3++)
	{
		if (texlevel[c3] && c3 != nXML_EVMAP)
		{

			pmapc = new float[totalglobaledgenumber * 2];
			UVmap[c3] = pmapc;
			preliure = reliure_adress;

			// be sure we use the correct buffers...
			ponfa = facenormals_adress;
			pedge = edge_adress;

			// go trough all entity' faces
			for (c = 0; c < nbface; c++)
			{
				nedge = preliure->nedge;
				surface = preliure->surf;
				pindex = preliure->index;

				// this face requires mapping coords!
				if (surface->CAPS & CAPStowatch[c3])
				{
					// planar or cubic
					if (surface->mappingtype[c3] == 0 ||
						surface->mappingtype[c3] == 1)
					{

						if (surface->mappingtype[c3] == 1)
						{
							nx = float(fabs(ponfa[0]));
							ny = float(fabs(ponfa[1]));
							nz = float(fabs(ponfa[2]));

							// map axis
							if (nz >= nx && nz >= ny)
								axis = 4;
							else if (ny >= nx && ny >= nz)
								axis = 2;
							else
								axis = 1;
						}
						else
							axis = surface->mappingaxis[c3];

						// map the coordinates
						for (c2 = 0; c2 < nedge; c2++)
						{
							float x = pedge[pindex[c2] + 0] - surface->center[c3][0];
							float y = pedge[pindex[c2] + 1] - surface->center[c3][1];
							float z = pedge[pindex[c2] + 2] - surface->center[c3][2];

							if (axis == 1)
								s = z / surface->size[c3][2];
							else
								s = x / surface->size[c3][0];
							if (axis == 2)
								t = z / surface->size[c3][2];
							else
								t = y / surface->size[c3][1];

							pmapc[c2 * 2 + 0] = float(.5 + s);
							pmapc[c2 * 2 + 1] = float(.5 - t);
						}
					}

					// cylindrical
					else if (surface->mappingtype[c3] == 2)

						for (c2 = 0; c2 < nedge; c2++)
						{
							float x = pedge[pindex[c2] + 0] - surface->center[c3][0];
							float y = pedge[pindex[c2] + 1] - surface->center[c3][1];
							float z = pedge[pindex[c2] + 2] - surface->center[c3][2];

							if (surface->mappingaxis[c3] == 1)		// X
							{
								lon =xyztoh(z, x, y);
								t = float(-x / surface->size[c3][0] + .5);
							}
							else if (surface->mappingaxis[c3] == 2)	// Y
							{
								lon = xyztoh(-x, -y, z);
								t = float(-y / surface->size[c3][1] + .5);
							}
							else									// Z
							{
								lon = xyztoh(-x, z, y);
								t = float(-z / surface->size[c3][2] + .5);
							}

							lon = float(1. - lon / HALFPI);
							pmapc[c2 * 2 + 0] = lon;
							pmapc[c2 * 2 + 1] = t;
						}

					// spherical
					else if (surface->mappingtype[c3] == 3)

						for (c2 = 0; c2 < nedge; c2++)
						{
							float x = pedge[pindex[c2] + 0] - surface->center[c3][0];
							float y = pedge[pindex[c2] + 1] - surface->center[c3][1];
							float z = pedge[pindex[c2] + 2] - surface->center[c3][2];

							if (surface->mappingaxis[c3] == 1)		// X
								xyztohp(z, x, -y, &lon, &lat);
							if (surface->mappingaxis[c3] == 2)		// Y
								xyztohp(-x, y, z, &lon, &lat);
							else
								xyztohp(-x, z, -y, &lon, &lat);

							lon = float(1. - lon / HALFPI);
							lat = float(.5 - lat / HALFPI);
							pmapc[c2 * 2 + 0] = lon;
							pmapc[c2 * 2 + 1] = lat;
						}
				}

				preliure++;
				pmapc += nedge * 2;
				ponfa += 3;
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
void	nX_Entity::Reset()
///////////////////////////////////////////////////////////////////////////////
{
	
	// reset the MD
	if (usemdd)
	{
		cMD = MD;
		stepTC = 0;
		MDbstop = false;
	}

}


///////////////////////////////////////////////////////////////////////////////
void	nX_Entity::Resize(float s)
///////////////////////////////////////////////////////////////////////////////
{

	// Resize vertice
	float	*pvtx = edge_adress;

	for (long c = 0; c < nbpoints; c++)
	{
		pvtx[0] *= s;
		pvtx[1] *= s;
		pvtx[2] *= s;
		pvtx += 3;
	}
	pivot[0] *= s;
	pivot[1] *= s;
	pivot[2] *= s;

	// Resize motion
	KF	*pKF = LWmotion.KFadrss;

	if (pKF)
		for (c = 0; c < LWmotion.nbKF; c++)
		{
			pKF[c].cv[0] *= s;
			pKF[c].cv[1] *= s;
			pKF[c].cv[2] *= s;
		}

	// Resize MD datas
	long	cf, ce;
	float	*vtx;

	if (usemdd)

		for (cf = 0; cf < nMDstep; cf++)
		{
			vtx = MD[cf].vertice;
			for (ce = 0; ce < nbpoints * 3; ce++)
				(*vtx++) *= s;

			if (MD[cf].blend)	// we resize vertice but !!!NOT!!! normals factors!
			{
				vtx = MD[cf].blend;
				for (ce = 0; ce < (nbpoints - 8) * 3; ce++)
					(*vtx++) *= s;
			}
		}

	// extra CAPS to resize
	if (LightRange)
	{
		if (LightRange->useenvelop)	
		{
			pKF = LightRange->KFadrss;
			for (c = 0; c < LightRange->nbKF; c++)
				pKF[c].cv[0] *=s;
		}
		else
			LightRange->values[0] *= s;
	}

}


///////////////////////////////////////////////////////////////////////////////
void	nX_Entity::GetMatrix()
///////////////////////////////////////////////////////////////////////////////
{

	hmatrixvalid = false;

//pnX->qSin(
	float	aX = angle[0], aY = angle[1], aZ = angle[2];
	float	psx = -pivot[0] * scale[0],
			psy = pivot[1] * scale[1],
			psz = -pivot[2] * scale[2];
	rmatrix[0] = pnX->qCos[radsincos(aZ)] * pnX->qCos[radsincos(aY)] + pnX->qSin[radsincos(aZ)] * pnX->qSin[radsincos(aX)] * pnX->qSin[radsincos(aY)];
	rmatrix[1] = pnX->qSin[radsincos(aZ)] * pnX->qCos[radsincos(aX)];
	rmatrix[2] = pnX->qSin[radsincos(aZ)] * pnX->qSin[radsincos(aX)] * pnX->qCos[radsincos(aY)] - pnX->qCos[radsincos(aZ)] * pnX->qSin[radsincos(aY)];
	rmatrix[3] = pnX->qCos[radsincos(aZ)] * pnX->qSin[radsincos(aX)] * pnX->qSin[radsincos(aY)] - pnX->qSin[radsincos(aZ)] * pnX->qCos[radsincos(aY)];
	rmatrix[4] = pnX->qCos[radsincos(aZ)] * pnX->qCos[radsincos(aX)];
	rmatrix[5] = pnX->qCos[radsincos(aZ)] * pnX->qSin[radsincos(aX)] * pnX->qCos[radsincos(aY)] + pnX->qSin[radsincos(aZ)] * pnX->qSin[radsincos(aY)];
	rmatrix[6] = pnX->qCos[radsincos(aX)] * pnX->qSin[radsincos(aY)];
	rmatrix[7] =-pnX->qSin[radsincos(aX)];
	rmatrix[8] = pnX->qCos[radsincos(aX)] * pnX->qCos[radsincos(aY)];

	// mpivot * mscale * mrotate * mtranslate result matrix

	matrix[0] = scale[0] * rmatrix[0]; matrix[1] = scale[0] * rmatrix[1]; matrix[2] = scale[0] * rmatrix[2]; matrix[3] = 0;
	matrix[4] = scale[1] * rmatrix[3]; matrix[5] = scale[1] * rmatrix[4]; matrix[6] = scale[1] * rmatrix[5]; matrix[7] = 0;
	matrix[8] = scale[2] * rmatrix[6]; matrix[9] = scale[2] * rmatrix[7]; matrix[10] = scale[2] * rmatrix[8]; matrix[11] = 0;
	matrix[12] = psx * rmatrix[0] + psy * rmatrix[3] + psz * rmatrix[6] + offset[0];
	matrix[13] = psx * rmatrix[1] + psy * rmatrix[4] + psz * rmatrix[7] - offset[1];
	matrix[14] = psx * rmatrix[2] + psy * rmatrix[5] + psz * rmatrix[8] + offset[2];
	matrix[15] = 1;

}


///////////////////////////////////////////////////////////////////////////////
void inline MMul44(float *a, float *b, float *dst)
///////////////////////////////////////////////////////////////////////////////
{

	long	i, j, k;

	for (j = 0; j < 4; j++)
		for (i = 0; i < 4; i++)
		{
			dst[i + j * 4] = 0;
			for (k = 0; k < 4; k++)
				dst[i + j * 4] += a[k + j * 4] * b[k * 4 + i];
		}

}


///////////////////////////////////////////////////////////////////////////////
void inline MMul33(float *a, float *b, float *dst)
///////////////////////////////////////////////////////////////////////////////
{

	long	i, j, k;

	for (j = 0; j < 3; j++)
		for (i = 0; i < 3; i++)
		{
			dst[i + j * 3] = 0;
			for (k = 0; k < 3; k++)
				dst[i + j * 3] += a[k + j * 3] * b[k * 3 + i];
		}

}


///////////////////////////////////////////////////////////////////////////////
void	nX_Entity::GetHMatrix()
///////////////////////////////////////////////////////////////////////////////
{

	// perform hierarchy concatenation
	float	tmatrixA[16], tmatrixB[16];
	float	rmatrixA[9], rmatrixB[9];
	float	*pmA = tmatrixA, *pmB = tmatrixB, *pmT;
	float	*prA = rmatrixA, *prB = rmatrixB, *prT;
	memcpy(tmatrixA, matrix, 16 * sizeof(float));
	memcpy(rmatrixA, rmatrix, 9 * sizeof(float));

	nX_Entity	*hl = this;

	// perform full calculation unless they have already been done...
	while (hl->dparent)
	{
		if (hl->dparent->hmatrixvalid)
			break;

		MMul44(pmA, hl->dparent->matrix, pmB);
		MMul33(prA, hl->dparent->rmatrix, prB);

		pmT = pmA; pmA = pmB; pmB = pmT;
		prT = prA; prA = prB; prB = prT;
		hl = hl->dparent;
	}

	// did we reached top of the chain or maybe we met a valid hmatrix...
	if (hl->dparent)
	{
		MMul44(pmA, hl->dparent->hmatrix, pmB);
		MMul33(prA, hl->dparent->rmatrix, prB);
		pmT = pmA; pmA = pmB; pmB = pmT;
		prT = prA; prA = prB; prB = prT;
	}

	// this entity's hmatrix is valid now
	memcpy(hmatrix, pmA, 16 * sizeof(float));
	memcpy(hrmatrix, prA, 9 * sizeof(float));
	hmatrixvalid = true;

}
