#pragma once


#include "ui/UIStatic.h"

class CActor;
class CUICustomMap;
//////////////////////////////////////////////////////////////////////////


class CUIZoneMap
{
	CUICustomMap*				m_activeMap{};
	float						m_fScale{};

	CUIStatic					m_background;
	CUIStatic					m_center;
	CUIStatic					m_compass;
	CUIStatic					m_clipFrame;
	CUIStatic					m_pointerDistanceText;
	CUIStatic					m_NoPower;
	CUIStatic					m_CurrentTime;
	CUIStatic					m_CurrentPower;
	CUIStatic					m_CurrentPowerLow;

public:
								CUIZoneMap		();
	virtual						~CUIZoneMap		();

	void						SetHeading		(float angle);
	void						Init			();

	void						Render			();
	void						UpdateRadar		(Fvector pos);

	void						SetScale		(float s)							{m_fScale = s;}
	float						GetScale		()									{return m_fScale;}

	bool						ZoomIn			();
	bool						ZoomOut			();

	CUIStatic&					Background		()									{return m_background;};
	CUIStatic&					ClipFrame		()									{return m_clipFrame; }; // alpet: для экспорта в скрипты
	CUIStatic&					Compass			()									{return m_compass; }; // alpet: для экспорта в скрипты

	void						SetupCurrentMap	();
};

