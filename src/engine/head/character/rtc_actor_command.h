
//////////////////////////////////////////////////////////////////////////
//
//   FileName : rtc_actor_command.h
//     Author : zxy
// CreateTime : 2010.04.20
//       Desc :
//
//////////////////////////////////////////////////////////////////////////

#ifndef _RtcAcotrCommand_H_
#define _RtcAcotrCommand_H_

typedef farray<DWORD> vcArray_t;
class CRT_ActorInstance;

typedef enum _Rac_CmdType
{
    Ractype_PlayPose = 0,
    Ractype_Setvc,
    Ractype_Acotrlink,
    Ractype_LoadSub,
    Ractype_Setscale,
    Ractype_Setbasecolor,
    Ractype_Setvisible,
    Ractype_Setmtl,
    Ractype_Needsyncmtl

} Rac_CmdType;

typedef struct _Rac_PlayPose
{
    enum { _cmdtype = Ractype_PlayPose };
    enum { _speed = 0, _time = 1 };
    Rac_CmdType ctype;
    DWORD  pmode;
    string pname;
    float  speed;
    bool   bloop;

} Rac_PlayPose;

typedef struct _Rac_Setvc
{
    enum { _cmdtype = Ractype_Setvc };
    Rac_CmdType ctype;
    vcArray_t* _vc;

} Rac_Setvc;

typedef struct _Rac_ActorLink
{
    enum { _cmdtype = Ractype_Acotrlink };
    enum { _link = 0, _unlink = 1   };
    Rac_CmdType ctype;
    DWORD ltype;
    CRT_ActorInstance* _parent;
    string _slot;
} Rac_ActorLink;

typedef struct _Rac_LoadSub
{
    enum { _cmdtype = Ractype_LoadSub };
    enum { _sub_skin = 0, _sub_mtl = 1, _sub_efft = 2 };
    enum { _load = 0, _unload = 1 };
    Rac_CmdType ctype;
    DWORD stype;
    DWORD otype;
    string sname;
    bool bdisableMip;
} Rac_LoadSub;

typedef struct _Rac_Setscale
{
    enum { _cmdtype = Ractype_Setscale };
    Rac_CmdType ctype;
    RtgVertex3  scale;
} Rac_Setscale;

typedef struct _Rac_Setbasecolor
{
    enum { _cmdtype = Ractype_Setbasecolor };
    Rac_CmdType ctype;
    RtgVertex3  color;
    bool        bchild;
} Rac_Setbasecolor;

typedef struct _Rac_Setvisible
{
    enum { _cmdtype = Ractype_Setvisible };
    Rac_CmdType ctype;
    float _visible;
} Rac_Setvisible;

typedef struct _Rac_Setmtl
{
    enum { _cmdtype = Ractype_Setmtl };
    Rac_CmdType ctype;
    size_t _skinIndex;
    string mtlname;
} Rac_Setmtl;

typedef struct _Rac_Needsyncmtl
{
    enum { _cmdtype = Ractype_Needsyncmtl };
    Rac_CmdType ctype;
} Rac_Needsyncmtl;

template<typename _ctype>
_ctype* CreateActorCommand()
{
    _ctype* _cmd = RT_NEW _ctype;
    _cmd->ctype = (Rac_CmdType)_ctype::_cmdtype;
    return _cmd;
}

inline Rac_CmdType GetActorCmdtype(void* _cmd)
{
    Rac_CmdType* _ctype = (Rac_CmdType*)_cmd;
    return *_ctype;
}

inline void DestroyActorCommand(void* _cmd)
{
    switch (GetActorCmdtype(_cmd))
    {
    case Ractype_PlayPose :
		{
			Rac_PlayPose* pp = (Rac_PlayPose*)_cmd;
			DEL_ONE(pp);
		}
        break;
    case Ractype_Setvc :
		{
			Rac_Setvc* pp = (Rac_Setvc*)_cmd;
			DEL_ONE(pp);
		}
        break;
    case Ractype_Acotrlink :
		{
			Rac_ActorLink* pp = (Rac_ActorLink*)_cmd;
			DEL_ONE(pp);
		}
        break;
    case Ractype_LoadSub :
		{
			Rac_LoadSub* pp = (Rac_LoadSub*)_cmd;
			DEL_ONE(pp);
		}
        break;
    case Ractype_Setscale :
		{
			Rac_Setscale* pp = (Rac_Setscale*)_cmd;
			DEL_ONE(pp);
		}
        break;
    case Ractype_Setbasecolor :
		{
			Rac_Setbasecolor* pp = (Rac_Setbasecolor*)_cmd;
			DEL_ONE(pp);
		}
        break;
    case Ractype_Setvisible :
		{
			Rac_Setvisible* pp = (Rac_Setvisible*)_cmd;
			DEL_ONE(pp);
		}
        break;
    case Ractype_Setmtl :
		{
			Rac_Setmtl* pp = (Rac_Setmtl*)_cmd;
			DEL_ONE(pp);
		}
        break;
    case Ractype_Needsyncmtl :
		{
			Rac_Needsyncmtl* pp = (Rac_Needsyncmtl*)_cmd;
			DEL_ONE(pp);
		}
        break;
    default :
        DEL_ONE(_cmd);
        break;
    }
}

#endif