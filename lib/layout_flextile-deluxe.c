typedef struct {
	void (*arrange)(Monitor *, int, int, int, int, int, int, int);
} LayoutArranger;

typedef struct {
	void (*arrange)(Monitor *, int, int, int, int, int, int, int, int, int);
} TileArranger;

static const LayoutArranger flexlayouts[] = {
	{ layout_no_split },
	{ layout_split_vertical },
	{ layout_split_horizontal },
	{ layout_split_centered_vertical },
	{ layout_split_centered_horizontal },
	{ layout_split_vertical_dual_stack },
	{ layout_split_horizontal_dual_stack },
	{ layout_floating_master },
	{ layout_split_vertical_fixed },
	{ layout_split_horizontal_fixed },
	{ layout_split_centered_vertical_fixed },
	{ layout_split_centered_horizontal_fixed },
	{ layout_split_vertical_dual_stack_fixed },
	{ layout_split_horizontal_dual_stack_fixed },
	{ layout_floating_master_fixed },
};

static const TileArranger flextiles[] = {
	{ arrange_top_to_bottom },
	{ arrange_left_to_right },
	{ arrange_monocle },
	{ arrange_gapplessgrid },
	{ arrange_gapplessgrid_cfacts },
	{ arrange_gapplessgrid_alt1 },
	{ arrange_gapplessgrid_alt2 },
	{ arrange_gridmode },
	{ arrange_horizgrid },
	{ arrange_dwindle },
	{ arrange_dwindle_cfacts },
	{ arrange_spiral },
	{ arrange_spiral_cfacts },
	{ arrange_tatami },
	{ arrange_tatami_cfacts },
};

static void
getfactsforrange(Monitor *m, int an, int ai, int size, int *rest, float *fact)
{
	int i;
	float facts;
	Client *c;
	int total = 0;

	facts = 0;
	for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++)
		if (i >= ai && i < (ai + an))
			facts += c->cfact;

	for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++)
		if (i >= ai && i < (ai + an))
			total += size * (c->cfact / facts);

	*rest = size - total;
	*fact = facts;
}

static void
setlayoutaxisex(const Arg *arg)
{
	int axis, arr;

	axis = arg->i & 0x3; // lower two bits indicates layout, master or stack1-2
	arr = arg->i >> 2;   // remaining upper bits indicate arrangement

	if ((axis == 0 && abs(arr) > LAYOUT_LAST)
			|| (axis > 0 && (arr > AXIS_LAST || arr < 0)))
		arr = 0;

	selmon->ltaxis[axis] = arr;
	selmon->pertag->ltaxis[selmon->pertag->curtag][axis] = selmon->ltaxis[axis];
	arrangemon(selmon);
}

static void
setlayoutex(const Arg *arg)
{
	setlayout(&((Arg) { .v = &layouts[arg->i] }));
}

static void
layout_no_split(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n)
{
	(&flextiles[m->ltaxis[m->nmaster >= n ? MASTER : STACK]])->arrange(m, x, y, h, w, ih, iv, n, n, 0);
}

static void
layout_split_vertical(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n)
{
	/* Split master into master + stack if we have enough clients */
	if (m->nmaster && n > m->nmaster) {
		layout_split_vertical_fixed(m, x, y, h, w, ih, iv, n);
	} else {
		layout_no_split(m, x, y, h, w, ih, iv, n);
	}
}

static void
layout_split_vertical_fixed(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n)
{
	int sw, sx;

	sw = (w - iv) * (1 - m->mfact);
	w = (w - iv) * m->mfact;
	if (m->ltaxis[LAYOUT] < 0) { // mirror
		sx = x;
		x += sw + iv;
	} else {
		sx = x + w + iv;
	}

	(&flextiles[m->ltaxis[MASTER]])->arrange(m, x, y, h, w, ih, iv, n, m->nmaster, 0);
	(&flextiles[m->ltaxis[STACK]])->arrange(m, sx, y, h, sw, ih, iv, n, n - m->nmaster, m->nmaster);
}

static void
layout_split_vertical_dual_stack(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n)
{
	/* Split master into master + stack if we have enough clients */
	if (!m->nmaster || n <= m->nmaster) {
		layout_no_split(m, x, y, h, w, ih, iv, n);
	} else if (n <= m->nmaster + (m->nstack ? m->nstack : 1)) {
		layout_split_vertical(m, x, y, h, w, ih, iv, n);
	} else {
		layout_split_vertical_dual_stack_fixed(m, x, y, h, w, ih, iv, n);
	}
}

static void
layout_split_vertical_dual_stack_fixed(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n)
{
	int sh, sw, sx, oy, sc;

	if (m->nstack)
		sc = m->nstack;
	else
		sc = (n - m->nmaster) / 2 + ((n - m->nmaster) % 2 > 0 ? 1 : 0);

	sw = (w - iv) * (1 - m->mfact);
	sh = (h - ih) / 2;
	w = (w - iv) * m->mfact;
	oy = y + sh + ih;
	if (m->ltaxis[LAYOUT] < 0) { // mirror
		sx = x;
		x += sw + iv;
	} else {
		sx = x + w + iv;
	}

	(&flextiles[m->ltaxis[MASTER]])->arrange(m, x, y, h, w, ih, iv, n, m->nmaster, 0);
	(&flextiles[m->ltaxis[STACK]])->arrange(m, sx, y, sh, sw, ih, iv, n, sc, m->nmaster);
	(&flextiles[m->ltaxis[STACK2]])->arrange(m, sx, oy, sh, sw, ih, iv, n, n - m->nmaster - sc, m->nmaster + sc);
}

static void
layout_split_horizontal(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n)
{
	/* Split master into master + stack if we have enough clients */
	if (m->nmaster && n > m->nmaster) {
		layout_split_horizontal_fixed(m, x, y, h, w, ih, iv, n);
	} else {
		layout_no_split(m, x, y, h, w, ih, iv, n);
	}
}

static void
layout_split_horizontal_fixed(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n)
{
	int sh, sy;

	sh = (h - ih) * (1 - m->mfact);
	h = (h - ih) * m->mfact;
	if (m->ltaxis[LAYOUT] < 0) { // mirror
		sy = y;
		y += sh + ih;
	} else {
		sy = y + h + ih;
	}

	(&flextiles[m->ltaxis[MASTER]])->arrange(m, x, y, h, w, ih, iv, n, m->nmaster, 0);
	(&flextiles[m->ltaxis[STACK]])->arrange(m, x, sy, sh, w, ih, iv, n, n - m->nmaster, m->nmaster);
}

static void
layout_split_horizontal_dual_stack(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n)
{
	/* Split master into master + stack if we have enough clients */
	if (!m->nmaster || n <= m->nmaster) {
		layout_no_split(m, x, y, h, w, ih, iv, n);
	} else if (n <= m->nmaster + (m->nstack ? m->nstack : 1)) {
		layout_split_horizontal(m, x, y, h, w, ih, iv, n);
	} else {
		layout_split_horizontal_dual_stack_fixed(m, x, y, h, w, ih, iv, n);
	}
}

static void
layout_split_horizontal_dual_stack_fixed(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n)
{
	int sh, sy, ox, sc;

	if (m->nstack)
		sc = m->nstack;
	else
		sc = (n - m->nmaster) / 2 + ((n - m->nmaster) % 2 > 0 ? 1 : 0);

	sh = (h - ih) * (1 - m->mfact);
	h = (h - ih) * m->mfact;
	sw = (w - iv) / 2;
	ox = x + sw + iv;
	if (m->ltaxis[LAYOUT] < 0) { // mirror
		sy = y;
		y += sh + ih;
	} else {
		sy = y + h + ih;
	}

	(&flextiles[m->ltaxis[MASTER]])->arrange(m, x, y, h, w, ih, iv, n, m->nmaster, 0);
	(&flextiles[m->ltaxis[STACK]])->arrange(m, x, sy, sh, sw, ih, iv, n, sc, m->nmaster);
	(&flextiles[m->ltaxis[STACK2]])->arrange(m, ox, sy, sh, sw, ih, iv, n, n - m->nmaster - sc, m->nmaster + sc);
}

static void
layout_split_centered_vertical(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n)
{
	/* Split master into master + stack if we have enough clients */
	if (!m->nmaster || n <= m->nmaster) {
		layout_no_split(m, x, y, h, w, ih, iv, n);
	} else if (n <= m->nmaster + (m->nstack ? m->nstack : 1)) {
		layout_split_vertical(m, x, y, h, w, ih, iv, n);
	} else {
		layout_split_centered_vertical_fixed(m, x, y, h, w, ih, iv, n);
	}
}

static void
layout_split_centered_vertical_fixed(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n)
{
	int sw, sx, ox, sc;

	if (m->nstack)
		sc = m->nstack;
	else
		sc = (n - m->nmaster) / 2 + ((n - m->nmaster) % 2 > 0 ? 1 : 0);

	sw = (w - 2*iv) * (1 - m->mfact) / 2;
	w = (w - 2*iv) * m->mfact;
	if (m->ltaxis[LAYOUT] < 0)  { // mirror
		sx = x;
		x += sw + iv;
		ox = x + w + iv;
	} else {
		ox = x;
		x += sw + iv;
		sx = x + w + iv;
	}

	(&flextiles[m->ltaxis[MASTER]])->arrange(m, x, y, h, w, ih, iv, n, m->nmaster, 0);
	(&flextiles[m->ltaxis[STACK]])->arrange(m, sx, y, h, sw, ih, iv, n, sc, m->nmaster);
	(&flextiles[m->ltaxis[STACK2]])->arrange(m, ox, y, h, sw, ih, iv, n, n - m->nmaster - sc, m->nmaster + sc);
}

static void
layout_split_centered_horizontal(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n)
{
	/* Split master into master + stack if we have enough clients */
	if (!m->nmaster || n <= m->nmaster) {
		layout_no_split(m, x, y, h, w, ih, iv, n);
	} else if (n <= m->nmaster + (m->nstack ? m->nstack : 1)) {
		layout_split_horizontal(m, x, y, h, w, ih, iv, n);
	} else {
		layout_split_centered_horizontal_fixed(m, x, y, h, w, ih, iv, n);
	}
}

static void
layout_split_centered_horizontal_fixed(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n)
{
	int sh, sy, oy, sc;

	if (m->nstack)
		sc = m->nstack;
	else
		sc = (n - m->nmaster) / 2 + ((n - m->nmaster) % 2 > 0 ? 1 : 0);

	sh = (h - 2*ih) * (1 - m->mfact) / 2;
	h = (h - 2*ih) * m->mfact;
	if (m->ltaxis[LAYOUT] < 0)  { // mirror
		sy = y;
		y += sh + ih;
		oy = y + h + ih;
	} else {
		oy = y;
		y += sh + ih;
		sy = y + h + ih;
	}

	(&flextiles[m->ltaxis[MASTER]])->arrange(m, x, y, h, w, ih, iv, n, m->nmaster, 0);
	(&flextiles[m->ltaxis[STACK]])->arrange(m, x, sy, sh, w, ih, iv, n, sc, m->nmaster);
	(&flextiles[m->ltaxis[STACK2]])->arrange(m, x, oy, sh, w, ih, iv, n, n - m->nmaster - sc, m->nmaster + sc);
}

static void
layout_floating_master(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n)
{
	/* Split master into master + stack if we have enough clients */
	if (!m->nmaster || n <= m->nmaster) {
		layout_no_split(m, x, y, h, w, ih, iv, n);
	} else {
		layout_floating_master_fixed(m, x, y, h, w, ih, iv, n);
	}
}

static void
layout_floating_master_fixed(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n)
{
	int mh, mw;

	/* Draw stack area first */
	(&flextiles[m->ltaxis[STACK]])->arrange(m, x, y, h, w, ih, iv, n, n - m->nmaster, m->nmaster);

	if (w > h) {
		mw = w * m->mfact;
		mh = h * 0.9;
	} else {
		mw = w * 0.9;
		mh = h * m->mfact;
	}
	x = x + (w - mw) / 2;
	y = y + (h - mh) / 2;

	(&flextiles[m->ltaxis[MASTER]])->arrange(m, x, y, mh, mw, ih, iv, n, m->nmaster, 0);
}

static void
arrange_left_to_right(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n, int an, int ai)
{
	int i, rest;
	float facts, fact = 1;
	Client *c;

	if (ai + an > n)
		an = n - ai;

	w -= iv * (an - 1);
	getfactsforrange(m, an, ai, w, &rest, &facts);
	for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++) {
		if (i >= ai && i < (ai + an)) {
			fact = c->cfact;
			resize(c, x, y, w * (fact / facts) + ((i - ai) < rest ? 1 : 0) - (2 * c->bw), h - (2 * c->bw), 0);
			x += WIDTH(c) + iv;
		}
	}
}

static void
arrange_top_to_bottom(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n, int an, int ai)
{
	int i, rest;
	float facts, fact = 1;
	Client *c;

	if (ai + an > n)
		an = n - ai;

	h -= ih * (an - 1);
	getfactsforrange(m, an, ai, h, &rest, &facts);
	for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++) {
		if (i >= ai && i < (ai + an)) {
			fact = c->cfact;
			resize(c, x, y, w - (2 * c->bw), h * (fact / facts) + ((i - ai) < rest ? 1 : 0) - (2 * c->bw), 0);
			y += HEIGHT(c) + ih;
		}
	}
}

static void
arrange_monocle(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n, int an, int ai)
{
	int i, stackno, minstackno = 0xFFFFFF;
	Client *c, *s, *f = NULL;

	for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++)
		if (i >= ai && i < (ai + an)) {
			for (stackno = 0, s = m->stack; s && s != c; s = s->snext, ++stackno);
			if (stackno < minstackno) {
				f = s;
				minstackno = stackno;
			}
		}

	for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++)
		if (i >= ai && i < (ai + an)) {
			if (c == f) {
				XMoveWindow(dpy, c->win, x, y);
				resize(c, x, y, w - (2 * c->bw), h - (2 * c->bw), 0);
			} else {
				XMoveWindow(dpy, c->win, WIDTH(c) * -2, c->y);
			}
		}
}

static void
arrange_gridmode(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n, int an, int ai)
{
	int i, cols, rows, ch, cw, cx, cy, cc, cr, chrest, cwrest; // counters
	Client *c;

	/* grid dimensions */
	for (rows = 0; rows <= an/2; rows++)
		if (rows*rows >= an)
			break;
	cols = (rows && (rows - 1) * rows >= an) ? rows - 1 : rows;

	/* window geoms (cell height/width) */
	ch = (h - ih * (rows - 1)) / (rows ? rows : 1);
	cw = (w - iv * (cols - 1)) / (cols ? cols : 1);
	chrest = h - ih * (rows - 1) - ch * rows;
	cwrest = w - iv * (cols - 1) - cw * cols;
	for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++) {
		if (i >= ai && i < (ai + an)) {
			cc = ((i - ai) / rows); // client column number
			cr = ((i - ai) % rows); // client row number
			cx = x + cc * (cw + iv) + MIN(cc, cwrest);
			cy = y + cr * (ch + ih) + MIN(cr, chrest);
			resize(c, cx, cy, cw + (cc < cwrest ? 1 : 0) - 2 * c->bw, ch + (cr < chrest ? 1 : 0) - 2 * c->bw, False);
		}
	}
}

static void
arrange_horizgrid(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n, int an, int ai)
{
	int ntop, nbottom, rh, rest;

	/* Exception when there is only one client; don't split into two rows */
	if (an == 1) {
		arrange_monocle(m, x, y, h, w, ih, iv, n, an, ai);
		return;
	}

	ntop = an / 2;
	nbottom = an - ntop;
	rh = (h - ih) / 2;
	rest = h - ih - rh * 2;
	arrange_left_to_right(m, x, y, rh + rest, w, ih, iv, n, ntop, ai);
	arrange_left_to_right(m, x, y + rh + ih + rest, rh, w, ih, iv, n, nbottom, ai + ntop);
}

static void
arrange_gapplessgrid(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n, int an, int ai)
{
	int i, cols, rows, ch, cw, cn, rn, cc, rrest, crest; // counters
	Client *c;

	/* grid dimensions */
	for (cols = 1; cols <= an/2; cols++)
		if (cols*cols >= an)
			break;
	if (an == 5) /* set layout against the general calculation: not 1:2:2, but 2:3 */
		cols = 2;
	rows = an/cols;
	cn = rn = cc = 0; // reset column no, row no, client count

	ch = (h - ih * (rows - 1)) / rows;
	rrest = (h - ih * (rows - 1)) - ch * rows;
	cw = (w - iv * (cols - 1)) / cols;
	crest = (w - iv * (cols - 1)) - cw * cols;

	for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++) {
		if (i >= ai && i < (ai + an)) {
			if (cc/rows + 1 > cols - an%cols) {
				rows = an/cols + 1;
				ch = (h - ih * (rows - 1)) / rows;
				rrest = (h - ih * (rows - 1)) - ch * rows;
			}
			resize(c,
				x,
				y + rn*(ch + ih) + MIN(rn, rrest),
				cw + (cn < crest ? 1 : 0) - 2 * c->bw,
				ch + (rn < rrest ? 1 : 0) - 2 * c->bw,
				0);
			rn++;
			cc++;
			if (rn >= rows) {
				rn = 0;
				x += cw + ih + (cn < crest ? 1 : 0);
				cn++;
			}
		}
	}
}

static void
arrange_gapplessgrid_cfacts(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n, int an, int ai)
{
	int i, cols, rows, ch, cw, cy, cn, rn, cc, crest, colw; // counters
	float cfacts_total = 0;
	Client *c;

	/* grid dimensions */
	for (cols = 1; cols <= an/2; cols++)
		if (cols*cols >= an)
			break;
	if (an == 5) /* set layout against the general calculation: not 1:2:2, but 2:3 */
		cols = 2;
	rows = an/cols;

	crest = colw = w - iv * (cols - 1);

	float cfacts[cols];
	int rrests[cols];
	for (i = 0; i < cols; i++) {
		cfacts[i] = 0;
		rrests[i] = 0;
	}

	/* Sum cfacts for columns */
	for (i = cn = rn = cc = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), ++i) {
		if (i >= ai && i < (ai + an)) {
			if (cc/rows + 1 > cols - an%cols)
				rows = an/cols + 1;
			cfacts[cn] += c->cfact;
			cfacts_total += c->cfact;
			rn++;
			cc++;
			if (rn >= rows) {
				rn = 0;
				cn++;
			}
		}
	}

	/* Work out cfact remainders */
	for (i = cn = rn = cc = 0, rows = an/cols, c = nexttiled(m->clients); c; c = nexttiled(c->next), ++i) {
		if (i >= ai && i < (ai + an)) {
			if (cc/rows + 1 > cols - an%cols)
				rows = an/cols + 1;
			rrests[cn] += (h - ih * (rows - 1)) * (c->cfact / cfacts[cn]) + (rn == 0 ? 0 : ih);
			rn++;
			cc++;
			if (rn >= rows) {
				rn = 0;
				cn++;
			}
		}
	}

	for (i = 0; i < cols; i++) {
		crest -= (int)(colw * (cfacts[i] / cfacts_total));
		rrests[i] = h - rrests[i];
	}

	for (i = cn = rn = cc = 0, cy = y, rows = an/cols, c = nexttiled(m->clients); c; c = nexttiled(c->next), ++i) {
		if (i >= ai && i < (ai + an)) {
			if (cc/rows + 1 > cols - an%cols)
				rows = an/cols + 1;
			cw = (int)(colw * (cfacts[cn] / cfacts_total)) + (cn < crest ? 1 : 0);
			ch = (h - ih * (rows - 1)) * ((double)c->cfact / (double)cfacts[cn]) + (rn < rrests[cn] ? 1 : 0);
			resize(c,
				x,
				cy,
				cw - 2 * c->bw,
				ch - 2 * c->bw,
				0);
			rn++;
			cc++;
			cy += ch + ih;
			if (rn >= rows) {
				rn = 0;
				x += cw + ih;
				cn++;
				cy = y;
			}
		}
	}
}

/* This version of gappless grid fills rows first */
static void
arrange_gapplessgrid_alt1(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n, int an, int ai)
{
	int i, cols, rows, rest, ch;

	/* grid dimensions */
	for (cols = 1; cols <= an/2; cols++)
		if (cols*cols >= an)
			break;
	rows = (cols && (cols - 1) * cols >= an) ? cols - 1 : cols;
	ch = (h - ih * (rows - 1)) / (rows ? rows : 1);
	rest = (h - ih * (rows - 1)) - ch * rows;

	for (i = 0; i < rows; i++) {
		arrange_left_to_right(m, x, y, ch + (i < rest ? 1 : 0), w, ih, iv, n, MIN(cols, an - i*cols), ai + i*cols);
		y += ch + (i < rest ? 1 : 0) + ih;
	}
}

/* This version of gappless grid fills columns first */
static void
arrange_gapplessgrid_alt2(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n, int an, int ai)
{
	int i, cols, rows, rest, cw;

	/* grid dimensions */
	for (rows = 0; rows <= an/2; rows++)
		if (rows*rows >= an)
			break;
	cols = (rows && (rows - 1) * rows >= an) ? rows - 1 : rows;
	cw = (w - iv * (cols - 1)) / (cols ? cols : 1);
	rest = (w - iv * (cols - 1)) - cw * cols;

	for (i = 0; i < cols; i++) {
		arrange_top_to_bottom(m, x, y, h, cw + (i < rest ? 1 : 0), ih, iv, n, MIN(rows, an - i*rows), ai + i*rows);
		x += cw + (i < rest ? 1 : 0) + iv;
	}
}

static void
arrange_fibonacci(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n, int an, int ai, int s)
{
	int i, j, nv, hrest = 0, wrest = 0, nx = x, ny = y, nw = w, nh = h, r = 1;
	Client *c;

	for (i = 0, j = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), j++) {
		if (j >= ai && j < (ai + an)) {
			if (r) {
				if ((i % 2 && ((nh - ih) / 2) <= (bh + 2 * c->bw)) || (!(i % 2) && ((nw - iv) / 2) <= (bh + 2 * c->bw))) {
					r = 0;
				}
				if (r && i < an - 1) {
					if (i % 2) {
						nv = (nh - ih) / 2;
						hrest = nh - 2*nv - ih;
						nh = nv;
					} else {
						nv = (nw - iv) / 2;
						wrest = nw - 2*nv - iv;
						nw = nv;
					}

					if ((i % 4) == 2 && !s)
						nx += nw + iv;
					else if ((i % 4) == 3 && !s)
						ny += nh + ih;
				}
				if ((i % 4) == 0) {
					if (s) {
						ny += nh + ih;
						nh += hrest;
					} else {
						nh -= hrest;
						ny -= nh + ih;
					}
				} else if ((i % 4) == 1) {
					nx += nw + iv;
					nw += wrest;
				} else if ((i % 4) == 2) {
					ny += nh + ih;
					nh += hrest;
					if (i < n - 1)
						nw += wrest;
				} else if ((i % 4) == 3) {
					if (s) {
						nx += nw + iv;
						nw -= wrest;
					} else {
						nw -= wrest;
						nx -= nw + iv;
						nh += hrest;
					}
				}
				if (i == 0)	{
					if (an != 1) {
						nw = (w - iv) - (w - iv) * (1 - m->mfact);
						wrest = 0;
					}
					ny = y;
				} else if (i == 1)
					nw = w - nw - iv;
				i++;
			}

			resize(c, nx, ny, nw - 2 * c->bw, nh - 2 * c->bw, False);
		}
	}
}

static void
arrange_fibonacci_cfacts(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n, int an, int ai, int s)
{
	Client *clients[4] = { NULL, NULL, NULL, NULL };
	int i, j, q, nx, ny, nw, nh, tnw, tnh;
	Client *t, *a, *b, *c, *d;

	nx = x;
	ny = y;
	nw = w;
	nh = h;

	for (i = 0, j = 0, q = 0, t = nexttiled(m->clients); t; t = nexttiled(t->next), j++) {
		if (j >= ai && j < (ai + an)) {
			clients[q] = t;
			++q;

			if (q < 4 && (j + 1 < ai + an) && j - ai < 7) // magic number 7 limits to 9 tiled clients
				continue;

			a = clients[0];
			b = clients[1];
			c = clients[2];
			d = clients[3];

			switch (q) {
			case 1:
				resize(a, nx, ny, nw - 2 * a->bw, nh - 2 * a->bw, False);
				q = 0;
				break;
			case 2:
				tnw = (nw - iv) * (a->cfact / (a->cfact + b->cfact));
				resize(a, nx, ny, tnw - 2 * a->bw, nh - 2 * a->bw, False);
				resize(b, nx + tnw + iv, ny, nw - iv - tnw - 2 * b->bw, nh - 2 * b->bw, False);
				nx += tnw + iv;
				q = 0;
				break;
			case 3:
				tnw = (nw - iv) * (a->cfact / (a->cfact + b->cfact));
				tnh = (nh - ih) * (b->cfact / (b->cfact + c->cfact));
				if (!s && i % 2) {
					resize(a, nx + nw - tnw, ny, tnw - 2 * a->bw, nh - 2 * a->bw, False);
					resize(b, nx, ny + nh - tnh, nw - iv - tnw - 2 * b->bw, tnh - 2 * b->bw, False);
					resize(c, nx, ny, nw - iv - tnw - 2 * c->bw, nh - ih - tnh - 2 * c->bw, False);

				} else {
					resize(a, nx, ny, tnw - 2 * a->bw, nh - 2 * a->bw, False);
					resize(b, nx + tnw + iv, ny, nw - iv - tnw - 2 * b->bw, tnh - 2 * b->bw, False);
					resize(c, nx + tnw + iv, ny + tnh + ih, nw - iv - tnw - 2 * c->bw, nh - ih - tnh - 2 * c->bw, False);
					nx += tnw + iv;
					ny += tnh + ih;
				}
				nw -= tnw + iv;
				nh -= tnh + ih;
				q = 0;
				break;
			case 4:
				if (!s && i % 2) {
					tnw = (nw - iv) * (a->cfact / (a->cfact + b->cfact));
					tnh = (nh - ih) * (b->cfact / (b->cfact + c->cfact));
					resize(a, nx + nw - tnw, ny, tnw - 2 * a->bw, nh - 2 * a->bw, False);
					resize(b, nx, ny + nh - tnh, nw - iv - tnw - 2 * b->bw, tnh - 2 * b->bw, False);
				} else {
					tnw = (nw - iv) * (a->cfact / (a->cfact + b->cfact));
					tnh = (nh - ih) * (b->cfact / (b->cfact + c->cfact));
					resize(a, nx, ny, tnw - 2 * a->bw, nh - 2 * a->bw, False);
					resize(b, nx + tnw + iv, ny, nw - iv - tnw - 2 * b->bw, tnh - 2 * b->bw, False);
					nx += tnw + iv;
					ny += tnh + ih;
				}
				nw -= tnw + iv;
				nh -= tnh + ih;

				if (j + 1 == ai + an) {
					tnw = (nw - iv) * (c->cfact / (c->cfact + d->cfact));
					resize(c, nx, ny, tnw - 2 * c->bw, nh - 2 * c->bw, False);
					resize(d, nx + tnw + iv, ny, nw - iv - tnw - 2 * d->bw, nh - 2 * d->bw, False);
					q = 0;
					break;
				}

				clients[0] = c;
				clients[1] = d;
				q = 2;
				break;
			}
			i++;
		}
	}
}

static void
arrange_dwindle(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n, int an, int ai)
{
	arrange_fibonacci(m, x, y, h, w, ih, iv, n, an, ai, 1);
}

static void
arrange_dwindle_cfacts(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n, int an, int ai)
{
	arrange_fibonacci_cfacts(m, x, y, h, w, ih, iv, n, an, ai, 1);
}

static void
arrange_spiral(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n, int an, int ai)
{
	arrange_fibonacci(m, x, y, h, w, ih, iv, n, an, ai, 0);
}

static void
arrange_spiral_cfacts(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n, int an, int ai)
{
	arrange_fibonacci_cfacts(m, x, y, h, w, ih, iv, n, an, ai, 0);
}

static void
arrange_tatami(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n, int an, int ai)
{
	unsigned int i, j, nx, ny, nw, nh, tnx, tny, tnw, tnh, nhrest, hrest, wrest, areas, mats, cats;
	Client *c;

	nx = x;
	ny = y;
	nw = w;
	nh = h;

	mats = an / 5;
	cats = an % 5;
	hrest = 0;
	wrest = 0;

	areas = mats + (cats > 0);
	nh = (h - ih * (areas - 1)) / areas;
	nhrest = (h - ih * (areas - 1)) % areas;

	for (i = 0, j = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), j++) {
		if (j >= ai && j < (ai + an)) {

			tnw = nw;
			tnx = nx;
			tnh = nh;
			tny = ny;

			if (j < ai + cats) {
				/* Arrange cats (all excess clients that can't be tiled as mats). Cats sleep on mats. */

	 			switch (cats) {
				case 1: // fill
					break;
				case 2: // up and down
					if ((i % 5) == 0) //up
						tnh = (nh - ih) / 2 + (nh - ih) % 2;
					else if ((i % 5) == 1) { //down
						tny += (nh - ih) / 2 + (nh - ih) % 2 + ih;
						tnh = (nh - ih) / 2;
					}
					break;
				case 3: //bottom, up-left and up-right
					if ((i % 5) == 0) { // up-left
						tnw = (nw - iv) / 2 + (nw - iv) % 2;
						tnh = (nh - ih) * 2 / 3 + (nh - ih) * 2 % 3;
					} else if ((i % 5) == 1) { // up-right
						tnx += (nw - iv) / 2 + (nw - iv) % 2 + iv;
						tnw = (nw - iv) / 2;
						tnh = (nh - ih) * 2 / 3 + (nh - ih) * 2 % 3;
					} else if ((i % 5) == 2) { //bottom
						tnh = (nh - ih) / 3;
						tny += (nh - ih) * 2 / 3 + (nh - ih) * 2 % 3 + ih;
					}
					break;
				case 4: // bottom, left, right and top
					if ((i % 5) == 0) { //top
						hrest = (nh - 2 * ih) % 4;
						tnh = (nh - 2 * ih) / 4 + (hrest ? 1 : 0);
					} else if ((i % 5) == 1) { // left
						tnw = (nw - iv) / 2 + (nw - iv) % 2;
						tny += (nh - 2 * ih) / 4 + (hrest ? 1 : 0) + ih;
						tnh = (nh - 2 * ih) * 2 / 4 + (hrest > 1 ? 1 : 0);
					} else if ((i % 5) == 2) { // right
						tnx += (nw - iv) / 2 + (nw - iv) % 2 + iv;
						tnw = (nw - iv) / 2;
						tny += (nh - 2 * ih) / 4 + (hrest ? 1 : 0) + ih;
						tnh = (nh - 2 * ih) * 2 / 4 + (hrest > 1 ? 1 : 0);
					} else if ((i % 5) == 3) { // bottom
						tny += (nh - 2 * ih) / 4 + (hrest ? 1 : 0) + (nh - 2 * ih) * 2 / 4 + (hrest > 1 ? 1 : 0) + 2 * ih;
						tnh = (nh - 2 * ih) / 4 + (hrest > 2 ? 1 : 0);
					}
					break;
				}

			} else {
				/* Arrange mats. One mat is a collection of five clients arranged tatami style */

				if (((i - cats) % 5) == 0) {
					if ((cats > 0) || ((i - cats) >= 5)) {
						tny = ny = ny + nh + (nhrest > 0 ? 1 : 0) + ih;
						--nhrest;
					}
				}

				switch ((i - cats) % 5) {
				case 0: // top-left-vert
					wrest = (nw - 2 * iv) % 3;
					hrest = (nh - 2 * ih) % 3;
					tnw = (nw - 2 * iv) / 3 + (wrest ? 1 : 0);
					tnh = (nh - 2 * ih) * 2 / 3 + hrest + iv;
					break;
				case 1: // top-right-hor
					tnx += (nw - 2 * iv) / 3 + (wrest ? 1 : 0) + iv;
					tnw = (nw - 2 * iv) * 2 / 3 + (wrest > 1 ? 1 : 0) + iv;
					tnh = (nh - 2 * ih) / 3 + (hrest ? 1 : 0);
					break;
				case 2: // center
					tnx += (nw - 2 * iv) / 3 + (wrest ? 1 : 0) + iv;
					tnw = (nw - 2 * iv) / 3 + (wrest > 1 ? 1 : 0);
					tny += (nh - 2 * ih) / 3 + (hrest ? 1 : 0) + ih;
					tnh = (nh - 2 * ih) / 3 + (hrest > 1 ? 1 : 0);
					break;
				case 3: // bottom-right-vert
					tnx += (nw - 2 * iv) * 2 / 3 + wrest + 2 * iv;
					tnw = (nw - 2 * iv) / 3;
					tny += (nh - 2 * ih) / 3 + (hrest ? 1 : 0) + ih;
					tnh = (nh - 2 * ih) * 2 / 3 + hrest + iv;
					break;
				case 4: // (oldest) bottom-left-hor
					tnw = (nw - 2 * iv) * 2 / 3 + wrest + iv;
					tny += (nh - 2 * ih) * 2 / 3 + hrest + 2 * iv;
					tnh = (nh - 2 * ih) / 3;
					break;
				}

			}

			resize(c, tnx, tny, tnw - 2 * c->bw, tnh - 2 * c->bw, False);
			++i;
		}
	}
}

static void
arrange_tatami_cfacts(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n, int an, int ai)
{
	Client *clients[5] = { NULL, NULL, NULL, NULL, NULL };
	unsigned int j, s, nx, ny, nw, nh, tnw, tmw, tnh, tmh, areas, mats, cats, nhrest;
	Client *t, *a, *b, *c, *d, *e;

	nx = x;
	ny = y;
	nw = w;
	nh = h;

	mats = an / 5;
	cats = an % 5;

	areas = mats + (cats > 0);
	if (cats == 0)
		cats = 5;
	nh = (h - ih * (areas - 1)) / areas;
	nhrest = (h - ih * (areas - 1)) % areas;

	for (j = 0, s = 0, t = nexttiled(m->clients); t; t = nexttiled(t->next), j++) {
		if (j >= ai && j < (ai + an)) {
			clients[s] = t;
			++s;

			if (s < cats)
				continue;

			a = clients[0];
			b = clients[1];
			c = clients[2];
			d = clients[3];
			e = clients[4];
			s = 0;

			if (cats < 5) {
				/* Arrange cats (all excess clients that can't be tiled as mats). Cats sleep on mats. */
				switch (cats) {
				case 1: // fill
					resize(a, nx, ny, nw - 2 * a->bw, nh - 2 * a->bw, False);
					break;
				case 2: // up and down
					tnh = (nh - ih) * (a->cfact / (a->cfact + b->cfact));
					resize(a, nx, ny, nw - 2 * a->bw, tnh - 2 * a->bw, False);
					resize(b, nx, ny + tnh + ih, nw - 2 * b->bw, (nh - tnh - ih) - 2 * b->bw, False);
					break;
				case 3: // bottom, up-left and up-right
					tnw = (nw - iv) * (a->cfact / (a->cfact + b->cfact));
					tnh = (nh - ih) * (c->cfact / (a->cfact + b->cfact + c->cfact));
					resize(a, nx, ny, tnw - 2 * a->bw, nh - ih - tnh - 2 * a->bw, False);
					resize(b, nx + tnw + iv, ny, nw - iv - tnw - 2 * b->bw, nh - ih - tnh - 2 * b->bw, False);
					resize(c, nx, ny + nh - tnh, nw - 2 * c->bw, tnh - 2 * c->bw, False);
					break;
				case 4: // bottom, left, right and top
					tnw = (nw - iv) * (b->cfact / (b->cfact + c->cfact));
					tnh = (nh - 2 * ih) * (a->cfact / (a->cfact + b->cfact + c->cfact + d->cfact));
					tmh = (nh - 2 * ih) * ((b->cfact + c->cfact) / (a->cfact + b->cfact + c->cfact + d->cfact));
					resize(a, nx, ny, nw - 2 * a->bw, tnh - 2 * a->bw, False);
					resize(b, nx, ny + tnh + iv, tnw - 2 * a->bw, tmh - 2 * a->bw, False);
					resize(c, nx + iv + tnw, ny + tnh + iv, nw - iv - tnw - 2 * a->bw, tmh - 2 * a->bw, False);
					resize(d, nx, ny + tnh + 2 * iv + tmh, nw - 2 * a->bw, nh - 2 * iv - tnh - tmh - 2 * a->bw, False);
					break;
				}

				cats = 5;
			} else {
				/* Arrange mats. One mat is a collection of five clients arranged tatami style */
				tnw = (nw - 2 * ih) * (a->cfact / (a->cfact + c->cfact + d->cfact));
				tmw = (nw - 2 * ih) * (d->cfact / (a->cfact + c->cfact + d->cfact));
				tnh = (nh - 2 * ih) * (e->cfact / (b->cfact + c->cfact + e->cfact));
				tmh = (nh - 2 * ih) * (b->cfact / (b->cfact + c->cfact + e->cfact));
				resize(a, nx, ny, tnw - 2 * a->bw, nh - tnh - ih - 2 * a->bw, False);
				resize(b, nx + tnw + iv, ny, nw - iv - tnw - 2 * a->bw, tmh - 2 * a->bw, False);
				resize(c, nx + tnw + iv, ny + tmh + ih, nw - tnw - tmw - 2 * iv - 2 * c->bw, nh - tnh - tmh - 2 * ih - 2 * c->bw, False);
				resize(d, nx + nw - tmw, ny + tmh + ih, tmw - 2 * d->bw, nh - tmh - ih - 2 * d->bw, False);
				resize(e, nx, ny + nh - tnh, nw - tmw - iv - 2 * e->bw, tnh - 2 * e->bw, False);
			}

			ny += nh + ih + (nhrest > 0 ? 1 : 0);
			--nhrest;
		}
	}
}

static void
flextile(Monitor *m)
{
	unsigned int n;
	int oh = 0, ov = 0, ih = 0, iv = 0; // gaps outer/inner horizontal/vertical
	getgaps(m, &oh, &ov, &ih, &iv, &n);

	if (m->layout->preset.layout != m->ltaxis[LAYOUT] ||
			m->layout->preset.masteraxis != m->ltaxis[MASTER] ||
			m->layout->preset.stack1axis != m->ltaxis[STACK] ||
			m->layout->preset.stack2axis != m->ltaxis[STACK2])
		setflexsymbols(m, n);
	else if (m->layout->preset.symbolfunc != NULL)
		m->layout->preset.symbolfunc(m, n);

	if (n == 0)
		return;

	if (enabled(SmartGapsMonocle)) {
		/* Apply outer gap factor if full screen monocle */
		if (abs(m->ltaxis[MASTER]) == MONOCLE && (abs(m->ltaxis[LAYOUT]) == NO_SPLIT || n <= m->nmaster)) {
			oh = m->gappoh * smartgaps_fact;
			ov = m->gappov * smartgaps_fact;
		}
	}

	(&flexlayouts[abs(m->ltaxis[LAYOUT])])->arrange(m, m->wx + ov, m->wy + oh, m->wh - 2*oh, m->ww - 2*ov, ih, iv, n);
	return;
}

static void
setflexsymbols(Monitor *m, unsigned int n)
{
	int l;
	char sym1, sym2, sym3;
	Client *c;

	if (n == 0)
		for (c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);

	l = abs(m->ltaxis[LAYOUT]);
	if (m->ltaxis[MASTER] == MONOCLE && (l == NO_SPLIT || !m->nmaster || n <= m->nmaster)) {
		monoclesymbols(m, n);
		return;
	}

	if (m->ltaxis[STACK] == MONOCLE && (l == SPLIT_VERTICAL || l == SPLIT_HORIZONTAL_FIXED)) {
		decksymbols(m, n);
		return;
	}

	/* Layout symbols */
	if (l == NO_SPLIT || !m->nmaster) {
		sym1 = sym2 = sym3 = (int)tilesymb[m->ltaxis[MASTER]];
	} else {
		sym2 = layoutsymb[l];
		if (m->ltaxis[LAYOUT] < 0) {
			sym1 = tilesymb[m->ltaxis[STACK]];
			sym3 = tilesymb[m->ltaxis[MASTER]];
		} else {
			sym1 = tilesymb[m->ltaxis[MASTER]];
			sym3 = tilesymb[m->ltaxis[STACK]];
		}
	}

	snprintf(m->ltsymbol, sizeof m->ltsymbol, "%c%c%c", sym1, sym2, sym3);
}

static void
monoclesymbols(Monitor *m, unsigned int n)
{
	if (n > 0)
		snprintf(m->ltsymbol, sizeof m->ltsymbol, "[%d]", n);
	else
		snprintf(m->ltsymbol, sizeof m->ltsymbol, "[M]");
}

static void
decksymbols(Monitor *m, unsigned int n)
{
	if (n > m->nmaster)
		snprintf(m->ltsymbol, sizeof m->ltsymbol, "[]%d", n);
	else
		snprintf(m->ltsymbol, sizeof m->ltsymbol, "[D]");
}

/* Mirror layout axis for flextile */
void
mirrorlayout(const Arg *arg)
{
	if (!selmon->layout->arrange)
		return;
	selmon->ltaxis[LAYOUT] *= -1;
	selmon->pertag->ltaxis[selmon->pertag->curtag][0] = selmon->ltaxis[LAYOUT];
	arrangemon(selmon);
}

/* Rotate layout axis for flextile */
void
rotatelayoutaxis(const Arg *arg)
{
	int incr = (arg->i > 0 ? 1 : -1);
	int axis = labs(arg->i) - 1;

	if (!selmon->layout->arrange)
		return;
	if (axis == LAYOUT) {
		if (selmon->ltaxis[LAYOUT] >= 0) {
			selmon->ltaxis[LAYOUT] += incr;
			if (selmon->ltaxis[LAYOUT] >= LAYOUT_LAST)
				selmon->ltaxis[LAYOUT] = 0;
			else if (selmon->ltaxis[LAYOUT] < 0)
				selmon->ltaxis[LAYOUT] = LAYOUT_LAST - 1;
		} else {
			selmon->ltaxis[LAYOUT] -= incr;
			if (selmon->ltaxis[LAYOUT] <= -LAYOUT_LAST)
				selmon->ltaxis[LAYOUT] = 0;
			else if (selmon->ltaxis[LAYOUT] > 0)
				selmon->ltaxis[LAYOUT] = -LAYOUT_LAST + 1;
		}
	} else {
		selmon->ltaxis[axis] += incr;
		if (selmon->ltaxis[axis] >= AXIS_LAST)
			selmon->ltaxis[axis] = 0;
		else if (selmon->ltaxis[axis] < 0)
			selmon->ltaxis[axis] = AXIS_LAST - 1;
	}
	selmon->pertag->ltaxis[selmon->pertag->curtag][axis] = selmon->ltaxis[axis];
	arrangemon(selmon);
	setflexsymbols(selmon, 0);
}
