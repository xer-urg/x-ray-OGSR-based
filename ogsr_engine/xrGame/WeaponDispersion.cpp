// WeaponDispersion.cpp: разбос при стрельбе
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "weapon.h"
#include "inventoryowner.h"
#include "actor.h"
#include "inventory_item_impl.h"

#include "actoreffector.h"
#include "effectorshot.h"
#include "EffectorShotX.h"


//возвращает 1, если оружие в отличном состоянии и >1 если повреждено
float CWeapon::GetConditionDispersionFactor() const
{
	if ( Core.Features.test( xrCore::Feature::npc_simplified_shooting ) ) {
	  const CActor* actor = smart_cast<const CActor*>( H_Parent() );
	  if ( !actor ) return 1.f;
	}
	return (1.f + fireDispersionConditionFactor*(1.f-GetCondition()));
}

float CWeapon::GetFireDispersion	(bool with_cartridge, bool with_owner)
{
	if (!with_cartridge || m_magazine.empty()) 
		return GetFireDispersion(0.f/*1.0f*/, with_owner);
	m_fCurrentCartirdgeDisp = m_magazine.back().m_kDisp;
	return GetFireDispersion	(m_fCurrentCartirdgeDisp, with_owner);
}

//текущая дисперсия (в радианах) оружия с учетом используемого патрона
float CWeapon::GetFireDispersion	(float cartridge_k, bool with_owner)
{
	//учет базовой дисперсии, состояние оружия и влияение патрона
	float fire_disp = fireDispersionBase + (fireDispersionBase * cartridge_k);//*cartridge_k*GetConditionDispersionFactor();
	fire_disp *= GetConditionDispersionFactor();
	//вычислить дисперсию, вносимую самим стрелком
	if(with_owner)
		if (auto pOwner = smart_cast<const CInventoryOwner*>(H_Parent())){
			const float parent_disp = pOwner->GetWeaponAccuracy();
			fire_disp += parent_disp;
		}
	return fire_disp;
}


//////////////////////////////////////////////////////////////////////////
// Для эффекта отдачи оружия
void CWeapon::AddShotEffector		()
{
	inventory_owner().on_weapon_shot_start	(this);
/**
	CActor* pActor = smart_cast<CActor*>(H_Parent());
	if(pActor){
		CCameraShotEffector* S	= smart_cast<CCameraShotEffector*>	(pActor->EffectorManager().GetEffector(eCEShot)); 
		if (!S)	S				= (CCameraShotEffector*)pActor->EffectorManager().AddEffector(xr_new<CCameraShotEffectorX> (camMaxAngle,camRelaxSpeed, camMaxAngleHorz, camStepAngleHorz, camDispertionFrac));
		R_ASSERT				(S);
		S->SetRndSeed(pActor->GetShotRndSeed());
		S->SetActor(pActor);
		S->Shot					(camDispersion+camDispersionInc*float(ShotsFired()));
	}
/**/
}

void  CWeapon::RemoveShotEffector	()
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(H_Parent());
	if (pInventoryOwner)
		pInventoryOwner->on_weapon_shot_stop	(this);
}

void	CWeapon::ClearShotEffector	()
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(H_Parent());
	if (pInventoryOwner)
		pInventoryOwner->on_weapon_hide	(this);

};

/**
const Fvector& CWeapon::GetRecoilDeltaAngle()
{
	CActor* pActor		= smart_cast<CActor*>(H_Parent());

	CCameraShotEffector* S = NULL;
	if(pActor)
		S = smart_cast<CCameraShotEffector*>(pActor->EffectorManager().GetEffector(eCEShot)); 

	if(S)
	{
		S->GetDeltaAngle(m_vRecoilDeltaAngle);
		return m_vRecoilDeltaAngle;
	}
	else
	{
		m_vRecoilDeltaAngle.set(0.f,0.f,0.f);
		return m_vRecoilDeltaAngle;
	}
}
/**/