#include <YRPP.h>
#include "TechnoTypeExt.h"
#include "Sides.h"
#include "Prerequisites.h"

EXT_P_DEFINE(TechnoTypeClass);

EXT_CTOR(TechnoTypeClass)
{
	if(!CONTAINS(Ext_p, pThis))
	{
		ALLOC(ExtData, pData);

		pData->Survivors_Pilots.SetCapacity(1, NULL);

		pData->Survivors_PilotChance = 0;
		pData->Survivors_PassengerChance = 0;
		pData->Survivors_Pilots[0] = NULL;

		pData->Data_Initialized = 0;

		Ext_p[pThis] = pData;
	}
}

EXT_DTOR(TechnoTypeClass)
{
	if(CONTAINS(Ext_p, pThis))
	{
		DEALLOC(Ext_p, pThis);
	}
}

EXT_LOAD(TechnoTypeClass)
{
	if(CONTAINS(Ext_p, pThis))
	{
		Create(pThis);

		ULONG out;
		pStm->Read(&Ext_p[pThis], sizeof(ExtData), &out);

		for(int i = 0; i < Ext_p[pThis]->Survivors_Pilots.get_Count(); ++i)
		{
			SWIZZLE(Ext_p[pThis]->Survivors_Pilots[i]);
		}
	}
}

EXT_SAVE(TechnoTypeClass)
{
	if(CONTAINS(Ext_p, pThis))
	{
		ULONG out;
		pStm->Write(&Ext_p[pThis], sizeof(ExtData), &out);
	}
}

void TechnoTypeClassExt::TechnoTypeClassData::Initialize(TechnoTypeClass *pThis)
{
	this->Survivors_Pilots.SetCapacity(SideClass::Array->get_Count(), NULL);

	this->Survivors_PilotChance = (int)RulesClass::Global()->get_CrewEscape() * 100;
	this->Survivors_PassengerChance = (int)RulesClass::Global()->get_CrewEscape() * 100;

	for(int i = 0; i < SideClass::Array->get_Count(); ++i)
	{
		this->Survivors_Pilots[i] = Sides::SideExt[SideClass::Array->GetItem(i)].Crew;
	}

	this->PrerequisiteLists.SetCapacity(0, NULL);
	this->PrerequisiteLists.AddItem(new DynamicVectorClass<int>);

	this->PrerequisiteTheaters = 0xFFFFFFFF;

	this->Data_Initialized = 1;
}

EXT_LOAD_INI(TechnoTypeClass)
{
	const char * section = pThis->get_ID();
	if(!CONTAINS(Ext_p, pThis) || !pINI->GetSection(section))
	{
		return;
	}

	ExtData *pData = Ext_p[pThis];
	if(!pData->Data_Initialized)
	{
		pData->Initialize(pThis);
	}

	pData->Survivors_Pilots.SetCapacity(SideClass::Array->get_Count(), NULL);

	pData->Survivors_PilotChance = pINI->ReadInteger(section, "Survivor.PilotChance", pData->Survivors_PilotChance);
	pData->Survivors_PassengerChance = pINI->ReadInteger(section, "Survivor.PassengerChance", pData->Survivors_PassengerChance);

	char buffer[512];
	char flag[256];
	for(int i = 0; i < SideClass::Array->get_Count(); ++i)
	{
		sprintf(flag, "Survivor.Side%d", i);
		PARSE_INFANTRY(flag, pData->Survivors_Pilots[i]);
	}

	int PrereqListLen = pINI->ReadInteger(section, "Prerequisite.Lists", pData->PrerequisiteLists.get_Count() - 1);

	if(PrereqListLen < 1)
	{
		PrereqListLen = 0;
	}
	++PrereqListLen;
	while(PrereqListLen > pData->PrerequisiteLists.get_Count())
	{
		pData->PrerequisiteLists.AddItem(new DynamicVectorClass<int>);
	}

	DynamicVectorClass<int> *dvc = pData->PrerequisiteLists.GetItem(0);
	if(pINI->ReadString(section, "Prerequisite", "", buffer, 0x200))
	{
		Prereqs::Parse(buffer, dvc);
	}
	for(int i = 1; i < PrereqListLen + 1; ++i)
	{
		sprintf(flag, "Prerequisite.List%d", i);
		if(pINI->ReadString(section, flag, "", buffer, 0x200))
		{
			dvc = pData->PrerequisiteLists.GetItem(i);
			Prereqs::Parse(buffer, dvc);
		}
	}

	dvc = &pData->PrerequisiteNegatives;
	if(pINI->ReadString(section, "Prerequisite.Negative", "", buffer, 0x200))
	{
		Prereqs::Parse(buffer, dvc);
	}

	if(pINI->ReadString(section, "Prerequisite.RequiredTheaters", "", buffer, 0x200))
	{
		pData->PrerequisiteTheaters = 0;
		for(char *cur = strtok(buffer, ","); cur; cur = strtok(NULL, ","))
		{
			DWORD idx = Theater::FindIndex(cur);
			if(idx > -1)
			{
				pData->PrerequisiteTheaters |= 1 << idx;
			}
		}
	}

}

