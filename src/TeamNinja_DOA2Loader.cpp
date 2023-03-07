	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		nXng Dead or Alive 2 importer implementation file

	----------------------------------------------------------------------------*/


	#include	"nX_HeaderList.h"
	#include	"IO/cqString.h"
	#include	"IO/MCIO.h"

	static		qString			cStr;
	static		MCIO			lMC;
	extern		unsigned char	mulshader[65536];


#ifdef nX_DOA2IMPORT

/////////////////////////////////////////////////////////////////////////////////////////
void	nX_Entity::ImportDOA2MeshFile(char *file)
/////////////////////////////////////////////////////////////////////////////////////////
{

	nX		*pnX = scene->pnX;
	oMesh = edge_adress;

	// load file via nX loader
	char			*buffer, *obuffer;
	long			size;
	pnX->Load(file, (unsigned char **)&buffer, (unsigned long *)&size, NULL);
	wsprintf(str, "    nX_Entity::LoadDOA(\"%s\");", file);
	pnX->Verbose(str);
	obuffer = buffer;

	// ensure we have something to decode
	if (!size)
	{
		pnX->Verbose("        ERROR: Loading DOA file...,");		
		return false;
	}

	// DOA model importer
	// ------------------








	// read MDD HEADER
	unsigned long			cf, ce;

	// # steps
	nMDstep = (unsigned short)mcI.GetLong(buffer);
	buffer += 4;

	// # points
	if (mcI.GetLong(buffer) != (unsigned long)(nbpoints - 8) )
	{
		pnX->Verbose("        ERROR: Incoherent MDD file ['#points != #MDced']... MDPlug disabled,");		
		return false;
	}
	buffer += 4;

	// allocate STEPS
	MD = new MDStep[nMDstep];
	memset(MD, 0, sizeof(MDStep) * nMDstep);
	for (cf = 0; cf < nMDstep; cf++)
		MD[cf].vertice = new float[nbpoints * 3];

	// read STEPS duration
	for (cf = 0; cf < nMDstep; cf++)
	{
		MD[cf].timestamp = (unsigned long)(mcI.LongAsFloat(mcI.GetLong( buffer )) * 1000);
		buffer += 4;
	}

	// read STEPS datas & compute normals
	float	*datas, *datad, *dst;

	for (cf = 0; cf < nMDstep; cf++)
	{
		datas = MD[cf].vertice;

		// get vertice
		for (ce = 0; ce < (unsigned long)(nbpoints - 8); ce++)
		{
			datas[0] = mcI.LongAsFloat(mcI.GetLong( buffer + 0 ));
			datas[1] = -mcI.LongAsFloat(mcI.GetLong( buffer + 4 ));
			datas[2] = mcI.LongAsFloat(mcI.GetLong( buffer + 8 ));
			buffer += 12;
			datas += 3;
		}

		// compute normals
		edge_adress = MD[cf].vertice;

		if (requirement & nX_USEFACENORMAL)
		{
			facenormals_adress = MD[cf].fnorm = new float[nbface * 3];
			GetFaceNormals();
		}
		if (requirement & nX_USEEDGENORMAL)
		{
			if (stdenormal)
					edgenormals_adress = MD[cf].enorm = new float[nbpoints * 3];
			else	edgenormals_adress = MD[cf].enorm = new float[totalglobaledgenumber * 3];
			GetEdgeNormals();
		}
	}	

	// compute blend factors between STEPS

	for (cf = 0; cf < (unsigned long)(nMDstep - 1); cf++)
	{
		// vertice
		datas = MD[cf].vertice;
		datad = MD[cf + 1].vertice;
		dst = MD[cf].blend = new float[nbpoints * 3];
		for (ce = 0; ce < (unsigned long)((nbpoints - 8) * 3); ce++)
			*dst++ = (*datad++) - (*datas++);

		// face normals
		if (requirement & nX_USEFACENORMAL)
		{
			datas = MD[cf].fnorm;
			datad = MD[cf + 1].fnorm;
			dst = MD[cf].bfnorm = new float[nbface * 3];
			for (ce = 0; ce < (nbface * 3); ce++)
				*dst++ = (*datad++) - (*datas++);
		}

		// edge normals
		if (requirement & nX_USEEDGENORMAL)
		{
			datas = MD[cf].enorm;
			datad = MD[cf + 1].enorm;

			if (stdenormal)
			{
				dst = MD[cf].benorm = new float[nbpoints * 3];
				for (ce = 0; ce < (unsigned long)(nbpoints * 3); ce++)
					*dst++ = (*datad++) - (*datas++);
			}
			else
			{
				dst = MD[cf].benorm = new float[totalglobaledgenumber * 3];
				for (ce = 0; ce < (totalglobaledgenumber * 3); ce++)
					*dst++ = (*datad++) - (*datas++);
			}
		}
	}	

	// we HAVE to replace 'edge_adress' with the original mesh datas
	edge_adress = oMesh;

	// okay reading
	delete (obuffer);
	wsprintf(str, "    Done... Read %d frame(s).", nMDstep);
	pnX->Verbose(str);
	return true;

}

void	nX_Entity::ApplyMDD()
{

	stepTC += scene->elapsedtickcount;

	while (stepTC > cMD[1].timestamp)
	{	
		cMD++;

		// loop MD
		if ( (cMD - MD) == (nMDstep - 1))
		{
			if (MDloop)
					cMD = MD;
			else	{	cMD--; MDbstop = true;	}
			stepTC = 0;
		}
	}

	// we reached end of steps with no looping...
	if (MDbstop)
	{
		edge_adress = cMD->vertice;
		facenormals_adress = cMD->fnorm;
		edgenormals_adress = cMD->enorm;
		return;
	}

	// blend MD steps directly over primary buffers
	unsigned long	ce;
	float			*s, *d, *b, t;
	t = (stepTC - cMD->timestamp) / (cMD[1].timestamp - cMD->timestamp);

	// vertice
	d = edge_adress = oMesh;
	s = cMD->vertice;
	b = cMD->blend;
	for (ce = 0; ce < (unsigned long)((nbpoints - 8) * 3); ce++)
		(*d++) = (*s++) + ((*b++) * t);

	// face normals
	if (cMD->bfnorm)
	{
		d = facenormals_adress = ofacenormals;
		s = cMD->fnorm;
		b = cMD->bfnorm;
		for (ce = 0; ce < (unsigned long)(nbface * 3); ce++)
			(*d++) = (*s++) + ((*b++) * t);
	}

	// edge normals
	if (cMD->benorm)
	{
		d = edgenormals_adress = oedgenormals;
		s = cMD->enorm;
		b = cMD->benorm;
		
		if (stdenormal)
			for (ce = 0; ce < (unsigned long)(nbpoints * 3); ce++)
				(*d++) = (*s++) + ((*b++) * t);
		else
			for (ce = 0; ce < (unsigned long)(totalglobaledgenumber * 3); ce++)
				(*d++) = (*s++) + ((*b++) * t);
	}

}


#endif