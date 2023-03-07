	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		nXng Software Rasterizer header file

		*** THESE DECLARATIONS ARE ENCAPSULATED IN nX OBJECT! ***

	----------------------------------------------------------------------------*/


	void	nFlat(nX_face *, unsigned long, ScreenMN *);
	void	nFlatZ(nX_face *, short, ScreenMN *);
	void	nFlatAdd(nX_face *, short, ScreenMN *);
	void	nFlatAlp(nX_face *, unsigned long, ScreenMN *);
	void	nFlatAlpTex(nX_face *, nXcolor *, nX_TexMap *, ScreenMN *);
	void	WIDE_nFlatAlpTex(nX_face *, nXcolor *, nX_TexMap *, ScreenMN *);

	void	nGrd(nX_face *, ScreenMN *);
	void	nGrdAlp(nX_face *, ScreenMN *);
	void	nGrdAdd(nX_face *, ScreenMN *);
	void	MMX_nGrdAdd(nX_face *, ScreenMN *);

	void	nLmap(nX_face *, nX_TexMap *, ScreenMN *, long);
	void	nLmapShade(nX_face *, nX_TexMap *, nXcolor *, ScreenMN *, long);
	void	nLmapGrd(nX_face *, nX_TexMap *, ScreenMN *, long);
	void	nLmapGrdChroma(nX_face *, nX_TexMap *, ScreenMN *, long);
	void	nLmapGrdAdd(nX_face *, nX_TexMap *, ScreenMN *, long);
	void	nLmapAdd(nX_face *, nX_TexMap *, ScreenMN *, long);
	void	nLmapDoubleMap(long *, short *, short *, ScreenMN *, long);
	void	nLmapAlp(nX_face *, nX_TexMap *, ScreenMN *, long);
	void	nLmapAlpChroma(nX_face *, nX_TexMap *, ScreenMN *, long);
	void	nLmapAddAlp(nX_face *, nX_TexMap *, ScreenMN *, long);
	void	nLmapChroma(nX_face *, nX_TexMap *, ScreenMN *, long);
	void	nLmapAlpTex(nX_face *, nX_TexMap *, nX_TexMap *, ScreenMN *, long);
	void	MMX_nLmapAdd(nX_face *, nX_TexMap *, ScreenMN *, long);
	void	WIDE_nLmap(nX_face *, nX_TexMap *, ScreenMN *, long);
	void	WIDE_nLmapAdd(nX_face *, nX_TexMap *, ScreenMN *, long);

	void	nLbump(nX_face *, nX_TexMap *, nX_TexMap *, ScreenMN *);

	void	nPmap(nX_face *, nX_TexMap *, ScreenMN *, long);
	void	nPmapGrd(nX_face *, nX_TexMap *, ScreenMN *, long);
	void	nPmapAdd(nX_face *, nX_TexMap *, ScreenMN *, long);
	void	WIDE_nPmap(nX_face *, nX_TexMap *, ScreenMN *, long);

	void	nPrecaPol(nX_face *, long *, float *, bool *);
	void	nGlow(nX_face *, unsigned char, unsigned char *, ScreenMN *);
	void	nZbuff(nX_face *);

	// .:* MAGIC *:. one! (:

	void	GENERIC_rasterizer(nX_face *, nX_Surface *, ScreenMN *);