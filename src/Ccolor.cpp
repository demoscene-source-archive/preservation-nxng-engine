	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		cColor implementation file

	----------------------------------------------------------------------------*/


	#include	"nXng/cColor.h"
	#define		USE_ASM


unsigned short cColor::RGB555toRGB565(unsigned short input)
{

	unsigned short output;

	_asm
	{
		mov	ax,input
		mov	ebx,eax
		and	eax,31
		add	ebx,ebx
		and	ebx,1111111111000000b
		add	ebx,eax
		mov	output,bx
	}

	return output;

}

unsigned short cColor::RGB555toBGR565(unsigned short input)
{

	unsigned short output = 0;
	return output;

}

unsigned long cColor::RGB555toRGB888(unsigned short RGB16)
{

	unsigned long RGB24;
	unsigned char R8, G8, B8;

	R8 = (RGB16 >> 10) & 31;
	G8 = (RGB16 >> 5) & 31;
	B8 = (RGB16) & 31;

	RGB24=	(R8 << (3 + 16)) +
			(G8 << (3 + 8)) +
			(B8 << (3));

	return RGB24;
}

unsigned short cColor::RGB888toRGB555(unsigned long RGB24)
{

	unsigned short RGB16;
	unsigned char R8, G8, B8;

	R8 = (unsigned char)((RGB24 >> 16) & 255);
	G8 = (unsigned char)((RGB24 >> 8) & 255);
	B8 = (unsigned char)((RGB24) & 255);
	R8 = (R8 >> 3) & 31;
	G8 = (G8 >> 3) & 31;
	B8 = (B8 >> 3) & 31;

	RGB16 =	(R8 << 10) +
			(G8 << 5) +
			B8;

	return	RGB16;

}

unsigned short cColor::RGB888toRGB565(unsigned long RGB24)
{

	unsigned short	RGB16;
	unsigned char	R8, G8, B8;

	R8 = (unsigned char)((RGB24 >> 16) & 255);
	G8 = (unsigned char)((RGB24 >> 8) & 255);
	B8 = (unsigned char)((RGB24) & 255);
	R8 = (R8 >> 3) & 31;
	G8 = (G8 >> 2) & 63;
	B8 = (B8 >> 3) & 31;

	RGB16 =	(R8 << 11) +
			(G8 << 5) +
			B8;

	return	RGB16;

}

unsigned char cColor::RGB555greyscale(unsigned short RGB16)
{

	float	R = float((RGB16 >> 10) & 31);
	float	G = float((RGB16 >> 5) & 31);
	float	B = float(RGB16 & 31);

	R *= 256.0 / 32.0;
	G *= 256.0 / 32.0;
	B *= 256.0 / 32.0;

	float	I = float(R * 0.212671 + G * 0.715160 + B * 0.072169);

	return	unsigned char(I);	

}

unsigned char cColor::RGB888greyscale(unsigned long RGB32)
{
#ifdef	USE_ASM

	unsigned long	*pRGB = &RGB32;
	unsigned char	val;

	_asm
	{
		xor		eax, eax
		mov		esi, pRGB	
		mov		al, [esi]		// B
		xor		ebx, ebx
		imul	eax, 4730
		mov		bl, [esi + 1]	// G
		xor		ecx, ecx
		imul	ebx, 46869
		mov		cl, [esi + 2]	// R
		add		eax, ebx
		imul	ecx, 13937
		add		eax, ecx
		shr		eax, 16
		mov		val, al
	}

	return	val;

#else

	float	R = float((RGB32 >> 16) & 255);
	float	G = float((RGB32 >> 8) & 255);
	float	B = float(RGB32 & 255);
	float	I = float(R * 0.212671 + G * 0.715160 + B * 0.072169);
	return	unsigned char(I);	

#endif
}

unsigned char cColor::RGBgreyscale(unsigned char R, unsigned char G, unsigned char B)
{
#ifdef	USE_ASM
	return	unsigned char((R * 13937 + G * 46869 + B * 4730) / 65536);
#else
	float	I = float(R * 0.212671 + G * 0.715160 + B * 0.072169);
	return	unsigned char(I);	
#endif
}

unsigned long cColor::bRGBaslRGB(unsigned char R, unsigned char G, unsigned char B)
{
	return	(R<<16)+(G<<8)+B;
}
