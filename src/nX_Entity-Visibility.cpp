	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		nXng Entity Visibility implementation file

		Function set used to determine visibility for a given entity.
		Visibility is at object level!

	----------------------------------------------------------------------------*/


	#include	"nX_HeaderList.h"
	#define		nX_BBZCUTFRONTIER	1.0


static bool	ClipBBRay(float *a, float *b, float *mnx, float *mny, float *mxx, float *mxy, nX_Scene *ts)
{

	bool	in = false;
	float	bx, by;
	float	ix, iy, coef;

	// Ray intersection with zcut
	if ( (a[2] <= nX_BBZCUTFRONTIER && b[2] > nX_BBZCUTFRONTIER) || (b[2] <= nX_BBZCUTFRONTIER && a[2] > nX_BBZCUTFRONTIER) )
	{
		bx  = ( (b[0] - a[0]) / (b[2] - a[2]) ) * (nX_BBZCUTFRONTIER - a[2]) + a[0];
		by  = ( (b[1] - a[1]) / (b[2] - a[2]) ) * (nX_BBZCUTFRONTIER - a[2]) + a[1];
		coef = float (1. / (nX_BBZCUTFRONTIER * ts->Focal->values[0]) );
		ix = bx * coef * ts->viewcx + ts->viewportMidX;
		iy = by * coef * ts->viewcy + ts->viewportMidY;
		if (ix > mxx[0]) mxx[0] = ix; if (ix < mnx[0]) mnx[0] = ix;
		if (iy > mxy[0]) mxy[0] = iy; if (iy < mny[0]) mny[0] = iy;
	}

	return in;

}


/* TestBBVisibility() ---------------------------------------------------------
  0 - BB out of screen, 1 - BB partially on screen, 2 - BB completely on screen
/////////////////////////////////////////////////////////////////////////////*/


void	nX_Entity::TestBBVisibility()
{

	if (usemdd)
	{
		visibility = 1;
		return;
	}

	float		*pbb = pnX->p3dpswap2 + (nbpoints - 8) * 3;
	float		minx, miny, maxx, maxy, coef;
	minx = miny = 9999999999.0;
	maxx = maxy = -9999999999.0;
	float		ix, iy;
	bool		in = false;

	// Zcut BBOX rays
	if (ClipBBRay(&pbb[0 * 3], &pbb[1 * 3], &minx, &miny, &maxx, &maxy, scene))	in = true;
	if (ClipBBRay(&pbb[0 * 3], &pbb[2 * 3], &minx, &miny, &maxx, &maxy, scene))	in = true;
	if (ClipBBRay(&pbb[3 * 3], &pbb[1 * 3], &minx, &miny, &maxx, &maxy, scene))	in = true;
	if (ClipBBRay(&pbb[3 * 3], &pbb[2 * 3], &minx, &miny, &maxx, &maxy, scene))	in = true;
	if (ClipBBRay(&pbb[0 * 3], &pbb[4 * 3], &minx, &miny, &maxx, &maxy, scene))	in = true;
	if (ClipBBRay(&pbb[1 * 3], &pbb[5 * 3], &minx, &miny, &maxx, &maxy, scene))	in = true;
	if (ClipBBRay(&pbb[2 * 3], &pbb[6 * 3], &minx, &miny, &maxx, &maxy, scene))	in = true;
	if (ClipBBRay(&pbb[3 * 3], &pbb[7 * 3], &minx, &miny, &maxx, &maxy, scene))	in = true;
	if (ClipBBRay(&pbb[4 * 3], &pbb[5 * 3], &minx, &miny, &maxx, &maxy, scene))	in = true;
	if (ClipBBRay(&pbb[4 * 3], &pbb[6 * 3], &minx, &miny, &maxx, &maxy, scene))	in = true;
	if (ClipBBRay(&pbb[7 * 3], &pbb[5 * 3], &minx, &miny, &maxx, &maxy, scene))	in = true;
	if (ClipBBRay(&pbb[7 * 3], &pbb[6 * 3], &minx, &miny, &maxx, &maxy, scene))	in = true;

	// Account for inside vertice...
	for (long c = 0; c < 8; c++)
	{
		if (pbb[2] > nX_BBZCUTFRONTIER)
		{
			in = true;
			coef = float (1. / (pbb[2] * scene->Focal->values[0]) );
			ix = pbb[0] * coef * scene->viewcx + scene->viewportMidX;
			iy = pbb[1] * coef * scene->viewcy + scene->viewportMidY;
			if (ix > maxx) maxx = ix; if (ix < minx) minx = ix;
			if (iy > maxy) maxy = iy; if (iy < miny) miny = iy;
		}
		pbb += 3;
	}

	// Let's test the results
	if (!in)
		visibility = 0;			// unseen!
	else if (
		minx > scene->viewportRightClip ||	maxx < scene->viewportLeftClip ||
		miny > scene->viewportDownClip || maxy < scene->viewportUpClip)
		visibility = 0;			// unseen!
	else if (
		minx > scene->viewportLeftClip && maxx < scene->viewportRightClip &&
		miny > scene->viewportUpClip && maxy < scene->viewportDownClip)
		visibility = 2;			// fully on screen!
	else
		visibility = 1;			// occluded!

	#ifdef	nX_DEEPBENCH
	{
		if (pnX->screen->bUseFrameReport && visibility && pnX->screen->bBoundingBox)
		{
			pnX->Square(minx, miny, maxx, maxy, 1.1, scene->radixprec, pnX->BlankSurface, 0.75, NULL);
			BBmnx = minx; BBmny = miny;
		}
	}
	#endif

}