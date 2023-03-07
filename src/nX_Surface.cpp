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
//		nXng Surface implementation file
//
//	------------------------------------------------------------------------------


	#include	"nXng/nX_HeaderList.h"
	static	char	str[256];


//---------------------------------
nX_Surface::nX_Surface(nX *creator)
//---------------------------------
{
	memset(this, 0, sizeof nX_Surface);
	pnX = creator;
}


//-----------------------
nX_Surface::~nX_Surface()
//-----------------------
{

	if (name)
		delete name;

}


//------------------------------------------------------------------
nX_MSG	nX_Surface::QuickSetup(char *bitmap, long nCAPS, long xCAPS)
//------------------------------------------------------------------
{

	CAPS = nCAPS;

	if (bitmap && ((CAPS & nX_MAPPED) || (CAPS & nX_EVMAP)))
	{
		map[0] = pnX->FindTexMap(bitmap, NULL);

		if (!map[0])
		{
			map[0] = new nX_TexMap(pnX);

			if (!map[0])
			{
				pnX->Verbose("QuickSetup :: Not enough memory to create TexMap.");
				delete map[0];
				map[0] = NULL;
				CAPS &= -(nX_MAPPED + 1);
				CAPS &= -(nX_EVMAP + 1);
				return nX_TEXMAPALLOCERROR + nX_TEXMAPERROR;
			}
			if (!map[0]->Load(bitmap, NULL))
			{
				wsprintf(str, "QuickSetup :: Image '%s' not found!", bitmap);
				pnX->Verbose(str);
				delete map[0];
				map[0] = NULL;
				CAPS &= -(nX_MAPPED + 1);
				CAPS &= -(nX_EVMAP + 1);
				return nX_TEXMAPLOADERROR + nX_TEXMAPERROR;
			}

			pnX->RegisterTexMap(map[0]);
			map[0]->ApplySurfaceFXs(this, 0);
			map[0]->ConformToD3D();
		}

		map[0]->AddRef();
	}

	return nX_OK;

}


//---------------------------------
void	nX_Surface::FreeAllTexMap()
//---------------------------------
{

	for (long c = 0; c < nX_MAXTEXLEVEL; c++)
		if (map[c])
			map[c]->Release();

}
