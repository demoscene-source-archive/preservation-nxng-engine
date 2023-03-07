/*

	nXng[2.0] by emmanuel julien, xbarr/syndrome.
	1998~2000 all rights reserved.

	>>>> nX runtime implementation file.

*/


	#include	"nXng/nX_HeaderList.h"
	#include	"nXng/nXCol_FASTinlines.cpp"


// nX_scene constructor
// >>>>


nX_Scene::nX_Scene(nX *creator)
{
	memset(this, 0, sizeof(nX_Scene));
	pnX = creator;

	renderResW = pnX->screen->referencewidth;
	renderResH = pnX->screen->referenceheight;
	// default LM settings
	minLMsize = 16;
	maxLMsize = 64;
	// default sizing
	mulfactor = 0;		// autosizing
	radixprec = 16.;
	precaprec = 4;
}


// nX_scene destructor
// >>>>


nX_Scene::~nX_Scene()
{

	long		c;

	// free scene motions
	if (Focal)
		delete Focal;

	// free entities
	if (nbentity)
	{
		for (c = 0; c <= nbentity; c++)
			if (!entityadress[c]->locked)
				delete entityadress[c];
	}

	// free all IK chains
	IK_Node		*pNode, *kNode;
	kNode = LLIK;

	while (kNode)
	{
		pNode = kNode->LLnext;
		delete kNode;
		kNode = pNode;
	}

	// free all texture maps
	nX_LLTexMap	*pMap = LLtexmap, *nMap;

	while (pMap)
	{
		nMap = pMap->next;
		pMap->nodemap->Release();
		pMap->nodemap = NULL;
		delete pMap;
		pMap = nMap;
	}

	// free surfaces
	nX_Surface	*pSrf, *kSrf;
	kSrf = LLsurface;

	while (kSrf)
	{
		pSrf = kSrf->LLnext;
		delete kSrf;
		kSrf = pSrf;
	}

	// FX stuff
	if (BGmap)
		delete BGsurf;
	if (mblurlen)
		delete mblurlen;
	if (AmbientInt)
		delete AmbientInt;

	// free MOA
	if (precaadress)
		delete precaadress;

	// free Fog
	if (FogMinDist)
		delete FogMinDist;
	if (FogMaxDist)
		delete FogMaxDist;

	// we're done
	loaded = false;

}


// nX_scene setup
// >>>>


void	nX_Scene::Setup(float mulu, float rdx)
{

	// set vital state' vars
	loaded = true;
	pnX->cscene = this;

	mulfactor = mulu;		// 0.0 here would mean autosizing

	// check if user provided valid values here
	if (rdx)
		radixprec = rdx;
	else
		radixprec = 16.;

	iradixprec = 1 / radixprec;
	flaresize = mulfactor;

	// set to default viewport
	viewportUpClip = 0;
	viewportLeftClip = 0;
	viewportDownClip = pnX->screen->referenceheight;
	viewportRightClip = pnX->screen->referencewidth;

}


// nX_scene setcamera
// >>>>


void	nX_Scene::SetCamera(nX_Entity *it)
{
	if (it)
		camEntity = it;
	else
		camEntity = entityadress[0];
}


// nX_scene MOA loader
// >>>>


bool	nX_Scene::LoadMOA(char *filename)
{
	if (!pnX->Load(filename, (unsigned char **)&precaadress, NULL, NULL))
		return false;
	usepreca = true;
	return true;
}


// nX_scene set as current scene in nX
// >>>>


void	nX_Scene::SetCurrent()
{
	pnX->cscene = this;

	// AUTOMATIC VIEWPORT -----------------------------------------------------
	///////////////////////////////////////////////////////////////////////////

	long	offsetX = (pnX->screen->referencewidth - renderResW) / 2;
	long	offsetY = (pnX->screen->referenceheight - renderResH) / 2;

	SetViewport(offsetX, offsetY, renderResW + offsetX, renderResH + offsetY);
	FXtonX();
}


// nX_scene sends its fxs to nX
// >>>>


void	nX_Scene::FXtonX()
{

	if (fog)
		if (pnX->screen->cd3ddevice->fogrange)
		{
			pnX->screen->pD3DDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, TRUE);
			pnX->screen->pD3DDevice->SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_LINEAR);
			pnX->screen->pD3DDevice->SetRenderState(D3DRENDERSTATE_FOGCOLOR, fogcolorRGB);
			pnX->childusingfog = true;
		}

}


// nX_scene set rendering resolution
// >>>>


void	nX_Scene::SetRenderRes(ulong w, ulong h)
{
	renderResW = w;
	renderResH = h;
}


// nX_scene reset
// >>>>


void	nX_Scene::Reset(bool autosttick, ulong sttick)
{

	SetCurrent();

	// check if scene exists.
	if (!loaded)
		return;

	// set the scene as current in engine
	looped = false;
	frame = firstframe;
	elapsedframecount = 0;

	// reset preca
	if (usepreca)
	{
		pprecabuffer = precaadress + 16;
		opprecabuffer = precaadress + 16;
		precaframecounter = -1;
		precafacecounter = 0;
	}

	for (long c = 0; c < nbentity; c++)
		entityadress[c]->Reset();

	if (autosttick)
		previoustick = startingtick = GetTickCount();
	else
		previoustick = startingtick = sttick;
	pnX->screen->FillColor = nXcoll(&backdropcolor);

}


// nX_scene viewport settings
// >>>>


void	nX_Scene::SetViewport(ulong sx, ulong sy, ulong ex, ulong ey)
{
	viewportMidX = (sx + ex) / 2;
	viewportMidY = (sy + ey) / 2;

	// clip the submitted viewport!
	if (sx < 0) sx = 0;
	if (sy < 0) sy = 0;
	if (ex > pnX->screen->referencewidth)
		ex = pnX->screen->referencewidth;
	if (ey > pnX->screen->referenceheight)
		ey = pnX->screen->referenceheight;

	viewportX = ex - sx;
	viewportY = ey - sy;
	viewportLeftClip = sx;
	viewportUpClip = sy;
	viewportRightClip = ex;
	viewportDownClip = ey;

	// adapt focal
	if (LWfocal != 0)
	{
		Focal->values[0] = float( 1. / LWfocal );
		viewcx = float ( float(viewportX) / 2. * (float(viewportY) / viewportX) );
		viewcy = float ( viewportY / 2. );
	}
	else
	{
		Focal->values[0] = 1;
		viewcx = 1;
		viewcy = 1;
	}
}


// nX_scene clipper settings
// >>>>


void	nX_Scene::SetClipping(ulong sx, ulong sy, ulong ex, ulong ey)
{
	viewportMidX = (sx + ex) / 2;
	viewportMidY = (sy + ey) / 2;

	// clip the submitted viewport!

	if (sx < 0) sx = 0;
	if (sy < 0) sy = 0;
	if (ex > pnX->screen->referencewidth)
		ex = pnX->screen->referencewidth;
	if (ey > pnX->screen->referenceheight)
		ey = pnX->screen->referenceheight;

	viewportX = ex - sx;
	viewportY = ey - sy;
	viewportLeftClip = sx;
	viewportUpClip = sy;
	viewportRightClip = ex;
	viewportDownClip = ey;
}