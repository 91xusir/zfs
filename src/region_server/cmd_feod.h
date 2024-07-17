#ifndef CMD_FEOD_H
#define CMD_FEOD_H

class CRegionCreature;
class CUnion;

int FeodCmdProcess_Svr(CRegionCreature* CmdGiver, CG_CmdPacket *cmd);
int FeodCmdProcess_FromGW(CG_CmdPacket *cmd);

#define FEOD_CMD_FUNCTION(func)				int func(CRegionCreature* CmdGiver, CG_CmdPacket *cmd)
#define FEOD_CMD_FUNCTION_FROM_GW(func)		int func(CG_CmdPacket *cmd)


FEOD_CMD_FUNCTION(cmd_c2r_feod_content_query);
FEOD_CMD_FUNCTION(cmd_c2r_feod_taxrate);
FEOD_CMD_FUNCTION(cmd_c2r_feod_feodbank_withdraw);

FEOD_CMD_FUNCTION_FROM_GW(cmd_r2c_feod_content);
FEOD_CMD_FUNCTION_FROM_GW(cmd_r2c_feod_taxrate_result);
FEOD_CMD_FUNCTION_FROM_GW(cmd_r2c_feod_feodbank_withdraw_result);
FEOD_CMD_FUNCTION_FROM_GW(cmd_g2r_feod_data);


class CFeodCmdBuilder_Svr
{
public:
	static void Build_r2c_feod_content(char* feodName, int incomeToday, short nTaxRate, int feodBank);
	static void Build_r2c_feod_taxrate_result(char* feodName, BYTE errorCode, short newTaxRate);
	static void Build_r2c_feod_feodbank_withdraw_result(char* feodName, BYTE errorCode, int newBankMoney);
	static void Build_r2c_feod_data();

	static void NetSend_r2g_feod_change_union(char* feodName, DWORD unionID);
};


#endif //CMD_FEOD_H
