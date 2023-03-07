	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		nXng Envelop implementation file

	----------------------------------------------------------------------------*/


	#include	<math.h>
	#include	<string.h>
	#include	"IO/cqString.h"
	#include	"nXng/cMotion.h"

	static		MotionEngine	eMotion;


/*
	inline float GetBehaviorFrame(float, long, float, float)
	--------------------------------------------------------
	Input: Current unclipped frame,
	Output: Motion's 'End Behavior' compliant frame.
*/


inline float GetBehaviorFrame(float frame, long endb, float lf, float offset)
{
	float	envframe;
	float	frm = frame - offset;

	// retrieve timed frame

	switch (endb)
	{
		// RESET is not handled
		case 0:

		// STOP
		case 1:
			if (frm > lf)
				envframe = lf; else envframe = frm;
			if (envframe < 0)
				envframe = 0;
			break;

		// REPEAT
		case 2:
			envframe = frm / lf;
			envframe -= float(floor(envframe));
			envframe *= lf;
			break;
	}

	return envframe;
}


/*
	nX_Envelop::LoadLWSMotion(char *)
	---------------------------------
	Load an envelop/motion from plain ASCII LWS file.
	This routine supports motion/envelop scaling trought mulfactor.
*/


char *nX_Envelop::LoadLWSMotion(char *pLWS)
{
	long	c, c2;
	qString	qS;

	// hey! we use an envelop here!
	useenvelop = true;

	// retrieve the number of channel to fill structure with
	while (pLWS[0] != 0xd && pLWS[1] != 0xa)
		pLWS++;
	pLWS += 4;

	nbchan = (unsigned char)qS.a2f(pLWS);

	// retrieve number of keyframe to be listed
	while (pLWS[0] != 0xd && pLWS[1] != 0xa)
		pLWS++;
	pLWS += 4;
	nbKF = (unsigned short)qS.a2f(pLWS);

	// allocate keyframes...
	KF	*pKF = KFadrss = new KF[nbKF];

	while (pLWS[0] != 0xd && pLWS[1] != 0xa)
		pLWS++;

	pLWS += 2;

	// process struct filling for each keyframe
	for (c = 0; c < nbKF; c++)
	{
		pLWS += 2;

		for (c2 = 0; c2 < nbchan; c2++)
		{
			// in a usual motion this is: x, y, z, h, p, b, xs, ys, zs.
			pKF->cv[c2] = qS.a2f(pLWS);
			while (*pLWS++ != ' ');
		}

		while (pLWS[0] == ' ')
			pLWS++;

		// frame number
		pKF->step = (int)(qS.a2f(pLWS));
		lf = float(pKF->step);		// higher frame for object.
		while (*pLWS++ != ' ');

		// linear
		pKF->linear = (int)(qS.a2f(pLWS));
		while (*pLWS++ != ' ');

		// tension
		pKF->tens = qS.a2f(pLWS);
		while (*pLWS++ != ' ');

		// continuité
		pKF->cont = qS.a2f(pLWS);
		while (*pLWS++ != ' ');

		// bias
		pKF->bias = qS.a2f(pLWS);
		while (pLWS[0] != 0xd && pLWS[1] != 0xa)
			pLWS++;

		pLWS += 2;

		// keyframe filled!
		pKF++;
	}

	if (!qS.qstrncmp(pLWS, "FrameOffset", 11))
	{
		pLWS += 12;
		offset = qS.a2f(pLWS);
		while (pLWS[0] != 0xd && pLWS[1] != 0xa)
			pLWS++;
		pLWS += 2;
	}
	// motion loop state.
	endb = (unsigned char)qS.a2f(pLWS + 12);

	return pLWS;
}


/*
	nX_Envelop::GetValue(float)
	---------------------------
	Returns value in envelop at a given frame
*/


void nX_Envelop::GetValues(float frame)
{
	if (!useenvelop)
		return;
	float envframe = GetBehaviorFrame(frame, endb, lf, offset);
	eMotion.MT.keylist = KFadrss;
	eMotion.MT.keys = nbKF;
	eMotion.MT.steps = int(lf);
	eMotion.MotionCalcStep(&eMotion.MT, values, envframe, nbchan);
}


/*
	nX_Envelop::nX_Envelop()
*/


nX_Envelop::nX_Envelop()
{
	memset(this, 0, sizeof nX_Envelop);
}


/*
	nX_Envelop::~nX_Envelop()
*/


nX_Envelop::~nX_Envelop()
{
	delete KFadrss;
}
