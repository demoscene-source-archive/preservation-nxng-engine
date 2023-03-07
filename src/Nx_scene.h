	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		SCENE DEFINITION v9
		-------------------

		nX Scene is a set of Objects and Lights with their motion.
		nX Scenes are optimized within the same nX instance

	   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  */


		#define	nX_FRACTALBUMPMAPPATH			"d:\\devellop\\nx_pics\\fractalnoise.jpg"
		#define	nX_CRUMPLEMAPPATH				"d:\\devellop\\nx_pics\\crumple.jpg"

		#define nX_NBFXFACE						4096

		#define nX_DefaultMaxSmoothingAngle		180.

		// memory reservation variables
		#define	nX_MaxSurfacePerEntity			4096
		#define nX_Maxentityinscene				4096

		static float	sparkUset[16]={0, 0.5, 0.5, 0, 0.5, 1, 1, 0.5, 0, 0.5, 0.5, 0, 0.5, 1, 1, 0.5};
		static float	sparkVset[16]={0, 0, 0.5, 0.5, 0, 0, 0.5, 0.5, 0.5, 0.5, 1, 1, 0.5, 0.5, 1, 1};


struct LoadLWOConfig
{
	bool	surface;
	bool	forcefacenorm;
	bool	forceedgenorm;
};


class nX_Scene
{

	public:

	// LOADING STUFF ----------------------------------------------------------
	///////////////////////////////////////////////////////////////////////////

	char			path[256];				// path to the scene file
	char			fullfile[256];			// full scene filename
	char			*lwsadress;				// adress of the LWS in memory
	char			*pLWS, *pLWO;			// loading pointers (one for LWS, one for LWOs)

	// META OBJECTS -----------------------------------------------------------
	///////////////////////////////////////////////////////////////////////////

	bool			hasmetaobjects;			// do the have metaobject to render?
	nX_metagrid		*metagrid;				// the metagrid we use to perform tesselation

	// FXs --------------------------------------------------------------------
	///////////////////////////////////////////////////////////////////////////

	bool			glow;					// do the scene have GLOW pass?
	float			iglow;					// scene GLOW' intensity
	float			glowradius;				// scene GLOW' radius (in meter)

	bool			fog;
	unsigned char	fogtype;	
	unsigned long	fogcolorRGB;
	unsigned short	fogcolorR, fogcolorG, fogcolorB;
	nX_Envelop		*FogMinDist;
	nX_Envelop		*FogMaxDist;

	nXcolor			backdropcolor;

	bool			BGmap;					// do the scene have Background Image?
	nX_Surface		*BGsurf;				// the Background Image surface...

	nX_Envelop		*mblurlen;				// the motion blur strength (NULL if no motion blur)

	void			FXtonX();				// transmit scene's FX to nX renderstates (like fog)

	bool			_2D_Background;			// 2D effect in background? (:

	// STATE & STATS ----------------------------------------------------------
	///////////////////////////////////////////////////////////////////////////

	bool			loaded;					// is the scene loaded?
	unsigned long	filesize;				// the filesize of .LWS
	unsigned long	loadtime;				// load time (in milisecond)
	float			mulfactor;				// scene multiplicator
	float			radixprec;				// precision of radix sort
	float			iradixprec;				// 1 / precision of radix sort

	unsigned short	sMOAobj;				// # objects skipped via MOA
	unsigned short	sBBXobj;				// # objects skipped via BoundingBoxes

	// MOA section
	bool			usepreca;				// does it have MOA?
	unsigned char	precaprec;				// precision of MOA
	unsigned long	sopreca;				// size of preca
	unsigned short	*precaadress, *pprecabuffer, *opprecabuffer;		// the MOA buffers
	unsigned long	precafacecounter;		// global face counter (counting all faces even rejected one)
	unsigned long	facewastodraw;			// number of faces that were to draw in that packet
	float			precaframecounter;		// frame counter for MOA (%precaprec, used to know when to get next MOA packet)
	unsigned short	packetnbface;			// number of face in that packet (as MOA told us)

	// buffers
	unsigned long	totalface;				// total # of faces
	unsigned long	totaltri;				// total # of independent triangles
	unsigned long	totaledge;				// # of vertice in the scene
	unsigned long	ngvtxface;				// total # of vertices with no sharing along faces
	unsigned long	nglobedgemaxobj;		// total # of vertices with no sharing along faces in the object that has more vertice than all others in the scene
	unsigned short	nedgemaxobj;			// # of vertice in the object that has more vertice than all others in the scene

	// replay stats
	bool			looped;					// true if scene was looped
	unsigned short	loopcount;				// # of times the scene was looped
	unsigned short	nbentity;				// # of entities in scene
	unsigned short	nbobj;					// # of .LWO in scene
	unsigned short	nbbone;					// global number of bones in scene
	unsigned short	nblight;				// # of lights in scene
	unsigned short	nbtexmap;				// # of TexMap in the scene
	unsigned short	nbsequence;				// # of sequence in scene
	unsigned short	nbsurface;				// # of surfaces in scene
	unsigned long	nbKFstruct;				// # of Keyframes in scene
	unsigned short	nbstatic;				// # of perfectly static entities in scene
	unsigned short	concave;				// # of detected concave polys at load time (needs special build)

	// FRAMING STUFF ----------------------------------------------------------
	///////////////////////////////////////////////////////////////////////////

	float			frame;					// current frame
	float			lastframe;				// last frame
	float			firstframe;				// first frame
	unsigned char	fps;					// frame per second
	unsigned long	startingtick;			// tick count when the scene was reseted
	unsigned long	previoustick;			// tick count when last UpdateFrame() call occured
	float			elapsedframecount;		// elapsed frame [FRACTIONAL] between two calls to UpdateFrame()
	float			elapsedtickcount;		// elapsed ticks [INTEGER] between two calls to UpdateFrame()

	// VIEWPORT STUFF ---------------------------------------------------------
	///////////////////////////////////////////////////////////////////////////

	nX_Envelop		*Focal;					// Envelop controled focal
	float			LWfocal;				// LW as-it-is focal
	float			offocal;				// Focal offset

	unsigned long	renderResW, renderResH;	// effective rendering resolution
	unsigned short	viewportX, viewportY;	// Width and Height of viewport
	unsigned short	viewportMidX,
					viewportMidY;			// Center coordinates of viewport
	unsigned short	viewportUpClip,
					viewportLeftClip,
					viewportDownClip,
					viewportRightClip;		// Viewport clipping box
	nX_Entity		*camEntity;				// adress of camera
	float			viewcx, viewcy,	zcut;	// viewport ratio between reference resolution and effective one

	void			SetRenderRes(unsigned long , unsigned long);
					// Change the clipping region for the scene and conform focal to it
	void			SetViewport(unsigned long, unsigned long, unsigned long, unsigned long);
					// Change the clipping region for the scene
	void			SetClipping(unsigned long, unsigned long, unsigned long, unsigned long);
	void			Resize(float);
	dXYZ			minBound, maxBound;

	// SCENE LIGHTNING --------------------------------------------------------
	///////////////////////////////////////////////////////////////////////////

	nX_Envelop		*AmbientInt;
	nXcolor			ambientcolor, iambientcolor;

	float			dissolve;
	float			flaresize;
	float			flareangle;

	unsigned short	firstlightno;
	float			matrix[16], rmatrix[9];

	// compute lightning at a given location for a given light
	float			GetLightningAtLocation(nX_Entity *, double, double, double, double, double, double, nX_Surface *, nXcolor *);
	// compute monochrome lightning at a given location for a given light
	float			GetMonoLightAtLocation(nX_Entity *, double, double, double, double, double, double, nX_Surface *, nXcolor *);

	// LIGHTMAP ---------------------------------------------------------------
	///////////////////////////////////////////////////////////////////////////

	void			GetUVLightMap();
	// does {x,y,z} sees {x,y,z} ? (against all poly in scene including alpha mapped ones)
	bool			TestVisibility(dXYZ *, cM_Vector *, nX_Entity *, cM_Plane *, nXcolor *);
	float			minLMsize, maxLMsize;	// Lightmaps delta size

	bool			bComputeLM;				// are we loading to compute lightmaps?

	// ENVIRONNEMENT ----------------------------------------------------------
	///////////////////////////////////////////////////////////////////////////

	nX				*pnX;					// the nX instance this scene is registered in
	nX_Scene		*ocscene;				// previous scene registered as current in our nX instance

	// LINKED LISTS -----------------------------------------------------------
	///////////////////////////////////////////////////////////////////////////

	// TexMap list
	bool			useslightmap;							// is there any lightmap infos for this scene?
	bool			shadowsonly;							// shadow/light maps

	nX_LLTexMap		*LLtexmap;
	void			AddMapToLLTexMap(nX_TexMap *);			// add TexMap to Scene linked list

	// Find/Load a TexMap and add a reference from the surface to it. (add LL)
	nX_TexMap		*LoadAndRegisterTexMapSurface(char *, long, nX_Surface *, xLinker *);
	// Find/Load a TexMap. (add LL)
	nX_TexMap		*LoadAndRegisterTexMap(char *, xLinker *);

	// IK list
	IK_Node			*LLIK;
	// Entity list
	nX_Entity		*entityadress[nX_Maxentityinscene];
	nX_Entity		*processOrder[nX_Maxentityinscene];
	// Surface lists
	nX_Surface		*LLsurface;
	nX_Surface		*entsurfacelkp[nX_MaxSurfacePerEntity];

	long			epow, fpow;		// ugly )):

	// PRECA THINGS -----------------------------------------------------------
	///////////////////////////////////////////////////////////////////////////

	long			*pPrecaScreen;
	float			*pPrecaZBuffer;
	bool			*pFaceSurvey;
	bool			*pTransSurvey;

					// MOA methods
	bool			PrecaClipFace();
	void			PrecaValidFace(float);
	void			PrecaBuildFace(nX_Entity *, float *);
	void			ComputePreca(char *);

	// METHODS ----------------------------------------------------------------
	///////////////////////////////////////////////////////////////////////////

	void			SetCamera(nX_Entity *);

	void			ComputeAllLWMotions();
	void			GetAllMatrix();
	void			DisplayWorld();
	void			RenderLWScene();			// RenderLWScene() superseeds ComputeAllLWMotions, TransformWorld and DisplayWorld

	void			SetCurrent();
					// Transmit scene's FXs to its parent engine instance (like fog)
	void			GetAllTexMapiD3D();
	void			FreeAllTexMapiD3D();

	void			UpdateSurfaceFx();

					// TRANSFORM ENGINE
					// ----------------

					// Transform 'long' vertice(s) from 'float *' to 'float *' using 4x4 matrix 'float *'
	void			TransformVertice(unsigned long , float *, float *, float *);
					// Transform 'long' normal(s) from 'float *' to 'float *' using 3x3 matrix 'float *'
	void			TransformNormal(unsigned long , float *, float *, float *);
					// Compute world 4x4 matrix
	void			GetMatrix();

					// GEOMETRY ENGINE
					// ---------------

					// MORPH nbp TRIPLETS FROM *sp TO *dp USING *mt COEF TABLE BY amount
	void			Morph(unsigned long, float *, float *, float *, float);
					// Worm-MORPH nbp TRIPLETS
	void			Worm(unsigned long, float *, float *);
	void			WormNorm(unsigned long, float *, float *);
					// Add an IK end effector entity to LLIK list
	void			AddIKChain(nX_Entity *);

					// Get next non-synched frame count
	void			GetNextFrame();
					// Get next synched frame count
	void			GetInterruptFrame(bool);

					// Resolve shortnames in LW5+
	void			MakeComply2Path(char *);


					// IFT3 ENGINE
					// -----------
					// Load and process an LWS intel-ascii file
	bool			LoadLWS(char *);
					// Load and add a MOA file to the current scene object
	bool			LoadMOA(char *);
					// Load an process an LWO file
	void			LoadLWO(char *, nX_Entity *, LoadLWOConfig *);

					// Add camera to the scene
	void			AddCameraFromLWS();
					// Add a light to the scene
	void			AddLightFromLWS(short);
					// Add null object to the scene
	void			AddNullObjectFromLWS(short);
					// Add object to the scene
	void			AddObjectFromLWS(short);

					// Subprocessing of an LWO file
	void			ProcessPNTS(nX_Entity *);
	void			ProcessSURF(nX_Entity *, char *, nX *);

					// Read parameters common to all entities from an LWS intel-ascii file
	bool			ReadCommonParameters(nX_Entity *);

					// call with RADIX sort factor and SCENE size value
					// the call MUST be done before any loading is intended!
	void			Setup(float , float);
					// Reset the scene states (false to override scene starting tick)
	void			Reset(bool, unsigned long);
					// Compute next frame offset (poll the scene)
	float			GetBehaviorFrame(long, float, float);
					// Add faces of an object to facelist and call TNL
	void			BuildFace(nX_Entity *, unsigned char);

	// TODO: move these to entity CLASS

	bool inline		TestObjectMotionChange(nX_Entity *);

	nX_Scene::nX_Scene(nX *);
	nX_Scene::~nX_Scene();

};