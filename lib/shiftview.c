void
shiftview(const Arg *arg)
{
	Arg shifted;
	unsigned int seltags = selmon->tags;

	if (arg->i > 0) // left circular shift
		shifted.ui = (seltags << arg->i) | (seltags >> (NUMTAGS - arg->i));
	else // right circular shift
		shifted.ui = seltags >> -arg->i | seltags << (NUMTAGS + arg->i);

	view(&shifted);
}
