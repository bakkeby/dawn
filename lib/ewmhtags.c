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
		savewindowfloatposition(c, c->mon);
		if (c->swallowing) {
			c->swallowing->idx = i;
			setclientflags(c->swallowing);
			setclientfields(c->swallowing);
			savewindowfloatposition(c->swallowing, c->swallowing->mon);
		}
	}

	XSync(dpy, False);
}

void
savewindowfloatposition(Client *c, Monitor *m)
{
	char atom[22];
	if (c->sfx == -9999)
		return;

	sprintf(atom, "_DAWN_FLOATPOS_%u", m->num);
	unsigned long pos[] = { (MAX(c->sfx, 0) & 0xffff) | ((MAX(c->sfy, 0) & 0xffff) << 16) };
	XChangeProperty(dpy, c->win, XInternAtom(dpy, atom, False), XA_CARDINAL, 32, PropModeReplace, (unsigned char *)pos, 1);

	sprintf(atom, "_DAWN_FLOATSIZE_%u", m->num);
	unsigned long size[] = { (c->sfw & 0xffff) | ((c->sfh & 0xffff) << 16) };
	XChangeProperty(dpy, c->win, XInternAtom(dpy, atom, False), XA_CARDINAL, 32, PropModeReplace, (unsigned char *)size, 1);

	XSync(dpy, False);
}

int
restorewindowfloatposition(Client *c, Monitor *m)
{
	char atom[22];
	Atom key, value;
	int x, y, w, h;

	sprintf(atom, "_DAWN_FLOATPOS_%u", m->num);

	key = XInternAtom(dpy, atom, False);
	if (!key)
		return 0;

	value = getatomprop(c, key, AnyPropertyType);
	if (!value)
		return 0;

	x = value & 0xffff;
	y = value >> 16;

	sprintf(atom, "_DAWN_FLOATSIZE_%u", m->num);

	key = XInternAtom(dpy, atom, False);
	if (!key)
		return 0;

	value = getatomprop(c, key, AnyPropertyType);
	if (!value)
		return 0;

	w = value & 0xffff;
	h = value >> 16;

	if (w <= 0 || h <= 0) {
		fprintf(stderr, "restorewindowfloatposition: bad float values x = %d, y = %d, w = %d, h = %d for client = %s\n", x, y, w, h, c->name);
		return 0;
	}

	c->sfx = x;
	c->sfy = y;
	c->sfw = w;
	c->sfh = h;

	return 1;
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
	Monitor *m;
	int nmons = 0;
	for (m = mons; m; m = m->next)
		nmons++;

	long data[nmons * 2];

	m = mons;
	for (int i = 0; i < nmons; i++) {
		data[i*2] = (long)m->mx;
		data[i*2+1] = (long)m->my;
		m = m->next;
	}

	XChangeProperty(dpy, root, netatom[NetDesktopViewport], XA_CARDINAL, 32,
			PropModeReplace, (unsigned char *)data, nmons * 2);
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
