//Find
ACMD (do_clear_affect);

///Add
#if defined(__BL_DISCORD_MESSAGE__)
ACMD(do_discord);
#endif

//Find
	{ "do_clear_affect", do_clear_affect, 	0, POS_DEAD,		GM_LOW_WIZARD},

///Add
#if defined(__BL_DISCORD_MESSAGE__)
	{ "discord",	do_discord,	0,	POS_DEAD,	GM_IMPLEMENTOR },
#endif