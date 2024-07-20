#include "gc_include.h"
#include "UIForm_Char.h"
#include "ui_form_union.h"
#include "ui_form_playeritems.h"
#include "gc_player.h"
#include "ui_form_msg.h"
#include "ui_form_trade.h"
#include "ui_form_shop.h"
#include "ui_form_skill.h"
#include "trump_show.h"
#include "weapon_show.h"

const std::string strBornTag[]			= {"子鼠","丑牛","寅虎","卯兔","辰龙","巳蛇","午马","未羊","申猴","酉鸡","戌狗","亥猪"};
const std::string strSex[]				= {"男","女"};
const std::string strEmployment[]		= {"","武台","花间","蜀山","苗疆"};
const std::string strXiuWuJieDuan[]		= {"","炼气","筑基","金丹","元婴","分神","道合","大乘","飞仙"};

const int maxshengwang = 1000000;

CUIForm_Char::CUIForm_Char(void):
m_istrtmp(0),
m_icontmp(0),
m_idextmp(0),
m_iinttmp(0),
m_ihittmp(0),
m_iAllpoint(0)
{
	guard;
	m_pFromThis			= RTW_WIDGET("fmplaymain");
	m_pFromThis->EvHide	+=RTW_CALLBACK(this,CUIForm_Char,OnHide);
	m_ptabThis			= RTW_WIDGET_T(RtwTab,"fmplaymain.tabitem");
	m_ptrHoverImage		= g_workspace.getImageFactory()->createImage("ui/textures/highlight_button.tga");
	m_eNowShow			= NOSELECT;

	m_ptabThis->EvChangeSelected	+= RTW_CALLBACK(this,CUIForm_Char,OnChangeTab);
	m_ptrHoverImage->getRenderImage()->SetTransparency(0.5);
	m_ptrHoverImage->getRenderImage()->eBlendDstFunc = RTGBLEND_ONE;
	m_ptrHoverImage->getRenderImage()->eBlendSrcFunc = RTGBLEND_SRCALPHA;
	m_ptrHoverImage->getRenderImage()->SetBlend(true);

	// 第一人物属性界面
	m_plbMetier			= RTW_WIDGET("fmplaymain.tabitem.fmAttrib.lbMeter");
	m_plbName			= RTW_WIDGET("fmplaymain.tabitem.fmAttrib.lbplayername");
	m_plbAnimal			= RTW_WIDGET("fmplaymain.tabitem.fmAttrib.lbAnimate");
	m_plbPKlev			= RTW_WIDGET("fmplaymain.tabitem.fmAttrib.lbpklev");
	m_plbLevel			= RTW_WIDGET("fmplaymain.tabitem.fmAttrib.lbplayerlev");
	m_checkShowModel	= RTW_WIDGET_T(CUiCheckButton,"fmplaymain.tabitem.fmAttrib.btnswitchmodel");
	m_btnLeftRotate		= RTW_WIDGET_T(RtwButton,"fmplaymain.tabitem.fmAttrib.btleft");
	m_btnRightRotate	= RTW_WIDGET_T(RtwButton,"fmplaymain.tabitem.fmAttrib.btright");
	m_btnAttribute		= RTW_WIDGET_T(RtwButton,"fmplaymain.tabitem.fmAttrib.btnupdate");
	m_p3DPlayerView		= RTW_WIDGET_T(Rtw3DView,"fmplaymain.tabitem.fmAttrib.lbplayeract");
	m_btnParts[CItemContainerBase::WEAPON_1]	= RTW_WIDGET_T(RtwButton, "fmplaymain.tabitem.fmAttrib.btnRightHand");
	m_btnParts[CItemContainerBase::WEAPON_2]	= NULL;
	m_btnParts[CItemContainerBase::BODY]		= RTW_WIDGET_T(RtwButton, "fmplaymain.tabitem.fmAttrib.btnCloth");
	m_btnParts[CItemContainerBase::CHARM]		= RTW_WIDGET_T(RtwButton, "fmplaymain.tabitem.fmAttrib.btnCharm");
	m_btnParts[CItemContainerBase::FOOT]		= RTW_WIDGET_T(RtwButton, "fmplaymain.tabitem.fmAttrib.btnShoes");
	m_btnParts[CItemContainerBase::NECKLACE]	= RTW_WIDGET_T(RtwButton, "fmplaymain.tabitem.fmAttrib.btnNecklace");
	m_btnParts[CItemContainerBase::GLOVE]		= RTW_WIDGET_T(RtwButton, "fmplaymain.tabitem.fmAttrib.btnGlove");
	m_btnParts[CItemContainerBase::RING]		= RTW_WIDGET_T(RtwButton, "fmplaymain.tabitem.fmAttrib.btnRing");
	m_btnParts[CItemContainerBase::CAESTUS]		= RTW_WIDGET_T(RtwButton, "fmplaymain.tabitem.fmAttrib.btnCaestus");
	m_btnParts[CItemContainerBase::RING2]		= RTW_WIDGET_T(RtwButton, "fmplaymain.tabitem.fmAttrib.btnRing2");
	m_btnParts[CItemContainerBase::KITS]		= RTW_WIDGET_T(RtwButton, "fmplaymain.tabitem.fmAttrib.btnKits");
	m_btnParts[CItemContainerBase::TRUMP]		= RTW_WIDGET_T(RtwButton, "fmplaymain.tabitem.fmAttrib.btnTrump");
	m_btnParts[CItemContainerBase::FASHION]		= RTW_WIDGET_T(RtwButton, "fmplaymain.tabitem.fmAttrib.btnFashion");
	m_btnParts[CItemContainerBase::SEALEDBOOK]	= RTW_WIDGET_T(RtwButton, "fmplaymain.tabitem.fmAttrib.btnBook");
	m_btnParts[CItemContainerBase::AEROCRAFT]	= RTW_WIDGET_T(RtwButton, "fmplaymain.tabitem.fmAttrib.btnFly");
	m_btnParts[CItemContainerBase::WEAPON_3]	= RTW_WIDGET_T(RtwButton, "fmplaymain.tabitem.fmAttrib.btnLeftHand");

	m_btnLeftRotate->EvMouseCapture			+= RTW_CALLBACK(this, CUIForm_Char, OnLClick_RotatePlayer);
	m_btnRightRotate->EvMouseCapture			+= RTW_CALLBACK(this, CUIForm_Char, OnLClick_RotatePlayer);

	for (int nPart=0; nPart<CItemContainerBase::MAX_EQUIP_PARTS; nPart++)
	{
		if(nPart == CItemContainerBase::WEAPON_2)
			continue;
		m_btnParts[nPart]->SetHighlightImage(m_ptrHoverImage);
	}

	for (int i=0; i<CItemContainerBase::MAX_EQUIP_PARTS; i++)
	{
		if(i == CItemContainerBase::WEAPON_2)
			continue;
		m_btnParts[i]->ModifyFlags(wfDragIn|wfDragClient|wfHint|wfDClick, 0);
		m_btnParts[i]->EvLClick			+= RTW_CALLBACK(this, CUIForm_Char, OnLClick_Parts);
		m_btnParts[i]->EvRClick			+= RTW_CALLBACK(this, CUIForm_Char, OnRClick_Parts);
		m_btnParts[i]->EvDrag			+= RTW_CALLBACK(this, CUIForm_Char, OnDrag_Parts);
		m_btnParts[i]->EvDragEnd		+= RTW_CALLBACK(this, CUIForm_Char, OnDragEnd_Parts);
		m_btnParts[i]->EvHint			+= RTW_CALLBACK(this, CUIForm_Char, OnGetHintText_Parts);
	}

	m_imgAnimal		= g_workspace.getImageFactory()->createImage("ui/textures/9.tga");
	m_imgMetier		= g_workspace.getImageFactory()->createImage("ui/textures/9.tga");
	m_imgAnimal->SetBlend(true);
	m_imgMetier->SetBlend(true);
	m_btnAttribute->EvLClick		+= RTW_CALLBACK(this, CUIForm_Char, OnLClick_BtnAttribute);

	m_pfrmChar			= RTW_WIDGET("fmnewchar");
	m_pfrmChar->EvHide	+=RTW_CALLBACK(this,CUIForm_Char,OnCharHide);

	/*m_plbSite			= RTW_WIDGET("layworld.fmnewchar.lbrank4.lbsitename");
	m_plbAnimals		= RTW_WIDGET("layworld.fmnewchar.lbrank6.lbtext3");
	m_plbSex			= RTW_WIDGET("layworld.fmnewchar.lbrank8.lbtext4");
	m_plbEmployment		= RTW_WIDGET("layworld.fmnewchar.lbrank10.lbjobname");*/
	m_hpBar				= RTW_WIDGET_T(RtwProgressBar,"fmnewchar.fmplayerstatus.fmstatuspbar.HPBar");
	m_mpBar				= RTW_WIDGET_T(RtwProgressBar,"fmnewchar.fmplayerstatus.fmstatuspbar.MPBar");
	m_powerBar			= RTW_WIDGET_T(RtwProgressBar,"fmnewchar.fmplayerstatus.fmstatuspbar.ENBar");
	m_expBar			= RTW_WIDGET_T(RtwProgressBar,"fmnewchar.fmplayerstatus.fmstatuspbar.EXPBar");

	m_plbhpcount		= RTW_WIDGET("fmnewchar.fmplayerstatus.fmstatuspbar.lbhpcount");
	m_plbmpcount		= RTW_WIDGET("fmnewchar.fmplayerstatus.fmstatuspbar.lbmpcount");
	m_plbpowercount		= RTW_WIDGET("fmnewchar.fmplayerstatus.fmstatuspbar.lbwlk3");
	m_plbexpcount		= RTW_WIDGET("fmnewchar.fmplayerstatus.fmstatuspbar.lbexpcount");
	m_plbUnion			= RTW_WIDGET("fmnewchar.fmplayerstatus.fmstatuspbar.lbgroupname");

	m_plbstr			= RTW_WIDGET("fmnewchar.fmproperty1.fmxmm1.lbtitle3.lbstrpoint");
	m_plbcon			= RTW_WIDGET("fmnewchar.fmproperty1.fmxmm2.lbtitle3.lbhealthpoint");
	m_plbdex			= RTW_WIDGET("fmnewchar.fmproperty1.fmxmm3.lbtitle3.lbhitpoint");
	m_plbint			= RTW_WIDGET("fmnewchar.fmproperty1.fmxmm4.lbtitle3.lbdexpoint");
	m_plbhit			= RTW_WIDGET("fmnewchar.fmproperty1.fmxmm5.lbtitle3.lbwispoint");
	m_plbApppoint		= RTW_WIDGET("fmnewchar.fmproperty1.fmxmm6.lbtitle3.lbappoint");

	m_pbtnStrAdd		= RTW_WIDGET_T(RtwButton,"fmnewchar.fmproperty1.fmxmm1.btnstradd");
	m_pbtnConAdd		= RTW_WIDGET_T(RtwButton,"fmnewchar.fmproperty1.fmxmm2.btnhealadd");
	m_pbtnDexAdd		= RTW_WIDGET_T(RtwButton,"fmnewchar.fmproperty1.fmxmm3.btnhitadd");
	m_pbtnIntAdd		= RTW_WIDGET_T(RtwButton,"fmnewchar.fmproperty1.fmxmm4.btndexadd");
	m_pbtnHitAdd		= RTW_WIDGET_T(RtwButton,"fmnewchar.fmproperty1.fmxmm5.btnwisadd");
	m_pbtnStrAdd->EvLClick		+= RTW_CALLBACK(this,CUIForm_Char,Add_SubButton);
	m_pbtnConAdd->EvLClick		+= RTW_CALLBACK(this,CUIForm_Char,Add_SubButton);
	m_pbtnDexAdd->EvLClick		+= RTW_CALLBACK(this,CUIForm_Char,Add_SubButton);
	m_pbtnIntAdd->EvLClick		+= RTW_CALLBACK(this,CUIForm_Char,Add_SubButton);
	m_pbtnHitAdd->EvLClick		+= RTW_CALLBACK(this,CUIForm_Char,Add_SubButton);

	m_pbtnStrSub		= RTW_WIDGET_T(RtwButton,"fmnewchar.fmproperty1.fmxmm1.lbless");
	m_pbtnConSub		= RTW_WIDGET_T(RtwButton,"fmnewchar.fmproperty1.fmxmm2.lbless");
	m_pbtnDexSub		= RTW_WIDGET_T(RtwButton,"fmnewchar.fmproperty1.fmxmm3.lbless");
	m_pbtnIntSub		= RTW_WIDGET_T(RtwButton,"fmnewchar.fmproperty1.fmxmm4.lbless");
	m_pbtnHitSub		= RTW_WIDGET_T(RtwButton,"fmnewchar.fmproperty1.fmxmm5.lbless");
	m_pbtnStrSub->EvLClick		+= RTW_CALLBACK(this,CUIForm_Char,Add_SubButton);
	m_pbtnConSub->EvLClick		+= RTW_CALLBACK(this,CUIForm_Char,Add_SubButton);
	m_pbtnDexSub->EvLClick		+= RTW_CALLBACK(this,CUIForm_Char,Add_SubButton);
	m_pbtnIntSub->EvLClick		+= RTW_CALLBACK(this,CUIForm_Char,Add_SubButton);
	m_pbtnHitSub->EvLClick		+= RTW_CALLBACK(this,CUIForm_Char,Add_SubButton);

	m_plbStrPower		= RTW_WIDGET_T(RtwButton,"fmnewchar.fmproperty1.fmxmm1.lbtitle4.lbstrpowernum");
	m_plbConPower		= RTW_WIDGET_T(RtwButton,"fmnewchar.fmproperty1.fmxmm2.lbtitle4.lbconpowernum");
	m_plbDexPower		= RTW_WIDGET_T(RtwButton,"fmnewchar.fmproperty1.fmxmm3.lbtitle4.lbhitpowernum");
	m_plbIntPower		= RTW_WIDGET_T(RtwButton,"fmnewchar.fmproperty1.fmxmm4.lbtitle4.lbdexpowernum");
	m_plbHitPower		= RTW_WIDGET_T(RtwButton,"fmnewchar.fmproperty1.fmxmm5.lbtitle4.lbintpowernum");

	m_pbtnOK			= RTW_WIDGET_T(RtwButton,"fmnewchar.fmproperty1.fmxmm6.btnok");
	m_pbtnCancel		= RTW_WIDGET_T(RtwButton,"fmnewchar.fmproperty1.fmxmm6.btncancel");
	m_pbtnOK->EvLClick		+= RTW_CALLBACK(this,CUIForm_Char,OnBtnOK);
	m_pbtnCancel->EvLClick	+= RTW_CALLBACK(this,CUIForm_Char,OnBtnCancel);
						
	m_ptabAttribute		= RTW_WIDGET_T(RtwTab,"fmnewchar.fmattrib.tbcattrib");

	m_plbAttack			= RTW_WIDGET("fmnewchar.fmattrib.tbcattrib.fmattackattrib.lbnum1");
	m_plbArmor			= RTW_WIDGET("fmnewchar.fmattrib.tbcattrib.fmattackattrib.lbnum3");
	m_plbDamage			= RTW_WIDGET("fmnewchar.fmattrib.tbcattrib.fmattackattrib.lbnum2");
	m_plbDodge			= RTW_WIDGET("fmnewchar.fmattrib.tbcattrib.fmattackattrib.lbnum4");
	m_plbAttackSpeed	= RTW_WIDGET("fmnewchar.fmattrib.tbcattrib.fmattackattrib.lbnum5");
	m_plbAttackCritical	= RTW_WIDGET("fmnewchar.fmattrib.tbcattrib.fmattackattrib.lbnum6");
	m_plbReAttackCritical=RTW_WIDGET("fmnewchar.fmattrib.tbcattrib.fmattackattrib.lbnum7");
	m_plbSkillCritical	= RTW_WIDGET("fmnewchar.fmattrib.tbcattrib.fmattackattrib.lbnum8");
	m_plbReSkillCritical= RTW_WIDGET("fmnewchar.fmattrib.tbcattrib.fmattackattrib.lbnum9");

	m_plbFireAtt		= RTW_WIDGET("fmnewchar.fmattrib.tbcattrib.fmelementattrib.fmfireAT.lbnum1");
	m_plbFireDef		= RTW_WIDGET("fmnewchar.fmattrib.tbcattrib.fmelementattrib.fmfireDEF.lbnum2");
	m_plbWaterAtt		= RTW_WIDGET("fmnewchar.fmattrib.tbcattrib.fmelementattrib.fmwaterAT.lbnum3");
	m_plbWaterDef		= RTW_WIDGET("fmnewchar.fmattrib.tbcattrib.fmelementattrib.fmwaterDEF.lbnum4");
	m_plbPoisonAtt		= RTW_WIDGET("fmnewchar.fmattrib.tbcattrib.fmelementattrib.fmposionAT.lbnum5");
	m_plbPoisonDef		= RTW_WIDGET("fmnewchar.fmattrib.tbcattrib.fmelementattrib.fmposionDEF.lbnum6");

	// 2
	m_pbtnTrump			= RTW_WIDGET_T(RtwButton,"fmplaymain.tabitem.fmmagicweapon.fmMWattrib.fmattrib.magicweaponicon");
	m_plbTrumpLev		= RTW_WIDGET("fmplaymain.tabitem.fmmagicweapon.fmMWattrib.fmattrib.lblMWlev");
	m_plbTrumpFireAtt	= RTW_WIDGET("fmplaymain.tabitem.fmmagicweapon.fmMWattrib.fmattrib.lblfireAT");
	m_plbTrumpFireDef	= RTW_WIDGET("fmplaymain.tabitem.fmmagicweapon.fmMWattrib.fmattrib.lblfireDEF");
	m_plbTrumpWaterAtt	= RTW_WIDGET("fmplaymain.tabitem.fmmagicweapon.fmMWattrib.fmattrib.lblwaterAT");
	m_plbTrumpWaterDef	= RTW_WIDGET("fmplaymain.tabitem.fmmagicweapon.fmMWattrib.fmattrib.lblwaterDEF");
	m_plbTrumpPoisonAtt	= RTW_WIDGET("fmplaymain.tabitem.fmmagicweapon.fmMWattrib.fmattrib.lblposionAT");
	m_plbTrumpPoisonDef	= RTW_WIDGET("fmplaymain.tabitem.fmmagicweapon.fmMWattrib.fmattrib.lblposionDEF");

	m_pproActorSoul		= RTW_WIDGET_T(RtwProgressBar,"fmplaymain.tabitem.fmmagicweapon.fmMWrefinery.fmrefinery.pbarmana");
	m_plbActorSoulNum	= RTW_WIDGET("fmplaymain.tabitem.fmmagicweapon.fmMWrefinery.fmrefinery.lblmana");

	m_pproTrumpSoul		= RTW_WIDGET_T(RtwProgressBar,"fmplaymain.tabitem.fmmagicweapon.fmMWrefinery.fmrefinery.pbarlevuping");
	m_plbTrumpSoulNum	= RTW_WIDGET("fmplaymain.tabitem.fmmagicweapon.fmMWrefinery.fmrefinery.lblMWlevuprate");
	m_pbtnToTrumpStart	= RTW_WIDGET_T(RtwButton,"fmplaymain.tabitem.fmmagicweapon.fmMWrefinery.fmrefinery.btnMWrefinery");
	m_pbtnToTrumpStart->EvLClick += RTW_CALLBACK(this,CUIForm_Char,OnLClick_TrumpRifine);

	m_pcomTrumpType		= RTW_WIDGET_T(RtwComboBox,"fmplaymain.tabitem.fmmagicweapon.fmMWrefinery.fmrefinery.cbtInjection");
	m_pcomTrumpType->EvSelect += RTW_CALLBACK(this,CUIForm_Char,OnSelectTrumpAttach);
	m_pcomTrumpType->RemoveAllItems();
	for (int i=0;i<MAX_ELEMENT;i++)
	{
		m_pcomTrumpType->AddListItem(GetRifineName(i));
	}
	m_pcomTrumpType->SetLineParam(ATTACK_FIRE,(void*)&ATTACK_FIRE);
	m_pcomTrumpType->SetLineParam(DEF_FIRE,(void*)&DEF_FIRE);
	m_pcomTrumpType->SetLineParam(ATTACK_WATER,(void*)&ATTACK_WATER);
	m_pcomTrumpType->SetLineParam(DEF_WATER,(void*)&DEF_WATER);
	m_pcomTrumpType->SetLineParam(ATTACK_POISON,(void*)&ATTACK_POISON);
	m_pcomTrumpType->SetLineParam(DEF_POISON,(void*)&DEF_POISON);

	m_pproTypeSoul		= RTW_WIDGET_T(RtwProgressBar,"fmplaymain.tabitem.fmmagicweapon.fmMWrefinery.fmrefinery.pbarInjecting");
	m_plbTypeSoulNum	= RTW_WIDGET("fmplaymain.tabitem.fmmagicweapon.fmMWrefinery.fmrefinery.lblMWInjectionrate");
	m_pbtnToTypeStart	= RTW_WIDGET_T(RtwButton,"fmplaymain.tabitem.fmmagicweapon.fmMWrefinery.fmrefinery.btnInjection");
	m_pbtnToTypeStart->EvLClick += RTW_CALLBACK(this,CUIForm_Char,OnLClick_TrumpRifine);
	m_plbTypeRate	= RTW_WIDGET("fmplaymain.tabitem.fmmagicweapon.fmMWrefinery.fmrefinery.lbrate");

	m_pbtnTrump->ModifyFlag(wfHint,0);
	m_pbtnTrump->EvHint	+=RTW_CALLBACK(this, CUIForm_Char, OnGetHintTrumpItem);
	RtString tmp;
	for(int i= 1;i<= MAXTRUMPSKILLNUM;i++)
	{
		tmp.Format("fmplaymain.tabitem.fmmagicweapon.fmMWskill.fmattrib.btnMWskillicon%d",i);
		m_pbtnTrumpSkill[i-1]= RTW_WIDGET_T(RtwButton,tmp.c_str());
		m_pbtnTrumpSkill[i-1]->ModifyFlag(wfDragOut|wfHint,0);
		m_pbtnTrumpSkill[i-1]->EvHint	+=RTW_CALLBACK(this, CUIForm_Char, OnGetHintTrumpSkill);
		m_pbtnTrumpSkill[i-1]->EvDrag	 += RTW_CALLBACK(this,CUIForm_Char,OnDrag_TrumpSkill);
		m_pbtnTrumpSkill[i-1]->EvDragEnd += RTW_CALLBACK(this,CUIForm_Char,OnDragEnd_TrumpSkill);
		tmp.Format("fmplaymain.tabitem.fmmagicweapon.fmMWskill.fmattrib.btnMWskill%d",i);
		m_pbtnTrumpSkillup[i-1]= RTW_WIDGET_T(RtwButton,tmp.c_str());
		m_pbtnTrumpSkillup[i-1]->EvLClick += RTW_CALLBACK(this,CUIForm_Char,OnLDClick_LevupTrumpSkill);
	}
	m_bIsRifine = false;

	// 3
	m_plbXiuWeiLev				= RTW_WIDGET("fmplaymain.tabitem.fmpractice.fmpractice2.fmpracticelev.lbpracticelevtext");
	m_plbJieDuan				= RTW_WIDGET("fmplaymain.tabitem.fmpractice.fmpractice2.fmpracticeStage.lbpracticelevtext");
	m_pproXiuWei				= RTW_WIDGET_T(RtwProgressBar,"fmplaymain.tabitem.fmpractice.fmpractice2.fmpresentpractice.pgbpresentpractice");
	m_plbXiuWeiPoint			= RTW_WIDGET("fmplaymain.tabitem.fmpractice.fmpractice2.fmpresentpractice.lbnumber1");
	m_pproActorSoul_XiuWei		= RTW_WIDGET_T(RtwProgressBar,"fmplaymain.tabitem.fmpractice.fmpractice2.fmSoul.pgbSoul");
	m_plbActorSoulNum_xiuWei	= RTW_WIDGET("fmplaymain.tabitem.fmpractice.fmpractice2.fmSoul.lbnumber2");

	m_plbSuming					= RTW_WIDGET("fmplaymain.tabitem.fmpractice.fmactivity2.fmlocalname.lbnumber3");
	m_ppgbSuming				= RTW_WIDGET_T(RtwProgressBar,"fmplaymain.tabitem.fmpractice.fmactivity2.fmlocalname.pgblocalname");
	m_plbMenGong				= RTW_WIDGET("fmplaymain.tabitem.fmpractice.fmactivity2.fmschool.lbnumber4");
	m_ppgbMenGong				= RTW_WIDGET_T(RtwProgressBar,"fmplaymain.tabitem.fmpractice.fmactivity2.fmschool.pgbschool");
	m_plbBangGong				= RTW_WIDGET("fmplaymain.tabitem.fmpractice.fmactivity2.fmfaction.lbnumber5");
	m_ppgbBangGong				= RTW_WIDGET_T(RtwProgressBar,"fmplaymain.tabitem.fmpractice.fmactivity2.fmfaction.pgbfaction");
	m_plbJunGong				= RTW_WIDGET("fmplaymain.tabitem.fmpractice.fmactivity2.fmachievement.lbnumber6");
	m_ppgbJunGong				= RTW_WIDGET_T(RtwProgressBar,"fmplaymain.tabitem.fmpractice.fmactivity2.fmachievement.pgbachievement");
	m_plbJiFen					= RTW_WIDGET("fmplaymain.tabitem.fmpractice.fmactivity2.fmintegral.lbnumber7");
	m_ppgbJiFen					= RTW_WIDGET_T(RtwProgressBar,"fmplaymain.tabitem.fmpractice.fmactivity2.fmintegral.pgbintegral");
	m_plbMorality				= RTW_WIDGET("fmplaymain.tabitem.fmpractice.fmactivity2.fmmoral.lbnumber8");
	m_ppgbMorality				= RTW_WIDGET_T(RtwProgressBar,"fmplaymain.tabitem.fmpractice.fmactivity2.fmmoral.pgbmoral");

	// 4
	m_plbXianYu					= RTW_WIDGET("fmplaymain.tabitem.fmFame.fmreputation2.fmreputation21.lbnumber1");
	m_ppgbXianYu				= RTW_WIDGET_T(RtwProgressBar,"fmplaymain.tabitem.fmFame.fmreputation2.fmreputation21.pgbreputation");
	m_pbtnSwap					= RTW_WIDGET_T(RtwButton,"fmplaymain.tabitem.fmFame.fmreputation2.btnchange");

	m_plbGuanfu					= RTW_WIDGET("fmplaymain.tabitem.fmFame.fmfame2.fmgovernment1.lbtext5");
	m_plbGuanfuCount			= RTW_WIDGET("fmplaymain.tabitem.fmFame.fmfame2.fmgovernment2.lbnumber2");
	m_ppgbGuanfu				= RTW_WIDGET_T(RtwProgressBar,"fmplaymain.tabitem.fmFame.fmfame2.fmgovernment2.pgbgovernment");
	m_plbManzu					= RTW_WIDGET("fmplaymain.tabitem.fmFame.fmfame2.fmtribe1.lbtext7");
	m_plbManzuCount				= RTW_WIDGET("fmplaymain.tabitem.fmFame.fmfame2.fmtribe2.lbnumber3");
	m_ppgbManzu					= RTW_WIDGET_T(RtwProgressBar,"fmplaymain.tabitem.fmFame.fmfame2.fmtribe2.pgbtribe");
	m_plbYishi					= RTW_WIDGET("fmplaymain.tabitem.fmFame.fmfame2.fmrighteousman1.lbtext9");
	m_plbYishiCount				= RTW_WIDGET("fmplaymain.tabitem.fmFame.fmfame2.fmrighteousman2.lbnumber4");
	m_ppgbYishi					= RTW_WIDGET_T(RtwProgressBar,"fmplaymain.tabitem.fmFame.fmfame2.fmrighteousman2.pgbrighteousman");
	m_plbYijun					= RTW_WIDGET("fmplaymain.tabitem.fmFame.fmfame2.fmopposesui1.lbtext11");
	m_plbYijunCount				= RTW_WIDGET("fmplaymain.tabitem.fmFame.fmfame2.fmopposesui2.lbnumber5");
	m_ppgbYijun					= RTW_WIDGET_T(RtwProgressBar,"fmplaymain.tabitem.fmFame.fmfame2.fmopposesui2.pgbopposesui");
	m_plbLianMeng				= RTW_WIDGET("fmplaymain.tabitem.fmFame.fmfame2.fmunion1.lbtext13");
	m_plbLianMengCount			= RTW_WIDGET("fmplaymain.tabitem.fmFame.fmfame2.fmunion2.lbnumber6");
	m_ppgbLianMeng				= RTW_WIDGET_T(RtwProgressBar,"fmplaymain.tabitem.fmFame.fmfame2.fmunion2.pgbunion");
	/*m_pbtnXiuXing		= RTW_WIDGET_T(RtwButton,"fmnewchar.tabctrl.btnwjs1");
	m_pbtnChengjiu		= RTW_WIDGET_T(RtwButton,"fmnewchar.tabctrl.btnwjs2");

	//m_btnClose          = RTW_WIDGET_T(RtwButton, "fmnewchar.btnclose");
	//m_btnClose->EvLClick += RTW_CALLBACK(this, CUIForm_Char, OnHideGuide);



	m_plbGuanfu         = RTW_WIDGET("fmnewchar.tabctrl.fmxmm1.lbwlk8");
	m_plbManzu          = RTW_WIDGET("fmnewchar.tabctrl.fmxmm1.lbwlk9");
	m_plbYishi          = RTW_WIDGET("fmnewchar.tabctrl.fmxmm1.lbwlk10");
	m_plbYijun          = RTW_WIDGET("fmnewchar.tabctrl.fmxmm1.lbwlk11");

	m_plbPk				= RTW_WIDGET("fmnewchar.tabctrl.fmxmm1.lbwlk7");*/
	RTW_WIDGET("fmplaymain.tabitem.btnname")->Disable();

	m_imgCharNormal = g_workspace.getImageFactory()->createImage("ui\\textures\\1.tga");
	m_imgCharNormal->SetSourceRect(RtwRect(119,352,161,394));
	m_imgCharNormal->SetBlend(true);
	m_imgCharAnimation = g_workspace.getImageFactory()->createImage("ui_texture/xinshou_07.gif");
	m_bShowAnimation = false;

	RtwRect imgRect[12]={RtwRect(3,130,47,170),RtwRect(48,130,92,170),RtwRect(91,130,132,170),RtwRect(133,130,175,170),
						 RtwRect(180,130,228,170),RtwRect(230,130,270,170),RtwRect(3,174,47,214),RtwRect(49,174,92,214),
						 RtwRect(94,174,137,214),RtwRect(138,174,175,214),RtwRect(178,174,219,214),RtwRect(222,174,268,214)};
	memcpy(m_imgAnimalRect,imgRect,sizeof(imgRect));
	RtwRect imgMeterRect[4] = {RtwRect(157,217,205,257),RtwRect(0,217,50,257),RtwRect(51,217,98,257),RtwRect(99,217,156,257)};
	memcpy(m_imgMetierRect,imgMeterRect,sizeof(imgMeterRect));

	m_strShengWang[0] = "宿生初逢";
	m_strShengWang[1] = "泛泛之交";
	m_strShengWang[2] = "相知相惜";
	m_strShengWang[3] = "肝胆相照";
	m_strShengWang[4] = "";
	m_strShengWang[5] = "";

	m_bShowChar = true;
	m_bFinishCreateGraph = false;
	unguard;
}

CUIForm_Char::~CUIForm_Char(void)
{
	guard;
	unguard;
}

void CUIForm_Char::Init()
{
	guard;
	unguard;
}

void CUIForm_Char::OnChangeTab(RtwWidget* sender, void*)
{
	guard;
	int iSelect = m_ptabThis->GetSelectedItem();
	if(iSelect == 0)
	{
		m_bShowChar = true;
	}
	else
		m_bShowChar = false;
	bool refresh= false;
	if(iSelect== EQUIP)
		refresh= true;
	Refresh(refresh);
	unguard;
}

bool bReleaseGraph = false;
void CUIForm_Char::Show(bool refresh)
{
	guard;
	m_pPlayer = GetWorld()->m_pPlayer;
	int iSelect = m_ptabThis->GetSelectedItem();

	if ((!IsVisible() && iSelect == EQUIP) || (IsVisible() && m_eNowShow != EQUIP && iSelect == EQUIP))
	{
		bReleaseGraph = false;
		m_bFinishCreateGraph = false;
		Create3DViewGraph();
	}
	else if (!bReleaseGraph && m_eNowShow != EQUIP)
	{
		bReleaseGraph = true;
		m_p3DPlayerView->ReleaseAllActor();
	}

	m_pFromThis->Show();
	switch (iSelect)
	{
	case (int)EQUIP:
		m_eNowShow = EQUIP;
		if(refresh)
			OnShowEquip();
		break;
	case (int)TRUMP:
		if (m_eNowShow == TRUMP)
		{
			RefreshTrump();
			break;
		}
		m_eNowShow = TRUMP;
		OnShowTrump();
		break;
	case (int)NAZARITIMS:
		OnShowXiuXing();
		m_eNowShow = NAZARITIMS;
		break;
	case (int)POPULARITY:
		OnShowShenWang();
		m_eNowShow = POPULARITY;
		break;
	case (int)TITLE:
		m_eNowShow = TITLE;
		break;
	case (int)NOSELECT:
		m_eNowShow = NOSELECT;
		m_pFromThis->Hide();
		break;
	}
	if(m_bShowChar)
	{
		GetWorld()->ShowLifePage();
		m_pfrmChar->Show();
		OnShowAttribute();
	}else
		m_pfrmChar->Hide();

	unguard;
}

void CUIForm_Char::OnShowEquip()
{
	guard;

	// 门派以及生肖的设置,人名以及等级
	SCreature *cre = &GetWorld()->m_pPlayer->m_core;
	char cTmp128[128];
	m_plbName->SetText(m_pPlayer->GetName());
	rt2_snprintf(cTmp128,128,"%d级",cre->Lev);
	//cTmp128[127] = 0;
	m_plbLevel->SetText(cTmp128);

	int iMetier = cre->Metier -1;
	if(iMetier >= 0 && iMetier <= 3)
	{
		RtwRect rect = m_plbMetier->GetFrameRect();
		rect.right = rect.left + m_imgMetierRect[iMetier].right-m_imgMetierRect[iMetier].left;
		m_imgMetier->SetSourceRect(m_imgMetierRect[iMetier]);
		m_plbMetier->SetBackgroundImage(m_imgMetier);
		m_plbMetier->MoveResize(rect,true);
	}
	if(cre->BornTag >=0 && cre->BornTag <= 11)
	{
		RtwRect rect = m_plbAnimal->GetFrameRect();
		rect.right = rect.left + m_imgAnimalRect[cre->BornTag].right-m_imgAnimalRect[cre->BornTag].left;
		m_imgAnimal->SetSourceRect(m_imgAnimalRect[cre->BornTag]);
		m_plbAnimal->SetBackgroundImage(m_imgAnimal);
		m_plbAnimal->MoveResize(rect,true);
	}

	char tmpStr[100];
	rt2_snprintf(tmpStr,100,"%d",cre->WarScores);
	m_plbPKlev->SetText(tmpStr);

	// 装备的处理
	CItemContainer *pC = &(m_pPlayer->mItem);
	SItemID item;
	SItemBase *pItemClass = NULL;
	int part;
	bool bSkipWeapon2 = false;

	// 身体上的装备
	for (part=CItemContainerBase::EQUIP_DEFAULT_PART+1; part<CItemContainerBase::MAX_EQUIP_PARTS; part++)
	{
		if (part==CItemContainerBase::WEAPON_2)
			continue;

		item = pC->GetEquipItem((CItemContainerBase::EEquipParts)part, true);
		if (ItemID_IsValid(item))
		{
			m_btnParts[part]->ModifyFlags(wfDragServer|wfHint, 0);
			pItemClass = GetWorld()->m_pItemManager->GetItem(item.type);
			if (pItemClass && strlen(pItemClass->icon) != 0)
			{
				//if (part==CItemContainerBase::WEAPON_1 && ((SWeapon*)pItemClass)->bBothHands)
				//{// 双手武器,右手显示为虚的
				//	//bSkipWeapon2 = true;
				//	////RtwImage* pImage1 = new RtwImage(pItemClass->icon);// change [3/17/2009 tooth.shi]
				//	//RtwImage* pImage1 = g_workspace.getImageFactory()->createImage(pItemClass->icon);
				//	//m_btnParts[CItemContainerBase::WEAPON_1]->SetBackgroundImage(pImage1);

				//	////RtwImage* pImage2 = new RtwImage(pItemClass->icon);
				//	//RtwImage* pImage2 = g_workspace.getImageFactory()->createImage(pItemClass->icon);
				//	//m_btnParts[CItemContainerBase::WEAPON_2]->SetBackgroundImage(pImage2);
				//	//pImage2->SetColor(0xff7f7f7f);
				//	//m_btnParts[CItemContainerBase::WEAPON_2]->EnableAlphaEffect(true);
				//	//m_btnParts[CItemContainerBase::WEAPON_2]->SetAlphaRef(128);//max
				//}
				//else 
				//{
				//	//RtwImage* pImage = new RtwImage(pItemClass->icon);
				//	RtwImage* pImage = g_workspace.getImageFactory()->createImage(pItemClass->icon);
				//	m_btnParts[part]->SetBackgroundImage(pImage);
				//}

				RtwImage* pImage = g_workspace.getImageFactory()->createImage(pItemClass->icon);
				m_btnParts[part]->SetBackgroundImage(pImage);
				switch(item.color)
				{
				case ItemColor_White:
					m_btnParts[part]->SetMaskImage((RtwImage*)NULL);
					break;
				case ItemColor_Green:
					{
						RtwImage* m_ptrHoverGreenImage = LoadSequenceImage("ui_texture/Pic_range_green.dds");
						if (m_ptrHoverGreenImage)
						{
							m_btnParts[part]->SetMaskImage(m_ptrHoverGreenImage);
						}
						break;
					}
				case ItemColor_Blue:
					{
						RtwImage* m_ptrHoverBlueImage = LoadSequenceImage("ui_texture/Pic_range_blue.dds");
						if (m_ptrHoverBlueImage)
						{
							m_btnParts[part]->SetMaskImage(m_ptrHoverBlueImage);
						}
						break;
					}
				case ItemColor_Golden:
					{
						RtwImage* m_ptrHoverGoldImage = LoadSequenceImage("ui_texture/Pic_range_golden.dds");
						if (m_ptrHoverGoldImage)
						{
							m_btnParts[part]->SetMaskImage(m_ptrHoverGoldImage);
						}
						break;
					}
				default:
					break;
				}
			}

			// 如果道具无效，做一个效果
			if (!pC->IsEquip((CItemContainerBase::EEquipParts)part))
			{
				m_btnParts[part]->EnableAlphaEffect(true);
				m_btnParts[part]->SetAlphaRef(128);
				if (part==CItemContainerBase::WEAPON_1 && ((SWeapon*)pItemClass)->bBothHands)
				{// 双手武器,右手显示为虚的
					//if (m_btnParts[CItemContainerBase::WEAPON_2])
					//{
					//	m_btnParts[CItemContainerBase::WEAPON_2]->EnableAlphaEffect(true);
					//	m_btnParts[CItemContainerBase::WEAPON_2]->SetAlphaRef(128);//max
					//}
				}
			}
			else 
			{
				m_btnParts[part]->EnableAlphaEffect(false);
				m_ptrHoverImage->getRenderImage()->SetTransparency(0.5);
				m_ptrHoverImage->getRenderImage()->eBlendDstFunc = RTGBLEND_ONE;
				m_ptrHoverImage->getRenderImage()->eBlendSrcFunc = RTGBLEND_SRCALPHA;
				m_ptrHoverImage->getRenderImage()->SetBlend(true);
				m_btnParts[part]->SetHoverImage(m_ptrHoverImage);
			}
		}
		else
		{
			m_btnParts[part]->ModifyFlags(0, wfDragServer|wfHint);
			m_btnParts[part]->SetBackgroundImage((RtwImage*)NULL);
			m_btnParts[part]->SetMaskImage((RtwImage*)NULL);
			m_btnParts[part]->EnableAlphaEffect(true);
			RtwComplexImage hoverImg;
			m_btnParts[part]->SetHoverImage(hoverImg);
		}
	}
	unguard;
}

void CUIForm_Char::ReCreate3DViewGraph()
{
	m_bFinishCreateGraph = false;
	Create3DViewGraph(false);
}

void CUIForm_Char::Create3DViewGraph(bool resetView/* = true*/)
{
	if (m_bFinishCreateGraph)
		return;

	if (resetView)
	{
		m_p3DPlayerView->ResetView();
		m_p3DPlayerView->SetDefYawDegree(0.f);
		m_p3DPlayerView->SetUpOffset(3.f);
	}
	else
	{
		m_p3DPlayerView->ReleaseAllActor();
	}
	GcActorGraph *pActorGraph = GetPlayer()->GetGraph();
	if (pActorGraph && pActorGraph->p())
	{
		if (GetPlayer()->GetRideState() == GcActor::ERS_ACTOR)
		{
			GcActorGraph *pPetGraph = GetPlayerPet()->GetPetActor()->GetGraph();
			if (pPetGraph)
			{
				m_p3DPlayerView->AddRenderActor(pPetGraph->p());
				m_p3DPlayerView->SetUpOffset(0);
				m_p3DPlayerView->SetDefForward(8.f);
				m_p3DPlayerView->SetDefYawDegree(38.f);
			}
			m_p3DPlayerView->AddRenderActor(pActorGraph->p());
		}
		else
		{
			if (GetPlayer()->m_bIsDead)
			{
				m_p3DPlayerView->SetDefForward(25.f);
			}
			else
			{
				m_p3DPlayerView->SetDefForward(-2.f);
			}
			m_p3DPlayerView->AddRenderActor(pActorGraph->p());
		}

		m_p3DPlayerView->AddRenderActor(pActorGraph->p());

		CRT_ActorInstance *_pActor = NULL;
		if (GetPlayer()->mBaseActor.m_pTrump && 
			GetPlayer()->mBaseActor.m_pTrump->m_body &&
			GetPlayer()->mBaseActor.m_pTrump->m_bRender &&
			GetPlayer()->m_cShapeshiftNet == 0 )
		{
			_pActor = GetPlayer()->mBaseActor.m_pTrump->m_body;
			if (_pActor->IsDisplay())
			{
				m_p3DPlayerView->AddRenderActor(_pActor);
			}
		}

		if (GetPlayer()->mBaseActor.m_pWeapon && 
			GetPlayer()->mBaseActor.m_pWeapon->m_body &&
			GetPlayer()->mBaseActor.m_pWeapon->m_bRender &&
			GetPlayer()->m_cShapeshiftNet == 0 )
		{
			if (ItemIsWeapon_Hoop(GetPlayer()->mBaseActor.m_pWeapon->m_item))
			{
				if (GetPlayer()->GetGraph()->GetLinkActor("Bip01 R Hand"))
				{
					_pActor = GetPlayer()->GetGraph()->GetLinkActor("Bip01 R Hand")->p();
					if (_pActor->IsDisplay())
					{
						m_p3DPlayerView->AddRenderActor(_pActor);
					}
				}
				if (GetPlayer()->GetGraph()->GetLinkActor("Bip01 L Hand"))
				{
					_pActor = GetPlayer()->GetGraph()->GetLinkActor("Bip01 L Hand")->p();
					if (_pActor->IsDisplay())
					{
						m_p3DPlayerView->AddRenderActor(_pActor);
					}
				}
			}
			else
			{
				_pActor = GetPlayer()->mBaseActor.m_pWeapon->m_body;
				int cont = 0;
				if (_pActor->IsDisplay())
				{
					cont = m_p3DPlayerView->AddRenderActor(_pActor);
					if (GetPlayer()->mBaseActor.m_pWeapon->m_vWorldMatrixOther.size() > 1)
					{
						for (int i=0;i<GetPlayer()->mBaseActor.m_pWeapon->m_vWorldMatrixOther.size()-1;i++)
						{
							m_p3DPlayerView->AddRenderActorMatrix(cont,&GetPlayer()->mBaseActor.m_pWeapon->m_vWorldMatrixOther[i]);
						}
					}
				}
			}
		}
		m_bFinishCreateGraph = true;
	}
}

void CUIForm_Char::OnLClick_RotatePlayer(RtwButton *sender, void*)
{
	if (m_btnLeftRotate == sender)
	{
		m_p3DPlayerView->SetYaw(-30.f);
	}
	else if (m_btnRightRotate == sender)
	{
		m_p3DPlayerView->SetYaw(30.f);
	}
}

void CUIForm_Char::OnShowAttribute()
{
	guard;
	//玩家打开属性页面的UI提示
	GetWorld()->ShowAttrPoint();

	// 恢复默认图片
	SetCharButtonNormal();
	SCreature *cre = &GetWorld()->m_pPlayer->m_core;
	char tmp[100];

	//rt2_snprintf(tmp,99,"%s",cre->Name.c_str());
	//m_pfrmname->SetText((char*)tmp);
	//// 势力
	//if (m_plbSite) 
	//{
	//	if (cre->Faction==FACTION_ZHOU)
	//	{
	//		m_plbSite->SetText(R(G_ZHOU));
	//	}
	//	else if (cre->Faction==FACTION_SHANG)
	//	{
	//		m_plbSite->SetText(R(G_SHANG));
	//	}
	//	else if (cre->Faction==FACTION_NEUTRAL)
	//	{
	//		m_plbSite->SetText(R(G_NEUTRAL));
	//	}
	//}
	//// 生肖
	//if(cre->BornTag >=0 && cre->BornTag <= 11)
	//	m_plbAnimals->SetText(strBornTag[cre->BornTag]);
	//// 性别
	//if(cre->Sex >= 0 && cre->Sex <= 1)
	//	m_plbSex->SetText(strSex[cre->Sex]);
	//// 门派
	//if(cre->Metier >= 0 && cre->Metier<= 4)
	//	m_plbEmployment->SetText(strEmployment[cre->Metier]);
	//// 等级
	//rt2_snprintf(tmp,99,"%d",cre->Lev);
	//m_plbLevel->SetText((char*)tmp);

	// 四个progressbar
	float expRate = (float)(cre->Exp % g_TableExp.GetNextLevExp(cre->Lev)) / g_TableExp.GetNextLevExp(cre->Lev);
	m_expBar->SetValue(expRate);
	m_hpBar->SetValue((float)cre->GetHp()/cre->GetMaxHp());	
	m_mpBar->SetValue((float)cre->GetMp()/cre->GetMaxMp());
	m_powerBar->SetValue((float)cre->GetEnP()/cre->GetMaxEnP());
	// 经验，hp，mp，能量
	rt2_snprintf(tmp,99,"%d/%u",cre->Exp % g_TableExp.GetNextLevExp(cre->Lev),g_TableExp.GetNextLevExp(cre->Lev));
	m_plbexpcount->SetText((char*)tmp);
	rt2_snprintf(tmp,99,"%d/%d",cre->GetHp(),cre->GetMaxHp());
	m_plbhpcount->SetText((char*)tmp);
	rt2_snprintf(tmp,99,"%d/%d",cre->GetMp(),cre->GetMaxMp());
	m_plbmpcount->SetText((char*)tmp);
	rt2_snprintf(tmp,99,"%d/%d",cre->GetEnP(),cre->GetMaxEnP());
	m_plbpowercount->SetText((char*)tmp);

	if (g_layerMain->m_formUnion)
	{
		if (g_layerMain->m_formUnion->m_unionData.m_nID<= 0)
			m_plbUnion->SetText(R(MSG_NO_UNION));
		else 
			m_plbUnion->SetText(g_layerMain->m_formUnion->m_unionData.m_strName);
	}
	// 潜能点
	m_iAllpoint = cre->AttrPoint;
	m_iAllpoint = m_iAllpoint - m_istrtmp - m_icontmp - m_idextmp - m_iinttmp - m_ihittmp;
	rt2_snprintf(tmp,99,"%d",m_iAllpoint);
	m_plbApppoint->SetText((char*)tmp);
	// 当玩家曾经加过技能点后，让确定按钮显示出来
	if(m_istrtmp>0 || m_icontmp >0 || m_idextmp>0 || m_iinttmp>0 || m_ihittmp>0)
	{
		m_pbtnOK->Show();
		m_pbtnCancel->Show();
		m_pbtnStrSub->Show();
		m_pbtnConSub->Show();
		m_pbtnDexSub->Show();
		m_pbtnIntSub->Show();
		m_pbtnHitSub->Show();
	}
	else
	{
		m_pbtnOK->Hide();
		m_pbtnCancel->Hide();
		m_pbtnStrSub->Hide();
		m_pbtnConSub->Hide();
		m_pbtnDexSub->Hide();
		m_pbtnIntSub->Hide();
		m_pbtnHitSub->Hide();
	}
	// 力道
	rt2_snprintf(tmp,99,"%d",cre->Str.GetValue() + m_istrtmp);
	m_plbstr->SetText((char*)tmp);
	if(m_istrtmp > 0)
		m_plbstr->SetTextColor(RtwPixel(0xFF00FF00));
	else
		m_plbstr->SetTextColor(RtwPixel(0xFFFFFFFF));
	// 韧性
	rt2_snprintf(tmp,99,"%d",cre->Con.GetValue() + m_icontmp);
	m_plbcon->SetText((char*)tmp);
	if(m_icontmp > 0)
		m_plbcon->SetTextColor(RtwPixel(0xFF00FF00));
	else
		m_plbcon->SetTextColor(RtwPixel(0xFFFFFFFF));
	// 身法
	rt2_snprintf(tmp,99,"%d",cre->Dex.GetValue() + m_idextmp);
	m_plbdex->SetText((char*)tmp);
	if(m_idextmp > 0)
		m_plbdex->SetTextColor(RtwPixel(0xFF00FF00));
	else
		m_plbdex->SetTextColor(RtwPixel(0xFFFFFFFF));
	// 慧根
	rt2_snprintf(tmp,99,"%d",cre->Int.GetValue() + m_iinttmp);
	m_plbint->SetText((char*)tmp);
	if(m_iinttmp > 0)
		m_plbint->SetTextColor(RtwPixel(0xFF00FF00));
	else
		m_plbint->SetTextColor(RtwPixel(0xFFFFFFFF));
	// 精准
	rt2_snprintf(tmp,99,"%d",cre->Hit.GetValue() + m_ihittmp);
	m_plbhit->SetText((char*)tmp);
	if(m_ihittmp > 0)
		m_plbhit->SetTextColor(RtwPixel(0xFF00FF00));
	else
		m_plbhit->SetTextColor(RtwPixel(0xFFFFFFFF));
	// 增强属性
	rt2_snprintf(tmp,99,"%d",cre->StrPower.GetValue());
	m_plbStrPower->SetText((char*)tmp);
	rt2_snprintf(tmp,99,"%d",cre->ConPower.GetValue());
	m_plbConPower->SetText((char*)tmp);
	rt2_snprintf(tmp,99,"%d",cre->DexPower.GetValue());
	m_plbDexPower->SetText((char*)tmp);
	rt2_snprintf(tmp,99,"%d",cre->IntPower.GetValue());
	m_plbIntPower->SetText((char*)tmp);
	rt2_snprintf(tmp,99,"%d",cre->HitPower.GetValue());
	m_plbHitPower->SetText((char*)tmp);

	// 按钮是否显示
	if(m_iAllpoint > 0)
	{
		m_pbtnStrAdd->Show();
		m_pbtnConAdd->Show();
		m_pbtnDexAdd->Show();
		m_pbtnIntAdd->Show();
		m_pbtnHitAdd->Show();
	}
	else
	{
		m_pbtnStrAdd->Hide();
		m_pbtnConAdd->Hide();
		m_pbtnDexAdd->Hide();
		m_pbtnIntAdd->Hide();
		m_pbtnHitAdd->Hide();
	}

	int iSelect = m_ptabAttribute->GetSelectedItem();
	if(iSelect == 0)
	{
		// 攻击力
		rt2_snprintf(tmp,99,"%d-%d",cre->GetDamageMin(),cre->GetDamageMax());
		m_plbAttack->SetText((char*)tmp);
		m_plbAttack->SetTextColor(GetColor(cre->DamageMin.Rate));
		// 防御
		rt2_snprintf(tmp,99,"%d",cre->Armor.GetValue());
		m_plbArmor->SetText((char*)tmp);
		m_plbArmor->SetTextColor(GetColor(cre->Armor.Rate));
		// 命中
		rt2_snprintf(tmp,99,"%d",cre->GetAttack());
		m_plbDamage->SetText((char*)tmp);
		m_plbDamage->SetTextColor(GetColor(cre->Attack.Rate));
		// 闪避
		rt2_snprintf(tmp,99,"%d",cre->GetDodge());
		m_plbDodge->SetText((char*)tmp);
		m_plbDodge->SetTextColor(GetColor(cre->Dodge.Rate));
		// 攻击速度
		rt2_snprintf(tmp,99,"%d",cre->AttSpeed.GetValue());
		m_plbAttackSpeed->SetText((char*)tmp);
		m_plbAttackSpeed->SetTextColor(GetColor(cre->Dodge.Rate));
		// 普通爆击
		rt2_snprintf(tmp,99,"%d",cre->CriticalHit.GetValue());
		m_plbAttackCritical->SetText((char*)tmp);
		m_plbAttackCritical->SetTextColor(GetColor(cre->Dodge.Rate));
		// 抗普通爆击
		rt2_snprintf(tmp,99,"%d",cre->ReCriticalHit.GetValue());
		m_plbReAttackCritical->SetText((char*)tmp);
		m_plbReAttackCritical->SetTextColor(GetColor(cre->Dodge.Rate));
		// 技能爆击
		rt2_snprintf(tmp,99,"%d",cre->SkillCriRate);
		m_plbSkillCritical->SetText((char*)tmp);
		m_plbSkillCritical->SetTextColor(GetColor(cre->Dodge.Rate));
		// 抗技能爆击
		rt2_snprintf(tmp,99,"%d",cre->ReSkillCritical.GetValue());
		m_plbReSkillCritical->SetText((char*)tmp);
		m_plbReSkillCritical->SetTextColor(GetColor(cre->Dodge.Rate));
	}else if(iSelect == 1)
	{
		// 属性攻击及防御
		rt2_snprintf(tmp,99,"%d",cre->ElementDamage[ELEMENT_FIRE].GetValue());
		m_plbFireAtt->SetText((char*)tmp);
		rt2_snprintf(tmp,99,"%d",cre->ElementArmor[ELEMENT_FIRE].GetValue());
		m_plbFireDef->SetText((char*)tmp);
		rt2_snprintf(tmp,99,"%d",cre->ElementDamage[ELEMENT_WATER].GetValue());
		m_plbWaterAtt->SetText((char*)tmp);
		rt2_snprintf(tmp,99,"%d",cre->ElementArmor[ELEMENT_WATER].GetValue());
		m_plbWaterDef->SetText((char*)tmp);
		rt2_snprintf(tmp,99,"%d",cre->ElementDamage[ELEMENT_POISON].GetValue());
		m_plbPoisonAtt->SetText((char*)tmp);
		rt2_snprintf(tmp,99,"%d",cre->ElementArmor[ELEMENT_POISON].GetValue());
		m_plbPoisonDef->SetText((char*)tmp);
	}
	//if (g_layerMain->m_formUnion)
	//{
	//	if (g_layerMain->m_formUnion->m_unionData.id==0)
	//		m_plbUnion->SetText(R(MSG_NO_UNION));
	//	else 
	//		m_plbUnion->SetText(g_layerMain->m_formUnion->m_unionData.name);
	//}
	//// 属性面板内的两个checkbutton
	//rt2_snprintf(tmp,99,"%ld",cre->JobRecord);
	//m_plbMenGong->SetText((char*)tmp);
	//rt2_snprintf(tmp,99,"%ld",cre->UnionRecord);
	//m_plbBangGong->SetText((char*)tmp);
	//rt2_snprintf(tmp,99,"%ld",cre->Contribute);
	//m_plbSuming->SetText((char*)tmp);
	//rt2_snprintf(tmp,99,"%ld",cre->Credit);
	//m_plbXianYu->SetText((char*)tmp);
	//rt2_snprintf(tmp,99,"%ld",cre->Record);
	//m_plbJunGong->SetText((char*)tmp);
	//rt2_snprintf(tmp,99,"%ld",cre->Point);
	//m_plbJiFen->SetText((char*)tmp);


	//rt2_snprintf(tmp,99,"%d",cre->GradeLev);
	//m_plbXiuWei->SetText((char*)tmp);
	//rt2_snprintf(tmp,99,"%ld",cre->Cul);
	//m_plbXiuWeiPoint->SetText((char*)tmp);
	//rt2_snprintf(tmp,99,"%ld",cre->Soul);
	//m_plbLingLi->SetText((char*)tmp);
	//rt2_snprintf(tmp,99,"%ld",cre->VigourPoint);
	//m_plbYuanLi->SetText((char*)tmp);
	//if(cre->GetGradeStage()>= 1 && cre->GetGradeStage() <= 8)
	//	m_plbJieDuan->SetText(strXiuWuJieDuan[cre->GetGradeStage()]);

	//rt2_snprintf(tmp,99,"%d", cre->Guanfu);
	//m_plbGuanfu->SetText((char*)tmp);
	//rt2_snprintf(tmp,99,"%d", cre->Manzu);
	//m_plbManzu->SetText((char*)tmp);
	//rt2_snprintf(tmp,99,"%d", cre->Yishi);
	//m_plbYishi->SetText((char*)tmp);
	//rt2_snprintf(tmp,99,"%d", cre->Yijun);
	//m_plbYijun->SetText((char*)tmp);
	//rt2_snprintf(tmp,99,"%d", cre->Pk / 1000);
	//m_plbPk->SetText((char*)tmp);
	////
	//vector<SCharImage> images;
	//if (!g_TableCharImage.GetCharImage(cre->actorID, images))
	//	return;

	//std::string strImage = "ui_texture/";
	//strImage += images[cre->HeadImageID].imageGame;
	//strImage += ".dds";

	//RtwImage *pImage = g_workspace.getImageFactory()->createImage(strImage);
	//pImage->SetBlend(true);
	//m_pimFace->SetBackgroundImage(pImage);
	unguard;
}

void CUIForm_Char::ResetTrumpUI()
{
	m_pbtnTrump->SetBackgroundImage((RtwImage*)NULL);
	m_plbTrumpLev->SetText("");
	m_plbTrumpFireAtt->SetText("");
	m_plbTrumpFireDef->SetText("");
	m_plbTrumpWaterAtt->SetText("");
	m_plbTrumpWaterDef->SetText("");
	m_plbTrumpPoisonAtt->SetText("");
	m_plbTrumpPoisonDef->SetText("");

	m_pproActorSoul->SetValue(0.f);
	m_plbActorSoulNum->SetText("");

	m_pproTrumpSoul->SetValue(0.f);
	m_plbTrumpSoulNum->SetText("");
	m_pbtnToTrumpStart->Disable();
	m_plbTypeRate->SetText("");

	m_pbtnToTrumpStart->SetText("炼化");
	m_pbtnToTypeStart->SetText("注入");

	m_pproTypeSoul->SetValue(0.f);
	m_plbTypeSoulNum->SetText("");
	m_pbtnToTypeStart->Disable();
	m_pcomTrumpType->SetSelectedItem();

	for (int i=0;i<MAXTRUMPSKILLNUM;i++)
	{
		m_pbtnTrumpSkill[i]->SetBackgroundImage((RtwImage*)NULL);
		m_pbtnTrumpSkillup[i]->Disable();
	}
	m_bIsRifine = false;
}

void CUIForm_Char::OnSelectTrumpAttach(RtwComboBox* sender, void*)
{
	RtwListBox::SLine *pLine =  m_pcomTrumpType->GetSelectLine();
	if (pLine && (int*)pLine->param)
	{
		int type = *(int*)pLine->param;
		RtString temStr;
		SItemID trump = GetPlayer()->mItem.GetEquipItem(CItemContainerBase::TRUMP, true);
		if (ItemIsTrump(trump))
		{
			int nMaxTypeExp = TRUMP_CentiSoul(trump,type) * 100.f;
			temStr.Format("%d/%d",trump.params[RifineToTrumpParamExp(type)],nMaxTypeExp);
			m_pproTypeSoul->SetValue((float)trump.params[RifineToTrumpParamExp(type)]/(float)nMaxTypeExp);
			m_plbTypeSoulNum->SetText(temStr.c_str());
			m_pbtnToTypeStart->Enable();
			return;
		}
	}
	m_pbtnToTypeStart->Disable();
}

void CUIForm_Char::OnLDClick_LevupTrumpSkill(RtwButton *sender, void*)
{
	GcActor* pActor = GetPlayer();
	if (pActor==NULL) return;
	unsigned short wSkillID = *(unsigned short*)sender->getParam1();
	SSkill* pSkill = Skill()->FindSkill(wSkillID);
	if (pSkill && pSkill->wClass == 13)
	{
		if (!pActor->m_Skill.FindAllSkill(wSkillID))
		{
			if (pActor->m_Skill.StudySkill(wSkillID, false))
			{
				Refresh();
				return;
			}
			PlayerFailOperate(pActor->m_Skill.GetLastErrInfo());
			return;
		}else
		{
			// 查找下一级的技能
			pSkill = Skill()->FindNextLevelSkill(wSkillID);
			if (pSkill)
			{
				if (pActor->m_Skill.StudySkill(pSkill->wID, false))
				{
					Refresh();
					return;
				}
				PlayerFailOperate(pActor->m_Skill.GetLastErrInfo());
				return;
			}
		}
	}
}

void CUIForm_Char::OnFinishRifineTrump(bool succeed)
{
	m_bIsRifine = false;
	GetPlayer()->AddDoCommandPose();
	GetPlayer()->SetWeaponTrumpVisible(true,true);
	OnShowTrump();
}

void CUIForm_Char::OnLClick_TrumpRifine(RtwButton *sender, void*)
{
	int type;
	if (sender == m_pbtnToTrumpStart)
	{
		type = MP_TRUMP;
	}
	else
	{
		RtwListBox::SLine *pLine =  m_pcomTrumpType->GetSelectLine();
		if (!pLine || !(int*)pLine->param)
			return;

		type = *(int*)pLine->param;
	}

	SItemID trump = GetPlayer()->mItem.GetEquipItem(CItemContainerBase::TRUMP, true);
	if (!ItemIsTrump(trump))
		return;

	//add by yz 2010-6-25: 摆摊时不能炼化
	if (GetPlayer()->m_bPShop)
	{
		ShowSystemMessage(R(MSG_BAITAN_CANNOTLIANHUA));
		return;
	}
	//end yz

	if (GetPlayer()->IsShapeshifted())
	{
		PlayerFailOperate(R(MSG_CANNOTRIFINE_WHEN_SHAPESHIFT));
		return;
	}

	int iValue = 0;
	switch(type)
	{
	case MP_TRUMP:
		if(trump.params[TRUMP_LEV] >= GetPlayer()->m_core.GradeLev)
		{
			PlayerFailOperate(R(MSG_CANNOTRIFINE_WHEN_GRADELEV));return;
		}
		iValue = trump.params[TRUMP_MP];
		break;
	default:
		iValue = trump.params[RifineToTrumpParamExp(type)];
		break;
	}

	float iSpeed = TRUMP_ActorSoulSec(trump,type);
	float rifinePointTime = TRUMP_CentiSoul(trump,type) / (float)iSpeed;
	long rifineNeedSoul = TRUMP_CentiSoul(trump,type) * 100.f;
	float rifineLevupTime = rifinePointTime * float(rifineNeedSoul - iValue);
	float actorSoulOverTime = GetPlayer()->m_core.Soul / iSpeed;

	if (GetPlayer()->m_core.Soul <= 0 || (actorSoulOverTime < 0.f && rifineLevupTime < 0.f) )
	{
		if (sender == m_pbtnToTrumpStart)
		{
			PlayerFailOperate(R(MSG_FABAO_NOSOULTOLEVUP));
		}
		else
		{
			PlayerFailOperate(R(MSG_FABAO_NOTENOUGHSOUL));
		}
		return;
	}

	if (m_bIsRifine)
	{
		DWORD selectTrumpType = m_pcomTrumpType->GetSelectedItem();
		OnFinishRifineTrump(false);
		GetPlayer()->SendTrumpRifine(type,true);
		m_pcomTrumpType->SetSelectedItem(selectTrumpType);
		return;
	}else
	{
		m_bIsRifine = true;
		GetPlayer()->AddDoCommandPose(DO_COMMAND_RIFINE);
		GetPlayer()->SendTrumpRifine(type);
	}
	if (sender == m_pbtnToTrumpStart)
	{
		m_pbtnToTypeStart->Disable();
		m_pbtnToTrumpStart->SetText("停止");
	}
	else
	{
		m_pbtnToTrumpStart->Disable();
		m_pbtnToTypeStart->SetText("停止");
	}
}

void CUIForm_Char::OnHide(void*,void*)
{
	m_pfrmChar->Hide();

	if (m_bIsRifine)
	{
		OnFinishRifineTrump(false);
		GetPlayer()->SendTrumpRifine(0,true);
	}
}

void CUIForm_Char::ReceiveTrumpCmd(char rifine,long succeed,long temp/* = 0*/)
{
	switch (succeed)
	{
	case 0:		// 失败
		{
			RtgVertex3 pos;
			if (rifine == MP_TRUMP)
			{
				PlayerFailOperate(R(MSG_FABAO_LEVELUPFAIL));
			}
			else
			{
				char str[64];
				char *rifineName = GetRifineName(rifine);
				rt2_sprintf(str,R(MSG_FABAO_SPECIALFAIL),rifineName);
				if (rifineName)
				{
					DEL_ARRAY(rifineName);
				}
				PlayerFailOperate(str);
			}
			OnFinishRifineTrump(false);
			m_pcomTrumpType->SetSelectedItem(rifine);
			g_pSoundMgr->PlayOnce("mw_failed.wav", false, 0, pos);
		}
		break;
	case 1:		// 成功
		{
			RtgVertex3 pos;
			if (rifine == MP_TRUMP)
			{
				DWORD selectTrumpType = m_pcomTrumpType->GetSelectedItem();
				PlayerFailOperate(R(MSG_FABAO_LEVELUP));
				m_bIsRifine = false;
				OnShowTrump();
				OnLClick_TrumpRifine(m_pbtnToTrumpStart,NULL);
				m_pcomTrumpType->SetSelectedItem(selectTrumpType);
			}
			else
			{
				char str[64];
				char *rifineName = GetRifineName(rifine);
				rt2_snprintf(str,64,R(MSG_FABAO_SPECIALLEVELUP),rifineName,temp);
				if (rifineName)
				{
					DEL_ARRAY(rifineName);
				}
				PlayerFailOperate(str);
				OnFinishRifineTrump(true);
				m_pcomTrumpType->SetSelectedItem(rifine);
			}
			g_pSoundMgr->PlayOnce("mw_succeed.wav", false, 0, pos);
		}
		break;
	case 2:		// 炼化中
		{
			RtString temStr;
			SItemID trump = GetPlayer()->mItem.GetEquipItem(CItemContainerBase::TRUMP, true);
			if (ItemIsTrump(trump))
			{
				if (rifine == MP_TRUMP)
				{
					int nMaxTypeExp = TRUMP_CentiSoul(trump,rifine) * 100.f;
					temStr.Format("%d/%d",temp,nMaxTypeExp);
					m_pproTrumpSoul->SetValue((float)temp/(float)nMaxTypeExp);
					m_plbTrumpSoulNum->SetText(temStr.c_str());
				}
				else
				{
					int nMaxTypeExp = TRUMP_CentiSoul(trump,rifine) * 100.f;
					temStr.Format("%d/%d",temp,nMaxTypeExp);
					m_pproTypeSoul->SetValue((float)temp/(float)nMaxTypeExp);
					m_plbTypeSoulNum->SetText(temStr.c_str());
				}
			}
			//GetPlayer()->m_core.Soul = temp;
			temStr.Format("%d/%d",GetPlayer()->m_core.Soul, GetPlayer()->m_core.GetSoulValueMax());
			m_pproActorSoul->SetValue((float)GetPlayer()->m_core.Soul / (float)GetPlayer()->m_core.GetSoulValueMax());
			m_plbActorSoulNum->SetText(temStr.c_str());
		}
		break;
	case 3:		// 中断
		PlayerFailOperate(R(MSG_FABAO_HAVENOSOUL));
		OnFinishRifineTrump(false);
		break;
	}
}

char *CUIForm_Char::GetRifineName(int attach)
{
	string attachType;
	switch (attach)
	{
	case ATTACK_FIRE:
		attachType = "火伤";
		break;
	case DEF_FIRE:
		attachType = "火防御";
		break;
	case ATTACK_WATER:
		attachType = "水伤";
		break;
	case DEF_WATER:
		attachType = "水防御";
		break;
	case ATTACK_POISON:
		attachType = "毒伤";
		break;
	case DEF_POISON:
		attachType = "毒防御";
		break;
	default:
		attachType = "";
	}
	size_t length = strlen(attachType.c_str())+1;
	char *str = RT_NEW char[length];
	rt2_strncpy(str,attachType.c_str(), length);
	return str;
}

void CUIForm_Char::OnTrumpSkillLevup()
{
	if (!IsVisible() || m_eNowShow != TRUMP)
	{
		return;
	}

	RtString temStr,tmp;
	SItemID trump = GetPlayer()->mItem.GetEquipItem(CItemContainerBase::TRUMP, true);
	if (ItemIsTrump(trump))
	{
		GcActorSkill* pActorSkill = &(GetPlayer()->m_Skill);
		SItemBase *pItemClass = NULL;
		RtwImage* pImage = NULL;
		SSkill *pSkill = NULL;
		SSkill *pSkillnext = NULL;
		bool bCanNotStudy = false;

		pItemClass = GetWorld()->m_pItemManager->GetItem(trump.type);
		if(!pItemClass)			return;
		string szImageName = pItemClass->icon;
		szImageName +=  "_b";
		m_pbtnTrump->SetBackgroundImage( g_workspace.getImageFactory()->createImage(szImageName) );

		for (int i=TRUMP_SKILL_1,j= 0;i<TRUMP_SKILL_MAX;i++,j++)
		{
			if(trump.params[i] != 0)
			{
				pSkill = Skill()->FindSkill(trump.params[i]);
				if (pSkill)
				{
					if(pSkill->iLevel < pSkill->iMaxLevel)
					{
						pSkillnext = Skill()->FindNextLevelSkill(pSkill->wID);
						bCanNotStudy = g_layerMain->m_fromSkill->CanStudySkill(pSkillnext,pActorSkill);
					}else
						bCanNotStudy = true;
					if(bCanNotStudy)
						m_pbtnTrumpSkillup[j]->Disable();
					else
						m_pbtnTrumpSkillup[j]->Enable();
					tmp = pSkill->szIcon;
					m_pbtnTrumpSkill[j]->SetParam1(&(pSkill->wID));
					m_pbtnTrumpSkillup[j]->SetParam1(&(pSkill->wID));
					if(g_layerMain->m_fromSkill->CreateIconForGameClient(tmp.c_str(),&pImage))
						m_pbtnTrumpSkill[j]->SetBackgroundImage(pImage);
				}
			}
		}
	}else
	{
		m_pbtnTrump->SetBackgroundImage((RtwImage*)NULL);
		m_plbTrumpLev->SetText("");
	}
}

void CUIForm_Char::RefreshTrump()
{
	RtString temStr;
	temStr.Format("%d/%d",GetPlayer()->m_core.Soul, GetPlayer()->m_core.GetSoulValueMax());
	m_pproActorSoul->SetValue((float)GetPlayer()->m_core.Soul / (float)GetPlayer()->m_core.GetSoulValueMax());
	m_plbActorSoulNum->SetText(temStr.c_str());

	SItemID trump = GetPlayer()->mItem.GetEquipItem(CItemContainerBase::TRUMP, true);
	if (ItemIsTrump(trump))
	{
		for (int i=TRUMP_SKILL_1,j= 0;i<TRUMP_SKILL_MAX;i++,j++)
		{
			if(trump.params[i] != 0)
			{
				SSkill*pSkill = Skill()->FindSkill(trump.params[i]);
				if (pSkill)
				{
					bool bCanNotStudy = true;
					if(pSkill->iLevel < pSkill->iMaxLevel)
					{
						SSkill*pSkillnext = Skill()->FindNextLevelSkill(pSkill->wID);
						bCanNotStudy = g_layerMain->m_fromSkill->CanStudySkill(pSkillnext,&(GetPlayer()->m_Skill));
					}else
						bCanNotStudy = true;
					if(bCanNotStudy)
						m_pbtnTrumpSkillup[j]->Disable();
					else
						m_pbtnTrumpSkillup[j]->Enable();
				}
			}
		}
	}
}

void CUIForm_Char::OnShowTrump()
{
	guard;
	GetWorld()->HideGuideForm();

	if (!m_bIsRifine)
		ResetTrumpUI();

	RtString temStr,tmp;
	SItemID trump = GetPlayer()->mItem.GetEquipItem(CItemContainerBase::TRUMP, true);
	if (ItemIsTrump(trump))
	{
		GcActorSkill* pActorSkill = &(GetPlayer()->m_Skill);
		SItemBase *pItemClass = NULL;
		RtwImage* pImage = NULL;
		SSkill *pSkill = NULL;
		SSkill *pSkillnext = NULL;
		bool bCanNotStudy = false;

		pItemClass = GetWorld()->m_pItemManager->GetItem(trump.type);
		if(!pItemClass)			return;
		string filename = pItemClass->icon;
		filename += "_b";
		m_pbtnTrump->SetBackgroundImage( g_workspace.getImageFactory()->createImage(filename) );
		
		m_plbTrumpLev->SetText(rtFormatNumber(trump.params[TRUMP_LEV]));
		m_plbTrumpFireAtt->SetText(rtFormatNumber(trump.params[TRUMP_FIRE_ATT]));
		m_plbTrumpFireDef->SetText(rtFormatNumber(trump.params[TRUMP_FIRE_DEF]));
		m_plbTrumpWaterAtt->SetText(rtFormatNumber(trump.params[TRUMP_WATER_ATT]));
		m_plbTrumpWaterDef->SetText(rtFormatNumber(trump.params[TRUMP_WATER_DEF]));
		m_plbTrumpPoisonAtt->SetText(rtFormatNumber(trump.params[TRUMP_POISON_ATT]));
		m_plbTrumpPoisonDef->SetText(rtFormatNumber(trump.params[TRUMP_POISON_DEF]));

		int nMaxSoul = TRUMP_CentiSoul(trump,MP_TRUMP) * 100.f;
		temStr.Format("%d/%d",trump.params[TRUMP_MP],nMaxSoul);
		m_pproTrumpSoul->SetValue((float)trump.params[TRUMP_MP]/(float)nMaxSoul);
		m_plbTrumpSoulNum->SetText(temStr.c_str());
		m_pbtnToTrumpStart->Enable();

		temStr.Format("%d%%",trump.params[TRUMP_ATTACH_RATE]);
		m_plbTypeRate->SetText(temStr.c_str());

		m_pcomTrumpType->SetSelectedItem(0);

		for (int i=TRUMP_SKILL_1,j= 0;i<TRUMP_SKILL_MAX;i++,j++)
		{
			if(trump.params[i] != 0)
			{
				pSkill = Skill()->FindSkill(trump.params[i]);
				if (pSkill)
				{
					if(pSkill->iLevel < pSkill->iMaxLevel)
					{
						pSkillnext = Skill()->FindNextLevelSkill(pSkill->wID);
						bCanNotStudy = g_layerMain->m_fromSkill->CanStudySkill(pSkillnext,pActorSkill);
					}else
						bCanNotStudy = true;
					if(bCanNotStudy)
						m_pbtnTrumpSkillup[j]->Disable();
					else
						m_pbtnTrumpSkillup[j]->Enable();
					tmp = pSkill->szIcon;
					m_pbtnTrumpSkill[j]->SetParam1(&(pSkill->wID));
					m_pbtnTrumpSkillup[j]->SetParam1(&(pSkill->wID));
					if(g_layerMain->m_fromSkill->CreateIconForGameClient(tmp.c_str(),&pImage))
						m_pbtnTrumpSkill[j]->SetBackgroundImage(pImage);
				}
			}
		}
	}else
	{
		m_pbtnTrump->SetBackgroundImage((RtwImage*)NULL);
		m_plbTrumpLev->SetText("");
	}

	temStr.Format("%d/%d",GetPlayer()->m_core.Soul, GetPlayer()->m_core.GetSoulValueMax());
	m_pproActorSoul->SetValue((float)GetPlayer()->m_core.Soul / (float)GetPlayer()->m_core.GetSoulValueMax());
	m_plbActorSoulNum->SetText(temStr.c_str());

	unguard;
}

void CUIForm_Char::OnShowXiuXing()
{
	guard;
	SCreature *cre = &GetWorld()->m_pPlayer->m_core;
	RtString tmp;
	tmp.Format("%d",cre->GradeLev);
	m_plbXiuWeiLev->SetText((char*)tmp);
	if(cre->GetGradeStage()>= 1 && cre->GetGradeStage() <= 8)
		m_plbJieDuan->SetText(strXiuWuJieDuan[cre->GetGradeStage()]);
	tmp.Format("%d/%d",cre->CulExp, g_TableExp.GetNextLevCulExp(cre->GradeLev-1));
	m_plbXiuWeiPoint->SetText((char*)tmp);
	m_pproXiuWei->SetValue((float)cre->CulExp/g_TableExp.GetNextLevCulExp(cre->GradeLev-1));

	tmp.Format("%d/%d",GetPlayer()->m_core.Soul, GetPlayer()->m_core.GetSoulValueMax());
	m_pproActorSoul_XiuWei->SetValue((float)GetPlayer()->m_core.Soul / (float)GetPlayer()->m_core.GetSoulValueMax());
	m_plbActorSoulNum_xiuWei->SetText(tmp.c_str());

	tmp.Format("%ld/%d",cre->Contribute,maxshengwang);
	m_plbSuming->SetText((char*)tmp);
	m_ppgbSuming->SetValue((float)cre->Contribute/maxshengwang);

	tmp.Format("%ld/%d",cre->JobRecord,maxshengwang);
	m_plbMenGong->SetText((char*)tmp);
	m_ppgbMenGong->SetValue((float)cre->JobRecord/maxshengwang);

	tmp.Format("%ld/%d",cre->UnionRecord,maxshengwang);
	m_plbBangGong->SetText((char*)tmp);
	m_ppgbBangGong->SetValue((float)cre->UnionRecord/maxshengwang);

	tmp.Format("%ld/%d",cre->Record,maxshengwang);
	m_plbJunGong->SetText((char*)tmp);
	m_ppgbJunGong->SetValue((float)cre->Record/maxshengwang);

	tmp.Format("%ld/%d",cre->Point,maxshengwang);
	m_plbJiFen->SetText((char*)tmp);
	m_ppgbJiFen->SetValue((float)cre->Point/maxshengwang);

	tmp.Format("%ld/%d",(int)(cre->Pk*0.001),maxshengwang);
	m_plbMorality->SetText((char*)tmp);
	m_ppgbMorality->SetValue((float)cre->Pk*0.001/maxshengwang);
	unguard;
}

void CUIForm_Char::OnShowShenWang()
{
	guard;
	SCreature *cre = &GetWorld()->m_pPlayer->m_core;
	RtString tmp;
	tmp.Format("%ld/%d",cre->Credit,maxshengwang);
	m_plbXianYu->SetText((char*)tmp);
	m_ppgbXianYu->SetValue((float)cre->Credit/maxshengwang);

	DWORD needEXP;
	needEXP = g_TablePopularity.GetNextGuanfuExp(cre->GuanfuLev);
	m_plbGuanfu->SetText(m_strShengWang[cre->GuanfuLev]);
	tmp.Format("%d/%u", cre->Guanfu,needEXP);
	m_plbGuanfuCount->SetText((char*)tmp);
	m_ppgbGuanfu->SetValue((float)cre->Guanfu/needEXP);

	needEXP = g_TablePopularity.GetNextGuanfuExp(cre->ManzuLev);
	m_plbManzu->SetText(m_strShengWang[cre->ManzuLev]);
	tmp.Format("%d/%u", cre->Manzu,needEXP);
	m_plbManzuCount->SetText((char*)tmp);
	m_ppgbManzu->SetValue((float)cre->Manzu/needEXP);

	needEXP = g_TablePopularity.GetNextGuanfuExp(cre->YishiLev);
	m_plbYishi->SetText(m_strShengWang[cre->YishiLev]);
	tmp.Format("%d/%u", cre->Yishi,needEXP);
	m_plbYishiCount->SetText((char*)tmp);
	m_ppgbYishi->SetValue((float)cre->Yishi/needEXP);

	needEXP = g_TablePopularity.GetNextGuanfuExp(cre->YijunLev);
	m_plbYijun->SetText(m_strShengWang[cre->YijunLev]);
	tmp.Format("%d/%u", cre->Yijun,needEXP);
	m_plbYijunCount->SetText((char*)tmp);
	m_ppgbYijun->SetValue((float)cre->Yijun/needEXP);

	needEXP = g_TablePopularity.GetNextGuanfuExp(cre->LianmengLev);
	m_plbLianMeng->SetText(m_strShengWang[cre->LianmengLev]);
	tmp.Format("%d/%u", cre->Lianmeng,needEXP);
	m_plbLianMengCount->SetText((char*)tmp);
	m_ppgbLianMeng->SetValue((float)cre->Lianmeng/needEXP);
	unguard;
}

void CUIForm_Char::Hide()
{
	guard;
	m_eNowShow = NOSELECT;
	m_pFromThis->Hide();

	if (m_bIsRifine)
	{
		m_bIsRifine = false;
		GetPlayer()->SendTrumpRifine(0,true);
	}

	m_p3DPlayerView->ResetView();
	unguard;
}

// refresh是否刷新装备界面
void CUIForm_Char::Refresh(bool refresh)
{
	guard;
	if(m_pFromThis->IsVisible())
		Show(refresh);
	RefreshChar();
	unguard;
}

void CUIForm_Char::RefreshChar()
{
	guard;
	if(m_pfrmChar->IsVisible())
		OnShowAttribute();
	unguard;
}

bool CUIForm_Char::IsVisible()
{
	guard;
	return m_pFromThis->IsVisible();
	unguard;
}

void CUIForm_Char::OnBtnOK(RtwWidget* vpSender, RtwEventDelegate* e)
{
	guard;
	GetWorld()->HideGuideForm();

	if(m_istrtmp>0 || m_icontmp >0 || m_idextmp>0 || m_iinttmp>0 || m_ihittmp>0)
	{
		CG_CmdPacket *cmd = NetBeginWrite();
		cmd->WriteShort(c2r_add_attr_point);
		cmd->WriteLong((long)m_istrtmp);
		cmd->WriteLong((long)m_icontmp);
		cmd->WriteLong((long)m_idextmp);
		cmd->WriteLong((long)m_iinttmp);
		cmd->WriteLong((long)m_ihittmp);
		NetSend(cmd);

		m_istrtmp = 0;
		m_icontmp = 0;
		m_idextmp = 0;
		m_iinttmp = 0;
		m_ihittmp = 0;
	}
	unguard;
}

void CUIForm_Char::OnBtnCancel(RtwWidget* vpSender, RtwEventDelegate* e)
{
	guard;
	m_iAllpoint += m_istrtmp + m_icontmp + m_idextmp + m_iinttmp + m_ihittmp;
	m_istrtmp = 0;
	m_icontmp = 0;
	m_idextmp = 0;
	m_iinttmp = 0;
	m_ihittmp = 0;
	RefreshChar();
	unguard;
}

void CUIForm_Char::OnHideGuide(RtwWidget* vpSender, RtwEventDelegate* e)
{
	GetWorld()->HideGuideForm();
	m_pFromThis->Hide();
}
void CUIForm_Char::Add_SubButton(RtwWidget* vpSender, RtwEventDelegate* e)
{
	guard;
	if (g_layerMain->m_formGuide5->IsVisible())
	{
		g_layerMain->m_formGuide5->Hide();
	}
	RtwButton* tmpUse = (RtwButton*)vpSender;
	if(tmpUse == m_pbtnStrAdd)
	{
		if(m_iAllpoint > 0)
		{
			m_iAllpoint -= 1;
			m_istrtmp += 1;
		}

	}
	else if(tmpUse == m_pbtnConAdd)
	{
		if(m_iAllpoint > 0)
		{
			m_iAllpoint -= 1;
			m_icontmp += 1;
		}
	}
	else if(tmpUse == m_pbtnDexAdd)
	{
		if(m_iAllpoint > 0)
		{
			m_iAllpoint -= 1;
			m_idextmp += 1;
		}
	}
	else if(tmpUse == m_pbtnIntAdd)
	{
		if(m_iAllpoint > 0)
		{
			m_iAllpoint -= 1;
			m_iinttmp += 1;
		}
	}
	else if(tmpUse == m_pbtnHitAdd)
	{
		if(m_iAllpoint > 0)
		{
			m_iAllpoint -= 1;
			m_ihittmp += 1;
		}
	}
	else if(tmpUse == m_pbtnStrSub)
	{
		if(m_istrtmp > 0)
		{
			m_iAllpoint += 1;
			m_istrtmp -= 1;
		}
	}
	else if(tmpUse == m_pbtnConSub)
	{
		if(m_icontmp > 0)
		{
			m_iAllpoint += 1;
			m_icontmp -= 1;
		}
	}
	else if(tmpUse == m_pbtnDexSub)
	{
		if(m_idextmp > 0)
		{
			m_iAllpoint += 1;
			m_idextmp -= 1;
		}
	}
	else if(tmpUse == m_pbtnIntSub)
	{
		if(m_iinttmp > 0)
		{
			m_iAllpoint += 1;
			m_iinttmp -= 1;
		}
	}
	else if(tmpUse == m_pbtnHitSub)
	{
		if(m_ihittmp >0)
		{
			m_iAllpoint += 1;
			m_ihittmp -= 1;
		}
	}
	if (m_iAllpoint == 0)
	{
		//玩家属性点分配的UI显示
		GetWorld()->ShowAttrPointAllocate();
	}
	RefreshChar();
	unguard;
}

RtwPixel CUIForm_Char::GetColor(float rate)
{
	if(rate-1 > 0.01)
	{
		return RtwPixel(0xFFFFFF00);
	}
	else if (rate-1 < -0.01)
	{
		return RtwPixel(0xFFFF0000);
	}
	else
	{
		return RtwPixel(0xFFFFFFFF);
	}
}
void CUIForm_Char::SetCharButtonNormal()
{
	guard;
	if(m_bShowAnimation)
	{
		RTW_WIDGET_T(RtwButton,"fmsystem.btnsyschar")->SetNormalImage(m_imgCharNormal);
		m_bShowAnimation = false;
	}
	unguard;
}
void CUIForm_Char::SetCharButtonAnimation()
{
	guard;
	if(!m_bShowAnimation && !IsVisible())
	{
		RTW_WIDGET_T(RtwButton,"fmsystem.btnsyschar")->SetNormalImage(m_imgCharAnimation);
		m_bShowAnimation = true;
	}
	unguard;
}
RtwImage* CUIForm_Char::LoadSequenceImage(const string &strFileName)
{
	RtwImage *pImage = g_workspace.getImageFactory()->createImageSequence();

	RtwImage *pTempImage = NULL;


	for (int j=0; j<6; ++j)
	{
		for (int i=0; i<6; ++i)
		{
			pTempImage = g_workspace.getImageFactory()->createImage();
			if (pTempImage->LoadFromFile(strFileName))
			{
				pTempImage->SetSourceRect(RtwRect(i*36+2, j*36+1, (i+1)*36, (j+1)*36+1));
				pTempImage->SetBlend(true);

				((RtwComplexImage*)pImage)->AddImage(pTempImage, 100);
				DROP_TRUI_IMAGE(pTempImage);
			}
		}
	}
	return pImage;
}
void CUIForm_Char::OnRClick_Parts(RtwWidget* sender, void*)
{
	guard;
	if (!m_pPlayer)
		m_pPlayer = GetWorld()->m_pPlayer;

	if(g_layerMain->m_fromPlayerItems->OperateIsLocked())
		return;
	const string& id = sender->GetID();
	int i = 0;
	CItemContainerBase::EEquipParts part;
	Is_Widget_Parts(id, part);

	SItemID item = m_pPlayer->mItem.GetEquipItem(part, true);
	if (!ItemID_IsValid(item))
		return;

	if (m_pPlayer->mItem.m_Bag.IsFull())
	{
		PlayerFailOperate(R(MSG_ITEM_CONTAINER_NOSPACE));
		return;
	}
	if ( m_pPlayer->mItem.OperateItem_CheckUIContent(item, IO_UnEquip))
	{
		if (ItemIsTrump(item))
		{
			SSkillContinueMask scm;
			for (int i=TRUMP_SKILL_1;i<TRUMP_SKILL_MAX;i++)
			{
				CSkillInstance *pSkill = m_pPlayer->m_Skill.FindActiveSkill(item.params[i]);
				if (pSkill)
				{
					scm.wSkillClass = pSkill->m_pAttr->wClass;
					scm.resumeTime = pSkill->m_dwResumeTime;
					scm.useTime = pSkill->m_dwUserTime;
					m_pPlayer->m_Skill.m_listMask.push_back(scm);
				}
			}
			if (m_bIsRifine)
			{
				OnFinishRifineTrump(false);
				GetPlayer()->SendTrumpRifine(0,true);
			}
		}
		CItemNetHelper::NetSend_UnEquipItem(item.id, part);
		g_layerMain->m_fromPlayerItems->LockOperate(true);
	}
	unguard;
}

void CUIForm_Char::OnDrag_Parts(RtwWidget* sender, void*)
{
	guard;
	RtwWidget* pDragSvr = g_workspace.GetDragServer();
	if (!pDragSvr)		return;
	if ( !m_pPlayer)	return;

	if (!m_pPlayer)
		m_pPlayer = GetWorld()->m_pPlayer;

	if (g_layerMain->m_fromPlayerItems->OperateIsLocked())
		return;

	const string& WidgetID = pDragSvr->GetID();
	CItemContainerBase::EEquipParts part;
	Is_Widget_Parts(sender->GetID(), part);

	/// 查找DragServer是什么(支持CBag的格子，人物身上的装备，快捷栏)
	// 是不是包裹中的格子：
	int i,j;
	if ( g_layerMain->m_fromPlayerItems->Is_Widget_Common(WidgetID, i, j) )
	{
		SItemID item = m_pPlayer->mItem.m_Bag.GetItem(i, j);

		//add by yz: 2010-6-9
		if (!Is_Widget_Parts(item, part))
			return;
		//end yz
		
		if ( ItemIsWeapon(item) )
		{
			// 特殊情况要处理
			if (part==CItemContainerBase::WEAPON_1)
			{
			}
			else if (part==CItemContainerBase::WEAPON_2)
			{
				SItemID item1 = m_pPlayer->mItem.GetEquipItem(CItemContainerBase::WEAPON_1, true);
				if (ItemID_IsValid(item1))
				{
					SWeapon *pWeaClass1 = (SWeapon *)GetItemMgr()->GetItem(item1.type);
					if (pWeaClass1 && pWeaClass1->bBothHands)
						part = CItemContainerBase::WEAPON_1;
				}
				else 
				{
					part = CItemContainerBase::WEAPON_1;
				}
			}

			if ( !m_pPlayer->mItem.CheckEquip(part, item, m_pPlayer->m_core) )
			{
				PlayerFailOperate(R(MSG_ITEM_EQUIP_FAIL), true, "cannotpickup.wav");
				return;
			}
			if ( m_pPlayer->mItem.OperateItem_CheckUIContent(item, IO_Equip))
			{
				if (item.cBind == IB_BindWhenEquip)
				{
					SItemBase *pItemClass = GetWorld()->m_pItemManager->GetItem(item.type);
					if (!pItemClass)
						return;
					char cTmp128[128];
					rt2_sprintf(cTmp128, R(MSG_EQUIP_WILLBIND), pItemClass->name);
					UIFormMsg *pFrm = UIFormMsg::ShowStatic(cTmp128, UIFormMsg::TYPE_OK_CANCEL);
					pFrm->EvOK = RTW_CALLBACK_1(this, UIFormPlayerItems, OnEquipItem,pFrm);
				}
				else 
				{
					CItemNetHelper::NetSend_EquipItem(item.id, part, ITEM_CONTAINER_PLAYER_COMMON_BAG, m_pPlayer->mItem.m_Bag.GetPageIndex(), i, j);
					g_layerMain->m_fromPlayerItems->LockOperate(true);
				}
			}
		}
		else if ( ItemIsArmor(item) || ItemIsShipin(item) )
		{
			if ( !m_pPlayer->mItem.CheckEquip(part, item, m_pPlayer->m_core) )
			{
				PlayerFailOperate(R(MSG_ITEM_EQUIP_FAIL), true, "cannotpickup.wav");
				return;
			}
			if ( m_pPlayer->mItem.OperateItem_CheckUIContent(item, IO_Equip))
			{
				if (item.cBind == IB_BindWhenEquip)
				{
					SItemBase *pItemClass = GetWorld()->m_pItemManager->GetItem(item.type);
					if (!pItemClass)
						return;
					char cTmp128[128];
					rt2_sprintf(cTmp128, R(MSG_EQUIP_WILLBIND), pItemClass->name);
					UIFormMsg *pFrm = UIFormMsg::ShowStatic(cTmp128, UIFormMsg::TYPE_OK_CANCEL);
					pFrm->EvOK = RTW_CALLBACK_1(this, UIFormPlayerItems, OnEquipItem,pFrm);
				}
				else 
				{
					CItemNetHelper::NetSend_EquipItem(item.id, part, ITEM_CONTAINER_PLAYER_COMMON_BAG, m_pPlayer->mItem.m_Bag.GetPageIndex(), i, j);
					g_layerMain->m_fromPlayerItems->LockOperate(true);
				}
			}
		}
		else
			PlayerFailOperate(R(MSG_ITEM_ZHUANGBEI_NOTFIT));
	}
	unguard;
}

void CUIForm_Char::OnDragEnd_Parts(RtwWidget* sender, void*)
{
	guard;
	RtwWidget* pClient = g_workspace.GetDragClient();
	if (!m_pPlayer)
		m_pPlayer = GetWorld()->m_pPlayer;
	CBag *pBag = &(m_pPlayer->mItem.m_Bag);
	const string& widgetID = sender->GetID();
	CItemContainerBase::EEquipParts part;
	Is_Widget_Parts(widgetID, part);

	if ( !pClient )// 把图标拖到空白处
	{
		//if ( g_layerMain->m_formPShop->m_PShop.IsValid() )			return;
		if ( g_layerMain->m_formTrade->IsShowing() )				return;
		if ( g_layerMain->m_formShop->IsVisible() )					return;
	}
	else
	{
		const RtwRect& rcClient = pClient->GetFrameRect();
		const RtwRect& rcHotKey = RTW_WIDGET("fmhotkey")->GetFrameRect();
		if ( rcHotKey.IsContained(rcClient.left, rcClient.top) )
		{
			// 把图标拖到快捷栏中
			SItemID itemID = m_pPlayer->mItem.GetEquipItem(part, true);
			SItemBase* pItemBase = GetWorld()->m_pItemManager->GetItem(itemID.type);
			CHECK( pItemBase && ItemID_IsValid(itemID) );
			string strID = "QS_" + sender->GetID();

			RtwButton* pButton = NULL;
			pButton = (RtwButton*)(RTW_WIDGET("fmitem")->GetChildWidget(strID));
			if (pButton==NULL)
			{
				// 				pButton = (RtwButton*)sender->Clone(NULL, strID);
				// 				pButton->ModifyFlags(RtwButton::wfDefault | wfHint | wfDragClient | wfDragServer, 0);// change [3/17/2009 tooth.shi]
				pButton->CloneFrom(sender);
				pButton->ModifyFlags(wfHint,0);
				pButton->LoadAttributesFromTemplate("IconBorder");

				DWORD *pDword = RT_NEW DWORD[3];
				pDword[0] = UIHotKey_Item;
				pDword[1] = itemID.id;
				pDword[2] = itemID.type;
				pButton->SetUserData(pDword);
				pButton->EvClose	+= RTW_CALLBACK(this, UIFormPlayerItems, DeleteQuickStartUserData);

				pButton->EvLClick	+= RTW_CALLBACK(this, UIFormPlayerItems, OnLClick_QuickStart);
				pButton->EvHint		+= RTW_CALLBACK(this, UIFormPlayerItems, OnGetHintText_QuickStart);
			}
			g_workspace.SetDragServer(pButton);
		}
	}
	unguard;
}

void CUIForm_Char::OnLClick_Parts(RtwWidget *sender, void*)
{
	guard;
	if (!m_pPlayer)
		m_pPlayer = GetWorld()->m_pPlayer;

	if (g_layerMain->m_fromPlayerItems->OperateIsLocked())
		return;

	const string& id = sender->GetID();
	int i = 0;
	CItemContainerBase::EEquipParts part;
	Is_Widget_Parts(id, part);

	SItemID item = m_pPlayer->mItem.GetEquipItem(part, true);
	SItemBase* pItemClass = GetWorld()->m_pItemManager->GetItem(item.type);

	if (!ItemID_IsValid(item))
		return;
	if(g_layerMain->GetFirstWidget()== g_layerMain->m_formShop->OnGetThisForm() && g_layerMain->m_formShop->m_bIsRepair)
	{
		g_layerMain->m_formShop->OnLClickRepair(item);
	}
	if (m_pPlayer->mItem.m_Bag.IsFull())
	{
		PlayerFailOperate(R(MSG_ITEM_CONTAINER_NOSPACE));
		return;
	}

	//如果是按住ctrl并单击,则是超链接
	bool bKeyShift = (GetKeyState(VK_SHIFT)&0xFF00)!=0;
	if (bKeyShift)
	{
		const RtwPixel GREENCOLOR	= 0xff30e512;
		const RtwPixel BLUECOLOR	= 0xff00aaff;
		const RtwPixel GOLDENCOLOR	= 0xffffd851;
		const RtwPixel WHITECOLOR	= 0xffffffff;
		RtwPixel color = WHITECOLOR;

		if ( ItemIsGreen(item) )			color = GREENCOLOR;
		else if ( ItemIsBlue(item) )		color = BLUECOLOR;
		else if ( ItemIsGolden(item) )	color = GOLDENCOLOR;
		else								color = WHITECOLOR;

		ItemHyperLinkSend(1, item.id, GetWorld()->m_pPlayer->ID(), pItemClass->name, color);
		return;
	}
	unguard;
}

void CUIForm_Char::OnGetHintText_Parts(RtwWidget* sender, void*)
{
	guard;
	if (!m_pPlayer)
		m_pPlayer = GetWorld()->m_pPlayer;

	const string& widgetID = sender->GetID();
	CItemContainerBase::EEquipParts part;
	Is_Widget_Parts(widgetID, part);
	SItemID itemID = m_pPlayer->mItem.GetEquipItem(part, true);
	if ( !ItemID_IsValid(itemID) )
	{
		sender->ModifyFlags(0, wfHint);
		return;
	}

	int nLines;
	RtString str = GenerateItemHtmlHint(nLines, itemID, m_pPlayer);

	g_workspace.SetHintText((const char*)str, true);
	//	g_workspace.SetHintSize(ITEM_HINT_WIDTH, nLines*ITEM_HINT_LINE_HEIGHT + ITEM_HINT_BLANK_HEIGHT);
	g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
	g_workspace.AutoHintSize();
	g_workspace.AutoHintPosition();
	unguard;
}

// 是不是人物的纸娃娃
bool CUIForm_Char::Is_Widget_Parts(const string& id, CItemContainerBase::EEquipParts& outPart)
{
	guard;

	if (!m_pPlayer)
		m_pPlayer = GetWorld()->m_pPlayer;
	int ii;
	for (ii=0; ii<CItemContainerBase::MAX_EQUIP_PARTS; ii++)
	{
		if (ii == CItemContainerBase::WEAPON_2)
		{
			continue;
		}
		if ( id==m_btnParts[ii]->GetID() )
			break;
	}
	if (ii<CItemContainerBase::MAX_EQUIP_PARTS)
	{
		outPart = (CItemContainerBase::EEquipParts)ii;
		return true;
	}
	else
		return false;

	unguard;
}

//add by yz: 2010-6-9
bool CUIForm_Char::Is_Widget_Parts(SItemID& rItemID, CItemContainerBase::EEquipParts inPart)
{
	switch(inPart)
	{
	case CItemContainerBase::WEAPON_1:
	case CItemContainerBase::WEAPON_2:
	case CItemContainerBase::WEAPON_3:
		{
			if (ItemIsWeapon(rItemID))
				return true;
		}		
		break;
	case CItemContainerBase::NECKLACE:
		{
			if (ItemIsNecklace(rItemID))
				return true;
		}
		break;

	case CItemContainerBase::GLOVE:
		{
			if (ItemIsGlove(rItemID))
				return true;
		}
		break;
	case CItemContainerBase::RING:
	case CItemContainerBase::RING2:
		{
			if (ItemIsRing(rItemID))
				return true;
		}
		break;
	case CItemContainerBase::BODY:
		{
			if (ItemIsCloth(rItemID))
				return true;
		}
		break;
	case CItemContainerBase::FOOT:
		{
			if (ItemIsShoes(rItemID))
				return true;
		}
		break;
	case CItemContainerBase::CAESTUS:
		{
			if (ItemIsCaestus(rItemID))
				return true;
		}
		break;
	case CItemContainerBase::KITS:
		{
			if (ItemIsKits(rItemID))
				return true;
		}
		break;
	case CItemContainerBase::CHARM:
		{
			return true;
		}
		break;
	case CItemContainerBase::TRUMP:
		{
			if (ItemIsTrump(rItemID))
				return true;
		}
		break;
	case CItemContainerBase::FASHION:
		{
			if (ItemIsFashion(rItemID))
				return true;
		}
		break;
	case CItemContainerBase::SEALEDBOOK:
		{
			if (ItemIsBook(rItemID))
				return true;
		}
		break;
	case CItemContainerBase::AEROCRAFT:
		{
			if (ItemIsFly(rItemID))
				return true;
		}
		break;
	}

	return false;
}
//end yz

void CUIForm_Char::ItemHyperLinkSend(DWORD itemTag, DWORD itemId, DWORD playerID, const std::string &strName, RtwPixel color)
{
	guard;
	GetWorld()->m_Chat.GetEditChat().AddHyperLinkText(hlinkItem, itemTag, itemId, playerID, strName, color);
	unguard;
}

void CUIForm_Char::OnLClick_BtnAttribute(void*,void*)
{
	guard;
	if(m_pfrmChar->IsVisible())
	{
		m_bShowChar = false;
		m_pfrmChar->Hide();
	}
	else
		m_bShowChar = true;
	unguard;
}

void CUIForm_Char::OnGetHintTrumpSkill(RtwWidget* sender, void*)
{
	guard;
	int iLine=0;
	GcActor* pActor = GetPlayer();
	if (pActor==NULL) return;

	unsigned short *skillID = (unsigned short*)sender->getParam1();
	if (!skillID) return;

	SSkill* pSkill = Skill()->FindSkill(*skillID);
	if(!pSkill)
		return;
	const char* pTxt = GenerateSkillHtmlHint(pSkill, pSkill->wID, true, true, iLine, NULL);
	if (iLine)
	{
		g_workspace.SetHintSize(180, 100);
		g_workspace.SetHintText(pTxt, true);
		g_workspace.AutoHintSize();
		g_workspace.AutoHintPosition();
	}else
	{
		g_workspace.SetHintText("");
	}
	unguard;
}

void CUIForm_Char::OnGetHintTrumpItem(RtwWidget* sender, void*)
{
	guard;
	OnGetHintText_Parts(m_btnParts[CItemContainerBase::TRUMP],NULL);
	unguard;
}

void CUIForm_Char::OnLClick_BtnTrumpSkillUp(RtwWidget *sender, void*)
{
	guard;
	for(int i= 0;i< MAXTRUMPSKILLNUM;i++)
	{
		if(m_pbtnTrumpSkillup[i]== sender)
		{
			GcActor* pActor = GetPlayer();
			if (pActor==NULL) return;
			unsigned short wSkillID = (unsigned short)m_pbtnTrumpSkill[i]->GetUserData();
			SSkill* pSkill = Skill()->FindSkill(wSkillID);
			if (pSkill->wClass== 13)
			{
				if (!pActor->m_Skill.FindAllSkill(wSkillID))
				{
					if (pActor->m_Skill.StudySkill(wSkillID, false))
					{
						Refresh();
						return;
					}
					PlayerFailOperate(pActor->m_Skill.GetLastErrInfo());
					return;
				}else
				{
					// 查找下一级的技能
					pSkill = Skill()->FindNextLevelSkill(wSkillID);
					if (pSkill)
					{
						if (pActor->m_Skill.StudySkill(pSkill->wID, false))
						{
							Refresh();
							return;
						}
						PlayerFailOperate(pActor->m_Skill.GetLastErrInfo());
						return;
					}
				}
			}
			// 失败
			PlayerFailOperate(NULL);
		}
	}
	unguard;
}

void CUIForm_Char::OnDragEnd_TrumpSkill(RtwButton *sender, void*)
{
	guard;
	if ( g_workspace.GetDragClient() )
	{
		char strId[32];
		RtwButton* pButton = NULL;
		GcActor* pActor = GetPlayer();
		if (pActor==NULL) return;
		unsigned short *skillID = (unsigned short*)sender->getParam1();
		GcSkill* pSkill = (GcSkill*)pActor->m_Skill.FindActiveSkill(*skillID);
		RtwRect rcClient;
		RtwRect rcHotKey,rcHotKey1,rcHotKey2;
		RtwRect rcSmallHotKey;
		bool	bIsInHotKey= false;
		int		iInHotKey = -1;
		rcHotKey =	RTW_WIDGET("fmhotkey")->GetFrameRect();
		rcHotKey1 =	RTW_WIDGET("fmshortcut_bar1")->GetFrameRect();
		rcHotKey2 =	RTW_WIDGET("fmshortcut_bar2")->GetFrameRect();
		if(rcHotKey.IsContained(g_workspace.GetMousePosX(), g_workspace.GetMousePosY())
			|| rcHotKey1.IsContained(g_workspace.GetMousePosX(), g_workspace.GetMousePosY())
			|| rcHotKey2.IsContained(g_workspace.GetMousePosX(), g_workspace.GetMousePosY()))
		{
			for(int i= 0;i< UI_HOT_KEY_COUNT;++i)
			{
				rcSmallHotKey = g_layerMain->m_hotKeyAlias[i]->GetFrameRect();
				if(rcSmallHotKey.IsContained(g_workspace.GetMousePosX(), g_workspace.GetMousePosY()))
				{
					bIsInHotKey = true;
					iInHotKey = i;
					break;
				}
			}
		}
		if(!bIsInHotKey)
		{
			g_workspace.SetDragClient(NULL);
			return;
		}
		if (pSkill)
		{
			// 先进行遍历一下快捷键，是否已经存在此快捷键，如果有，删除原来的 // gao 2010.3.30
			for(int i= 0;i< UI_HOT_KEY_COUNT;++i)
			{
				if(!g_layerMain->m_hotKeyAlias[i]->GetButton())
					continue;
				DWORD *pDWord = (DWORD*)g_layerMain->m_hotKeyAlias[i]->GetButton()->GetUserData();
				if(pDWord[1] == pSkill->m_pAttr->wID)
				{
					if(iInHotKey == i)
						return;
					else
						g_layerMain->m_hotKeyAlias[i]->SetButton(NULL);
				}
			}
			rt2_sprintf(strId, "btnMWskillicon_%03d", pSkill->m_pAttr->wID);
			pButton = (RtwButton*)(m_pFromThis->GetChildWidget(strId));
			if (pButton==NULL)
			{
				// change [3/20/2009 tooth.shi] 				pButton = (RtwButton*)sender->Clone(NULL, strId);
				// 				pButton->ModifyFlags(RtwButton::wfDefault | wfHint | wfDragClient, 0);
				pButton = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton, strId);
				pButton->CloneFrom(sender);
				pButton->ModifyFlags(wfDragOut|wfDragIn|wfHint|wfDClick|wfGrabMouse, 0);
				DWORD *pDWord = RT_NEW DWORD[2];
				pDWord[0] = UIHotKey_Skill;
				pDWord[1] = pSkill->m_pAttr->wID;
				pButton->SetBorderColor(0xee000000);
				pButton->SetBorderSize(0);
				pButton->SetUserData(pDWord);
				//                pButton->SetUserData((void*)pSkill->m_pAttr->wID);

				pButton->EvClose       += RTW_CALLBACK(this, UIFormSkill, OnSkillItemButtonClose);
				//pButton->EvLClick      += RTW_CALLBACK(this, UIFormSkill, OnLClickCommonButton);
				pButton->EvRClick	   += RTW_CALLBACK(this, UIFormSkill, OnLClickCommonButton);
				pButton->EvHint		   += RTW_CALLBACK(this, UIFormSkill, OnGetHintTextCommonButton);

				pSkill->SetUIButton(pButton);
			}
			g_workspace.SetDragServer(pButton);
			if(g_workspace.getMouseHover())
			{
				if(g_workspace.getMouseHover()->getWidgetType() == wtAliasButton)
					return;
				//此处 由于当设置一次按钮之后，此处按钮会将原快捷键盖住，特添加此处理 gao 2010.3.30
				rcClient =	g_workspace.getMouseHover()->GetFrameRect();
				if(rcHotKey.IsContained(rcClient.left, rcClient.top)
					|| rcHotKey1.IsContained(rcClient.left, rcClient.top)
					|| rcHotKey2.IsContained(rcClient.left, rcClient.top))
				{
					for(int i= 0;i< UI_HOT_KEY_COUNT;++i)
					{
						rcSmallHotKey = g_layerMain->m_hotKeyAlias[i]->GetFrameRect();
						if(rcSmallHotKey.IsContained(rcClient.left, rcClient.top))
						{
							if(g_layerMain->m_hotKeyAlias[i]->GetButton())
							{
								g_layerMain->m_hotKeyAlias[i]->SetButton(pButton);
								break;
							}
						}
					}
				}
			}
			g_workspace.SetDragClient(NULL);
		}else
		{
			CHECKEX("UIFormSkill: SetUserData() error");
		}
		return;
	}
	unguard;
}

void CUIForm_Char::OnDrag_TrumpSkill(RtwButton *sender, void*)
{
	guard;
	unguard;
}