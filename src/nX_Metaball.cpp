// -------------------------------
// nX METABALL implementation
// 01/2000 xBaRr/Syndrome
// Code is issued from POV i think
//
// Parametrable tesselation
// -------------------------------


	#include	"nX_HeaderList.h"
	#include	"nXng/nX_MetaLUT.h"


// Render the whole metagrid in a scene
// ------------------------------------

void	nX_metagrid::Render(float isoval)
{
	long	rx, ry, rz;

	// tesellate each cell in grid
	for (rz = 0; rz < resz - 1; rz++)
		for (ry = 0; ry < resy - 1; ry++)
			for (rx = 0; rx < resx - 1; rx++)
			{
				GetRenderCell(rx, ry, rz);
				rCell.Render(pnX, isoval);
			}
}

// Build a metagrid
// ----------------

void	nX_metagrid::Setup(long dx, long dy, long dz, float u)
{
	// setup the grid
	resx = dx;
	resy = dy;
	resz = dz;
	unit = u;

	// allocate field buffer
	area = dx * dy;
	field = new float[area * dz];
	Reset();
}

// Reset force field in metagrid
// -----------------------------

void	nX_metagrid::Reset()
{
	memset(field, 0, area * resz * sizeof(float));
}

// Get temporary render cell
// -------------------------

void	nX_metagrid::GetRenderCell(long px, long py, long pz)
{
	// set cell coordinates
	rCell.p[0].x = px * unit; rCell.p[0].y = py * unit; rCell.p[0].z = pz * unit;
	rCell.p[1].x = (px + 1) * unit; rCell.p[1].y = py * unit; rCell.p[1].z = pz * unit;
	rCell.p[2].x = px * unit; rCell.p[2].y = (py + 1) * unit; rCell.p[2].z = pz * unit;
	rCell.p[3].x = (px + 1) * unit; rCell.p[3].y = (py + 1) * unit; rCell.p[3].z = pz * unit;
	rCell.p[4].x = px * unit; rCell.p[4].y = py * unit; rCell.p[4].z = (pz + 1) * unit;
	rCell.p[5].x = (px + 1) * unit; rCell.p[5].y = py * unit; rCell.p[5].z = (pz + 1) * unit;
	rCell.p[6].x = px * unit; rCell.p[6].y = (py + 1) * unit; rCell.p[6].z = (pz + 1) * unit;
	rCell.p[7].x = (px + 1) * unit; rCell.p[7].y = (py + 1) * unit; rCell.p[7].z = (pz + 1) * unit;

	// set cell field values
	rCell.val[0] = field[px + py * resx + pz * area];
	rCell.val[1] = field[px + 1 + py * resx + pz * area];
	rCell.val[2] = field[px + (py + 1) * resx + pz * area];
	rCell.val[3] = field[px + 1 + (py + 1) * resx + pz * area];
	rCell.val[4] = field[px + py * resx + (pz + 1) * area];
	rCell.val[5] = field[px + 1 + py * resx + (pz + 1) * area];
	rCell.val[6] = field[px + (py + 1) * resx + (pz + 1) * area];
	rCell.val[7] = field[px + 1 + (py + 1) * resx + (pz + 1) * area];
}

// Linearly interpolate the position where an isosurface cuts
// an edge between two vertices, each with their own scalar value
fXYZ	inline	VertexInterp(float isolevel, fXYZ p1, fXYZ p2, float valp1, float valp2)
{
	float	mu;
	fXYZ		p;

	mu = (isolevel - valp1) / (valp2 - valp1);
	p.x = p1.x + mu * (p2.x - p1.x);
	p.y = p1.y + mu * (p2.y - p1.y);
	p.z = p1.z + mu * (p2.z - p1.z);

	return p;
}

//	Given a grid cell and an isolevel, calculate the triangular
//	facets required to represent the isosurface through the cell.
//	Return the number of triangular facets, the array "triangles"
//	will be loaded up with the vertices at most 5 triangular facets.
//	0 will be returned if the grid cell is either totally above
//	of totally below the isolevel.

void	nX_Cell::Render(nX *pnX, float isolevel)
{
	long	i;//, ntriang;
	long	cubeindex;
	fXYZ	vertlist[12];
	fXYZ	*pv0, *pv1, *pv2;

	// Determine the index into the edge table which tells us which vertices are inside of the surface
	cubeindex = 0;
	if (val[0] < isolevel) cubeindex |= 1;
	if (val[1] < isolevel) cubeindex |= 2;
	if (val[2] < isolevel) cubeindex |= 4;
	if (val[3] < isolevel) cubeindex |= 8;
	if (val[4] < isolevel) cubeindex |= 16;
	if (val[5] < isolevel) cubeindex |= 32;
	if (val[6] < isolevel) cubeindex |= 64;
	if (val[7] < isolevel) cubeindex |= 128;

	// cube is entirely in/out of iso-surface
	if (!edgeTable[cubeindex])
		return;

	// find the vertices where the surface intersects the cube
	if (edgeTable[cubeindex] & 1)
		vertlist[0] = VertexInterp(isolevel, p[0], p[1], val[0], val[1]);
	if (edgeTable[cubeindex] & 2)
		vertlist[1] = VertexInterp(isolevel, p[1], p[2], val[1], val[2]);
	if (edgeTable[cubeindex] & 4)
		vertlist[2] = VertexInterp(isolevel, p[2], p[3], val[2], val[3]);
	if (edgeTable[cubeindex] & 8)
		vertlist[3] = VertexInterp(isolevel, p[3], p[0], val[3], val[0]);
	if (edgeTable[cubeindex] & 16)
		vertlist[4] = VertexInterp(isolevel, p[4], p[5], val[4], val[5]);
	if (edgeTable[cubeindex] & 32)
		vertlist[5] = VertexInterp(isolevel, p[5], p[6], val[5], val[6]);
	if (edgeTable[cubeindex] & 64)
		vertlist[6] = VertexInterp(isolevel, p[6], p[7], val[6], val[7]);
	if (edgeTable[cubeindex] & 128)
		vertlist[7] = VertexInterp(isolevel, p[7], p[4], val[7], val[4]);
	if (edgeTable[cubeindex] & 256)
		vertlist[8] = VertexInterp(isolevel, p[0], p[4], val[0], val[4]);
	if (edgeTable[cubeindex] & 512)
		vertlist[9] = VertexInterp(isolevel, p[1], p[5], val[1], val[5]);
	if (edgeTable[cubeindex] & 1024)
		vertlist[10] = VertexInterp(isolevel, p[2], p[6], val[2], val[6]);
	if (edgeTable[cubeindex] & 2048)
		vertlist[11] = VertexInterp(isolevel, p[3], p[7], val[3], val[7]);

	// Create the triangle and add it to face table!
//	ntriang = 0;

	for (i = 0; triTable[cubeindex][i] != -1; i += 3)
	{
		pv0 = &vertlist[triTable[cubeindex][i + 0]];
		pv1 = &vertlist[triTable[cubeindex][i + 1]];
		pv2 = &vertlist[triTable[cubeindex][i + 2]];

		pnX->Trigone(pv0->x, pv0->y, pv0->z, pv1->x, pv1->y, pv1->z, pv2->x, pv2->y, pv2->z, pnX->BlankSurface, 0);
	}
}