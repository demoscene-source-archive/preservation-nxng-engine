	#include	"nXng/nX_HeaderList.h"


float inline cclipvalue(float v0, float v1, float coef, float z0, float z1, float zclip)
{
	v0/=z0;
	v1/=z1;
	return ((v1-v0)*coef + v0) * zclip;
}

float inline clipvalue(float v0, float v1, float coef)
{
	return (v1-v0)*coef+v0;
}

float inline zclipvalue(float v0, float v1, float coef)
{
	v0=1/v0;
	v1=1/v1;
	return 1 / ((v1-v0)*coef + v0);
}


void inline	ClipCAPS(nX_Surface *surf, nX_vertex *pworkvertex, nX_vertex *pvertex, float coef)
{

	if (surf->CAPS & nX_MAPPED)
	{
		pworkvertex->u[nXML_COLOR] = clipvalue((pvertex+1)->u[nXML_COLOR], pvertex->u[nXML_COLOR], coef);
		pworkvertex->v[nXML_COLOR] = clipvalue((pvertex+1)->v[nXML_COLOR], pvertex->v[nXML_COLOR], coef);
	}
	if (surf->CAPS & nX_EVMAP)
	{
		pworkvertex->u[nXML_EVMAP] = clipvalue((pvertex+1)->u[nXML_EVMAP], pvertex->u[nXML_EVMAP], coef);
		pworkvertex->v[nXML_EVMAP] = clipvalue((pvertex+1)->v[nXML_EVMAP], pvertex->v[nXML_EVMAP], coef);
	}
	if (surf->CAPS & nX_ALPHATEX)
	{
		pworkvertex->u[nXML_TRANSPARENCY] = clipvalue((pvertex+1)->u[nXML_TRANSPARENCY], pvertex->u[nXML_TRANSPARENCY], coef);
		pworkvertex->v[nXML_TRANSPARENCY] = clipvalue((pvertex+1)->v[nXML_TRANSPARENCY], pvertex->v[nXML_TRANSPARENCY], coef);
	}
//	if (surf->CAPS & nX_LIGHTMAP)
	{
		pworkvertex->u[nXML_LIGHTMAP] = clipvalue((pvertex+1)->u[nXML_LIGHTMAP], pvertex->u[nXML_LIGHTMAP], coef);
		pworkvertex->v[nXML_LIGHTMAP] = clipvalue((pvertex+1)->v[nXML_LIGHTMAP], pvertex->v[nXML_LIGHTMAP], coef);
	}
	if (surf->CAPS & nX_GOURAUD)
	{
		pworkvertex->diff.r = (uchar) clipvalue((pvertex+1)->diff.r, pvertex->diff.r, coef);
		pworkvertex->diff.g = (uchar) clipvalue((pvertex+1)->diff.g, pvertex->diff.g, coef);
		pworkvertex->diff.b = (uchar) clipvalue((pvertex+1)->diff.b, pvertex->diff.b, coef);
	}
	if (surf->xtdCAPS & nX_SPECULAR)
	{
		pworkvertex->spec.r = (uchar) clipvalue((pvertex+1)->spec.r, pvertex->spec.r, coef);
		pworkvertex->spec.g = (uchar) clipvalue((pvertex+1)->spec.g, pvertex->spec.g, coef);
		pworkvertex->spec.b = (uchar) clipvalue((pvertex+1)->spec.b, pvertex->spec.b, coef);
	}

}

void inline	cClipCAPS(nX_Surface *surf, nX_vertex *pworkvertex, nX_vertex *pvertex, float coef)
{

	if (surf->CAPS & nX_MAPPED)
	{
		pworkvertex->u[nXML_COLOR] = cclipvalue((pvertex+1)->u[nXML_COLOR], pvertex->u[nXML_COLOR], coef, (pvertex+1)->z, pvertex->z, (pworkvertex)->z);
		pworkvertex->v[nXML_COLOR] = cclipvalue((pvertex+1)->v[nXML_COLOR], pvertex->v[nXML_COLOR], coef, (pvertex+1)->z, pvertex->z, (pworkvertex)->z);
	}
	if (surf->CAPS & nX_EVMAP)
	{
		pworkvertex->u[nXML_EVMAP] = cclipvalue((pvertex+1)->u[nXML_EVMAP], pvertex->u[nXML_EVMAP], coef, (pvertex+1)->z, pvertex->z, (pworkvertex)->z);
		pworkvertex->v[nXML_EVMAP] = cclipvalue((pvertex+1)->v[nXML_EVMAP], pvertex->v[nXML_EVMAP], coef, (pvertex+1)->z, pvertex->z, (pworkvertex)->z);
	}
	if (surf->CAPS & nX_ALPHATEX)
	{
		pworkvertex->u[nXML_TRANSPARENCY] = cclipvalue((pvertex+1)->u[nXML_TRANSPARENCY], pvertex->u[nXML_TRANSPARENCY], coef, (pvertex+1)->z, pvertex->z, (pworkvertex)->z);
		pworkvertex->v[nXML_TRANSPARENCY] = cclipvalue((pvertex+1)->v[nXML_TRANSPARENCY], pvertex->v[nXML_TRANSPARENCY], coef, (pvertex+1)->z, pvertex->z, (pworkvertex)->z);
	}
//	if (surf->CAPS & nX_LIGHTMAP)
	{
		pworkvertex->u[nXML_LIGHTMAP] = cclipvalue((pvertex+1)->u[nXML_LIGHTMAP], pvertex->u[nXML_LIGHTMAP], coef, (pvertex+1)->z, pvertex->z, (pworkvertex)->z);
		pworkvertex->v[nXML_LIGHTMAP] = cclipvalue((pvertex+1)->v[nXML_LIGHTMAP], pvertex->v[nXML_LIGHTMAP], coef, (pvertex+1)->z, pvertex->z, (pworkvertex)->z);
	}
	if (surf->CAPS & nX_GOURAUD)
	{
		pworkvertex->diff.r = (uchar) clipvalue((pvertex+1)->diff.r, pvertex->diff.r, coef);
		pworkvertex->diff.g = (uchar) clipvalue((pvertex+1)->diff.g, pvertex->diff.g, coef);
		pworkvertex->diff.b = (uchar) clipvalue((pvertex+1)->diff.b, pvertex->diff.b, coef);
	}
	if (surf->xtdCAPS & nX_SPECULAR)
	{
		pworkvertex->spec.r = (uchar) clipvalue((pvertex+1)->spec.r, pvertex->spec.r, coef);
		pworkvertex->spec.g = (uchar) clipvalue((pvertex+1)->spec.g, pvertex->spec.g, coef);
		pworkvertex->spec.b = (uchar) clipvalue((pvertex+1)->spec.b, pvertex->spec.b, coef);
	}

}


/*

	ClipFace(long)

	Face is assumed to be already in table! The function will increment
	the global face pointer if the face is validated. If the face do
	not pass the clipping then it will be automatically discarded
	>> extra infos: discarded faces stay on facelist but are overwritten
	   by any new face.
		--> Return true if face was valid, false else.

*/


		static nX_vertex
					workvertex0[nX_MAXEDGEPOLY],
					workvertex1[nX_MAXEDGEPOLY];			// scratch buffers


bool	nX::ClipFace()
{

		nX_vertex	*pvertex = pfacelist->vertice;			// get current face vertex list
		nX_Surface	*surf = pfacelist->surf;
		nX_vertex	*pworkvertex0, *pworkvertex1;

		long		nedge, c;
		float		coef;
		nedge = pfacelist->nedge;

		// make a quick test to avoid useless clipping


			// TODO? hmmm not sure that would improve performance...


		// copy first vertex to last position

		memcpy(pvertex + nedge, pvertex, sizeof(nX_vertex));

		// Z-cut
		////////

		pworkvertex0 = workvertex0;
		pfacelist->nedge = 0;

		for (c = 0; c < nedge; c++)
		{
			if ( pvertex->z > cscene->zcut && (pvertex+1)->z > cscene->zcut )
			{
				memcpy(pworkvertex0++, pvertex, sizeof(nX_vertex));
				pfacelist->nedge++;	
			}
			else if ( pvertex->z <= cscene->zcut && (pvertex+1)->z > cscene->zcut )
			{
				coef=float ( 1/ (pvertex->z-(pvertex+1)->z)*(cscene->zcut-(pvertex+1)->z) );

				pworkvertex0->x=clipvalue((pvertex+1)->x, pvertex->x, coef);
				pworkvertex0->y=clipvalue((pvertex+1)->y, pvertex->y, coef);
				pworkvertex0->z=cscene->zcut;
				ClipCAPS(surf, pworkvertex0, pvertex, coef);		// clip extra components

				pworkvertex0++;
				pfacelist->nedge++;	
			}
			else if ( pvertex->z > cscene->zcut && (pvertex+1)->z <= cscene->zcut )
			{
				coef=float ( 1/ (pvertex->z-(pvertex+1)->z)*(cscene->zcut-(pvertex+1)->z) );

				memcpy(pworkvertex0++, pvertex, sizeof(nX_vertex));
				pworkvertex0->x=clipvalue((pvertex+1)->x, pvertex->x, coef);
				pworkvertex0->y=clipvalue((pvertex+1)->y, pvertex->y, coef);
				pworkvertex0->z=cscene->zcut;
				ClipCAPS (surf, pworkvertex0, pvertex, coef);		// clip extra components

				pworkvertex0++;
				pfacelist->nedge+=2;
			}
			pvertex++;
		}

		if (pfacelist->nedge < nX_MINEDGEPOLY)
			return false;


		// PROJECTION 3D>>2D
		////////////////////

		pworkvertex0 = workvertex0;

		for (c = 0; c < pfacelist->nedge; c++)
		{
			coef = float (1.0 / (pworkvertex0->z * (cscene->Focal->values[0] + cscene->offocal) ));
			pworkvertex0->x *= coef * cscene->viewcx;
			pworkvertex0->x += cscene->viewportMidX;
			pworkvertex0->y *= coef * cscene->viewcy;
			pworkvertex0->y += cscene->viewportMidY;
			pworkvertex0++;
		}

		nedge = pfacelist->nedge;
		pworkvertex0 = workvertex0;
		memcpy(pworkvertex0 + nedge, pworkvertex0, sizeof(nX_vertex));

		// CLIPPING UP
		//////////////

		pworkvertex1 = workvertex1;
		pfacelist->nedge = 0;

		for (c = 0; c < nedge; c++)
		{
			if (pworkvertex0->y >= cscene->viewportUpClip && (pworkvertex0+1)->y >= cscene->viewportUpClip)
			{
				memcpy(pworkvertex1++, pworkvertex0, sizeof(nX_vertex));
				pfacelist->nedge++;	
			}
			else if (pworkvertex0->y < cscene->viewportUpClip && (pworkvertex0+1)->y >= cscene->viewportUpClip)
			{
				coef=float ( 1/ (pworkvertex0->y-(pworkvertex0+1)->y)*(cscene->viewportUpClip-(pworkvertex0+1)->y) );

				pworkvertex1->x=clipvalue((pworkvertex0+1)->x, pworkvertex0->x, coef);
				pworkvertex1->y=float (cscene->viewportUpClip);
				pworkvertex1->z=zclipvalue((pworkvertex0+1)->z, pworkvertex0->z, coef);
				cClipCAPS (surf, pworkvertex1, pworkvertex0, coef);		// z correct clip extra components

				pworkvertex1++;
				pfacelist->nedge++;	
			}
			else if (pworkvertex0->y >= cscene->viewportUpClip && (pworkvertex0+1)->y < cscene->viewportUpClip)
			{
				coef=float ( 1/ (pworkvertex0->y-(pworkvertex0+1)->y)*(cscene->viewportUpClip-(pworkvertex0+1)->y) );

				memcpy(pworkvertex1++, pworkvertex0, sizeof(nX_vertex));
				pworkvertex1->x=clipvalue((pworkvertex0+1)->x, pworkvertex0->x, coef);
				pworkvertex1->y=float (cscene->viewportUpClip);
				pworkvertex1->z=zclipvalue((pworkvertex0+1)->z, pworkvertex0->z, coef);
				cClipCAPS (surf, pworkvertex1, pworkvertex0, coef);		// z correct clip extra components

				pworkvertex1++;
				pfacelist->nedge += 2;	
			}
			pworkvertex0++;
		}

		if (pfacelist->nedge < nX_MINEDGEPOLY)
			return false;

		nedge=pfacelist->nedge;
		pworkvertex1=workvertex1;
		memcpy(pworkvertex1+nedge, pworkvertex1, sizeof(nX_vertex));

		// CLIPPING DOWN
		////////////////

		pworkvertex0=workvertex0;
		pfacelist->nedge=0;

		for (c=0; c<nedge; c++)
		{
			if (pworkvertex1->y <= cscene->viewportDownClip && (pworkvertex1+1)->y <= cscene->viewportDownClip)
			{
				memcpy(pworkvertex0++, pworkvertex1, sizeof(nX_vertex));
				pfacelist->nedge++;	
			}
			else if (pworkvertex1->y > cscene->viewportDownClip && (pworkvertex1+1)->y <= cscene->viewportDownClip)
			{
				coef=float ( 1/ (pworkvertex1->y-(pworkvertex1+1)->y)*(cscene->viewportDownClip-(pworkvertex1+1)->y) );

				pworkvertex0->x=clipvalue((pworkvertex1+1)->x, pworkvertex1->x, coef);
				pworkvertex0->y=float (cscene->viewportDownClip);
				pworkvertex0->z=zclipvalue((pworkvertex1+1)->z, pworkvertex1->z, coef);
				cClipCAPS (surf, pworkvertex0, pworkvertex1, coef);		// z correct clip extra components

				pworkvertex0++;
				pfacelist->nedge++;	
			}
			else if (pworkvertex1->y <= cscene->viewportDownClip && (pworkvertex1+1)->y > cscene->viewportDownClip)
			{
				coef=float ( 1/ (pworkvertex1->y-(pworkvertex1+1)->y)*(cscene->viewportDownClip-(pworkvertex1+1)->y) );

				memcpy(pworkvertex0++, pworkvertex1, sizeof(nX_vertex));
				pworkvertex0->x=clipvalue((pworkvertex1+1)->x, pworkvertex1->x, coef);
				pworkvertex0->y=float (cscene->viewportDownClip);
				pworkvertex0->z=zclipvalue((pworkvertex1+1)->z, pworkvertex1->z, coef);
				cClipCAPS (surf, pworkvertex0, pworkvertex1, coef);		// z correct clip extra components

				pworkvertex0++;
				pfacelist->nedge+=2;	
			}
			pworkvertex1++;
		}

		if (pfacelist->nedge < nX_MINEDGEPOLY)
			return false;

		nedge=pfacelist->nedge;
		pworkvertex0=workvertex0;
		memcpy(pworkvertex0+nedge, pworkvertex0, sizeof(nX_vertex));

		// CLIPPING LEFT
		////////////////

		pworkvertex1=workvertex1;
		pfacelist->nedge=0;

		for (c=0; c<nedge; c++)
		{
			if (pworkvertex0->x >= cscene->viewportLeftClip && (pworkvertex0+1)->x >= cscene->viewportLeftClip)
			{
				memcpy(pworkvertex1++, pworkvertex0, sizeof(nX_vertex));
				pfacelist->nedge++;	

			}
			else if (pworkvertex0->x < cscene->viewportLeftClip && (pworkvertex0+1)->x >= cscene->viewportLeftClip)
			{
				coef=float ( 1/ (pworkvertex0->x-(pworkvertex0+1)->x)*(cscene->viewportLeftClip-(pworkvertex0+1)->x) );

				pworkvertex1->x=float (cscene->viewportLeftClip);
				pworkvertex1->y=clipvalue((pworkvertex0+1)->y, pworkvertex0->y, coef);
				pworkvertex1->z=zclipvalue((pworkvertex0+1)->z, pworkvertex0->z, coef);
				cClipCAPS (surf, pworkvertex1, pworkvertex0, coef);		// z correct clip extra components

				pworkvertex1++;
				pfacelist->nedge++;	
			}
			else if (pworkvertex0->x >= cscene->viewportLeftClip && (pworkvertex0+1)->x < cscene->viewportLeftClip)
			{
				coef=float ( 1/ (pworkvertex0->x-(pworkvertex0+1)->x)*(cscene->viewportLeftClip-(pworkvertex0+1)->x) );

				memcpy(pworkvertex1++, pworkvertex0, sizeof(nX_vertex));
				pworkvertex1->x=float (cscene->viewportLeftClip);
				pworkvertex1->y=clipvalue((pworkvertex0+1)->y, pworkvertex0->y, coef);
				pworkvertex1->z=zclipvalue((pworkvertex0+1)->z, pworkvertex0->z, coef);
				cClipCAPS (surf, pworkvertex1, pworkvertex0, coef);		// z correct clip extra components

				pworkvertex1++;
				pfacelist->nedge+=2;	
			}
			pworkvertex0++;
		}

		if (pfacelist->nedge < nX_MINEDGEPOLY)
			return false;

		nedge=pfacelist->nedge;
		pworkvertex1=workvertex1;
		memcpy(pworkvertex1+nedge, pworkvertex1, sizeof(nX_vertex));

		// CLIPPING RIGHT
		/////////////////

		pworkvertex0=workvertex0;
		pfacelist->nedge=0;

		for (c=0; c<nedge; c++)
		{
			if (pworkvertex1->x <= cscene->viewportRightClip && (pworkvertex1+1)->x <= cscene->viewportRightClip)
			{
				memcpy(pworkvertex0++, pworkvertex1, sizeof(nX_vertex));
				pfacelist->nedge++;	

			}
			else if (pworkvertex1->x > cscene->viewportRightClip && (pworkvertex1+1)->x <= cscene->viewportRightClip)
			{
				coef=float ( 1/ (pworkvertex1->x-(pworkvertex1+1)->x)*(cscene->viewportRightClip-(pworkvertex1+1)->x) );

				pworkvertex0->x=float (cscene->viewportRightClip);
				pworkvertex0->y=clipvalue((pworkvertex1+1)->y, pworkvertex1->y, coef);
				pworkvertex0->z=zclipvalue((pworkvertex1+1)->z, pworkvertex1->z, coef);
				cClipCAPS (surf, pworkvertex0, pworkvertex1, coef);		// z correct clip extra components

				pworkvertex0++;
				pfacelist->nedge++;	
			}
			else if (pworkvertex1->x <= cscene->viewportRightClip && (pworkvertex1+1)->x > cscene->viewportRightClip)
			{
				coef=float ( 1/ (pworkvertex1->x-(pworkvertex1+1)->x)*(cscene->viewportRightClip-(pworkvertex1+1)->x) );

				memcpy(pworkvertex0++, pworkvertex1, sizeof(nX_vertex));
				pworkvertex0->x = float (cscene->viewportRightClip);
				pworkvertex0->y = clipvalue((pworkvertex1+1)->y, pworkvertex1->y, coef);
				pworkvertex0->z = zclipvalue((pworkvertex1+1)->z, pworkvertex1->z, coef);
				cClipCAPS (surf, pworkvertex0, pworkvertex1, coef);		// z correct clip extra components

				pworkvertex0++;
				pfacelist->nedge += 2;	
			}
			pworkvertex1++;
		}

		if (pfacelist->nedge < nX_MINEDGEPOLY)
			return false;

		// THE FACE WENT TROUGHT ALL CLIPPINGS
		//////////////////////////////////////

		pworkvertex0 = workvertex0;

		for (c=0; c<pfacelist->nedge; c++)
		{
			memcpy(pvertexlist, pworkvertex0++, sizeof (nX_vertex));
			pvertexlist->x *= screen->wcoef;
			pvertexlist->y *= screen->hcoef;
			pvertexlist++;
		}

		return true;

}

/*

	ClipSprite()

	A SPRITE IS 4 EDGE POLYGON!!!

	0------1
	|      |
	|      |
	|      |
	3------2

	Sprite is assumed to be already in table! The function will increment
	the global face pointer if the face is validated. If the face do
	not pass the clipping then it will be automatically discarded
	>> extra infos: discarded faces stay on facelist but are overwritten
	   by any new face.

	SPRITES MUST BE IN RENDER SPACE COORDS! X * screen.wCoef, Y * screen.hCoef
	ONLY COLOR UV IS CLIPPED! NO GOURAUD SHADING ON SPRITES WITH THIS ONE ):

		--> Return true if sprite is on screen, false else.

*/

bool	nX::ClipSprite()
{

		nX_vertex	*pvertex = pvertexlist;			// get current face vertex list
		float		lc = cscene->viewportLeftClip * screen->wcoef;
		float		rc = cscene->viewportRightClip * screen->wcoef;
		float		uc = cscene->viewportUpClip * screen->hcoef;
		float		dc = cscene->viewportDownClip * screen->hcoef;
		float		coef;

		// reject trivial cases...
		if (pvertex[1].x < lc)
			return false;
		if (pvertex[0].x > rc)
			return false;
		if (pvertex[0].y > dc)
			return false;
		if (pvertex[2].y < uc)
			return false;

		// <-- clip to the left
		if (pvertex[0].x < lc)
		{
			// clip UVs
			coef = float ( 1.0f / (pvertex[0].x - pvertex[1].x) * (lc - pvertex[1].x) );
			pvertex[0].u[nXML_COLOR] = clipvalue(pvertex[1].u[nXML_COLOR], pvertex[0].u[nXML_COLOR], coef);
			pvertex[0].v[nXML_COLOR] = clipvalue(pvertex[1].v[nXML_COLOR], pvertex[0].v[nXML_COLOR], coef);
			pvertex[3].u[nXML_COLOR] = clipvalue(pvertex[2].u[nXML_COLOR], pvertex[3].u[nXML_COLOR], coef);
			pvertex[3].v[nXML_COLOR] = clipvalue(pvertex[2].v[nXML_COLOR], pvertex[3].v[nXML_COLOR], coef);

			pvertex[0].x = lc;
			pvertex[3].x = lc;
		}

		// --> clip to the right
		if (pvertex[1].x > rc)
		{
			// clip UVs
			coef = float ( 1.0f / (pvertex[0].x - pvertex[1].x) * (rc - pvertex[1].x) );
			pvertex[1].u[nXML_COLOR] = clipvalue(pvertex[1].u[nXML_COLOR], pvertex[0].u[nXML_COLOR], coef);
			pvertex[1].v[nXML_COLOR] = clipvalue(pvertex[1].v[nXML_COLOR], pvertex[0].v[nXML_COLOR], coef);
			pvertex[2].u[nXML_COLOR] = clipvalue(pvertex[2].u[nXML_COLOR], pvertex[3].u[nXML_COLOR], coef);
			pvertex[2].v[nXML_COLOR] = clipvalue(pvertex[2].v[nXML_COLOR], pvertex[3].v[nXML_COLOR], coef);

			pvertex[1].x = rc;
			pvertex[2].x = rc;
		}

		// /\ clip up
		if (pvertex[0].y < uc)
		{
			// clip UVs
			coef = float ( 1.0f / (pvertex[0].y - pvertex[2].y) * (uc - pvertex[2].y) );
			pvertex[0].u[nXML_COLOR] = clipvalue(pvertex[3].u[nXML_COLOR], pvertex[0].u[nXML_COLOR], coef);
			pvertex[0].v[nXML_COLOR] = clipvalue(pvertex[3].v[nXML_COLOR], pvertex[0].v[nXML_COLOR], coef);
			pvertex[1].u[nXML_COLOR] = clipvalue(pvertex[2].u[nXML_COLOR], pvertex[1].u[nXML_COLOR], coef);
			pvertex[1].v[nXML_COLOR] = clipvalue(pvertex[2].v[nXML_COLOR], pvertex[1].v[nXML_COLOR], coef);

			pvertex[0].y = uc;
			pvertex[1].y = uc;
		}

		// \/ clip down
		if (pvertex[3].y > dc)
		{
			// clip UVs
			coef = float ( 1.0f / (pvertex[0].y - pvertex[2].y) * (dc - pvertex[2].y) );
			pvertex[0].u[nXML_COLOR] = clipvalue(pvertex[3].u[nXML_COLOR], pvertex[0].u[nXML_COLOR], coef);
			pvertex[0].v[nXML_COLOR] = clipvalue(pvertex[3].v[nXML_COLOR], pvertex[0].v[nXML_COLOR], coef);
			pvertex[1].u[nXML_COLOR] = clipvalue(pvertex[2].u[nXML_COLOR], pvertex[1].u[nXML_COLOR], coef);
			pvertex[1].v[nXML_COLOR] = clipvalue(pvertex[2].v[nXML_COLOR], pvertex[1].v[nXML_COLOR], coef);

			pvertex[3].y = dc;
			pvertex[2].y = dc;
		}

		// that's ok!
		pvertexlist += 4;
		return true;

}


/*

	ClipFace2D()

	Face is assumed to be already in table! The function will increment
	the global face pointer if the face is validated. If the face do
	not pass the clipping then it will be automatically discarded
	>> extra infos: discarded faces stay on facelist but are overwritten
	   by any new face.
		--> Return true if face was valid, false else.

*/


bool	nX::ClipFace2D()
{

		nX_vertex	*pvertex = pfacelist->vertice;			// get current face vertex list
		nX_Surface	*surf = pfacelist->surf;
		nX_vertex	*pworkvertex0, *pworkvertex1;

		long		nedge, c;
		float		coef;

		// vertice to work buffers
		nedge = pfacelist->nedge;
		pworkvertex0 = workvertex0;
		memcpy(pworkvertex0, pvertex, sizeof(nX_vertex) * nedge);
		memcpy(pworkvertex0 + nedge, pvertex, sizeof(nX_vertex));

		// CLIPPING UP
		//////////////

		pworkvertex1 = workvertex1;
		pfacelist->nedge = 0;

		for (c=0; c<nedge; c++)
		{
			if (pworkvertex0->y >= cscene->viewportUpClip && (pworkvertex0+1)->y >= cscene->viewportUpClip)
			{
				memcpy(pworkvertex1++, pworkvertex0, sizeof(nX_vertex));
				pfacelist->nedge++;	
			}
			else if (pworkvertex0->y < cscene->viewportUpClip && (pworkvertex0+1)->y >= cscene->viewportUpClip)
			{
				coef=float ( 1/ (pworkvertex0->y-(pworkvertex0+1)->y)*(cscene->viewportUpClip-(pworkvertex0+1)->y) );

				pworkvertex1->x = clipvalue((pworkvertex0+1)->x, pworkvertex0->x, coef);
				pworkvertex1->y = float (cscene->viewportUpClip);
				pworkvertex1->z = zclipvalue((pworkvertex0+1)->z, pworkvertex0->z, coef);
				cClipCAPS (surf, pworkvertex1, pworkvertex0, coef);		// z correct clip extra components

				pworkvertex1++;
				pfacelist->nedge++;	
			}
			else if (pworkvertex0->y >= cscene->viewportUpClip && (pworkvertex0+1)->y < cscene->viewportUpClip)
			{
				coef=float ( 1/ (pworkvertex0->y-(pworkvertex0+1)->y)*(cscene->viewportUpClip-(pworkvertex0+1)->y) );

				memcpy(pworkvertex1++, pworkvertex0, sizeof(nX_vertex));
				pworkvertex1->x = clipvalue((pworkvertex0+1)->x, pworkvertex0->x, coef);
				pworkvertex1->y = float (cscene->viewportUpClip);
				pworkvertex1->z = zclipvalue((pworkvertex0+1)->z, pworkvertex0->z, coef);
				cClipCAPS (surf, pworkvertex1, pworkvertex0, coef);		// z correct clip extra components

				pworkvertex1++;
				pfacelist->nedge+=2;	
			}
			pworkvertex0++;
		}

		if (pfacelist->nedge < nX_MINEDGEPOLY)
			return false;

		nedge = pfacelist->nedge;
		pworkvertex1 = workvertex1;
		memcpy(pworkvertex1 + nedge, pworkvertex1, sizeof(nX_vertex));

		// CLIPPING DOWN
		////////////////

		pworkvertex0 = workvertex0;
		pfacelist->nedge = 0;

		for (c=0; c<nedge; c++)
		{
			if (pworkvertex1->y < cscene->viewportDownClip && (pworkvertex1+1)->y < cscene->viewportDownClip)
			{
				memcpy(pworkvertex0++, pworkvertex1, sizeof(nX_vertex));
				pfacelist->nedge++;	
			}
			else if (pworkvertex1->y >= cscene->viewportDownClip && (pworkvertex1+1)->y < cscene->viewportDownClip)
			{
				coef=float ( 1/ (pworkvertex1->y-(pworkvertex1+1)->y)*(cscene->viewportDownClip-(pworkvertex1+1)->y) );

				pworkvertex0->x=clipvalue((pworkvertex1+1)->x, pworkvertex1->x, coef);
				pworkvertex0->y=float (cscene->viewportDownClip);
				pworkvertex0->z=zclipvalue((pworkvertex1+1)->z, pworkvertex1->z, coef);
				cClipCAPS (surf, pworkvertex0, pworkvertex1, coef);		// z correct clip extra components

				pworkvertex0++;
				pfacelist->nedge++;	
			}
			else if (pworkvertex1->y < cscene->viewportDownClip && (pworkvertex1+1)->y >= cscene->viewportDownClip)
			{
				coef=float ( 1/ (pworkvertex1->y-(pworkvertex1+1)->y)*(cscene->viewportDownClip-(pworkvertex1+1)->y) );

				memcpy(pworkvertex0++, pworkvertex1, sizeof(nX_vertex));
				pworkvertex0->x=clipvalue((pworkvertex1+1)->x, pworkvertex1->x, coef);
				pworkvertex0->y=float (cscene->viewportDownClip);
				pworkvertex0->z=zclipvalue((pworkvertex1+1)->z, pworkvertex1->z, coef);
				cClipCAPS (surf, pworkvertex0, pworkvertex1, coef);		// z correct clip extra components

				pworkvertex0++;
				pfacelist->nedge+=2;	
			}
			pworkvertex1++;
		}

		if (pfacelist->nedge < nX_MINEDGEPOLY)
			return false;

		nedge=pfacelist->nedge;
		pworkvertex0=workvertex0;
		memcpy(pworkvertex0+nedge, pworkvertex0, sizeof(nX_vertex));

		// CLIPPING LEFT
		////////////////

		pworkvertex1=workvertex1;
		pfacelist->nedge=0;

		for (c=0; c<nedge; c++)
		{
			if (pworkvertex0->x >= cscene->viewportLeftClip && (pworkvertex0+1)->x >= cscene->viewportLeftClip)
			{
				memcpy(pworkvertex1++, pworkvertex0, sizeof(nX_vertex));
				pfacelist->nedge++;	

			}
			else if (pworkvertex0->x < cscene->viewportLeftClip && (pworkvertex0+1)->x >= cscene->viewportLeftClip)
			{
				coef=float ( 1/ (pworkvertex0->x-(pworkvertex0+1)->x)*(cscene->viewportLeftClip-(pworkvertex0+1)->x) );

				pworkvertex1->x=float (cscene->viewportLeftClip);
				pworkvertex1->y=clipvalue((pworkvertex0+1)->y, pworkvertex0->y, coef);
				pworkvertex1->z=zclipvalue((pworkvertex0+1)->z, pworkvertex0->z, coef);
				cClipCAPS (surf, pworkvertex1, pworkvertex0, coef);		// z correct clip extra components

				pworkvertex1++;
				pfacelist->nedge++;	
			}
			else if (pworkvertex0->x >= cscene->viewportLeftClip && (pworkvertex0+1)->x < cscene->viewportLeftClip)
			{
				coef=float ( 1/ (pworkvertex0->x-(pworkvertex0+1)->x)*(cscene->viewportLeftClip-(pworkvertex0+1)->x) );

				memcpy(pworkvertex1++, pworkvertex0, sizeof(nX_vertex));
				pworkvertex1->x = float (cscene->viewportLeftClip);
				pworkvertex1->y = clipvalue((pworkvertex0+1)->y, pworkvertex0->y, coef);
				pworkvertex1->z = zclipvalue((pworkvertex0+1)->z, pworkvertex0->z, coef);
				cClipCAPS (surf, pworkvertex1, pworkvertex0, coef);		// z correct clip extra components

				pworkvertex1++;
				pfacelist->nedge += 2;
			}
			pworkvertex0++;
		}

		if (pfacelist->nedge < nX_MINEDGEPOLY)
			return false;

		nedge = pfacelist->nedge;
		pworkvertex1 = workvertex1;
		memcpy(pworkvertex1+nedge, pworkvertex1, sizeof nX_vertex);

		// CLIPPING RIGHT
		/////////////////

		pworkvertex0=workvertex0;
		pfacelist->nedge=0;

		for (c = 0; c < nedge; c++)
		{
			if (pworkvertex1->x < cscene->viewportRightClip && (pworkvertex1+1)->x < cscene->viewportRightClip)
			{
				memcpy(pworkvertex0++, pworkvertex1, sizeof(nX_vertex));
				pfacelist->nedge++;	

			}
			else if (pworkvertex1->x >= cscene->viewportRightClip && (pworkvertex1+1)->x < cscene->viewportRightClip)
			{
				coef=float ( 1/ (pworkvertex1->x-(pworkvertex1+1)->x)*(cscene->viewportRightClip-(pworkvertex1+1)->x) );

				pworkvertex0->x=float (cscene->viewportRightClip);
				pworkvertex0->y=clipvalue((pworkvertex1+1)->y, pworkvertex1->y, coef);
				pworkvertex0->z=zclipvalue((pworkvertex1+1)->z, pworkvertex1->z, coef);
				cClipCAPS (surf, pworkvertex0, pworkvertex1, coef);		// z correct clip extra components

				pworkvertex0++;
				pfacelist->nedge++;	
			}
			else if (pworkvertex1->x < cscene->viewportRightClip && (pworkvertex1+1)->x >= cscene->viewportRightClip)
			{
				coef=float ( 1 / (pworkvertex1->x-(pworkvertex1+1)->x) * (cscene->viewportRightClip-(pworkvertex1+1)->x) );

				memcpy(pworkvertex0++, pworkvertex1, sizeof(nX_vertex));
				pworkvertex0->x = float (cscene->viewportRightClip);
				pworkvertex0->y = clipvalue((pworkvertex1+1)->y, pworkvertex1->y, coef);
				pworkvertex0->z = zclipvalue((pworkvertex1+1)->z, pworkvertex1->z, coef);
				cClipCAPS (surf, pworkvertex0, pworkvertex1, coef);		// z correct clip extra components

				pworkvertex0++;
				pfacelist->nedge += 2;	
			}
			pworkvertex1++;
		}

		if (pfacelist->nedge < nX_MINEDGEPOLY)
			return false;

		// THE FACE WENT TROUGHT ALL CLIPPINGS
		//////////////////////////////////////

		pworkvertex0 = workvertex0;

		for (c = 0; c < pfacelist->nedge; c++)
		{
			memcpy(pvertexlist, pworkvertex0++, sizeof nX_vertex);
			pvertexlist->x *= screen->wcoef;
			pvertexlist->y *= screen->hcoef;
			pvertexlist++;
		}

		return true;

}


/*

	ValidFace(float):

	This function add face to the sorting list
	and keep face counters up to date...

	Should be called any time a ClipFace() call
	returned true or prepare for a crash!
	You can call it manually after having added
	a face manually to pfacelist (it MUST be
	clipped or must not need it! no further
	clipping will be performed beyond this call)

*/


void	nX::ValidFace(ulong basez, ulong offsetz)
{

	// adaptative z buffer depth
	if (cscene->iradixprec)
	{
		if ( basez * cscene->iradixprec > largestZ )
			largestZ = basez * cscene->iradixprec;
	}
	else
	{
		if ( basez > largestZ )
			largestZ = (float)basez;
	}

	// opaque faces are sorted front 2 back, transparent one are sorted back 2 front with no zbuffer write!
	if (offsetz > 0)
			pbuffer2sort[0].z = offsetz - basez;
	else	pbuffer2sort[0].z = basez;

	pbuffer2sort[0].faceindex = buildedface++;			// keep track of face index
	pbuffer2sort[0].scene = cscene;
	pbuffer2sort++;
	pfacelist++;

}