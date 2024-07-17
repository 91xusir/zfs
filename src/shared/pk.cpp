
#include "core/rt2_core.h"
#include "rt1tort2.h"
#include "pk.h"

SPKConfig *g_pkConfig = NULL;

void LoadPKConfig(const char* szPKConfigFile)
{
    RtIni ini;
    if (!ini.OpenFile(szPKConfigFile))
    {
        RtCoreLog().Info("Error: Can't open PK Config File [%s].\n", szPKConfigFile);
        return;
    }

    // StateChangeTime
	g_pkConfig = RT_NEW SPKConfig();
    ini.GetEntry("StateChangeTime", "YellowName",   &(g_pkConfig->iStateChangeTimeYellowName));
    ini.GetEntry("StateChangeTime", "RedName",      &(g_pkConfig->iStateChangeTimeRedName));
    ini.GetEntry("StateChangeTime", "Increase",     &(g_pkConfig->iStateChangeTimeIncrease));


    // SafeRebirth

    ini.GetEntry("SafeRebirth", "ZouX",     &(g_pkConfig->fSafeRebirthZouX));
    ini.GetEntry("SafeRebirth", "ZouY",     &(g_pkConfig->fSafeRebirthZouY));
    ini.GetEntry("SafeRebirth", "ShangX",   &(g_pkConfig->fSafeRebirthShangX));
    ini.GetEntry("SafeRebirth", "ShangY",   &(g_pkConfig->fSafeRebirthShangY));
	ini.GetEntry("SafeRebirth", "ZhouOtherX",	&(g_pkConfig->fSafeRebirthZhouOtherX));
	ini.GetEntry("SafeRebirth", "ZhouOtherY",	&(g_pkConfig->fSafeRebirthZhouOtherY));
	ini.GetEntry("SafeRebirth", "ShangOtherX",	&(g_pkConfig->fSafeRebirthShangOtherX));
	ini.GetEntry("SafeRebirth", "ShangOtherY",	&(g_pkConfig->fSafeRebirthShangOtherY));

	//tim.yang  °×ÃûµôÂäµÀ¾ß

	ini.GetEntry("WhiteName", "Money",         &(g_pkConfig->lostWhite.iMoney));
	ini.GetEntry("WhiteName", "MoneyMinRate",  &(g_pkConfig->lostWhite.iMoneyMinRate));
	ini.GetEntry("WhiteName", "MoneyMaxRate",  &(g_pkConfig->lostWhite.iMoneyMaxRate));

	ini.GetEntry("WhiteName", "Item",          &(g_pkConfig->lostWhite.iItem));
	ini.GetEntry("WhiteName", "ItemMinPiece",  &(g_pkConfig->lostWhite.iItemMinPiece));
	ini.GetEntry("WhiteName", "ItemMaxPiece",  &(g_pkConfig->lostWhite.iItemMaxPiece));

	ini.GetEntry("WhiteName", "Equip",         &(g_pkConfig->lostWhite.iEquip));
	ini.GetEntry("WhiteName", "EquipMinPiece", &(g_pkConfig->lostWhite.iEquipMinPiece));
	ini.GetEntry("WhiteName", "EquipMaxPiece", &(g_pkConfig->lostWhite.iEquipMaxPiece));
	//end

    // YellowName

    ini.GetEntry("YellowName", "Money",         &(g_pkConfig->lostYellow.iMoney));
    ini.GetEntry("YellowName", "MoneyMinRate",  &(g_pkConfig->lostYellow.iMoneyMinRate));
    ini.GetEntry("YellowName", "MoneyMaxRate",  &(g_pkConfig->lostYellow.iMoneyMaxRate));

    ini.GetEntry("YellowName", "Item",          &(g_pkConfig->lostYellow.iItem));
    ini.GetEntry("YellowName", "ItemMinPiece",  &(g_pkConfig->lostYellow.iItemMinPiece));
    ini.GetEntry("YellowName", "ItemMaxPiece",  &(g_pkConfig->lostYellow.iItemMaxPiece));

    ini.GetEntry("YellowName", "Equip",         &(g_pkConfig->lostYellow.iEquip));
    ini.GetEntry("YellowName", "EquipMinPiece", &(g_pkConfig->lostYellow.iEquipMinPiece));
    ini.GetEntry("YellowName", "EquipMaxPiece", &(g_pkConfig->lostYellow.iEquipMaxPiece));


    // RedName

    ini.GetEntry("RedName", "Money",            &(g_pkConfig->lostRed.iMoney));
    ini.GetEntry("RedName", "MoneyMinRate",     &(g_pkConfig->lostRed.iMoneyMinRate));
    ini.GetEntry("RedName", "MoneyMaxRate",     &(g_pkConfig->lostRed.iMoneyMaxRate));

    ini.GetEntry("RedName", "Item",             &(g_pkConfig->lostRed.iItem));
    ini.GetEntry("RedName", "ItemMinPiece",     &(g_pkConfig->lostRed.iItemMinPiece));
    ini.GetEntry("RedName", "ItemMaxPiece",     &(g_pkConfig->lostRed.iItemMaxPiece));

    ini.GetEntry("RedName", "Equip",            &(g_pkConfig->lostRed.iEquip));
    ini.GetEntry("RedName", "EquipMinPiece",    &(g_pkConfig->lostRed.iEquipMinPiece));
    ini.GetEntry("RedName", "EquipMaxPiece",    &(g_pkConfig->lostRed.iEquipMaxPiece));

	//exploit
	ini.GetEntry("exploit", "on_off",			&(g_pkConfig->exploitManager.iOn_Off));
	ini.GetEntry("exploit", "begintime",		&(g_pkConfig->exploitManager.iBeginTime));
	ini.GetEntry("exploit", "endtime",			&(g_pkConfig->exploitManager.iEndTime));
	ini.GetEntry("exploit", "die_interval",		&(g_pkConfig->exploitManager.iDie_Interval));
	ini.GetEntry("exploit", "random",			&(g_pkConfig->exploitManager.iRandom));

}


/*----------------------------------------------------------------------------
-   End.
----------------------------------------------------------------------------*/
