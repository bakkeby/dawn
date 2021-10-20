void
tagtoleft(const Arg *arg)
{
	Monitor *m = selmon;
	if (m->sel && __builtin_popcount(m->tags & TAGMASK) == 1 && m->tags) {
		if (enabled(Desktop))
			for (m = mons; m; m = m->next)
				m->sel->tags >>= 1;
		else
			m->sel->tags >>= 1;
		focus(NULL);
		arrange(m);
	}
}

void
tagtoright(const Arg *arg)
{
	Monitor *m = selmon;
	if (m->sel && __builtin_popcount(m->tags & TAGMASK) == 1 && m->tags & (TAGMASK >> 1)) {
		if (enabled(Desktop))
			for (m = mons; m; m = m->next)
				m->sel->tags <<= 1;
		else
			m->sel->tags <<= 1;
		focus(NULL);
		arrange(m);
	}
}

void
viewtoleft(const Arg *arg)
{
	Monitor *m = selmon;
	if (__builtin_popcount(m->tags & TAGMASK) == 1 && m->tags > 1) {
		if (enabled(Desktop)) {
			for (m = mons; m; m = m->next) {
				m->prevtags = m->tags;
				m->tags >>= 1;
			}
		} else {
			m->prevtags = m->tags;
			m->tags >>= 1;
		}
		focus(NULL);
		arrange(m);
	}
}

void
viewtoright(const Arg *arg)
{
	Monitor *m = selmon;
	if (__builtin_popcount(m->tags & TAGMASK) == 1 && m->tags & (TAGMASK >> 1)) {
		if (enabled(Desktop)) {
			for (m = mons; m; m = m->next) {
				m->prevtags = m->tags;
				m->tags <<= 1;
			}
		} else {
			m->prevtags = m->tags;
			m->tags <<= 1;
		}
		focus(NULL);
		arrange(m);
	}
}

void
tagandviewtoleft(const Arg *arg)
{
	Monitor *m = selmon;
	if (m->sel != NULL && __builtin_popcount(m->tags & TAGMASK) == 1 && m->tags > 1) {
		selmon->sel->tags >>= 1;
		if (enabled(Desktop)) {
			for (m = mons; m; m = m->next) {
				m->prevtags = m->tags;
				m->tags >>= 1;
			}
		} else {
			m->prevtags = m->tags;
			m->tags >>= 1;
		}
		focus(selmon->sel);
		arrange(m);
	}
}

void
tagandviewtoright(const Arg *arg)
{
	Monitor *m = selmon;
	if (m->sel != NULL && __builtin_popcount(m->tags & TAGMASK) == 1	&& m->tags & (TAGMASK >> 1)) {
		selmon->sel->tags <<= 1;
		if (enabled(Desktop)) {
			for (m = mons; m; m = m->next) {
				m->prevtags = m->tags;
				m->tags <<= 1;
			}
		} else {
			m->prevtags = m->tags;
			m->tags <<= 1;
		}
		focus(selmon->sel);
		arrange(m);
	}
}
