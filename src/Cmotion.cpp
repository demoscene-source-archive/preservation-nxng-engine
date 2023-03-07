	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		nXng Lightwave Motion implementation file
		Author: Allan Fergusson

	----------------------------------------------------------------------------*/
	

	#include	"nXng/cMotion.h"


// Compute Hermite spline coefs

inline void CompHermite(float t, float *h1, float *h2, float *h3, float *h4)
{
	float t2, t3, z;
	t2 = t * t;
	t3 = t * t2;
	z = (float)(3.0 * t2 - t3 - t3);

	*h1 = 1 - z;
	*h2 = z;
	*h3 = t3 - t2 - t2 + t;
	*h4 = t3 - t2;
}

// Fill motion struct

void MotionEngine::MotionCalcStep(Motion *mot,ChanVec resVec, float step, unsigned char nbchan)
{
	KF			*key0, *key1;
	float		t, h1, h2, h3, h4, res, d10;
	float		dd0a, dd0b, ds1a, ds1b;
	float		adj0, adj1, dd0, ds1;
	int			i, tlength;

	// if there is but one key, the values are constant

	if (mot->keys == 1) 
	{
		for (i = 0; i < nbchan; i++)
			resVec[i] = mot->keylist[0].cv[i];
		return;
	}

	// get keyframe pair to evaluate. this should be within the range
	// of the motion or this will raise an illegal access

	key0 = mot->keylist;
	while (step > key0[1].step)
		key0++;
	key1=key0+1;

	// test if we have a position 'switch'

	if (key0->step == key1->step - 1)
	{
		for (i = 0; i < nbchan; i++)
			resVec[i] = key1->cv[i];
		return;
	}

	step-=key0->step;

	// get tween length and fractional tween position

	tlength = key1->step - key0->step;
	t = step / tlength;

	// precompute spline coefficients

	if (!key1->linear) {
		CompHermite(t, &h1, &h2, &h3, &h4);
		dd0a = (1 - key0->tens) * (1 + key0->cont)
			 * (1 + key0->bias);
		dd0b = (1 - key0->tens) * (1 - key0->cont)
			 * (1 - key0->bias);
		ds1a = (1 - key1->tens) * (1 - key1->cont)
			 * (1 + key1->bias);
		ds1b = (1 - key1->tens) * (1 + key1->cont)
			 * (1 - key1->bias);

		if (key0->step != 0)
		    adj0 = tlength / (float)(key1->step - key0[-1].step);
		if (key1->step != mot->steps)
		    adj1 = tlength / (float)(key1[1].step - key0->step);
	}

	// compute the channel components

	for (i = 0; i < nbchan; i++) {
		d10 = key1->cv[i] - key0->cv[i];

		if (!key1->linear) {
			if (key0->step == 0)
				dd0 = (float)(0.5 * (dd0a + dd0b) * d10);
			else
				dd0 = adj0 * (dd0a
					* (key0->cv[i] - key0[-1].cv[i])
					+ dd0b * d10);

			if (key1->step == mot->steps)
				ds1 = (float)(0.5 * (ds1a + ds1b) * d10);
			else
				ds1 = adj1 * (ds1a * d10 + ds1b
					* (key1[1].cv[i] - key1->cv[i]));

			res = key0->cv[i] * h1 + key1->cv[i] * h2
				+ dd0 * h3 + ds1 * h4;
		} else
			res = key0->cv[i] + t * d10;

		resVec[i] = res;
	}
}

// Fill GIZMO motion

void MotionEngine::GizmoCalcStep(GizmoMotion *mot, ChanVec resVec, float step, unsigned char nbchan)
{
	GizmoKF		*key0, *key1;
	float		t, h1, h2, h3, h4, res, d10;
	float		dd0a, dd0b, ds1a, ds1b;
	float		adj0, adj1, dd0, ds1;
	int			i, tlength;

	/* If there is but one key, the values are constant. */
	if (mot->keys == 1) 
	{
		for (i = 0; i < nbchan; i++)
			resVec[i] = mot->keylist[0].cv[i];
		return;
	}

	/* Get keyframe pair to evaluate.  This should be within the range
		of the motion or this will raise an illegal access. */

	key0=mot->keylist;
	while (step>key0[1].step)
		key0++;
	key1=key0+1;

	/* Test if we have a position 'switch'. */
	if (key0->step == key1->step - 1)
	{
		for (i = 0; i < nbchan; i++)
			resVec[i] = key1->cv[i];
		return;
	}

	step-=key0->step;

	/* Get tween length and fractional tween position. */
	tlength = key1->step - key0->step;
	t = step / tlength;

	/*
	 * Precompute spline coefficients.
	 */
	if (!key1->linear) {
		CompHermite (t, &h1, &h2, &h3, &h4);
		dd0a = (1 - key0->tens) * (1 + key0->cont)
			 * (1 + key0->bias);
		dd0b = (1 - key0->tens) * (1 - key0->cont)
			 * (1 - key0->bias);
		ds1a = (1 - key1->tens) * (1 - key1->cont)
			 * (1 + key1->bias);
		ds1b = (1 - key1->tens) * (1 + key1->cont)
			 * (1 - key1->bias);

		if (key0->step != 0)
		    adj0 = tlength / (float)(key1->step - key0[-1].step);
		if (key1->step != mot->steps)
		    adj1 = tlength / (float)(key1[1].step - key0->step);
	}

	/*
	 * Compute the channel components.
	 */
	for (i = 0; i < nbchan; i++) {
		d10 = key1->cv[i] - key0->cv[i];

		if (!key1->linear) {
			if (key0->step == 0)
				dd0 = (float)(.5 * (dd0a + dd0b) * d10);
			else
				dd0 = adj0 * (dd0a
					* (key0->cv[i] - key0[-1].cv[i])
					+ dd0b * d10);

			if (key1->step == mot->steps)
				ds1 = (float)(.5 * (ds1a + ds1b) * d10);
			else
				ds1 = adj1 * (ds1a * d10 + ds1b
					* (key1[1].cv[i] - key1->cv[i]));

			res = key0->cv[i] * h1 + key1->cv[i] * h2
				+ dd0 * h3 + ds1 * h4;
		} else
			res = key0->cv[i] + t * d10;

		resVec[i] = res;
	}
}