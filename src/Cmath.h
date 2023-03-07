class	cM_Vector;
struct	fXYZ;
struct	dXYZ;

struct fXYZ
{
	float x, y, z;
	void	operator +(dXYZ *); void	operator -(dXYZ *); void	operator *(dXYZ *);	void	operator /(dXYZ *);
	void	operator +(fXYZ *);	void	operator -(fXYZ *);	void	operator *(fXYZ *);	void	operator /(fXYZ *);
	void operator +(cM_Vector *); void operator -(cM_Vector *); void operator *(cM_Vector *); void operator /(cM_Vector *);
	fXYZ(float, float, float);
	fXYZ();
};

struct dXYZ
{
	double x, y, z;
	void operator +(dXYZ *); void operator -(dXYZ *);	void operator *(dXYZ *); void operator /(dXYZ *);
	void operator +(fXYZ *); void operator -(fXYZ *);	void operator *(fXYZ *); void operator /(fXYZ *);
	void operator +(cM_Vector *); void operator -(cM_Vector *); void operator *(cM_Vector *); void operator /(cM_Vector *);
	dXYZ(double, double, double);
	dXYZ();
};

class cM_Vector
{

	public:
	double	x, y, z, l;								// geometry

	void	Normalize();							// Normalize the vector
	double	ComputeLength();						// Compute Length of the vector

	// Common useful operators
	void	operator +(cM_Vector *);				// Add two vectors
	void	operator /(cM_Vector *);				// Divide two vectors
	void	operator *(cM_Vector *);				// Multiply two vectors
	void	operator -(cM_Vector *);				// Substract two vectors
	void	operator +(dXYZ *);
	void	operator -(dXYZ *);
	void	operator *(dXYZ *);
	void	operator /(dXYZ *);
	void	operator +(fXYZ *);
	void	operator -(fXYZ *);
	void	operator *(fXYZ *);
	void	operator /(fXYZ *);
	void	operator =(cM_Vector *);

	// Constructors
	cM_Vector(double, double, double);
	cM_Vector();

};

class cM_Plane
{

	public:
	double	a, b, c, d;

	void	ComputeScalar(dXYZ *);					// Compute plane scalar component, requires point-in-poly
	void	ComputeScalar(fXYZ *);					// Compute plane scalar component, requires point-in-poly

	// Constructors
	cM_Plane(double, double, double, double);
	cM_Plane();

};

class cMath
{

	public:

	float	Modulo(float, float);					// Modulo a:b

	double	DotProd(cM_Vector *, cM_Vector *);		// Cosine between 2 vectors
	double	DotProd(cM_Vector *, dXYZ *);			// Cosine between 2 vectors
	double	DotProd(dXYZ *, cM_Vector *);			// Cosine between 2 vectors
	double	DotProd(dXYZ *, dXYZ *);				// Cosine between 2 vectors

	// w, u, v -> w = normal to the plane build by u and v
	void	CrossProd(cM_Vector *, cM_Vector *, cM_Vector *);

	/*
		cMath::RayIntersectPlane()
		--------------------------
		In: ray origin, ray direction, plane.
		Out: point of intersection (if any) in dXYZ.
		Return: true if intersect, false else.
	*/
	bool	RayIntersectPlane(dXYZ *, cM_Vector *, cM_Plane *, dXYZ *);

	/*
		cMath::RayIntersectPlane()
		--------------------------
		In: PlaneA, PlaneB.
		Return: true if PlaneA = PlaneB.
	*/
	bool	IdentityPlane(cM_Plane *, cM_Plane *);

	/*
		cMath::dPointToPlane()
		----------------------
		In: Plane, dXYZ.
		Return: Distance from point to plane.
	*/

	double	dPointToPlane(cM_Plane *, dXYZ *);

	/*
		cMath::BaryCoord()
		------------------
		Returns in *bcoord the barycentric coordinates of p in triangle {tri0, tri1, tri2}
	*/
	void	BaryCoord(dXYZ *, dXYZ *, dXYZ *, dXYZ *, dXYZ *);
};
