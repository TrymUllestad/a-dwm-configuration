/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>
#include "movestack.c"

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int gappx     = 4;        /* gaps between windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;        /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "Inconsolata-g:size=12" };
static const char dmenufont[]       =   "Inconsolata-g:size=12";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char *colors[][3]      = {
    /*               fg         bg         border   */
    [SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
    [SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
    /* xprop(1):
     *    WM_CLASS(STRING) = instance, class
     *    WM_NAME(STRING) = title
     */
    /* class                            instance    title           tags mask   isfloating monitor */
    { "Gimp",                           NULL,       NULL,           0,          1,         -1 },
    { "Firefox",                        NULL,       NULL,           1,          0,         -1 },
    { "discord",                        NULL,       NULL,           2,          0,         -1 },
    { "STM32CubeIDE",                   NULL,       NULL,           1 << 7,     0,         -1 },
    { "MATLAB R2019a - academic use",   NULL,       NULL,           1 << 8,     0,         -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

#include "fibonacci.c"
static const Layout layouts[] = {
    /* symbol   arrange function */
    { "[]=",    tile    }, /* first entry is default */
    { "><>",    NULL    }, /* no layout function means floating behavior */
    { "[M]",    monocle }, /* master gets fullscreened */
    { "[]]",    deck    }, /* master + monocle on client stack */
    { "[\\]",   dwindle }, /* fibonacci spiral dwindles to bottom left */
    { "[@]",    spiral  }, /* stack spiral inwards like a fibonacci spiral */
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
static const char *dmenucmd[]         = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]          = { "st", NULL };
static const char scratchpadname[]    = "floating terminal";
static const char *scratchpadcmd[]    = { "st", "-t", scratchpadname, "-g", "80x22", NULL };
static const char *sleepcmd[]         = { "systemctl", "suspend", NULL };
static const char *refreshcmd[]       = { "refreshstatus", NULL };
static const char *calendarcmd[]      = { "st", "calcurse", NULL };
static const char *bitwardencmd[]     = { "bitwarden", NULL };
static const char *vifmcmd[]          = { "st", "vifmrun", NULL };
static const char *volcmd[][4]        = { 
    { "pamixer", "-i", "5", NULL },
    { "pamixer", "-d", "5", NULL },
    { "pamixer", "-t", NULL,NULL },
};
static const char *brightcmd[][5]     = {
    { "sudo", "light", "-A", "5", NULL },
    { "sudo", "light", "-U", "5", NULL },
};

static Key keys[] = {
    /* modifier                     key                 function        argument */
    { MODKEY,                       XK_r,               spawn,          {.v = dmenucmd }        },    // r for run
    { MODKEY,                       XK_Return,          spawn,          {.v = termcmd }         },    // spawn terminal
    { MODKEY,                       XK_apostrophe,      togglescratch,  {.v = scratchpadcmd }   },    // toggles:    floating terminal
    { MODKEY|ShiftMask,             XK_space,           togglefloating, {0}                     },    //            floating behaviour
    { MODKEY|ShiftMask,             XK_Return,          setlayout,      {0}                     },    //            previous layout
    { MODKEY|ShiftMask,             XK_b,               togglebar,      {0}                     },    //            statusbar
    { MODKEY,                       XK_m,               focusmaster,    {0}                     },    // master commands:    focus
    { MODKEY,                       XK_space,           zoom,           {0}                     },    //                    promote
    { MODKEY,                       XK_i,               incnmaster,     {.i = +1 }              },    //                    add clients
    { MODKEY|ShiftMask,             XK_i,               incnmaster,     {.i = -1 }              },    //                    decrease clients
    { MODKEY,                       XK_q,               killclient,     {0}                     },    // kill:    client
    { MODKEY|ShiftMask,             XK_q,               quit,           {0}                     },    //            Xorg
    { MODKEY,                       XK_t,               setlayout,      {.v = &layouts[0]}      },    // sets layout to:    tiled
    { MODKEY,                       XK_n,               setlayout,      {.v = &layouts[1]}      },    //            null/floating
    { MODKEY|ShiftMask,             XK_m,               setlayout,      {.v = &layouts[2]}      },    //            monocle
    { MODKEY,                       XK_d,               setlayout,      {.v = &layouts[3]}      },    //            deck
    { MODKEY,                       XK_f,               setlayout,      {.v = &layouts[4]}      },    //            dwindle
    { MODKEY|ShiftMask,             XK_f,               setlayout,      {.v = &layouts[5]}      },    //            fibonacci
    { MODKEY,                       XK_plus,            setgaps,        {.i = +5 }              },    // gaps:    increase
    { MODKEY,                       XK_minus,           setgaps,        {.i = -5 }              },    //        decrease
    { MODKEY|ShiftMask,             XK_plus,            setgaps,        {.i =  0 }              },    //        reset
    /*      Dvorak specific bindings                    */
    { MODKEY,                       XK_comma,           focusstack,     {.i = -1 }              },    // change focus:    up
    { MODKEY,                       XK_o,               focusstack,     {.i = +1 }              },    //            down
    { MODKEY,                       XK_aring,           focusmon,       {.i = -1 }              },    //            next monitor
    { MODKEY,                       XK_period,          focusmon,       {.i = +1 }              },    //            prev monitor
    { MODKEY|ShiftMask,             XK_comma,           movestack,      {.i = -1 }              },    // move client:        up
    { MODKEY|ShiftMask,             XK_o,               movestack,      {.i = +1 }              },    //            down
    { MODKEY|ShiftMask,             XK_aring,           tagmon,         {.i = -1 }              },    //            next monitor
    { MODKEY|ShiftMask,             XK_period,          tagmon,         {.i = +1 }              },    //            prev monitor
    { MODKEY,                       XK_a,               setmfact,       {.f = -0.05}            },    // master area:    increase
    { MODKEY,                       XK_e,               setmfact,       {.f = +0.05}            },    //        decrease
    /*            Qwerty equivalent                        
    { MODKEY,                       XK_w,               focusstack,     {.i = -1 }              },
    { MODKEY,                       XK_s,               focusstack,     {.i = +1 }              },
    { MODKEY,                       XK_q,               focusmon,       {.i = -1 }              },     // NB! killclient also assigned to mod+q
    { MODKEY,                       XK_e,               focusmon,       {.i = +1 }              },
    { MODKEY|ShiftMask,             XK_w,               movestack,      {.i = -1 }              },
    { MODKEY|ShiftMask,             XK_s,               movestack,      {.i = +1 }              },
    { MODKEY|ShiftMask,             XK_q,               tagmon,         {.i = -1 }              },    // NB! killclient also assigned to mod+q
    { MODKEY|ShiftMask,             XK_e,               tagmon,         {.i = +1 }              },
    { MODKEY,                       XK_a,               setmfact,       {.f = -0.05}            },
    { MODKEY,                       XK_d,               setmfact,       {.f = +0.05}            },    */
    { MODKEY,                       XK_Up,              moveresize,     {.v = (int []){   0, -20,   0,   0 }}   },    // move focused floating window
    { MODKEY,                       XK_Down,            moveresize,     {.v = (int []){   0,  20,   0,   0 }}   },    
    { MODKEY,                       XK_Right,           moveresize,     {.v = (int []){  20,   0,   0,   0 }}   },
    { MODKEY,                       XK_Left,            moveresize,     {.v = (int []){ -20,   0,   0,   0 }}   },
    { MODKEY|ShiftMask,             XK_Up,              moveresize,     {.v = (int []){   0,   0,   0, -20 }}   },    // resize focused floating window
    { MODKEY|ShiftMask,             XK_Down,            moveresize,     {.v = (int []){   0,   0,   0,  20 }}   },    // will be resized from top left corner
    { MODKEY|ShiftMask,             XK_Right,           moveresize,     {.v = (int []){   0,   0,  20,   0 }}   },
    { MODKEY|ShiftMask,             XK_Left,            moveresize,     {.v = (int []){   0,   0, -20,   0 }}   },
    { MODKEY,                       XK_KP_End,          movesnap,       {.ui = WIN_SW }         },    // make client floating snap it in 3x3 grid
    { MODKEY,                       XK_KP_Down,         movesnap,       {.ui = WIN_S  }         },    // snapment depends on which key in the keypad is pressed    
    { MODKEY,                       XK_KP_Next,         movesnap,       {.ui = WIN_SE }         },
    { MODKEY,                       XK_KP_Left,         movesnap,       {.ui = WIN_W  }         },
    { MODKEY,                       XK_KP_Begin,        movesnap,       {.ui = WIN_C  }         },
    { MODKEY,                       XK_KP_Right,        movesnap,       {.ui = WIN_E  }         },
    { MODKEY,                       XK_KP_Home,         movesnap,       {.ui = WIN_NW }         },
    { MODKEY,                       XK_KP_Up,           movesnap,       {.ui = WIN_N  }         },
    { MODKEY,                       XK_KP_Prior,        movesnap,       {.ui = WIN_NE }         },
    { MODKEY|ShiftMask,             XK_KP_End,          moveplace,      {.ui = WIN_SW }         },    // make client floating snap and resize it in 3x3 grid
    { MODKEY|ShiftMask,             XK_KP_Down,         moveplace,      {.ui = WIN_S  }         },    // placement depends on which key in the keypad is pressed    
    { MODKEY|ShiftMask,             XK_KP_Next,         moveplace,      {.ui = WIN_SE }         },
    { MODKEY|ShiftMask,             XK_KP_Left,         moveplace,      {.ui = WIN_W  }         },
    { MODKEY|ShiftMask,             XK_KP_Begin,        moveplace,      {.ui = WIN_C  }         },
    { MODKEY|ShiftMask,             XK_KP_Right,        moveplace,      {.ui = WIN_E  }         },
    { MODKEY|ShiftMask,             XK_KP_Home,         moveplace,      {.ui = WIN_NW }         },
    { MODKEY|ShiftMask,             XK_KP_Up,           moveplace,      {.ui = WIN_N  }         },
    { MODKEY|ShiftMask,             XK_KP_Prior,        moveplace,      {.ui = WIN_NE }         },
    { MODKEY,                       XK_j,               moveplace,      {.ui = WIN_C  }         },
    { MODKEY,                       XK_v,               spawn,          {.v = vifmcmd }         },    // start:    vifm
    { MODKEY,                       XK_c,               spawn,          {.v = calendarcmd }     },    //        calcurse
    { MODKEY,                       XK_b,               spawn,          {.v = bitwardencmd }    },    //        bitwarden, handled by dmenu
    { MODKEY,                       XK_F1,              spawn,          {.v = sleepcmd }        },    // suspend to RAM
    { MODKEY,                       XK_F5,              spawn,          {.v = refreshcmd }      },    // refresh statusbar (custom script)
    { 0,          XF86XK_AudioRaiseVolume,              spawn,          {.v = &volcmd[0] }      },    // laptop controls:    volume up
    { 0,          XF86XK_AudioLowerVolume,              spawn,          {.v = &volcmd[1] }      },    //            volume down
    { 0,                 XF86XK_AudioMute,              spawn,          {.v = &volcmd[2] }      },    //            volume mute
    { 0,           XF86XK_MonBrightnessUp,              spawn,          {.v = brightcmd[0] }    },    //            brightness up
    { 0,         XF86XK_MonBrightnessDown,              spawn,          {.v = brightcmd[1] }    },    //            brightness down
    { MODKEY,                       XK_Tab,             view,           {0}                     },    // toggles previous tag mask
    { MODKEY,                       XK_0,               view,           {.ui = ~0 }             },    // view all tags
    { MODKEY|ShiftMask,             XK_0,               tag,            {.ui = ~0 }             },    // tag client to all tags
    TAGKEYS(                        XK_1,               0)
    TAGKEYS(                        XK_2,               1)
    TAGKEYS(                        XK_3,               2)
    TAGKEYS(                        XK_4,               3)
    TAGKEYS(                        XK_5,               4)
    TAGKEYS(                        XK_6,               5)
    TAGKEYS(                        XK_7,               6)
    TAGKEYS(                        XK_8,               7)
    TAGKEYS(                        XK_9,               8)
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
    { ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
    { ClkTagBar,            0,              Button1,        view,           {0} },
    { ClkTagBar,            0,              Button3,        toggleview,     {0} },
    { ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
    { ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

