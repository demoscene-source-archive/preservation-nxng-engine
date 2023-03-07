// -------------------------------
// nX METABALL implementation
// 01/2000 xBaRr/Syndrome
// Code is issued from POV i think
//
// Parametrable tesselation
// -------------------------------

struct TRIANGLE
{
	fXYZ	p[3];
};

class	nX_Cell
{
	public:
	fXYZ	p[8];
	float	val[8];
	void	Render(nX *, float);
};

class	nX_metaball
{
	public:
	float		radius;
	nX_Entity	*pEnt;
};

class	nX_metagrid
{
	public:
	// the force field values
	float	*field;
	// temp cell used for tesselation
	nX_Cell	rCell;
	// the engine the grid belong to
	nX		*pnX;

	// grid resolution
	long	resx, resy, resz, area;
	// grid unit (in meters)
	float	unit;

	// Allocate memory to store force field values	
	void	Setup(long, long, long, float);
	void	GetRenderCell(long, long, long);
	// Reset force field
	void	Reset();
	// Add metaball and compute the new force field	
	void	AddMetaball(nX_metaball *);
	// tesellate each cell in grid
	void	Render(float);
};