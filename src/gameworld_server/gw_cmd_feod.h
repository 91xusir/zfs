#ifndef _GW_CMD_FEOD_H
#define _GW_CMD_FEOD_H

class CFeod;

int GW_FeodCmdProcess(CG_CmdPacket *cmd);

#define GW_FEOD_CMD_FUNCTION(func) int func(CG_CmdPacket *packet)

GW_FEOD_CMD_FUNCTION(cmd_c2r_feod_content_query);
GW_FEOD_CMD_FUNCTION(cmd_c2r_feod_taxrate);
GW_FEOD_CMD_FUNCTION(cmd_c2r_feod_feodbank_withdraw);
GW_FEOD_CMD_FUNCTION(cmd_r2g_feod_new_income);
GW_FEOD_CMD_FUNCTION(cmd_r2g_feod_change_union);


class CFeodCmdHelper_GW
{
public:
	static void Send_r2c_feod_content(DWORD playerSendTo, char* feodName, int incomeToday, short nTaxRate, int feodBank);
	static void Send_r2c_feod_taxrate_result(DWORD playerSendTo, char* feodName, BYTE errorCode, short nRate);
	static void Send_r2c_feod_feodbank_withdraw_result(DWORD playerSendTo, char* feodName, BYTE errorCode, int newBankMoney, int withdrawMoney);

	static void UpdateRegionFeodData_r2c_feod_data(int iSessionIndex);
};


#endif //_GW_CMD_FEOD_H
