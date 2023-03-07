	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		nXng rasterizer header file

	----------------------------------------------------------------------------*/


	// D3D Vertex structure for MonoTexture/MultiTexture renderers


struct	D3Dvtx
{
	float	x, y, z;
	float	rhw;
	long	diff;

	// tex coords
	float	u0, v0;
	float	u1, v1;
	float	u2, v2;
	float	u3, v3;
};