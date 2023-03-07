	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		nXng rasterizer implementation file

	----------------------------------------------------------------------------*/


	#include	"nXng/nX_HeaderList.h"


// Function set to begin and end a frame
////////////////////////////////////////

void	nX::Rasterize()
{

	if (screen->bBloom)
		ProcessBloom();

	if ( (screen->cd3ddevice->mxnbmap > 1) && !screen->bForceSingleRaster)
			RasterizeMultiTexture();
	else
			RasterizeMonoTexture();

}

void	nX::BeginFrame(bool clear)
{

	// Update all TexMap
	UpdateTexMap();

	largestZ = 0;
	screen->pD3DDevice->BeginScene();
	if (clear)
		screen->pD3DDevice->Clear(NULL, NULL, D3DCLEAR_TARGET, screen->FillColor, 1., 0);

}

void nX::EndFrame()
{

	screen->pD3DDevice->EndScene();

	HRESULT	hret;
	if (screen->bWindowed)
			hret = screen->pDDSPrimary->Blt(&screen->rcScreen, screen->pDDSBack, &screen->rcViewport, DDBLT_ASYNC, NULL);
	else	hret = screen->pDDSPrimary->Flip(NULL, DDFLIP_WAIT);
	if (hret == DDERR_SURFACELOST)
		screen->pDD->RestoreAllSurfaces();

}