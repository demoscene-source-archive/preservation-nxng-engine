	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		nXng Entity Motion control implementation file

	----------------------------------------------------------------------------*/


	#include	<math.h>
	#include	"nX_HeaderList.h"
	static		char		str[256];
	static		float		velocity = 0.0;
	static		bool		b0, b1;

void	nX_Entity::ReadMotionFromUser()
{

	long	x = 0, y = 0;
	bool	intersection = false;
	dXYZ	from, to;

	if (pnX->cIn)
	{
		pnX->cIn->SwitchFocus(pnX->screen->bActive);
		pnX->cIn->ReadFromMouse(&x, &y, &b0, &b1);
		angle[1] += float(x) / 100.0;
		angle[0] -= float(y) / 100.0;

		if (b0)
		{
			velocity += 7.5 * pnX->cscene->elapsedframecount;
			if (velocity > 40.0) velocity = 40.0;
		}
		else if (b1)
		{
			velocity -= 7.5 * pnX->cscene->elapsedframecount;
			if (velocity < -40.0) velocity = -40.0;
		}
		else
		{
			if (velocity != 0.0)
				velocity /= pow(1.25, pnX->cscene->elapsedframecount);
			if (fabs(velocity) < 0.0001)
				velocity = 0.0;
		}

		from.x = offset[0];
		from.y = offset[1];
		from.z = offset[2];
		to.x = from.x + matrix[8] * velocity * scene->elapsedframecount * pnX->screen->RideRatio;
		to.y = from.y - matrix[9] * velocity * scene->elapsedframecount * pnX->screen->RideRatio;
		to.z = from.z + matrix[10] * velocity * scene->elapsedframecount * pnX->screen->RideRatio;
/*
		for (c = 1; c < scene->nbobj; c++)
			if (scene->entityadress[c]->TSP && scene->entityadress[c]->IntersectRayWithTSP(&from, &to))
				intersection = true;

		wsprintf(str, "Intersection: %d", intersection);
		pnX->screen->Print(str, 14, 15);
*/
		offset[0] = to.x;
		offset[1] = to.y;
		offset[2] = to.z;
	}

}