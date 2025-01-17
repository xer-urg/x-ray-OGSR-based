#pragma once

class CWound;
class NET_Packet;
class CEntityAlive;
class CLevel;

#include "hit_immunity.h"
#include "Hit.h"
#include "Level.h"

enum eInfluenceParams {
	//instant
	eHealthInfluence,
	ePowerInfluence,
	eMaxPowerInfluence,
	eSatietyInfluence,
	eRadiationInfluence,
	ePsyHealthInfluence,
	eAlcoholInfluence,
	eWoundsHealInfluence,

	eInfluenceMax,
};

enum eBoostParams {
	//boost
	eHealthBoost,
	ePowerBoost,
	eMaxPowerBoost,
	eSatietyBoost,
	eRadiationBoost,
	ePsyHealthBoost,
	eAlcoholBoost,
	eWoundsHealBoost,

	eRestoreBoostMax,

	eAdditionalSprintBoost = eRestoreBoostMax,
	eAdditionalJumpBoost,
	eAdditionalWeightBoost,

	eHitTypeProtectionBoosterIndex,

	eBurnImmunityBoost = eHitTypeProtectionBoosterIndex,
	eShockImmunityBoost,
	eStrikeImmunityBoost,
	eWoundImmunityBoost,
	eRadiationImmunityBoost,
	eTelepaticImmunityBoost,
	eChemicalBurnImmunityBoost,
	eExplosionImmunitBoost,
	eFireWoundImmunityBoost,

	eBoostMax,
};

struct SBooster {
	eBoostParams	m_BoostType{};
	float			f_BoostValue{};
	float			f_BoostTime{};
};

class CEntityCondition;
class CEntityConditionSimple
{
	float					m_fHealth;
	float					m_fHealthMax;
public:
							CEntityConditionSimple	();
	virtual					~CEntityConditionSimple	(){};

	IC float				GetHealth				() const			{return m_fHealth;}
	IC float 				GetMaxHealth			() const			{return m_fHealthMax;}
	IC float&				health					()					{return	m_fHealth;}
	IC float&				max_health				()					{return	m_fHealthMax;}

	virtual	CEntityCondition*	cast_entity_condition()					{ return NULL;  }
};

class CEntityCondition: public CEntityConditionSimple, public CHitImmunity
{
private:
	bool					m_use_limping_state;
	CEntityAlive			*m_object;

public:
							CEntityCondition		(CEntityAlive *object);
	virtual					~CEntityCondition		(void);

	virtual void			LoadCondition			(LPCSTR section);
	virtual void			remove_links			(const CObject *object);

	virtual void			save					(NET_Packet &output_packet);
	virtual void			load					(IReader &input_packet);

	IC float				GetPower				() const			{return m_fPower;}	
	IC float				GetRadiation			() const			{return m_fRadiation;}
	IC float				GetPsyHealth			() const			{return m_fPsyHealth;}
	IC float 				GetEntityMorale			() const			{return m_fEntityMorale;}
	virtual float 			GetAlcohol				() { return 0.f; }
	virtual float			GetSatiety				() { return 0.f; }

	IC float 				GetHealthLost			() const			{return m_fHealthLost;}

	void 					ChangeHealth			(float value);
	void 					ChangePower				(float value);
	void 					ChangeRadiation			(float value);
	void 					ChangePsyHealth			(float value);
	virtual void			ChangeSatiety			(float value){};
	virtual void 			ChangeAlcohol			(float value){};
			void 			ChangeMaxPower			(float value);

	IC void					SetMaxPower				(float val)			{m_fPowerMax = val; clamp(m_fPowerMax,0.1f,1.0f);};
	IC float				GetMaxPower				() const			{return m_fPowerMax;};

	void 					ChangeBleeding			(float percent);

	void 					ChangeEntityMorale		(float value);

	virtual CWound*			ConditionHit			(SHit* pHDS);
	//обновления состояния с течением времени
	virtual void			UpdateCondition			();
	void					UpdateWounds			();
	void					UpdateConditionTime		();
	IC void					SetConditionDeltaTime	(float DeltaTime) { m_fDeltaTime = DeltaTime; };

	virtual void					UpdatePower();
	
	//скорость потери крови из всех открытых ран 
	virtual float			BleedingSpeed			();

	CObject*				GetWhoHitLastTime		() {return m_pWho;}
	u16						GetWhoHitLastTimeID		() {return m_iWhoID;}

	CWound*					AddWound				(float hit_power, ALife::EHitType hit_type, u16 element);

	IC void 				SetCanBeHarmedState		(bool CanBeHarmed) 			{m_bCanBeHarmed = CanBeHarmed;}
	IC bool					CanBeHarmed				() const					{return m_bCanBeHarmed;};
	
	void					ClearWounds();
protected:
	virtual void			UpdateHealth			();
	virtual void			UpdateRadiation			();
	virtual void			UpdatePsyHealth			();
	virtual void			UpdateSatiety			(){};
	virtual void			UpdateAlcohol			(){};

	void					UpdateEntityMorale		();


	//изменение силы хита в зависимости от надетого костюма
	//(только для InventoryOwner)
	float					HitOutfitEffect			(SHit*);
	//изменение потери сил в зависимости от надетого костюма
	float					HitPowerEffect			(float power_loss);
	
	//для подсчета состояния открытых ран,
	//запоминается кость куда был нанесен хит
	//и скорость потери крови из раны
	DEFINE_VECTOR(CWound*, WOUND_VECTOR, WOUND_VECTOR_IT);
	WOUND_VECTOR			m_WoundVector;
	//очистка массива ран
	

	//все величины от 0 до 1			
	float m_fPower;					//сила
	float m_fRadiation;				//доза радиактивного облучения
	float m_fPsyHealth;				//здоровье

	float m_fEntityMorale;			//мораль

	//максимальные величины
	float m_fPowerMax;
	float m_fRadiationMax;
	float m_fPsyHealthMax;

	float m_fEntityMoraleMax;

	//величины изменения параметров на каждом обновлении
	float m_fDeltaHealth;
	float m_fDeltaPower;
	float m_fDeltaRadiation;
	float m_fDeltaPsyHealth;
	float m_fDeltaEntityMorale{};

	struct SConditionChangeV
	{
const	static int		PARAMS_COUNT = 7;

		float			m_fV_Radiation;
		float			m_fV_PsyHealth;
		float			m_fV_EntityMorale;
		float			m_fV_RadiationHealth;
		float			m_fV_Bleeding;
		float			m_fV_WoundIncarnation;
		float			m_fV_HealthRestore;
		float			&value(LPCSTR name);
		void			load(LPCSTR sect, LPCSTR prefix);
	};
	
	SConditionChangeV m_change_v{};

	float				m_fMinWoundSize;
	bool				m_bIsBleeding;

	//части хита, затрачиваемые на уменьшение здоровья и силы
	float m_fHealthHitPart[ALife::eHitTypeMax]{};
	float				m_fPowerHitPart;



	//потеря здоровья от последнего хита
	float				m_fHealthLost;


	//для отслеживания времени 
	u64					m_iLastTimeCalled;
	float				m_fDeltaTime{};
	//кто нанес последний хит
	CObject*			m_pWho;
	u16					m_iWhoID;

	//для передачи параметров из DamageManager
	float				m_fHitBoneScale;
	float				m_fWoundBoneScale;

	float				m_limping_threshold{};

	bool				m_bTimeValid;
	bool				m_bCanBeHarmed;

public:
	virtual void					reinit				();
	
	IC const	float				fdelta_time			() const 	{return		(m_fDeltaTime);			}
	IC const	WOUND_VECTOR&		wounds				() const	{return		(m_WoundVector);		}
	IC float&						radiation			()			{return		(m_fRadiation);			}
	IC float&						hit_bone_scale		()			{return		(m_fHitBoneScale);		}
	IC float&						wound_bone_scale	()			{return		(m_fWoundBoneScale);	}
	virtual	CEntityCondition*		cast_entity_condition()					{ return this; }
	static  void					script_register(lua_State *L);
	virtual float					GetParamByName		(LPCSTR name);
	IC SConditionChangeV&			GetChangeValues() { return m_change_v; }

	virtual float GetWoundIncarnation	() { return m_change_v.m_fV_WoundIncarnation; };
	virtual float GetHealthRestore		() { return m_change_v.m_fV_HealthRestore; };
	virtual float GetRadiationRestore	() { return m_change_v.m_fV_Radiation; };
	virtual float GetPsyHealthRestore	() { return m_change_v.m_fV_PsyHealth; };
	virtual float GetPowerRestore		() { return 1.f; };
	virtual float GetMaxPowerRestore	() { return 1.f; };
	virtual float GetSatietyRestore		() { return 1.f; };
	virtual float GetAlcoholRestore		() { return 1.f; };

	//застосувати зміни параметрів сутності у абсолюдних значеннях
	virtual void					ApplyInfluence	(int, float);
	virtual void					ApplyBooster	(SBooster&);

	virtual void					UpdateBoosters();

	virtual float					GetBoostedParams			(int);
	virtual float					GetBoostedHitTypeProtection (int);
	virtual void					BoostParameters			(const SBooster&);
	virtual void					DisableBoostParameters	(const SBooster&);

	virtual void					ClearAllBoosters		();
	//застосувати зміни параметрів сутності у відносних значеннях
	virtual void					ApplyRestoreBoost		(int, float);

	typedef							xr_map<eBoostParams, SBooster> BOOSTER_MAP;

protected:
	BOOSTER_MAP						m_boosters;
	svector<float, eBoostMax>		m_BoostParams;
};