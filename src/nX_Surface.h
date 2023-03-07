	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		SURFACE DEFINITION v9
		---------------------

		nX Surfaces holds informations about the way polygons belonging to a surface
		are to be rendered. A surface as almost total control over its polygon render.

		nX Surfaces are stored as Linked List to avoid memory waste.

	   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  */


		#define	nXML_NONE						-1
		#define	nXML_COLOR						0		// color mapped
		#define	nXML_EVMAP						1		// evmapped
//		#define	nXML_BUMP						2		// bump mapped
		#define	nXML_TRANSPARENCY				2		// transparency map
		#define	nXML_LIGHTMAP					3		// light mapped

		// definitions for surface caps

		#define nX_NULL							0
		#define nX_FLAT							1
		#define nX_MAPPED						2
		#define nX_LIGHTSOURCE					4
		#define nX_GOURAUD						8
		#define nX_BUMP							16
		#define nX_PHONGOVERSHADING				32
		#define nX_PERSPECTIVE					64
		#define nX_ADDITIVE						128
		#define nX_CHROMA						256
		#define nX_SUBSTRACTIVE					512
		#define nX_EVMAP						1024
		#define nX_NOSURFACE					2048
		#define	nX_ALPHATEX						4096
		#define	nX_LIGHTMAP						8192
		#define	nX_DOUBLESIDED					16384
		#define	nX_WIREFRAME					32768

		// definitions for extended surface caps

		#define nX_SPECULAR						1
		#define nX_ZBUFFER						2
		#define	nX_WIDEMAP						4
		#define	nX_FOREMOST						8
		#define	nX_MERGEDALPHA					16
		#define	nX_PROCEDURALMAPPING			32
		#define	nX_NOZBUFFER					64
		#define	nX_NOZWRITE						128
		#define	nX_BACKGROUND					256
		#define	nX_ADDEVMAP						512
		#define	nX_TRAIL						1024


struct nX_Surface
{

	char	*name;									// surface name, minimal allocation

	// ATTRIBUTES -------------------------------------------------------------
	///////////////////////////////////////////////////////////////////////////

	unsigned long	CAPS;							// the surface CAPS bitfield
	unsigned long	xtdCAPS;						// extended surface CAPS bitfield
	unsigned char	texcount;						// number of texture level in the surface

	nXcolor			color;							// color of the surface
	unsigned char	luminosity;						// luminosity of the surface
	float			transparency;					// transparency of the surface
	float			diffuse;						// diffuse of the surface
	float			specular;						// specular of the surface
	float			glossiness;						// glossiness of the surface
	float			glow;							// glow of the surface
	float			maxsmoothingangle;				// maxsmoothingangle for the surface
	unsigned long	requirement;					// requirement of the surface (edge/face normal)

	nX_Surface		*LLnext;						// next surface
	nX				*pnX;							// nX instance the surface is registered into

	// SURFACE TEXTURES -------------------------------------------------------
	///////////////////////////////////////////////////////////////////////////

	bool			texlevel[nX_MAXTEXLEVEL];		// is level mapped? table
	nX_TexMap		*map[nX_MAXTEXLEVEL];			// wich map at wich level?

	long			mappingtype[nX_MAXTEXLEVEL];	// type of mapping for each level
	char			mappingaxis[nX_MAXTEXLEVEL];	// axis of mapping for each level
	short			mapuwrap[nX_MAXTEXLEVEL];		// u wrapping state for each level
	short			mapvwrap[nX_MAXTEXLEVEL];		// v wrapping state for each level
	float			size[nX_MAXTEXLEVEL][3];		// size of texmap for each level
	float			center[nX_MAXTEXLEVEL][3];		// center of texmap for each level
	float			velocity[nX_MAXTEXLEVEL][3];	// velocity of texmap for each level

	bool			negative[nX_MAXTEXLEVEL];		// is map negative?
	bool			pblend[nX_MAXTEXLEVEL];			// is map bilinear?

	float			offsetx[nX_MAXTEXLEVEL];		// offsets for each level
	float			offsety[nX_MAXTEXLEVEL];
	float			offsetz[nX_MAXTEXLEVEL];

	// ------------------------------------------------------------------------
	//	lighting model advanced specifications:
	//		0 -> no advanced spec
	//		1 -> restrict lightning to n specific light (LGT_x[+x][+...])
	// ------------------------------------------------------------------------

	unsigned long	lightspec;						// enhanced lightning models
	bool			lightallowed[10];

	// METHODS ----------------------------------------------------------------
	///////////////////////////////////////////////////////////////////////////
	
	nX_MSG			QuickSetup(char *, long, long);	// quickly setup a basic surface
	void			FreeAllTexMap();				// free all texmap in all level of the surface (release d3d surfaces also)

	// CLASS CONSTRUCTOR/DESTRUCTOR -------------------------------------------
	///////////////////////////////////////////////////////////////////////////

	nX_Surface::nX_Surface(nX *);
	nX_Surface::~nX_Surface();

};