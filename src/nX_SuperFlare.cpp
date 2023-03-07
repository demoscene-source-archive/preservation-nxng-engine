	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		nXng Entity Super Lens Flare implementation file

	----------------------------------------------------------------------------*/


	#include	<math.h>
	#include	"nX_HeaderList.h"

	static		char	str[256];


#ifdef	nX_SPLENSFX
#define	SLENSFXNB_RAY	48			// 48
#define	SLENSFXNB_FQRAY	6			// 8

void	nX_Entity::SpLensFX(float x, float y, float z, nX_Surface *s)
{

/*	ADAPTIVE VERSION
	float	qf = z / 100.0;
	if (qf > 3)
		qf = 3;
	long	SLENSFXNB_RAY = 48 / qf;
	long	SLENSFXNB_FQRAY = 8;
	if (SLENSFXNB_RAY <= 1)
		SLENSFXNB_RAY = 1;
	if (SLENSFXNB_FQRAY <= 1)
		SLENSFXNB_FQRAY = 1;	*/

	float	m;
	if (FlareInt)
			m = scene->mulfactor * FlareInt->values[0] * 1.5f;
	else	m = scene->mulfactor;

	float	cf = (3.1415927f * 2) / SLENSFXNB_RAY;
	float	a = scene->frame / 5.0f;
	float	cs;
	nX		*pnX = scene->pnX;
	float	alpha = float(rand()) / RAND_MAX;
	alpha /= 4;

	nXcolor		col;
	if (type == nXETYPE_LIGHT)
	{	col.r = lightcolor.r;
		col.g = lightcolor.g;
		col.b = lightcolor.b;
		col.a = 0;	}
	else
	{	col.r = 200;
		col.g = 180;
		col.b = 120;
		col.a = 0;	}

	for (long c = 0; c < SLENSFXNB_RAY; c++)
	{

		pnX->pfacelist->nedge = 3;
		pnX->pfacelist->surf = s;
		pnX->pfacelist->alpha = 0;
		pnX->pfacelist->vertice = pnX->pvertexlist;
		pnX->pfacelist->diff = col;

		(pnX->pvertexlist + 0)->x = x;
		(pnX->pvertexlist + 0)->y = y;
		(pnX->pvertexlist + 0)->z = z;
		(pnX->pvertexlist + 0)->u[0] = 0.5f;
		(pnX->pvertexlist + 0)->v[0] = 0.5f;

		cs = (float)cos( (cf * c) * SLENSFXNB_FQRAY + a ) * 0.25f + 0.75f;
		(pnX->pvertexlist + 1)->x = x + (float)cos( cf * c ) * m * cs;
		(pnX->pvertexlist + 1)->y = y + (float)sin( cf * c ) * m * cs;
		(pnX->pvertexlist + 1)->z = z;
		(pnX->pvertexlist + 1)->u[0] = 0.5f + (float)cos( cf * c ) * 0.5f;
		(pnX->pvertexlist + 1)->v[0] = 0.5f + (float)sin( cf * c ) * 0.5f;

		cs = (float)cos( (cf * c + cf) * SLENSFXNB_FQRAY + a ) * 0.25f + 0.75f;
		(pnX->pvertexlist + 2)->x = x + (float)cos( cf * c + cf ) * m * cs;
		(pnX->pvertexlist + 2)->y = y + (float)sin( cf * c + cf ) * m * cs;
		(pnX->pvertexlist + 2)->z = z;
		(pnX->pvertexlist + 2)->u[0] = 0.5f + (float)cos( cf * c + cf ) * 0.5f;
		(pnX->pvertexlist + 2)->v[0] = 0.5f + (float)sin( cf * c + cf ) * 0.5f;

		if (pnX->ClipFace())
			pnX->ValidFace( unsigned long(z * scene->radixprec), pnX->nX_TRANSPARENTRADIXCONST);

		pnX->pfacelist->nedge = 3;
		pnX->pfacelist->surf = s;
		pnX->pfacelist->alpha = alpha;
		pnX->pfacelist->vertice = pnX->pvertexlist;
		pnX->pfacelist->diff = col;
		if ( (long(pnX->pfacelist->diff.r) + 150) > 255)
				pnX->pfacelist->diff.r = 255;
		else	pnX->pfacelist->diff.r += 150;
		if ( (long(pnX->pfacelist->diff.g) + 150) > 255)
				pnX->pfacelist->diff.g = 255;
		else	pnX->pfacelist->diff.g += 150;
		if ( (long(pnX->pfacelist->diff.b) + 150) > 255)
				pnX->pfacelist->diff.b = 255;
		else	pnX->pfacelist->diff.b += 150;

		(pnX->pvertexlist + 0)->x = x;
		(pnX->pvertexlist + 0)->y = y;
		(pnX->pvertexlist + 0)->z = z;
		(pnX->pvertexlist + 0)->u[0] = 0.5f;
		(pnX->pvertexlist + 0)->v[0] = 0.5f;

		cs = (float)cos( (cf * c) * SLENSFXNB_FQRAY * 2 - a ) * 0.25f + 0.5f;
		(pnX->pvertexlist + 1)->x = x + (float)cos( cf * c ) * m * cs;
		(pnX->pvertexlist + 1)->y = y + (float)sin( cf * c ) * m * cs;
		(pnX->pvertexlist + 1)->z = z;
		(pnX->pvertexlist + 1)->u[0] = 0.5f + (float)cos( cf * c ) * 0.5f;
		(pnX->pvertexlist + 1)->v[0] = 0.5f + (float)sin( cf * c ) * 0.5f;

		cs = (float)cos( (cf * c + cf) * SLENSFXNB_FQRAY * 2 - a ) * 0.25f + 0.5f;
		(pnX->pvertexlist + 2)->x = x + (float)cos( cf * c + cf ) * m * cs;
		(pnX->pvertexlist + 2)->y = y + (float)sin( cf * c + cf ) * m * cs;
		(pnX->pvertexlist + 2)->z = z;
		(pnX->pvertexlist + 2)->u[0] = 0.5f + (float)cos( cf * c + cf ) * 0.5f;
		(pnX->pvertexlist + 2)->v[0] = 0.5f + (float)sin( cf * c + cf ) * 0.5f;

		if (pnX->ClipFace())
			pnX->ValidFace( unsigned long(z * scene->radixprec), pnX->nX_TRANSPARENTRADIXCONST);

	}

}

#endif