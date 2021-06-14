void
loadxrdb()
{
	Display *display;
	char * resm;
	XrmDatabase xrdb;
	char *type;
	XrmValue value;

	if (disabled(Xresources))
		return;

	display = XOpenDisplay(NULL);

	if (display != NULL) {
		resm = XResourceManagerString(display);

		if (resm != NULL) {
			xrdb = XrmGetStringDatabase(resm);

			if (xrdb != NULL) {
				XRDB_LOAD_COLOR("dawn.normfgcolor", normfgcolor);
				XRDB_LOAD_COLOR("dawn.normbgcolor", normbgcolor);
				XRDB_LOAD_COLOR("dawn.normbordercolor", normbordercolor);
				XRDB_LOAD_COLOR("dawn.selfgcolor", selfgcolor);
				XRDB_LOAD_COLOR("dawn.selbgcolor", selbgcolor);
				XRDB_LOAD_COLOR("dawn.selbordercolor", selbordercolor);
				XRDB_LOAD_COLOR("dawn.titlenormfgcolor", titlenormfgcolor);
				XRDB_LOAD_COLOR("dawn.titlenormbgcolor", titlenormbgcolor);
				XRDB_LOAD_COLOR("dawn.titlenormbordercolor", titlenormbordercolor);
				XRDB_LOAD_COLOR("dawn.titleselfgcolor", titleselfgcolor);
				XRDB_LOAD_COLOR("dawn.titleselbgcolor", titleselbgcolor);
				XRDB_LOAD_COLOR("dawn.titleselbordercolor", titleselbordercolor);
				XRDB_LOAD_COLOR("dawn.tagsnormfgcolor", tagsnormfgcolor);
				XRDB_LOAD_COLOR("dawn.tagsnormbgcolor", tagsnormbgcolor);
				XRDB_LOAD_COLOR("dawn.tagsnormbordercolor", tagsnormbordercolor);
				XRDB_LOAD_COLOR("dawn.tagsselfgcolor", tagsselfgcolor);
				XRDB_LOAD_COLOR("dawn.tagsselbgcolor", tagsselbgcolor);
				XRDB_LOAD_COLOR("dawn.tagsselbordercolor", tagsselbordercolor);
				XRDB_LOAD_COLOR("dawn.hidfgcolor", hidfgcolor);
				XRDB_LOAD_COLOR("dawn.hidbgcolor", hidbgcolor);
				XRDB_LOAD_COLOR("dawn.hidbordercolor", hidbordercolor);
				XRDB_LOAD_COLOR("dawn.urgfgcolor", urgfgcolor);
				XRDB_LOAD_COLOR("dawn.urgbgcolor", urgbgcolor);
				XRDB_LOAD_COLOR("dawn.urgbordercolor", urgbordercolor);
				XRDB_LOAD_COLOR("dawn.markedfgcolor", markedfgcolor);
				XRDB_LOAD_COLOR("dawn.markedbgcolor", markedbgcolor);
				XRDB_LOAD_COLOR("dawn.markedbordercolor", markedbordercolor);
				XRDB_LOAD_COLOR("dawn.scratchnormcolor", scratchnormcolor);
				XRDB_LOAD_COLOR("dawn.scratchnormbgcolor", scratchnormbgcolor);
				XRDB_LOAD_COLOR("dawn.scratchnormbordercolor", scratchnormbordercolor);
				XRDB_LOAD_COLOR("dawn.scratchselcolor", scratchselcolor);
				XRDB_LOAD_COLOR("dawn.scratchselbgcolor", scratchselbgcolor);
				XRDB_LOAD_COLOR("dawn.scratchselbordercolor", scratchselbordercolor);
				XRDB_LOAD_COLOR("dawn.normTTBbgcolor", normTTBbgcolor);
				XRDB_LOAD_COLOR("dawn.normLTRbgcolor", normLTRbgcolor);
				XRDB_LOAD_COLOR("dawn.normMONObgcolor", normMONObgcolor);
				XRDB_LOAD_COLOR("dawn.normGRIDbgcolor", normGRIDbgcolor);
				XRDB_LOAD_COLOR("dawn.normGRIDCbgcolor", normGRIDCbgcolor);
				XRDB_LOAD_COLOR("dawn.normGRD1bgcolor", normGRD1bgcolor);
				XRDB_LOAD_COLOR("dawn.normGRD2bgcolor", normGRD2bgcolor);
				XRDB_LOAD_COLOR("dawn.normGRDMbgcolor", normGRDMbgcolor);
				XRDB_LOAD_COLOR("dawn.normHGRDbgcolor", normHGRDbgcolor);
				XRDB_LOAD_COLOR("dawn.normDWDLbgcolor", normDWDLbgcolor);
				XRDB_LOAD_COLOR("dawn.normDWDLCbgcolor", normDWDLCbgcolor);
				XRDB_LOAD_COLOR("dawn.normSPRLbgcolor", normSPRLbgcolor);
				XRDB_LOAD_COLOR("dawn.normSPRLCbgcolor", normSPRLCbgcolor);
				XRDB_LOAD_COLOR("dawn.normSTTMIbgcolor", normTTMIbgcolor);
				XRDB_LOAD_COLOR("dawn.normSTTMICbgcolor", normTTMICbgcolor);
				XRDB_LOAD_COLOR("dawn.normfloatbgcolor", normfloatbgcolor);
				XRDB_LOAD_COLOR("dawn.actTTBbgcolor", actTTBbgcolor);
				XRDB_LOAD_COLOR("dawn.actLTRbgcolor", actLTRbgcolor);
				XRDB_LOAD_COLOR("dawn.actMONObgcolor", actMONObgcolor);
				XRDB_LOAD_COLOR("dawn.actGRIDbgcolor", actGRIDbgcolor);
				XRDB_LOAD_COLOR("dawn.actGRICDbgcolor", actGRIDCbgcolor);
				XRDB_LOAD_COLOR("dawn.actGRD1bgcolor", actGRD1bgcolor);
				XRDB_LOAD_COLOR("dawn.actGRD2bgcolor", actGRD2bgcolor);
				XRDB_LOAD_COLOR("dawn.actGRDMbgcolor", actGRDMbgcolor);
				XRDB_LOAD_COLOR("dawn.actHGRDbgcolor", actHGRDbgcolor);
				XRDB_LOAD_COLOR("dawn.actDWDLbgcolor", actDWDLbgcolor);
				XRDB_LOAD_COLOR("dawn.actDWDCLbgcolor", actDWDLCbgcolor);
				XRDB_LOAD_COLOR("dawn.actSPRLbgcolor", actSPRLbgcolor);
				XRDB_LOAD_COLOR("dawn.actSPRCLbgcolor", actSPRLCbgcolor);
				XRDB_LOAD_COLOR("dawn.actTTMILbgcolor", actTTMIbgcolor);
				XRDB_LOAD_COLOR("dawn.actTTMICLbgcolor", actTTMICbgcolor);
				XRDB_LOAD_COLOR("dawn.actfloatbgcolor", actfloatbgcolor);
				XRDB_LOAD_COLOR("dawn.selTTBbgcolor", selTTBbgcolor);
				XRDB_LOAD_COLOR("dawn.selLTRbgcolor", selLTRbgcolor);
				XRDB_LOAD_COLOR("dawn.selMONObgcolor", selMONObgcolor);
				XRDB_LOAD_COLOR("dawn.selGRIDbgcolor", selGRIDbgcolor);
				XRDB_LOAD_COLOR("dawn.selGRICDbgcolor", selGRIDCbgcolor);
				XRDB_LOAD_COLOR("dawn.selGRD1bgcolor", selGRD1bgcolor);
				XRDB_LOAD_COLOR("dawn.selGRD2bgcolor", selGRD2bgcolor);
				XRDB_LOAD_COLOR("dawn.selGRDMbgcolor", selGRDMbgcolor);
				XRDB_LOAD_COLOR("dawn.selHGRDbgcolor", selHGRDbgcolor);
				XRDB_LOAD_COLOR("dawn.selDWDLbgcolor", selDWDLbgcolor);
				XRDB_LOAD_COLOR("dawn.selDWDCLbgcolor", selDWDLCbgcolor);
				XRDB_LOAD_COLOR("dawn.selSPRLbgcolor", selSPRLbgcolor);
				XRDB_LOAD_COLOR("dawn.selSPRCLbgcolor", selSPRLCbgcolor);
				XRDB_LOAD_COLOR("dawn.selTTMILbgcolor", selTTMIbgcolor);
				XRDB_LOAD_COLOR("dawn.selTTMICLbgcolor", selTTMICbgcolor);
				XRDB_LOAD_COLOR("dawn.selfloatbgcolor", selfloatbgcolor);
				XRDB_LOAD_COLOR("color0", termcol0);
				XRDB_LOAD_COLOR("color1", termcol1);
				XRDB_LOAD_COLOR("color2", termcol2);
				XRDB_LOAD_COLOR("color3", termcol3);
				XRDB_LOAD_COLOR("color4", termcol4);
				XRDB_LOAD_COLOR("color5", termcol5);
				XRDB_LOAD_COLOR("color6", termcol6);
				XRDB_LOAD_COLOR("color7", termcol7);
				XRDB_LOAD_COLOR("color8", termcol8);
				XRDB_LOAD_COLOR("color9", termcol9);
				XRDB_LOAD_COLOR("color10", termcol10);
				XRDB_LOAD_COLOR("color11", termcol11);
				XRDB_LOAD_COLOR("color12", termcol12);
				XRDB_LOAD_COLOR("color13", termcol13);
				XRDB_LOAD_COLOR("color14", termcol14);
				XRDB_LOAD_COLOR("color15", termcol15);
			}
		}
	}

	XCloseDisplay(display);
}

void
xrdb(const Arg *arg)
{
	loadxrdb();
	int i;
	for (i = 0; i < LENGTH(colors); i++)
		scheme[i] = drw_scm_create(drw, colors[i], alphas[i], ColCount);
	focus(NULL);
	arrange(NULL);
}
