//	------------------------------------------------------------------------------
//
//		nXv9 by xBaRr/Syndrome 2000
//			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
//			All rights reserved 2000.
//
//		Part of nXng.
//		mailto://xbarr@pingoo.com
//		http://xbarr.planet-d.net
//
//		nXng Scene implementation file
//
//	------------------------------------------------------------------------------


	#include	"nX_HeaderList.h"
	#include	"IO/cqString.h"
	#include	"IO/MCIO.h"

	static		cMath	cMth;
	static		char	str[256];
	extern		unsigned char	mulshader[65536];


//--------------------------------------------
void	nX_Scene::GetAllTexMapiD3D()
// nX_scene get all texmap Direct3D interfaces
//--------------------------------------------
{
	nX_LLTexMap	*pMap = LLtexmap;

	while (pMap)
	{
		pMap->nodemap->GetiD3D();
		pMap = pMap->next;
	}

	if (BGmap)
		BGsurf->map[0]->GetiD3D();
}


//---------------------------------------------
void	nX_Scene::FreeAllTexMapiD3D()
// nX_scene free all texmap Direct3D interfaces
//---------------------------------------------
{
	nX_LLTexMap	*pMap = LLtexmap;

	while (pMap)
	{
		if (!pMap->nodemap->RefCount)
		{
			pMap->nodemap->sD3D->Release();
			pMap->nodemap->sD3D = NULL;
		}
		pMap = pMap->next;
	}

	if (BGmap)
	{
		BGsurf->map[0]->sD3D->Release();
		BGsurf->map[0]->sD3D = NULL;
	}
}


//--------------------------------------------------------------------------------
void	nX_Scene::TransformVertice(ulong nbp, float *sp, float *dp, float *matrix)
//--------------------------------------------------------------------------------
{
	for (ulong c = 0; c < nbp; c++)
	{
		dp[0] = sp[0] * matrix[0] + sp[1] * matrix[4] + sp[2] * matrix[8] + matrix[12];
		dp[1] = sp[0] * matrix[1] + sp[1] * matrix[5] + sp[2] * matrix[9] + matrix[13];
		dp[2] = sp[0] * matrix[2] + sp[1] * matrix[6] + sp[2] * matrix[10] + matrix[14];
		sp += 3; dp += 3;
	}
}


//-------------------------------------------------------------------------------
void	nX_Scene::TransformNormal(ulong nbp, float *sp, float *dp, float *matrix)
//-------------------------------------------------------------------------------
{
	for (ulong c = 0; c < nbp; c++)
	{
		dp[0] = sp[0] * matrix[0] + sp[1] * matrix[3] + sp[2] * matrix[6];
		dp[1] = sp[0] * matrix[1] + sp[1] * matrix[4] + sp[2] * matrix[7];
		dp[2] = sp[0] * matrix[2] + sp[1] * matrix[5] + sp[2] * matrix[8];
		sp += 3; dp += 3;
	}
}


//-------------------------------------------------------------------------------
void	nX_Scene::Morph(ulong nbp, float *sp, float *dp, float *mt, float amount)
//-------------------------------------------------------------------------------
{
	for (ulong c = 0; c < nbp; c++)
	{
		dp[0] = sp[0] + mt[0] * amount;
		dp[1] = sp[1] + mt[1] * amount;
		dp[2] = sp[2] + mt[2] * amount;
		sp += 3; mt += 3; dp += 3;
	}
}


//-----------------------------------------------------
void	nX_Scene::Worm(ulong nbp, float *sp, float *dp)
//		worm-morph effector
//-----------------------------------------------------
{
	float	base = sp[2], delta;
	float	angle = frame / 10.0f;
	float	dispx, dispy;
	float	offx, offy;
	float	nd;

	for (ulong c = 0; c < nbp; c++)
	{
		delta = (sp[2] - base) / 100.0f;
		nd = (float)sin((angle * 4.0f + delta) / 4.0f);
		dispx = (float)cos((angle * 2.0f + delta) / 6.0f) * 150.0f;
		dispy = (float)sin((angle * 4.0f + delta) / 4.0f) * 140.0f;
		offx = (float)sin(angle + delta) * 0.1f + 0.7f;
		offy = (float)sin(angle + delta) * 0.1f + 0.7f;

		dp[0] = sp[0] * offx + dispx;
		dp[1] = sp[1] * offy + dispy;
		dp[2] = sp[2];

		sp += 3; dp += 3;
	}
}


//---------------------------
void	nX_Scene::GetMatrix()
//---------------------------
{
	nX_Entity	*entity = camEntity;

	float	aX = -entity->angle[0], aY = -entity->angle[1], aZ = -entity->angle[2];

	rmatrix[0] = float((pnX->qCos[radsincos(aY)] * pnX->qCos[radsincos(aZ)]) - (pnX->qSin[radsincos(aY)] * pnX->qSin[radsincos(aX)] * pnX->qSin[radsincos(aZ)]));
	rmatrix[1] = float((pnX->qCos[radsincos(aY)] * pnX->qSin[radsincos(aZ)]) + (pnX->qSin[radsincos(aY)] * pnX->qSin[radsincos(aX)] * pnX->qCos[radsincos(aZ)]));
	rmatrix[2] = float(-pnX->qSin[radsincos(aY)] * pnX->qCos[radsincos(aX)]);
	rmatrix[3] = float(-pnX->qCos[radsincos(aX)] * pnX->qSin[radsincos(aZ)]);
	rmatrix[4] = float( pnX->qCos[radsincos(aX)] * pnX->qCos[radsincos(aZ)]);
	rmatrix[5] = float( pnX->qSin[radsincos(aX)]);
	rmatrix[6] = float((pnX->qCos[radsincos(aY)] * pnX->qSin[radsincos(aX)] * pnX->qSin[radsincos(aZ)]) + (pnX->qSin[radsincos(aY)] * pnX->qCos[radsincos(aZ)]));
	rmatrix[7] = float((pnX->qSin[radsincos(aY)] * pnX->qSin[radsincos(aZ)]) - (pnX->qCos[radsincos(aY)] * pnX->qSin[radsincos(aX)] * pnX->qCos[radsincos(aZ)]));
	rmatrix[8] = float( pnX->qCos[radsincos(aY)] * pnX->qCos[radsincos(aX)]);

	// mpivot * mscale * mrotate * mtranslate result matrix

	matrix[0] = rmatrix[0];	matrix[1] = rmatrix[1]; matrix[2] = rmatrix[2]; matrix[3] = 0;
	matrix[4] = rmatrix[3];	matrix[5] = rmatrix[4]; matrix[6] = rmatrix[5]; matrix[7] = 0;
	matrix[8] = rmatrix[6];	matrix[9] = rmatrix[7]; matrix[10] = rmatrix[8]; matrix[11] = 0;
	matrix[12] = -entity->offset[0] * rmatrix[0] + entity->offset[1] * rmatrix[3] - entity->offset[2] * rmatrix[6];
	matrix[13] = -entity->offset[0] * rmatrix[1] + entity->offset[1] * rmatrix[4] - entity->offset[2] * rmatrix[7];
	matrix[14] = -entity->offset[0] * rmatrix[2] + entity->offset[1] * rmatrix[5] - entity->offset[2] * rmatrix[8];
	matrix[15] = 1;
}


//---------------------------------
void	nX_Scene::UpdateSurfaceFx()
//---------------------------------
{

	nX_Surface	*surf;
	surf = LLsurface;

	for (long c = 0; c < nbsurface; c++)
	{
		for (long c2 = 0; c2 < nX_MAXTEXLEVEL; c2++)
		{
			surf->offsetx[c2] += surf->velocity[c2][0] * elapsedframecount;
			surf->offsety[c2] += surf->velocity[c2][1] * elapsedframecount;
			surf->offsetz[c2] += surf->velocity[c2][2] * elapsedframecount;
		}
		surf = surf->LLnext;
	}

}


//--------------------------------------------------------------------------
float	nX_Scene::GetBehaviorFrame(long endb, float lf, float offset)
// function to return a frame within the range of an end behaviored movement
//--------------------------------------------------------------------------
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
			envframe = cMth.Modulo(frm, lf);
			break;
	}

	return envframe;
}


///////////////////////////////////////////////////////////////////////////////
void	nX_Scene::GetInterruptFrame(bool loop)
// Function to compute frame advancement for a given scene
///////////////////////////////////////////////////////////////////////////////
{
	unsigned long	tick = GetTickCount();
	elapsedtickcount = (float)(tick - previoustick);
	pnX->FPS = (unsigned short)(1000.0f / elapsedtickcount);

	if (!pnX->Synch)
	{
		elapsedframecount = 1;
		precaframecounter++;
		frame++;
	}
	else
	{
		elapsedframecount = elapsedtickcount / 1000.0f * fps;
		previoustick = tick;
		precaframecounter += elapsedframecount;
		frame += elapsedframecount;
	}
	if (frame >= lastframe)
	{
		looped = true;
		loopcount++;
		if (loop)
		{
			frame = 0;
			startingtick = tick;
			Reset(true, NULL);
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
void	nX_Scene::GetNextFrame()
// Function to compute next timed frame
///////////////////////////////////////////////////////////////////////////////
{
	unsigned long tick = GetTickCount();
	elapsedframecount = 1;
	previoustick = (unsigned long)tick;
	precaframecounter += 1;
	frame += 1;

	if (frame >= lastframe)
	{
		looped++;
		frame = 0;
		startingtick = tick;
	}
}


///////////////////////////////////////////////////////////////////////////////
bool inline SwapVtxBuf(nX *pnX, bool db)
///////////////////////////////////////////////////////////////////////////////
{
	if (db)
	{
		pnX->p3dpswapt = pnX->p3dpswap1;
		pnX->p3dpswap1 = pnX->p3dpswap2;
		pnX->p3dpswap2 = pnX->p3dpswapt;
		return true;
	}
	else
	{
		pnX->p3dpswap1 = pnX->edgesbuffer1_adress;
		pnX->p3dpswap2 = pnX->edgesbuffer2_adress;
		return true;
	}
}


///////////////////////////////////////////////////////////////////////////////
bool inline SwapFNormBuf(nX *pnX, bool db)
///////////////////////////////////////////////////////////////////////////////
{
	if (db)
	{
		pnX->pfnormt = pnX->pfnorm1;
		pnX->pfnorm1 = pnX->pfnorm2;
		pnX->pfnorm2 = pnX->pfnormt;
		return true;
	}
	else
	{
		pnX->pfnorm1 = pnX->facenormalsA_adress;
		pnX->pfnorm2 = pnX->facenormalsB_adress;
		return true;
	}
}


///////////////////////////////////////////////////////////////////////////////
bool inline SwapENormBuf(nX *pnX, bool db)
///////////////////////////////////////////////////////////////////////////////
{
	if (db)
	{
		pnX->penormt = pnX->penorm1;
		pnX->penorm1 = pnX->penorm2;
		pnX->penorm2 = pnX->penormt;
		return true;
	}
	else
	{
		pnX->penorm1 = pnX->edgenormalsA_adress;
		pnX->penorm2 = pnX->edgenormalsB_adress;
		return true;
	}
}


///////////////////////////////////////////////////////////////////////////////
void	nX_Scene::ComputeAllLWMotions()
///////////////////////////////////////////////////////////////////////////////
{

	nX_Entity		*entity, *entity2;
	long			c;
	ocscene = pnX->cscene;
	pnX->cscene = this;

	if (fog)
	{
		FogMinDist->GetValues(frame);
		FogMaxDist->GetValues(frame);
	}

	// Ambient light

	AmbientInt->GetValues(frame);
	if (AmbientInt->useenvelop)
	{
		if (AmbientInt->values[0] < 0) AmbientInt->values[0] = 0;
		iambientcolor.r = mulshader[ambientcolor.r * 256 + long(AmbientInt->values[0] * 255)];
		iambientcolor.g = mulshader[ambientcolor.g * 256 + long(AmbientInt->values[0] * 255)];
		iambientcolor.b = mulshader[ambientcolor.b * 256 + long(AmbientInt->values[0] * 255)];
	}

	// Focal

	if (pnX->screen->bFreeRide)
			Focal->GetValues(lastframe);
	else	Focal->GetValues(frame);
	if (Focal->useenvelop)
	{
		LWfocal = Focal->values[0];
		Focal->values[0] = 1.0f / LWfocal;
		viewcx =  ( float(viewportX) / 2.0f * (float(viewportY) / viewportX) );
		viewcy = viewportY / 2.0f;
	}

	// Entity motion

	float	head, pitch;
	float	dx, dy, dz, hyp;

	if (pnX->screen->bFreeRide)
	{
		c = 1;
		entityadress[0]->ReadMotionFromUser();
	}
	else
		c = 0;

	for (; c <= nbentity; c++)
	{

		entity = entityadress[c];
		entity->GetLWMotion(frame);
		entity->GetExtraLWMotions(frame);

		// TARGETING

		if (entity->dtarget)
		{
			entity2 = entity->dtarget;

			// targeting vector
			dx = entity2->offset[0] - entity->offset[0];
			dy = entity2->offset[1] - entity->offset[1];
			dz = entity2->offset[2] - entity->offset[2];

			// heading (align heading using two-axis)
			hyp = (float)sqrt( dx * dx + dz * dz );
			head = (float)acos( dz / hyp );
			if (dx < 0)	head = -head;

			// pitch (align pitch using full space)
			hyp = (float)sqrt( dx * dx + dy * dy + dz * dz );
			pitch = (float)asin( dy / hyp );

			// okydoky
			entity->angle[0] = pitch;
			entity->angle[1] = head;
		}

	}

	// RESOLVE IK CHAINS (BUGGY from nXng[2.0] made changes in matrix work scheme)
/*
	IK_Node	*IKNode;
	IKNode = LLIK;

	while (IKNode)
	{
		IKNode->IKsrc->ResolveIK();
		IKNode = IKNode->LLnext;
	}
*/
	pnX->cscene = ocscene;

}


///////////////////////////////////////////////////////////////////////////////
void	nX_Scene::GetAllMatrix()
///////////////////////////////////////////////////////////////////////////////
{

	ocscene = pnX->cscene;
	pnX->cscene = this;

	// World matrix
	GetMatrix();

	// Actor's self matrix
	for (long c = 0; c <= nbentity; c++)
		entityadress[c]->GetMatrix();

	// Actor's full matrix
	for (c = 0; c <= nbentity; c++)
	{
		entityadress[c]->GetHMatrix();
		pnX->MatMul44(entityadress[c]->hmatrix, matrix, entityadress[c]->fmatrix);
		pnX->MatMul33(entityadress[c]->hrmatrix, rmatrix, entityadress[c]->frmatrix);
	}

	pnX->cscene = ocscene;

}


///////////////////////////////////////////////////////////////////////////////
void	nX_Scene::DisplayWorld()
///////////////////////////////////////////////////////////////////////////////
{

	if (dissolve >= 1)
		return;

	nX_Spark		*spark;
	nX_SparkEmiter	*emit;
	nX_Entity		*entity;
	long			c, c2, alive;

	SetCurrent();

	// # object statistics
	sMOAobj = 0;
	sBBXobj = 0;

	// SCENE MOTION -----------------------------------------------------------

	if (usepreca)
	{
		while (precaframecounter >= PRECAREFRESHRATE)
		{
			precaframecounter -= PRECAREFRESHRATE;
			
			// did we reached end of motion ?
			if (opprecabuffer[0] == -1 || frame >= lastframe)
			{
				frame = 0;
				precaframecounter = 1;
				opprecabuffer = precaadress;
				startingtick = GetTickCount();
			}
			else
				opprecabuffer += opprecabuffer[0] + 1;
		}
		pprecabuffer = opprecabuffer + 1;
		packetnbface = opprecabuffer[0];
		precafacecounter = 0;
		facewastodraw = 0;
	}

	// Update FXs
	UpdateSurfaceFx();

	// Lens rotation
	flareangle -= 0.05f * elapsedframecount;

	// LIGHTS

	for (c = firstlightno; c <= nbentity; c++)
	{
		entity = entityadress[c];

		// CAST LENSFLARE
		if (entity->lensflare)
			entity->SpLensFX(entity->fmatrix[12], entity->fmatrix[13], entity->fmatrix[14], entity->flaresurface);
	}

	// OBJECT FACE CREATION ---------------------------------------------------

	bool	vtxdbbuffer, fndbbuffer, endbbuffer;

	for (c = 1; c <= nbobj; c++)
	{
		entity = entityadress[c];

		// handle nX custom entities

		switch (entity->type)
		{

			// METAOBJECTS are to be add to scene's metagrid
			case nXETYPE_METABALL:
/*				entity->hmatrix[12]
				entity->hmatrix[13]
				entity->hmatrix[14]*/
				break;

			// spark emiter
			case nXETYPE_SPARKEMITER:

				emit = entity->sparkemiter;
				spark = emit->sparklist;

				// update spark positions

				alive = 0;
				pnX->p3dpswap1 = pnX->edgesbuffer2_adress;

				for (c2 = 0; c2 < emit->count; c2++)
				{
					if (spark->deathwish > 0)
					{
						spark->x += spark->vx * elapsedframecount;
						spark->y += spark->vy * elapsedframecount;
						spark->z += spark->vz * elapsedframecount;
						spark->vy += 0.001f * elapsedframecount * entity->scale[1];
						spark->deathwish -= elapsedframecount;

						pnX->p3dpswap1[0] = spark->x;
						pnX->p3dpswap1[1] = spark->y;
						pnX->p3dpswap1[2] = spark->z;
						pnX->p3dpswap1 += 3;

						alive++;
					}
					else if (spark->delay < 0)
					{
						spark->x = entity->hmatrix[12];
						spark->y = entity->hmatrix[13];
						spark->z = entity->hmatrix[14];
						spark->vx = (float)(rand() - 16383) / (655360) * entity->scale[0] * mulfactor / 3;
						spark->vy = 0.02f * entity->scale[1] * mulfactor / 1.5f;
						spark->vz = (rand() - 16383) / (655360) * entity->scale[2]  * mulfactor / 3;
						spark->deathwish = 60.0f + (float)(rand() - 16383) / 655360;
					}
					else
						spark->delay -= elapsedframecount;

					spark++;
				}

				// add emiter as a flare
				pnX->p3dpswap1[0] = entity->hmatrix[12];
				pnX->p3dpswap1[1] = entity->hmatrix[13];
				pnX->p3dpswap1[2] = entity->hmatrix[14];

				// rotate sparks trough camera

				TransformVertice(alive + 1, pnX->edgesbuffer2_adress, pnX->edgesbuffer1_adress, matrix);
				pnX->p3dpswap2 = pnX->edgesbuffer1_adress;

				// draw them to screen

				for (c2 = 0; c2 < alive; c2++)
				{
					entity->LensFX(pnX->p3dpswap2[0], pnX->p3dpswap2[1], pnX->p3dpswap2[2], pnX->DefaultFlareSurface);
					pnX->p3dpswap2 += 3;
				}
				entity->SpLensFX(pnX->p3dpswap2[0], pnX->p3dpswap2[1], pnX->p3dpswap2[2], pnX->DefaultFlareSurface);
			break;


			// standard object or null object

			case nXETYPE_OBJECT:

				// CHECK MOA FOR OBJECT SKIPPING

				if (!entity->nbpoints)
					break;
				if (usepreca && entity->PolySize->values[0] == 1)
				{
					if (facewastodraw == packetnbface)
						goto nomoreObj;
					if (precafacecounter + entity->nbface <= pprecabuffer[0])
					{
						precafacecounter += entity->nbface;
						sMOAobj++;
						break;		// skip this object
					}	
				}

				// HANDLE EFFECTORS -------------------------------------------

				if (entity->wave)
					entity->ApplyWave();
				else if (entity->usemdd)
					entity->ApplyMDD();

				// HANDLE STATIC OBJECTS --------------------------------------

				if (entity->staticinscene && entity->freezed)
				{
					pnX->p3dpswap1 = entity->previous_edge;
					pnX->p3dpswap2 = pnX->edgesbuffer1_adress;
					pnX->pfnorm1 = entity->previous_facenormals;
					pnX->pfnorm2 = pnX->facenormalsA_adress;
					pnX->penorm1 = entity->previous_edgenormals;
					pnX->penorm2 = pnX->edgenormalsA_adress;
				}
				else

				// HANDLE MOVING OBJECTS --------------------------------------
				///////////////////////////////////////////////////////////////

				{

					// DOUBLE BUFFER VERTICE/NORMALS

					vtxdbbuffer = false;
					pnX->p3dpswap1 = entity->edge_adress;
					pnX->p3dpswap2 = pnX->edgesbuffer1_adress;
					fndbbuffer = false;
					pnX->pfnorm1 = entity->facenormals_adress;
					pnX->pfnorm2 = pnX->facenormalsA_adress;
					endbbuffer = false;
					pnX->penorm1 = entity->edgenormals_adress;
					pnX->penorm2 = pnX->edgenormalsA_adress;

					// TRANSFORM VERTICE/NORMAL (MORPH/GIZMO/BONES)
					
					// Worm effector
					if (entity->useworm)
					{
						Worm(entity->nbpoints, pnX->p3dpswap1, pnX->p3dpswap2);
						vtxdbbuffer = SwapVtxBuf(pnX, vtxdbbuffer);
					}

					// Lightwave's silly metamorphing
					if (entity->Morph && entity->Morph->values[0] != 0)
					{
						Morph(entity->nbpoints, pnX->p3dpswap1, pnX->p3dpswap2, entity->xyzmorphdeltas, entity->Morph->values[0]);
						vtxdbbuffer = SwapVtxBuf(pnX, vtxdbbuffer);

						if ( entity->requirement & nX_USEFACENORMAL )
						{
							Morph(entity->nbface, pnX->pfnorm1, pnX->pfnorm2, entity->nfamorphdeltas, entity->Morph->values[0]);
							fndbbuffer = SwapFNormBuf(pnX, fndbbuffer);
						}
						if ( entity->requirement & nX_USEEDGENORMAL )
						{
							if (entity->stdenormal)
									Morph(entity->nbpoints, pnX->penorm1, pnX->penorm2, entity->nedmorphdeltas, entity->Morph->values[0]);
							else	Morph(entity->totalglobaledgenumber, pnX->penorm1, pnX->penorm2, entity->nedmorphdeltas, entity->Morph->values[0]);
							endbbuffer = SwapENormBuf(pnX, endbbuffer);
						}
					}

					// Enhanced morph via GIZMO
/*
					if (entity->gizmoadress)
					{
						Gizmo(entity->nbpoints, pnX->p3dpswap1, pnX->p3dpswap2, entity->gizmoadress, entity->gizmoadress->xyzmorphdeltas);
						vtxdbbuffer = SwapVtxBuf(pnX, vtxdbbuffer);

						if ( entity->requirement & nX_USEFACENORMAL )
						{
							Gizmo(entity->nbface, pnX->pfnorm1, pnX->pfnorm2, entity->gizmoadress, entity->gizmoadress->nfamorphdeltas);
							fndbbuffer = SwapFNormBuf(pnX, fndbbuffer);
						}
						if ( entity->requirement & nX_USEEDGENORMAL )
						{
							Gizmo(entity->totalglobaledgenumber, pnX->penorm1, pnX->penorm2, entity->gizmoadress, entity->gizmoadress->nedmorphdeltas);
							endbbuffer = SwapENormBuf(pnX, endbbuffer);
						}
					}
*/
					// copy buffers if entity's static!

					if (entity->staticinscene)
					{
						memcpy(entity->previous_edge, pnX->p3dpswap1, entity->nbpoints * 3 * 4);
						if ( entity->requirement & nX_USEFACENORMAL )
							memcpy(entity->previous_facenormals, pnX->pfnorm1, entity->nbface * 3 * 4);
						if ( entity->requirement & nX_USEEDGENORMAL )
						{
							if (entity->stdenormal)
									memcpy(entity->previous_edgenormals, pnX->penorm1, entity->nbpoints * 3 * 4);
							else	memcpy(entity->previous_edgenormals, pnX->penorm1, entity->totalglobaledgenumber * 3 * 4);
						}
						entity->freezed = true;		// SNAPSHOOTED! [;
					}

				}

				// ALL OBJECTS DEFAULT HERE! ----------------------------------
				///////////////////////////////////////////////////////////////

				if (entity->usemdd)
						TransformVertice(entity->nbpoints, pnX->p3dpswap1, pnX->p3dpswap2, matrix);
				else	TransformVertice(entity->nbpoints, pnX->p3dpswap1, pnX->p3dpswap2, entity->fmatrix);

				entity->TestBBVisibility();
				if (!usepreca && !entity->visibility)
					sBBXobj++;

				if (entity->visibility || usepreca)
				{
					if (entity->usemdd)
					{
						if ( entity->requirement & nX_USEFACENORMAL )
							TransformNormal(entity->nbface, pnX->pfnorm1, pnX->pfnorm2, rmatrix);
						if ( entity->requirement & nX_USEEDGENORMAL )
							TransformNormal(entity->totalglobaledgenumber, pnX->penorm1, pnX->penorm2, rmatrix);
					}
					else
					{
						if ( entity->requirement & nX_USEFACENORMAL )
							TransformNormal(entity->nbface, pnX->pfnorm1, pnX->pfnorm2, entity->frmatrix);
						if ( entity->requirement & nX_USEEDGENORMAL )
						{
							if (entity->stdenormal)
									TransformNormal(entity->nbpoints, pnX->penorm1, pnX->penorm2, entity->frmatrix);
							else	TransformNormal(entity->totalglobaledgenumber, pnX->penorm1, pnX->penorm2, entity->frmatrix);
						}
					}

					if (entity->PolySize->values[0] == 1.)
							entity->Freeze();		// create polys
					else	pnX->EdgeAsSparks(entity, pnX->p3dpswap2);

					entity->DrawTrail(pnX->p3dpswap2);
				}

			break;

			// END OF ETYPE SWITCH

		}

	}
	nomoreObj:;


	// RENDER ISO GRID --------------------------------------------------------


//	if (hasmetaobject)
//		metagrid->Render(1.0);

	// BACKGROUND IMAGE FX (MUST be last call to ValidFace!) ------------------


	if (BGmap)
	{
		if (BGsurf->map[0])
			BGsurf->map[0]->Update();
		pnX->Square(viewportLeftClip, viewportUpClip,
				viewportRightClip, viewportDownClip,
				pnX->largestZ + 1.0f, (long)radixprec, BGsurf, dissolve, NULL);
	}

	if (pnX->screen->bUseFrameReport)
		pnX->Square(0, 0, 640, 60, 1.5f, (long)radixprec, pnX->ReportSurf, 0.5f, NULL);

}

void	nX_Scene::Resize(float s)
{

	// Resize all entities in scene
	for (long c = 0; c <= nbentity; c++)
		entityadress[c]->Resize(s);

	// Extra CAPS
	KF	*pKF;

	if (FogMinDist)
	{
		if (FogMinDist->useenvelop)
		{
			pKF = FogMinDist->KFadrss;
			for (c = 0; c < FogMinDist->nbKF; c++)
				pKF[c].cv[0] *= s;
		}
		else
			FogMinDist->values[0] *= s;
	}
	if (FogMaxDist)
	{
		if (FogMaxDist->useenvelop)
		{
			pKF = FogMaxDist->KFadrss;
			for (c = 0; c < FogMaxDist->nbKF; c++)
				pKF[c].cv[0] *= s;
		}
		else
			FogMaxDist->values[0] *= s;
	}

}


///////////////////////////////////////////////////////////////////////////////
void	nX_Scene::RenderLWScene()
///////////////////////////////////////////////////////////////////////////////
{

	ComputeAllLWMotions();
	GetAllMatrix();
	DisplayWorld();

}
