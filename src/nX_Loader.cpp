	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		nX Loader implementation file

	----------------------------------------------------------------------------*/


	#include	"nX_HeaderList.h"
	static		qFileIO		lIO;				// last chance ON-DISK file reader
	static		char		str[256];


void	nX::SetLoader(qFileIO *io1, qFileIO *io2)
{
	iIO1 = io1; iIO2 = io2;
}


bool	nX::Load(char *file, unsigned char **to, unsigned long *sze, xLinker *xPK)
{

	unsigned long size;

	if (!file)
		return false;

	if (xPK)
	{
		if (size = xPK->SizeFromxLink(file))
			to[0] = (unsigned char*)xPK->LoadFromxLink(file);
		else
		{
			wsprintf(str, "nXLoader::xLinkLoad() failed on '%s'", file);
			Verbose(str);
			return false;
		}
	}
	else
	{
		if (iIO1 && (size = iIO1->GetSize(file)))
			to[0] = (unsigned char *)iIO1->Load(file);
		else if (iIO2 && (size = iIO2->GetSize(file)))
			to[0] = (unsigned char *)iIO2->Load(file);
		else if (size = lIO.GetSize(file))
			to[0] = (unsigned char *)lIO.Load(file);
		else
		{
			wsprintf(str, "nXLoader::cIOLoad() failed on '%s'", file);
			Verbose(str);
			return false;
		}
	}

	if (sze)
		sze[0] = size;

	return true;

}

bool	nX::FileExist(char *file, xLinker *xPK)
{

	if (!file)
		return false;
	if (xPK)
	{	if (xPK->SizeFromxLink(file))
			return true;	else	return false;	}
	else
	{	if ( (iIO1 && iIO1->GetSize(file)) ||
			 (iIO2 && iIO2->GetSize(file)) ||
			  lIO.GetSize(file))
			return true;	else	return false;	}

}