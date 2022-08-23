////////////////////////////////////////////////////////////////////////////
//	Module 		: eatable_item.cpp
//	Created 	: 24.03.2003
//  Modified 	: 29.01.2004
//	Author		: Yuri Dobronravin
//	Description : Eatable item
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "eatable_item.h"
#include "physic_item.h"
#include "Level.h"
#include "entity_alive.h"
#include "EntityCondition.h"
#include "InventoryOwner.h"

#include "xrServer_Objects_ALife_Items.h"

CEatableItem::CEatableItem()
{
	m_iPortionsNum = -1;

	m_physic_item	= 0;

	m_ItemInfluence.clear();
	m_ItemInfluence.resize(eInfluenceMax);
}

CEatableItem::~CEatableItem()
{
}

DLL_Pure *CEatableItem::_construct	()
{
	m_physic_item	= smart_cast<CPhysicItem*>(this);
	return			(inherited::_construct());
}

void CEatableItem::Load(LPCSTR section)
{
	inherited::Load(section);

	m_ItemInfluence[eHealthInfluence]		= READ_IF_EXISTS	(pSettings,r_float,section,"eat_health",		0.0f);//pSettings->r_float(section, "eat_health");
	m_ItemInfluence[ePowerInfluence]		= READ_IF_EXISTS	(pSettings,r_float,section,"eat_power",			0.0f);//pSettings->r_float(section, "eat_power");
	m_ItemInfluence[eMaxPowerInfluence]		= READ_IF_EXISTS	(pSettings,r_float,section,"eat_max_power",		0.0f);
	m_ItemInfluence[eSatietyInfluence]		= READ_IF_EXISTS	(pSettings,r_float,section,"eat_satiety",		0.0f);//pSettings->r_float(section, "eat_satiety");
	m_ItemInfluence[eRadiationInfluence]	= READ_IF_EXISTS	(pSettings,r_float,section,"eat_radiation",		0.0f);//pSettings->r_float(section, "eat_radiation");
	m_ItemInfluence[ePsyHealthInfluence]	= READ_IF_EXISTS	(pSettings,r_float,section,"eat_psyhealth",		0.0f);
	m_ItemInfluence[eAlcoholInfluence]		= READ_IF_EXISTS	(pSettings,r_float,section,"eat_alcohol",		0.0f);
	m_ItemInfluence[eThirstInfluence]		= READ_IF_EXISTS	(pSettings,r_float,section,"eat_thirst",		0.0f);
	m_ItemInfluence[eWoundsHealPerc]		= READ_IF_EXISTS	(pSettings,r_float,section,"wounds_heal_perc",	0.0f);//pSettings->r_float(section, "wounds_heal_perc");
	clamp						(m_ItemInfluence[eWoundsHealPerc], 0.f, 1.f);
	
	m_iStartPortionsNum			= READ_IF_EXISTS	(pSettings, r_s32, section, "eat_portions_num", 1);
	VERIFY						(m_iPortionsNum < 10000);

	m_bUsePortionVolume			= !!READ_IF_EXISTS(pSettings, r_bool, section, "use_portion_volume", false);

	m_fSelfRadiationInfluence	= READ_IF_EXISTS(pSettings, r_float, section, "eat_radiation_self", 0.1f);

	m_sUseMenuTip				= READ_IF_EXISTS(pSettings, r_string, section, "menu_use_tip", "st_use");
}

BOOL CEatableItem::net_Spawn				(CSE_Abstract* DC)
{
	if (!inherited::net_Spawn(DC)) return FALSE;

	if (auto se_eat = smart_cast<CSE_ALifeItemEatable*>(DC))
	{
		m_iPortionsNum = se_eat->m_portions_num;
		if (m_iPortionsNum > 0) {
			float   w = GetOnePortionWeight();
			float   v = GetOnePortionVolume();
			float   weight = w * m_iPortionsNum;
			float   volume = v * m_iPortionsNum;
			u32     c = GetOnePortionCost();
			u32     cost = c * m_iPortionsNum;
			SetWeight(weight);
			SetVolume(volume);
			SetCost(cost);
		}
	}
	else
		m_iPortionsNum = m_iStartPortionsNum;

	return TRUE;
};

void CEatableItem::net_Export(CSE_Abstract* E) {
	inherited::net_Export(E);
	auto se_eat = smart_cast<CSE_ALifeItemEatable*>(E);
	se_eat->m_portions_num = m_iPortionsNum;
};

bool CEatableItem::Useful() const
{
	if(!inherited::Useful()) return false;

	//проверить не все ли еще съедено
	if(Empty()) return false;

	return true;
}

void CEatableItem::OnH_B_Independent(bool just_before_destroy)
{
	if(!Useful()) 
	{
		object().setVisible(FALSE);
		object().setEnabled(FALSE);
		if (m_physic_item)
			m_physic_item->m_ready_to_destroy	= true;
	}
	inherited::OnH_B_Independent(just_before_destroy);
}

void CEatableItem::UseBy (CEntityAlive* entity_alive)
{
	CInventoryOwner* IO	= smart_cast<CInventoryOwner*>(entity_alive);
	R_ASSERT		(IO);
	//R_ASSERT		(m_pCurrentInventory==IO->m_inventory);
	//R_ASSERT		(object().H_Parent()->ID()==entity_alive->ID());

	for (int i = 0; i < eInfluenceMax; ++i) {
		ApplyInfluence(ItemInfluence(i), entity_alive, GetItemInfluence(ItemInfluence(i)));
	}
	
	//уменьшить количество порций
	if(m_iPortionsNum > 0)
		--(m_iPortionsNum);
	else
		m_iPortionsNum = 0;

	// Real Wolf: Уменьшаем вес и цену после использования.
	float   w = GetOnePortionWeight();
	float   weight = m_weight - w;

	float   v = GetOnePortionVolume();
	float   volume = m_volume - v;

	u32     c = GetOnePortionCost();
	u32     cost = m_cost - c;

	SetWeight	(weight);
	SetVolume	(volume);
	SetCost		(cost);
}
void CEatableItem::ZeroAllEffects()
{
	for (int i = 0; i < eInfluenceMax; ++i) {
		//nullifying values
		m_ItemInfluence[i] = 0.f;
	}
}

float CEatableItem::GetOnePortionWeight()
{
	float   rest = 0.0f;
	LPCSTR  sect = object().cNameSect().c_str();
	float   weight = READ_IF_EXISTS(pSettings, r_float, sect, "inv_weight", 0.100f);
	s32     portions = pSettings->r_s32(sect, "eat_portions_num");

	if (portions > 0) {
		rest = weight / portions;
	}
	else {
		rest = weight;
	}
	return rest;
}

float CEatableItem::GetOnePortionVolume()
{
	float   rest = 0.0f;

	if (!m_bUsePortionVolume)
		return rest;

	LPCSTR  sect = object().cNameSect().c_str();
	float   volume = READ_IF_EXISTS(pSettings, r_float, sect, "inv_volume", 0.f);
	s32     portions = pSettings->r_s32(sect, "eat_portions_num");

	if (portions > 0) {
		rest = volume / portions;
	}
	else {
		rest = volume;
	}
	return rest;
}

u32 CEatableItem::GetOnePortionCost()
{
	u32     rest = 0;
	LPCSTR  sect = object().cNameSect().c_str();
	u32     cost = READ_IF_EXISTS(pSettings, r_u32, sect, "cost", 1);
	s32     portions = pSettings->r_s32(sect, "eat_portions_num");

	if (portions > 0) {
		rest = cost / portions;
	}
	else {
		rest = cost;
	}

	return rest;
}

float CEatableItem::GetItemInfluence(ItemInfluence influence) {
	if (influence == eRadiationInfluence) {
		return (m_ItemInfluence[influence] + GetItemEffect(eRadiationRestoreSpeed) * m_fSelfRadiationInfluence) * GetCondition();
	}
	return m_ItemInfluence[influence] * GetCondition();
}

void CEatableItem::ApplyInfluence(ItemInfluence influence_num, CEntityAlive* entity_alive, float value) {
	if (!entity_alive || fis_zero(value)) return;
	auto econd = &entity_alive->conditions();
	switch (influence_num)
	{
	case eHealthInfluence: {
		econd->ChangeHealth(value);
	}break;
	case ePowerInfluence: {
		econd->ChangePower(value);
	}break;
	case eMaxPowerInfluence: {
		econd->ChangeMaxPower(value);
	}break;
	case eSatietyInfluence: {
		econd->ChangeSatiety(value);
	}break;
	case eRadiationInfluence: {
		econd->ChangeRadiation(value);
	}break;
	case ePsyHealthInfluence: {
		econd->ChangePsyHealth(value);
	}break;
	case eAlcoholInfluence: {
		econd->ChangeAlcohol(value);
	}break;
	case eThirstInfluence: {
		econd->ChangeThirst(value);
	}break;
	case eWoundsHealPerc: {
		econd->ChangeBleeding(value);
	}break;
	default:
		Msg("%s unknown influence num [%d]", __FUNCTION__, influence_num);
		break;
	}
}