///////////////////////////////////////////////////////////////
// GraviArtifact.h
// GraviArtefact - гравитационный артефакт, прыгает на месте
// и парит над землей
///////////////////////////////////////////////////////////////

#pragma once
#include "artifact.h"

class CGraviArtefact : public CArtefact 
{
private:
	typedef CArtefact inherited;
public:
	CGraviArtefact(void);
	virtual ~CGraviArtefact(void) {};

	virtual void Load				(LPCSTR section);

protected:
	virtual void	UpdateCLChild	();
	//параметры артефакта
	float m_fJumpHeight{};
	float m_fEnergy{ 1.f };

	float GetJumpHeight() { return m_fJumpHeight * GetCondition(); }
};
