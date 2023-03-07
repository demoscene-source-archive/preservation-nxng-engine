	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		nXng Polygon Handler implementation file

	----------------------------------------------------------------------------*/



	#include	"nX_HeaderList.h"
	#include	"nXng/nXCol_FASTinlines.cpp"


/*

		nX_EdgeAsSparks:
	
	Call to draw each vertex of an object
	as a particle

*/


void	nX::EdgeAsSparks(nX_Entity *entity, float *c2d)
{

	long	c;
	float	x, y, z, s;
	s = float(entity->PolySize->values[0] * cscene->mulfactor * 0.45);

	nX_vertex	*localvertex;
	
	for (c = 0; c < entity->nbpoints - 8; c++)
	{
		x = c2d[0];
		y = c2d[1];
		z = c2d[2];
		c2d += 3;

		pfacelist->nedge = 4;
		pfacelist->surf = DefaultPolySizeSurface;
		if (entity->Dissolve)
			pfacelist->alpha = entity->Dissolve->values[0];
		else
			pfacelist->alpha = 0;
		pfacelist->vertice = pvertexlist;

		localvertex = pvertexlist;
		localvertex->x = float(x-s);
		localvertex->y = float(y-s);
		localvertex->z = float(z);
		localvertex->u[0] = (float)0;
		localvertex->v[0] = (float)0;
		localvertex++;
		localvertex->x = float(x+s);
		localvertex->y = float(y-s);
		localvertex->z = float(z);
		localvertex->u[0] = (float)1;
		localvertex->v[0] = (float)0;
		localvertex++;
		localvertex->x = float(x+s);
		localvertex->y = float(y+s);
		localvertex->z = float(z);
		localvertex->u[0] = (float)1;
		localvertex->v[0] = (float)1;
		localvertex++;
		localvertex->x = float(x-s);
		localvertex->y = float(y+s);
		localvertex->z = float(z);
		localvertex->u[0] = (float)0;
		localvertex->v[0] = (float)1;

		if (ClipFace())
			ValidFace( ulong(z * cscene->radixprec), nX_TRANSPARENTRADIXCONST );
	}

}


void nX::Trigone(float x0, float y0, float z0, float x1, float y1, float z1, float x2, float y2, float z2, nX_Surface *surf, float alpha)
{
	pfacelist->nedge = 3;
	pfacelist->surf = surf;
	pfacelist->alpha = alpha;

	if (pfacelist->alpha > 1.) pfacelist->alpha = 1.;
	pfacelist->vertice = pvertexlist;

	(pvertexlist + 0)->x = x0;
	(pvertexlist + 0)->y = y0;
	(pvertexlist + 0)->z = z0;
	(pvertexlist + 0)->u[0] = 0;
	(pvertexlist + 0)->v[0] = 0;
	(pvertexlist + 1)->x = x1;
	(pvertexlist + 1)->y = y1;
	(pvertexlist + 1)->z = z1;
	(pvertexlist + 1)->u[0] = 1;
	(pvertexlist + 1)->v[0] = 0;
	(pvertexlist + 2)->x = x2;
	(pvertexlist + 2)->y = y2;
	(pvertexlist + 2)->z = z2;
	(pvertexlist + 2)->u[0] = 1;
	(pvertexlist + 2)->v[0] = 1;

	// get farthest z
	unsigned long	farth = (unsigned long)z0;
	if (z1 > farth) farth = (unsigned long)z1;
	if (z2 > farth) farth = (unsigned long)z2;

	if (ClipFace())
		ValidFace(unsigned long(farth), nX_OPAQUERADIXCONST);

}

void nX::Square(float sx, float sy, float ex, float ey, float z, long msort, nX_Surface *surf, float alpha, nXcolor *diff)
{

	pfacelist->nedge = 4;
	pfacelist->surf = surf;
	pfacelist->alpha = alpha;
	if (pfacelist->alpha > 1.) pfacelist->alpha = 1.;
	pfacelist->vertice = pvertexlist;
	if (diff)
		nXcolcpy(&pfacelist->diff, diff);

	(pvertexlist + 0)->x = sx;
	(pvertexlist + 0)->y = sy;
	(pvertexlist + 0)->z = z;
	(pvertexlist + 0)->u[0] = 0;
	(pvertexlist + 0)->v[0] = 0;
	(pvertexlist + 1)->x = ex;
	(pvertexlist + 1)->y = sy;
	(pvertexlist + 1)->z = z;
	(pvertexlist + 1)->u[0] = 1;
	(pvertexlist + 1)->v[0] = 0;
	(pvertexlist + 2)->x = ex;
	(pvertexlist + 2)->y = ey;
	(pvertexlist + 2)->z = z;
	(pvertexlist + 2)->u[0] = 1;
	(pvertexlist + 2)->v[0] = 1;
	(pvertexlist + 3)->x = sx;
	(pvertexlist + 3)->y = ey;
	(pvertexlist + 3)->z = z;
	(pvertexlist + 3)->u[0] = 0;
	(pvertexlist + 3)->v[0] = 1;

	if (ClipFace2D())
	{
		if (alpha > 0.0)
				ValidFace(unsigned long(z * msort), nX_TRANSPARENTRADIXCONST);
		else	ValidFace(unsigned long(z * msort), nX_OPAQUERADIXCONST);
	}

}

void nX::GaussianPlane(float sx, float sy, float ex, float ey, float r, float alpha, nX_Surface *s)
{
	float	cx = (sx + ex) / 2, ucx;
	float	cy = (sy + ey) / 2, ucy;
	long	w = long( (ex - sx) / 2 );
	long	h = long( (ey - sy) / 2 );

	// draw 4 circles of 15 planes each...
	float	a, z = 10.0;
	float	mul = 0.7f;
	long	nc, npinc;

	for (nc = 0; nc < 3; nc++)
	{
		for (npinc = 0; npinc < 9; npinc++)
		{
			a = float( (2 * digitPI) / 10.0 * npinc );
			ucx = float( cx + sin(a) * r * mul );
			ucy = float( cy + cos(a) * r * mul );
			Square(ucx - w, ucy - h, ucx + w, ucy + h, z, 1, s, alpha, NULL);
			z += 0.5;
		}
		mul += 0.7f;
	}
}

void nX::Plane(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3, float z, long msort, nX_Surface *surf, float alpha)
{
	pfacelist->nedge = 4;
	pfacelist->surf = surf;
	pfacelist->alpha = alpha;
	if (pfacelist->alpha > 1.) pfacelist->alpha = 1.;
	pfacelist->vertice = pvertexlist;

	(pvertexlist + 0)->x = x0;
	(pvertexlist + 0)->y = y0;
	(pvertexlist + 0)->z = z;
	(pvertexlist + 0)->u[0] = 0;
	(pvertexlist + 0)->v[0] = 0;
	(pvertexlist + 1)->x = x1;
	(pvertexlist + 1)->y = y1;
	(pvertexlist + 1)->z = z;
	(pvertexlist + 1)->u[0] = 1;
	(pvertexlist + 1)->v[0] = 0;
	(pvertexlist + 2)->x = x2;
	(pvertexlist + 2)->y = y2;
	(pvertexlist + 2)->z = z;
	(pvertexlist + 2)->u[0] = 1;
	(pvertexlist + 2)->v[0] = 1;
	(pvertexlist + 3)->x = x3;
	(pvertexlist + 3)->y = y3;
	(pvertexlist + 3)->z = z;
	(pvertexlist + 3)->u[0] = 0;
	(pvertexlist + 3)->v[0] = 1;

	if (ClipFace2D())
		ValidFace(unsigned long(z * msort), nX_OPAQUERADIXCONST);

}