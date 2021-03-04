int
width_wintitle_hidden(Bar *bar, BarArg *a)
{
	return a->w;
}

int
draw_wintitle_hidden(Bar *bar, BarArg *a)
{
	drw_rect(drw, a->x, a->y, a->w, a->h, 1, 1);
	return calc_wintitle_hidden(bar, a->x, a->w, -1, flextitledraw, NULL, a);
}

int
click_wintitle_hidden(Bar *bar, Arg *arg, BarArg *a)
{
	if (calc_wintitle_hidden(bar, 0, a->w, a->x, flextitleclick, arg, a))
		return ClkWinTitle;
	return -1;
}

int
calc_wintitle_hidden(
	Bar *bar, int offx, int tabw, int passx,
	void(*tabfn)(Monitor *, Client *, int, int, int, int, Arg *arg, BarArg *barg),
	Arg *arg, BarArg *a
) {
	Monitor *m = bar->mon;
	Client *c;
	int clientsnhidden = 0;
	int groupactive = GRP_HIDDEN;

	for (c = m->clients; c; c = c->next) {
		if (!ISVISIBLE(c))
			continue;
		if (HIDDEN(c))
			clientsnhidden++;
	}

	if (!clientsnhidden)
		return 0;

	/* This avoids drawing a separator on the left hand side of the wintitle section if
	 * there is a border and the wintitle module rests at the left border. */
	if (bar->borderpx && a->x > bar->bx + bar->borderpx) {
		offx += flexwintitle_separator;
		tabw -= flexwintitle_separator;
	}

	/* This avoids drawing a separator on the right hand side of the wintitle section if
	 * there is a border and the wintitle module rests at the right border. */
	if (bar->borderpx && a->x + a->w < bar->bx + bar->bw - 2 * bar->borderpx)
		tabw -= flexwintitle_separator;

	if (bar->borderpx) {
		XSetForeground(drw->dpy, drw->gc, scheme[bar->scheme][ColBorder].pixel);
		XFillRectangle(drw->dpy, drw->drawable, drw->gc, a->x, a->y, a->w, a->h);
	}

	c = flextitledrawarea(m, m->clients, offx, tabw, clientsnhidden, SCHEMEFOR(GRP_HIDDEN), 0, 1, 0, passx, tabfn, arg, a);
	return 1;
}