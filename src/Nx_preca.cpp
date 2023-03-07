

	#include	<stdio.h>
	#include	"nXng/nX_HeaderList.h"


float inline clipvalue(float v0, float v1, float coef)
{
	return (v1-v0)*coef+v0;
}

float inline zclipvalue(float v0, float v1, float coef)
{
	v0 = 1/v0;
	v1 = 1/v1;
	return 1 / ((v1 - v0) * coef + v0);
}

bool inline SwapVtxBuf(nX *pnX, bool db)
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

bool inline SwapFNormBuf(nX *pnX, bool db)
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

bool inline SwapENormBuf(nX *pnX, bool db)
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


/*

	GetMOAFilename(char *)

*/


void	nX::GetMOAFilename(char *filename)
{

	if (!filename)
		return;
	unsigned short	len;

	if ( (len = strlen(filename)) > 3 )
	{
		filename[len - 3] = 'm';
		filename[len - 2] = 'o';
		filename[len - 1] = 'a';
		filename[len] = 0;
	}

}


/*

	CheckMOACompliance(char *)

*/


bool	nX::CheckMOACompliance(char *file, long scenesize)
{

	// file exist?

	char	*moa, *omoa;
	unsigned long	sze;
	if (!Load(file, (unsigned char **)&moa, &sze, NULL))
		return false;
	omoa = moa;

	// version test

	if (strncmp(moa, "MOA4", 4))
		goto failure;

	moa += 4;

	// scene filesize test

	if (*(long *)moa != scenesize)
		goto failure;	

	moa += 4;

	// suceeded

	delete omoa;
	return true;

	// failed

failure:;
	delete omoa;
	return false;

}


/*

	PrecaClipFace

*/


bool	nX_Scene::PrecaClipFace()
{

		nX_vertex	workvertex0[nX_MAXEDGEPOLY],
					workvertex1[nX_MAXEDGEPOLY];			// scratch buffers
		nX_vertex	*pvertex = pnX->pfacelist->vertice;		// get current face vertex list
		nX_Surface	*surf = pnX->pfacelist->surf;
		nX_vertex	*pworkvertex0, *pworkvertex1;

		long		nedge, c;
		float		coef;

		nedge = pnX->pfacelist->nedge;
		memcpy(pvertex + nedge, pvertex, sizeof(nX_vertex));

		// Z-cut
		////////

		pworkvertex0 = workvertex0;
		pnX->pfacelist->nedge = 0;

		for (c = 0; c < nedge; c++)
		{
			if ( pvertex->z >= zcut && (pvertex+1)->z >= zcut )
			{
				memcpy(pworkvertex0++, pvertex, sizeof(nX_vertex));
				pnX->pfacelist->nedge++;	
			}
			else if ( pvertex->z < zcut && (pvertex+1)->z >= zcut )
			{
				coef=float ( 1/ (pvertex->z-(pvertex+1)->z)*(zcut-(pvertex+1)->z) );

				pworkvertex0->x = clipvalue((pvertex+1)->x, pvertex->x, coef);
				pworkvertex0->y = clipvalue((pvertex+1)->y, pvertex->y, coef);
				pworkvertex0->z = zcut;
				pworkvertex0++;
				pnX->pfacelist->nedge++;	
			}
			else if ( pvertex->z >= zcut && (pvertex+1)->z < zcut )
			{
				coef=float ( 1/ (pvertex->z-(pvertex+1)->z)*(zcut-(pvertex+1)->z) );

				memcpy(pworkvertex0++, pvertex, sizeof(nX_vertex));
				pworkvertex0->x=clipvalue((pvertex+1)->x, pvertex->x, coef);
				pworkvertex0->y=clipvalue((pvertex+1)->y, pvertex->y, coef);
				pworkvertex0->z=zcut;
				pworkvertex0++;
				pnX->pfacelist->nedge+=2;
			}
			pvertex++;
		}

		if (pnX->pfacelist->nedge < 3)
			return false;


		// PROJECTION 3D>>2D
		////////////////////

		pworkvertex0 = workvertex0;

		for (c = 0; c < pnX->pfacelist->nedge; c++)
		{
			coef = float (1.0 / (pworkvertex0->z * Focal->values[0]));

			pworkvertex0->x *= coef * viewcx;
			pworkvertex0->x += viewportMidX;
			pworkvertex0->y *= coef * viewcy;
			pworkvertex0->y += viewportMidY;

			pworkvertex0++;
		}

		nedge = pnX->pfacelist->nedge;
		pworkvertex0 = workvertex0;
		memcpy(pworkvertex0 + nedge, pworkvertex0, sizeof(nX_vertex));

		// CLIPPING UP
		//////////////

		pworkvertex1 = workvertex1;
		pnX->pfacelist->nedge = 0;

		for (c = 0; c < nedge; c++)
		{
			if (pworkvertex0->y >= 0 && (pworkvertex0+1)->y >= 0)
			{
				memcpy(pworkvertex1++, pworkvertex0, sizeof(nX_vertex));
				pnX->pfacelist->nedge++;	
			}
			else if (pworkvertex0->y < 0 && (pworkvertex0+1)->y >= 0)
			{
				coef = float ( 1/ (pworkvertex0->y-(pworkvertex0+1)->y)*(0-(pworkvertex0+1)->y) );

				pworkvertex1->x=clipvalue((pworkvertex0+1)->x, pworkvertex0->x, coef);
				pworkvertex1->y=float (0);
				pworkvertex1->z=zclipvalue((pworkvertex0+1)->z, pworkvertex0->z, coef);
				pworkvertex1++;
				pnX->pfacelist->nedge++;	
			}
			else if (pworkvertex0->y >= 0 && (pworkvertex0+1)->y < 0)
			{
				coef = float ( 1/ (pworkvertex0->y-(pworkvertex0+1)->y)*(0-(pworkvertex0+1)->y) );

				memcpy(pworkvertex1++, pworkvertex0, sizeof(nX_vertex));
				pworkvertex1->x=clipvalue((pworkvertex0+1)->x, pworkvertex0->x, coef);
				pworkvertex1->y=float (0);
				pworkvertex1->z=zclipvalue((pworkvertex0+1)->z, pworkvertex0->z, coef);
				pworkvertex1++;
				pnX->pfacelist->nedge+=2;	
			}
			pworkvertex0++;
		}

		if (pnX->pfacelist->nedge < 3)
			return false;

		nedge = pnX->pfacelist->nedge;
		pworkvertex1 = workvertex1;
		memcpy(pworkvertex1 + nedge, pworkvertex1, sizeof(nX_vertex));

		// CLIPPING DOWN
		////////////////

		pworkvertex0 = workvertex0;
		pnX->pfacelist->nedge = 0;

		for (c = 0; c < nedge; c++)
		{
			if (pworkvertex1->y < pnX->screen->referenceheight && (pworkvertex1+1)->y < pnX->screen->referenceheight)
			{
				memcpy(pworkvertex0++, pworkvertex1, sizeof(nX_vertex));
				pnX->pfacelist->nedge++;	
			}
			else if (pworkvertex1->y >= pnX->screen->referenceheight && (pworkvertex1+1)->y < pnX->screen->referenceheight)
			{
				coef=float ( 1/ (pworkvertex1->y-(pworkvertex1+1)->y)*(pnX->screen->referenceheight-(pworkvertex1+1)->y) );

				pworkvertex0->x=clipvalue((pworkvertex1+1)->x, pworkvertex1->x, coef);
				pworkvertex0->y=float (pnX->screen->referenceheight);
				pworkvertex0->z=zclipvalue((pworkvertex1+1)->z, pworkvertex1->z, coef);
				pworkvertex0++;
				pnX->pfacelist->nedge++;	
			}
			else if (pworkvertex1->y < pnX->screen->referenceheight && (pworkvertex1+1)->y >= pnX->screen->referenceheight)
			{
				coef=float ( 1/ (pworkvertex1->y-(pworkvertex1+1)->y)*(pnX->screen->referenceheight-(pworkvertex1+1)->y) );

				memcpy(pworkvertex0++, pworkvertex1, sizeof(nX_vertex));
				pworkvertex0->x=clipvalue((pworkvertex1+1)->x, pworkvertex1->x, coef);
				pworkvertex0->y=float (pnX->screen->referenceheight);
				pworkvertex0->z=zclipvalue((pworkvertex1+1)->z, pworkvertex1->z, coef);
				pworkvertex0++;
				pnX->pfacelist->nedge+=2;	
			}
			pworkvertex1++;
		}

		if (pnX->pfacelist->nedge < 3)
			return false;

		nedge = pnX->pfacelist->nedge;
		pworkvertex0 = workvertex0;
		memcpy(pworkvertex0 + nedge, pworkvertex0, sizeof(nX_vertex));

		// CLIPPING LEFT
		////////////////

		pworkvertex1 = workvertex1;
		pnX->pfacelist->nedge = 0;

		for (c = 0; c < nedge; c++)
		{
			if (pworkvertex0->x >= 0 && (pworkvertex0+1)->x >= 0)
			{
				memcpy(pworkvertex1++, pworkvertex0, sizeof(nX_vertex));
				pnX->pfacelist->nedge++;	

			}
			else if (pworkvertex0->x < 0 && (pworkvertex0+1)->x >= 0)
			{
				coef = float ( 1/ (pworkvertex0->x-(pworkvertex0+1)->x)*(0-(pworkvertex0+1)->x) );

				pworkvertex1->x=float (0);
				pworkvertex1->y=clipvalue((pworkvertex0+1)->y, pworkvertex0->y, coef);
				pworkvertex1->z=zclipvalue((pworkvertex0+1)->z, pworkvertex0->z, coef);
				pworkvertex1++;
				pnX->pfacelist->nedge++;	
			}
			else if (pworkvertex0->x >= 0 && (pworkvertex0+1)->x < 0)
			{
				coef = float ( 1/ (pworkvertex0->x-(pworkvertex0+1)->x)*(0-(pworkvertex0+1)->x) );

				memcpy(pworkvertex1++, pworkvertex0, sizeof(nX_vertex));
				pworkvertex1->x=float (0);
				pworkvertex1->y=clipvalue((pworkvertex0+1)->y, pworkvertex0->y, coef);
				pworkvertex1->z=zclipvalue((pworkvertex0+1)->z, pworkvertex0->z, coef);
				pworkvertex1++;
				pnX->pfacelist->nedge+=2;	
			}
			pworkvertex0++;
		}

		if (pnX->pfacelist->nedge < 3)
			return false;

		nedge = pnX->pfacelist->nedge;
		pworkvertex1 = workvertex1;
		memcpy(pworkvertex1 + nedge, pworkvertex1, sizeof(nX_vertex));

		// CLIPPING RIGHT
		/////////////////

		pworkvertex0 = workvertex0;
		pnX->pfacelist->nedge = 0;

		for (c = 0; c < nedge; c++)
		{
			if (pworkvertex1->x < pnX->screen->referencewidth && (pworkvertex1+1)->x < pnX->screen->referencewidth)
			{
				memcpy(pworkvertex0++, pworkvertex1, sizeof(nX_vertex));
				pnX->pfacelist->nedge++;	

			}
			else if (pworkvertex1->x >= pnX->screen->referencewidth && (pworkvertex1+1)->x < pnX->screen->referencewidth)
			{
				coef = float ( 1/ (pworkvertex1->x-(pworkvertex1+1)->x)*(pnX->screen->referencewidth-(pworkvertex1+1)->x) );

				pworkvertex0->x=float (pnX->screen->referencewidth);
				pworkvertex0->y=clipvalue((pworkvertex1+1)->y, pworkvertex1->y, coef);
				pworkvertex0->z=zclipvalue((pworkvertex1+1)->z, pworkvertex1->z, coef);
				pworkvertex0++;
				pnX->pfacelist->nedge++;	
			}
			else if (pworkvertex1->x < pnX->screen->referencewidth && (pworkvertex1+1)->x >= pnX->screen->referencewidth)
			{
				coef = float ( 1/ (pworkvertex1->x-(pworkvertex1+1)->x)*(pnX->screen->referencewidth-(pworkvertex1+1)->x) );

				memcpy(pworkvertex0++, pworkvertex1, sizeof(nX_vertex));
				pworkvertex0->x = float (pnX->screen->referencewidth);
				pworkvertex0->y = clipvalue((pworkvertex1+1)->y, pworkvertex1->y, coef);
				pworkvertex0->z = zclipvalue((pworkvertex1+1)->z, pworkvertex1->z, coef);
				pworkvertex0++;
				pnX->pfacelist->nedge += 2;	
			}
			pworkvertex1++;
		}

		if (pnX->pfacelist->nedge < 3)
			return false;

		// THE FACE WENT TROUGHT ALL CLIPPINGS
		//////////////////////////////////////

		pworkvertex0 = workvertex0;

		for (c=0; c<pnX->pfacelist->nedge; c++)
		{
			memcpy(pnX->pvertexlist, pworkvertex0++, sizeof (nX_vertex));
			pnX->pvertexlist->x *= pnX->screen->wcoef;
			pnX->pvertexlist->y *= pnX->screen->hcoef;
			pnX->pvertexlist++;
		}

		pnX->pfacelist++;

		#ifdef HADDOCK
			nXTW_Clip += HADDOCK_Finish();
		#endif

		return true;
}


/*
	PrecaValidFace(long, float):

	This function add face to the preca sorting list
	and keep preca face counters up to date...

	Should be called any time a ClipFace() call
	returned true or prepare for a crash!
	You can call it manually after having added
	a face manually to pfacelist (it MUST be
	clipped or must not need one! no further
	clipping will be done beyond this call)

*/


void	nX_Scene::PrecaValidFace(float z)
{
	pnX->pbuffer2sort[0].z = long(z);
	pnX->pbuffer2sort[0].faceindex = pnX->buildedface++;		// keep track of face index
	pnX->pbuffer2sort[0].scene = this;
	pnX->pbuffer2sort++;
}


/*

	nX_PrecaBuildFace

*/


void	nX_Scene::PrecaBuildFace(nX_Entity *entity, float *c2d)
{

	long			c, c2;
	nX_Surface		*surf;

	// reset indexes

	epow = 0;	fpow = 0;

	// infos to build polygons...

	long		nedge;
	nX_reliure	*preliure = entity->reliure_adress;
	ulong		*pindex;
	nX_vertex	*pworklocalvertex;
	float		farthest;

	// BUILD AND CLIP ALL ENTITY'S FACE
	///////////////////////////////////

	for (c = 0; c < entity->nbface; c++)
	{

		nedge = preliure->nedge;
		surf = preliure->surf;
		pindex = preliure->index;

		// point/line are always seen by MOA
/*
		if (nedge < 3)
		{
			pFaceSurvey[precafacecounter] = true;
			goto	skipface;
		}
*/
		// backcull face

		if (
			c2d[pindex[2]] * (c2d[pindex[0] + 2] * c2d[pindex[1] + 1] - c2d[pindex[0] + 1] * c2d[pindex[1] + 2]) +
			c2d[pindex[2] + 1] * (c2d[pindex[0]] * c2d[pindex[1] + 2] - c2d[pindex[0] + 2] * c2d[pindex[1]]) +
			c2d[pindex[2] + 2] * (c2d[pindex[0] + 1] * c2d[pindex[1]] - c2d[pindex[0]] * c2d[pindex[1] + 1])
			>= 0
		   )
			goto	skipface;

		// get face, fill its header

		pnX->pfacelist->nedge = nedge;
		pnX->pfacelist->surf = surf;
		if (entity->Dissolve)
				pnX->pfacelist->alpha = float (surf->transparency + (1 - surf->transparency) * entity->Dissolve->values[0]);
		else	pnX->pfacelist->alpha = surf->transparency;
		if (pnX->pfacelist->alpha >= 1)
			goto	skipface;
		pnX->pfacelist->id = precafacecounter;
		if (pnX->pfacelist->alpha < 0)
			pnX->pfacelist->alpha = 0;

		// build it without clipping if needed

		farthest = 0;
		pnX->pfacelist->vertice = pnX->pvertexlist;			// get next free vertex
		pworklocalvertex = pnX->pvertexlist;


		// EFFECTIVE BUILDING OF FACES
		//////////////////////////////


		for (c2 = 0; c2 < nedge; c2++)
		{
			pworklocalvertex->x = c2d[pindex[0]+0];
			pworklocalvertex->y = c2d[pindex[0]+1];
			pworklocalvertex->z = c2d[pindex[0]+2];

			// sorting is done via farthest Z

			if ( c2d[pindex[0]+2] * radixprec > farthest )
				farthest = float (c2d[pindex[0]+2] * radixprec);

			// onto next vertex

			pworklocalvertex++;
			pindex++;
		}

		//  FACE CLIPPING

		if (PrecaClipFace())
			PrecaValidFace(farthest);

		// KEEP TRACK AND INDEXES UP-TO-DATE
		////////////////////////////////////

skipface:;
		preliure++;
		precafacecounter++;
	}

}


/*

	nX_ComputePreca

*/


void	nX_Scene::ComputePreca(char *lwsf)
{

	nX_Entity	*entity, *entity2;
	bool		stop = false;
	long		pkc, pkc2;
	long		c;
	bool		vtxdbbuffer;

	// reserve buffers to compute preca

	pnX->Verbose("Updating MOA... please wait!");
	pnX->Verbose(" ");
	pPrecaScreen = new long[pnX->screen->referencewidth * pnX->screen->referenceheight];
	pPrecaZBuffer = new float[pnX->screen->referencewidth * pnX->screen->referenceheight];
	pFaceSurvey = new bool[65536];
	pTransSurvey = new bool[65536];
	unsigned short *rpPrecaBuffer = new unsigned short[500000];
	unsigned short *opPrecaBuffer, *pPrecaBuffer;
	opPrecaBuffer = pPrecaBuffer = rpPrecaBuffer;
	memset(pPrecaBuffer, 0, 500000 * 2);

	// write MOA header. (THE POINTER IS SHORT* !!!)

	strncpy( (char *)pPrecaBuffer, "MOA4", 4 );
	pPrecaBuffer += 2;
	*(long *)pPrecaBuffer = filesize;
	pPrecaBuffer += 2;
	pPrecaBuffer += 2;
	pPrecaBuffer += 2;
	*(long *)pPrecaBuffer = precaprec;
	pPrecaBuffer += 2;
	*(float *)pPrecaBuffer = firstframe;
	pPrecaBuffer += 2;

	pPrecaBuffer += 16 - 12;	// align 32 bytes header
	frame = 0;

	// do preca the whole movement
	//////////////////////////////


	while	(!stop)
	{

		// begin a new packet, reset face surveys

		for (pkc = 0; pkc < 65536; pkc++)
		{
			pFaceSurvey[pkc] = false;
			pTransSurvey[pkc] = false;
		}

		// create face survey for packet

		for (pkc2 = 0; pkc2 < (PRECAREFRESHRATE / precaprec); pkc2++)
		{

			// keep track of discarded faces

			precafacecounter = 0;
	
			// buffer de tri des faces
			
			pnX->pbuffer2sort = pnX->buffer2sort;
			pnX->lastfaceoffset = 0;
			pnX->buildedface = 0;

			// catch end of a motion else go trough movement
			// this also ensure that while playback we'll catch the ending -1

			if (!stop)
				frame += precaprec;
			if (frame >= lastframe)
			{
				stop = true;
				frame = lastframe;
			}
	
			// PRECA UPDATE FRAME
			/////////////////////

			// envellope du focal

			Focal->GetValues(frame);
			if (Focal->useenvelop)
			{
				LWfocal = Focal->values[0];
				Focal->values[0] = 1. / LWfocal;
				viewcx =  ( float(viewportX) / 2. * (float(viewportY) / viewportX) );
				viewcy = viewportY / 2.;
			}

			// ENTITIES MOTION
			//////////////////

			for (c = 0; c <= nbentity; c++)
			{
				entity = entityadress[c];

				// get motion for current frame
				entity->LWmotion.GetValues(frame);

				// Entity OFFSETS
				entity->offset[0] = entity->LWmotion.values[0];
				entity->offset[1] = entity->LWmotion.values[1];
				entity->offset[2] = entity->LWmotion.values[2];

				// Entity ANGLES
				entity->angle[0] = -entity->LWmotion.values[4] * (digitPI / 180);
				entity->angle[1] =  entity->LWmotion.values[3] * (digitPI / 180);
				entity->angle[2] = -entity->LWmotion.values[5] * (digitPI / 180);

				// Entity SCALES
				entity->scale[0] = entity->LWmotion.values[6];
				entity->scale[1] = entity->LWmotion.values[7];
				entity->scale[2] = entity->LWmotion.values[8];

				// Entity DISSOLVE
				if (entity->Dissolve && entity->Dissolve->useenvelop)
				{
					entity->Dissolve->GetValues(frame);
					if (entity->Dissolve->values[0] < 0)
						entity->Dissolve->values[0] = 0;
					else if (entity->Dissolve->values[0] > 1)
						entity->Dissolve->values[0] = 1;
				}

				// Entity MORPH
				if (entity->Morph && entity->Morph->useenvelop)
					entity->Morph->GetValues(frame);

/*				// Entity GIZMO
				if (entity->gizmoadress)
				{
					gizmo = entity->gizmoadress;

					// gizmo does not support end behavior so stop the motion

					envframe = GetBehaviorFrame(1, gizmo->lastframe, gizmo->offset);
					gMT.keylist = gizmo->KF;
					gMT.keys = gizmo->nbKF;
					gMT.steps = int(gizmo->lastframe);
					GizmoCalcStep(&gMT, Hermite, envframe, gizmo->nbtarget);

					for (c2 = 0; c2 < gizmo->nbtarget; c2++)
						gizmo->morphamount[c2] = float(Hermite[c2]);
				}
*/
				// Entity POLYGON SIZE
				if (entity->PolySize->useenvelop)
					entity->PolySize->GetValues(frame);

			}

			// CAMERA TARGET

			float	head, pitch;
			float	dx, dy, dz, hyp;
			entity = entityadress[0];

			if (entity->dtarget)
			{
				entity2 = entity->dtarget;

				// targeting vector
				dx = entity2->offset[0] - entity->offset[0];
				dy = entity2->offset[1] - entity->offset[1];
				dz = entity2->offset[2] - entity->offset[2];

				// heading (align heading using two-axis)
				hyp = sqrt( dx * dx + dz * dz );
				head = acos( dz / hyp );
				if (dx < 0)	head = -head;

				// pitch (align pitch using full space)
				hyp = sqrt( dx * dx + dy * dy + dz * dz );
				pitch = asin( dy / hyp );

				// okydoky
				entity->angle[0] = pitch;
				entity->angle[1] = head;
			}

			// COMPUTE WORLD MATRIX

			GetMatrix();

			// COMPUTE ACTORS MATRIX

			for (c = 0; c <= nbentity; c++)
				entityadress[c]->GetMatrix();

			// concatenate with its chain's members & world matrix
			for (c = 0; c <= nbentity; c++)
			{
				entityadress[c]->GetHMatrix();
				pnX->MatMul44(entityadress[c]->hmatrix, matrix, entityadress[c]->fmatrix);
			}

			// OBJECT FACE CREATION
			///////////////////////


			for (c = 1; c <= nbobj; c++)
			{
				entity = entityadress[c];

				// handle nX custom entities
				switch (entity->type)
				{
					// standard object or null object

					case nXETYPE_OBJECT:

						// SKIP DISSOLVED OBJECTS
						if (entity->Dissolve && entity->Dissolve->values[0] == 1)
						{
							precafacecounter += entity->nbface;
							break;
						}


						// HANDLE EFFECTORS -------------------------------------------

						if (entity->wave)
							entity->ApplyWave();
						else if (entity->usemdd)
							entity->ApplyMDD();

						// DOUBLE BUFFER VERTICE/NORMALS

						vtxdbbuffer = false;
						pnX->p3dpswap1 = entity->edge_adress;
						pnX->p3dpswap2 = pnX->edgesbuffer1_adress;

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
						}

						// Enhanced morph via GIZMO
/*						if (entity->gizmoadress)
						{
							Gizmo(entity->nbpoints, pnX->p3dpswap1, pnX->p3dpswap2, entity->gizmoadress, entity->gizmoadress->xyzmorphdeltas);
							vtxdbbuffer = SwapVtxBuf(pnX, vtxdbbuffer);
						}
*/
						// ALL OBJECTS DEFAULT HERE! ----------------------------------
						///////////////////////////////////////////////////////////////
	
						if (entity->usemdd)
								TransformVertice(entity->nbpoints, pnX->p3dpswap1, pnX->p3dpswap2, matrix);
						else	TransformVertice(entity->nbpoints, pnX->p3dpswap1, pnX->p3dpswap2, entity->fmatrix);

						entity->TestBBVisibility();

//						if (entity->visibility)
							if (entity->PolySize->values[0] == 1.)
								PrecaBuildFace(entity, pnX->p3dpswap2);		// create polys

					break;

					// END OF ETYPE SWITCH
				}
			}

			//	build virtual scene to check unseen poly/object(s)

			if (pnX->buildedface > 0)
			{
				pnX->RadixSortFaceList();
				pnX->pbuffersorted = pnX->pbuffer2sort + (pnX->buildedface - 1);

				// cleanup precascreen & zBuffer
				for (pkc = 0; pkc < long(pnX->screen->referencewidth * pnX->screen->referenceheight); pkc++)
				{
					pPrecaZBuffer[pkc] = 0;
					pPrecaScreen[pkc] = -1;
				}

				// process each polygon down to screen
				pnX->FPUsetup();
				for (pkc = 0; pkc < pnX->buildedface; pkc++)
				{
					pnX->pfacelist = pnX->facelist + pnX->pbuffersorted[0].faceindex;
					pnX->pbuffersorted--;

					pnX->nPrecaPol(pnX->pfacelist, pPrecaScreen, pPrecaZBuffer, pTransSurvey);
				}
				pnX->FPUrestore();

				// face survey

				for (pkc = 0; pkc < long(pnX->screen->referencewidth * pnX->screen->referenceheight); pkc++)
					if (pPrecaScreen[pkc] != -1)
						pFaceSurvey[pPrecaScreen[pkc]] = true;

			}

			pnX->ResetBuffers();

			// END OF FRAME SURVEY
			//////////////////////
		}

		// build survey for one packet

		precafacecounter = 0;

		opPrecaBuffer = pPrecaBuffer;
		pPrecaBuffer++;

		for (pkc = 0; pkc < 65536; pkc++)
			if ( pFaceSurvey[pkc] || pTransSurvey[pkc] )
			{
				*pPrecaBuffer++ = pkc;
				precafacecounter++;
			}

		opPrecaBuffer[0] = precafacecounter;
		sopreca = (pPrecaBuffer - rpPrecaBuffer) * 2;
		pnX->console->SetProgress( char(frame * 100. / lastframe) );

	}

	// end preca file
	
	pPrecaBuffer[0] = 65535;

	// save it!

	FILE *file;
	file = fopen(lwsf, "wb");

	if (file == NULL)
	{
		pnX->Verbose("*** Could not open MOA file! ***");
		pnX->Verbose("Check for an already existing file with read-only flag.");
		pnX->Verbose("    MOA disabled...");
		usepreca = false;
	}
	else
	{
		fwrite(rpPrecaBuffer, sopreca + 2, 1, file);
		fclose(file);
	}

	// free temporary buffers once the PRK has been computed

	delete pPrecaScreen;
	delete pPrecaZBuffer;
	delete pFaceSurvey;
	delete pTransSurvey;
	delete rpPrecaBuffer;
	pnX->ResetBuffers();

}