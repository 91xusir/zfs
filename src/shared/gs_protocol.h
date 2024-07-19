#ifndef _INC_GS_PROTOCOL_H_
#define _INC_GS_PROTOCOL_H_

enum
{
	c2gs_base				= 100,
	c2gs_query_us_list		= 101,
	c2gs_query_gws_list		= 102,
    c2gs_query              = 103,

	gs2c_base				= 200,
	gs2c_us_list			= 201,
	gs2c_gws_list			= 202,
    gs2c_answer             = 203,
    gs2c_us_info			= 204,
    gs2c_gws_info			= 205,

	u2gs_base				= 300,
	u2gs_update_info		= 301,
    u2gs_check_version      = 302,

    gs2u_base               = 600,
    gs2u_check_version_ret  = 601,

    m2gs_base               = 400,
    m2gs_query_info         = 401,
    m2gs_reboot             = 402,
    m2gs_shutdown           = 403,
    m2gs_bulletin           = 404,
    m2gs_setstate           = 405,
    m2gs_tester             = 406,

    gs2m_base               = 500,
    gs2m_update_info        = 501,
    gs2m_tester_ret         = 502,
};

#endif // _INC_GS_PROTOCOL_H_
