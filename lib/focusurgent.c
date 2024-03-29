void
focusurgent(const Arg *arg)
{
	Client *c;
	int i;
	for (c = selmon->clients; c && !ISURGENT(c); c = c->next);
	if (c) {
		for (i = 0; i < NUMTAGS && !((1 << i) & c->tags); i++);
		if (i < NUMTAGS) {
			if (((1 << i) & TAGMASK) != selmon->tags)
				view(&((Arg) { .ui = 1 << i }));
			focus(c);
		}
	}
}
