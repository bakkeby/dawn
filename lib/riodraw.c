static int riodimensions[4] = { -1, -1, -1, -1 };
static pid_t riopid = 0;

int
riodraw(Client *c, const char slopstyle[])
{
	int i;
	char str[100];
	char strout[100];
	char tmpstring[30] = {0};
	char slopcmd[100] = "slop -f x%xx%yx%wx%hx ";
	int firstchar = 0;
	int counter = 0;

	strcat(slopcmd, slopstyle);
	FILE *fp = popen(slopcmd, "r");

	while (fgets(str, 100, fp) != NULL)
		strcat(strout, str);

	pclose(fp);

	if (strlen(strout) < 6)
		return 0;

	for (i = 0; i < strlen(strout); i++){
		if (!firstchar) {
			if (strout[i] == 'x')
				firstchar = 1;
			continue;
		}

		if (strout[i] != 'x')
			tmpstring[strlen(tmpstring)] = strout[i];
		else {
			riodimensions[counter] = atoi(tmpstring);
			counter++;
			memset(tmpstring,0,strlen(tmpstring));
		}
	}

	if (riodimensions[0] <= -40 || riodimensions[1] <= -40 || riodimensions[2] <= 50 || riodimensions[3] <= 50) {
		riodimensions[3] = -1;
		return 0;
	}

	if (c) {
		rioposition(c, riodimensions[0], riodimensions[1], riodimensions[2], riodimensions[3]);
		drawbar(c->mon);
		arrange(c->mon);
		return 0;
	}

	return 1;
}

void
rioposition(Client *c, int x, int y, int w, int h)
{
	Monitor *m;
	if ((m = recttomon(x, y, w, h)) && m != c->mon) {
		detach(c);
		detachstack(c);
		arrange(c->mon);
		drawbar(c->mon);
		c->mon = m;
		c->tags = m->tags;
		attachx(c, AttachBottom, m);
		attachstack(c);
		selmon = m;
		focus(c);
	}

	if (enabled(RioDrawIncludeBorders)) {
		c->x = x;
		c->y = y;
		c->w = w - (c->bw * 2);
		c->h = h - (c->bw * 2);
	} else {
		c->x = x - c->bw;
		c->y = y - c->bw;
		c->w = w;
		c->h = h;
	}

	SETFLOATING(c);
	savefloats(c);
	riodimensions[3] = -1;
	riopid = 0;
}

/* drag out an area using slop and resize the selected window to it */
void
rioresize(const Arg *arg)
{
	Client *c = CLIENT;
	if (c)
		riodraw(c, slopresizestyle);
}

/* spawn a new window and drag out an area using slop to postiion it */
void
riospawn(const Arg *arg)
{
	if (enabled(RioDrawSpawnAsync)) {
		riopid = spawncmd(arg);
		riodraw(NULL, slopspawnstyle);
	} else if (riodraw(NULL, slopspawnstyle))
		riopid = spawncmd(arg);
}
