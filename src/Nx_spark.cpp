	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		nXng Spark System implementation file

	----------------------------------------------------------------------------*/


	#include	<math.h>
	#include	"nX_HeaderList.h"

	#define		M_PI			3.1415926597
	#define		HALFPI			(M_PI * 2)
	static		char			str[256];
	static		xLinker			xPK;
	extern		unsigned char	mulshader[65536];


// SetupSparkEmit() -----------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

void	nX_Entity::SetupSparkEmit(unsigned short type)
{

	// create a new emiter
	nX_SparkEmiter	*emit = new(nX_SparkEmiter);
	memset(emit, 0, sizeof(nX_SparkEmiter));
	sparkemiter = emit;

	// setup sparks for emiter
	emit->count = 150;
	emit->sparklist = new nX_Spark[emit->count];
	memset(emit->sparklist, 0, sizeof(nX_Spark) * emit->count);

	nX_Spark	*spark;
	spark = emit->sparklist;

	for (long c = 0; c < emit->count; c++)
	{
		spark->delay = float(rand() / 80.);
		spark++;
	}

}
