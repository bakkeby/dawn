void
togglesticky(const Arg *arg)
{
	Client *c = CLIENT;
	for (c = nextmarked(NULL, c); c; c = nextmarked(c->next, NULL))
		setflag(c, Sticky, !ISSTICKY(c));
}
