	#define		WIN32_LEAN_AND_MEAN

	// General Header list for nX objects

	#include	<dinput.h>
	#include	<ddraw.h>
	#include	<d3d.h>

	#include	<math.h>
	#include	<stdio.h>

	#include	"nXng/nX_CompilationDirectives.h"

	// I/O module
	#include	"xPack/xPack.h"
	#include	"IO/qFileIO.h"
	#include	"cInput/cInput.h"

	// nX engine
	#include	"nXng/cMath.h"
	#include	"nXng/nX_Console.h"
	#include	"nXng/cScreenMN.h"
	#include	"nXng/cMotion.h"
	#include	"nXng/nX_MSG.h"
	#include	"nXng/nX_RawTypes.h"
	#include	"nXng/nX_Rasterize.h"

	// geometry effectors
	#include	"nXng/nX_Geometry.h"
	#include	"nXng/nX_Spark.h"
	#include	"nXng/nX_Metaball.h"

	// nX classes
	#include	"nXng/nX_Preca.h"
	#include	"nXng/nX_Core.h"
	#include	"nXng/nX_Scene.h"
	#include	"nXng/nX_Entity.h"
	#include	"nXng/nX_Surface.h"
	#include	"nXng/nX_TexMap.h"
