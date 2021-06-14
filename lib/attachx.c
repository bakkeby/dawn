void
attachx(Client *c, unsigned long mode, Monitor *m)
{
	if (!c)
		return;

	Client *at, *last;
	unsigned int n;
	unsigned long attachmode
		= mode
		? mode
		: c->flags & AttachMaster
		? AttachMaster
		: c->flags & AttachAbove
		? AttachAbove
		: c->flags & AttachAside
		? AttachAside
		: c->flags & AttachBelow
		? AttachBelow
		: c->flags & AttachBottom
		? AttachBottom
		: attachdefault;

	for (last = c; last && last->next; last = last->next);

	if (!m)
		m = c->mon;
	else if (c->mon != m) {
		for (at = c; at; at = at->next) {
			at->mon = m;
			at->reverttags = 0;
			updateclientdesktop(at);
		}
	}

	if (c->idx > 0) { /* then the client has a designated position in the client list */
		for (at = m->clients; at; at = at->next)
			if (c->idx < at->idx) {
				last->next = at;
				m->clients = c;
				return;
			} else if (at->idx <= c->idx && (!at->next || c->idx <= at->next->idx)) {
				last->next = at->next;
				at->next = c;
				return;
			}
	}

	if (attachmode == AttachAbove) {
		if (!(m->sel == NULL || m->sel == m->clients || ISFLOATING(m->sel))) {
			for (at = m->clients; at->next != m->sel; at = at->next);
			last->next = at->next;
			at->next = c;
			return;
		}
	} else if (attachmode == AttachAside) {
		for (at = m->clients, n = 0; at; at = at->next)
			if (!ISFLOATING(at))
				if (++n >= m->nmaster)
					break;

		if (at && m->nmaster) {
			last->next = at->next;
			at->next = c;
			return;
		}
	} else if (attachmode == AttachBelow) {
		if (!(m->sel == NULL || m->sel == c || ISFLOATING(m->sel))) {
			last->next = m->sel->next;
			m->sel->next = c;
			return;
		}
	} else if (attachmode == AttachBottom) {
		for (at = m->clients; at && at->next; at = at->next);
		if (at) {
			at->next = c;
			last->next = NULL;
			return;
		}
	}

	/* Attach master (default) */
	last->next = m->clients;
	m->clients = c;
}
