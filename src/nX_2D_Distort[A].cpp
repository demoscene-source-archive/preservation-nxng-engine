	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		nXng 2D Distortion implementation file

		'There is no challenge in doing that ):'
		'I feel so lazy to rewrite things i've done 6 years ago ):'

		730 faces ): ....

	----------------------------------------------------------------------------*/


	#include	<math.h>
	#include	"nXng/nX_HeaderList.h"


#ifdef _2D_EFFECT_


//	nX::_2D_BlendMatrice()
//
//	Blend two matrix [a&b] together putting the resulting matrix in A.


void	nX::_2D_BlendMatrix(nX_vertex *a, float ca, nX_vertex *b, float cb)
{

	for (unsigned long c = 0; c < (screen->referenceheight / 20 + 1) * (screen->referencewidth / 20 + 1); c++)
	{
		a->u[0] = a->u[0] * ca + b->u[0] * cb;
		a->v[0] = a->v[0] * ca + b->v[0] * cb;
		a++;
		b++;
	}

}


//	nX::_2D_Distort_TA()
//
//	Compute a 2D effect matrix giving result like Tortozoom


void	nX::_2D_Distort_TA(nX_vertex *pv)
{

	long			a = _2D_dTick / 2;
	unsigned long	cx, cy;

	float		phx, phy, pvx, pvy, dhx, dhy, dvx, dvy;

	dhx =   qCos[modsincos(a)] / 20.0f; dhy = qSin[modsincos(a)] / 20.0f;
	dvx =   qCos[modsincos(a + sincosREV / 4)] / 20.0f; dvy = qSin[modsincos(a + sincosREV / 4)] / 20.0f;
	pvx =   qCos[modsincos(a)] * 8.0f;
	pvy =   qSin[modsincos(a)] * 4.0f;

	for (cy = 0; cy <= (screen->referenceheight / 20); cy++)
	{
		phx = pvx;
		phy = pvy;
		for (cx = 0; cx <= (screen->referencewidth / 20); cx++)
		{
			pv->u[0] = phx;
			pv->v[0] = phy;
			pv->z = 1000.0f;
			pv++;

			phx += dhx + qCos[modsincos( (cx * 20 * cy + a) )] / 40.0f;
			phy += dhy + qSin[modsincos( (cx * 700 + a * 2) )] / 40.0f;
		}
		pvx += dvx + qCos[modsincos( (cy * 500) )] / 10.0f;;
		pvy += dvy + qSin[modsincos( (cy * 700 + a) )] / 10.0f;
	}
}


//	nX::_2D_Distort_TB()
//
//	Compute a 2D effect matrix giving result like
//	Ring Water effect in The Prey by Polka Brothers


void	nX::_2D_Distort_TB(nX_vertex *pv)
{

	float			a = _2D_dTick / 100.0f;
	float			amp = qCos[radsincos(a / 4.0f)] * 0.03f + 0.05f;
	float			fqA = qCos[radsincos(a / 6.0f)] * 0.5f + 3.5f;

	// compute buffers
	unsigned long	cx, cy;
	float			dx, dy, d, ag, r;

	for (cy = 0; cy <= (screen->referenceheight / 20); cy++)
	{
		dy = float(cy) - float(screen->referenceheight / 40);

		for (cx = 0; cx <= (screen->referencewidth / 20); cx++)
		{
			dx = float(cx) - float(screen->referencewidth / 40);

			d = float(sqrt(dx * dx + dy * dy));
			if (d)	ag = float(acos(dx / d));
			else	ag = 0;
			if (dy < 0)	ag = 2.0f * 3.1415927f - ag;

			r = d * qCos[radsincos( (d * 2.0f / fqA) - a * 2000.0f )] * amp;

			pv->u[0] = qCos[radsincos(ag)] * r;
			pv->v[0] = qSin[radsincos(ag)] * r;
			pv->z = 1000.0f;
			pv++;
		}
	}
}

#endif