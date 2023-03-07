	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		nXng Screen Manegement header file

	----------------------------------------------------------------------------*/


	#define			DDRAW_Maxboard				8
	#define			DDRAW_MaxDevice				8
	#define			DDRAW_Maxvmode				128

	#define			SASPECT_TELE				(4. / 3.)
	#define			SASPECT_CINEMA				(16. / 9.)
	#define			SASPECT_NSTCWIDESCREEN		(21. / 8.)

	// Supported output renderers
	#define			nX_RSOFT					0
	#define			nX_RD3D						1
	#define			nX_OPENGL					2

	// App States
	#define			APP_INIT					0
	#define			APP_RUNNING					1
	#define			APP_PAUSED					2
	#define			APP_FINISHNOW				3
	#define			APP_CRASH					4			// trop drôle non? (=
	#define			APP_FINISH					5

	// Messages handler
	#define			CSC_MSG						unsigned short

	#define			CSC_NOERROR					0			// no error
	#define			CSC_GENERICERROR			1			// generic error (#undefined)
	#define			CSC_WRONGDX					2			// wrong DirectX version (7+)
	#define			CSC_ERRCREATEDD				3			// error while creating DDRAW
	#define			CSC_ERRCOOPLVL				4			// error while setting Cooperative Level
	#define			CSC_ERRCREATESURF			5			// error while creating surface
	#define			CSC_ERRGETDEVID				6			// error while getting device id
	#define			CSC_ERRGETCAPS				7			// error while getting device caps
	#define			CSC_ERRQUERYD3D				8			// error querying D3D
	#define			CSC_ERRENUMDEV				9			// error while enumerating devices
	#define			CSC_ERRENUMMODES			10			// error while enumerating display modes
	#define			CSC_ERRCREATECLIP			11			// error while creating clipper object
	#define			CSC_ERRSETDISPLAYMODE		12			// error while setting display mode
	#define			CSC_ERRGETATTSURF			13			// error while getting attached surface
	#define			CSC_NOZBUFFER				14			// no matching zbuffer support
	#define			CSC_ERRCREATEDEVICE			15			// error while creating D3D device
	#define			CSC_ERRSETVIEWPORT			16			// error while setting viewport
	#define			CSC_NOTEXTURE				17			// no matching texture format
	#define			CSC_NOALPHATEXTURE			18			// no matching alpha texture format
	#define			CSC_ERRCREAWIN				19			// error while creating the render window

	// pixel format for texture maps with our rating
	#define			TF_NULL						-127
	#define			TF_PAL4						1
	#define			TF_PAL8						2
	#define			TF_RGB15					3
	#define			TF_BGR16					4
	#define			TF_RGB16					5
	#define			TF_RGB32					6
	#define			TF_X						7
	#define			TF_OUTOFDEF					8

	// pixel format for alpha texture maps with our rating
	#define			ATF_NULL					-127
	#define			ATF_RGB1555					1
	#define			ATF_RGB4444					2
	#define			ATF_RGB8888					3
	#define			ATF_X						4
	#define			ATF_OUTOFDEF				5

	// z buffers with our rating
	#define			ZB_NULL						-127
	#define			ZB_16						1
	#define			ZB_24						2
	#define			ZB_32						3
	#define			ZB_X						4
	#define			ZB_OUTOFDEF					5

struct VideoMode
{
	char			desc[128];
	unsigned short	width;
	unsigned short	height;
	long			pitch;
	unsigned char	bpp;
};

struct D3DDevice
{
	char			*name;
	GUID			dGUID;

	// D3D infos...
	bool			software;

	char			*lack;					// String with missing features... (:
	unsigned short	minmapw, minmaph;
	unsigned short	maxmapw, maxmaph;
	bool			pow2tex, squaretex;
	unsigned char	nbstage, mxnbmap;
	bool			fogrange;

	bool			renderbpp[4];			// 8/16/24/32 bpp
};

struct DDAdapter
{
	char			*name;
	GUID			GUID;

	bool			d3d;
	bool			windowed;
	unsigned char	nbdevice, cdevice;

	unsigned short	nbvm;
	VideoMode		vm[DDRAW_Maxvmode], cvm;

	D3DDevice		device[DDRAW_MaxDevice];
};

class ScreenMN
{

	public:

	bool					bWindowed;
	bool					bActive;
	bool					bReady;
	bool					page;
	RECT					rcWindow;
	RECT					rcViewport;
	RECT					rcScreen;

	HWND					hWnd;
	HDC						dc;
	HFONT					font;

	// DIRECT DRAW	

	LPDIRECTDRAW7			pDD;
	LPDIRECTDRAWSURFACE7	pDDSPrimary;
	LPDIRECTDRAWSURFACE7	pDDSBack;

	// DIRECT 3D

	LPDIRECT3D7				pD3D;
	LPDIRECT3DDEVICE7		pD3DDevice;
	D3DVIEWPORT7			pD3DView;
	D3DRECT					prcViewportRect;
	LPDIRECTDRAWSURFACE7	pDDZbuffer;

	// I/O with outer world

	nX_Console				*console;

	// Runtime management
	// ------------------

	unsigned short			AppState;

	HINSTANCE				hInstance;
	HACCEL					hAccel;

	CSC_MSG					OpenRenderWindow();
	void					CloseRenderWindow();
	unsigned long			Message();

	// Screen structure we're currently running with
	// ---------------------------------------------

	// Size informations

	unsigned short			width;
	unsigned short			height;
	unsigned short			pitch;

	/* This is the default resolution the demo was designed for... Every drawing op that output to screen
		should perform appropriate scalings to keep correct ratios...	*/

	unsigned long			referencewidth;
	unsigned long			referenceheight;
	unsigned long			referencebpp;
	float					wcoef;
	float					hcoef;

	// Rendering options

	bool					dither;
	bool					bilinear;

	// Color model informations

	unsigned char			vmbpp;
	unsigned char			bpp;
	bool					RGB16is555;
	unsigned long			FillColor;

	// This could seem useless but it allow us to force a certain mode

	char					TF_rating[TF_OUTOFDEF];
	char					currentRGBrating;
	DDPIXELFORMAT			D3D_RGBtexfmt;			// the RGB mode we'll use

	// This could seem useless but it allow us to force a certain mode

	char					ATF_rating[ATF_OUTOFDEF];
	char					currentARGBrating;
	DDPIXELFORMAT			D3D_ARGBtexfmt;			// the ARGB mode we'll use

	// This could seem useless but it allows us to force a certain mode

	char					ZB_rating[ZB_OUTOFDEF];
	char					currentZBrating;
	DDPIXELFORMAT			D3D_Zbuffer;			// the Zbuffer we'll use

	unsigned char			desktopbpp;
	char					DDRAW_ERROR[256];

	// Misc. definitions

	bool					dX_Bilinear;
	bool					dX_Dither;
	bool					bUseFrameReport;
	bool					bForceSingleRaster;
	bool					bFreeRide;
	bool					bBoundingBox;			// any BoundingBox to draw?
	bool					bBloom;
	char					*tmpMsg;				// temporary message mechanism
	unsigned short			tmpMsgCounter, tmpX, tmpY;
	float					RideRatio;
	unsigned long			DXRScount;				// # calls made to RenderState
	unsigned long			DXDPcount;				// # calls made to DrawPrimitive

	// Video Modes structure

	unsigned char			nbboard, cboard;
	DDAdapter				board[DDRAW_Maxboard];
	DDAdapter				*cddboard;				// current DDRAW board
	D3DDevice				*cd3ddevice;			// current D3D device on current DDRAW board

	// Prototypes

	CSC_MSG					DD_Init();
	void					DD_Free();

	void					GetErrDesc(CSC_MSG, char *);
	void					GetZBDesc(char, char *);
	void					GetTFDesc(char, char *);
	void					GetATFDesc(char, char *);
	char					BPP2fBPP(char);			// return flag corresponding to 8/16/24 or 32 bpp

	CSC_MSG					QueryBoards();
	bool					GetBoardsCaps();
	void					SelectBoard(unsigned char);			// select board
	void					SelectDevice(unsigned char);		// select device for current board. Must have called selectboard before!

	void					BeginFrame(bool);
	void					FrameRGB32toRGB15(unsigned char *, long);
	void					FrameRGB32toRGB16(unsigned char *, long);
	void					FrameRGB32toRGB24(unsigned char *, long);
	void					EndFrame();
	void					SetPhysicResolution(unsigned short, unsigned short, unsigned char);

	CSC_MSG					InitD3DObjects();
	CSC_MSG					ChangeD3DCoopLevel();
	void					ReleaseAllD3DObjects();

	// System communication

	void					SetConsole(nX_Console *);
	void					Verbose(char *);			// Display message in nX_Console if any
	// Slow way to output text to render window...
	void					Print(char *, unsigned short, unsigned short);
	// Fast way to output text to render window... MUST CALL PrintInit() BEFORE and PrintFree() AFTER!
	void					qPrint(char *, unsigned short);
	// Display temporary message lasting for unsigned short frame
	void					TempMsg(char *, unsigned short, unsigned short, unsigned short);
	void					DisplayTempMsg();			// Display temporary message if any
	unsigned short			yPos;
	bool					bPrintInitDone;
	void					PrintInit(unsigned short);	// Setup surface's DC and init initial Y position of print output
	void					PrintFree();				// Free surface's DC

	// Constructor/Destructor

	ScreenMN(HINSTANCE);
	~ScreenMN();

};
