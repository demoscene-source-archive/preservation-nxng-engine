	/*----------------------------------------------------------------------------

		nXv9 by xBaRr/Syndrome 2000
			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
			All rights reserved 2000.

		Part of nXng.
		mailto://xbarr@pingoo.com
		http://xbarr.planet-d.net

		nXng GUI header file

	----------------------------------------------------------------------------*/


	#define		GUI_NULL		0
	#define		GUI_QUIT		1
	#define		GUI_REPLAY		2

struct	nX_TVitem
{
	bool	sequence;			// new sequence to begin?
	char	*txt;
	bool	lws;
};

class	nX_GUI
{

	public:
	HWND			hWnd, phWnd;
	HINSTANCE		hInstance;
	HACCEL			Accel;

	HBRUSH			bgBrush;
	HBRUSH			lbbgBrush;
	HTREEITEM		htRoot, tvCur, tvPar;
	LPNMTREEVIEW	nTV;

	char			*basedir;

	void			CreateGUI(HINSTANCE, HWND);
	void			DestroyGUI();
	unsigned char	Pool(), command;
	void			InitItemRetrieval();
	bool			PoolItemRetrieval(nX_TVitem *);

	bool			LoadTree(char *);
	bool			SaveTree(char *);

	bool			seqb, dragging;
	HTREEITEM		xpRoot, xpChild;
	HIMAGELIST		hIL, drgIL;
	int				hILWS, hILXM, hISEQ, hIXMP;

	void			SetConsole(nX_Console *);
	nX_Console		*console;
	ScreenMN		*Screen;

	nX_GUI::nX_GUI();
	nX_GUI::~nX_GUI();

};