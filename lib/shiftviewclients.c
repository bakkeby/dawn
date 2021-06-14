void
shiftviewclients(const Arg *arg)
{
	Arg shifted;
	Client *c;
	unsigned int tagmask = 0;

	for (c = selmon->clients; c; c = c->next)
		tagmask |= c->tags;

	shifted.ui = selmon->tags;
	if (arg->i > 0) // left circular shift
		do {
			shifted.ui = (shifted.ui << arg->i)
			   | (shifted.ui >> (NUMTAGS - arg->i));
		} while (tagmask && !(shifted.ui & tagmask));
	else // right circular shift
		do {
			shifted.ui = (shifted.ui >> -arg->i)
			   | (shifted.ui << (NUMTAGS + arg->i));
		} while (tagmask && !(shifted.ui & tagmask));

	view(&shifted);
}
