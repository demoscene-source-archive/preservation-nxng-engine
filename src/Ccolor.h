	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		cColor definition file

	----------------------------------------------------------------------------*/


class cColor
{

	public:

	unsigned short	RGB555toRGB565(unsigned short);
	unsigned short	RGB555toBGR565(unsigned short);
	unsigned long	RGB555toRGB888(unsigned short);
	unsigned short	RGB888toRGB555(unsigned long);
	unsigned short	RGB888toRGB565(unsigned long);
	unsigned char	RGB555greyscale(unsigned short);
	unsigned char	RGB888greyscale(unsigned long);
	unsigned long	bRGBaslRGB(unsigned char, unsigned char, unsigned char);
	unsigned char	RGBgreyscale(unsigned char, unsigned char, unsigned char);

};