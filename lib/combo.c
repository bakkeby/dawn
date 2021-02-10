static int combo = 0;

void
keyrelease(XEvent *e)
{
	combo = 0;
	if (keepmarks)
		unmarkall(NULL);
}

void
combotag(const Arg *arg)
{
	Client *c = selmon->sel;
	int tags = arg->ui & TAGMASK;

	if (!tags)
		return;

	if (combo) {
		for (c = nextmarked(NULL, c); c; c = nextmarked(c->next, NULL))
			c->tags |= tags;
		arrange(selmon);
	} else {
		keepmarks = 1;
		combo = 1;
		tag(arg);
	}
}

void
comboview(const Arg *arg)
{
	Monitor *m;

	if (enabled(Desktop)) {
		for (m = mons; m; m = m->next)
			comboviewmon(m, arg);
		focus(NULL);
		arrange(NULL);
	} else {
		comboviewmon(selmon, arg);
		focus(NULL);
		arrange(selmon);
	}
	combo = 1;
	updatecurrentdesktop();
}

void
comboviewmon(Monitor *m, const Arg *arg)
{
	unsigned newtags = arg->ui & TAGMASK;
	if (combo)
		m->tags |= newtags;
	else if (newtags)
		viewmon(m, &((Arg) { .ui = newtags }));
}
