	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		nXng Scene BLOOM implementation file

	----------------------------------------------------------------------------*/


	#include	<math.h>
	#include	"nX_HeaderList.h"


#ifdef	nX_ENABLEBLOOM
#define		ZOFF7			5

void	nX::ProcessBloom()
{

	nX_Surface		*surface;
	nX_vertex		*pvertex;

	long			c2, nedge;
	long			mnx, mny, mxx, mxy;
	long			cx, cy, cz;
	unsigned long	i, m;
	nX_face			*wfacelist = facelist;
	unsigned long	lbface = buildedface, c;

	if (lbface > 0)
	{
		for (c = 0; c < lbface; c++)
		{
			nedge = wfacelist->nedge;
			surface = wfacelist->surf;
			pvertex = wfacelist->vertice;

			// Lighted faces could cause BLOOM to occur
			if ( (surface->CAPS & nX_GOURAUD) || (surface->CAPS & nX_LIGHTSOURCE) )
			{
				if (surface->CAPS & nX_GOURAUD)
				{
					pfacelist->diff.r = (unsigned char)(pvertex->diff.r / 12);
					pfacelist->diff.g = (unsigned char)(pvertex->diff.g / 12);
					pfacelist->diff.b = (unsigned char)(pvertex->diff.b / 12);
				}
				else
				{
					pfacelist->diff.r = (unsigned char)(pfacelist->diff.r / 12);
					pfacelist->diff.g = (unsigned char)(pfacelist->diff.g / 12);
					pfacelist->diff.b = (unsigned char)(pfacelist->diff.b / 12);
				}
				i =		(
							pfacelist->diff.r * pfacelist->diff.r +
							pfacelist->diff.g * pfacelist->diff.g +
							pfacelist->diff.b * pfacelist->diff.b
						);

				if (i > 100)
				{
					// Compute polygone area
					cx = (long)pvertex[0].x;
					cy = (long)pvertex[0].y;
					cz = (long)pvertex[0].z;
					mnx = mxx = cx;
					mny = mxy = cy;
					for (c2 = 1; c2 < nedge; c2++)
					{
						cx += (long)pvertex[c2].x;
						cy += (long)pvertex[c2].y;
						cz += (long)pvertex[c2].z;
						if (pvertex[c2].x > mxx)	mxx = (long)pvertex[c2].x;
						if (pvertex[c2].x < mnx)	mnx = (long)pvertex[c2].x;
						if (pvertex[c2].y > mxy)	mxy = (long)pvertex[c2].y;
						if (pvertex[c2].y < mny)	mny = (long)pvertex[c2].y;
					}
					cx /= nedge;
					cy /= nedge;
					cz /= nedge;
					m = (mxx - mnx) + (mxy - mny);
//					m += (m >> 1);

					pfacelist->nedge = 4;
					pfacelist->surf = DefaultBLOOMSurface;
					pfacelist->alpha = 0;
					pfacelist->vertice = pvertexlist;

					if (surface->CAPS & nX_GOURAUD)
					{
						pfacelist->diff.r = (unsigned char)(pvertex->diff.r / 8);
						pfacelist->diff.g = (unsigned char)(pvertex->diff.g / 8);
						pfacelist->diff.b = (unsigned char)(pvertex->diff.b / 8);
					}
					else
					{
						pfacelist->diff.r = (unsigned char)(pfacelist->diff.r / 8);
						pfacelist->diff.g = (unsigned char)(pfacelist->diff.g / 8);
						pfacelist->diff.b = (unsigned char)(pfacelist->diff.b / 8);
					}

					(pvertexlist + 0)->x = float(cx - m);
					(pvertexlist + 0)->y = float(cy - m);
					(pvertexlist + 0)->z = float(cz - ZOFF7);
					(pvertexlist + 0)->u[nXML_COLOR] = 0.0;
					(pvertexlist + 0)->v[nXML_COLOR] = 0.0;

					(pvertexlist + 1)->x = float(cx + m);
					(pvertexlist + 1)->y = float(cy - m);
					(pvertexlist + 1)->z = float(cz - ZOFF7);
					(pvertexlist + 1)->u[nXML_COLOR] = 1.0;
					(pvertexlist + 1)->v[nXML_COLOR] = 0.0;

					(pvertexlist + 2)->x = float(cx + m);
					(pvertexlist + 2)->y = float(cy + m);
					(pvertexlist + 2)->z = float(cz - ZOFF7);
					(pvertexlist + 2)->u[nXML_COLOR] = 1.0;
					(pvertexlist + 2)->v[nXML_COLOR] = 1.0;

					(pvertexlist + 3)->x = float(cx - m);
					(pvertexlist + 3)->y = float(cy + m);
					(pvertexlist + 3)->z = float(cz - ZOFF7);
					(pvertexlist + 3)->u[nXML_COLOR] = 0.0;
					(pvertexlist + 3)->v[nXML_COLOR] = 1.0;

					pvertexlist += 4;
					ValidFace( unsigned long((cz - ZOFF7) * cscene->radixprec), nX_TRANSPARENTRADIXCONST);
				}

			}
			wfacelist++;
		}

	}
}

#endif