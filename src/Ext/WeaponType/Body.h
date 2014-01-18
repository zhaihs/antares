#ifndef WEAPONTYPE_EXT_H
#define WEAPONTYPE_EXT_H

#include <xcompile.h>
#include <Helpers/Template.h>
#include <BombClass.h>
#include <BombListClass.h>
#include <BulletClass.h>
#include <CCINIClass.h>
#include <Drawing.h>
#include <EBolt.h>
#include <RadBeam.h>
#include <RadSiteClass.h>
#include <VocClass.h>
#include <WarheadTypeClass.h>
#include <WaveClass.h>
#include <WeaponTypeClass.h>
#include <ScenarioClass.h>
#include <MouseClass.h>


#include "../../Enum/RadTypes.h"

#include "../../Misc/Debug.h"

#include "../_Container.hpp"

#include "../../Utilities/Template.h"

struct SHPStruct;

class WeaponTypeExt
{
public:
	typedef WeaponTypeClass TT;
	class ExtData : public Extension<TT>
	{
	public:
		// static defaults
		static ColorStruct DefaultWaveColor;
		static ColorStruct DefaultWaveColorSonic;
		static ColorStruct DefaultWaveColorMagBeam;

		// Generic
		bool Weapon_Loaded;

		// Coloured Rad Beams
		Customizable<ColorStruct> Beam_Color;
		int    Beam_Duration;
		double Beam_Amplitude;
		bool   Beam_IsHouseColor;

		// Coloured EBolts
		Nullable<ColorStruct> Bolt_Color1;
		Nullable<ColorStruct> Bolt_Color2;
		Nullable<ColorStruct> Bolt_Color3;

		// TS Lasers
		bool   Wave_IsHouseColor;
		bool   Wave_IsLaser;
		bool   Wave_IsBigLaser;
		Customizable<ColorStruct> Wave_Color;
		bool   Wave_Reverse[5];

		Valueable<int> Laser_Thickness;
/*
		int    Wave_InitialIntensity;
		int    Wave_IntensityStep;
		int    Wave_FinalIntensity;
*/
		// custom Ivan Bombs
		Valueable<bool> Ivan_KillsBridges;
		Valueable<bool> Ivan_Detachable;
		Nullable<int> Ivan_Damage;
		Nullable<int> Ivan_Delay;
		NullableIdx<VocClass> Ivan_TickingSound;
		NullableIdx<VocClass> Ivan_AttachSound;
		Nullable<WarheadTypeClass *> Ivan_WH;
		Nullable<SHPStruct *> Ivan_Image;
		Nullable<int> Ivan_FlickerRate;
		Nullable<bool> Ivan_CanDetonateTimeBomb;
		Nullable<bool> Ivan_CanDetonateDeathBomb;

		RadType * Rad_Type;

//		MouseCursor Cursor_Attack;
//		bool Cursor_Custom;

		// #680 Chrono Prison
		Valueable<bool> Abductor; //!< Will this weapon force eligible targets into the passenger hold of the shooter?
		Valueable<AnimTypeClass *> Abductor_AnimType;
		Valueable <bool> Abductor_ChangeOwner;
		Valueable<double> Abductor_AbductBelowPercent;
		
		ExtData(TT* const OwnerObject) : Extension<TT>(OwnerObject),
			Weapon_Loaded (false),
			Beam_Color (&RulesClass::Instance->RadColor),
			Beam_Duration (15),
			Beam_Amplitude (40.0),
			Beam_IsHouseColor (false),
			Bolt_Color1 (),
			Bolt_Color2 (),
			Bolt_Color3 (),
			Wave_IsHouseColor (false),
			Wave_IsLaser (false),
			Wave_IsBigLaser (false),
			Wave_Color (nullptr),
			Laser_Thickness (-1),
			Ivan_KillsBridges (true),
			Ivan_Detachable (true),
			Ivan_Damage (),
			Ivan_Delay (),
			Ivan_TickingSound (),
			Ivan_AttachSound (),
			Ivan_WH (),
			Ivan_Image (),
			Ivan_FlickerRate (),
			Ivan_CanDetonateTimeBomb (),
			Ivan_CanDetonateDeathBomb (),
			Rad_Type (nullptr),
			Abductor(false),
			Abductor_AnimType(nullptr),
			Abductor_ChangeOwner(false),
			Abductor_AbductBelowPercent(1)
			{
//				this->Beam_Color = ColorStruct(255, 255, 255);
//				this->Wave_Color = ColorStruct(255, 255, 255);
				for(int i = 0; i < 5; ++i) {
					this->Wave_Reverse[i] = false;
				}
			};

		virtual void LoadFromINIFile(TT *pThis, CCINIClass *pINI);
		virtual void Initialize(TT* pThis);

		virtual void InvalidatePointer(void *ptr, bool bRemoved) {
			AnnounceInvalidPointer(Rad_Type, ptr);
		}

		bool IsWave(WeaponTypeClass *pThis) {
			return pThis->IsSonic || pThis->IsMagBeam || this->Wave_IsLaser || this->Wave_IsBigLaser;
		}

		bool conductAbduction(BulletClass *);

		void PlantBomb(TechnoClass* pSource, ObjectClass* pTarget) const;
	};

/*
	struct WeaponWeight
	{
		short index;
		bool InRange;
		float DPF;
		bool operator < (const WeaponWeight &RHS) const {
			return (this->InRange < RHS.InRange && this->DPF < RHS.DPF);
		}
	};

	EXT_P_DECLARE(WeaponTypeClass);
	EXT_FUNCS(WeaponTypeClass);
	EXT_INI_FUNCS(WeaponTypeClass);

*/

	static Container<WeaponTypeExt> ExtMap;

	static hash_map<BombClass *, ExtData *> BombExt;
	static hash_map<WaveClass *, ExtData *> WaveExt;
	static hash_map<EBolt *, ExtData *> BoltExt;
	static hash_map<RadSiteClass *, ExtData *> RadSiteExt;

#define idxVehicle 0
#define idxAircraft 1
#define idxBuilding 2
#define idxInfantry 3
#define idxOther 4

	static char AbsIDtoIdx(eAbstractType absId)
		{
			switch(absId)
			{
				case abs_Unit:
					return idxVehicle;
				case abs_Aircraft:
					return idxAircraft;
				case abs_Building:
					return idxBuilding;
				case abs_Infantry:
					return idxInfantry;
				default:
					return idxOther;
			}
		};

	// @return skipNormalHandling?
	static bool ModifyWaveColor(WORD *src, WORD *dst, int Intensity, WaveClass *Wave);
};

typedef hash_map<BombClass *, WeaponTypeExt::ExtData *> hash_bombExt;
typedef hash_map<WaveClass *, WeaponTypeExt::ExtData *> hash_waveExt;
typedef hash_map<EBolt *, WeaponTypeExt::ExtData *> hash_boltExt;
typedef hash_map<RadSiteClass *, WeaponTypeExt::ExtData *> hash_radsiteExt;

#endif
