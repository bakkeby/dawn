#define SCHEMEFOR(c) getschemefor(m, c, groupactive == c)

enum { GRP_NOSELECTION, GRP_MASTER, GRP_STACK1, GRP_STACK2, GRP_FLOAT, GRP_HIDDEN };

int
width_flexwintitle(Bar *bar, BarArg *a)
{
	return a->w;
}

int
draw_flexwintitle(Bar *bar, BarArg *a)
{
	drw_rect(drw, a->x, a->y, a->w, a->h, 1, 1);
	return flextitlecalculate(bar, a->x, a->w, -1, flextitledraw, NULL, a);
}

int
click_flexwintitle(Bar *bar, Arg *arg, BarArg *a)
{
	flextitlecalculate(bar, 0, a->w, a->x, flextitleclick, arg, a);
	return ClkWinTitle;
}

Client *
flextitledrawarea(Monitor *m, Client *c, int x, int w, int num_clients, int scheme, int draw_tiled, int draw_hidden, int draw_floating,
	int passx, void(*tabfn)(Monitor *, Client *, int, int, int, int, Arg *arg, BarArg *barg), Arg *arg, BarArg *barg)
{
	int i, rw, cw;
	cw = (w - flexwintitle_separator * (num_clients - 1)) / num_clients;
	rw = (w - flexwintitle_separator * (num_clients - 1)) % num_clients;
	for (i = 0; c && i < num_clients; c = c->next) {
		if (
			(draw_tiled && !ISFLOATING(c) && !HIDDEN(c)) ||
			(draw_floating && ISFLOATING(c) && !HIDDEN(c)) ||
			(draw_hidden && HIDDEN(c))
		) {
			tabfn(m, c, passx, x, cw + (i < rw ? 1 : 0), scheme, arg, barg);
			x += cw + (i < rw ? 1 : 0) + flexwintitle_separator;
			i++;
		}
	}
	return c;
}

int
getschemefor(Monitor *m, int group, int activegroup)
{
	switch (group) {
	case GRP_NOSELECTION:
		return SchemeTitleNorm;
	case GRP_MASTER:
	case GRP_STACK1:
	case GRP_STACK2:
		if (m->layout->arrange == &flextile)
			return (activegroup ? SchemeFlexActTTB + m->ltaxis[group] : SchemeFlexInaTTB + m->ltaxis[group]);
		return SchemeTitleNorm;
	case GRP_HIDDEN:
		return SchemeHid;
	case GRP_FLOAT:
		return (activegroup ? SchemeFlexActFloat : SchemeFlexInaFloat);
	}
	return SchemeTitleNorm;
}

void
getclientcounts(Monitor *m, int *groupactive, int *n, int *clientsnmaster, int *clientsnstack, int *clientsnstack2, int *clientsnfloating, int *clientsnhidden)
{
		Client *c;
		int i, selidx = 0, cm = 0, cs1 = 0, cs2 = 0, cf = 0, ch = 0, center, dualstack;

		for (i = 0, c = m->clients; c; c = c->next) {
			if (!ISVISIBLE(c))
				continue;
			if (HIDDEN(c)) {
				if (flexwintitle_hiddenweight)
					ch++;
				continue;
			}

			if (ISFLOATING(c)) {
				if (flexwintitle_floatweight)
					cf++;
				continue;
			}

			if (m->sel == c)
				selidx = i;

			if (!m->nmaster || i < m->nmaster || m->ltaxis[LAYOUT] == NO_SPLIT)
				cm++;
			else if (m->nstack) {
				if (cs1 < m->nstack)
					cs1++;
				else
					cs2++;
			}
			else if ((i - m->nmaster) % 2)
				cs2++;
			else
				cs1++;
			i++;
		}

		*n = cm + cs1 + cs2 + cf + ch;
		center = iscenteredlayout(m, *n);
		dualstack = isdualstacklayout(m);

		if ((!center && !dualstack) || (center && *n <= m->nmaster + (m->nstack ? m->nstack : 1))) {
			cs1 += cs2;
			cs2 = 0;
		}

		if (!m->sel)
			*groupactive = GRP_NOSELECTION;
		else if (HIDDEN(m->sel))
			*groupactive = GRP_HIDDEN;
		else if (ISFLOATING(m->sel))
			*groupactive = GRP_FLOAT;
		else if (selidx < cm)
			*groupactive = GRP_MASTER;
		else if (selidx < cm + cs1)
			*groupactive = GRP_STACK1;
		else if (selidx < cm + cs1 + cs2)
			*groupactive = GRP_STACK2;

		*clientsnmaster = cm;
		*clientsnstack = cs1;
		*clientsnstack2 = cs2;
		*clientsnfloating = cf;
		*clientsnhidden = ch;
}

int
isdualstacklayout(Monitor *m)
{
	if (m->layout->arrange != &flextile)
		return 0;

	int layout = m->ltaxis[LAYOUT];
	if (layout < 0)
		layout *= -1;

	return (
		layout == SPLIT_HORIZONTAL_DUAL_STACK ||
		layout == SPLIT_HORIZONTAL_DUAL_STACK_FIXED ||
		layout == SPLIT_VERTICAL_DUAL_STACK ||
		layout == SPLIT_VERTICAL_DUAL_STACK_FIXED
	);
}

int
iscenteredlayout(Monitor *m, int n)
{
	if (m->layout->arrange != &flextile)
		return 0;

	int layout = m->ltaxis[LAYOUT];
	if (layout < 0)
		layout *= -1;

	return (
			(layout == SPLIT_CENTERED_VERTICAL && (n - m->nmaster > 1)) ||
			layout == SPLIT_CENTERED_VERTICAL_FIXED ||
			(layout == SPLIT_CENTERED_HORIZONTAL && (n - m->nmaster > 1)) ||
			layout == SPLIT_CENTERED_HORIZONTAL_FIXED ||
			layout == FLOATING_MASTER
	);
}

int
isfixedlayout(Monitor *m)
{
	if (m->layout->arrange != &flextile)
		return 0;

	int layout = m->ltaxis[LAYOUT];
	if (layout < 0)
		layout *= -1;

	return layout > FLOATING_MASTER;
}

int
ismirroredlayout(Monitor *m)
{
	if (m->layout->arrange != &flextile)
		return 0;

	return m->ltaxis[LAYOUT] < 0;
}

int
getselschemefor(int scheme)
{
	if (scheme == SchemeFlexActFloat || scheme == SchemeFlexInaFloat)
		return SchemeFlexSelFloat;
	if (scheme >= SchemeFlexInaTTB)
		return scheme + SchemeFlexInaTTB - SchemeFlexActTTB;
	if (scheme >= SchemeFlexActTTB)
		return scheme + SchemeFlexSelTTB - SchemeFlexActTTB;
	return SchemeTitleSel;
}

void
flextitledraw(Monitor *m, Client *c, int unused, int x, int w, int tabscheme, Arg *arg, BarArg *barg)
{
	if (!c)
		return;

	int i, nclienttags = 0, nviewtags = 0, pad = lrpad / 2;
	int clientscheme = (
		ISMARKED(c)
		? SchemeMarked
		: c->scratchkey != 0 && c == selmon->sel
		? SchemeScratchSel
		: c->scratchkey != 0
		? SchemeScratchNorm
		: c == selmon->sel
		? getselschemefor(tabscheme)
		: HIDDEN(c)
		? SchemeHid
		: ISURGENT(c)
		? SchemeUrg
		: tabscheme
	);
	drw_setscheme(drw, scheme[clientscheme]);
	XSetWindowBorder(dpy, c->win, scheme[clientscheme][ColBorder].pixel);

	if (w <= TEXTW("A") - lrpad + pad) // reduce text padding if wintitle is too small
		pad = (w - TEXTW("A") + lrpad < 0 ? 0 : (w - TEXTW("A") + lrpad) / 2);
	else if (enabled(CenteredWindowName) && TEXTW(c->name) < w)
		pad = (w - TEXTW(c->name) + lrpad) / 2;

	drw_text(drw, x, barg->y, w, barg->h, pad, c->name, 0, False);
	drawstateindicator(m, c, 1, x + 2, barg->y, w, barg->h, 0, 0, 0);

	/* Optional tags icons */
	for (i = 0; i < NUMTAGS; i++) {
		if ((m->tags >> i) & 1)
			nviewtags++;
		if ((c->tags >> i) & 1)
			nclienttags++;
	}

	if (TAGSINDICATOR == 2 || nclienttags > 1 || nviewtags > 1)
		drawindicator(m, c, 1, x, barg->y, w, barg->h, 0, 0, 0, INDICATOR_RIGHT_TAGS);
}

void
flextitleclick(Monitor *m, Client *c, int passx, int x, int w, int unused, Arg *arg, BarArg *barg)
{
	if (passx >= x && passx <= x + w)
		arg->v = c;
}

int
flextitlecalculate(
	Bar *bar, int offx, int tabw, int passx,
	void(*tabfn)(Monitor *, Client *, int, int, int, int, Arg *arg, BarArg *barg),
	Arg *arg, BarArg *a
) {
	Client *c;
	Monitor *m = bar->mon;
	int n, center = 0, mirror = 0, fixed = 0; // layout configuration
	int groupactive = 0, clientsnmaster = 0, clientsnstack = 0, clientsnstack2 = 0, clientsnfloating = 0, clientsnhidden = 0;
	int w, r, den, fulllayout = 0;
	int rw, rr;

	int mas_x, st1_x, st2_x, hid_x, flt_x;
	int mas_w = 0, st1_w = 0, st2_w = 0, hid_w = 0, flt_w = 0;

	/* This avoids drawing a separator on the left hand side of the wintitle section if
	 * there is a border and the wintitle module rests at the left border. */
	if (bar->borderpx && a->x > bar->bx + bar->borderpx) {
		offx += flexwintitle_separator;
		tabw -= flexwintitle_separator;
	}

	/* This avoids drawing a separator on the right hand side of the wintitle section if
	 * there is a border and the wintitle module rests at the right border. */
	if (bar->borderpx && a->x + a->w < bar->bx + bar->bw - 2 * bar->borderpx)
		tabw -= flexwintitle_separator;

	mas_x = st1_x = st2_x = hid_x = flt_x = offx;

	getclientcounts(m, &groupactive, &n, &clientsnmaster, &clientsnstack, &clientsnstack2, &clientsnfloating, &clientsnhidden);

	if (n == 0)
	 	return 0;
	else if (m->layout->arrange == &flextile) {
		mirror = ismirroredlayout(m);
		fixed = isfixedlayout(m);
		center = iscenteredlayout(m, n);
	}

	/* Certain layouts have no master / stack areas */
	if (!m->layout->arrange                            // floating layout
		|| (!n || (!fixed && m->nmaster && n <= m->nmaster)) // no master
		|| (m->layout->arrange == &flextile && m->ltaxis[LAYOUT] == NO_SPLIT)
	)
		fulllayout = 1;

	c = m->clients;

	/* floating mode */
	if ((fulllayout && flexwintitle_floatweight > 0) || clientsnmaster + clientsnstack == 0 || !m->layout->arrange) {
		den = clientsnmaster + clientsnstack + clientsnstack2 + clientsnfloating + clientsnhidden;
		c = flextitledrawarea(m, c, mas_x, tabw, den, !m->layout->arrange ? SchemeFlexActFloat : SCHEMEFOR(GRP_MASTER), 1, flexwintitle_hiddenweight, flexwintitle_floatweight, passx, tabfn, arg, a); // floating
	/* no master and stack mode, e.g. monocole, grid layouts, fibonacci */
	} else if (fulllayout) {
		den = clientsnmaster + clientsnstack + clientsnstack2 + clientsnhidden;
		c = flextitledrawarea(m, c, mas_x, tabw, den, SCHEMEFOR(GRP_MASTER), 1, flexwintitle_hiddenweight, 0, passx, tabfn, arg, a); // full
	/* tiled mode */
	} else {
		den = clientsnmaster * flexwintitle_masterweight
		    + (clientsnstack + clientsnstack2) * flexwintitle_stackweight
		    + clientsnhidden * flexwintitle_hiddenweight
		    + clientsnfloating * flexwintitle_floatweight;
		w = (tabw - (n - 1) * flexwintitle_separator) / den;
		r = (tabw - (n - 1) * flexwintitle_separator) % den;

		rw = r / n; // rest incr per client
		rr = r % n; // rest rest

		if (clientsnmaster && flexwintitle_masterweight)
			mas_w = clientsnmaster * rw + w * clientsnmaster * flexwintitle_masterweight + (rr > 0 ? MIN(rr, clientsnmaster) : 0) + (clientsnmaster - 1) * flexwintitle_separator;
		rr -= clientsnmaster;
		if (clientsnstack && flexwintitle_stackweight)
			st1_w = clientsnstack * (rw + w * flexwintitle_stackweight) + (rr > 0 ? MIN(rr, clientsnstack) : 0) + (clientsnstack - 1) * flexwintitle_separator;
		rr -= clientsnstack;
		if (clientsnstack2 && flexwintitle_stackweight)
			st2_w = clientsnstack2 * (rw + w * flexwintitle_stackweight) + (rr > 0 ? MIN(rr, clientsnstack2) : 0) + (clientsnstack2 - 1) * flexwintitle_separator;
		rr -= clientsnstack2;
		if (clientsnhidden && flexwintitle_hiddenweight)
			hid_w = clientsnhidden * (rw + w * flexwintitle_hiddenweight) + (rr > 0 ? MIN(rr, clientsnhidden) : 0) + (clientsnhidden - 1) * flexwintitle_separator;
		rr -= clientsnhidden;
		if (clientsnfloating && flexwintitle_floatweight)
			flt_w = clientsnfloating * (rw + w * flexwintitle_floatweight) + (rr > 0 ? MIN(rr, clientsnfloating) : 0) + (clientsnfloating - 1) * flexwintitle_separator;
		if (rr > 0)
			mas_w += rr;

		if (mirror) {
			if (center && clientsnstack2) {
				mas_x = st1_x + (st1_w ? st1_w + flexwintitle_separator : 0);
				st2_x = mas_x + (mas_w ? mas_w + flexwintitle_separator : 0);
				hid_x = st2_x + (st2_w ? st2_w + flexwintitle_separator : 0);
			} else {
				if (clientsnstack2) {
					st2_x = st1_x + (st1_w ? st1_w + flexwintitle_separator : 0);
					mas_x = st2_x + (st2_w ? st2_w + flexwintitle_separator : 0);
				} else
					mas_x = st1_x + (st1_w ? st1_w + flexwintitle_separator : 0);
				hid_x = mas_x + (mas_w ? mas_w + flexwintitle_separator : 0);
			}
		} else {
			if (center && clientsnstack2) {
				mas_x = st2_x + (st2_w ? st2_w + flexwintitle_separator : 0);
				st1_x = mas_x + (mas_w ? mas_w + flexwintitle_separator : 0);
				hid_x = st1_x + (st1_w ? st1_w + flexwintitle_separator : 0);
			} else {
				st1_x = mas_x + (mas_w ? mas_w + flexwintitle_separator : 0);
				if (clientsnstack2) {
					st2_x = st1_x + (st1_w ? st1_w + flexwintitle_separator : 0);
					hid_x = st2_x + (st2_w ? st2_w + flexwintitle_separator : 0);
				} else
					hid_x = st1_x + (st1_w ? st1_w + flexwintitle_separator : 0);
			}
		}
		flt_x = hid_x + (hid_w ? hid_w + flexwintitle_separator : 0);

		if (clientsnmaster)
			c = flextitledrawarea(m, c, mas_x, mas_w, clientsnmaster, SCHEMEFOR(GRP_MASTER), 1, 0, 0, passx, tabfn, arg, a); // master1
		if (clientsnstack)
			c = flextitledrawarea(m, c, st1_x, st1_w, clientsnstack, SCHEMEFOR(GRP_STACK1), 1, 0, 0, passx, tabfn, arg, a); // stack1
		if (clientsnstack2)
			c = flextitledrawarea(m, c, st2_x, st2_w, clientsnstack2, SCHEMEFOR(GRP_STACK2), 1, 0, 0, passx, tabfn, arg, a); // stack2
		if (clientsnhidden)
			c = flextitledrawarea(m, m->clients, hid_x, hid_w, clientsnhidden, SCHEMEFOR(GRP_HIDDEN), 0, 1, 0, passx, tabfn, arg, a); // hidden
		if (clientsnfloating)
			c = flextitledrawarea(m, m->clients, flt_x, flt_w, clientsnfloating, SCHEMEFOR(GRP_FLOAT), 0, 0, 1, passx, tabfn, arg, a); // floating
	}
	return 1;
}