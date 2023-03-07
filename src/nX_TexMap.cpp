	/*----------------------------------------------------------------------------

		nXv9 by xBaRr.Syndrome 2000
			(nXs3) by xBaRr.Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		nXng TexMap implementation file

	----------------------------------------------------------------------------*/


	#include	"IMG.h"
	#include	"TARGA.h"
	#include	"generic.h"

	#include	"nXng/nX_HeaderList.h"
	#include	"IO/BitConvertIO.h"
	#include	"nXng/cColor.h"

	// some externals
	extern	qFileIO		iIO1, iIO2;
	extern	unsigned char	mulshader[65536];

	// some statics
	static	char str[256];			// the error string to display msgbox
	static	cColor	iColor;			// the color object used to do conversion
	static	BitIO	iBit;			// the BitIO object used to perform bit manipulation
	static	qFileIO	lIO;
	static	cMath	lMath;


// EvaluateNearestTexel(float, float, nXcolor *) ------------------------------
///////////////////////////////////////////////////////////////////////////////

void	nX_TexMap::EvaluateNearestTexel(float u, float v, nXcolor *tex)
{

	if (!adress)
		return;

	char	*mapadr = (char *)adress;
	mapadr += ( (long(u * width) + long(v * height) * width) ) * 4;
	tex->r = mapadr[2];
	tex->g = mapadr[1];
	tex->b = mapadr[0];

}

// EvaluateLinearTexel(float, float, nXcolor *) ------------------------------
///////////////////////////////////////////////////////////////////////////////

void	nX_TexMap::EvaluateLinearTexel(float ut, float vt, nXcolor *tex)
{

	if (!adress)
		return;

	float	u = ut * width;
	float	v = vt * height;
	unsigned char	*mapadr = (unsigned char *)adress;
	unsigned char	*adroff;
	nXcolor	center, right, down, dnrg;

	u = lMath.Modulo(u, width);
	v = lMath.Modulo(v, height);

	adroff = mapadr + ( long(u) + long(v) * width ) * 4;
	if ( (u < 1.0) || (v < 1.0) || (u > width - 1) || (v > height - 1) )
	{
		tex->r = adroff[2]; tex->g = adroff[1];	tex->b = adroff[0];
		return;
	}
	else
	{	center.r = adroff[2]; center.g = adroff[1];	center.b = adroff[0];	}

	adroff = mapadr + ( long(u) + long(v) * width ) * 4 + 4;
	right.r = adroff[2]; right.g = adroff[1]; right.b = adroff[0];
	adroff = mapadr + ( long(u) + long(v) * width ) * 4 + width * 4;
	down.r = adroff[2];	down.g = adroff[1];	down.b = adroff[0];
	adroff = mapadr + ( long(u) + long(v) * width ) * 4 + width * 4 + 4;
	dnrg.r = adroff[2];	dnrg.g = adroff[1];	dnrg.b = adroff[0];

	float	dx = u - long(u);
	float	omdx = float(1.0 - dx);
	float	dy = v - long(v);
	float	omdy = float(1.0 - dy);

	// compute bilinear texel
	tex->r = unsigned char( float(center.r) * omdx * omdy +
							float(down.r) * omdx * dy +
							float(right.r) * dx * omdy +
							float(dnrg.r) * dx * dy	);
	tex->g = unsigned char( float(center.g) * omdx * omdy +
							float(down.g) * omdx * dy +
							float(right.g) * dx * omdy +
							float(dnrg.g) * dx * dy	);
	tex->b = unsigned char( float(center.b) * omdx * omdy +
							float(down.b) * omdx * dy +
							float(right.b) * dx * omdy +
							float(dnrg.b) * dx * dy	);

}

// Decode() -------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

bool	nX_TexMap::Load(char *id, xLinker *xpk)
{

	IMG		lIMG;
	GENERIC	lGEN(&lIMG);
	unsigned char	*adr;
	unsigned long	sze;

	name = new char[strlen(id) + 1];
	strcpy(name, id);

	// Detect IMAGE sequences...
	long			c;
	long			len = strlen(id) - 11;		// REAL length (without the appended ' (sequence)' string)
	char			*path, *base, *ext, *imgname, *pw, *pw2;
	char			*psq = id + len;
	nX_AnimTexMap	*pFrame = &BankList, *prvFrame;

	if (len > 0 && !strcmp(psq, " (sequence)"))
	{
		pnX->cscene->nbsequence++;

		// We have a sequence to load!
		path = new char[strlen(id) + 1];
		base = new char[strlen(id) + 1];
		ext = new char[strlen(id) + 1];
		imgname = new char[strlen(id) + 1];

		// Find path
		while (psq[0] != '\\') psq--;
		pw = path; pw2 = id;
		while (pw2 != psq)
			*pw++ = *pw2++;
		pw[0] = '\\'; pw[1] = 0;

		// Find basename
		psq[0] = 0;
		pw = base;
		for (c = strlen(id) + 1; c < len; c++)
		{
			if (id[c] >= '0' && id[c] <= '9')	break;
			*pw++ = id[c];
		}
		pw[0] = 0;

		// Find extension
		while (id[c] != '.') c++;
		pw = ext;
		*pw++ = '.';
		for (c++; c < len; c++)
			*pw++ = id[c];
		pw[0] = 0;

		// Load sequence
		while (true)
		{
			wsprintf(imgname, "%s%s%.4d%s", path, base, nbframe + 1, ext);
			if (!pnX->Load(imgname, &adr, &sze, xpk))
			{
				// if we could not load at least 1 pic then we failed!
				if (!nbframe)
				{
					delete(path);
					delete(base);
					delete(ext);
					delete(imgname);
					return false;
				}
				else
				{
					delete (pFrame);
					prvFrame->nFrame = &BankList;
					break;
				}
			}
			
			// Load new frame
			lIMG.UseAfterDeath(true);
			lIMG.SetSource(adr, sze);
			lGEN.GetType(imgname);
			if ( lGEN.DecodeBuffer() != IMG_OK )
			{
				wsprintf(str, "SEQUENCED DecodeBuffer() failed! Could not decode '%s'.", imgname);
				pnX->Verbose(str);
				return false;
			}

			pFrame->width = (unsigned short)lIMG.width;
			pFrame->height = (unsigned short)lIMG.height;
			pFrame->adress = (unsigned long *)lIMG.adress;
			pFrame->nFrame = new nX_AnimTexMap;
			memset(pFrame->nFrame, 0, sizeof(nX_AnimTexMap));
			prvFrame = pFrame;
			pFrame = pFrame->nFrame;
			nbframe++;
		}

		// Goooood (:
		delete(path);
		delete(base);
		delete(ext);
		delete(imgname);

	}
	else
	{

		// Still image to go...
		if (!pnX->Load(id, &adr, &sze, xpk))
			return false;

		lIMG.UseAfterDeath(true);
		lIMG.SetSource(adr, sze);
		lGEN.GetType(id);

		if ( lGEN.DecodeBuffer() != IMG_OK )
		{
			wsprintf(str, "DecodeBuffer() failed! Could not decode '%s'.", name);
			pnX->Verbose(str);
			return false;
		}

		BankList.width = (unsigned short)lIMG.width;
		BankList.height = (unsigned short)lIMG.height;
		BankList.adress = (unsigned long *)lIMG.adress;

	}

	return true;

}

// GetChroma() ----------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

void	nX_TexMap::GetChroma()
{

	long			x, y;
	unsigned char	*pa;
	unsigned long	*pw;
	long			v;
	nX_AnimTexMap	*pFrame = &BankList;

	while (true)
	{
		SetCurrentFrame(pFrame);

		if (!alpha)
		{
			alpha = new unsigned char[width * height];
			pa = alpha;
			pw = adress;

			for (y = 0; y < height; y++)
			{
				for (x = 0; x < width; x++)
				{
					v = long(float (iColor.RGB888greyscale(pw[x]) - 20.0) * (255.0 / 60.0));
					if (v < 0) v = 0; else if (v > 255) v = 255;
					pa[x] = unsigned char(255 - v);
				}
				pw += width;
				pa += width;
			}
		}

		UploadToFrame(pFrame);
		pFrame = pFrame->nFrame;
		if (pFrame == &BankList)
			break;
	}

}

// GetAlpha() -----------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

void	nX_TexMap::GetAlpha()
{

	long	x, y;
	unsigned char	*pa;
	unsigned long	*pw;
	nX_AnimTexMap	*pFrame = &BankList;

	while (true)
	{
		SetCurrentFrame(pFrame);

		if (!alpha)
		{
			alpha = new unsigned char[width * height];
			pa = alpha;
			pw = adress;

			for (y = 0; y < height; y++)
			{
				for (x = 0; x < width; x++)
					pa[x] = iColor.RGB888greyscale(pw[x]);
				pw += width;
				pa += width;
			}
		}

		UploadToFrame(pFrame);
		pFrame = pFrame->nFrame;
		if (pFrame == &BankList)
			break;
	}

}

// GetLOD(long) : Return nearest LOD ------------------------------------------
///////////////////////////////////////////////////////////////////////////////

unsigned short	nX_TexMap::GetLOD(unsigned short UNround)
{
	unsigned short	LOD = 1;

	while (LOD < UNround)
		LOD *= 2;

	return	LOD;
}

// Negative() -----------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

void	nX_TexMap::Negative()
{

	long	c, c2;
	unsigned char	*ptr, *wptr;

	nX_AnimTexMap	*pFrame = &BankList;

	while (true)
	{
		SetCurrentFrame(pFrame);
		ptr = (unsigned char *)adress;

		for (c2 = 0; c2 < height; c2++)
		{
			wptr = ptr;
			for (c = 0; c < width; c++)
			{
				wptr[0] = 255 - wptr[0];
				wptr[1] = 255 - wptr[1];
				wptr[2] = 255 - wptr[2];
				wptr += 4;
			}
			ptr += width * 4;
		}

		UploadToFrame(pFrame);
		pFrame = pFrame->nFrame;
		if (pFrame == &BankList)
			break;
	}

	stdCAPS |= nXTMC_NEGATIVE;

}

// Stretch(long, long, long): Resize the map using bilinear filtering ---------
///////////////////////////////////////////////////////////////////////////////

void	nX_TexMap::Stretch(unsigned short w, unsigned short h)
{

	// buffer to stretch
	unsigned char	*src;
	unsigned long	*dest, *wdst;
	long	dx, dy;
	long	offset;
	long	rh, gh, bh, rv, gv, bv;
	long	px, py = 0;
	long	coefx, coefy;

	if (w != width || h != height)
	{

		// PERFORM STRETCHING
		/////////////////////

		src = (unsigned char *) adress;
		dest = new unsigned long[w * h];
		wdst = dest;

		dx = long (width * 65536 / w);
		dy = long (height * 65536 / h);

		for (long c2 = 0; c2 < h; c2++)
		{
			px = 0;

			for (long c = 0; c < w; c++)
			{

				offset = long(py >> 16) * width + long(px >> 16);

				coefx = px & 0xff00;
				coefy = py & 0xff00;

				// color map


				// linear x interp
				if ( ((px >> 16) < width - 1) &&
					(src[offset * 4 + 2] != src[(offset + 1) * 4 + 2]) &&
					(src[offset * 4 + 1] != src[(offset + 1) * 4 + 1]) &&
					(src[offset * 4 + 0] != src[(offset + 1) * 4 + 0])
				   )
				{
					rh = mulshader[(0xff00 - coefx) + src[offset * 4 + 2]] + mulshader[coefx + src[(offset + 1) * 4 + 2]];
					gh = mulshader[(0xff00 - coefx) + src[offset * 4 + 1]] + mulshader[coefx + src[(offset + 1) * 4 + 1]];
					bh = mulshader[(0xff00 - coefx) + src[offset * 4 + 0]] + mulshader[coefx + src[(offset + 1) * 4 + 0]];
				}
				else
				{
					rh = src[offset * 4 + 2];
					gh = src[offset * 4 + 1];
					bh = src[offset * 4 + 0];
				}
				
				// linear y interp
				if ( ((py >> 16) < height - 1) &&
					(src[offset * 4 + 2] != src[(offset + width) * 4 + 2]) &&
					(src[offset * 4 + 1] != src[(offset + width) * 4 + 1]) &&
					(src[offset * 4 + 0] != src[(offset + width) * 4 + 0])
				   )
				{
					rv = mulshader[(0xff00 - coefy) + src[offset * 4 + 2]] + mulshader[coefy + src[(offset + width) * 4 + 2]];
					gv = mulshader[(0xff00 - coefy) + src[offset * 4 + 1]] + mulshader[coefy + src[(offset + width) * 4 + 1]];
					bv = mulshader[(0xff00 - coefy) + src[offset * 4 + 0]] + mulshader[coefy + src[(offset + width) * 4 + 0]];
				}
				else
				{
					rv = src[offset * 4 + 2];
					gv = src[offset * 4 + 1];
					bv = src[offset * 4 + 0];
				}

				rh = (rh + rv) / 2;
				gh = (gh + gv) / 2;
				bh = (bh + bv) / 2;
				wdst[c] = (rh << 16) + (gh << 8) + bh;
				px += dx;

			}
			wdst += w;
			py += dy;
		}

		// erase old buffer
		width = w;
		height = h;

		// we're done
		delete adress;
		adress = dest;
	}

}

// ApplySurfaceFXs(nX_Surface *, long) ----------------------------------------
///////////////////////////////////////////////////////////////////////////////

void	nX_TexMap::ApplySurfaceFXs(nX_Surface *surface, long texlvl)
{
	if (surface->negative[texlvl])
		Negative();
}

// ConformToD3D() -------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

void	nX_TexMap::ConformToD3D()
{

	unsigned short	w, h;
	nX_AnimTexMap	*pFrame = &BankList;

	while (true)
	{

		SetCurrentFrame(pFrame);

		w = GetLOD(width);
		h = GetLOD(height);
		if (w > pnX->screen->cd3ddevice->maxmapw)
			w = pnX->screen->cd3ddevice->maxmapw;
		else if (w < pnX->screen->cd3ddevice->minmapw)
			w = pnX->screen->cd3ddevice->minmapw;
		if (h > pnX->screen->cd3ddevice->maxmaph)
			h = pnX->screen->cd3ddevice->maxmaph;
		else if (h < pnX->screen->cd3ddevice->minmaph)
			h = pnX->screen->cd3ddevice->minmaph;
		Stretch(w, h);

		UploadToFrame(pFrame);
		pFrame = pFrame->nFrame;
		if (pFrame == &BankList)
			break;

	}

}

// Update() -------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

void	nX_TexMap::Update()
{

	msdelay -= pnX->cscene->elapsedframecount;
	while (msdelay <= 0)
	{
		msdelay += nX_MSANIMDELAY;
		cFrame = cFrame->nFrame;
	}
	SetCurrentFrame(cFrame);

}


// SetCurrentFrame() ----------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

void	nX_TexMap::SetD3DMergeAlpha(nX_TexMap *mapma)
{
	nX_AnimTexMap	*pFrame = &BankList;

	while (true)
	{
		SetCurrentFrame(pFrame);
		d3dmergealpha = mapma;
		UploadToFrame(pFrame);
		pFrame = pFrame->nFrame;
		if (pFrame == &BankList)
			break;
	}
}

// SetCurrentFrame() ----------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

void	nX_TexMap::SetCurrentFrame(nX_AnimTexMap *frame)
{

	sD3D = frame->sD3D;
	width = frame->width;
	height = frame->height;
	adress = frame->adress;
	alpha = frame->alpha;
	d3dmergealpha = frame->d3dmergealpha;
	cFrame = frame;

}

// UploadToFrame() ------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

void	nX_TexMap::UploadToFrame(nX_AnimTexMap *frame)
{

	frame->sD3D = sD3D;
	frame->width = width;
	frame->height = height;
	frame->adress = adress;
	frame->alpha = alpha;
	frame->d3dmergealpha = d3dmergealpha;

}

// GetiD3D() ------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

bool	nX_TexMap::GetiD3D()
{

	bool					colorpixels, alphapixels;
	long					LODheight, LODwidth;
	long					c, c2, pitch;
	unsigned short			rbit, gbit, bbit, abit;
	unsigned short			rshift, gshift, bshift, ashift;
	unsigned char			r, g, b, a;
	unsigned char			*src, *dst, *alp;
	DDSURFACEDESC2			ddsd;

	nX_AnimTexMap	*pFrame = &BankList;

	while (true)
	{

		// set current frame as active frame in TexMap
		SetCurrentFrame(pFrame);

		if (!sD3D)
		{

			// make sure frame size is within accelerator limits!
			LODwidth = width;
			LODheight = height;

			// worry about alpha channel
			colorpixels = false;
			alphapixels = false;

			if (d3dmergealpha || alpha)
				alphapixels = true;
			if (adress)
				colorpixels = true;

			// we create a texture surface
			memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
			ddsd.dwSize				= sizeof(DDSURFACEDESC2);
			ddsd.dwFlags			= DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT | DDSD_TEXTURESTAGE;
			ddsd.dwWidth			= LODwidth;
			ddsd.dwHeight			= LODheight;
			ddsd.ddsCaps.dwCaps		= DDSCAPS_TEXTURE;
			ddsd.ddsCaps.dwCaps2	= DDSCAPS2_D3DTEXTUREMANAGE | DDSCAPS2_OPAQUE;

			// get some more infos on our destination pixel format
			if (alphapixels)
					memcpy (&ddsd.ddpfPixelFormat, &pnX->screen->D3D_ARGBtexfmt, sizeof (DDPIXELFORMAT));
			else	memcpy (&ddsd.ddpfPixelFormat, &pnX->screen->D3D_RGBtexfmt, sizeof (DDPIXELFORMAT));

			rbit = iBit.getnbbits(ddsd.ddpfPixelFormat.dwRBitMask);
			gbit = iBit.getnbbits(ddsd.ddpfPixelFormat.dwGBitMask);
			bbit = iBit.getnbbits(ddsd.ddpfPixelFormat.dwBBitMask);
			abit = iBit.getnbbits(ddsd.ddpfPixelFormat.dwRGBAlphaBitMask);
			rshift = iBit.getshiftbit(ddsd.ddpfPixelFormat.dwRBitMask);
			gshift = iBit.getshiftbit(ddsd.ddpfPixelFormat.dwGBitMask);
			bshift = iBit.getshiftbit(ddsd.ddpfPixelFormat.dwBBitMask);
			ashift = iBit.getshiftbit(ddsd.ddpfPixelFormat.dwRGBAlphaBitMask);

			// create the texture
			if (pnX->screen->pDD->CreateSurface(&ddsd, &sD3D, NULL) != DD_OK)
				return false;

			// now is time to perform some conversions (((: hmmm... we handle every texture format uhuh gulp (:
			src = (unsigned char *)adress;
			if (d3dmergealpha)
					alp = (unsigned char *)d3dmergealpha->alpha;
			else	alp = (unsigned char *)alpha;

			sD3D->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
			pitch = ddsd.lPitch;
			dst = (unsigned char *)ddsd.lpSurface;

			if (pnX->screen->D3D_RGBtexfmt.dwRGBBitCount == 32)
				for (c2 = 0; c2 < LODheight; c2++)
				{
					for (c = 0; c < LODwidth; c++)
					{
						b = src[0];
						g = src[1];
						r = src[2];
						src += 4;

						if (alphapixels)		// A
						{
							a = *alp++;
							*(unsigned long *)(dst + c * 4) = (a << ashift);
						}
						else
							*(unsigned long *)(dst + c * 4) = 0;

						if (colorpixels)		// RGB
							*(unsigned long *)(dst + c * 4) += (r << rshift) + (g << gshift) + (b << bshift);
					}
					dst += pitch;
				}

			else if (pnX->screen->D3D_RGBtexfmt.dwRGBBitCount == 24)
			{
			}
			else if (pnX->screen->D3D_RGBtexfmt.dwRGBBitCount == 16)
				for (c2 = 0; c2 < LODheight; c2++)
				{
					for (c = 0; c < LODwidth; c++)
					{
						b = src[0] >> (8 - bbit);
						g = src[1] >> (8 - gbit);
						r = src[2] >> (8 - rbit);
						src += 4;

						if (alphapixels)		// A
						{
							a = *alp++ >> (8 - abit);
							*(unsigned short *)(dst + c * 2) = (a << ashift);
						}
						else
							*(unsigned short *)(dst + c * 2) = 0;

						if (colorpixels)		// RGB
							*(unsigned short *)(dst + c * 2) += (r << rshift) + (g << gshift) + (b << bshift);
					}
					dst += pitch;
				}

			sD3D->Unlock(NULL);

			// free software textures memory
			delete adress;
			adress = NULL;
		}

		UploadToFrame(pFrame);
		pFrame = pFrame->nFrame;
		if (pFrame == &BankList)
			break;

	}

	return true;

}

// nX_TexMap CLASS destructor -------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

nX_TexMap::~nX_TexMap()
{

	nX_AnimTexMap	*pFrame = &BankList, *nFrame;

	while(true)
	{
		if (pFrame->alpha)
			delete(pFrame->alpha);
		if (pFrame->adress)
			delete(pFrame->adress);
		if (pFrame->sD3D)
			pFrame->sD3D->Release();

		nFrame = pFrame->nFrame;
		if (pFrame != &BankList)
			delete pFrame;
		if (nFrame == &BankList)
			break;
		pFrame = nFrame;
	}

	if (name)
		delete name;

	// we've been freed!!!
	if (pnXLL)
		pnX->UnregisterTexMap(pnXLL);

}

// nX_TexMap CLASS constructor ------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

nX_TexMap::nX_TexMap(nX *creator)
{

	memset(this, 0, sizeof(nX_TexMap));
	pnX = creator;

	// default TexMap to single frame animation
	memset(&BankList, 0, sizeof(nX_AnimTexMap));
	BankList.nFrame = &BankList;
	cFrame = &BankList;
	msdelay = nX_MSANIMDELAY;

}