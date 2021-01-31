struct Pertag {
	unsigned int curtag, prevtag; /* current and previous tag */
	int nmasters[NUMTAGS + 1]; /* number of windows in master area */
	int nstacks[NUMTAGS + 1]; /* number of windows in primary stack area */
	int ltaxis[NUMTAGS + 1][LTAXIS_LAST];
	const Layout *ltidxs[NUMTAGS + 1][3]; /* matrix of tags and layouts indexes  */
	float mfacts[NUMTAGS + 1]; /* mfacts per tag */
	unsigned int sellts[NUMTAGS + 1]; /* selected layouts */
	int showbars[NUMTAGS + 1]; /* display bar for the current tag */
	Client *prevzooms[NUMTAGS + 1]; /* store zoom information */
	int enablegaps[NUMTAGS + 1];
};