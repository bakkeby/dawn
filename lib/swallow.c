#include <X11/Xlib-xcb.h>
#include <xcb/res.h>
#ifdef __OpenBSD__
#include <sys/sysctl.h>
#include <kvm.h>
#endif /* __OpenBSD__ */

static int scanner;
static xcb_connection_t *xcon;

int
swallow(Client *t, Client *c)
{
	if (disabled(Swallow) || NOSWALLOW(c) || ISTERMINAL(c))
		return 0;
	if (!RULED(c) && disabled(SwallowFloating) && ISFLOATING(c))
		return 0;

	if (ISFULLSCREEN(t))
		setfullscreen(c, 1, ISFAKEFULLSCREEN(t));

	replaceclient(t, c);
	addflag(c, IgnoreCfgReq);
	c->swallowing = t;

	return 1;
}

void
replaceclient(Client *old, Client *new)
{
	Client *c = NULL;
	Monitor *mon = old->mon;

	new->mon = mon;
	new->tags = old->tags;
	setflag(new, Floating, ISFLOATING(old));

	new->next = old->next;
	new->snext = old->snext;

	if (old == mon->clients)
		mon->clients = new;
	else {
		for (c = mon->clients; c && c->next != old; c = c->next);
		c->next = new;
	}

	if (old == mon->stack)
		mon->stack = new;
	else {
		for (c = mon->stack; c && c->snext != old; c = c->snext);
		c->snext = new;
	}

	old->next = NULL;
	old->snext = NULL;

	XMoveWindow(dpy, old->win, WIDTH(old) * -2, old->y);

	if (ISVISIBLE(new) && !ISFULLSCREEN(new)) {
		if (ISFLOATING(new))
			resize(new, old->x, old->y, new->w, new->h, 0);
		else
			resize(new, old->x, old->y, old->w, old->h, 0);
	}
}

void
unswallow(Client *c)
{
	replaceclient(c, c->swallowing);
	c->swallowing = NULL;
}

pid_t
winpid(Window w)
{
	pid_t result = 0;

	#ifdef __linux__
	xcb_res_client_id_spec_t spec = {0};
	spec.client = w;
	spec.mask = XCB_RES_CLIENT_ID_MASK_LOCAL_CLIENT_PID;

	xcb_generic_error_t *e = NULL;
	xcb_res_query_client_ids_cookie_t c = xcb_res_query_client_ids(xcon, 1, &spec);
	xcb_res_query_client_ids_reply_t *r = xcb_res_query_client_ids_reply(xcon, c, &e);

	if (!r)
		return (pid_t)0;

	xcb_res_client_id_value_iterator_t i = xcb_res_query_client_ids_ids_iterator(r);
	for (; i.rem; xcb_res_client_id_value_next(&i)) {
		spec = i.data->spec;
		if (spec.mask & XCB_RES_CLIENT_ID_MASK_LOCAL_CLIENT_PID) {
			uint32_t *t = xcb_res_client_id_value_value(i.data);
			result = *t;
			break;
		}
	}

	free(r);

	if (result == (pid_t)-1)
		result = 0;

	#endif /* __linux__ */
	#ifdef __OpenBSD__
	Atom type;
	int format;
	unsigned long len, bytes;
	unsigned char *prop;
	pid_t ret;

	if (XGetWindowProperty(dpy, w, XInternAtom(dpy, "_NET_WM_PID", 1), 0, 1, False, AnyPropertyType, &type, &format, &len, &bytes, &prop) != Success || !prop)
		return 0;

	ret = *(pid_t*)prop;
	XFree(prop);
	result = ret;
	#endif /* __OpenBSD__ */

	return result;
}

pid_t
getparentprocess(pid_t p)
{
	unsigned int v = 0;

#ifdef __linux__
	FILE *f;
	char buf[256];
	snprintf(buf, sizeof(buf) - 1, "/proc/%u/stat", (unsigned)p);

	if (!(f = fopen(buf, "r")))
		return (pid_t)0;

	if (fscanf(f, "%*u %*s %*c %u", (unsigned *)&v) != 1)
		v = (pid_t)0;
	fclose(f);
#endif /* __linux__ */
#ifdef __OpenBSD__
	int n;
	kvm_t *kd;
	struct kinfo_proc *kp;

	kd = kvm_openfiles(NULL, NULL, NULL, KVM_NO_FILES, NULL);
	if (!kd)
		return 0;

	kp = kvm_getprocs(kd, KERN_PROC_PID, p, sizeof(*kp), &n);
	v = kp->p_ppid;
#endif /* __OpenBSD__ */
	return (pid_t)v;
}

int
isdescprocess(pid_t p, pid_t c)
{
	while (p != c && c != 0)
		c = getparentprocess(c);

	return (int)c;
}

Client *
termforwin(const Client *w)
{
	Client *c;
	Monitor *m;

	if (!w->pid || ISTERMINAL(w))
		return NULL;

	c = selmon->sel;
	if (c && ISTERMINAL(c) && !c->swallowing && c->pid && isdescprocess(c->pid, w->pid))
		return c;

	for (m = mons; m; m = m->next) {
		for (c = m->clients; c; c = c->next) {
			if (ISTERMINAL(c) && !c->swallowing && c->pid && isdescprocess(c->pid, w->pid))
				return c;
		}
	}

	return NULL;
}

Client *
swallowingclient(Window w)
{
	Client *c;
	Monitor *m;

	for (m = mons; m; m = m->next) {
		for (c = m->clients; c; c = c->next) {
			if (c->swallowing && c->swallowing->win == w)
				return c;
		}
	}

	return NULL;
}
