void
persistmonitorstate(Monitor *m)
{
	Client *c;
	unsigned int i;
	char atom[22];

	sprintf(atom, "_DAWN_MONITOR_TAGS_%u", m->num);

	unsigned long data[] = { m->tags };
	XChangeProperty(dpy, root, XInternAtom(dpy, atom, False), XA_CARDINAL, 32, PropModeReplace, (unsigned char *)data, 1);

	/* set dawn client atoms */
	for (i = 1, c = m->clients; c; c = c->next, ++i) {
		c->idx = i;
		setclientflags(c);
		setclientfields(c);
		if (c->swallowing) {
			c->swallowing->idx = i;
			setclientflags(c->swallowing);
			setclientfields(c->swallowing);
		}
	}

	XSync(dpy, False);
}

void
setcurrentdesktop(void)
{
	long data[] = { 0 };
	XChangeProperty(dpy, root, netatom[NetCurrentDesktop], XA_CARDINAL, 32, PropModeReplace, (unsigned char *)data, 1);
}

void
setdesktopnames(void)
{
	Monitor *m;
	int i;
	XTextProperty text;
	char *tags[NUMTAGS * num_mons];
	for (m = mons; m; m = m->next)
		for (i = 0; i < NUMTAGS; i++)
			tags[i + NUMTAGS * m->num] = tagicon(m, i);
	Xutf8TextListToTextProperty(dpy, tags, NUMTAGS * num_mons, XUTF8StringStyle, &text);
	XSetTextProperty(dpy, root, &text, netatom[NetDesktopNames]);
}

void
setfloatinghint(Client *c)
{
	unsigned int floating[1] = {ISFLOATING(c) ? 1 : 0};
	XChangeProperty(dpy, c->win, clientatom[IsFloating], XA_CARDINAL, 32, PropModeReplace, (unsigned char *)floating, 1);
}

void
setclientflags(Client *c)
{
	unsigned long data1[] = { c->flags & 0xFFFFFFFF };
	unsigned long data2[] = { c->flags >> 32 };
	XChangeProperty(dpy, c->win, clientatom[DawnClientFlags1], XA_CARDINAL, 32, PropModeReplace, (unsigned char *)data1, 1);
	XChangeProperty(dpy, c->win, clientatom[DawnClientFlags2], XA_CARDINAL, 32, PropModeReplace, (unsigned char *)data2, 1);
}

void
setclientfields(Client *c)
{
	unsigned long data[] = { c->mon->num | (c->idx << 4) | (c->tags << 12) | (c->scratchkey << 21) };
	XChangeProperty(dpy, c->win, clientatom[DawnClientTags], XA_CARDINAL, 32, PropModeReplace, (unsigned char *)data, 1);
}

void
getclientflags(Client *c)
{
	unsigned long flags1 = getatomprop(c, clientatom[DawnClientFlags1], AnyPropertyType) & 0xFFFFFFFF;
	unsigned long flags2 = getatomprop(c, clientatom[DawnClientFlags2], AnyPropertyType);

	if (flags1 || flags2) {
		c->flags = flags1 | (flags2 << 32);
		removeflag(c, Marked);
		removeflag(c, Centered);
	}
}

void
getclientfields(Client *c)
{
	Monitor *m;
	Atom fields = getatomprop(c, clientatom[DawnClientTags], AnyPropertyType);
	if (fields) {
		c->scratchkey = (fields >> 21);
		c->tags = (fields >> 12) & TAGMASK;
		c->idx = (fields & 0xFF0) >> 4;
		for (m = mons; m; m = m->next)
			if (m->num == (fields & 0xF)) {
				c->mon = m;
				break;
			}
	}
}

void
getmonitorstate(Monitor *m)
{
	char atom[22];
	int di;
	unsigned long dl;
	unsigned char *p = NULL;
	Atom da, tags = None;

	sprintf(atom, "_DAWN_MONITOR_TAGS_%u", m->num);

	Atom monitortags = XInternAtom(dpy, atom, True);
	if (!monitortags)
		return;

	if (XGetWindowProperty(dpy, root, monitortags, 0L, sizeof tags, False, AnyPropertyType,
		&da, &di, &dl, &dl, &p) == Success && p) {
		tags = *(Atom *)p;
		XFree(p);
	}

	if (tags)
		m->tags = tags;
}

void
setnumdesktops(void)
{
	long data[] = { NUMTAGS * num_mons };
	XChangeProperty(dpy, root, netatom[NetNumberOfDesktops], XA_CARDINAL, 32, PropModeReplace, (unsigned char *)data, 1);
}

void
setviewport(void)
{
	long data[] = { 0, 0 };
	XChangeProperty(dpy, root, netatom[NetDesktopViewport], XA_CARDINAL, 32, PropModeReplace, (unsigned char *)data, 2);
}

void
updatecurrentdesktop(void)
{
	long rawdata[] = { selmon->tags };
	int i = 0;
	while (*rawdata >> (i + 1)) {
		i++;
	}
	long data[] = { i + NUMTAGS * selmon->num };
	XChangeProperty(dpy, root, netatom[NetCurrentDesktop], XA_CARDINAL, 32, PropModeReplace, (unsigned char *)data, 1);
}

void
updateclientdesktop(Client *c)
{
	long rawdata[] = { selmon->tags };
	int i = 0;
	while (*rawdata >> (i + 1)) {
		i++;
	}
	long data[] = { i + NUMTAGS * selmon->num };
	XChangeProperty(dpy, c->win, netatom[NetWMDesktop], XA_CARDINAL, 32, PropModeReplace, (unsigned char *)data, 1);
}