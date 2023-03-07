	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		nXng Console header file

	----------------------------------------------------------------------------*/


class	nX_Console
{

	public:
	HWND		hWnd, phWnd;
	HINSTANCE	hInstance;
	bool		active;

	void		CreateConsole(HINSTANCE, HWND);
	void		DestroyConsole();
	void		Print(char *);
	void		PrintOver(char *);
	void		Message();
	void		SetProgress(char);
	void		Clear();

	nX_Console::nX_Console();
	nX_Console::~nX_Console();

};