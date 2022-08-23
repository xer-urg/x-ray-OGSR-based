#include "stdafx.h"
#include "UIEatableParams.h"
#include "UIXmlInit.h"

#include "string_table.h"

#include "eatable_item.h"

constexpr auto EATABLE_PARAMS = "eatable_params.xml";

LPCSTR effect_names[] = {
	"ui_inv_health",
	"ui_inv_power",
	"ui_inv_max_power",
	"ui_inv_satiety",
	"ui_inv_radiation",
	"ui_inv_psy_health",
	"ui_inv_alcohol",
	"ui_inv_thirst",
	"ui_inv_wounds_heal_perc",
};

LPCSTR effect_static_names[] = {
	"eat_health",
	"eat_power",
	"eat_max_power",
	"eat_satiety",
	"eat_radiation",
	"eat_psyhealth",
	"eat_alcohol",
	"eat_thirst",
	"wounds_heal_perc",
};

CUIEatableParams::CUIEatableParams() {
}

CUIEatableParams::~CUIEatableParams() {
	for (u32 i = 0; i < _max_item_index; ++i){
		CUIStatic* _s = m_info_items[i];
		xr_delete(_s);
	}
}

void CUIEatableParams::Init() {
	CUIXml uiXml;
	uiXml.Init(CONFIG_PATH, UI_PATH, EATABLE_PARAMS);

	LPCSTR _base = "eatable_params";
	if (!uiXml.NavigateToNode(_base, 0))	return;

	string256					_buff;
	CUIXmlInit::InitWindow(uiXml, _base, 0, this);

	for (u32 i = 0; i < _max_item_index; ++i)
	{
		strconcat(sizeof(_buff), _buff, _base, ":static_", effect_static_names[i]);

		if (uiXml.NavigateToNode(_buff, 0)){
			m_info_items[i] = xr_new<CUIStatic>();
			CUIStatic* _s = m_info_items[i];
			_s->SetAutoDelete(false);
			CUIXmlInit::InitStatic(uiXml, _buff, 0, _s);
		}
	}
}

bool CUIEatableParams::Check(CInventoryItem* obj) {
	if (smart_cast<CEatableItem*>(obj)) {
		return true;
	}
	else
		return false;
}

void CUIEatableParams::SetInfo(CInventoryItem* obj) {
	string128					text_to_show;
	float						_h = 0.0f;
	DetachAll();

	if (!obj) return;

	auto pEatable = smart_cast<CEatableItem*>(obj);

	for (u32 i = 0; i < _max_item_index; ++i){
		
		CUIStatic* _s = m_info_items[i];
		
		float _val = pEatable->GetItemInfluence(CEatableItem::ItemInfluence(i));
		if (fis_zero(_val)) continue;

		auto effect_name = CStringTable().translate(effect_names[i]).c_str();

		_val *= 100.0f;
		LPCSTR _sn = "%";
		LPCSTR _color = (_val > 0) ? "%c[green]" : "%c[red]";
		sprintf_s(text_to_show, "%s %s %+.1f %s",
			effect_name,
			_color,
			_val,
			_sn);

		_s->SetText(text_to_show);
		_s->SetWndPos(_s->GetWndPos().x, _h);
		_h += _s->GetWndSize().y;
		AttachChild(_s);
	}
	SetHeight(_h);
}