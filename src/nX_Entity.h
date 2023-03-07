	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		ENTITY DEFINITION v9
		--------------------
		Source code by Emmanuel JULIEN alias [xBaRr/Syndrome] 1998~2000.

		nX Entity can be of 3 types:
			Camera, Object or Light.

		nX Entity are optimized within the same nX instance

		nX_Entity's implements:
			- Bounding Box,
			- Sectorisation,
			- TSP,

		TODO: Heritage would do many goods here...

	   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  */


		// advanced lightning definitions
		#define		nX_RESTRICTLIGHT				1
		#define		nX_NOAMBIENT					2
		#define		nX_LIGHTCOLOR					4

		// bits definitions for objects requirements
		#define		nX_USEFACENORMAL				1
		#define		nX_USEEDGENORMAL				2
		#define		nX_USEBUMPOFFSETS				4
		#define		nX_USECUBICMAP					8

		// different entity types
		#define		nXETYPE_CAMERA					0
		#define		nXETYPE_OBJECT					1
		#define		nXETYPE_LIGHT					2
		#define		nXETYPE_SPARKEMITER				3
		#define		nXETYPE_METABALL				4



#ifdef nX_MDPlug
struct	MDStep
{

	float			*vertice;					// vertice for step
	float			*fnorm, *enorm;				// face & edge normals for step
	float			*blend;						// vertice blend factors to next step
	float			*bfnorm, *benorm;			// face & edge normals blend factors to next step
	unsigned long	timestamp;					// step to go to this one
	
};
#endif

#ifdef nX_ENABLETRAIL
#define		nX_MAXTRAILLENGTH		48

struct	nX_Trail
{
	unsigned long	nbcoords;
	nX_reliure		*prel;
	float			*trail_coord[nX_MAXTRAILLENGTH];
	nX_Trail		*next;
};
#endif

struct	nX_Entity
{

	char			*file;						// entity filename
	float			frame;						// current frame
	nX_Envelop		LWmotion;					// the entity motion for LIGHTWAVE
	void			GetLWMotion(float);			// Compute LW Motion at given frame
	void			GetExtraLWMotions(float);	// Compute extra LW motions at given frame (Dissolve, Morph, etc...)
	nX				*pnX;						// the nX instance we're registered into
	nX_Scene		*scene;						// then scene instance that created this

	bool			locked;						// the object is locked so nobody will delete it!
	void			Reset();					// reset entity states

	// EFFECTOR members -------------------------------------------------------

#ifdef nX_EFFECTOR
	bool			wave;						// wave effector?
	float			*pLong, *pLat;				// longitude/latitude lookups
	float			waveA;						// wave angle

	void			GetWaveGradients();			// compute edge coordinates
	void			ApplyWave();				// apply [WAVE] effector to entity
#endif

#ifdef nX_MDPlug
	bool			usemdd;						// MDD displacement?
	unsigned short	nMDstep;					// # steps in MDD
	MDStep			*MD, *cMD;					// MD Steps list and current step
	float			stepTC;						// elapsed tick count for current step
	bool			MDloop, MDbstop;			// MDD loop? Stop MD blend

	bool			LoadMDD(char *);			// load MDD 1.0 file
	void			ApplyMDD();					// apply MDD to entity
#endif

#ifdef	nX_ENABLETRAIL
	nX_Trail		*traillist;					// trail lists
	void			ComputeTrail();				// allocate trail lists for entity
	void			DrawTrail(float *);			// update and draw trails
	void			FreeTrail();				// free trails
	void			ResetTrail();				// reset all trails
#endif

	// IMPORTER

#ifdef nX_DOA2IMPORT
	void			ImportDOA2MeshFile(char *);	// load a DOA2 mesh
#endif

	// COMMON STUFF -----------------------------------------------------------

	unsigned char	type;
	bool			hmatrixvalid;				// tell wether hmatrix reflect the actual hierarchy concatened matrix or needs update
	bool			inIKchain;					// is this entity affected by an IK chain?

	float			matrix[16];					// pmatrix * smatrix * rmatrix * tmatrix
	float			rmatrix[9];					// concatened hierarchy rotation matrix
	float			hmatrix[16];				// concatened hierarchy matrix
	float			hrmatrix[9];				// concatened rotation matrix
	float			fmatrix[16];				// final matrix concatened with world matrix
	float			frmatrix[9];				// final rotation matrix concatened with world matrix

	void			GetMatrix();				// Compute entity matrix
	void			GetHMatrix();				// Compute hierarchical entity matrix

	float			offset[3];					// Position {x;y;z}
	float			angle[3];					// Angles
	float			scale[3];					// Scales
	float			pivot[3];					// Pivot point

	// HIERARCHY ------------

	bool			hMatrixisUptodate;			// Is hmatrix fully computed?
	unsigned short	iparent;					// Temporary link to # parent
	nX_Entity		*dparent;					// Direct link to parent adress
	unsigned short	igoal;						// Temporary link to # goal
	nX_Entity		*dgoal;						// Direct link to goal adress
	unsigned short	itarget;					// Temporary link to # target
	nX_Entity		*dtarget;					// Direct link to target adress

	// IK -------------------

	void			ResolveIK();				// Resolve IK problem for current IK chain node
	bool			IKAnchor;					// Is entity an anchor in its IK chain?
	float			IKminHeading, IKmaxHeading;	// Heading constraints
	float			IKminPitch, IKmaxPitch;		// Pitch constraints
	bool			IKHLimits, IKPLimits;		// Tells wich constraints to apply (head, pitch) [0||1]

	// GEOMETRY ---------------------------------------------------------------

	unsigned short	nbpoints;					// # of vertice
	unsigned long	totalglobaledgenumber;		// # of vertice, counted as if all polygons were independent
	unsigned long	nbface;						// # of polygons
	unsigned long	nbtri;						// # of triangles (polygons are counted in too but as triangls of course (:)
	unsigned short	nbsurf;						// # of surface
	nX_reliure		*reliure_adress;			// adress of entity's reliure
	unsigned long	*indexlist;					// vertex index for polys

	void			GetMorphGradient(nX_Entity *, float **, float **, float **);
	void			GetFaceNormals();			// Compute entity's face normals
	void			GetEdgeNormals();			// Compute entity's edge normals
	void			Resize(float);				// Multiply entity's vertice and motion by float
	bool			stdenormal;					// standard edge normal
	float			*oMesh;						// original mesh
	float			*edge_adress;				// adress of vertice
	float			*previous_edge;				// vertice transformed during previous frame
	float			*ofacenormals;				// original face normals adress
	float			*oedgenormals;				// original edge normals adress
	float			*edgenormals_adress;		// adress of edge normals
	float			*previous_edgenormals;		// edge normals transformed during previous frame
	float			*facenormals_adress;		// adress of face normals
	float			*previous_facenormals;		// face normals transformed during previous frame

	bool			perframenormalcalc;			// object requiring all of its normals to be recalculated every frame?
	long			*qfacefromedge;				// buffer to speedup normal calculation
	bool			ffeuptodate;				// is buffer up to date?

	void			RotateNormals(long, float *, float *, float *, long, bool);
	void			RotateXYZ();				// Rotate entity's vertice

	void			TestBBVisibility();			// test entity's BB against scene viewport. 0-Unseen, 1-Fully on screen, 2-Partially on screen
	unsigned char	visibility;					// last BB test result
	dXYZ			BBmin, BBmax;				// Bounding box
	short			BBmnx, BBmny;				// Corners of BBox AFTER clipping
	char			BBv;						// Bounding value

	void			Freeze();					// add transformed geometry to nX_Facelist
	bool			freezed;					// tells wether 'previous' buffers are up-to-date

	// TEXMAPS ----------------------------------------------------------------

	void			GetUV();					// Compute UV mapping coords for TexMap
	float			*UVmap[nX_MAXTEXLEVEL];		// UV mapping coords for each texture level
	bool			texlevel[nX_MAXTEXLEVEL];	// texlevel active?
	unsigned long	requirement;				// entity requirement (face normal/edge normal/etc...)

	// TSP IMPLEMENTATION -----------------------------------------------------

	nX_TSPnode		*TSP;												// TSP root node (NULL if no tree)
	unsigned char	PositionPolyPlane(nX_TSPpoly *, cM_Plane *);		// Get position of an TSPPoly in refrence of a Plane
	void			PutPolyInTSP(nX_TSPnode *, nX_TSPpoly *);			// Insert a TSPPoly in tree
	nX_TSPpoly		*CreateTSPPoly(nX_reliure *, unsigned short);		// Create a TSPPoly
	void			GetTSP();											// Compute static TSP for object

	// Intersect viewport with entity's TSP, last to NULL to disable FARZ Clipping
	unsigned short	IntersectViewWithTSP(cM_Plane *, cM_Plane *, cM_Plane *, cM_Plane *, cM_Plane *);
	bool			IntersectRayWithTSP(dXYZ *, dXYZ *);				// Does ray intersect TSP?
	bool			ConfirmRayIntersect(nX_TSPpoly *);					// Test point-in-poly
	void			IntersectFiniteRayWithTSP(nX_TSPnode *);			// Intersect finite ray with TSP
	unsigned char	PositionFiniteRayPlane(dXYZ *, dXYZ *, cM_Plane *);	// Get position of finite ray in refrence of a Plane

	// LIGHTMAPS --------------------------------------------------------------

	void			GetUVLightMap();			// Compute UV mapping coords for LightMaps
	fXYZ			*polydeltas;				// entity polygons deltas
	cM_Plane		*faceplanes;				// plane equations for every faces
	nX_LightMapInfo	*lmaplist;					// lightmap infos for every faces in entity

	bool			forceray, unseenray;		// force raytracer to see/unsee object
	bool			nolmap, dolmap;				// force lightmap computing or prevent computation
	bool			nocastshadow;				// do we cast shadows?
	bool			staticinscene;				// is the object perfectly static in scene? including its hierarchy!	

	// Get entity's UV lightmap coordinate set
	unsigned char	*GetUVLightMap(long, char *, unsigned char *, xLinker *);

	// FXs --------------------------------------------------------------------

	nX_Envelop		*Morph;						// Morph envelop driven
	unsigned short	morphtarget;				// Link to # of morph target
	float			*xyzmorphdeltas;			// Vertice morph gradients [0~1]
	float			*nfamorphdeltas;			// Face normals morph gradients [0~1]
	float			*nedmorphdeltas;			// Edge normals morph gradients [0~1]

	bool			useworm;					// Worm effector
	bool			unaffectedbyfog;			// is entity affected by fog?
	nX_Envelop		*Dissolve;					// Envelop controlled dissolve (object alpha channel)
	nX_Envelop		*PolySize;					// Envelop controlled polygon size

	// Spark Engine ---------
	void			SetupSparkEmit(unsigned short);		// Setup SparkEmiter
	nX_SparkEmiter	*sparkemiter;				// spark emiter

	// MetaObjects (currently only support metaballs)
	nX_metaball		*metaball;					// valid if object is a meta object

	// Reads mouse input & compute resulting motion...
	void			ReadMotionFromUser();		// Get motion from I/O inputs (mouse)

	// Insert a Lens in facelist
	void			LensFX(float, float, float, nX_Surface *);
#ifdef	nX_SPLENSFX
	// Insert a super Lens in facelist
	void			SpLensFX(float, float, float, nX_Surface *);
#endif

	// LIGHT SPECIFIC ---------------------------------------------------------

	long			lighttype;					// Light type: POINT/SPOT/DISTANT
	nXcolor			lightcolor, ilightcolor;	// Light color
	nX_Envelop		*LightInt;					// Envelop driven light intensity
	nX_Envelop		*EdgeAngle;					// Envelop driven spot edge angle
	nX_Envelop		*ConeAngle;					// Envelop driven spot cone angle
	nX_Envelop		*LightRange;				// Envelop driven light range
	bool			lensflare;					// Is there a lens flare here?
	bool			lensreflection;				// Enable lens reflection?
	nX_Surface		*flaresurface;				// The flare surface
	nX_Envelop		*FlareInt;					// Envelop driven flare intensity

	// LINKED LISTS -----------------------------------------------------------

	nX_Entity		*LLnext;					// Link to next entity

	// CONSTRUCTOR/DESTRUCTOR -------------------------------------------------

	nX_Entity(nX *);
	~nX_Entity();

};