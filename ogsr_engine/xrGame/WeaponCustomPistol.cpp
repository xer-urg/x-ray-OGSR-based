#include "stdafx.h"

#include "Entity.h"
#include "WeaponCustomPistol.h"
#include "game_object_space.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponCustomPistol::CWeaponCustomPistol(LPCSTR name) : CWeaponMagazined(name,SOUND_TYPE_WEAPON_PISTOL){
}

CWeaponCustomPistol::~CWeaponCustomPistol(){
}
void CWeaponCustomPistol::switch2_Fire()
{
	if (GetCurrentFireMode() == 1) 
	{
		m_bFireSingleShot = true;
		bWorking = true;
		m_iShotNum = 0;
		m_bStopedAfterQueueFired = false;

		StateSwitchCallback(GameObject::eOnActorWeaponStartFiring, GameObject::eOnNPCWeaponStartFiring);
	}
	else 
	{
		inherited::switch2_Fire();
	}
}

void CWeaponCustomPistol::FireEnd() 
{
	if (fTime <= 0 && GetCurrentFireMode() == 1)
	{
		SetPending(FALSE);
	}

	inherited::FireEnd();
}