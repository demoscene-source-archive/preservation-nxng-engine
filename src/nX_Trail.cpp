	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		nXng Entity trail implementation file

	----------------------------------------------------------------------------*/


	#include	<math.h>
	#include	"nX_HeaderList.h"


#ifdef	nX_ENABLETRAIL

#define	nX_TRAILDELAY		4	// update every 4 frames

void	nX_Entity::ComputeTrail()
{
	nX_Trail		*pt;
	float			*vtx = edge_adress;
	nX_reliure		*prel = reliure_adress;
	unsigned long	c, tc;

	for (c = 0; c < nbface; c++)
	{
		if (prel->surf->xtdCAPS & nX_TRAIL)
		{
			if (!traillist)		// allocate root trail polygon
			{
				pt = new nX_Trail();
				memset(pt, 0, sizeof(nX_Trail));
				traillist = pt;
			}
			else			// allocate next trail polygon
			{
				pt->next = new nX_Trail();
				pt = pt->next;
				memset(pt, 0, sizeof(nX_Trail));
			}

			// fill freshly newed struct
			pt->prel = prel;

			for (tc = 0; tc < nX_MAXTRAILLENGTH; tc++)
				pt->trail_coord[tc] = new float[prel->nedge * 3];
		}
		prel++;
	}
}

void	nX_Entity::DrawTrail(float *tvtx)
{

	nX_Trail		*pt = traillist;
	unsigned long	tc, ec;
	float			z;
	float			i, di;

	while (pt)
	{
		// decay...
		for (tc = (nX_MAXTRAILLENGTH - 1); tc > 0; tc--)

			for (ec = 0; ec < pt->prel->nedge; ec++)
			{
				pt->trail_coord[tc][ec * 3 + 0] = pt->trail_coord[tc - 1][ec * 3 + 0];
				pt->trail_coord[tc][ec * 3 + 1] = pt->trail_coord[tc - 1][ec * 3 + 1];
				pt->trail_coord[tc][ec * 3 + 2] = pt->trail_coord[tc - 1][ec * 3 + 2];
			}

		// new position
		for (ec = 0; ec < pt->prel->nedge; ec++)
		{
			pt->trail_coord[0][ec * 3 + 0] = tvtx[pt->prel->index[ec] + 0];
			pt->trail_coord[0][ec * 3 + 1] = tvtx[pt->prel->index[ec] + 1];
			pt->trail_coord[0][ec * 3 + 2] = tvtx[pt->prel->index[ec] + 2];
		}
		if (pt->nbcoords < nX_MAXTRAILLENGTH)
			pt->nbcoords++;

		// build polygons
		if (pt->nbcoords > 1)
		{
			i = 0.0f;
			di = (1.0f - i) / (pt->nbcoords - 1);

			for (tc = 0; tc < (pt->nbcoords - 1); tc++)
			{
				pnX->pfacelist->nedge = 4;
				pnX->pfacelist->surf = pt->prel->surf;
				pnX->pfacelist->alpha = 0;//i;
				pnX->pfacelist->vertice = pnX->pvertexlist;

				(pnX->pvertexlist + 0)->x = pt->trail_coord[tc + 0][0 * 3 + 0];
				(pnX->pvertexlist + 0)->y = pt->trail_coord[tc + 0][0 * 3 + 1];
			z = (pnX->pvertexlist + 0)->z = pt->trail_coord[tc + 0][0 * 3 + 2];
				(pnX->pvertexlist + 0)->u[nXML_COLOR] = 0.0f;
				(pnX->pvertexlist + 0)->v[nXML_COLOR] = i;

				(pnX->pvertexlist + 1)->x = pt->trail_coord[tc + 0][1 * 3 + 0];
				(pnX->pvertexlist + 1)->y = pt->trail_coord[tc + 0][1 * 3 + 1];
				(pnX->pvertexlist + 1)->z = pt->trail_coord[tc + 0][1 * 3 + 2];
				(pnX->pvertexlist + 1)->u[nXML_COLOR] = 1.0f;
				(pnX->pvertexlist + 1)->v[nXML_COLOR] = i;

				(pnX->pvertexlist + 2)->x = pt->trail_coord[tc + 1][1 * 3 + 0];
				(pnX->pvertexlist + 2)->y = pt->trail_coord[tc + 1][1 * 3 + 1];
				(pnX->pvertexlist + 2)->z = pt->trail_coord[tc + 1][1 * 3 + 2];
				(pnX->pvertexlist + 2)->u[nXML_COLOR] = 1.0f;
				(pnX->pvertexlist + 2)->v[nXML_COLOR] = i + di;

				(pnX->pvertexlist + 3)->x = pt->trail_coord[tc + 1][0 * 3 + 0];
				(pnX->pvertexlist + 3)->y = pt->trail_coord[tc + 1][0 * 3 + 1];
				(pnX->pvertexlist + 3)->z = pt->trail_coord[tc + 1][0 * 3 + 2];
				(pnX->pvertexlist + 3)->u[nXML_COLOR] = 0.0f;
				(pnX->pvertexlist + 3)->v[nXML_COLOR] = i + di;

				if (pnX->ClipFace())
					pnX->ValidFace( unsigned long(z * scene->radixprec), pnX->nX_TRANSPARENTRADIXCONST);

				i += di;
			}
		}

		pt = pt->next;
	}


}

void	nX_Entity::FreeTrail()
{
	nX_Trail		*pt = traillist;
	unsigned long	tc;

	while (pt)
	{
		for (tc = 0; tc < nX_MAXTRAILLENGTH; tc++)
			delete (pt->trail_coord[tc]);
		pt = pt->next;
	}
}

#endif