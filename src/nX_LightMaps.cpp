	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		nXng Scene LightMaps implementation file

		1.0: Brute force resolving with entity BBox ray hit test,
		1.1: Speeded up caculation by a speed factor of 2x to 10x
			 by using TSP tree world representation,
		1.2: Added virtual planes to world TSP.

	----------------------------------------------------------------------------*/


	#include	<float.h>

	#include	"nX_HeaderList.h"
	#include	"nXng/cColor.h"

	#include	"IMG.h"
	#include	"JPEG.h"
	#include	"generic.h"

	extern		unsigned char	mulshader[65536];
	static		char			str[256];
	static		cMath			lMath;
	static		cColor			cCol;
	static		xLinker			xPK;

	// The JPEG quality lightmaps are recorded to
	#define		JPEG_QUALITY			90
	// EDGESHARING_TOLERANCE
	#define		EDGESHARING_TOLERANCE	1.0


	// TSP structure

	static		dXYZ			*gfrom, *gto;
	static		cM_Vector		*gview;
	static		nX_Entity		*glgt;
	static		unsigned short	gcent, gcface;
	static		nXcolor			*graycolor;
	static		bool			FULLHIT;


	#define		BEHIND			0
	#define		BETWEEN			1
	#define		INFRONT			2

	// TSP tree statistics
	static		unsigned long	TSPfrontnodeCount;
	static		unsigned long	TSPbacknodeCount;
	static		unsigned long	TSPonnodeCount;

	// TSP usage statistics
	static		unsigned long	runCount;
	static		unsigned long	runInterCount;
	static		double			totalInterCount;
	static		unsigned long	runPlaneTestCount;
	static		double			totalPlaneTestCount;
	static		unsigned long	TSPTotalNodeCount;


struct nX_iTSPpoly
{
	unsigned char	cvtx;						// # vertice
	dXYZ			*vtx;						// vertice list
	dXYZ			deltas;						// polygon deltas
	cM_Plane		p;							// polygon plane
	float			*UV[2];						// UV mapping set for COLOR & TRANSPARENCY
	unsigned short	cEnt, cFace;				// entity index, face index in entity
	nX_Surface		*surf;						// polygon surface
};

struct nX_iTSPnode
{
	nX_iTSPpoly		*poly;
	nX_iTSPnode		*back, *on, *front;
};


bool inline SwapVtxBuf(nX *pnX, bool db)
{
	if (db)
	{
		pnX->p3dpswapt = pnX->p3dpswap1;
		pnX->p3dpswap1 = pnX->p3dpswap2;
		pnX->p3dpswap2 = pnX->p3dpswapt;
		return true;
	}
	else
	{
		pnX->p3dpswap1 = pnX->edgesbuffer1_adress;
		pnX->p3dpswap2 = pnX->edgesbuffer2_adress;
		return true;
	}
}

bool inline SwapFNormBuf(nX *pnX, bool db)
{
	if (db)
	{
		pnX->pfnormt = pnX->pfnorm1;
		pnX->pfnorm1 = pnX->pfnorm2;
		pnX->pfnorm2 = pnX->pfnormt;
		return true;
	}
	else
	{
		pnX->pfnorm1 = pnX->facenormalsA_adress;
		pnX->pfnorm2 = pnX->facenormalsB_adress;
		return true;
	}
}

static nX_iTSPpoly *CreateTSPPolyPlane(float a, float b, float c, float d)
{

	nX_iTSPpoly		*tmp = new nX_iTSPpoly;
	memset(tmp, 0, sizeof(nX_iTSPpoly));
	tmp->cvtx = 0;
	tmp->p.a = a;
	tmp->p.b = b;
	tmp->p.c = c;
	tmp->p.d = d;
	return	tmp;

}

static nX_iTSPpoly *CreateTSPPoly(nX_reliure *prel, float *vtx, float *nrm, float *uvc, float *uvt)
{

	float			xmin, xmax, ymin, ymax, zmin, zmax;
	nX_iTSPpoly		*tmp = new nX_iTSPpoly;
	memset(tmp, 0, sizeof(nX_iTSPpoly));

	tmp->cvtx = prel->nedge;
	tmp->vtx = new dXYZ[prel->nedge];
	if (uvc) tmp->UV[0] = new float[prel->nedge * 2];
	if (uvt) tmp->UV[1] = new float[prel->nedge * 2];
	tmp->surf = prel->surf;

	xmax = xmin = vtx[prel->index[0] + 0];
	ymax = ymin = vtx[prel->index[0] + 1];
	zmax = zmin = vtx[prel->index[0] + 2];

	for (long c = 0; c < prel->nedge; c++)
	{
		tmp->vtx[c].x = vtx[prel->index[c] + 0];
		tmp->vtx[c].y = vtx[prel->index[c] + 1];
		tmp->vtx[c].z = vtx[prel->index[c] + 2];
		if (uvc)
		{	tmp->UV[0][c * 2 + 0] = uvc[c * 2 + 0];
			tmp->UV[0][c * 2 + 1] = uvc[c * 2 + 1];		}
		if (uvt)
		{	tmp->UV[1][c * 2 + 0] = uvt[c * 2 + 0];
			tmp->UV[1][c * 2 + 1] = uvt[c * 2 + 1];		}

		// find 3d deltas
		if (vtx[prel->index[c] + 0] > xmax) xmax = vtx[prel->index[c] + 0];
		if (vtx[prel->index[c] + 0] < xmin) xmin = vtx[prel->index[c] + 0];
		if (vtx[prel->index[c] + 1] > ymax) ymax = vtx[prel->index[c] + 1];
		if (vtx[prel->index[c] + 1] < ymin) ymin = vtx[prel->index[c] + 1];
		if (vtx[prel->index[c] + 2] > zmax) zmax = vtx[prel->index[c] + 2];
		if (vtx[prel->index[c] + 2] < zmin) zmin = vtx[prel->index[c] + 2];
	}

	tmp->p.a = nrm[0];
	tmp->p.b = nrm[1];
	tmp->p.c = nrm[2];
	tmp->p.ComputeScalar(tmp->vtx);
	tmp->deltas.x = xmax - xmin;
	tmp->deltas.y = ymax - ymin;
	tmp->deltas.z = zmax - zmin;

	return	tmp;

}


static unsigned char PositionFiniteRayPlane(dXYZ *a, dXYZ *b, cM_Plane *p)
{

	double	d;
	bool	back = false, front = false;

	d = lMath.dPointToPlane(p, a);
	if (d < -0.0001)
		back = true;
	else if (d > 0.0001)
		front = true;

	d = lMath.dPointToPlane(p, b);
	if (d < -0.0001)
		back = true;
	else if (d > 0.0001)
		front = true;

	if (back && !front)
		return BEHIND;
	if (!back && front)
		return INFRONT;

	return BETWEEN;

}


static unsigned char PositionInfiniteRayPlane(dXYZ *a, cM_Vector *view, cM_Plane *p)
{

	double	d;
	bool	back = false, front = false;

	d = lMath.dPointToPlane(p, a);
	if (d < -0.0001)
		back = true;
	else if (d > 0.0001)
		front = true;

	cM_Vector	normal(p->a, p->b, p->c);
	double	cross = lMath.DotProd(&normal, view);

	if (back)
		if (cross > 0.0) return BETWEEN; else return BEHIND;
	if (front)
		if (cross < 0.0) return BETWEEN; else return INFRONT;
	return BETWEEN;

}


static unsigned char PositionPolyPlane(nX_iTSPpoly *pol, cM_Plane *p)
{

	double	d;
	bool	back, front;
	back = false;
	front = false;

	for (long c = 0; c < pol->cvtx; c++)
	{
		d = lMath.dPointToPlane(p, &pol->vtx[c]);
		if (d < - 0.0001)
			back = true;
		else if (d > 0.0001)
			front = true;
	}

	if (back && !front)
		return BEHIND;
	if (!back && front)
		return INFRONT;

	return BETWEEN;

}


static void PutPolyInTSP(nX_iTSPnode *node, nX_iTSPpoly *poly)
{

	switch (PositionPolyPlane(poly, &node->poly->p))
	{
		case BEHIND:
			if (node->back == NULL)
			{
				node->back = new nX_iTSPnode;
				memset(node->back, 0, sizeof(nX_iTSPnode));
				node->back->poly = poly;
				TSPbacknodeCount++;
			}
			else
				PutPolyInTSP(node->back, poly);
			break;
		case BETWEEN:
			if (node->on == NULL)
			{
				node->on = new nX_iTSPnode;
				memset(node->on, 0, sizeof(nX_iTSPnode));
				node->on->poly = poly;
				TSPonnodeCount++;
			}
			else
				PutPolyInTSP(node->on, poly);
			break;
		case INFRONT:	
			if (node->front == NULL)
			{
				node->front = new nX_iTSPnode;
				memset(node->front, 0, sizeof(nX_iTSPnode));
				node->front->poly = poly;
				TSPfrontnodeCount++;
			}
			else
				PutPolyInTSP(node->front, poly);
			break;
	}

}


static void DestroyTSPNode(nX_iTSPnode *node)
{

	if (node->back)
		DestroyTSPNode(node->back);
	if (node->on)
		DestroyTSPNode(node->on);
	if (node->front)
		DestroyTSPNode(node->front);

	// Destroy node by itself
	if (node->poly->UV[0])
		delete node->poly->UV[0];
	if (node->poly->UV[1])
		delete node->poly->UV[1];
	if (node->poly->vtx)
		delete node->poly->vtx;
	if (node->poly)
		delete node->poly;
	delete node;

}


static bool	ConfirmRayIntersect(nX_iTSPpoly *poly)
{

	// Update statistic model
	if (poly->cvtx == 0)
		return false;
	runInterCount += 1;

	// TexMap handling
	float			ui, vi;
	float			*UV;
	nX_TexMap		*map;
	nXcolor			texel;
	unsigned long	eval;

	// Intersection point
	bool			OutOfRange;
	cM_Vector		dist2hit;
	cM_Vector		dist2light;

	// Info about poly
	dXYZ			tri1, tri2, tri3;
	dXYZ			inter, normal, bcoord;

	cM_Vector	view(gto->x - gfrom->x, gto->y - gfrom->y, gto->z - gfrom->z);
	view.ComputeLength();
	view.Normalize();

	normal.x = poly->p.a;
	normal.y = poly->p.b;
	normal.z = poly->p.c;

	if (lMath.DotProd(&view, &normal) > 0.0 &&
		lMath.RayIntersectPlane(gfrom, &view, &poly->p, &inter))
	{

		/* Before determining point-in-poly we need to ensure the
		   polygon we just hit is between point and light...
		   Distant light are infinitly far away so any polygon that
		   passed the previous test is between point and light.	*/	

		OutOfRange = false;
		if (glgt->lighttype != 0)
		{
			dist2hit.x = inter.x - gfrom->x;
			dist2hit.y = inter.y - gfrom->y;
			dist2hit.z = inter.z - gfrom->z;
			dist2hit.ComputeLength();
			dist2light.x = glgt->hmatrix[12] - gfrom->x;
			dist2light.y = glgt->hmatrix[13] - gfrom->y;
			dist2light.z = glgt->hmatrix[14] - gfrom->z;
			dist2light.ComputeLength();

			if (dist2hit.l >= dist2light.l)
				OutOfRange = true;
		}

		if (!OutOfRange)
		{
			// Point-in-poly?
			tri1.x = poly->vtx[0].x;
			tri1.y = poly->vtx[0].y;
			tri1.z = poly->vtx[0].z;

			for (long cv = 1; cv < poly->cvtx - 1; cv++)
			{

				tri2.x = poly->vtx[cv + 0].x;
				tri2.y = poly->vtx[cv + 0].y;
				tri2.z = poly->vtx[cv + 0].z;
				tri3.x = poly->vtx[cv + 1].x;
				tri3.y = poly->vtx[cv + 1].y;
				tri3.z = poly->vtx[cv + 1].z;
				lMath.BaryCoord(&tri1, &tri2, &tri3, &inter, &bcoord);

				// INTERSECTION CODE
				////////////////////

				if (bcoord.x > 0.0 && bcoord.y > 0.0 && bcoord.z > 0.0)
				{
					if (poly->surf->CAPS & nX_ALPHATEX)
					{
						// uv in poly
						UV = poly->UV[1];
						ui = (float)(bcoord.x * UV[0] + bcoord.y * UV[cv * 2 + 0] + bcoord.z * UV[cv * 2 + 2]);
						vi = (float)(bcoord.x * UV[1] + bcoord.y * UV[cv * 2 + 1] + bcoord.z * UV[cv * 2 + 3]);

						// texel adress
						map = poly->surf->map[nXML_TRANSPARENCY];
						map->EvaluateLinearTexel(ui, vi, &texel);

						// ray passing trough
						graycolor->r = unsigned char( float(graycolor->r) * float(texel.r) / 256 ) ;
						graycolor->g = unsigned char( float(graycolor->g) * float(texel.g) / 256 ) ;
						graycolor->b = unsigned char( float(graycolor->b) * float(texel.b) / 256 ) ;

						// We got a hit but the ray pass trough... unless raylight remainder is black!!!
						if (graycolor->r == 0 && graycolor->g == 0 && graycolor->b == 0)
							return false;

					}
					else if (poly->surf->transparency != 0)
					{

						if (poly->surf->CAPS & nX_MAPPED)
						{
							// uv in poly
							UV = poly->UV[0];
							ui = (float)(bcoord.x * UV[0] + bcoord.y * UV[cv * 2 + 0] + bcoord.z * UV[cv * 2 + 2]);
							vi = (float)(bcoord.x * UV[1] + bcoord.y * UV[cv * 2 + 1] + bcoord.z * UV[cv * 2 + 3]);

							// texel adress
							map = poly->surf->map[nXML_COLOR];
							map->EvaluateLinearTexel(ui, vi, &texel);

							// texel components...
// *** THIS IS THE CORRECT WAY OF CALCULATING COLOR FILTER... HOWEVER LIGHTWAVE IS WRONG AND I DECIDED TO CONFORM TO ITS RENDER! ***
//							raycolor->r = unsigned char( (float(texel.r)*(1-surf->transparency)*float(raycolor->r)*(surf->transparency)) / 256 + float(raycolor->r) * surf->transparency ) ;
//							raycolor->g = unsigned char( (float(texel.g)*(1-surf->transparency)*float(raycolor->g)*(surf->transparency)) / 256 + float(raycolor->g) * surf->transparency ) ;
//							raycolor->b = unsigned char( (float(texel.b)*(1-surf->transparency)*float(raycolor->b)*(surf->transparency)) / 256 + float(raycolor->b) * surf->transparency ) ;

							eval = unsigned long( float(texel.r) * poly->surf->transparency * float(graycolor->r) / 256.0 );
							if (eval > 255) eval = 255; else if (eval < 0) eval = 0; graycolor->r = eval;
							eval = unsigned long( float(texel.g) * poly->surf->transparency * float(graycolor->g) / 256.0 );
							if (eval > 255) eval = 255; else if (eval < 0) eval = 0; graycolor->g = eval;
							eval = unsigned long( float(texel.b) * poly->surf->transparency * float(graycolor->b) / 256.0 );
							if (eval > 255) eval = 255; else if (eval < 0) eval = 0; graycolor->b = eval;
						}
						else
						{
							// ray * surface color
							graycolor->r = unsigned char( (float(poly->surf->color.r) * (1 - poly->surf->transparency) * float(graycolor->r) * (poly->surf->transparency)) / 256 + float(graycolor->r) * poly->surf->transparency ) ;
							graycolor->g = unsigned char( (float(poly->surf->color.g) * (1 - poly->surf->transparency) * float(graycolor->g) * (poly->surf->transparency)) / 256 + float(graycolor->g) * poly->surf->transparency ) ;
							graycolor->b = unsigned char( (float(poly->surf->color.b) * (1 - poly->surf->transparency) * float(graycolor->b) * (poly->surf->transparency)) / 256 + float(graycolor->b) * poly->surf->transparency ) ;
						}

						// We got a hit but the ray pass trough... unless raylight remainder is black!!!
						if (graycolor->r == 0 && graycolor->g == 0 && graycolor->b == 0)
							return false;
					}
					else
						// we have a hit!
						return true;

				}
			}
		}
	}

	// no hit! (:
	return false;

}


static void IntersectFiniteRayWithTSP(nX_iTSPnode *node)
{
	runPlaneTestCount += 1;

	switch (PositionFiniteRayPlane(gfrom, gto, &node->poly->p))
	{
		case BEHIND:
			if (!FULLHIT && node->back)	IntersectFiniteRayWithTSP(node->back);
			if (!FULLHIT && node->on)	IntersectFiniteRayWithTSP(node->on);
			break;

		case BETWEEN:
			if ( (gcent != node->poly->cEnt || gcface != node->poly->cFace) && ConfirmRayIntersect(node->poly) )
				FULLHIT = true;		// RAY is fully blocked!!! STOP iterating!

			if (!FULLHIT && node->back)	IntersectFiniteRayWithTSP(node->back);
			if (!FULLHIT && node->on)	IntersectFiniteRayWithTSP(node->on);
			if (!FULLHIT && node->front)IntersectFiniteRayWithTSP(node->front);
			break;

		case INFRONT:
			if (!FULLHIT && node->on)	IntersectFiniteRayWithTSP(node->on);
			if (!FULLHIT && node->front)IntersectFiniteRayWithTSP(node->front);
			break;
	}
}


static bool TSPTestVisibility(nX_iTSPnode *root, dXYZ *from, cM_Vector *view, nX_Entity *lgt, unsigned short cEnt, unsigned short cFace, nXcolor *raycolor)
{

	dXYZ	to;
	to.x = lgt->hmatrix[12];
	to.y = lgt->hmatrix[13];
	to.z = lgt->hmatrix[14];

	// All these are fixed variables so we avoid passing them by stack calls!!!
	gfrom = from; gto = &to;
	gview = view;
	glgt = lgt;
	gcent = cEnt; gcface = cFace;
	graycolor = raycolor;

	// Statistic model
	runInterCount = 0;
	runPlaneTestCount = 0;
	runCount++;

	// Point and Spot case...
	FULLHIT = false;	// all recursive functions should return as soon as one instance set this to true!
	IntersectFiniteRayWithTSP(root);

	// Update statistics
	totalInterCount += runInterCount;
	totalInterCount /= 2;
	totalPlaneTestCount += runPlaneTestCount;
	totalPlaneTestCount /= 2;
	return FULLHIT;

}


// GetUVLightMap() ------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////


void	nX_Scene::GetUVLightMap()
{

	float		lmapbench = GetTickCount();
	float		deltaLM = maxLMsize - minLMsize;

	// ACTOR'S MOTION/STATE ---------------------------------------------------

	long		c;
	nX_Entity	*entity;
	pnX->Verbose("Seeking to frame... Freezing light(s)...");

	for (c = 0; c <= nbentity; c++)
	{
		entity = entityadress[c];
		entity->GetLWMotion(frame);
		entity->GetExtraLWMotions(frame);
	}
	this->GetAllMatrix();


	// TRANSFORM GEOMETRY -----------------------------------------------------
	///////////////////////////////////////////////////////////////////////////


	bool			vtxdbbuffer, fndbbuffer;
	unsigned long	c2, *pindex;
	nX_reliure		*prel;
	pnX->Verbose("Transforming geometry... Looking up areas...");

	for (c = 1; c <= nbobj; c++)
	{
		entity = entityadress[c];

		if (entity->type == nXETYPE_OBJECT)
		{
			vtxdbbuffer = false;
			pnX->p3dpswap1 = entity->edge_adress;
			pnX->p3dpswap2 = pnX->edgesbuffer1_adress;
			fndbbuffer = false;
			pnX->pfnorm1 = entity->facenormals_adress;
			pnX->pfnorm2 = pnX->facenormalsA_adress;
			
			// Lightwave's silly metamorphing
			if (entity->Morph && entity->Morph->values[0] != 0)
			{
				Morph(entity->nbpoints, pnX->p3dpswap1, pnX->p3dpswap2, entity->xyzmorphdeltas, entity->Morph->values[0]);
				vtxdbbuffer = SwapVtxBuf(pnX, vtxdbbuffer);

				if ( entity->requirement & nX_USEFACENORMAL )
				{
					Morph(entity->nbface, pnX->pfnorm1, pnX->pfnorm2, entity->nfamorphdeltas, entity->Morph->values[0]);
					fndbbuffer = SwapFNormBuf(pnX, fndbbuffer);
				}
			}

			TransformVertice(entity->nbpoints, pnX->p3dpswap1, entity->previous_edge, entity->hmatrix);
			if ( entity->requirement & nX_USEFACENORMAL )
				TransformNormal(entity->nbface, pnX->pfnorm1, entity->previous_facenormals, entity->hrmatrix);
		}
	}


	// SETUP TSP --------------------------------------------------------------
	///////////////////////////////////////////////////////////////////////////


	pnX->Verbose("   ...Building TSP...");
	nX_iTSPnode		*iTSProot, *iTSPp;
	nX_iTSPpoly		*iTSPpoly;
	iTSProot = NULL;
	TSPTotalNodeCount = 0;
	TSPfrontnodeCount = 0;
	TSPbacknodeCount = 0;
	TSPonnodeCount = 0;

	// Create TSP root node (2 planes)
	iTSPpoly = CreateTSPPolyPlane(0, 1, 0, (float)((minBound.y + maxBound.y) / 2 * mulfactor));
	iTSProot = new nX_iTSPnode;
	iTSPp = iTSProot;
	memset(iTSProot, 0, sizeof(nX_iTSPnode));
	iTSProot->poly = iTSPpoly;

	iTSPpoly = CreateTSPPolyPlane(1, 0, 0, (float)((minBound.x + maxBound.x) / 2 * mulfactor));
	PutPolyInTSP(iTSProot, iTSPpoly);
	TSPTotalNodeCount += 2;


	// COMPUTE POLYGONS DELTAS AND BUILD TSP ----------------------------------
	///////////////////////////////////////////////////////////////////////////


	dXYZ			pip;							// point-in-poly
	cM_Vector		trU, trV, nrm;					// polygon edge and normal
	float			gDX = -1, gDY = -1, gDZ = -1;
	float			*pfn;							// pointer to object's face normal list
	float			*pmap, *pmaptr;					// UV coords set

	for (c = 1; c <= nbobj; c++)
	{
		entity = entityadress[c];

		if ( entity->type == nXETYPE_OBJECT && (entity->staticinscene || entity->forceray)  && !entity->nocastshadow )
		{
			pfn = entity->previous_facenormals;
			prel = entity->reliure_adress;
			pmap = entity->UVmap[nXML_COLOR];
			pmaptr = entity->UVmap[nXML_TRANSPARENCY];

			for (c2 = 0; c2 < entity->nbface; c2++)
			{
				iTSPpoly = CreateTSPPoly(prel, entity->previous_edge, pfn, pmap, pmaptr);
				iTSPpoly->cEnt = c;
				iTSPpoly->cFace = c2;
				TSPTotalNodeCount += 1;

				// Find poly location in TSP
				PutPolyInTSP(iTSProot, iTSPpoly);

				// World biggest virtual polygon deltas
				if (iTSPpoly->deltas.x > gDX) gDX = iTSPpoly->deltas.x;
				if (iTSPpoly->deltas.y > gDY) gDY = iTSPpoly->deltas.y;
				if (iTSPpoly->deltas.z > gDZ) gDZ = iTSPpoly->deltas.z;

				if (pmap)	pmap += 2 * prel->nedge;
				if (pmaptr)	pmaptr += 2 * prel->nedge;
				pfn += 3;
				prel++;
			}
		}
	}


	// DONE TSP, DISPLAY STATISTICS -------------------------------------------
	///////////////////////////////////////////////////////////////////////////


	pnX->Verbose("    Done... TSP statistics:");
	wsprintf(str, "        %d node(s),", TSPTotalNodeCount);
	pnX->Verbose(str);
	wsprintf(str, "        %d%% front, %d%% on, %d%% back.", unsigned long(float(TSPfrontnodeCount) / TSPTotalNodeCount * 100.0), unsigned long(float(TSPonnodeCount) / TSPTotalNodeCount * 100.0), unsigned long(float(TSPbacknodeCount) / TSPTotalNodeCount * 100.0));
	pnX->Verbose(str);
	pnX->Verbose(" ");


	// OBJECT LIGHTMAP VARIABLES ----------------------------------------------
	///////////////////////////////////////////////////////////////////////////


	pnX->Verbose(" ");
	pnX->Verbose("***Beginning LIGHTMAP***");
	wsprintf(str, "%d object(s) in scene, %d light(s).", nbobj, nblight);
	pnX->Verbose(str);
	pnX->Verbose(" ");

	long			cFace;			// face counter
	long			cLight;			// light loop variabole
	long			cLMap = 0;		// lightmap indice

	nX_Entity		*light;			// light instance
	long			cEdge, nedge;	// #vertice in poly
	float			*pVtx;

	nX_Surface		*surf;			// polygon's surface
	nX_reliure		*preliure;		// polygon's geometry

	IMG				*lightmap;		// the tmp lightmap we'll work on
	nXcolor			out;			// usd to store lightning at a given location in world
	JPEG			*lJPG;			// the JPEG module so we can save our lightmaps
	long			lmX, lmY;		// used to scan the lightmap
	unsigned long	*plmBuff;		// pointer to the lightmap buffer
	long			lmU, lmV;		// lightmap size

	double			D;				// polygon determinant

	// The variable set needed to UV map from TexSpace to WorldSpace

	double			u[333], v[333];		// polygon UV map coords
	dXYZ			polymin, polymax;
	dXYZ			topleft, topright, downleft, downright;
	dXYZ			viscan, vescan, hscan;
	double			edgeDelta;
	cM_Vector		sfvect;
	dXYZ			pvi, pve, ph;
	dXYZ			lightpos;

	char			fname[256];
	nX_Entity		*cEntity;			// the entity that own the face we're shadow feeling
	long			dr, dg, db;			// final lighted~shadowed RGB components
	long			ur, ug, ub;			// final lighted~unoccluded RGB components

	// Aspect ratio handler
	float			arU, arV;
	float			iuoc, iref;			// unocludded intensity, reference intensity


	// xLINK UV SET FILE CONTAINER --------------------------------------------
	///////////////////////////////////////////////////////////////////////////


	strcpy(str, fullfile);
	strcpy(str + strlen(str) - 3, "nxl");
	xPK.CreatexLink(str);

	// Estimate UVset filesize
	float	eDissolve;
	char	*UVmap = new char[this->totalface * (6 + 12 * 2)];		// type, bTri, UV * 12
	char	*pUV = UVmap;

	// Write LMAP header
	*(unsigned long *)pUV = 'LMP2';
	pUV += 4;
	pUV[0] = minLMsize;
	pUV[1] = maxLMsize;
	pUV += 2;

	// LIGHTMAP COMPUTATIONS --------------------------------------------------
	///////////////////////////////////////////////////////////////////////////

	runCount = 0;
	totalInterCount = 0;
	totalPlaneTestCount = 0;
	unsigned long ttLMAP = 0;
	unsigned long RGBLMAP = 0, FULLLMAP = 0;

	for (c = 1; c <= nbobj; c++)
	{

		cEntity = entity = entityadress[c];
		pVtx = entity->previous_edge;

		if (entity->Dissolve)
			eDissolve = entity->Dissolve->values[0]; else eDissolve = 0;

		// Perform UV computation & lightmap creation
		if (
			(entity->type == nXETYPE_OBJECT && entity->staticinscene && eDissolve == 0 && !entity->nolmap)
			||
			(entity->type == nXETYPE_OBJECT && entity->dolmap)
		   )
		{

			*pUV++ = 1;							// Object is to be lightmapped
			pfn = entity->facenormals_adress;

			// Begin calculus (:

			wsprintf(str, "   Generating UV set for object #%d... %d face(s)", c, entity->nbface);
			pnX->Verbose(str);
			pnX->Verbose(" ");

			preliure = entity->reliure_adress;
			pfn = entity->previous_facenormals;
			cLMap = 0;

			// Work on each of object faces

			for (cFace = 0; cFace < entity->nbface; cFace++)
			{

				nedge = preliure->nedge;
				surf = preliure->surf;
				pindex = preliure->index;

				// We won't bother lightmapping surfaces that do not want lightning!

				if (
						(surf->requirement & nX_USEFACENORMAL || surf->requirement & nX_USEEDGENORMAL) &&
						!(surf->CAPS & nX_ALPHATEX) && (surf->transparency == 0.0)
				   )
				{

					// GET WORLD SPACE TO TEXTURE SPACE ---------------------------------------
					///////////////////////////////////////////////////////////////////////////


					polymin.x = pVtx[pindex[0] + 0];
					polymin.y = pVtx[pindex[0] + 1];
					polymin.z = pVtx[pindex[0] + 2];
					polymax.x = polymin.x; polymax.y = polymin.y; polymax.z = polymin.z;

					for (cEdge = 1; cEdge < nedge; cEdge++)
					{
						if (pVtx[pindex[cEdge] + 0] > polymax.x)
							polymax.x = pVtx[pindex[cEdge] + 0];
						if (pVtx[pindex[cEdge] + 1] > polymax.y)
							polymax.y = pVtx[pindex[cEdge] + 1];
						if (pVtx[pindex[cEdge] + 2] > polymax.z)
							polymax.z = pVtx[pindex[cEdge] + 2];
						if (pVtx[pindex[cEdge] + 0] < polymin.x)
							polymin.x = pVtx[pindex[cEdge] + 0];
						if (pVtx[pindex[cEdge] + 1] < polymin.y)
							polymin.y = pVtx[pindex[cEdge] + 1];
						if (pVtx[pindex[cEdge] + 2] < polymin.z)
							polymin.z = pVtx[pindex[cEdge] + 2];
					}

					// Polygon 3D-plane determinant
					D = - (pfn[0] * pVtx[pindex[0] + 0] + pfn[1] * pVtx[pindex[0] + 1] + pfn[2] * pVtx[pindex[0] + 2]);

					// Planar map upon X axis
					if (fabs(pfn[0]) >= fabs(pfn[1]) && fabs(pfn[0]) >= fabs(pfn[2]))
					{
						arU = polymax.z - polymin.z;
						arV = polymax.y - polymin.y;
						lmU = arU / gDZ * deltaLM + minLMsize;
						lmV = arV / gDY * deltaLM + minLMsize;
						for (cEdge = 0; cEdge < nedge; cEdge++)
						{
							u[cEdge] = (pVtx[pindex[cEdge] + 2] - polymin.z) / arU;
							v[cEdge] = (pVtx[pindex[cEdge] + 1] - polymin.y) / arV;
						}
						// Space coords for left-top/right-down corner of texmap
						topleft.x = - (D + pfn[1] * polymin.y + pfn[2] * polymin.z) / pfn[0];
						topleft.y = polymin.y;
						topleft.z = polymin.z;
						topright.x = - (D + pfn[1] * polymin.y + pfn[2] * polymax.z) / pfn[0];
						topright.y = polymin.y;
						topright.z = polymax.z;
						downleft.x = - (D + pfn[1] * polymax.y + pfn[2] * polymin.z) / pfn[0];
						downleft.y = polymax.y;
						downleft.z = polymin.z;
						downright.x = - (D + pfn[1] * polymax.y + pfn[2] * polymax.z) / pfn[0];
						downright.y = polymax.y;
						downright.z = polymax.z;
					}

					// Planar map upon Y axis
					else if (fabs(pfn[1]) >= fabs(pfn[0]) && fabs(pfn[1]) >= fabs(pfn[2]))
					{
						arU = polymax.x - polymin.x;
						arV = polymax.z - polymin.z;
						lmU = arU / gDX * deltaLM + minLMsize;
						lmV = arV / gDZ * deltaLM + minLMsize;
						for (cEdge = 0; cEdge < nedge; cEdge++)
						{
							u[cEdge] = (pVtx[pindex[cEdge] + 0] - polymin.x) / (polymax.x - polymin.x);
							v[cEdge] = (pVtx[pindex[cEdge] + 2] - polymin.z) / (polymax.z - polymin.z);
						}
						// Space coords for left-top/right-down corner of texmap
						topleft.x = polymin.x;
						topleft.y = - (D + pfn[0] * polymin.x + pfn[2] * polymin.z) / pfn[1];
						topleft.z = polymin.z;
						topright.x = polymax.x;
						topright.y = - (D + pfn[0] * polymax.x + pfn[2] * polymin.z) / pfn[1];
						topright.z = polymin.z;
						downleft.x = polymin.x;
						downleft.y = - (D + pfn[0] * polymin.x + pfn[2] * polymax.z) / pfn[1];
						downleft.z = polymax.z;
						downright.x = polymax.x;
						downright.y = - (D + pfn[0] * polymax.x + pfn[2] * polymax.z) / pfn[1];
						downright.z = polymax.z;
					}

					// Planar map upon Z axis
					else
					{
						arU = polymax.x - polymin.x;
						arV = polymax.y - polymin.y;
						lmU = arU / gDX * deltaLM + minLMsize;
						lmV = arV / gDY * deltaLM + minLMsize;
						for (cEdge = 0; cEdge < nedge; cEdge++)
						{
							u[cEdge] = (pVtx[pindex[cEdge] + 0] - polymin.x) / (polymax.x - polymin.x);
							v[cEdge] = (pVtx[pindex[cEdge] + 1] - polymin.y) / (polymax.y - polymin.y);
						}
						// Space coords for left-top/right-down corner of texmap
						topleft.x = polymin.x;
						topleft.y = polymin.y;
						topleft.z = - (D + pfn[0] * polymin.x + pfn[1] * polymin.y) / pfn[2];
						topright.x = polymax.x;
						topright.y = polymin.y;
						topright.z = - (D + pfn[0] * polymax.x + pfn[1] * polymin.y) / pfn[2];
						downleft.x = polymin.x;
						downleft.y = polymax.y;
						downleft.z = - (D + pfn[0] * polymin.x + pfn[1] * polymax.y) / pfn[2];
						downright.x = polymax.x;
						downright.y = polymax.y;
						downright.z = - (D + pfn[0] * polymax.x + pfn[1] * polymax.y) / pfn[2];
					}

					// MAP WORLD SPACE TO TEXTURE SPACE ---------------------------------------
					///////////////////////////////////////////////////////////////////////////


					// Allocate the lightmap
					lightmap = new IMG();
					lightmap->width = lmU;
					lightmap->height = lmV;
					lightmap->PixelFormat.bpp = 32;
					lightmap->adress = new unsigned long[lightmap->width * lightmap->height];
					plmBuff = (unsigned long *)lightmap->adress;

					// Compute texmap gradients
					edgeDelta = ((downleft.x - topleft.x) / lightmap->height) * EDGESHARING_TOLERANCE;
					viscan.x = ( downleft.x - topleft.x - 2 * edgeDelta) / lightmap->height;
					pvi.x = topleft.x + edgeDelta;
					edgeDelta = ((downleft.y - topleft.y) / lightmap->height) * EDGESHARING_TOLERANCE;
					viscan.y = (downleft.y - topleft.y - 2 * edgeDelta) / lightmap->height;
					pvi.y = topleft.y + edgeDelta;
					edgeDelta = ((downleft.z - topleft.z) / lightmap->height) * EDGESHARING_TOLERANCE;
					viscan.z = (downleft.z - topleft.z - 2 * edgeDelta) / lightmap->height;
					pvi.z = topleft.z + edgeDelta;
					edgeDelta = ((downright.x - topright.x) / lightmap->height) * EDGESHARING_TOLERANCE;
					vescan.x = (downright.x - topright.x - 2 * edgeDelta) / lightmap->height;
					pve.x = topright.x + edgeDelta;
					edgeDelta = ((downright.y - topright.y) / lightmap->height) * EDGESHARING_TOLERANCE;
					vescan.y = (downright.y - topright.y - 2 * edgeDelta) / lightmap->height;
					pve.y = topright.y + edgeDelta;
					edgeDelta = ((downright.z - topright.z) / lightmap->height) * EDGESHARING_TOLERANCE;
					vescan.z = (downright.z - topright.z - 2 * edgeDelta) / lightmap->height;
					pve.z = topright.z + edgeDelta;

					for (lmY = 0; lmY < lightmap->height; lmY++)
					{

						// Compute scan world GRADIENTS
						edgeDelta = ((pve.x - pvi.x) / lightmap->width) * EDGESHARING_TOLERANCE;
						hscan.x = (pve.x - pvi.x - 2 * edgeDelta) / lightmap->width;
						ph.x = pvi.x + edgeDelta;
						edgeDelta = ((pve.y - pvi.y) / lightmap->width) * EDGESHARING_TOLERANCE;
						hscan.y = (pve.y - pvi.y - 2 * edgeDelta) / lightmap->width;
						ph.y = pvi.y + edgeDelta;
						edgeDelta = ((pve.z - pvi.z) / lightmap->width) * EDGESHARING_TOLERANCE;
						hscan.z = (pve.z - pvi.z - 2 * edgeDelta) / lightmap->width;
						ph.z = pvi.z + edgeDelta;

						for (lmX = 0; lmX < lightmap->width; lmX++)
						{
							// 3D ph[x, y, z] <=> 2D {lmX, lmY}
							dr = iambientcolor.r; dg = iambientcolor.g;	db = iambientcolor.b;
							dr += surf->luminosity; dg += surf->luminosity; db += surf->luminosity;

							ur = dr; ug = dg; ub = db;

							for (cLight = firstlightno; cLight <= nbentity; cLight++)
							{
								light = entityadress[cLight];

								switch (light->lighttype)
								{
									case 0:			// DISTANT
										sfvect.x = - light->hmatrix[8];
										sfvect.y = - light->hmatrix[9];
										sfvect.z = - light->hmatrix[10];
										break;
									case 1:			// POINT
										sfvect.x = light->hmatrix[12] - ph.x;
										sfvect.y = light->hmatrix[13] - ph.y;
										sfvect.z = light->hmatrix[14] - ph.z;
										break;
									case 2:			// SPOT (POINT special case)
										sfvect.x = light->hmatrix[12] - ph.x;
										sfvect.y = light->hmatrix[13] - ph.y;
										sfvect.z = light->hmatrix[14] - ph.z;
										break;
								}

								// Shadow feeler
								sfvect.ComputeLength();
								sfvect.Normalize();
								lightpos.x = light->hmatrix[12];
								lightpos.y = light->hmatrix[13];
								lightpos.z = light->hmatrix[14];

								if (GetLightningAtLocation(light, ph.x, ph.y, ph.z, pfn[0], pfn[1], pfn[2], surf, &out) > 0.0)
								{
									ur += out.r; ug += out.g; ub += out.b;

									if (!TSPTestVisibility(iTSProot, &ph, &sfvect, light, c, cFace, &out))
									{	dr += out.r; dg += out.g; db += out.b;	}
								}

								// We have no time to waste here...
								if (dr >= 255 && dg >= 255 && db >= 255)
									break;

							}
							// Clip resulting occluded RGB
							if (dr > 255) dr = 255;
							if (dg > 255) dg = 255;
							if (db > 255) db = 255;

							// Output shadow map
							if (shadowsonly)
							{
								// Clip lightning RGB
								if (ur > 255) ur = 255;
								if (ug > 255) ug = 255;
								if (ub > 255) ub = 255;

								iuoc = cCol.RGBgreyscale(ur, ug, ub);
								iref = cCol.RGBgreyscale(dr, dg, db);
								ur = unsigned char(iref / iuoc * 255.0);
								*plmBuff++ = (ur << 16) + (ur << 8) + ur;
							}
							// Output light & shadow maps
							else
								*plmBuff++ = (dr << 16) + (dg << 8) + db;

							ph.x += hscan.x; ph.y += hscan.y; ph.z += hscan.z;

						}

						pvi.x += viscan.x; pvi.y += viscan.y; pvi.z += viscan.z;
						pve.x += vescan.x; pve.y += vescan.y; pve.z += vescan.z;
						pnX->SetProgress(long( float(lmY) / lightmap->height * 100.0));

					}


					// OPTIMIZE RESULTING LIGHTMAP --------------------------------------------
					///////////////////////////////////////////////////////////////////////////

					// test if lightmap is monocolored!
/*
					if ( lightmap->Average(0.0005, &monoR, &monoG, &monoB) )
					{
//						if (monoR == monoG == monoB == 255)
						pUV[0] = 3;			// This face is light shaded...
						if ( (surf->CAPS & nX_MAPPED) || (surf->CAPS & nX_EVMAP) )
						{
							pUV[1] = monoR;
							pUV[2] = monoG;
							pUV[3] = monoB;
						}
						else
						{
							pUV[1] = unsigned char( float(monoR) * float(surf->color.r) / 256.0 );
							pUV[2] = unsigned char( float(monoG) * float(surf->color.g) / 256.0 );
							pUV[3] = unsigned char( float(monoB) * float(surf->color.b) / 256.0 );
						}
						pUV += 4;
						wsprintf(str, "        Lightmap #%d sampled as RGB{%d;%d;%d}", cLMap, monoR, monoG, monoB);
						RGBLMAP++;
						pnX->VerboseOver(str);
					}
					else
*/					{
						// BLUR OUTPUT LIGHTMAP
						// 3x3 matrix strong blur
/*
						lightmap->convolutionmatrix[0] = 1;
						lightmap->convolutionmatrix[1] = 2;
						lightmap->convolutionmatrix[2] = 1;
						lightmap->convolutionmatrix[3] = 2;
						lightmap->convolutionmatrix[4] = 4;
						lightmap->convolutionmatrix[5] = 2;
						lightmap->convolutionmatrix[6] = 1;
						lightmap->convolutionmatrix[7] = 2;
						lightmap->convolutionmatrix[8] = 1;
						lightmap->Apply3x3Convolution();
*/

						// SAVE LIGHTMAP TO JPEG --------------------------------------------------
						///////////////////////////////////////////////////////////////////////////


						lJPG = new JPEG(lightmap);
						wsprintf(fname, "c:\\lm%d-%d.jpg", c, cLMap);	// alias name
						if (lJPG->Save("c:\\nxlmap.tmp", IDCT_FLOAT, JPEG_QUALITY, true) != IMG_OK)
						{
							wsprintf(str, "   *** IMNLib::Could not save lightmap to 'c:\\nxlmap.tmp' => %dx%d ***", lightmap->width, lightmap->height);
							pnX->Verbose(str);
						}
						else
						{
							wsprintf(str, "        Lightmap #%d sampled as %dx%d", cLMap, lightmap->width, lightmap->height);
							xPK.AddToxLinkAlias("c:\\nxlmap.tmp", fname);
							DeleteFile("c:\\nxlmap.tmp");
							pnX->VerboseOver(str);
						}

						FULLLMAP++;
						delete	lJPG;

						// Store LightMap ID in UVset
						if (shadowsonly)
						{
							*pUV++ = 2;										// This face is shadowmapped by a texture
							*(unsigned short *)pUV = cLMap;
							pUV += 2;

							// Store UV coords
							for (cEdge = 0; cEdge < nedge; cEdge++)
							{
								*(unsigned short *)pUV = (unsigned short)(u[cEdge] * 64000);
								pUV += 2;
								*(unsigned short *)pUV = (unsigned short)(v[cEdge] * 64000);
								pUV += 2;
							}
						}
						else
						{
							*pUV++ = 1;										// This face is lightmapped by a texture
							*(unsigned short *)pUV = cLMap;
							pUV += 2;

							// Store UV coords
							for (cEdge = 0; cEdge < nedge; cEdge++)
							{
								*(unsigned short *)pUV = (unsigned short)(u[cEdge] * 64000);
								pUV += 2;
								*(unsigned short *)pUV = (unsigned short)(v[cEdge] * 64000);
								pUV += 2;
							}
						}
					}

					// Next lightmap
					cLMap++;
					ttLMAP++;
					delete lightmap;
				}
				else
				{
					*pUV++ = 0;										// This face has no lightmap
					wsprintf(str, "   Skipped face... no shading in its surface...");
					pnX->VerboseOver(str);
				}

				// To the next face
				pfn += 3;
				preliure++;
			}
			pnX->VerboseOver("         Done...");

		}
		else
		{

			*pUV++ = 0;												// Object is not to be lightmapped
			wsprintf(str, "   Skipped entity #%d. Not [shaded|object|visible|static]...", c);
			pnX->Verbose(str);

		}

	}


	// EO LIGHTMAP FOR SCENE --------------------------------------------------
	///////////////////////////////////////////////////////////////////////////


	// Close the xLink object
	xPK.AddBufferToxLink("UVset", UVmap, pUV - UVmap);
	xPK.ClosexLink();

	// Display report to user
	lmapbench = GetTickCount() - lmapbench;

	DestroyTSPNode(iTSProot);
	pnX->Verbose(" ");
	wsprintf(str, "*** LIGHTMAP ended *** Took %d sec. to render %d map(s).", (unsigned long)(lmapbench / 1000), ttLMAP);
	pnX->Verbose(str);
	wsprintf(str, "                            %d RGB(s), %d JPEG(s).", RGBLMAP, FULLLMAP);
	pnX->Verbose(str);
	pnX->Verbose(" ");

	wsprintf(str, "    TEXEL LEVEL TSP USAGE:,", (unsigned long)(totalPlaneTestCount));
	pnX->Verbose(str);
	wsprintf(str, "        - TSP total node count: %d,", (unsigned long)(TSPTotalNodeCount));
	pnX->Verbose(str);
	wsprintf(str, "        - TSP visited node count: %d,", (unsigned long)(totalPlaneTestCount));
	pnX->Verbose(str);
	wsprintf(str, "        - TSP intersected node count: %d.", (unsigned long)(totalInterCount));
	pnX->Verbose(str);
	wsprintf(str, "        -> %d%% Intersection on TSP.", (unsigned long)(totalInterCount / TSPTotalNodeCount * 100.0));
	pnX->Verbose(str);
	wsprintf(str, "        -> %d%% Intersection on visit.", (unsigned long)(totalInterCount / totalPlaneTestCount * 100.0));
	pnX->Verbose(str);
	pnX->Verbose(" ");

}
