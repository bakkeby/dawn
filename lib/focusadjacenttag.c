void
tagtoleft(const Arg *arg)
{
	Monitor *m;
	if (selmon->sel != NULL
	&& __builtin_popcount(selmon->tagset[selmon->seltags] & TAGMASK) == 1
	&& selmon->tagset[selmon->seltags] > 1) {
		if (enabled(Desktop)) {
			for (m = mons; m; m = m->next)
				m->sel->tags >>= 1;
			focus(NULL);
			arrange(NULL);
		} else {
			selmon->sel->tags >>= 1;
			focus(NULL);
			arrange(selmon);
		}
	}
}

void
tagtoright(const Arg *arg)
{
	Monitor *m;
	if (selmon->sel != NULL
	&& __builtin_popcount(selmon->tagset[selmon->seltags] & TAGMASK) == 1
	&& selmon->tagset[selmon->seltags] & (TAGMASK >> 1)) {
		if (enabled(Desktop)) {
			for (m = mons; m; m = m->next)
				m->sel->tags <<= 1;
			focus(NULL);
			arrange(NULL);
		} else {
			selmon->sel->tags <<= 1;
			focus(NULL);
			arrange(selmon);
		}
	}
}

void
viewtoleft(const Arg *arg)
{
	Monitor *m;
	if (__builtin_popcount(selmon->tagset[selmon->seltags] & TAGMASK) == 1
	&& selmon->tagset[selmon->seltags] > 1) {
		if (enabled(Desktop)) {
			for (m = mons; m; m = m->next) {
				m->seltags ^= 1; /* toggle sel tagset */
				m->tagset[m->seltags] = m->tagset[m->seltags ^ 1] >> 1;
			}
			focus(NULL);
			arrange(NULL);
		} else {
			selmon->seltags ^= 1; /* toggle sel tagset */
			selmon->tagset[selmon->seltags] = selmon->tagset[selmon->seltags ^ 1] >> 1;
			focus(NULL);
			arrange(selmon);
		}
	}
}

void
viewtoright(const Arg *arg)
{
	Monitor *m;
	if (__builtin_popcount(selmon->tagset[selmon->seltags] & TAGMASK) == 1
	&& selmon->tagset[selmon->seltags] & (TAGMASK >> 1)) {
		if (enabled(Desktop)) {
			for (m = mons; m; m = m->next) {
				m->seltags ^= 1; /* toggle sel tagset */
				m->tagset[m->seltags] = m->tagset[m->seltags ^ 1] << 1;
			}
			focus(NULL);
			arrange(NULL);
		} else {
			selmon->seltags ^= 1; /* toggle sel tagset */
			selmon->tagset[selmon->seltags] = selmon->tagset[selmon->seltags ^ 1] << 1;
			focus(NULL);
			arrange(selmon);
		}
	}
}

void
tagandviewtoleft(const Arg *arg)
{
	Monitor *m;
	if (__builtin_popcount(selmon->tagset[selmon->seltags] & TAGMASK) == 1
	&& selmon->tagset[selmon->seltags] > 1) {
		if (enabled(Desktop)) {
			for (m = mons; m; m = m->next) {
				m->sel->tags >>= 1;
				m->seltags ^= 1; /* toggle sel tagset */
				m->tagset[m->seltags] = m->tagset[m->seltags ^ 1] >> 1;
			}
			focus(selmon->sel);
			arrange(NULL);
		} else {
			selmon->sel->tags >>= 1;
			selmon->seltags ^= 1; /* toggle sel tagset */
			selmon->tagset[selmon->seltags] = selmon->tagset[selmon->seltags ^ 1] >> 1;
			focus(selmon->sel);
			arrange(selmon);
		}
	}
}

void
tagandviewtoright(const Arg *arg)
{
	Monitor *m;
	if (__builtin_popcount(selmon->tagset[selmon->seltags] & TAGMASK) == 1
	&& selmon->tagset[selmon->seltags] & (TAGMASK >> 1)) {
		if (enabled(Desktop)) {
			for (m = mons; m; m = m->next) {
				m->sel->tags <<= 1;
				m->seltags ^= 1; /* toggle sel tagset */
				m->tagset[m->seltags] = m->tagset[m->seltags ^ 1] << 1;
			}
			focus(selmon->sel);
			arrange(NULL);
		} else {
			selmon->sel->tags <<= 1;
			selmon->seltags ^= 1; /* toggle sel tagset */
			selmon->tagset[selmon->seltags] = selmon->tagset[selmon->seltags ^ 1] << 1;
			focus(selmon->sel);
			arrange(selmon);
		}
	}
}