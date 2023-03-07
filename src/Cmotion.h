	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		nXng Lightwave Motion/Envelop header file

	----------------------------------------------------------------------------*/

// MOTION STRUCTURES SECTION --------------------------------------------------
///////////////////////////////////////////////////////////////////////////////


		#define nX_MAXGIZMOTARGETPEROBJECT		32


typedef float	ChanVec[nX_MAXGIZMOTARGETPEROBJECT];

struct GizmoKF
{
	float	cv[nX_MAXGIZMOTARGETPEROBJECT];
	float	tens, cont, bias;
	int		linear;
	int		step;
};

struct KF
{
	float	cv[9];
	float	tens, cont, bias;
	int		linear;
	int		step;
};

struct GizmoMotion
{
	GizmoKF	*keylist;
	int		keys, steps;
};

struct Motion
{
	KF		*keylist;
	int		keys, steps;
};

struct KFfillerfdbck
{
	KF				*KFadress;
	unsigned short	nbKF;
	float			highframe;
	float			frameoffset;
	unsigned char	endbehavior;
};

class nX_Envelop
{
	public:
	float			values[9];

					// Envelop MOTION
					// --------------

	bool			useenvelop;
	KF				*KFadrss;
	unsigned short	nbKF;
	float			lf;
	unsigned char	endb;
	float			offset;
	unsigned char	nbchan;

					// Envelop FUNCTIONS
					// -----------------

	void			GetValues(float);				// Return channel value for a given frame
	char			*LoadLWSMotion(char *);	// Load an envelop from plain ASCII LWS file in memory

					// Construction/Destruction
	nX_Envelop();
	~nX_Envelop();
};

class MotionEngine
{
	public:;
	Motion			MT;
	GizmoMotion		gMT;
	ChanVec			Hermite;

	void	MotionCalcStep(Motion *, ChanVec, float, unsigned char);
	void	GizmoCalcStep(GizmoMotion *, ChanVec, float, unsigned char);
};
