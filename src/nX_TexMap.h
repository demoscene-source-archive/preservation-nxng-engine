	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		TEXMAP DEFINITION vA
		--------------------

		nX Texmap a standard raw RGBA8888 pictures hold in linear memory space.
		They are used to map polygon or display bitmaps to screen.
		nX TexMaps are optimized within the same nX instance

		nX TexMaps are stored as Linked List to avoid memory waste.
		They also use a RefCount variable system in order to be shared.

		Added: -Animated frame support,
				Supports: Loopable, different size frame, FPS.

	   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  */


		#define		vTEXMAP				"1.A"
		#define		fTEXMAP				"Animated TexMap Class: Multiple frame size, FPS, Alpha."
		#define		nX_MSANIMDELAY		1.0		// 1 fps


		// Linked list node structure for texmaps


struct	nX_LLTexMap
{
	nX_TexMap			*nodemap;
	nX_LLTexMap			*prev, *next;
};


	/* These CAPS are meant to let us know if the essential RAW datas in the RGB buffer
	   have been modified and if so, how it was modified...
	   As a good example WIDE should not be in these CAPS because it only fill one extra
	   buffer in the TexMap and it doesn't modify the RGB datas itself.
	   But as all rules have their exceptions it still is here because it COULD modify the
	   RGB buffer for wrapping (because wrapping is not enabled in WIDE mode and if a surface
	   not enabled for WIDE map with wrapping find the map to fit its requirement then it
	   will resize the buffer to 256x256 making the wide rasterizer crash! ):		*/


	#define		nXTMC			unsigned long

	#define		nXTMC_NONE		0
	#define		nXTMC_NEGATIVE	1
	#define		nXTMC_WIDE		2

	// this flag is here only to ensure we'll not share the texmap
	#define		nXTMC_UNIQUE	3


struct	nX_AnimTexMap
{

	unsigned long			*adress;				// frame RAW adress
	unsigned char			*alpha;					// alpha replicate
	unsigned short			width, height;			// frame dimensions
	nX_TexMap				*d3dmergealpha;			// non multitexture card will like that (:
	LPDIRECTDRAWSURFACE7	sD3D;					// D3D surface adress

	nX_AnimTexMap			*nFrame;					// next frame

};

class	nX_TexMap
{

	public:

	// FRAME HANDLING ---------------------------------------------------------
	///////////////////////////////////////////////////////////////////////////

	char					*name;					// frame name
	void					SetCurrentFrame(nX_AnimTexMap *);
	void					UploadToFrame(nX_AnimTexMap *);
	nX_AnimTexMap			BankList;				// the "REAL" frame(s)
	nX_AnimTexMap			*cFrame;				// current frame
	unsigned char			fps;					// FPS
	unsigned short			nbframe;				// number of frame in anim
	float					frame;					// current anim frame
	float					msdelay;				// delay before next swap...
	bool					looped;					// Did the Anim looped already?
	bool					loopAnim;				// Does the anim loop?
	nX_TexMap				*d3dmergealpha;			// non multitexture card will like that (:
	void					Update();				// Update TexMap... setup LPsurf and frame counters

	// COMMON Variables and Methods -------------------------------------------
	///////////////////////////////////////////////////////////////////////////

	nXTMC					stdCAPS;				// the texmap CAPS
	unsigned long			*adress;				// frame RAW adress
	unsigned char			*alpha;					// alpha replicate
	unsigned short			width, height;			// frame dimensions

	// METHODS ----------------------------------------------------------------
	///////////////////////////////////////////////////////////////////////////

	// decode texture: ID, adress, size of buffer.
	bool			Load(char *, xLinker *);		// Load and Decode file to TexMap (animation included)

	void			ConformToD3D();					// conform texmap to d3d renderer
	// apply fx and constraint from the surface to the texmap
	void			ApplySurfaceFXs(nX_Surface *, long);


	void			GetAlpha();						// compute alpha map(s)
	void			GetChroma();					// compute alpha map(s) using colorkeying
	unsigned short	GetLOD(unsigned short);			// returns nearest pod to a unsigned short

	void			Negative();						// RGB negative
	// resize map (pitch support)
	void			Stretch(unsigned short, unsigned short);
	void			SetD3DMergeAlpha(nX_TexMap *);

	// Reference Count
	unsigned long	RefCount;						// number of scenes refering to this TexMap
	void			AddRef();						// Increment reference count
	void			Release();						// gluh!? (: hmmm... No! Microsoft did not invent the reference count principe! It was just OBVIOUS!
	nX_LLTexMap		*pnXLL;							// adress of this texmap in nX LLtexmap
	nX				*pnX;							// the adress of nX instance the TexMap is registered into...

	// Texel READ Methods -----------------------------------------------------
	///////////////////////////////////////////////////////////////////////////

	/*
		Evaluate the texel at coordinates {float u, float v}
		Nearest texel is returned.
	*/
	void			EvaluateNearestTexel(float, float, nXcolor *);
	/*
		Evaluate the texel at coordinates {float u, float v}
		Bilinear texel is returned.
	*/
	void			EvaluateLinearTexel(float, float, nXcolor *);

	// DIRECT3D Variables and Methods -----------------------------------------
	///////////////////////////////////////////////////////////////////////////

	LPDIRECTDRAWSURFACE7	sD3D;					// the current d3d surface
	bool					GetiD3D();				// get the d3d surface(s)

	// Constructor/Destructor -------------------------------------------------
	///////////////////////////////////////////////////////////////////////////

	nX_TexMap(nX *);
	~nX_TexMap();

};