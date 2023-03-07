	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		nXng Entity Easy Lens Flare implementation file

	----------------------------------------------------------------------------*/


	#include	<math.h>
	#include	"nX_HeaderList.h"
	static		char	str[256];


void	nX_Entity::LensFX(float x, float y, float z, nX_Surface *s)
{

	float	m;
	if (FlareInt)
			m = scene->mulfactor * FlareInt->values[0] * 0.14f;
	else	m = scene->mulfactor * 0.14f;
	nX		*pnX = scene->pnX;


	pnX->pfacelist->nedge = 4;
	pnX->pfacelist->surf = s;
	pnX->pfacelist->alpha = 0;
	pnX->pfacelist->vertice = pnX->pvertexlist;
	pnX->pfacelist->diff.r = 200;
	pnX->pfacelist->diff.g = 170;
	pnX->pfacelist->diff.b = 120;

	(pnX->pvertexlist + 0)->x = x - m;
	(pnX->pvertexlist + 0)->y = y - m;
	(pnX->pvertexlist + 0)->z = z;
	(pnX->pvertexlist + 0)->u[0] = 0.0;
	(pnX->pvertexlist + 0)->v[0] = 0.0;

	(pnX->pvertexlist + 1)->x = x + m;
	(pnX->pvertexlist + 1)->y = y - m;
	(pnX->pvertexlist + 1)->z = z;
	(pnX->pvertexlist + 1)->u[0] = 1.0;
	(pnX->pvertexlist + 1)->v[0] = 0.0;

	(pnX->pvertexlist + 2)->x = x + m;
	(pnX->pvertexlist + 2)->y = y + m;
	(pnX->pvertexlist + 2)->z = z;
	(pnX->pvertexlist + 2)->u[0] = 1.0;
	(pnX->pvertexlist + 2)->v[0] = 1.0;

	(pnX->pvertexlist + 3)->x = x - m;
	(pnX->pvertexlist + 3)->y = y + m;
	(pnX->pvertexlist + 3)->z = z;
	(pnX->pvertexlist + 3)->u[0] = 0.0;
	(pnX->pvertexlist + 3)->v[0] = 1.0;

	if (pnX->ClipFace())
		pnX->ValidFace( unsigned long(z * scene->radixprec), pnX->nX_TRANSPARENTRADIXCONST);

	pnX->pfacelist->nedge = 4;
	pnX->pfacelist->surf = s;
	pnX->pfacelist->alpha = ((float)rand() / RAND_MAX) / 4;
	pnX->pfacelist->vertice = pnX->pvertexlist;
	pnX->pfacelist->diff.r = 240;
	pnX->pfacelist->diff.g = 220;
	pnX->pfacelist->diff.b = 150;

	m *= 0.25;
	(pnX->pvertexlist + 0)->x = x - m;
	(pnX->pvertexlist + 0)->y = y - m;
	(pnX->pvertexlist + 0)->z = z;
	(pnX->pvertexlist + 0)->u[0] = 0.0;
	(pnX->pvertexlist + 0)->v[0] = 0.0;

	(pnX->pvertexlist + 1)->x = x + m;
	(pnX->pvertexlist + 1)->y = y - m;
	(pnX->pvertexlist + 1)->z = z;
	(pnX->pvertexlist + 1)->u[0] = 1.0;
	(pnX->pvertexlist + 1)->v[0] = 0.0;

	(pnX->pvertexlist + 2)->x = x + m;
	(pnX->pvertexlist + 2)->y = y + m;
	(pnX->pvertexlist + 2)->z = z;
	(pnX->pvertexlist + 2)->u[0] = 1.0;
	(pnX->pvertexlist + 2)->v[0] = 1.0;

	(pnX->pvertexlist + 3)->x = x - m;
	(pnX->pvertexlist + 3)->y = y + m;
	(pnX->pvertexlist + 3)->z = z;
	(pnX->pvertexlist + 3)->u[0] = 0.0;
	(pnX->pvertexlist + 3)->v[0] = 1.0;

	if (pnX->ClipFace())
		pnX->ValidFace( unsigned long(z * scene->radixprec), pnX->nX_TRANSPARENTRADIXCONST);
}
