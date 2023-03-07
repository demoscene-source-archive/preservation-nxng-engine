// SPARK EMITERS STRUCTURES SECTION -------------------------------------------
///////////////////////////////////////////////////////////////////////////////

struct	nX_Spark
{
	float	x, y, z;
	float	vx, vy, vz;
	float	deathwish, delay;
};

struct nX_SparkEmiter
{
	unsigned char		type;
	unsigned short		count;
	nX_Spark			*sparklist;
};