	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		nXng [WAVE] Entity effector implementation file

	----------------------------------------------------------------------------*/


	#include		<math.h>
	#include		"nX_HeaderList.h"

#ifdef nX_EFFECTOR

void	nX_Entity::GetWaveGradients()
{

	if (!pLong)	pLong = new float[nbpoints];
	if (!pLat)	pLat = new float[nbpoints];

	oMesh = edge_adress;
	edge_adress = new float[nbpoints * 8 * 3];

	float	*pEdge = oMesh, x, y, z, l;
	float	*wLong = pLong;
	float	*wLat = pLat;

	for (long c = 0; c < nbpoints; c++)
	{
		x = pEdge[0];
		y = pEdge[1];
		z = pEdge[2];
		pEdge += 3;

		// get parametric coordinates
		l = float(sqrt( x * x + y * y + z * z ));
		*wLong++ = float(acos( y / l ));
		*wLat++ = float(acos( z / l ));
	}

}

void	nX_Entity::ApplyWave()
{

	float	*wLong = pLong;
	float	*wLat = pLat;
	float	*pm = oMesh;
	float	*pEdge = edge_adress;
	float	cf;
	waveA += scene->elapsedframecount * 0.025f;				// 0.025
	float	fqA = 3, fqB = 3;								// 3, 3

	for (long c = 0; c < nbpoints; c++)
	{
		cf = (float)(
			   cos( ( (*wLong++) + waveA ) * fqA )			// spike A
			 * cos( ( (*wLat++) + waveA * 2 ) * fqB )		// spike B
			 * (cos(waveA) * 0.25 + 0.5)					// amplitude
			 + 1.0f
					);

		pEdge[0] = pm[0] * cf;
		pEdge[1] = pm[1] * cf;
		pEdge[2] = pm[2] * cf;
		pEdge += 3;
		pm += 3;
	}

	if (requirement & nX_USEFACENORMAL)
		GetFaceNormals();
	if (requirement & nX_USEEDGENORMAL)
		GetEdgeNormals();

}

#endif