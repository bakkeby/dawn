static int width_wintitle_floating(Bar *bar, BarArg *a);
static int draw_wintitle_floating(Bar *bar, BarArg *a);
static int click_wintitle_floating(Bar *bar, Arg *arg, BarArg *a);
static int calc_wintitle_floating(
	Bar *bar, int offx, int tabw, int passx,
	void(*tabfn)(Monitor *, Client *, int, int, int, int, Arg *arg, BarArg *barg),
	Arg *arg, BarArg *barg
);
