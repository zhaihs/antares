#include "Body.h"

#include <SuperClass.h>
#include <ProgressScreenClass.h>

//0x4F8C97
DEFINE_HOOK(4F8C97, Sides_BuildConst, 6)
{
	GET(HouseClass *, pThis, ESI);

	for(int i = 0; i < RulesClass::Instance->BuildConst.Count; ++i) {
		if(pThis->OwnedBuildingTypes1.GetItemCount(RulesClass::Instance->BuildConst.GetItem(i)->ArrayIndex) > 0) {
			return 0x4F8D02;	//"low power"
		}
	}

	return 0x4F8DB1;
}

//0x4F8F54
DEFINE_HOOK(4F8F54, Sides_SlaveMinerCheck, 6)
{
	GET(HouseClass *, pThis, ESI);
	GET(int, n, EDI);

	for(int i = 0; i < RulesClass::Instance->BuildRefinery.Count; ++i) {
		 //new sane way to find a slave miner
		if(RulesClass::Instance->BuildRefinery.Items[i]->SlavesNumber > 0) {
			n += pThis->OwnedBuildingTypes1.GetItemCount(
				RulesClass::Instance->BuildRefinery.Items[i]->ArrayIndex);
		}
	}

	R->EDI(n);
	return 0x4F8F75;
}

//0x505C95
DEFINE_HOOK(505C95, Sides_BaseDefenseCounts, 7)
{
	GET(HouseClass *, pThis, EBX);
	int n = R->Stack32(0x80);	//just to be on the safe side, we're not getting it from the House

	SideClass* pSide = SideClass::Array->GetItem(n);
	if(SideExt::ExtData *pData = SideExt::ExtMap.Find(pSide)) {
		if(pThis->AIDifficulty < pData->BaseDefenseCounts.Count) {
			R->EAX<int>(pData->BaseDefenseCounts.GetItem(pThis->AIDifficulty));
			return 0x505CE9;
		} else {
			Debug::Log("WTF! vector has %d items, requested item #%d\n", pData->BaseDefenseCounts.Count, pThis->AIDifficulty);
		}
	}
	return 0;
}

//0x507BCA
DEFINE_HOOK(507BCA, Sides_BaseDefenses1, 6)
	{ return SideExt::BaseDefenses(R, 0x507C00); }

//0x507DBA
DEFINE_HOOK(507DBA, Sides_BaseDefenses2, 6)
	{ return SideExt::BaseDefenses(R, 0x507DF0); }

//0x507FAA
DEFINE_HOOK(507FAA, Sides_BaseDefenses3, 6)
	{ return SideExt::BaseDefenses(R, 0x507FE0); }

//0x52267D
DEFINE_HOOK(52267D, Sides_Disguise1, 6)
{
	GET(HouseClass *, pHouse, EAX);

	int n = pHouse->SideIndex;
	SideClass* pSide = SideClass::Array->GetItem(n);
	if(SideExt::ExtData *pData = SideExt::ExtMap.Find(pSide)) {
		R->EAX<InfantryTypeClass *>(pData->DefaultDisguise);
		return 0x5226B7;
	} else {
		return 0;
	}
}

//0x5227A3
DEFINE_HOOK(5227A3, Sides_Disguise2, 6)
	{ return SideExt::Disguise(R, 0x5227EC, false); }

//0x6F422F
DEFINE_HOOK(6F422F, Sides_Disguise3, 6)
	{ return SideExt::Disguise(R, 0x6F4277, true); }

//0x707D40
DEFINE_HOOK(707D40, Sides_Crew, 6)
{
	GET(HouseClass *, pHouse, ECX);

	int n = pHouse->SideIndex;
	SideClass* pSide = SideClass::Array->GetItem(n);
	if(SideExt::ExtData *pData = SideExt::ExtMap.Find(pSide)) {
		R->ESI<InfantryTypeClass *>(pData->Crew);
		return 0x707D81;
	} else {
		return 0;
	}
}

//0x451358
DEFINE_HOOK(451358, Sides_SurvivorDivisor, 6)
{
	GET(HouseClass *, pHouse, EDX);

	SideClass* pSide = SideClass::Array->GetItem(pHouse->SideIndex);
	if(SideExt::ExtData *pData = SideExt::ExtMap.Find(pSide)) {
		R->ESI<int>(pData->SurvivorDivisor);
		return 0x451391;
	} else {
		return 0;
	}
}

/*
 * this is as good as it can get without tearing the scenario reader apart
 * - find house early, set color from its data...
 * but finding house needs the house array to be ready
 * instantiating house needs data from rules
 * instantiating rules takes shitloads of time, we can't show a blank screen so long
A_FINE_HOOK(687586, INIClass_ReadScenario, 7)
{
	GET(LoadProgressManager *, Mgr, EAX);
	if(SessionClass::Instance->GameMode == GameMode::Campaign) {
		GET_STACK(CCINIClass *, pINI, STACK_OFFS(0x174, 0x15C));

		HouseClass::LoadFromINIList(pINI); // comment out this line to make it work everywhere except for the very first scenario you try

		pINI->ReadString("Basic", "Player", "Americans", Ares::readBuffer, Ares::readLength);
		int idxHouse = HouseClass::FindIndexByName(Ares::readBuffer);
		Debug::Log("Side was %d and iH = %d\n", ProgressScreenClass::Instance->GetSide(), idxHouse);
		if(idxHouse > -1 && idxHouse < HouseClass::Array->Count) {
			int idxSide = HouseClass::Array->GetItem(idxHouse)->Type->SideIndex;

			ProgressScreenClass::Instance->SetSide(idxSide);
			Debug::Log("Side is now %d\n", idxSide);
		}
	}

	Mgr->Draw();
	return 0x68758D;
}
*/

// WRONG! Stoopidwood passes CD= instead of Side= into singleplayer campaigns, TODO: fix that shit
DEFINE_HOOK(642B36, Sides_LoadTextColor1, 5)
	{ return SideExt::LoadTextColor(R, 0x68CAA9); }

// WRONG! Stoopidwood passes CD= instead of Side= into singleplayer campaigns, TODO: fix that shit
DEFINE_HOOK(643BB9, Sides_LoadTextColor2, 5)
	{ return SideExt::LoadTextColor(R, 0x643BEF); }

DEFINE_HOOK(642B91, Sides_LoadTextColor3, 5)
	{ return SideExt::LoadTextColor(R, 0x68CAA9); }

//0x534FB1
DEFINE_HOOK(534FB1, Sides_MixFileIndex, 5)
{
	GET(int, n, ESI);
	SideClass* pSide = SideClass::Array->GetItem(n);
	if(SideExt::ExtData *pData = SideExt::ExtMap.Find(pSide)) {
		// original code is
		// sprintf(mixname, "SIDEC%02dMD.MIX", ESI + 1);
		// it's easier to sub 1 here than to fix the calculation in the orig code
		R->ESI(pData->SidebarMixFileIndex - 1);
	} else if(n == 2) {
		R->ESI(1);
	}

	return 0x534FBB;
}

DEFINE_HOOK(72FA1A, Sides_MixFileYuriFiles1, 7)
	{ return SideExt::MixFileYuriFiles(R, 0x72FA23, 0x72FA6A); }

DEFINE_HOOK(72F370, Sides_MixFileYuriFiles2, 7)
	{ return SideExt::MixFileYuriFiles(R, 0x72F379, 0x72F3A0); }

DEFINE_HOOK(72FBC0, Sides_MixFileYuriFiles3, 5)
	{ return SideExt::MixFileYuriFiles(R, 0x72FBCE, 0x72FBF5); }

DEFINE_HOOK(6CD3C1, Sides_ParaDrop, 9)
{
	GET(SuperClass *, SW, EBX);
	HouseClass * pHouse = SW->Owner;
	GET(CellClass *, Cell, EBP);

	SideClass* pSide = SideClass::Array->GetItem(pHouse->SideIndex);
	if(SideExt::ExtData *pData = SideExt::ExtMap.Find(pSide)) {
		Ares::SendPDPlane(
			pHouse,
			Cell,
			AircraftTypeClass::Array->GetItem(R->ESI()),
			&pData->ParaDrop,
			&pData->ParaDropNum);

		return 0x6CD500;
	}
	return 0;
}

/*
//0x752F46
XPORT Sides_LoadVoxFromINI(REGISTERS* R)
{
	VoxClass* pThis = (VoxClass*)R->get_ESI();
	CCINIClass* pINI = (CCINIClass*)R->get_EBP();

	DEBUGLOG("VoxClass::LoadFromINI (%s, pINI = 0x%08X)\n", pThis->get_Name(), pINI);

	DynamicVectorClass<SideExt::VoxFileNameStruct> FileNames;
	SideExt::VoxFileNameStruct vfn;
	char buffer[0x10] = "\0";

	for(int i = 0; i < SideClass::Array->get_Count(); i++)
	{
		if(SideExt::Map.find((*SideClass::Array)[i]) != SideExt::Map.end())
		{
			pINI->ReadString(
				pThis->get_Name(),
				SideExt::Map[(*SideClass::Array)[i]].EVATag,
				"",
				buffer,
				0x10);

			strcpy(vfn.FileName, buffer);

			FileNames.AddItem(vfn);
		}
		else
		{
			*vfn.FileName = 0;
			FileNames.AddItem(vfn);	//make sure there's an entry for every side
		}
	}

	SideExt::EVAFiles[pThis] = FileNames;
	return 0;
}
*/

//0x7528E8
DEFINE_HOOK(7528E8, Sides_LoadVoxFile, 7)
{
	GET(VoxClass *, pThis, EBP);
	if(SideExt::EVAFiles.find(pThis) != SideExt::EVAFiles.end()) {
		int nSideIndex = *((int*)0xB1D4C8);

		R->EDI(SideExt::EVAFiles[pThis][nSideIndex].FileName);
		return 0x752901;
	}

	return 0;
}

/* fixes to reorder the savegame */
DEFINE_HOOK(67D315, SaveGame_EarlySaveSides, 5)
{
	GET(LPSTREAM, pStm, ESI);
	return (Game::Save_Sides(pStm, SideClass::Array) >= 0)
		? 0
		: 0x67E0B8
	;
}

DEFINE_HOOK(67E09A, SaveGame_LateSkipSides, 5)
{
	GET(int, success, EAX);
	return success >= 0
		? 0x67E0C2
		: 0x67E0B8
	;
}


DEFINE_HOOK(67E74A, LoadGame_EarlyLoadSides, 5)
{
	GET(LPSTREAM, pStm, ESI);

	int length = 0;
	LPVOID out;
	if(pStm->Read(&length, 4, 0) < 0) {
		return 0x67F7A3;
	}
	for(int i = 0; i < length; ++i) {
		if((*Imports::OleLoadFromStream)(pStm, &IIDs::AbstractClass_0, &out) < 0) {
			return 0x67F7A3;
		}
	}

	return 0;
}

DEFINE_HOOK(67F281, LoadGame_LateSkipSides, 7)
{
	return 0x67F2BF;
}