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
//		nX Core implementation file
//
//	------------------------------------------------------------------------------


	#include	"nXng/nX_HeaderList.h"
	#include	"IO/cqString.h"

	static		qString	cStr;
	static		char str[256];
	extern		unsigned char	mulshader[65536];


//----------------------
nX::nX(ScreenMN *target)
//----------------------
{

	memset(this, 0, sizeof nX);

	// Set default nX object behavior
	ComputeRGBshades();
	SurfDefaultShading = nX_NULL;
	Synch = true;

	SetDrawForward();

	screen = target;
	ResetAlloc();

	// Init qSin, qCos

	for (long c = 0; c < sincosREV; c++)
	{
		qSin[c] = sin( float(2.0f * digitPI / sincosREV) * c);
		qCos[c] = cos( float(2.0f * digitPI / sincosREV) * c);
	}

}


//-------
nX::~nX()
//-------
{

	FlushTexMap();
	_2D_Free();

}


//--------------------------
void	nX::SetDrawForward()
//--------------------------
{

	Draw_Forward = true;
	nX_TRANSPARENTRADIXCONST = (1 << 20);
	nX_OPAQUERADIXCONST = (0);

}


//---------------------------
void	nX::SetDrawBackward()
//---------------------------
{

	Draw_Forward = false;
	nX_TRANSPARENTRADIXCONST = (0);
	nX_OPAQUERADIXCONST = (1 << 30);

}


//------------------------
void	nX::UpdateTexMap()
//------------------------
{
	nX_LLTexMap	*ptLL = LLtexmap;
	while (ptLL)
	{
		ptLL->nodemap->Update();
		ptLL = ptLL->next;
	}
}


//-----------------------------------
void nX::SetConsole(nX_Console *eCon)
//-----------------------------------
{
	console = eCon;
}


//-------------------------
void nX::Verbose(char *msg)
//-------------------------
{
	if (console)
		console->Print(msg);
}


//-----------------------------
void nX::VerboseOver(char *msg)
//-----------------------------
{
	if (console)
		console->PrintOver(msg);
}


//--------------------------
void nX::SetProgress(char p)
//--------------------------
{
	if (console)
		console->SetProgress(p);
}


//--------------------------------
void nX::RequireAllNormals(bool b)
//--------------------------------
{
	ForceAllNormals = b;
}


//--------------------------------------------------
void	nX::MatMul44(float *a, float *b, float *dst)
//--------------------------------------------------
{
	long	i, j;

	for (j = 0; j < 4; j++)
		for (i = 0; i < 4; i++)
			dst[i + j * 4] = a[j * 4] * b[i] +
							 a[1 + j * 4] * b[4 + i] +
							 a[2 + j * 4] * b[8 + i] +
							 a[3 + j * 4] * b[12 + i];

}


//--------------------------------------------------
void	nX::MatMul33(float *a, float *b, float *dst)
//--------------------------------------------------
{
	long	i, j;

	for (j = 0; j < 3; j++)
		for (i = 0; i < 3; i++)
			dst[i + j * 3] = a[0 + j * 3] * b[i] +
							 a[1 + j * 3] * b[3 + i] +
							 a[2 + j * 3] * b[6 + i];
}


//-----------------------------
void	nX::RadixSortFaceList()
//		4 pass for 2^32 entries
//-----------------------------
{

	unsigned long	c, index, rpass;
	ulong			value;
	ulong			radix[257];
	RadixNode		*tmp;

	pbuffer2sort = buffer2sort;
	pbuffersorted = buffersorted;

	// 4 pass: sort 32 bit unsigned 
	for (rpass = 0; rpass < 4; rpass++)
	{

		// clear radix buffer
		for (c = 0; c < 256; c++)
			radix[c] = 0;

		// setup rb
		for (c = 0; c < buildedface; c++)
		{
			value = ulong(pbuffer2sort[c].z & 255);
			radix[value + 1]++;
		}	

		// convert hit count to index
		for (c = 0; c < 256; c++)
			radix[c + 1] += radix[c];

		//
		for (c = 0; c < buildedface; c++)
		{
			value = ulong (pbuffer2sort[c].z & 255);
			index = radix[value]++;
			pbuffersorted[index].z = long (pbuffer2sort[c].z >> 8);
			pbuffersorted[index].faceindex = pbuffer2sort[c].faceindex;
		}

		tmp = pbuffersorted;
		pbuffersorted = pbuffer2sort;
		pbuffer2sort = tmp;

	}

}


//-----------------------
void	nX::FlushTexMap()
//-----------------------
{
	nX_LLTexMap	*pMap = LLtexmap, *nMap;

	// the UnregisterTexMap will set LLtexmap to NULL if it is last item
	// the TexMap must be destroyed, so we override Release()
	// pMap was destroyed by the call to UnregisterTexMap()
	while (LLtexmap)
	{
		nMap = pMap->next;
		if (pMap->nodemap)
			delete pMap->nodemap;
		pMap = nMap;
	}

	LLtexmap = NULL;
}


//--------------------
void	nX::FPUsetup()
//--------------------
{
	unsigned short	cw, uw;
	_asm
	{
		push	eax
		fstcw	cw
		mov		ax,cw
		and		ax,1111000011000000b	// float-24
		or		ax,0000100000111111b	// roundup nearest and no exceptions
		mov		uw,ax
		fldcw	uw
		pop		eax
	}
	FPUcw = cw;
}


//----------------------
void	nX::FPUforceOK()
//----------------------
{
	unsigned short	cw;
	_asm
	{
		fstcw	cw
		mov		ax,cw
		and		ax,1111000011000000b	// float-24
		or		ax,0000100000111111b	// roundup nearest and no exceptions
		mov		cw,ax
		fldcw	cw
	}
}


//----------------------
void	nX::FPUrestore()
//----------------------
{
	if (!FPUcw)	return;
	unsigned short cw = (unsigned short)FPUcw;
	_asm
	{
		fldcw	cw
	}
}


//--------------------------
void	nX::SetSynch(bool v)
//--------------------------
{
	Synch = v;
}


//---------------------------------------------
void	nX::SetDefaultShading(unsigned long sD)
//		Set default shading for all surface
//---------------------------------------------
{
	SurfDefaultShading = sD;
}


//--------------------------------
void	nX::SetAntialias(bool val)
//--------------------------------
{
	antialias = val;
}


//-----------------------------
void	nX::SetLWpath(char *LW)
//-----------------------------
{
	LWpath = LW;
}


//----------------
void nX::ResetFX()
//----------------
{

	if (childusingzbuffer)
		childusingzbuffer = false;
	if (childusingfog)
	{
		screen->pD3DDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, false);
		childusingfog = false;
	}

}


//-------------------
void	nX::InitFXs()
//-------------------
{

	char	local[256];

	// setup blank surface
	BlankSurface = new nX_Surface(this);
	BlankSurface->QuickSetup(NULL, nX_FLAT, NULL);
	BlankSurface->color.r = 127; BlankSurface->color.g = 127; BlankSurface->color.b = 127;

	ReportSurf = new nX_Surface(this);
	ReportSurf->QuickSetup(NULL, nX_FLAT, NULL);

	// load default LensFlare
	strcpy(local, "d:\\devellop\\nx_pics\\flare.jpg");
	DefaultFlareSurface = new nX_Surface(this);
	DefaultFlareSurface->QuickSetup(local, nX_ADDITIVE + nX_MAPPED + nX_LIGHTSOURCE, nX_NOZBUFFER);

	// load default sparks
	strcpy(local, "d:\\devellop\\nx_pics\\spark.jpg");
	DefaultSparkSurface = new nX_Surface(this);
	DefaultSparkSurface->QuickSetup(local, nX_ADDITIVE + nX_MAPPED, 0);

	// load unshade flare
	strcpy(local, "d:\\devellop\\nx_pics\\flare.jpg");
	DefaultPolySizeSurface = new nX_Surface(this);
	DefaultPolySizeSurface->QuickSetup(local, nX_ADDITIVE + nX_MAPPED, 0);

	// default BLOOM surface
	strcpy(local, "d:\\devellop\\nx_pics\\flare.jpg");
	DefaultBLOOMSurface = new nX_Surface(this);
	DefaultBLOOMSurface->QuickSetup(local, nX_ADDITIVE + nX_MAPPED + nX_LIGHTSOURCE, nX_NOZWRITE + nX_NOZBUFFER);

	// load default evmap
	strcpy(local, "d:\\devellop\\nx_pics\\defaultevmap.jpg");
	DefaultEVMAP = new nX_TexMap(this);
	DefaultEVMAP->Load(local, NULL);
	RegisterTexMap(DefaultEVMAP);
	DefaultEVMAP->AddRef();

}


//----------------------
void	nX::GetFXsiD3D()
//----------------------
{

	DefaultFlareSurface->map[0]->GetiD3D();
	DefaultBLOOMSurface->map[0]->GetiD3D();
	DefaultSparkSurface->map[0]->GetiD3D();
	DefaultPolySizeSurface->map[0]->GetiD3D();
	DefaultEVMAP->GetiD3D();

}


//-------------------
void	nX::FreeFXs()
//-------------------
{

	if (DefaultFlareSurface)
	{
		delete(DefaultFlareSurface);
		DefaultFlareSurface = NULL;
	}
	if (DefaultBLOOMSurface)
	{
		delete(DefaultBLOOMSurface);
		DefaultBLOOMSurface = NULL;
	}
	if (DefaultSparkSurface)
	{
		delete(DefaultSparkSurface);
		DefaultSparkSurface = NULL;
	}
	if (DefaultPolySizeSurface)
	{
		delete(DefaultPolySizeSurface);
		DefaultPolySizeSurface = NULL;
	}
	if (BlankSurface)
	{
		delete(BlankSurface);
		BlankSurface = NULL;
	}
	if (ReportSurf)
	{
		delete(ReportSurf);
		ReportSurf = NULL;
	}

}


//------------------------
void	nX::ResetBuffers()
//------------------------
{
	pbuffer2sort = buffer2sort;
	pbuffersorted = buffersorted;
	lastfaceoffset = 0;
	statbuildedface = buildedface;
	buildedface = 0;
	pfacelist = facelist;
	pvertexlist = vertexlist;
}


//----------------------
void	nX::ResetAlloc()
//----------------------
{
	memset(&ComputeAlloc, 0, sizeof(nX_AllocInfo));
}


//------------------------------------------------------------
void	nX::ComputeSceneMemReq(nX_Scene *lscene, bool multi)
//		Call to compute memory requirement for a given scene
//		specify wether the scene is to be replayed among other
//		or alone.
//------------------------------------------------------------
{
	// if the scene is to be replayed with other scenes running meanwhile
	if (multi)
	{
		ComputeAlloc.totalface += lscene->totalface;
		ComputeAlloc.nedgemaxobj += lscene->nedgemaxobj;
		ComputeAlloc.nglobedgemaxobj += lscene->nglobedgemaxobj;
		ComputeAlloc.ngvtxface += lscene->ngvtxface;
	}
	else
	{
		if (lscene->totalface > ComputeAlloc.totalface)
			ComputeAlloc.totalface = lscene->totalface;
		if (lscene->nedgemaxobj > ComputeAlloc.nedgemaxobj)
			ComputeAlloc.nedgemaxobj = lscene->nedgemaxobj;
		if (lscene->nglobedgemaxobj > ComputeAlloc.nglobedgemaxobj)
			ComputeAlloc.nglobedgemaxobj = lscene->nglobedgemaxobj;
		if (lscene->ngvtxface > ComputeAlloc.ngvtxface)
			ComputeAlloc.ngvtxface = lscene->ngvtxface;
	}
}


//--------------------------
void	nX::DeallocBuffers()
//--------------------------
{
	if (edgesbuffer1_adress) delete(edgesbuffer1_adress);
	edgesbuffer1_adress = NULL;
	if (edgesbuffer2_adress) delete(edgesbuffer2_adress);
	edgesbuffer2_adress = NULL;
	if (facelist) delete(facelist);
	facelist = NULL;
	if (vertexlist) delete(vertexlist);
	vertexlist = NULL;
	if (facenormalsA_adress) delete(facenormalsA_adress);
	facenormalsA_adress = NULL;
	if (facenormalsB_adress) delete(facenormalsB_adress);
	facenormalsB_adress = NULL;
	if (edgenormalsA_adress) delete(edgenormalsA_adress);
	edgenormalsA_adress = NULL;
	if (edgenormalsB_adress) delete(edgenormalsB_adress);
	edgenormalsB_adress = NULL;
	if (buffer2sort) delete(buffer2sort);
	buffer2sort = NULL;
	if (buffersorted) delete(buffersorted);
	buffersorted = NULL;
}


//------------------------------------
void	nX::AllocBuffers(ulong dalloc)
//------------------------------------
{
	if (!dalloc)	dalloc++;
	nX_AllocInfo	*info = &ComputeAlloc;

	// prevent badly written code to loose track of allocations...
	DeallocBuffers();

	edgesbuffer1_adress = new float[info->nedgemaxobj * 3 + nX_NBFXFACE * 4];
	edgesbuffer2_adress = new float[info->nedgemaxobj * 3 + nX_NBFXFACE * 4];
	memset(edgesbuffer1_adress, 0, (info->nedgemaxobj * 3 + nX_NBFXFACE * 4) * sizeof(float));
	memset(edgesbuffer2_adress, 0, (info->nedgemaxobj * 3 + nX_NBFXFACE * 4) * sizeof(float));

	// allocate facelist
	ulong	nbface2alloc = info->totalface / dalloc + nX_NBFXFACE;
	facelist = new nX_face[nbface2alloc];
	memset(facelist, 0, nbface2alloc * sizeof nX_face);

	// allocate vertexlist
	ulong	nbvtx2alloc = info->ngvtxface / dalloc + nX_NBFXFACE * 16;
	vertexlist = new nX_vertex[nbvtx2alloc];
	memset(vertexlist, 0, nbvtx2alloc * sizeof nX_vertex);

	// allocate buffer for rotating normals (they need fullblown!)
	facenormalsA_adress = new float[(info->totalface + nX_NBFXFACE) * 3];
	memset(facenormalsA_adress, 0, (info->totalface + nX_NBFXFACE) * 3 * sizeof(float));
	facenormalsB_adress = new float[(info->totalface + nX_NBFXFACE) * 3];
	memset(facenormalsB_adress, 0, (info->totalface + nX_NBFXFACE) * 3 * sizeof(float));

	// allocate buffer for rotating edges (they need fullblown!)
	edgenormalsA_adress = new float[info->nglobedgemaxobj * 3 + nX_NBFXFACE * 4];
	edgenormalsB_adress = new float[info->nglobedgemaxobj * 3 + nX_NBFXFACE * 4];
	memset(edgenormalsA_adress, 0, (info->nglobedgemaxobj * 3 + nX_NBFXFACE * 4) * sizeof(float));
	memset(edgenormalsB_adress, 0, (info->nglobedgemaxobj * 3 + nX_NBFXFACE * 4) * sizeof(float));

	// allocate buffer to sort facelist
	buffer2sort = new RadixNode[nbface2alloc];
	memset(buffer2sort, 0, nbface2alloc * sizeof(RadixNode));
	buffersorted = new RadixNode[nbface2alloc];
	memset(buffersorted, 0, nbface2alloc * sizeof(RadixNode));

	ResetBuffers();

}


//------------------------------
void	nX::SetcInput(cInput *c)
//------------------------------
{
	cIn = c;
}


//------------------------------
void	nX::DisplayFrameReport()
//------------------------------
{

	screen->PrintInit(8);

	screen->qPrint("<F1> - SingleTexture Raster", 14);
	screen->qPrint("<F2> - FreeRide Mode", 14);
	screen->qPrint("<F3> - Show BoundingBox Mode", 14);
	screen->qPrint("<F4> - Bloom ON/OFF", 14);
	screen->qPrint("<+-> - FreeRide Speed Control", 14);
	wsprintf(str, "nXng: F=%d; FPS=%.3d; #POLY=%.5d; #SMOA=%d; #SBBX=%d", (unsigned long)(cscene->frame), FPS, (unsigned long)(buildedface), cscene->sMOAobj, cscene->sBBXobj);
	screen->qPrint(str, 14);

	// display stats for all entities
	nX_Entity	*entity;

	if (screen->bBoundingBox)
	{
		for (long c = 1; c <= cscene->nbobj; c++)
		{
			entity = cscene->entityadress[c];

			if (entity->BBv)
			{
				wsprintf(str, "'%s'", entity->file);
				screen->Print(str, entity->BBmnx + 2, entity->BBmny + 2);
				wsprintf(str, "BBv = %d; Freeze = %d", entity->BBv, entity->freezed);
				screen->Print(str, entity->BBmnx + 2, entity->BBmny + 10);
			}
		}
	}
	screen->PrintFree();

}


//----------------------------
void	nX::ComputeRGBshades()
//----------------------------
{

	Verbose("Initializing piXel!");
	uchar	*pshade = &mulshader[0];
	float	pp = 0;

	for (long c = 0; c < 256; c++)			// alpha
		for (long c2= 0; c2 < 256; c2++)	// color
		{
			*pshade++ = uchar ( float(c * c2) / 255.0 );
			SetProgress( char (pp / 65536 * 100.0) );
			pp += 1.0;
		}

}
