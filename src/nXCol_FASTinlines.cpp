static	void			inline lnXcol(nXcolor *nXcol, unsigned long color)
{
	_asm
	{
		mov		edi,nXcol
		mov		eax,color
		mov		[edi],eax
	}
}

static	unsigned long	inline	nXcoll(nXcolor *color)
{
	ulong	res;

	_asm
	{
		mov		esi,color
		mov		eax,[esi]
		mov		res,eax
	}

	return	res;
}

static	void			inline	nXcolcpy(nXcolor *dst, nXcolor *src)
{
	_asm
	{
		mov		esi,src
		mov		edi,dst
		mov		eax,[esi]
		mov		[edi],eax
	}
}