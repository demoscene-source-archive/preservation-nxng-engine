	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		nXng Lightwave Plugin MorphGizmo Loader declaration file

	----------------------------------------------------------------------------*/


	#define		nX_GIZMOINSIDE


struct nX_GizTargetLL
{

	nX_Entity			*entity;			// direct entity for target
	nX_GizTargetLL		*target;			// next target in list...
	nX_Envelop			*motion;			// target motion
	unsigned short		itarget;			// unresolved target

};


struct nX_GIZMO
{

	unsigned short		nbtarget;			// # target(s) in Gizmo
	nX_GizTargetLL		*targetroot;		// target list root
	
};