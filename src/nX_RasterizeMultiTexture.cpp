	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		nXng MultiTexture capable rasterizer implementation file

	----------------------------------------------------------------------------*/


	#include	"nXng/nX_HeaderList.h"
	#include	"nXng/nXCol_FASTinlines.cpp"
	static		D3Dvtx	dVtx[nX_MAXEDGEPOLY];


void	nX::RasterizeMultiTexture()
{

	nX_Surface		*surface, *oldsurface;
	nX_vertex		*pvertex;

	long			c, c2, nedge;
	float			oldalpha = float(-999.55);

	long			D3D_texlvlc, D3D_alphastage;
	bool			istransparent;

	float	fogmin;
	float	fogmax;

	long	FLAT;
	long	MAPPED, EVMAP, WIREFRAME;//, BUMP;
	long	LIGHTSOURCE, GOURAUD, LIGHTMAP, LIGHTSHADE;
	long	ALPHATEX;
	long	ADDITIVE;
	long	sum;

	// DIRECT3D RENDERER --------------------------------------------------
	///////////////////////////////////////////////////////////////////////

	oldsurface = NULL;

	if (buildedface > 0)
	{

		// RADIX (byte version) sort
		RadixSortFaceList();
		if (Draw_Forward)
				pbuffersorted = buffer2sort;
		else	pbuffersorted = buffer2sort + (buildedface - 1);

		// dynamically adjust fog range
		if (childusingfog)
		{
			fogmin = cscene->FogMinDist->values[0] / (largestZ + 1);
			fogmax = cscene->FogMaxDist->values[0] / (largestZ + 1);
			screen->pD3DDevice->SetRenderState(D3DRENDERSTATE_FOGSTART, *(DWORD *)(&fogmin));
			screen->pD3DDevice->SetRenderState(D3DRENDERSTATE_FOGEND, *(DWORD *)(&fogmax));
		}

		for (c = 0; c < buildedface; c++)
		{

			pfacelist = facelist + pbuffersorted[0].faceindex;
			if (Draw_Forward)
					pbuffersorted++;
			else	pbuffersorted--;


			// nX-CAPS to D3D attributes ------------------------------
			///////////////////////////////////////////////////////////


			nedge = pfacelist->nedge;
			surface = pfacelist->surf;
			pvertex = pfacelist->vertice;

			FLAT = surface->CAPS & nX_FLAT;
			ADDITIVE = surface->CAPS & nX_ADDITIVE;
			MAPPED = surface->CAPS & nX_MAPPED;
			EVMAP = surface->CAPS & nX_EVMAP;
			ALPHATEX = surface->CAPS & nX_ALPHATEX;
			LIGHTSOURCE = surface->CAPS & nX_LIGHTSOURCE;
			GOURAUD = surface->CAPS & nX_GOURAUD;
			WIREFRAME = surface->CAPS & nX_WIREFRAME;

			// LIGHTMAP RENDERSTATES

			if (pfacelist->lmap)
			{
				switch (pfacelist->lmap->lightmapped)
				{
					case 0:
						LIGHTMAP = false;
						LIGHTSOURCE = false;
						LIGHTSHADE = false;
						GOURAUD = false;
						break;
					case 1:
						LIGHTMAP = 1;
						LIGHTSOURCE = false;
						GOURAUD = false;
						break;
					case 2:
						LIGHTMAP = 2;
						LIGHTSHADE = false;
						break;
					case 3:
						LIGHTMAP = false;
						LIGHTSOURCE = true;
						GOURAUD = false;
						LIGHTSHADE = true;
						break;
				}
				oldsurface = NULL;
			}
			else
			{
				LIGHTMAP = false;
				LIGHTSHADE = false;
			}


			// SETUP D3D RENDERSTATE ----------------------------------
			///////////////////////////////////////////////////////////

			// undefine this FIRST if bug!
			#define	OPTIMRSS

			#ifdef	OPTIMRSS
			if (surface == oldsurface && pfacelist->alpha == oldalpha)
				goto setD3Dok;
			#endif

			// GENERAL ALPHA ------------------------------------------
			///////////////////////////////////////////////////////////

			D3D_alphastage = 0;

			if (ADDITIVE || pfacelist->alpha || ALPHATEX || (surface->xtdCAPS & nX_MERGEDALPHA) )
			{
				if (ALPHATEX || pfacelist->alpha || (surface->xtdCAPS & nX_MERGEDALPHA) )
						screen->pD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_INVSRCALPHA);
				else	screen->pD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);

				if (ADDITIVE)
						screen->pD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
				else	screen->pD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_SRCALPHA);

				if (surface->xtdCAPS & nX_MERGEDALPHA)
				{
					screen->pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
					screen->pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
				}
				else if (pfacelist->alpha)
				{
					screen->pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
					screen->pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
				}
				else
					screen->pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

				screen->pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, true);

				istransparent = true;
				D3D_alphastage++;
			}
			else
			{
				screen->pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, false);
				screen->pD3DDevice->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_GREATEREQUAL);
				if (childusingfog)
					screen->pD3DDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, true);
				istransparent = false;
			}

			// exclude polygon from fog!
			if (childusingfog)
			{
				if (pfacelist->caster && pfacelist->caster->unaffectedbyfog || ADDITIVE)
					screen->pD3DDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, false);
				else
					screen->pD3DDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, true);
			}

			#ifdef	OPTIMRSS
			if (surface == oldsurface)
				goto setD3Dok;
			#endif

			// SHADERS SETUP ------------------------------------------
			///////////////////////////////////////////////////////////

			if (GOURAUD)
				screen->pD3DDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
			else
				screen->pD3DDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_FLAT);
			screen->pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			screen->pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);

			// TEXTURES SETUP -----------------------------------------
			///////////////////////////////////////////////////////////

			D3D_texlvlc = 0;

			if (MAPPED)
			{
				screen->pD3DDevice->SetTextureStageState(D3D_texlvlc, D3DTSS_TEXCOORDINDEX, nXML_COLOR);

				// Wrapping...
				if (surface->mapuwrap[nXML_COLOR] == 2 || surface->mapvwrap[nXML_COLOR] == 2)
					screen->pD3DDevice->SetTextureStageState(D3D_texlvlc, D3DTSS_ADDRESS, D3DTADDRESS_WRAP);
				else
					screen->pD3DDevice->SetTextureStageState(D3D_texlvlc, D3DTSS_ADDRESS, D3DTADDRESS_CLAMP);

				screen->pD3DDevice->SetTextureStageState(D3D_texlvlc, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				screen->pD3DDevice->SetTexture(D3D_texlvlc, surface->map[nXML_COLOR]->sD3D);

				if (GOURAUD || LIGHTSOURCE)
					screen->pD3DDevice->SetTextureStageState(D3D_texlvlc, D3DTSS_COLOROP, D3DTOP_MODULATE);
				else
					screen->pD3DDevice->SetTextureStageState(D3D_texlvlc, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

				D3D_texlvlc++;
			}

			// EVMAP

			if (EVMAP)
			{
				screen->pD3DDevice->SetTextureStageState(D3D_texlvlc, D3DTSS_TEXCOORDINDEX, nXML_EVMAP);
				screen->pD3DDevice->SetTextureStageState(D3D_texlvlc, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				screen->pD3DDevice->SetTexture(D3D_texlvlc, surface->map[nXML_EVMAP]->sD3D);
				screen->pD3DDevice->SetTextureStageState(D3D_texlvlc, D3DTSS_ADDRESS, D3DTADDRESS_CLAMP);

				if (!D3D_texlvlc)
				{
					if (GOURAUD || LIGHTSOURCE)
						screen->pD3DDevice->SetTextureStageState(D3D_texlvlc, D3DTSS_COLOROP, D3DTOP_MODULATE);
					else
						screen->pD3DDevice->SetTextureStageState(D3D_texlvlc, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
				}
				else
				{
					screen->pD3DDevice->SetTextureStageState(D3D_texlvlc, D3DTSS_COLORARG2, D3DTA_CURRENT);
					if (surface->xtdCAPS & nX_ADDEVMAP)
						screen->pD3DDevice->SetTextureStageState(D3D_texlvlc, D3DTSS_COLOROP, D3DTOP_ADD);
					else
						screen->pD3DDevice->SetTextureStageState(D3D_texlvlc, D3DTSS_COLOROP, D3DTOP_MODULATE);
				}						

				D3D_texlvlc++;
			}

			// LIGHTMAP

			if (LIGHTMAP)
			{
				screen->pD3DDevice->SetTextureStageState(D3D_texlvlc, D3DTSS_TEXCOORDINDEX, nXML_LIGHTMAP);
				screen->pD3DDevice->SetTextureStageState(D3D_texlvlc, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				screen->pD3DDevice->SetTexture(D3D_texlvlc, pfacelist->lmap->lmap->sD3D);
				screen->pD3DDevice->SetTextureStageState(D3D_texlvlc, D3DTSS_ADDRESS, D3DTADDRESS_CLAMP);

				if (D3D_texlvlc)
					screen->pD3DDevice->SetTextureStageState(D3D_texlvlc, D3DTSS_COLORARG2, D3DTA_CURRENT);
				screen->pD3DDevice->SetTextureStageState(D3D_texlvlc, D3DTSS_COLOROP, D3DTOP_MODULATE);

				D3D_texlvlc++;
			}

			// LAST STAGE! alpha tex & specular -----------------------
			///////////////////////////////////////////////////////////

			if (ALPHATEX)
			{
				screen->pD3DDevice->SetTextureStageState(D3D_texlvlc, D3DTSS_TEXCOORDINDEX, nXML_TRANSPARENCY);
				screen->pD3DDevice->SetTexture(D3D_texlvlc, surface->map[nXML_TRANSPARENCY]->sD3D);
				if (surface->mapuwrap[nXML_TRANSPARENCY] == 2 || surface->mapvwrap[nXML_TRANSPARENCY] == 2)
					screen->pD3DDevice->SetTextureStageState(D3D_texlvlc, D3DTSS_ADDRESS, D3DTADDRESS_WRAP);
				else
					screen->pD3DDevice->SetTextureStageState(D3D_texlvlc, D3DTSS_ADDRESS, D3DTADDRESS_CLAMP);

				screen->pD3DDevice->SetTextureStageState(D3D_texlvlc, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
				screen->pD3DDevice->SetTextureStageState(D3D_texlvlc, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

				if (D3D_texlvlc)
				{
					screen->pD3DDevice->SetTextureStageState(D3D_texlvlc, D3DTSS_COLORARG1, D3DTA_CURRENT);
					screen->pD3DDevice->SetTextureStageState(D3D_texlvlc, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
				}
				D3D_texlvlc++;
				D3D_alphastage++;
			}

			// disable next stages
			screen->pD3DDevice->SetTextureStageState(D3D_alphastage, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
			screen->pD3DDevice->SetTextureStageState(D3D_texlvlc, D3DTSS_COLOROP, D3DTOP_DISABLE);

			// Z-BUFFER BEHAVIOR --------------------------------------
			///////////////////////////////////////////////////////////

			if (istransparent || surface->xtdCAPS & nX_NOZWRITE)
					screen->pD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, false);
			else	screen->pD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, true);
			if (surface->xtdCAPS & nX_NOZBUFFER)
					screen->pD3DDevice->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_ALWAYS);
			else	screen->pD3DDevice->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_GREATEREQUAL);

setD3Dok:;
			// FILL D3D VERTEX LIST -----------------------------------
			///////////////////////////////////////////////////////////

			for (c2 = 0; c2 < nedge; c2++)
			{
				dVtx[c2].x = pvertex->x;
				dVtx[c2].y = pvertex->y;
				dVtx[c2].z = 1.0 / pvertex->z;
				dVtx[c2].rhw = dVtx[c2].z;
				dVtx[c2].diff = uchar(pfacelist->alpha * 255) << 24;

				if (LIGHTMAP == 1)		// we have 3 shadings...
					dVtx[c2].diff += nXcoll(&surface->color);
				else if (LIGHTSHADE)
					dVtx[c2].diff += nXcoll(&pfacelist->lmap->shade);
				else if (GOURAUD)
					dVtx[c2].diff += nXcoll(&pvertex->diff);
				else if (LIGHTSOURCE)
					dVtx[c2].diff +=  nXcoll(&pfacelist->diff);
				else
				{
					if (MAPPED || EVMAP)
							dVtx[c2].diff += 0xffffff;
					else	dVtx[c2].diff += nXcoll(&surface->color);
				}

				if (MAPPED)
				{
					dVtx[c2].u0 = pvertex->u[nXML_COLOR];
					dVtx[c2].v0 = pvertex->v[nXML_COLOR];
				}
				if (EVMAP)
				{
					dVtx[c2].u1 = pvertex->u[nXML_EVMAP];
					dVtx[c2].v1 = pvertex->v[nXML_EVMAP];
				}
				if (ALPHATEX)
				{
					dVtx[c2].u2 = pvertex->u[nXML_TRANSPARENCY];
					dVtx[c2].v2 = pvertex->v[nXML_TRANSPARENCY];
				}
				if (LIGHTMAP)
				{
					dVtx[c2].u3 = pvertex->u[nXML_LIGHTMAP];
					dVtx[c2].v3 = pvertex->v[nXML_LIGHTMAP];
				}

				pvertex++;
			}

			// RENDER -------------------------------------------------
			///////////////////////////////////////////////////////////

			oldsurface = surface;
			oldalpha = pfacelist->alpha;

			// DRAW THAT THINGU...
			if (WIREFRAME)
			{
				screen->pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX4, dVtx, nedge, 0);
				for (c2 = 0; c2 < nedge; c2++)	dVtx[c2].y += 0.5f;
				screen->pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX4, dVtx, nedge, 0);
				for (c2 = 0; c2 < nedge; c2++)	dVtx[c2].x += 0.5f;
				screen->pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX4, dVtx, nedge, 0);
				for (c2 = 0; c2 < nedge; c2++)	dVtx[c2].y -= 0.5f;
				screen->pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX4, dVtx, nedge, 0);
			}
			else	screen->pD3DDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX4, dVtx, nedge,	0);
		}

		// SPECULAR PASS ---------------------------------------------------------
		// We perform this in two pass to avoid stupid changes in render states...
		//////////////////////////////////////////////////////////////////////////

		pbuffersorted = buffer2sort + (buildedface - 1);

		screen->pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, true);
		screen->pD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, false);
		screen->pD3DDevice->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_GREATEREQUAL);

		screen->pD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
		screen->pD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
		screen->pD3DDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
		screen->pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		screen->pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
		screen->pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		screen->pD3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

		for (c = 0; c < buildedface; c++)
		{
			pfacelist = facelist + pbuffersorted[0].faceindex;
			pbuffersorted--;

			// nX-CAPS to D3D attributes
			nedge = pfacelist->nedge;
			surface = pfacelist->surf;
			pvertex = pfacelist->vertice;

			if (surface->xtdCAPS & nX_SPECULAR)
			{
				sum = 0;
				for (c2 = 0; c2 < nedge; c2++)
					sum += (pvertex + c2)->spec.g;
				if (sum > 0)
				{
					for (c2 = 0; c2 < nedge; c2++)
					{
						dVtx[c2].x = pvertex->x;
						dVtx[c2].y = pvertex->y;
						dVtx[c2].z = 1.0 / pvertex->z;
						dVtx[c2].rhw = dVtx[c2].z;
						dVtx[c2].diff = (uchar(pfacelist->alpha * 255) << 24) + nXcoll(&pvertex->spec);
						pvertex++;
					}
					screen->pD3DDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX4, dVtx, nedge,	0);
				}
			}
		}
		screen->pD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, false);
		screen->pD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, true);
		oldsurface = NULL;	// ensure next Rasterize() will reset renderstates!
	}

	// rasterizing is done
	screen->pD3DDevice->Clear(NULL, NULL, D3DCLEAR_ZBUFFER, 0, 0., 0);

	if (screen->bUseFrameReport)
		DisplayFrameReport();
	screen->DisplayTempMsg();

	// scene face/point/sort buffers
	ResetBuffers();

}