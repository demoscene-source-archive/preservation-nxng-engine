	#include	"nXng/nX_HeaderList.h"
	#include	"nXng/cColor.h"
	#include	"nXng/nXCol_FASTinlines.cpp"


void nX::ModelLight(nX_Entity *obj, nX_Surface *surf, float *c2d, float *pnorm, long bspec)
{

	nX_Entity	*light;

	long		c, dr, dg, db, si;
	float		lvx, lvy, lvz, lvl, ilvl;
	float		spotangle;
	long		coef, dot;

	// SET RGB TO SCENE AMBIENT LIGHTNING
	/////////////////////////////////////

	dr = long (cscene->iambientcolor.r);
	dg = long (cscene->iambientcolor.g);
	db = long (cscene->iambientcolor.b);
	si = 0;

	// PERFORM LIGHTNING FOR EACH LIGHT
	///////////////////////////////////

	for (c = 0; c < cscene->nblight; c++)
	{

		if ( (surf->lightspec & nX_RESTRICTLIGHT) && !surf->lightallowed[c] )
			goto skiplight;

		// get current light

		light = cscene->entityadress[c + cscene->firstlightno];

		switch (light->lighttype)
		{
			case 0:					// DISTANT

				dot = long
					 ((pnorm[0] * light->fmatrix[8] +
					   pnorm[1] * light->fmatrix[9] +
					   pnorm[2] * light->fmatrix[10]) * 256);
				if (dot <= 0)
					break;

				coef = dot * long(surf->diffuse * light->LightInt->values[0] * 256);
				dr += (coef * light->lightcolor.r) >> 16;
				dg += (coef * light->lightcolor.g) >> 16;
				db += (coef * light->lightcolor.b) >> 16;
				if (bspec)
					si += long (pow(float(dot) / 256, surf->glossiness) * surf->specular * light->LightInt->values[0] * 255) << 1;
			break;

			case 1:					// POINT

				lvx = c2d[0] - light->fmatrix[12];
				lvy = c2d[1] - light->fmatrix[13];
				lvz = c2d[2] - light->fmatrix[14];
				lvl = float( sqrt(lvx * lvx + lvy * lvy + lvz * lvz) );

				if (light->LightRange && lvl >= light->LightRange->values[0])
					break;

				ilvl = float(1 / lvl);
				dot = long ((lvx * pnorm[0] + lvy * pnorm[1] + lvz * pnorm[2]) * ilvl * 256);
				if (dot <= 0)
					break;

				if (light->LightRange)
				{
					coef = dot * long(surf->diffuse * ( 1 - lvl / light->LightRange->values[0] ) * light->LightInt->values[0] * 256);
					dr += (coef * light->lightcolor.r) >> 16;
					dg += (coef * light->lightcolor.g) >> 16;
					db += (coef * light->lightcolor.b) >> 16;
					if (bspec)
						si += long (pow(float(dot) / 256, surf->glossiness) * surf->specular * ( 1 - lvl/light->LightRange->values[0] ) * light->LightInt->values[0] * 255) << 1;
				}
				else
				{
					coef = dot * long(surf->diffuse * light->LightInt->values[0] * 256);
					dr += (coef * light->lightcolor.r) >> 16;
					dg += (coef * light->lightcolor.g) >> 16;
					db += (coef * light->lightcolor.b) >> 16;
					if (bspec)
						si += long (pow(float(dot) / 256, surf->glossiness) * surf->specular * light->LightInt->values[0] * 255) << 1;
				}

			break;

			case 2:					// SPOT

				lvx = c2d[0] - light->fmatrix[12];
				lvy = c2d[1] - light->fmatrix[13];
				lvz = c2d[2] - light->fmatrix[14];
				lvl = float( sqrt(lvx * lvx + lvy * lvy + lvz * lvz) );

				if (light->LightRange && lvl >= light->LightRange->values[0])
					break;

				ilvl = float(1 / lvl);
				dot = long((lvx * pnorm[0] + lvy * pnorm[1] + lvz * pnorm[2]) * ilvl * 256);
				if (dot <= 0)
					break;

				if (light->LightRange)
				{
					spotangle=(lvx * light->fmatrix[8] + lvy * light->fmatrix[9] + lvz * light->fmatrix[10]) * ilvl;

					if (spotangle > light->ConeAngle->values[0])
					{
						coef = dot * long(surf->diffuse * ( 1 - lvl/light->LightRange->values[0] ) * light->LightInt->values[0] * 256);
						dr += (coef * light->lightcolor.r) >> 16;
						dg += (coef * light->lightcolor.g) >> 16;
						db += (coef * light->lightcolor.b) >> 16;
						if (bspec)
							si += long (pow(float(dot) / 256, surf->glossiness) * surf->specular * ( 1 - lvl/light->LightRange->values[0] ) * light->LightInt->values[0] * 255) << 1;
					}
					else 
					{
						spotangle = light->ConeAngle->values[0] - spotangle;

						if (spotangle < light->EdgeAngle->values[0])
						{
							coef = dot * long(surf->diffuse * (1 - lvl/light->LightRange->values[0]) * light->LightInt->values[0] * (1 - spotangle / light->EdgeAngle->values[0]) * 256);
							dr += (coef * light->lightcolor.r) >> 16;
							dg += (coef * light->lightcolor.g) >> 16;
							db += (coef * light->lightcolor.b) >> 16;
							if (bspec)
								si += long (pow(float(dot) / 256, surf->glossiness) * surf->specular * (1 - lvl/light->LightRange->values[0]) * light->LightInt->values[0] * (1 - spotangle/light->EdgeAngle->values[0]) * 255) << 1;
						}
					}
				}
				else
				{
					spotangle = (lvx * light->fmatrix[8] + lvy * light->fmatrix[9] + lvz * light->fmatrix[10]) * ilvl;

					if (spotangle > light->ConeAngle->values[0])
					{
						coef = dot * long(surf->diffuse * light->LightInt->values[0] * 256);
						dr += (coef * light->lightcolor.r) >> 16;
						dg += (coef * light->lightcolor.g) >> 16;
						db += (coef * light->lightcolor.b) >> 16;
						if (bspec)
							si += long (pow(float(dot) / 256, surf->glossiness) * surf->specular * light->LightInt->values[0] * 255) << 1;
					}
					else 
					{
						spotangle = light->ConeAngle->values[0] - spotangle;
						if (spotangle < light->EdgeAngle->values[0])
						{
							coef = dot * long(surf->diffuse * light->LightInt->values[0] * (1 - spotangle / light->EdgeAngle->values[0]) * 256);
							dr += (coef * light->lightcolor.r) >> 16;
							dg += (coef * light->lightcolor.g) >> 16;
							db += (coef * light->lightcolor.b) >> 16;
							if (bspec)
								si += long (pow(float(dot) / 256, surf->glossiness) * surf->specular * light->LightInt->values[0] * (1 - spotangle/light->EdgeAngle->values[0])) << 1;
						}
					}
				}

			break;
		}

		skiplight:;
	}

	if (dr > 255) dr = 255;
	if (dg > 255) dg = 255;
	if (db > 255) db = 255;
	lnXcol(&diff, (dr << 16) + (dg << 8) + db);

	if (bspec)
	{
		if (si > 255) si = 255;
		lnXcol(&spec, (si << 16) + (si << 8) + si);
	}
}


float nX_Scene::GetLightningAtLocation(nX_Entity *light, double lX, double lY, double lZ, double lnX, double lnY, double lnZ, nX_Surface *surf, nXcolor *out)
{

	long		si;
	float		lvx, lvy, lvz, lvl, ilvl;
	float		spotangle, dot, coef;
	si = 0;
	out->r = 0; out->g = 0; out->b = 0;
	coef = 0;

	// PERFORM LIGHTNING FOR EACH LIGHT
	///////////////////////////////////

	switch (light->lighttype)
	{
		case 0:					// DISTANT
			dot=(lnX * light->hmatrix[8] +
				 lnY * light->hmatrix[9] +
				 lnZ * light->hmatrix[10]);
			if (dot <= 0)
				break;

			coef = dot * surf->diffuse * light->LightInt->values[0];
			if (coef > 1) coef = 1;
			out->r = long(coef * light->lightcolor.r);
			out->g = long(coef * light->lightcolor.g);
			out->b = long(coef * light->lightcolor.b);
		break;

		case 1:					// POINT
			lvx = lX - light->hmatrix[12];
			lvy = lY - light->hmatrix[13];
			lvz = lZ - light->hmatrix[14];
			lvl = float( sqrt(lvx * lvx + lvy * lvy + lvz * lvz) );

			if (light->LightRange && lvl >= light->LightRange->values[0])
				break;

			ilvl = float(1 / lvl);
			dot = (lvx * lnX + lvy * lnY + lvz * lnZ) * ilvl;
			if (dot <= 0)
				break;

			if (light->LightRange)
			{
				coef = dot * surf->diffuse * ( 1 - lvl / light->LightRange->values[0] ) * light->LightInt->values[0];
				if (coef > 1) coef = 1;
				out->r = long(coef * light->lightcolor.r);
				out->g = long(coef * light->lightcolor.g);
				out->b = long(coef * light->lightcolor.b);
			}
			else
			{
				coef = dot * surf->diffuse * light->LightInt->values[0];
				if (coef > 1) coef = 1;
				out->r = long(coef * light->lightcolor.r);
				out->g = long(coef * light->lightcolor.g);
				out->b = long(coef * light->lightcolor.b);
			}
		break;

		case 2:					// SPOT
			lvx = lX - light->hmatrix[12];
			lvy = lY - light->hmatrix[13];
			lvz = lZ - light->hmatrix[14];
			lvl = float( sqrt(lvx * lvx + lvy * lvy + lvz * lvz) );

			if (light->LightRange && lvl >= light->LightRange->values[0])
				break;

			ilvl=float(1 / lvl);
			dot=(lvx * lnX + lvy * lnY + lvz * lnZ) * ilvl;
			if (dot <= 0)
				break;

			if (light->LightRange)
			{
				spotangle=(lvx * light->hmatrix[8] + lvy * light->hmatrix[9] + lvz * light->hmatrix[10]) * ilvl;

				if (spotangle > light->ConeAngle->values[0])
				{
					coef = dot * surf->diffuse * ( 1 - lvl/light->LightRange->values[0] ) * light->LightInt->values[0];
					if (coef > 1) coef = 1;
					out->r = long(coef * light->lightcolor.r);
					out->g = long(coef * light->lightcolor.g);
					out->b = long(coef * light->lightcolor.b);
				}
				else 
				{
					spotangle = light->ConeAngle->values[0] - spotangle;

					if (spotangle < light->EdgeAngle->values[0])
					{
						coef = float (dot * surf->diffuse * (1 - lvl/light->LightRange->values[0]) * light->LightInt->values[0] * (1 - spotangle / light->EdgeAngle->values[0]));
						if (coef > 1) coef = 1;
						out->r = long(coef * light->lightcolor.r);
						out->g = long(coef * light->lightcolor.g);
						out->b = long(coef * light->lightcolor.b);
					}
				}
			}
			else
			{
				spotangle = (lvx * light->hmatrix[8] + lvy * light->hmatrix[9] + lvz * light->hmatrix[10]) * ilvl;

				if (spotangle > light->ConeAngle->values[0])
				{
					coef = dot * surf->diffuse * light->LightInt->values[0];
					if (coef > 1) coef = 1;
					out->r = long(coef * light->lightcolor.r);
					out->g = long(coef * light->lightcolor.g);
					out->b = long(coef * light->lightcolor.b);
				}
				else 
				{
					spotangle = light->ConeAngle->values[0] - spotangle;
					if (spotangle < light->EdgeAngle->values[0])
					{
						coef = dot * surf->diffuse * light->LightInt->values[0] * (1 - spotangle / light->EdgeAngle->values[0]);
						if (coef > 1) coef = 1;
						out->r = long(coef * light->lightcolor.r);
						out->g = long(coef * light->lightcolor.g);
						out->b = long(coef * light->lightcolor.b);
					}
				}
			}
		break;
	}
	return coef;
}
