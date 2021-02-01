void
swaptags(const Arg *arg)
{
	Monitor *m;
	unsigned int newtag = arg->ui & TAGMASK;
	unsigned int curtag = selmon->tags;

	if (newtag == curtag || !curtag || (curtag & (curtag-1)))
		return;

	for (Client *c = selmon->clients; c != NULL; c = c->next) {
		if ((c->tags & newtag) || (c->tags & curtag))
			c->tags ^= curtag ^ newtag;

		if (!c->tags)
			c->tags = newtag;
	}

	if (enabled(Desktop)) {
		for (m = mons; m; m = m->next)
			m->tags = newtag;
		focus(NULL);
		arrange(NULL);
	} else {
		selmon->tags = newtag;
		focus(NULL);
		arrange(selmon);
	}
}