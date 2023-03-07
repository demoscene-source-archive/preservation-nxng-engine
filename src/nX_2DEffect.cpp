	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		nXng 2D Effects implementation file

		'There is no challenge in doing that ):'
		'I feel so lazy to rewrite things i've done 6 years ago ):'

	----------------------------------------------------------------------------*/


	#include	<math.h>
	#include	"nXng/nX_HeaderList.h"


/*
	nX::Tilemap()
	-------------
	Fills screen with tiles of dimensions 8x8.
	Mapping them with coordinates set in v (x, y members of v are ignored!).

	Note: 8x8 is NOT relative to the referencewidth and referenceheight
	----  screen members.
*/


#ifdef _2D_EFFECT_

#define	vBORDER		80

void	nX::TileMap(nX_vertex *v, nX_Surface *s)
{

	// pool timers
	unsigned long	tmp = GetTickCount();
	_2D_dTick = tmp - _2D_Tick;

	long			dx = screen->referencewidth / 20 + 1;
	unsigned char	bgt;
	v += dx * (vBORDER / 20);

	for (unsigned long ty = vBORDER; ty < (screen->referenceheight - vBORDER); ty += 20)
	{
		for (unsigned long tx = 0; tx < screen->referencewidth; tx += 20)
		{
			pfacelist->nedge = 4;
			pfacelist->surf = s;
			bgt = unsigned char (sin(float(tx) / 200 + float(_2D_dTick) / 150) * 127 + (255 - 127) );
			pfacelist->diff.r = bgt;
			pfacelist->diff.g = bgt;
			pfacelist->diff.b = bgt;
			pfacelist->alpha = 0;
			pfacelist->vertice = pvertexlist;

			(pvertexlist+0)->x = (tx + 1) * screen->wcoef;
			(pvertexlist+0)->y = (ty + 1) * screen->hcoef;
			(pvertexlist+0)->z = v[0].z;
			(pvertexlist+0)->u[0] = v[0].u[0];
			(pvertexlist+0)->v[0] = v[0].v[0];

			(pvertexlist+1)->x = (tx + 19) * screen->wcoef;
			(pvertexlist+1)->y = (ty + 1) * screen->hcoef;
			(pvertexlist+1)->z = v[1].z;
			(pvertexlist+1)->u[0] = v[1].u[0];
			(pvertexlist+1)->v[0] = v[1].v[0];

			(pvertexlist+2)->x = (tx + 19) * screen->wcoef;
			(pvertexlist+2)->y = (ty + 19) * screen->hcoef;
			(pvertexlist+2)->z = v[dx + 1].z;
			(pvertexlist+2)->u[0] = v[dx + 1].u[0];
			(pvertexlist+2)->v[0] = v[dx + 1].v[0];

			(pvertexlist+3)->x = (tx + 1) * screen->wcoef;
			(pvertexlist+3)->y = (ty + 19) * screen->hcoef;
			(pvertexlist+3)->z = v[dx].z;
			(pvertexlist+3)->u[0] = v[dx].u[0];
			(pvertexlist+3)->v[0] = v[dx].v[0];

			ValidFace(unsigned long(v[0].z), nX_OPAQUERADIXCONST);
			pvertexlist += 4;
			v++;
		}
		v++;
	}
}

void	nX::_2D_Init()
{
	_2D_Free();

	_2D_BufferA = new nX_vertex[(screen->referencewidth / 20 + 1) * (screen->referenceheight / 20 + 1)];
	_2D_BufferB = new nX_vertex[(screen->referencewidth / 20 + 1) * (screen->referenceheight / 20 + 1)];
}

void	nX::_2D_Free()
{
	if (_2D_BufferA)
		delete (_2D_BufferA);
	_2D_BufferA = NULL;
	if (_2D_BufferB)
		delete (_2D_BufferB);
	_2D_BufferB = NULL;
}
#endif