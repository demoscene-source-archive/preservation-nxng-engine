	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		nXng Entity Triple Space Partitioning implementation file

	----------------------------------------------------------------------------*/


	#include	<float.h>
	#include	"nX_HeaderList.h"
	#include	"nXng/cColor.h"

	static		char			str[256];
	static		cMath			lMath;
	#define		INFRONT			0
	#define		BETWEEN			1
	#define		BEHIND			2


/*
	Return position of a polygon relative to plane
	returns: [BEHIND, BETWEEN, ON]
	----------------------------------------------
*/


unsigned char nX_Entity::PositionPolyPlane(nX_TSPpoly *pol, cM_Plane *p)
{

	double	d;
	bool	back, front;
	back = false;
	front = false;
	float	*pvtx = edge_adress;
	dXYZ	pt;

	for (long c = 0; c < pol->face->nedge; c++)
	{
		pt.x = edge_adress[pol->face->index[c] + 0];
		pt.y = edge_adress[pol->face->index[c] + 1];
		pt.z = edge_adress[pol->face->index[c] + 2];

		d = lMath.dPointToPlane(p, &pt);
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


/*
	Free a TSP node
	---------------
*/


void	nX::FreeTSPNode(nX_TSPnode *node)
{

	if (node->back)
		FreeTSPNode(node->back);
	if (node->on)
		FreeTSPNode(node->on);
	if (node->front)
		FreeTSPNode(node->front);

	delete(node->poly);
	delete(node);

}


/*
	Create a TSP poly...
	--------------------
*/


nX_TSPpoly	*nX_Entity::CreateTSPPoly(nX_reliure *prel, unsigned short index)
{

	nX_TSPpoly	*tmp = new nX_TSPpoly;
	memset(tmp, 0, sizeof(nX_TSPpoly));
	tmp->face = prel;
	tmp->iface = index;

	// Setup static normal
	float	*nrm = facenormals_adress + index * 3;
	tmp->p.a = nrm[0];
	tmp->p.b = nrm[1];
	tmp->p.c = nrm[2];

	// Compute plane scalar component
	float	*pvtx = &edge_adress[prel->index[0]];
	fXYZ	vtx(pvtx[0], pvtx[1], pvtx[2]);
	tmp->p.ComputeScalar(&vtx);

	// We have one more
	return	tmp;

}


/*
	Insert TSP poly in tree
	-----------------------
*/


void	nX_Entity::PutPolyInTSP(nX_TSPnode *node, nX_TSPpoly *poly)
{

	switch (PositionPolyPlane(poly, &node->poly->p))
	{
		case BEHIND:
			if (node->back == NULL)
			{
				node->back = new nX_TSPnode;
				memset(node->back, 0, sizeof(nX_TSPnode));
				node->back->poly = poly;
			}
			else
				PutPolyInTSP(node->back, poly);
			break;
		case BETWEEN:
			if (node->on == NULL)
			{
				node->on = new nX_TSPnode;
				memset(node->on, 0, sizeof(nX_TSPnode));
				node->on->poly = poly;
			}
			else
				PutPolyInTSP(node->on, poly);
			break;
		case INFRONT:	
			if (node->front == NULL)
			{
				node->front = new nX_TSPnode;
				memset(node->front, 0, sizeof(nX_TSPnode));
				node->front->poly = poly;
			}
			else
				PutPolyInTSP(node->front, poly);
			break;
	}

}


/*
	Compute Static TSP for entity
	-----------------------------
*/


void	nX_Entity::GetTSP()
{

	// If we had a TSP before we just free it now...
	if (TSP)
		pnX->FreeTSPNode(TSP);

	// Create the root poly
	nX_reliure	*prel = reliure_adress;
	float		*pfn = previous_facenormals;

	nX_TSPpoly	*TSPpoly = CreateTSPPoly(prel++, 0);
	TSP = new nX_TSPnode;
	memset(TSP, 0, sizeof(nX_TSPnode));
	TSP->poly = TSPpoly;

	// Insert all of the entity face(s) in tree
	for (long c = 0; c < nbface - 1; c++)
	{
		TSPpoly = CreateTSPPoly(prel++, c);
		PutPolyInTSP(TSP, TSPpoly);
	}

}


static	bool		FULLHIT;
static	dXYZ		gfrom, gto;
static	cM_Vector	view;


unsigned char	nX_Entity::PositionFiniteRayPlane(dXYZ *a, dXYZ *b, cM_Plane *p)
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


bool	nX_Entity::ConfirmRayIntersect(nX_TSPpoly *poly)
{

	// Intersection point
	bool			OutOfRange;
	cM_Vector		dist2hit;

	// Info about poly
	dXYZ			tri1, tri2, tri3;
	dXYZ			inter, normal, bcoord;
	float			*vtx = edge_adress;

	normal.x = poly->p.a;
	normal.y = poly->p.b;
	normal.z = poly->p.c;

	if (lMath.DotProd(&view, &normal) > 0.0 &&
		lMath.RayIntersectPlane(&gfrom, &view, &poly->p, &inter))
	{
		OutOfRange = false;
		dist2hit.x = inter.x - gfrom.x;
		dist2hit.y = inter.y - gfrom.y;
		dist2hit.z = inter.z - gfrom.z;
		dist2hit.ComputeLength();

		if (dist2hit.l <= view.l)
		{
			tri1.x = vtx[poly->face->index[0] + 0];
			tri1.y = vtx[poly->face->index[0] + 1];
			tri1.z = vtx[poly->face->index[0] + 2];
			for (long cv = 1; cv < poly->face->nedge - 1; cv++)
			{
				tri2.x = vtx[poly->face->index[cv + 0] + 0];
				tri2.y = vtx[poly->face->index[cv + 0] + 1];
				tri2.z = vtx[poly->face->index[cv + 0] + 2];
				tri3.x = vtx[poly->face->index[cv + 1] + 0];
				tri3.y = vtx[poly->face->index[cv + 1] + 1];
				tri3.z = vtx[poly->face->index[cv + 1] + 2];
				lMath.BaryCoord(&tri1, &tri2, &tri3, &inter, &bcoord);
				if (bcoord.x > 0.0 && bcoord.y > 0.0 && bcoord.z > 0.0)
						return true;	// we have a hit!
			}
		}
	}
	return false;	// no hit! (:
}


/*
static bool	ConfirmRayIntersect(nX_Entity *e, nX_reliure *prel, dXYZ *normal)
{

	// Intersection point
	bool			OutOfRange;
	cM_Vector		dist2hit;

	// Info about poly
	dXYZ			tri1, tri2, tri3;
	dXYZ			inter, bcoord;
	float			*vtx = e->edge_adress;

	normal.x = poly->p.a;
	normal.y = poly->p.b;
	normal.z = poly->p.c;

	if (lMath.DotProd(&view, normal) > 0.0 &&
		lMath.RayIntersectPlane(&gfrom, &view, &poly->p, &inter))
	{
		OutOfRange = false;
		dist2hit.x = inter.x - gfrom.x;
		dist2hit.y = inter.y - gfrom.y;
		dist2hit.z = inter.z - gfrom.z;
		dist2hit.ComputeLength();

		if (dist2hit.l <= view.l)
		{
			tri1.x = vtx[prel->index[0] + 0];
			tri1.y = vtx[prel->index[0] + 1];
			tri1.z = vtx[prel->index[0] + 2];
			for (long cv = 1; cv < poly->face->nedge - 1; cv++)
			{
				tri2.x = vtx[prel->index[cv + 0] + 0];
				tri2.y = vtx[prel->index[cv + 0] + 1];
				tri2.z = vtx[prel->index[cv + 0] + 2];
				tri3.x = vtx[prel->index[cv + 1] + 0];
				tri3.y = vtx[prel->index[cv + 1] + 1];
				tri3.z = vtx[prel->index[cv + 1] + 2];
				lMath.BaryCoord(&tri1, &tri2, &tri3, &inter, &bcoord);
				if (bcoord.x > 0.0 && bcoord.y > 0.0 && bcoord.z > 0.0)
						return true;	// we have a hit!
			}
		}
	}
	return false;	// no hit! (:
}
*/

void	nX_Entity::IntersectFiniteRayWithTSP(nX_TSPnode *node)
{
	switch (PositionFiniteRayPlane(&gfrom, &gto, &node->poly->p))
	{
		case BEHIND:
			if (!FULLHIT && node->back)	IntersectFiniteRayWithTSP(node->back);
			if (!FULLHIT && node->on)	IntersectFiniteRayWithTSP(node->on);
			break;

		case BETWEEN:
			if ( ConfirmRayIntersect(node->poly) )
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

bool	nX_Entity::IntersectRayWithTSP(dXYZ *from, dXYZ *to)
{

/*
	FULLHIT = false;
	gfrom.x = from->x; gfrom.y = from->y; gfrom.z = from->z;
	gto.x = to->x; gto.y = to->y; gto.z = to->z;

	view.x = to->x - from->x;
	view.y = to->y - from->y;
	view.z = to->z - from->z;
	view.ComputeLength();
	view.Normalize();

	IntersectFiniteRayWithTSP(TSP);
*/
	float	*pfn = facenormals_adress;
	dXYZ	normal;

	for (long c = 0; c < nbface; c++)
	{
		normal.x = pfn[0];
		normal.y = pfn[1];
		normal.z = pfn[2];
//		if (ConfirmRayIntersect(this, prel++, &normal))
			return true;
		pfn += 3;
	}


	return FULLHIT;

}

unsigned short	nX_Entity::IntersectViewWithTSP(cM_Plane *lClip, cM_Plane *uClip, cM_Plane *rClip, cM_Plane *dClip, cM_Plane *fzClip)
{
	// Valid all faces of tree if they are in front or between our plane.
	return 0;
}