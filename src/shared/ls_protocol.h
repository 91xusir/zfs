#ifndef _LS_PROTOCOL_H_
#define _LS_PROTOCOL_H_

#define T_LOGIN_SERVER	0
#define T_GW_SERVER		1
#define T_REGION_SERVER	2

const short ls2c_base					= 1000;
const short c2ls_base					= 2000;

const short c2ls_create_license			= c2ls_base + 1;/* 请求创建认证文件 */
const short c2ls_forwatch_license		= c2ls_base + 2;/* 被监控端			*/
const short c2ls_iamwatch_license		= c2ls_base + 3;/* 我是监控端		*/
const short c2ls_lc_check_once_license	= c2ls_base + 4;/* 认证检查			*/
const short c2ls_shutdown_login_license	= c2ls_base + 5;/* 关闭login		*/
const short c2ls_refresh_data_license	= c2ls_base + 6;/* 刷新数据			*/

const short ls2c_create_license_rep			= ls2c_base + 1;/* 传输认证文件 */
const short ls2c_iamwatch_license_rep		= ls2c_base + 2;/* 传输认证文件 */
const short ls2c_iplist_license				= ls2c_base + 3;/* 被监控ip地址 */
const short ls2c_lc_check_once_license_rep	= ls2c_base + 4;/* 认证检查结果 */
const short ls2c_shutdown_login_license		= ls2c_base + 5;/* 转发关闭login*/
const short ls2c_refresh_data_license		= ls2c_base + 6;/* 刷新数据		*/

#endif //_LS_PROTOCOL_H_

