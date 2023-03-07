	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		cScreen/DirectDraw implementation file

	----------------------------------------------------------------------------*/


		#include	<ddraw.h>
		#include	<d3d.h>

		#include	"nXng/nX_console.h"
		#include	"nXng/cScreenMN.h"


		////////////////////////////////
		// INITIALISATION/FREEING SOURCE
		////////////////////////////////


/*
	ScreenMN::DD_Init()
	-------------------
	Initialize DDRAW for current selected board
*/


CSC_MSG ScreenMN::DD_Init()
{

	if (pDD)
		DD_Free();

	// get a DX7 object

	if (cboard == 0)
	{
		if (DirectDrawCreateEx(NULL, (void **)&pDD, IID_IDirectDraw7, NULL) != DD_OK)
			return CSC_ERRCREATEDD;
	}
	else
	{
		if (DirectDrawCreateEx(&cddboard->GUID, (void **)&pDD, IID_IDirectDraw7, NULL) != DD_OK)
			return CSC_ERRCREATEDD;
	}

	return	CSC_NOERROR;

}


/*
	ScreenMN::DD_Free()
	-------------------
	Free currently initialized DDRAW
*/


void ScreenMN::DD_Free()
{
	if (pDD)
		pDD->Release();
	pDD = NULL;
	bReady = false;
}