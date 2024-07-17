/*	
 *		MAX_commands.h - defines all the invocable MAX UI commands - for the 'max' scripter construct
 *
 *			Copyright � John Wainwright 1996
 *
 */

#ifndef _H_MAX_COMMANDS
#define _H_MAX_COMMANDS

#include "maxcom.h"
#include "buildver.h"

 /* the commands... these are the command codes defined in "maxcom.h".  they MUST be in
 * alphabetical order */

	"accel pan",				MAXCOM_ACCEL_PAN,				0,
	"acthomegrid",				MAXCOM_ACTHOMEGRID,				MR_R2,
	"activate grid object",		MAXCOM_ACTGRIDOBJ ,				0,
	"activate home grid",		MAXCOM_ACTHOMEGRID,				0,
	"adaptive persp grid",		MAXCOM_ADAPTIVE_PERSP_GRID_TOGGLE,	0,
	"adaptive perspective grid toggle",	MAXCOM_ADAPTIVE_PERSP_GRID_TOGGLE,	MR_R2,
	"align",					MAXCOM_ALIGN,					0,
	"align camera",				MAXCOM_TOOLS_ALIGNCAMERA,		0,
	"align normals",			MAXCOM_ALIGNNORMALS,			0,
	"alignnormals",				MAXCOM_ALIGNNORMALS,			MR_R2,
	"angle snap toggle",		MAXCOM_ANGLE_SNAP_TOGGLE,		0,
	"apply ik",					MAXCOM_APPLY_IK,				0,
#ifndef NO_TOOL_ARRAY	// russom - 10/22/01
	"array",					MAXCOM_ARRAY,					0,
#endif
	"backface",					MAXCOM_BACKFACE,				MR_R2,
	"backface cull toggle",		MAXCOM_BACKFACE,				0,
	"background",				MAXCOM_BACKGROUND,				MR_R2,
	"background display toggle", MAXCOM_BACKGROUND,				0,
	"bind space warp mode",		MAXCOM_BINDWSM,					0,
	"bindwsm",					MAXCOM_BINDWSM,					MR_R2,
	"box mode",					MAXCOM_BOX_MODE,				MR_R2,
	"box mode selected",		MAXCOM_BOX_MODE,				0,
	"box mode toggle",			MAXCOM_BOX_TOGGLE,				0,
	"box toggle",				MAXCOM_BOX_TOGGLE,				MR_R2,
	"configure paths",			MAXCOM_CONFIGURE_PATHS,			0,
#ifndef NO_CREATE_TASK	// russom - 12/06/01
	"create mode",				MAXCOM_CREATE_MODE,				0,
#endif
#ifndef NO_CUI	// russom - 02/12/02
    "customize TP",				MAXCOM_CUSTOMIZE_CUSTOMIZETP,	0,
	"customize UI",				MAXCOM_CUSTOMIZE_CUSTOMIZEUI,	0,
#endif
	"cycle select",				MAXCOM_CYCLE_SELECT_METHOD ,	0,
	"cycle sublevel",			MAXCOM_CYCLE_SUBLEVEL,			MR_R2,
	"cycle subobject level",	MAXCOM_CYCLE_SUBLEVEL,			0,
	"def lgt toggle",			MAXCOM_DEF_LGT_TOGGLE,			MR_R2,
	"default lighting toggle",	MAXCOM_DEF_LGT_TOGGLE,			0,
	"delete",					MAXCOM_EDIT_DELETE ,			0,
#ifndef NO_TOOL_DISPLAY_FLOATER	// russom - 10/22/01
	"display floater",			MAXCOM_TOOLS_DISPLAYFLOATER,	0,
#endif
#ifndef NO_DISPLAY_TASK	// russom - 12/06/01
	"display mode",				MAXCOM_DISPLAY_MODE,			0,
#endif
	"dolly",					MAXCOM_DOLLY,					MR_R2,
	"dolly mode",				MAXCOM_DOLLY,					0,
	"drawing aids",				MAXCOM_DRAWINGAIDS,				0,
	"drawingaids",				MAXCOM_DRAWINGAIDS,				MR_R2,
	"fetch",					MAXCOM_FETCH,					0,
	"file archive",				MAXCOM_FILE_ARCHIVE,			0,
	"file export",				MAXCOM_FILE_EXPORT,				0,
	"file export selected",		MAXCOM_FILE_EXPORTSELECTED,		0,
	"file import",				MAXCOM_FILE_IMPORT,				0,
	"file insert tracks",		MAXCOM_FILE_INSERTTRACKS,		0,
	"file load animation",		MAXCOM_FILE_LOADXMLANIMATION,	0,
	"file merge",				MAXCOM_FILE_MERGE,				0,
	"file new",					MAXCOM_FILE_NEW,				0,
	"file open",				MAXCOM_FILE_OPEN,				0,
	"file preferences",			MAXCOM_FILE_PREFERENCES,		0,
	"file replace",				MAXCOM_FILE_REPLACE,			0,
	"file save",				MAXCOM_FILE_SAVE,				0,
	"file save animation",		MAXCOM_FILE_SAVEXMLANIMATION,	0,
	"file save selected",		MAXCOM_FILE_SAVESELECTED,		0,
	"file saveas",				MAXCOM_FILE_SAVEAS,				0,
	"file savecopyas",				MAXCOM_FILE_SAVECOPYAS,				0,
	"file summary",				MAXCOM_FILE_SUMMARYINFO,		0,
	"file xref object",			MAXCOM_FILE_XREFOBJECT,			0,
	"file xref scene",			MAXCOM_FILE_XREFSCENE,			0,
	"fov",						MAXCOM_FOV,	0,
	"freeze inv",				MAXCOM_FREEZE_INV,	0,
	"freeze selection",			MAXCOM_FREEZE_SELECTION,	0,
	"fullinteract",				MAXCOM_FULLINTERACT,	0,
	"grid nudge down",			MAXCOM_GRID_NUDGE_DOWN,	0,
	"grid nudge up",			MAXCOM_GRID_NUDGE_UP,	0,
	"grid toggle",				MAXCOM_GRID_TOGGLE,	0,
	"grids align",				MAXCOM_GRIDS_ALIGN,	0,
	"group attach",				MAXCOM_GROUP_ATTACH,	0,
	"group close",				MAXCOM_GROUP_CLOSE,	0,
	"group detach",				MAXCOM_GROUP_DETACH,	0,
	"group explode",			MAXCOM_GROUP_EXPLODE,	0,
	"group group",				MAXCOM_GROUP_GROUP,	0,
	"group open",				MAXCOM_GROUP_OPEN,	0,
	"group ungroup",			MAXCOM_GROUP_UNGROUP,	0,
	"help about",				MAXCOM_HELP_ABOUT,	0,
	"hide camera toggle",		MAXCOM_HIDE_CAMERA_TOGGLE,	0,
	"hide command panel toggle", MAXCOM_HIDE_CMD_PAN,	0,
	"hide floating toolbars toggle", MAXCOM_HIDE_FLOATERS,	0,
	"hide helper toggle",		MAXCOM_HIDE_HELPER_TOGGLE,	0,
	"hide inv",					MAXCOM_HIDE_INV,	0,
	"hide light toggle",		MAXCOM_HIDE_LIGHT_TOGGLE,	0,
#ifndef NO_STANDARD_TOOLBAR_OPTIONS	// russom - 02/12/02
	"hide main toolbar toggle",	MAXCOM_HIDE_MAIN_TB,	0,
#endif
	"hide object toggle",		MAXCOM_HIDE_OBJECT_TOGGLE,	0,
	"hide selection",			MAXCOM_HIDE_SELECTION,	0,
	"hide shape toggle",		MAXCOM_HIDE_SHAPE_TOGGLE,	0,
	"hide system toggle",		MAXCOM_HIDE_SYSTEM_TOGGLE,	0,
#if !defined(NO_STANDARD_TOOLBAR_OPTIONS) && !defined(NO_CUI_TABBED_PANELS)	// russom - 02/12/02
	"hide tab panel toggle",	MAXCOM_HIDE_SHELF,	0,
#endif
	"hide wsm toggle",			MAXCOM_HIDE_WSM_TOGGLE,	0,
#ifndef NO_HIERARCHY_TASK  // JP Morel - July 17th 2002
	"hierarchy mode",			MAXCOM_HIERARCHY_MODE,	0,
#endif
	"hold",						MAXCOM_HOLD,	0,
	"ik terminator",			MAXCOM_IK_TERMINATOR,	0,
	"ipan",						MAXCOM_IPAN,	0,
	"izoom in",					MAXCOM_IZOOM_IN,	0,
	"izoom out",				MAXCOM_IZOOM_OUT,	0,
	"key mode",					MAXCOM_KEY_MODE,	0,
	"link",						MAXCOM_LINK,	0,
#ifndef NO_CUI	// russom - 02/12/02
	"load custom UI",			MAXCOM_CUSTOMIZE_LOADCUI,	0,
#endif
	"lock UI layout",			MAXCOM_CUSTOMIZE_LOCKUILAYOUT,	0,
	"material browser",			MAXCOM_TOOLS_MTLMAPBROWSER,	0,
	"mirror",					MAXCOM_MIRROR,	0,
	"modify mode",				MAXCOM_MODIFY_MODE,	0,
#ifndef NO_MOTION_TASK  // JP Morel - July 12th 2002
	"motion mode",				MAXCOM_MOTION_MODE,	0,
#endif
	"move",						MAXCOM_MOVE,	0,
	"mtledit",		 			MAXCOM_MTLEDIT,	0,
	"next mod",					MAXCOM_NEXT_MOD,	0,
	"override",					MAXCOM_OVERRIDE,	0,
	"pancamera",				MAXCOM_PANCAMERA,	0,
	"panview",					MAXCOM_PANVIEW,	0,
	"percent snap toggle",		MAXCOM_PERCENT_SNAP_TOGGLE,	0,
	"persp",					MAXCOM_PERSP,	0,
	"place highlight",			MAXCOM_EDIT_PLACEHIGHLIGHT,	0,
	"prev mod",					MAXCOM_PREV_MOD,	0,
	"preview",					MAXCOM_PREVIEW,	0,
	"properties",				MAXCOM_PROPERTIES,	0,
	"quick render",				MAXCOM_QUICK_RENDER,	0,
	"redo",						MAXCOM_EDIT_REDO ,	0,
	"renamepreview",			MAXCOM_RENAMEPREVIEW,	0,
	"render last",				MAXCOM_RENDER_LAST,	0,
	"render scene",				MAXCOM_RENDER_SCENE,	0,
	"reset file",				MAXCOM_RESET_FILE,	0,
	"revert custom UI",			MAXCOM_CUSTOMIZE_REVERTCUI,	0,
	"rns",						MAXCOM_RNS,	0,
	"roll",						MAXCOM_ROLL,	0,
	"rotate",					MAXCOM_ROTATE,	0,
	"rotateview",				MAXCOM_ROTATEVIEW,	0,
#ifndef NO_SAFE_FRAMES // orb -7-20-2001
	"safeframe toggle",			MAXCOM_SAFEFRAME_TOGGLE,	0,
#endif // NO_SAFE_FRAMES
#ifndef NO_CUI	// russom - 02/12/02
	"save custom UI as",		MAXCOM_CUSTOMIZE_SAVECUIAS,	0,
#endif
	"saveplus",					MAXCOM_SAVEPLUS,	0,
	"scale",					MAXCOM_SCALE,	0,
	"scale cycle",				MAXCOM_SCALE_CYCLE,	0,
	"select",					MAXCOM_SELECT,	0,
	"select all",				MAXCOM_EDIT_SELECTALL ,	0,
	"select by color",			MAXCOM_SELECT_BY_COLOR,	0,
	"select child",				MAXCOM_SELECT_CHILD,	0,
	"select invert",			MAXCOM_EDIT_SELECTINVERT ,	0,
	"select none",				MAXCOM_EDIT_SELECTNONE ,	0,
	"select parent",			MAXCOM_SELECT_PARENT,	0,
	"selection floater",		MAXCOM_TOOLS_SELECTIONFLOATER,	0,

	"set key all",									MAXCOM_TOOL_SETKEY_HOTKEYALL,	0,
	"set key all filter",							MAXCOM_TOOL_SETKEY_ALL,	0,
	"set key all position",							MAXCOM_TOOL_SETKEY_HOTKEYPOS,	0,
	"set key all position rotation and scale",		MAXCOM_TOOL_SETKEY_HOTKEYPRS,	0,
	"set key all postion",							MAXCOM_TOOL_SETKEY_HOTKEYPOS,	0,
	"set key all postion rotation and scale",		MAXCOM_TOOL_SETKEY_HOTKEYPRS,	0,
	"set key all rotation",							MAXCOM_TOOL_SETKEY_HOTKEYROT,	0,
	"set key all scale",							MAXCOM_TOOL_SETKEY_HOTKEYSCALE,	0,
	"set key all x position",						MAXCOM_TOOL_SETKEY_HOTKEYPOSX,	0,
	"set key all x postion",						MAXCOM_TOOL_SETKEY_HOTKEYPOSX,	0,
	"set key all x rotation",						MAXCOM_TOOL_SETKEY_HOTKEYROTX,	0,
	"set key all x scale",							MAXCOM_TOOL_SETKEY_HOTKEYSCALEX,	0,
	"set key all y position",						MAXCOM_TOOL_SETKEY_HOTKEYPOSY,	0,
	"set key all y postion",						MAXCOM_TOOL_SETKEY_HOTKEYPOSY,	0,
	"set key all y rotation",						MAXCOM_TOOL_SETKEY_HOTKEYROTY,	0,
	"set key all y scale",							MAXCOM_TOOL_SETKEY_HOTKEYSCALEY,	0,
	"set key all z position",						MAXCOM_TOOL_SETKEY_HOTKEYPOSZ,	0,
	"set key all z postion",						MAXCOM_TOOL_SETKEY_HOTKEYPOSZ,	0,
	"set key all z rotation",						MAXCOM_TOOL_SETKEY_HOTKEYROTZ,	0,
	"set key all z scale",							MAXCOM_TOOL_SETKEY_HOTKEYSCALEZ,	0,
	"set key clear",								MAXCOM_TOOL_SETKEY_REVERT,	0,
	"set key customattributes filter",				MAXCOM_TOOL_SETKEY_CUSTOMATTRIBUTES,	0,
	"set key ikparams filter",						MAXCOM_TOOL_SETKEY_IK_PARAMS,	0,
	"set key keys",									MAXCOM_TOOL_SETKEY,	0,
	"set key materials filter",						MAXCOM_TOOL_SETKEY_MATERIALS,	0,
	"set key mode",									MAXCOM_TOOL_SETKEYMODE,	0,
	"set key modifiers filter",						MAXCOM_TOOL_SETKEY_MODIFIERS,	0,
	"set key objectparams filter",					MAXCOM_TOOL_SETKEY_OBJECTPARAMS,	0,
	"set key on selected",							MAXCOM_TOOL_SETKEY_SELECTED,	0,
	"set key other filter",							MAXCOM_TOOL_SETKEY_OTHER,	0,	
	"set key position filter",						MAXCOM_TOOL_SETKEY_POSITION,	0,
	"set key rotation filter",						MAXCOM_TOOL_SETKEY_ROTATION,	0,
	"set key scale filter",							MAXCOM_TOOL_SETKEY_SCALE,	0,	

	"shade selected",			MAXCOM_SHADE_SELECTED,	0,
	"show last img",			MAXCOM_SHOW_LAST_IMG,	0,
	"showaxisicon",				MAXCOM_SHOWAXISICON,	0,
	"showhomegrid",				MAXCOM_SHOWHOMEGRID,	0,
	"snap to frozen object",	MAXCOM_SNAP_SNAP_TO_FROZEN_OBJ,	0,
	"snap toggle",				MAXCOM_SNAP_TOGGLE,	0,
	"snap use axis constraints",MAXCOM_SNAP_USE_AXIS_CONSTRAINT,	0,
	"snapshot",					MAXCOM_EDIT_SNAPSHOT,	0,
	"spacebar",					MAXCOM_SPACEBAR,	0,
	"spacing tool",				MAXCOM_TOOLS_SPACINGTOOL,	0,
	"spinsnap toggle",			MAXCOM_SPINSNAP_TOGGLE,	0,
	"subobject sel",			MAXCOM_SUBOBJECT_SEL,	0,
	"swap layouts",				MAXCOM_SWAP_LAYOUTS,	0,
	"texture correct",			MAXCOM_TEXTURE_CORRECT,	0,
	"time back",				MAXCOM_TIME_BACK,	MR_DISABLED,
	"time config",				MAXCOM_TIME_CONFIG,	0,
	"time end",					MAXCOM_TIME_END,	0,
	"time forward",				MAXCOM_TIME_FORWARD,	MR_DISABLED,
	"time play",				MAXCOM_TIME_PLAY,	0,
	"time start",				MAXCOM_TIME_START,	0,
	"toggle ik",				MAXCOM_TOGGLE_IK,	0,
	"toggle key mode",			MAXCOM_KEYMODE_TOGGLE,	0,
	"toggle keyboard shortcuts",MAXCOM_KBDSHORTCUT_TOGGLE,	0,
	"toggle sound",				MAXCOM_TOGGLE_SOUND,	0,
	"tool animmode",			MAXCOM_TOOL_ANIMMODE,	0,
	"tool center",				MAXCOM_TOOL_CENTER,	0,
	"tool dualplanes",			MAXCOM_TOOL_DUALPLANES,	0,
	"tool hlist",				MAXCOM_TOOL_HLIST,	0,
	"tool maximize",			MAXCOM_TOOL_MAXIMIZE,	0,
	"tool region toggle",		MAXCOM_TOOL_REGION_TOGGLE,	0,
	"tool x",					MAXCOM_TOOL_X,	0,
	"tool xy",					MAXCOM_TOOL_XY,	0,
	"tool y",					MAXCOM_TOOL_Y,	0,
	"tool z",					MAXCOM_TOOL_Z,	0,
	"tool zoom",				MAXCOM_TOOL_ZOOM,	0,
	"tool zoomall",				MAXCOM_TOOL_ZOOMALL,	0,
	"tool zoomextents",			MAXCOM_TOOL_ZOOMEXTENTS,	0,
	"tool zoomextents all",		MAXCOM_TOOL_ZOOMEXTENTS_ALL,	0,
	"tool zoomregion",			MAXCOM_TOOL_ZOOMREGION,	0,
	"trajectories",				MAXCOM_TRAJECTORIES,	0,
	"treeview",					MAXCOM_TREEVIEW,	0,
	"truck",					MAXCOM_TRUCK,	0,
	"tti",						MAXCOM_TTI,	0,
	"undo",						MAXCOM_EDIT_UNDO ,	0,
	"unfreeze all",				MAXCOM_UNFREEZE_ALL,	0,
	"unfreeze by hit",			MAXCOM_UNFREEZE_BY_HIT,	0,
	"unfreeze by name",			MAXCOM_UNFREEZE_BY_NAME,	0,
	"unhide all",				MAXCOM_UNHIDE_ALL,	0,
	"unhide by name",			MAXCOM_UNHIDE_BY_NAME,	0,
	"unitsetup",				MAXCOM_UNITSETUP,	0,
	"unlink",					MAXCOM_UNLINK,	0,
#ifndef NO_UTILITY_TASK	// russom - 12/06/01
	"utility mode",				MAXCOM_UTILITY_MODE,	0,
#endif
	"videopost",				MAXCOM_VIDEOPOST,	0,
	"view file",				MAXCOM_VIEW_FILE,	0,
	"view redo",				MAXCOM_VIEW_REDO,	0,
	"viewpreview",				MAXCOM_VIEWPREVIEW,	0,
	"views redraw",				MAXCOM_VIEWS_REDRAW,	0,
	"views undo",				MAXCOM_VIEWS_UNDO,	0,
	"vpt back",					MAXCOM_VPT_BACK,	0,
	"vpt bottom",				MAXCOM_VPT_BOTTOM,	0,
	"vpt camera",				MAXCOM_VPT_CAMERA,	0,
	"vpt disable",				MAXCOM_VPT_DISABLE,	0,
	"vpt front",				MAXCOM_VPT_FRONT,	0,
	"vpt grid",					MAXCOM_VPT_GRID,	0,
	"vpt iso user",				MAXCOM_VPT_ISO_USER,	0,
	"vpt left",					MAXCOM_VPT_LEFT,	0,
	"vpt persp user",			MAXCOM_VPT_PERSP_USER,	0,
	"vpt right",				MAXCOM_VPT_RIGHT,	0,
	"vpt shape",				MAXCOM_VPT_SHAPE,	0,
	"vpt spot",					MAXCOM_VPT_SPOT,	0,
	"vpt tab",					MAXCOM_VPT_TAB,	0,
	"vpt top",					MAXCOM_VPT_TOP,	0,
	"vpt track",				MAXCOM_VPT_TRACK,	0,
	"vptconfig",				MAXCOM_VPTCONFIG,	0,
	"tool walkthrough",			MAXCOM_TOOL_WALKTHROUGH, 0,
	"wire facet",				MAXCOM_WIRE_FACET,	0,
	"wire smooth",				MAXCOM_WIRE_SMOOTH,	0,
	"zoom in 2x",				MAXCOM_ZOOM_IN_2X,	0,
	"zoom in 2x all",			MAXCOM_ZOOM_IN_2X_ALL,	0,
	"zoom out 2x",				MAXCOM_ZOOM_OUT_2X,	0,
	"zoom out 2x all",			MAXCOM_ZOOM_OUT_2X_ALL,	0,
	"zoomext sel",				MAXCOM_ZOOMEXT_SEL,	0,
	"zoomext sel all",			MAXCOM_ZOOMEXT_SEL_ALL,	0,

#endif
