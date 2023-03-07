	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		cScreen/Direct3D implementation file

	----------------------------------------------------------------------------*/


		#include	<ddraw.h>
		#include	<d3d.h>

		#include	"nXng/nX_console.h"
		#include	"nXng/cScreenMN.h"
		static		char	str[256], tmp[128];


/*
	D3DEnumzBufferFormats()
	-----------------------
	Select Zbuffer format based on the ZB_rate table
*/


static HRESULT CALLBACK D3DEnumzBufferFormats(DDPIXELFORMAT *lpDDPF, VOID *lpContext)
{

	ScreenMN	*calling = (ScreenMN *)lpContext;

	if (lpDDPF->dwFlags == DDPF_ZBUFFER)
	{
		if ( lpDDPF->dwZBufferBitDepth > 32 && calling->currentZBrating < calling->ZB_rating[ZB_X] )
		{
			memcpy(&calling->D3D_Zbuffer, lpDDPF, sizeof(DDPIXELFORMAT));
			calling->currentZBrating = calling->ZB_rating[ZB_X];
		}
		if ( lpDDPF->dwZBufferBitDepth == 32 && calling->currentZBrating < calling->ZB_rating[ZB_32] )
		{
			memcpy(&calling->D3D_Zbuffer, lpDDPF, sizeof(DDPIXELFORMAT));
			calling->currentZBrating = calling->ZB_rating[ZB_32];
		}
		if ( lpDDPF->dwZBufferBitDepth == 24 && calling->currentZBrating < calling->ZB_rating[ZB_24] )
		{
			memcpy(&calling->D3D_Zbuffer, lpDDPF, sizeof(DDPIXELFORMAT));
			calling->currentZBrating = calling->ZB_rating[ZB_24];
		}
		if ( lpDDPF->dwZBufferBitDepth == 16 && calling->currentZBrating < calling->ZB_rating[ZB_16] )
		{
			memcpy(&calling->D3D_Zbuffer, lpDDPF, sizeof(DDPIXELFORMAT));
			calling->currentZBrating = calling->ZB_rating[ZB_16];
		}
	}

	return DDENUMRET_OK;

}


/*
	D3DEnumzBufferFormats()
	-----------------------
	Select Texture pixel format based on the TF_rate table
*/


static HRESULT CALLBACK D3DEnumPixelFormatsCallback(LPDDPIXELFORMAT lpDDPixFmt, LPVOID lpContext)
{

	ScreenMN	*calling = (ScreenMN *)lpContext;

	// RGB textures with alpha
	if ( (lpDDPixFmt->dwFlags & DDPF_RGB) && (lpDDPixFmt->dwFlags & DDPF_ALPHAPIXELS) )
	{
		// RGB5555 (rated 3)
		if ( lpDDPixFmt->dwRGBBitCount == 32 && calling->currentARGBrating < calling->ATF_rating[ATF_RGB8888] )
		{
			memcpy(&calling->D3D_ARGBtexfmt, lpDDPixFmt, sizeof(DDPIXELFORMAT));
			calling->currentARGBrating = calling->ATF_rating[ATF_RGB8888];
		}
		if ( lpDDPixFmt->dwRGBBitCount == 16 )
		{
			// RGB1555 (rated 1)
			if ( lpDDPixFmt->dwRBitMask== 0x1f<<10 &&
				 lpDDPixFmt->dwGBitMask== 0x1f<<5 &&
				 lpDDPixFmt->dwBBitMask== 0x1f && calling->currentARGBrating < calling->ATF_rating[ATF_RGB1555] )
			{
				memcpy(&calling->D3D_ARGBtexfmt, lpDDPixFmt, sizeof(DDPIXELFORMAT));
				calling->currentARGBrating = calling->ATF_rating[ATF_RGB1555];
			}
			// BGR4444 (rated 2)
			if ( lpDDPixFmt->dwRBitMask== 0x0f << 8 &&
				 lpDDPixFmt->dwGBitMask== 0x0f << 4 &&
				 lpDDPixFmt->dwBBitMask== 0x0f && calling->currentARGBrating < calling->ATF_rating[ATF_RGB4444] )
			{
				memcpy(&calling->D3D_ARGBtexfmt, lpDDPixFmt, sizeof(DDPIXELFORMAT));
				calling->currentARGBrating = calling->ATF_rating[ATF_RGB4444];
			}
		}
	}

	// RGB textures
	if ( (lpDDPixFmt->dwFlags & DDPF_RGB) && !(lpDDPixFmt->dwFlags & DDPF_ALPHAPIXELS) )
	{
		// RGB32 (rated 5)
		if ( lpDDPixFmt->dwRGBBitCount == 32 && calling->currentRGBrating < calling->TF_rating[TF_RGB32] )
		{
			memcpy(&calling->D3D_RGBtexfmt, lpDDPixFmt, sizeof(DDPIXELFORMAT));
			calling->currentRGBrating = calling->TF_rating[TF_RGB32];
		}
		if ( lpDDPixFmt->dwRGBBitCount == 16 )
		{
			// RGB15 (rated 1)
			if ( lpDDPixFmt->dwRBitMask== 0x1f<<10 &&
				 lpDDPixFmt->dwGBitMask== 0x1f<<5 &&
				 lpDDPixFmt->dwBBitMask== 0x1f && calling->currentRGBrating < calling->TF_rating[TF_RGB15] )
			{
				memcpy(&calling->D3D_RGBtexfmt, lpDDPixFmt, sizeof(DDPIXELFORMAT));
				calling->currentRGBrating = calling->TF_rating[TF_RGB15];
			}
			// BGR16 (rated 2)
			if ( lpDDPixFmt->dwBBitMask== 0x1f<<11 &&
				 lpDDPixFmt->dwGBitMask== 0x3f<<5 &&
				 lpDDPixFmt->dwRBitMask== 0x1f && calling->currentRGBrating < calling->TF_rating[TF_BGR16] )
			{
				memcpy(&calling->D3D_RGBtexfmt, lpDDPixFmt, sizeof(DDPIXELFORMAT));
				calling->currentRGBrating = calling->TF_rating[TF_BGR16];
			}
			// RGB16 (rated 3)
			if ( lpDDPixFmt->dwRBitMask== 0x1f<<11 &&
				 lpDDPixFmt->dwGBitMask== 0x3f<<5 &&
				 lpDDPixFmt->dwBBitMask== 0x1f && calling->currentRGBrating < calling->TF_rating[TF_RGB16] )
			{
				memcpy(&calling->D3D_RGBtexfmt, lpDDPixFmt, sizeof(DDPIXELFORMAT));
				calling->currentRGBrating = calling->TF_rating[TF_RGB16];
			}
		}
	}

	return D3DENUMRET_OK;

}


/*
	ScreenMN::InitD3DObjects()
	--------------------------
	Initialisation of ALL direct3d objects
*/

#pragma optimize( "", off )

CSC_MSG	ScreenMN::InitD3DObjects()
{

	Verbose("Screen::InitD3D();");

	DDSURFACEDESC2	ddsd;
    DDSCAPS2		ddscaps;
    LPDIRECTDRAWCLIPPER pClipper;

	pD3DDevice	= NULL;
	pDDSPrimary	= NULL;
	pDDSBack	= NULL;
	pDDZbuffer	= NULL;
	pD3D		= NULL;

	if (bWindowed)
	{

		// get normal windowed mode
		if (pDD->SetCooperativeLevel(hWnd, DDSCL_NORMAL | DDSCL_FPUSETUP) != DD_OK)
			return CSC_ERRCOOPLVL;

		// get dimensions of viewport and screen bounds
		GetClientRect(hWnd, &rcViewport);
	 	GetClientRect(hWnd, &rcScreen);
		ClientToScreen(hWnd, (POINT*)&rcScreen.left);
		ClientToScreen(hWnd, (POINT*)&rcScreen.right);

		// create the primary surface
        memset(&ddsd, 0, sizeof(ddsd));
		ddsd.dwSize				=	sizeof(ddsd);
		ddsd.dwFlags			=	DDSD_CAPS;
		ddsd.ddsCaps.dwCaps		=	DDSCAPS_PRIMARYSURFACE;

		if (pDD->CreateSurface(&ddsd, &pDDSPrimary, NULL) != DD_OK)
			return CSC_ERRCREATESURF;

        // create a clipper object since this is for a windowed render
		if (pDD->CreateClipper(0, &pClipper, NULL) != DD_OK)
			return CSC_ERRCREATECLIP;

		// associate the clipper with the window
        pClipper->SetHWnd(0, hWnd);
        pDDSPrimary->SetClipper(pClipper);
        pClipper->Release();
        pClipper = NULL;
	
		// create the 3D surface...
		ddsd.dwSize			= sizeof(ddsd);
		ddsd.dwFlags		= DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
		ddsd.dwWidth		= width;
		ddsd.dwHeight		= height;
		ddsd.ddsCaps.dwCaps = DDSCAPS_3DDEVICE;
		if (cd3ddevice->software)
			ddsd.ddsCaps.dwCaps |= DDSCAPS_OFFSCREENPLAIN;

		if (pDD->CreateSurface(&ddsd, &pDDSBack, NULL)!=DD_OK)
			return CSC_ERRCREATESURF;

	}
	else
	{

		// switch to fullscreen
		if (pDD->SetCooperativeLevel(hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_FPUSETUP) != DD_OK)
			return CSC_ERRCOOPLVL;

		// set video mode
		wsprintf(str, "    Setting target resolution %dx%dx%d.", width, height, vmbpp);
		Verbose(str);
		if (pDD->SetDisplayMode(width, height, vmbpp, 0, 0) != DD_OK)
			return CSC_ERRSETDISPLAYMODE;

		// primary surface
		memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
		ddsd.dwSize				= sizeof(DDSURFACEDESC2);
		ddsd.dwFlags			= DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		ddsd.ddsCaps.dwCaps		= DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE |
								  DDSCAPS_COMPLEX | DDSCAPS_FLIP;
		ddsd.dwBackBufferCount	= 1;

		if (pDD->CreateSurface(&ddsd, &pDDSPrimary, NULL) != DD_OK)
			return CSC_ERRCREATESURF;
   
		// attached surface
		memset(&ddscaps, 0, sizeof(DDSURFACEDESC2));
		ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
		if (pDDSPrimary->GetAttachedSurface (&ddscaps, &pDDSBack) != DD_OK)
			return CSC_ERRGETATTSURF;

	}

	// get D3D interface if required
	if (pDD->QueryInterface (IID_IDirect3D7, (void **)&pD3D) != S_OK)
		return CSC_ERRQUERYD3D;

	// find the best zbuffer format available
	currentZBrating = ZB_NULL;
	pD3D->EnumZBufferFormats(cd3ddevice->dGUID, D3DEnumzBufferFormats, this);
	if (currentZBrating == ZB_NULL)
		return CSC_NOZBUFFER;

	GetZBDesc(currentZBrating, tmp);
	wsprintf(str, "    ZBuffer format: %s,", tmp);
	Verbose(str);

	// create the zbuffer
	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize				= sizeof(ddsd);
	ddsd.dwFlags			= DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_PIXELFORMAT;
	ddsd.ddsCaps.dwCaps		= DDSCAPS_ZBUFFER;
	if (cd3ddevice->software)
			ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
	else	ddsd.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
	ddsd.dwWidth			= width;
	ddsd.dwHeight			= height;

	memcpy(&ddsd.ddpfPixelFormat, &D3D_Zbuffer, sizeof(DDPIXELFORMAT));

	if (pDD->CreateSurface(&ddsd, &pDDZbuffer, NULL) != DD_OK)
		return CSC_ERRCREATESURF;
	if (pDDSBack->AddAttachedSurface(pDDZbuffer) != DD_OK)
		return CSC_ERRGETATTSURF;

	// create primary 3d-accel device
	if (FAILED(pD3D->CreateDevice(cd3ddevice->dGUID, pDDSBack, &pD3DDevice)))
		return CSC_ERRCREATEDEVICE;

	// fill viewport structure
	memset(&pD3DView, 0, sizeof(D3DVIEWPORT7));
	pD3DView.dwX		= 0;
	pD3DView.dwY		= 0;
	pD3DView.dwWidth	= width;
	pD3DView.dwHeight	= height;
	pD3DView.dvMinZ		= 0.;
	pD3DView.dvMaxZ		= 1.;

	if (pD3DDevice->SetViewport(&pD3DView) != DD_OK)
		return CSC_ERRSETVIEWPORT;

	// set basic device states
	pD3DDevice->SetRenderState(D3DRENDERSTATE_SUBPIXEL, true);

//	if (dither)
//		pD3DDevice->SetRenderState(D3DRENDERSTATE_DITHERENABLE, true);
//	else
//		pD3DDevice->SetRenderState(D3DRENDERSTATE_DITHERENABLE, false);

	pD3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, true);

	// bilinear
	pD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);
	pD3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFG_LINEAR);
	pD3DDevice->SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTFG_LINEAR);
	pD3DDevice->SetTextureStageState(1, D3DTSS_MINFILTER, D3DTFG_LINEAR);

	// you're so cute you little d3d, but i think i know how to clip MY polygons... yeah... POLYgones
	pD3DDevice->SetRenderState(D3DRENDERSTATE_CLIPPING, false);
	pD3DDevice->SetRenderState(D3DRENDERSTATE_LIGHTING, false);	// hum... HUM... FUCK OFF!
	pD3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);

	// use w-buffer if set and the board has one
	pD3DDevice->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_GREATEREQUAL);
	pD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, true);

	// check supported texture formats and get the best we find (:
	currentRGBrating = TF_NULL;
	currentARGBrating = ATF_NULL;

	pD3DDevice->EnumTextureFormats(&D3DEnumPixelFormatsCallback, this);

	if (currentRGBrating == TF_NULL)
		return CSC_NOTEXTURE;
	if (currentARGBrating == ATF_NULL)
		return CSC_NOALPHATEXTURE;

	// display ddraw stats
	GetTFDesc(currentRGBrating, tmp);
	wsprintf(str, "    Texture format: %s,", tmp);
	Verbose(str);
	GetATFDesc(currentARGBrating, tmp);
	wsprintf(str, "    Alpha texture format: %s,", tmp);
	Verbose(str);
	TempMsg("<TAB> to bring up Console, <SPACE> to break current Sequence", 10, height - 15, 300);

	// we're all done
	Verbose("Done...");
	Verbose(" ");
	bReady = true;
	return CSC_NOERROR;

}
#pragma optimize( "", on )


/*
	ScreenMN::InitD3DObjects()
	--------------------------
	handles switching windowed/fullscreen
*/


CSC_MSG	ScreenMN::ChangeD3DCoopLevel()
{

	CSC_MSG	errmsg;

	// release all objects that need to be re-created for the new device
	ReleaseAllD3DObjects();

    // fullscreen->windowed => restore window size
	if (bWindowed)
			Verbose("Switching: Fullscreen->Window.");
	else	Verbose("Switching: Window->Fullscreen.");

    if (bWindowed)
		SetWindowPos(hWnd, HWND_TOPMOST,
					 rcWindow.left, rcWindow.top,
					(rcWindow.right - rcWindow.left), 
					(rcWindow.bottom - rcWindow.top),
					 SWP_SHOWWINDOW );
	else
	{
		GetClientRect(hWnd, &rcScreen);
		SetWindowPos(hWnd, HWND_TOPMOST,
					 0, 0,
                     rcScreen.right, 
                     rcScreen.bottom,
					 SWP_SHOWWINDOW);
	}

    // re-create the surfaces
	errmsg = InitD3DObjects();
	if (errmsg == CSC_NOERROR)
		pDD->RestoreAllSurfaces();

	return errmsg;				// in most case this will be the so expected CSC_NOERROR

}


/*
	ScreenMN::ReleaseAllD3DObjects()
	--------------------------------
	release all used d3d objects
*/


void ScreenMN::ReleaseAllD3DObjects()
{

	if (pDD != NULL)
	{
		pDD->SetCooperativeLevel(hWnd, DDSCL_NORMAL);

		if (pD3DDevice != NULL)
			pD3DDevice->Release();
		pD3DDevice = NULL;
		if (pDDZbuffer != NULL)
			pDDZbuffer->Release();
		pDDZbuffer = NULL;
		if (pDDSBack != NULL)
			pDDSBack->Release();
		pDDSBack = NULL;
		if (pDDSPrimary != NULL)
			pDDSPrimary->Release();
		pDDSPrimary = NULL;
		if (pD3D != NULL)
			pD3D->Release();
		pD3D = NULL;
	}
	bReady = false;

}