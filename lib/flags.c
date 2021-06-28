void
addflag(Client *c, const unsigned long flag)
{
	c->prevflags = (c->prevflags & ~flag) | (c->flags & flag);
	c->flags |= flag;
}

void
setflag(Client *c, const unsigned long flag, const int value)
{
	value ? addflag(c, flag) : removeflag(c, flag);
}

void
removeflag(Client *c, const unsigned long flag)
{
	c->prevflags = (c->prevflags & ~flag) | (c->flags & flag);
	c->flags &= ~flag;
}

void
toggleflag(Client *c, const unsigned long flag)
{
	c->flags ^= flag;
}

void
toggleflagex(const Arg *arg)
{
	toggleflag(selmon->sel, getflagbyname(arg->v));
	arrange(selmon);
}

const unsigned long
getflagbyname(const char *name)
{
	if (strcmp(name, "AlwaysOnTop") == 0)
		return AlwaysOnTop;
	else if (strcmp(name, "Fixed") == 0)
		return Fixed;
	else if (strcmp(name, "Floating") == 0)
		return Floating;
	else if (strcmp(name, "Urgent") == 0)
		return Urgent;
	else if (strcmp(name, "NeverFocus") == 0)
		return NeverFocus;
	else if (strcmp(name, "FullScreen") == 0)
		return FullScreen;
	else if (strcmp(name, "FakeFullScreen") == 0)
		return FakeFullScreen;
	else if (strcmp(name, "RestoreFakeFullScreen") == 0)
		return RestoreFakeFullScreen;
	else if (strcmp(name, "Centered") == 0)
		return Centered;
	else if (strcmp(name, "Permanent") == 0)
		return Permanent;
	else if (strcmp(name, "Hidden") == 0)
		return Hidden;
	else if (strcmp(name, "Sticky") == 0)
		return Sticky;
	else if (strcmp(name, "Terminal") == 0)
		return Terminal;
	else if (strcmp(name, "NoSwallow") == 0)
		return NoSwallow;
	else if (strcmp(name, "Locked") == 0)
		return Locked;
	else if (strcmp(name, "Transient") == 0)
		return Transient;
	else if (strcmp(name, "OnlyModButtons") == 0)
		return OnlyModButtons;
	else if (strcmp(name, "FlagPlaceholder0x20000") == 0)
		return FlagPlaceholder0x20000;
	else if (strcmp(name, "AttachMaster") == 0)
		return AttachMaster;
	else if (strcmp(name, "AttachAbove") == 0)
		return AttachAbove;
	else if (strcmp(name, "AttachBelow") == 0)
		return AttachBelow;
	else if (strcmp(name, "AttachAside") == 0)
		return AttachAside;
	else if (strcmp(name, "AttachBottom") == 0)
		return AttachBottom;
	else if (strcmp(name, "SwitchTag") == 0)
		return SwitchTag;
	else if (strcmp(name, "EnableTag") == 0)
		return EnableTag;
	else if (strcmp(name, "RevertTag") == 0)
		return RevertTag;
	else if (strcmp(name, "IgnoreCfgReq") == 0)
		return IgnoreCfgReq;
	else if (strcmp(name, "IgnoreCfgReqPos") == 0)
		return IgnoreCfgReqPos;
	else if (strcmp(name, "IgnoreCfgReqSize") == 0)
		return IgnoreCfgReqSize;
	else if (strcmp(name, "IgnorePropTransientFor") == 0)
		return IgnorePropTransientFor;
	else if (strcmp(name, "IgnoreSizeHints") == 0)
		return IgnoreSizeHints;
	else if (strcmp(name, "IgnoreMinimumSizeHints") == 0)
		return IgnoreMinimumSizeHints;
	else if (strcmp(name, "IgnoreDecorationHints") == 0)
		return IgnoreDecorationHints;
	else if (strcmp(name, "NoBorder") == 0)
		return NoBorder;
	else if (strcmp(name, "FlagPlaceholder0x400000000") == 0)
		return FlagPlaceholder0x400000000;
	else if (strcmp(name, "FlagPlaceholder0x800000000") == 0)
		return FlagPlaceholder0x800000000;
	else if (strcmp(name, "RespectSizeHints") == 0)
		return RespectSizeHints;
	else if (strcmp(name, "RioDrawNoMatchPID") == 0)
		return RioDrawNoMatchPID;
	else if (strcmp(name, "FlagPlaceholder274877906944") == 0)
		return FlagPlaceholder274877906944;
	else if (strcmp(name, "FlagPlaceholder549755813888") == 0)
		return FlagPlaceholder549755813888;
	else if (strcmp(name, "FlagPlaceholder1099511627776") == 0)
		return FlagPlaceholder1099511627776;
	else if (strcmp(name, "FlagPlaceholder2199023255552") == 0)
		return FlagPlaceholder2199023255552;
	else if (strcmp(name, "Lower") == 0)
		return Lower;
	else if (strcmp(name, "Raise") == 0)
		return Raise;
	else if (strcmp(name, "FlagPlaceholder17592186044416") == 0)
		return FlagPlaceholder17592186044416;
	else if (strcmp(name, "FlagPlaceholder35184372088832") == 0)
		return FlagPlaceholder35184372088832;
	else if (strcmp(name, "FlagPlaceholder70368744177664") == 0)
		return FlagPlaceholder70368744177664;
	else if (strcmp(name, "FlagPlaceholder140737488355328") == 0)
		return FlagPlaceholder140737488355328;
	else if (strcmp(name, "FlagPlaceholder281474976710656") == 0)
		return FlagPlaceholder281474976710656;
	else if (strcmp(name, "FlagPlaceholder562949953421312") == 0)
		return FlagPlaceholder562949953421312;
	else if (strcmp(name, "FlagPlaceholder1125899906842624") == 0)
		return FlagPlaceholder1125899906842624;
	else if (strcmp(name, "FlagPlaceholder2251799813685248") == 0)
		return FlagPlaceholder2251799813685248;
	else if (strcmp(name, "FlagPlaceholder4503599627370496") == 0)
		return FlagPlaceholder4503599627370496;
	else if (strcmp(name, "FlagPlaceholder9007199254740992") == 0)
		return FlagPlaceholder9007199254740992;
	else if (strcmp(name, "FlagPlaceholder18014398509481984") == 0)
		return FlagPlaceholder18014398509481984;
	else if (strcmp(name, "FlagPlaceholder36028797018963968") == 0)
		return FlagPlaceholder36028797018963968;
	else if (strcmp(name, "FlagPlaceholder72057594037927936") == 0)
		return FlagPlaceholder72057594037927936;
	else if (strcmp(name, "Invisible") == 0)
		return Invisible;
	else if (strcmp(name, "MoveResize") == 0)
		return MoveResize;
	else if (strcmp(name, "MovePlace") == 0)
		return MovePlace;
	else if (strcmp(name, "NeedResize") == 0)
		return NeedResize;
	else if (strcmp(name, "Ruled") == 0)
		return Ruled;
	else if (strcmp(name, "Marked") == 0)
		return Marked;
	else if (strcmp(name, "Unmanaged") == 0)
		return Unmanaged;

	return 0;
}
