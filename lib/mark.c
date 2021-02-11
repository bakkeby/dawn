static int num_marked = 0;
static int keepmarks = 0;

Client *
nextmarked(Client *prev, Client *def)
{
	if (!num_marked)
		return def;

	Client *c = NULL;
	Monitor *m;

	for (m = (prev ? prev->mon : mons); m && !c; m = m->next)
		for (c = (prev ? prev : m->clients); c && !ISMARKED(c); c = c->next, prev = NULL);

	if (c && ISMARKED(c) && !keepmarks)
		unmarkclient(c);
	return c;
}

void
mark(const Arg *arg)
{
	Client *c = CLIENT;
	if (!c || ISMARKED(c))
		return;
	markclient(c);
	drawbar(c->mon);
}

void
markclient(Client *c)
{
	if (!ISMARKED(c)) {
		addflag(c, Marked);
		++num_marked;
	}
}

void
markall(const Arg *arg)
{
	Client *c;
	for (c = selmon->clients; c; c = c->next) {
		if (ISMARKED(c) || !ISVISIBLE(c))
			continue;

		if ((arg->i == 2 && !HIDDEN(c)) || (arg->i != 2 && HIDDEN(c)))
			continue;

		if (arg->i == 1 && !ISFLOATING(c))
			continue;

		markclient(c);
	}
	drawbar(selmon);
}

void
markmouse(const Arg *arg)
{
	Client *r = selmon->sel;
	Client *prevr = r;
	Monitor *m;
	XEvent ev;
	Time lasttime = 0;
	unsigned long mark = (arg->i == 1 ? Marked : arg->i);

	if (r && mark != ISMARKED(r))
		togglemark(&((Arg) { .v = r }));

	if (XGrabPointer(dpy, root, False, MOUSEMASK, GrabModeAsync, GrabModeAsync,
		None, cursor[CurMove]->cursor, CurrentTime) != GrabSuccess)
		return;

	readclientstackingorder();

	do {
		XMaskEvent(dpy, MOUSEMASK|ExposureMask|SubstructureRedirectMask, &ev);
		switch (ev.type) {
		case ConfigureRequest:
		case Expose:
		case MapRequest:
			handler[ev.type](&ev);
			break;
		case MotionNotify:
			if ((ev.xmotion.time - lasttime) <= (1000 / 60))
				continue;
			lasttime = ev.xmotion.time;

			if ((m = recttomon(ev.xmotion.x, ev.xmotion.y, 1, 1)) && m != selmon)
				selmon = m;

			r = recttoclient(ev.xmotion.x, ev.xmotion.y, 1, 1, 1);
			if (r != prevr && r && mark != ISMARKED(r))
				togglemark(&((Arg) { .v = r }));

			prevr = r;
			break;
		}
	} while (ev.type != ButtonRelease);
	XUngrabPointer(dpy, CurrentTime);
}

void
togglemark(const Arg *arg)
{
	Client *c = CLIENT;
	if (ISMARKED(c))
		unmarkclient(c);
	else
		markclient(c);
	drawbar(c->mon);
}

void
unmark(const Arg *arg)
{
	Client *c = CLIENT;
	if (!c)
		return;
	unmarkclient(c);
	drawbar(c->mon);
}

void
unmarkclient(Client *c)
{
	if (ISMARKED(c)) {
		removeflag(c, Marked);
		--num_marked;
	}
}

void
unmarkall(const Arg *arg)
{
	keepmarks = 0;
	for (Client *c = nextmarked(NULL, NULL); c; c = nextmarked(c->next, NULL));
	drawbars();
}