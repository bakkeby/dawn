void savefloats(Client *c)
{
	/* save last known float dimensions */
	c->sfx = c->x;
	c->sfy = c->y;
	c->sfw = c->w;
	c->sfh = c->h;
}

void
restorefloats(Client *c)
{
	if (!ISVISIBLE(c))
		return;
	if (c->sfx == -9999)
		savefloats(c);

	int x = c->sfx;
	int y = c->sfy;
	int w = c->sfw;
	int h = c->sfh;

	XMoveResizeWindow(dpy, c->win, x, y, w, h);
	resize(c, x, y, w, h, 0);
}

void
restoremonfloats(Monitor *m)
{
	Client *c;
	/* restore last known float dimensions for all visible clients */
	for (c = m->clients; c; c = c->next)
		if (ISVISIBLE(c) && !HIDDEN(c))
			restorefloats(c);
}
