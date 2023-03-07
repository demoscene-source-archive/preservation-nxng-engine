// Raster common

	static nX_vertex		*pint, *pext;
	static float			coef;
	static long				tmpcalc, ztmpcalc, scansize;
	static float			fscansize;
	static unsigned long	*opscreen, *pscreen, *peoscan, gcolor;
	static long				*pwbuffer;
	static short			shorttmp;
	static long				longtmp;
	static unsigned long	*ylkup;

// Intensity (gouraud, map-gouraud, ...)

	static unsigned char	r, g, b;
	static unsigned char	*prshade, *pgshade, *pbshade;
	static unsigned char	*pmshade, *pimshade;

	static short			*pilookup;
	static short			*piilookup;

	static long				rpente, rint, gpente, gint, bpente, bint;
	static float			frint, frdint, frext, frdext;
	static float			fpenterint, fpenterext, frpente;
	static float			fgint, fgdint, fgext, fgdext;
	static float			fpentegint, fpentegext, fgpente;
	static float			fbint, fbdint, fbext, fbdext;
	static float			fpentebint, fpentebext, fbpente;

	static long				lalp, ialp;
	static short			*plalp, *pialp;

// Mapping (mapping, pmapping, map-gouraud, ...)

	static unsigned long	*mapadr, *refladr;
	static short			*bumpadr;
	static unsigned char	*alpadr, *ialpadr;

	static unsigned long	uint, vint, zint;
	static unsigned long	u2int, v2int, z2int;
	static unsigned long	u3int, v3int, z3int;
	static unsigned long	u4int, v4int, z4int;

	static long				upente, vpente, zpente;
	static long				u2pente, v2pente, z2pente;
	static long				u3pente, v3pente, z3pente;
	static long				u4pente, v4pente, z4pente;

	static float			fuint, fudint, fuext, fudext;
	static float			fvint, fvdint, fvext, fvdext;
	static float			fu2int, fu2dint, fu2ext, fu2dext;
	static float			fv2int, fv2dint, fv2ext, fv2dext;
	static float			fu3int, fu3dint, fu3ext, fu3dext;
	static float			fv3int, fv3dint, fv3ext, fv3dext;
	static float			fu4int, fu4dint, fu4ext, fu4dext;
	static float			fv4int, fv4dint, fv4ext, fv4dext;

	static float			fzint, fzdint, fzext, fvzext;

	static float			fpenteuint, fpenteuext, fupente;
	static float			fpentevint, fpentevext, fvpente;
	static float			fpenteu2int, fpenteu2ext, fu2pente;
	static float			fpentev2int, fpentev2ext, fv2pente;
	static float			fpenteu3int, fpenteu3ext, fu3pente;
	static float			fpentev3int, fpentev3ext, fv3pente;
	static float			fpenteu4int, fpenteu4ext, fu4pente;
	static float			fpentev4int, fpentev4ext, fv4pente;

	static float			fpentezint, fpentezext, fzpente;

// Correct mapping...

	static float			fscanu, fscanv, fscanz;
	static float			tupente, tvpente, tzpente, tuint, tvint, tzint, tcoef;

// FogPass:: Perform fogging of a given Screen structure.

	static long				count, fogcol;