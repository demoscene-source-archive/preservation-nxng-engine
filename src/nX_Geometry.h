	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		nXng Geometry header file

	----------------------------------------------------------------------------*/


		#define	nX_MAXTEXLEVEL				4				// # UV/vtx
		#define	nX_ZCUTFRONTIER				1.0
		#define	nX_MINEDGEPOLY				3
		#define	nX_MAXEDGEPOLY				245				// some edges left for clipping... (upto 255)

		#define digitPI						3.14159265359
		#define	qsinBC						14
		#define	sincosREV					(1 << qsinBC)
		#define	sincosMOD					(sincosREV - 1)

		#define	CRSC						( float(sincosREV) / (2 * digitPI) )
		#define	CDSC						( float(sincosREV) / 360.0f )
		#define	radsincos(a)				( long(CRSC * a) & sincosMOD )
		#define	degsincos(a)				( long(CDSC * a) & sincosMOD )
		#define	modsincos(a)				( long(a) & sincosMOD )

		#define	deg2rad(a)					( a * digitPI / 180.0f )
		#define	rad2deg(a)					( a * 180.0f / digitPI )


struct	nXcolor
{
	unsigned char		b, g, r, a;
};

struct	nX_vertex
{
	float				x, y, z;	
	nXcolor				diff, spec;
	float				u[nX_MAXTEXLEVEL], v[nX_MAXTEXLEVEL];
};

struct	nX_lightvertex
{
	float				x, y, z;
	float				nx, ny, nz;
};

struct nX_reliure
{
	unsigned char		nedge;
	nX_Surface			*surf;
	unsigned long		*index;
};

typedef	nX_reliure *	pnX_reliure;

struct nX_LightMapInfo
{
	unsigned char		lightmapped;				// is poly lightmapped?
	nX_TexMap			*lmap;						// if the poly is to be lightmapped then it will be by this one
	nXcolor				shade;						// if there is no texmap then lightshade it with this!
};

struct	nX_face
{
	unsigned char		nedge;						// # of vertice in face
	nX_Entity			*caster;					// the entity that casted this face
	nX_Surface			*surf;						// link to the surface used
	float				alpha;						// component for the face
	nX_vertex			*vertice;					// adress of vertex array
	nXcolor				diff, spec;					// component for the face
	nX_LightMapInfo		*lmap;						// lightmap infos for the face
	unsigned short		id;							// the scene the poly belongs to
};

		// TSP structure
		#define			TSPBEHIND			0
		#define			TSPBETWEEN			1
		#define			TSPINFRONT			2

struct nX_TSPpoly
{
	nX_reliure			*face;							// entity face
	unsigned short		iface;							// # of face in entity
	cM_Plane			p;								// polygon plane
};

struct nX_TSPnode
{
	nX_TSPpoly			*poly;
	nX_TSPnode			*back, *on, *front;
};

// SORTING STRUCTURE SECTION --------------------------------------------------
///////////////////////////////////////////////////////////////////////////////


struct	RadixNode
{
	unsigned long		z;
	unsigned long		faceindex;
	nX_Scene			*scene;
};


// GEOMETRY EFFECTORS STRUCTURES SECTION --------------------------------------
// Bones, Morph-Gizmo and Inverse Kinematic
///////////////////////////////////////////////////////////////////////////////

struct nX_Gizmo
{

	char		*path;

	// Motion infos...
	float		offset;
	float		fps;
	float		speedadjust;
	float		firstframe;
	float		lastframe;

	// Motion vars...
	float		frame;
	long		endbehavior;
	long		nbKF;
	GizmoKF		*KF;

	// Xyd morphing infos...
	long		nbtarget;
	float		morphamount[nX_MAXGIZMOTARGETPEROBJECT];
	float		*xyzmorphdeltas[nX_MAXGIZMOTARGETPEROBJECT];
	float		*nedmorphdeltas[nX_MAXGIZMOTARGETPEROBJECT];
	float		*nfamorphdeltas[nX_MAXGIZMOTARGETPEROBJECT];

};

struct	IK_Node
{
	nX_Entity	*IKsrc;				// this entity is an IK end effector...
	nX_Entity	*top;
	IK_Node		*LLnext;
};


