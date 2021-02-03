static int combo = 0;

void
keyrelease(XEvent *e)
{
	combo = 0;
}

void
combotag(const Arg *arg)
{
	Monitor *m = selmon;
	if (!(m->sel && arg->ui & TAGMASK))
		return;

	if (m->sel->reverttags)
		m->sel->reverttags = 0;
	if (combo)
		m->sel->tags |= arg->ui & TAGMASK;
	else
		m->sel->tags = arg->ui & TAGMASK;
	focus(NULL);
	arrange(m);
	combo = 1;
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
