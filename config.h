/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int gappih    = 8;        /* horiz inner gap between windows */
static const unsigned int gappiv    = 8;        /* vert inner gap between windows */
static const unsigned int gappoh    = 8;        /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 8;        /* vert outer gap between windows and screen edge */
static const int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const int vertpad            = 8;        /* vertical padding of bar */
static const int sidepad            = 8;        /* horizontal padding of bar */
static const char *fonts[]          = { "mononoki:size=12", "fontawesome:size=12" };
static const char dmenufont[]       = "monospace:size=12";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#000000";
static const char col_cyan[]        = "#ff6600";    // blue - #3399ff
static const unsigned int baralpha = 0xd0;
static const unsigned int borderalpha = OPAQUE;
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
};
static const unsigned int alphas[][3]      = {
	/*               fg      bg        border     */
	[SchemeNorm] = { OPAQUE, baralpha, borderalpha },
	[SchemeSel]  = { OPAQUE, baralpha, borderalpha },
};

typedef struct {
	const char *name;
	const void *cmd;
} Sp;
const char *spcmd1[] = {"st", "-n", "spterm", "-g", "120x34", NULL };
const char *spcmd2[] = {"st", "-n", "spfm", "-g", "144x41", "-e", "ranger", NULL };
static Sp scratchpads[] = {
	/* name          cmd  */
	{"spterm",      spcmd1},
	{"spranger",    spcmd2},
};

/* tagging */
static const char *tags[] = { "", "", "", "", "", "", "", "", "" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class            instance  title           tags mask  isfloating  isterminal  noswallow  canfocus monitor */
	{ "Gimp",           NULL,     NULL,           0,         1,          0,           0,         1,        -1 },
	{ "Firefox",        NULL,     NULL,           1 << 8,    0,          0,          -1,         1,        -1 },
	{ "st-256color",    NULL,     NULL,           0,         0,          1,           0,         1,        -1 },
	{ NULL,	        "spterm",	  NULL,	          SPTAG(0),	 1,          1,           0, 		 1,        -1 },
	{ NULL,	          "spfm",	  NULL,	          SPTAG(1),	 1,          0,          -1,		 1,        -1 },
	{ NULL,             NULL,  "Event Tester",    0,         0,          0,           1,         1,        -1 }, /* xev */
};

/* layout(s) */
static const float mfact     = 0.5; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ " |  | ",      tile },    /* first entry is default */
	{ " |  | ",      monocle },
	{ " | ><> | ",      NULL },    /* no layout function means floating behavior */
	{ " | |M| | ",      centeredmaster },
	{ " | >M> |",      centeredfloatingmaster },
	{ " | === |",      bstackhoriz },
	{ " | TTT |",      bstack },
	{ NULL,       NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[]  = { "dmenu_run", "-h", "40", "-c", "-l", "10", NULL };
static const char *clipmenucmd[]  = { "/home/yash/.config/dwm/automate_scripts/clipmenu.sh", NULL };
static const char *termcmd[]   = { "terminator", NULL };
static const char *stcmd[]     = { "st", NULL };
static const char *firefox[]   = { "firefox", NULL };

static const char *brupcmd[]   = { "xbacklight", "-inc", "1", NULL };
static const char *brdowncmd[] = { "xbacklight", "-dec", "1", NULL };

static const char *volumeupcmd[] =   { "/home/yash/.config/dwm/volume_control/up.sh", NULL };
static const char *volumedowncmd[] = { "/home/yash/.config/dwm/volume_control/down.sh", NULL };
static const char *volumemutecmd[] = { "/home/yash/.config/dwm/volume_control/mute.sh", NULL };

static const char *cmustogglecmd[] =   { "/home/yash/.config/dwm/automate_scripts/cmus_start_stop.sh", NULL };
static const char *cmusnextcmd[] = { "/home/yash/.config/dwm/automate_scripts/cmus_next.sh", NULL };
static const char *cmuspreviouscmd[] = { "/home/yash/.config/dwm/automate_scripts/cmus_previous.sh", NULL };

static const char *poweroffcmd[]   = { "/home/yash/.config/dwm/automate_scripts/shutdown.sh", NULL };

static const char *screenshotcmd[] = { "/home/yash/.config/dwm/automate_scripts/screenshot.sh", NULL };
static const char *rectshotcmd[] = { "/home/yash/.config/dwm/automate_scripts/rectangular_screenshot.sh", NULL };

static const char *screenlockcmd[] = { "i3lock-fancy", NULL };


#include "movestack.c"
static Key keys[] = {
	/* modifier                     key        function        argument */
    
    /* My applications */
	{ MODKEY|Mod1Mask,              XK_x,      spawn,          {.v = dmenucmd } },      // Dmenu_run
	{ MODKEY|Mod1Mask,              XK_c,      spawn,          {.v = clipmenucmd } },   // Clipmenu
	{ MODKEY|Mod1Mask,              XK_b,      spawn,          {.v = firefox } },       // firefox
	{ MODKEY,                       XK_Return, spawn,          {.v = stcmd   } },       // st terminal
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },       // terminator

    /* Layouts */
	{ MODKEY|ControlMask,           XK_period, cyclelayout,    {.i = +1 } },            // Cycle through the available layouts moving forward
	{ MODKEY|ControlMask,		    XK_comma,  cyclelayout,    {.i = -1 } },            // Cycle through the available layouts moving backward
	{ MODKEY,                       XK_space,  setlayout,      {0} },                   // Toggle between the previous and current layout
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },    // Set tiling layout
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },    // Set fullscreen layout
//	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },    /*
//	{ MODKEY,                       XK_u,      setlayout,      {.v = &layouts[3]} },      \
//	{ MODKEY,                       XK_o,      setlayout,      {.v = &layouts[4]} },        Layouts that dont have any key binding currently
//	{ MODKEY,                       XK_q,      setlayout,      {.v = &layouts[5]} },      /
//	{ MODKEY,                       XK_w,      setlayout,      {.v = &layouts[6]} },    */
	{ MODKEY|ShiftMask,             XK_t,      togglefloating, {0} },                   // Force floating into tiling

    /* Window Navigation */
	{ MODKEY,                       XK_Tab,    view,           {0} },                   // Toggle between the previous and the current tag
	{ ControlMask|Mod1Mask,         XK_Right,  setmfact,       {.f = +0.01 } },         // Increment the master area (mod1mask is altkey)
	{ ControlMask|Mod1Mask,         XK_Left,   setmfact,       {.f = -0.01 } },         // Decrement the master area
	{ MODKEY,                       XK_Right,  focusstack,     {.i = +1 } },            // Move the focus to next window
	{ MODKEY,                       XK_Left,   focusstack,     {.i = -1 } },            // Move the focus to previous window
	{ MODKEY,                       XK_b,      togglebar,      {0} },                   // Toggle the status bar
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },            // Increment the number of windows in the master area by 1
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },            // Decrement the number of windows in the master area by 1
	{ Mod1Mask|ShiftMask,           XK_Return, zoom,           {0} },                   // I still dont know what that does

    /* Window Actions */
	{ MODKEY,                       XK_q,      killclient,     {0} },                   // Kill the focused client
	{ MODKEY|ShiftMask,             XK_Right,  movestack,      {.i = +1 } },            // Move the focused client to the right
	{ MODKEY|ShiftMask,             XK_Left,   movestack,      {.i = -1 } },            // Move the focused client to the left
	{ MODKEY|ShiftMask,             XK_Down,   movestack,      {.i = +1 } },            // Move the focused client to the right
	{ MODKEY|ShiftMask,             XK_Up,     movestack,      {.i = -1 } },            // Move the focused client to the left


    /* Manipulating Gaps */
	{ MODKEY|Mod1Mask,              XK_h,      incrgaps,       {.i = +1 } },            // Increase all gaps
	{ MODKEY|Mod1Mask,              XK_l,      incrgaps,       {.i = -1 } },            // Decrease all gaps
	{ MODKEY|Mod1Mask|ShiftMask,    XK_h,      incrogaps,      {.i = +1 } },            // Increase outer gaps
	{ MODKEY|Mod1Mask|ShiftMask,    XK_l,      incrogaps,      {.i = -1 } },            // Decrease outer gaps
	{ MODKEY|Mod1Mask|ControlMask,  XK_h,      incrigaps,      {.i = +1 } },            // Increase inner gaps
	{ MODKEY|Mod1Mask|ControlMask,  XK_l,      incrigaps,      {.i = -1 } },            // Decrease inner gaps
	{ MODKEY|Mod1Mask,              XK_0,      togglegaps,     {0} },                   // Toggle gaps ON/OFF
	{ MODKEY|Mod1Mask|ShiftMask,    XK_0,      defaultgaps,    {0} },                   // Reset gaps back to default
	{ MODKEY,                       XK_y,      incrihgaps,     {.i = +1 } },            // Increase inner horizontal gaps
	{ MODKEY,                       XK_o,      incrihgaps,     {.i = -1 } },            // Decrease inner horizontal gaps
	{ MODKEY|ControlMask,           XK_y,      incrivgaps,     {.i = +1 } },            // Increase inner vertical gaps
	{ MODKEY|ControlMask,           XK_o,      incrivgaps,     {.i = -1 } },            // Decrease inner vertical gaps
	{ MODKEY|Mod1Mask,              XK_y,      incrohgaps,     {.i = +1 } },            // Increase outer horizontal gaps
	{ MODKEY|Mod1Mask,              XK_o,      incrohgaps,     {.i = -1 } },            // Decrease outer horizontal gaps
	{ MODKEY|ShiftMask,             XK_y,      incrovgaps,     {.i = +1 } },            // Increase outer vertical gaps
	{ MODKEY|ShiftMask,             XK_o,      incrovgaps,     {.i = -1 } },            // Decrease outer vertical gaps

    /* Scratchpad */
	{ MODKEY|Mod1Mask,            	XK_s,  	   togglescratch,  {.ui = 0 } },            // Toggle terminal scratchpad
	{ MODKEY|Mod1Mask,            	XK_f,  	   togglescratch,  {.ui = 1 } },            // Toggle ranger scratchpad

    /* Tags */
	TAGKEYS(                        XK_1,                      0)                       //------
	TAGKEYS(                        XK_2,                      1)                       //     |
	TAGKEYS(                        XK_3,                      2)                       //     |
	TAGKEYS(                        XK_4,                      3)                       //     |
	TAGKEYS(                        XK_5,                      4)                       //      Move to the specific tag. Pressing mod+0 selects all tags
	TAGKEYS(                        XK_6,                      5)                       //     |   
	TAGKEYS(                        XK_7,                      6)                       //     |
	TAGKEYS(                        XK_8,                      7)                       //     |
	TAGKEYS(                        XK_9,                      8)                       //------    
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },           // Select all tags 
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },           // Broadcast the current window to all the tags. (Broadcasting not copying)

    /* Dwm Actions */
	{ MODKEY|ShiftMask,             XK_s,      spawn,          {.v = poweroffcmd } },   // Launch the custom poweroff script that I made using dmenu
//  { MODKEY|ShiftMask,             XK_s,      quit,           {0} },
	{ MODKEY|ShiftMask,             XK_l,      spawn,          {.v = screenlockcmd } }, // Launch the i3lock-fancy screen locker

    /* Multimedia Keys */
	{ 0, 			 XF86XK_MonBrightnessUp,   spawn,          {.v = brupcmd } },       // Increase brightness by 2%
	{ 0, 			 XF86XK_MonBrightnessDown, spawn,          {.v = brdowncmd } },     // Decrease brightness by 2%
	{ 0, 			 XF86XK_AudioLowerVolume,  spawn,          {.v = volumedowncmd } }, // Increase volume by 2%
	{ 0, 			 XF86XK_AudioRaiseVolume,  spawn,          {.v = volumeupcmd } },   // Decrease volume by 2%
	{ 0, 			 XF86XK_AudioMute,         spawn,          {.v = volumemutecmd } }, // Mute audio
	{ 0, 			 XF86XK_AudioPlay,         spawn,          {.v = cmustogglecmd } }, // Toggle cmus
	{ 0, 			 XF86XK_AudioNext,         spawn,          {.v = cmusnextcmd } },   // Previous track in cmus
	{ 0, 			 XF86XK_AudioPrev,         spawn,          {.v = cmuspreviouscmd }},// Next track in cmus 

    /* Screenshot */
	{ MODKEY|ShiftMask,             XK_p,      spawn,          {.v = screenshotcmd } }, // Take screenshot
	{ MODKEY|ShiftMask|ControlMask, XK_p,      spawn,            {.v = rectshotcmd } }, // Take rectangular selection screenshot

    /* Multi monitor */
//  { MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },            // \
//	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },            //  \
//	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },            //   These keybindings are usefull when on multimonitor 
//	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },            // /
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button1,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
