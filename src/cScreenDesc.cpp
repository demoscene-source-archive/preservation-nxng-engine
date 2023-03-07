	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		cScreen formats description implementation file

	----------------------------------------------------------------------------*/


	#include	<ddraw.h>
	#include	<d3d.h>
	#include	<string.h>

	#include	"nXng/nX_console.h"
	#include	"nXng/cScreenMN.h"


/*
	ScreenMN::GetErrDesc()
	----------------------
	Copy error description string in *str
*/


void ScreenMN::GetErrDesc(CSC_MSG errCode, char *str)
{

	switch (errCode)
	{
		case	CSC_NOERROR:
			strcpy(str, "no error");
			break;
		case	CSC_GENERICERROR:
			strcpy(str, "generic error (#undefined)");
			break;
		case	CSC_WRONGDX:
			strcpy(str, "wrong DirectX version (7+)");
			break;
		case	CSC_ERRCREATEDD:
			strcpy(str, "error while creating DDRAW");
			break;
		case	CSC_ERRCOOPLVL:
			strcpy(str, "error while setting Cooperative Level");
			break;
		case	CSC_ERRCREATESURF:
			strcpy(str, "error while creating surface");
			break;
		case	CSC_ERRGETDEVID:
			strcpy(str, "error while getting device id");
			break;
		case	CSC_ERRGETCAPS:
			strcpy(str, "error while getting device caps");
			break;
		case	CSC_ERRQUERYD3D:
			strcpy(str, "error querying D3D");
			break;
		case	CSC_ERRENUMDEV:
			strcpy(str, "error while enumerating devices");
			break;
		case	CSC_ERRENUMMODES:
			strcpy(str, "error while enumerating display modes");
			break;
		case	CSC_ERRCREATECLIP:
			strcpy(str, "error while creating clipper object");
			break;
		case	CSC_ERRSETDISPLAYMODE:
			strcpy(str, "error while setting display mode");
			break;
		case	CSC_ERRGETATTSURF:
			strcpy(str, "error while getting attached surface");
			break;
		case	CSC_NOZBUFFER:
			strcpy(str, "no matching zbuffer support");
			break;
		case	CSC_ERRCREATEDEVICE:
			strcpy(str, "error while creating D3D device. Try another BPP!");
			break;
		case	CSC_ERRSETVIEWPORT:
			strcpy(str, "error while setting viewport");
			break;
		case	CSC_NOTEXTURE:
			strcpy(str, "no matching texture format");
			break;
		case	CSC_NOALPHATEXTURE:
			strcpy(str, "no matching alpha texture format");
			break;
		case	CSC_ERRCREAWIN:
			strcpy(str, "error while creating the render window");
			break;
		default:
			strcpy(str, "*** undefined *** OUT OF RANGE");
			break;
	}

}


/*
	ScreenMN::GetZBDesc()
	---------------------
	Copy ZBuffer description string in *str
*/


void ScreenMN::GetZBDesc(char zb, char *str)
{

		 if (zb == this->ZB_rating[ZB_NULL])
		strcpy(str, "none");
	else if (zb == this->ZB_rating[ZB_16])
		strcpy(str, "16 bit");
	else if (zb == this->ZB_rating[ZB_24])
		strcpy(str, "24 bit");
	else if (zb == this->ZB_rating[ZB_32])
		strcpy(str, "32 bit");
	else
		strcpy(str, "better than 32 bit! Wow... KWEEL");

}


/*
	ScreenMN::GetATFDesc()
	----------------------
	Copy alpha texture description string in *str
*/


void ScreenMN::GetATFDesc(char tf, char *str)
{

		 if (tf == this->ATF_rating[ATF_NULL])
		strcpy(str, "none");
	else if (tf == this->ATF_rating[ATF_RGB1555])
		strcpy(str, "ARGB1555");
	else if (tf == this->ATF_rating[ATF_RGB4444])
		strcpy(str, "ARGB4444");
	else if (tf == this->ATF_rating[ATF_RGB8888])
		strcpy(str, "ARGB8888");
	else
		strcpy(str, "better than ARGB8888! Wow! (:");

}


/*
	ScreenMN::GetTFDesc()
	----------------------
	Copy texture description string in *str
*/


void ScreenMN::GetTFDesc(char tf, char *str)
{

		 if (tf == this->TF_rating[TF_NULL])
		strcpy(str, "none");
	else if (tf == this->TF_rating[TF_PAL4])
		strcpy(str, "PAL4");
	else if (tf == this->TF_rating[TF_PAL8])
		strcpy(str, "PAL8");
	else if (tf == this->TF_rating[TF_RGB15])
		strcpy(str, "RGB555");
	else if (tf == this->TF_rating[TF_BGR16])
		strcpy(str, "BGR565");
	else if (tf == this->TF_rating[TF_RGB16])
		strcpy(str, "RGB565");
	else if (tf == this->TF_rating[TF_RGB32])
		strcpy(str, "RGB888");
	else
		strcpy(str, "better than RGB888! Wow! (:");

}
