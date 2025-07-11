/*
 * This file is part of the CMaNGOS Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef MANGOSSERVER_PET_H
#define MANGOSSERVER_PET_H

#include "Common.h"
#include "Entities/ObjectGuid.h"
#include "Entities/Creature.h"
#include "Entities/Unit.h"
#include "Maps/InstanceData.h"

enum PetType
{
    SUMMON_PET              = 0,
    HUNTER_PET              = 1,
    GUARDIAN_PET            = 2,
    MINI_PET                = 3,
    MAX_PET_TYPE            = 4
};

#define MAX_PET_STABLES         2

// stored in character_pet.slot
enum PetSaveMode
{
    PET_SAVE_AS_DELETED        = -1,                        // not saved in fact
    PET_SAVE_AS_CURRENT        =  0,                        // in current slot (with player)
    PET_SAVE_FIRST_STABLE_SLOT =  1,
    PET_SAVE_LAST_STABLE_SLOT  =  MAX_PET_STABLES,          // last in DB stable slot index (including), all higher have same meaning as PET_SAVE_NOT_IN_SLOT
    PET_SAVE_NOT_IN_SLOT       =  100,                      // for avoid conflict with stable size grow will use 100
    PET_SAVE_REAGENTS          =  101                       // PET_SAVE_NOT_IN_SLOT with reagents return
};

// There might be a lot more
enum PetModeFlags
{
    PET_MODE_STAY              = 0x0000000,
    PET_MODE_FOLLOW            = 0x0000001,
    PET_MODE_ATTACK            = 0x0000002,
    PET_MODE_PASSIVE           = 0x0000000,
    PET_MODE_DEFENSIVE         = 0x0000100,
    PET_MODE_AGGRESSIVE        = 0x0000200,

    PET_MODE_DISABLE_ACTIONS   = 0x8000000,

    // autoset in client at summon
    PET_MODE_DEFAULT           = PET_MODE_FOLLOW | PET_MODE_DEFENSIVE,
};

enum HappinessState
{
    UNHAPPY = 1,
    CONTENT = 2,
    HAPPY   = 3
};

enum LoyaltyLevel
{
    REBELLIOUS  = 1,
    UNRULY      = 2,
    SUBMISSIVE  = 3,
    DEPENDABLE  = 4,
    FAITHFUL    = 5,
    BEST_FRIEND = 6
};

enum PetSpellState
{
    PETSPELL_UNCHANGED = 0,
    PETSPELL_CHANGED   = 1,
    PETSPELL_NEW       = 2,
    PETSPELL_REMOVED   = 3
};

enum PetSpellType
{
    PETSPELL_NORMAL = 0,
    PETSPELL_FAMILY = 1,
};

struct PetSpell
{
    uint8 active;                                           // use instead enum (not good use *uint8* limited enum in case when value in enum not positive in *int8*)

    PetSpellState state : 8;
    PetSpellType type   : 8;
};

enum ActionFeedback
{
    FEEDBACK_NONE            = 0,
    FEEDBACK_PET_DEAD        = 1,
    FEEDBACK_NOTHING_TO_ATT  = 2,
    FEEDBACK_CANT_ATT_TARGET = 3
};

enum PetTalk
{
    PET_TALK_SPECIAL_SPELL  = 0,
    PET_TALK_ATTACK         = 1
};

enum PetNameInvalidReason
{
    // custom, not send
    PET_NAME_SUCCESS                                        = 0,

    PET_NAME_INVALID                                        = 1,
    PET_NAME_NO_NAME                                        = 2,
    PET_NAME_TOO_SHORT                                      = 3,
    PET_NAME_TOO_LONG                                       = 4,
    PET_NAME_MIXED_LANGUAGES                                = 6,
    PET_NAME_PROFANE                                        = 7,
    PET_NAME_RESERVED                                       = 8,
    PET_NAME_THREE_CONSECUTIVE                              = 11,
    PET_NAME_INVALID_SPACE                                  = 12,
    PET_NAME_CONSECUTIVE_SPACES                             = 13,
    PET_NAME_RUSSIAN_CONSECUTIVE_SILENT_CHARACTERS          = 14,
    PET_NAME_RUSSIAN_SILENT_CHARACTER_AT_BEGINNING_OR_END   = 15,
    PET_NAME_DECLENSION_DOESNT_MATCH_BASE_NAME              = 16
};

typedef std::unordered_map<uint32, PetSpell> PetSpellMap;
typedef std::map<uint32, uint32> TeachSpellMap;
typedef std::vector<uint32> AutoSpellList;

#define HAPPINESS_LEVEL_SIZE        333000

extern const uint32 LevelUpLoyalty[6];
extern const uint32 LevelStartLoyalty[6];

#define ACTIVE_SPELLS_MAX           4

class Player;

class Pet : public Creature
{
    public:
        explicit Pet(PetType type = MAX_PET_TYPE);
        virtual ~Pet();

        void AddToWorld() override;
        void RemoveFromWorld() override;

        PetType getPetType() const { return m_petType; }
        void setPetType(PetType type) { m_petType = type; }
        bool isControlled() const { return getPetType() == SUMMON_PET || getPetType() == HUNTER_PET; }
        bool isTemporarySummoned() const { return m_duration > 0; }
        bool IsGuardian() const { return getPetType() == GUARDIAN_PET; }

        bool Create(uint32 guidlow, CreatureCreatePos& cPos, CreatureInfo const* cinfo, uint32 pet_number);
        bool CreateBaseAtCreature(Creature* creature);
        bool LoadPetFromDB(Player* owner, Position const& spawnPos, uint32 petentry = 0, uint32 petnumber = 0, bool current = false, uint32 healthPercentage = 0, bool permanentOnly = false, bool forced = false);
        void SavePetToDB(PetSaveMode mode, Player* owner);
        Position GetPetSpawnPosition(Player* owner);
        bool isLoading() const { return m_loading; }
        void SetLoading(bool state) { m_loading = state; }
        void Unsummon(PetSaveMode mode, Unit* owner = nullptr);
        static void DeleteFromDB(uint32 guidlow, bool separate_transaction = true);
        static void DeleteFromDB(Unit* owner, PetSaveMode slot);
        static SpellCastResult TryLoadFromDB(Unit* owner, uint32 petentry = 0, uint32 petnumber = 0, bool current = false, PetType mandatoryPetType = MAX_PET_TYPE);
        void PlayDismissSound();

        ObjectGuid const GetSpawnerGuid() const override { return GetOwnerGuid(); }

        void SetOwnerGuid(ObjectGuid owner) override;

        Player* GetSpellModOwner() const override;

        void SetDeathState(DeathState s) override;          // overwrite virtual Creature::SetDeathState and Unit::SetDeathState
        void Update(const uint32 diff) override;  // overwrite virtual Creature::Update and Unit::Update

        uint8 GetPetAutoSpellSize() const override { return m_autospells.size(); }
        uint32 GetPetAutoSpellOnPos(uint8 pos) const override
        {
            if (pos >= m_autospells.size())
                return 0;
            return m_autospells[pos];
        }

        bool CanSwim() const override
        {
            if (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED))
                return true;
            if (Unit const* owner = GetOwner())
                return static_cast<Creature const*>(owner)->CanSwim();
            return Creature::CanSwim();
        }

        void RegenerateAll(uint32 update_diff) override;    // overwrite Creature::RegenerateAll
        void LooseHappiness();
        void TickLoyaltyChange();
        void ModifyLoyalty(int32 addvalue);
        HappinessState GetHappinessState() const;
        uint32 GetMaxLoyaltyPoints(uint32 level) const;
        uint32 GetStartLoyaltyPoints(uint32 level) const;
        void KillLoyaltyBonus(uint32 level);
        uint32 GetLoyaltyLevel() { return GetByteValue(UNIT_FIELD_BYTES_1, UNIT_BYTES_1_OFFSET_PET_LOYALTY); }
        void SetLoyaltyLevel(LoyaltyLevel level);
        void GivePetXP(uint32 xp, Player* owner);
        void GivePetLevel(uint32 level);
        void SynchronizeLevelWithOwner();
        void InitStatsForLevel(uint32 petlevel);
        void InitPetScalingAuras();
        bool HaveInDiet(ItemPrototype const* item) const;
        uint32 GetCurrentFoodBenefitLevel(uint32 itemlevel) const;
        void SetDuration(int32 dur) { m_duration = dur; }
        int32 GetDuration() const { return m_duration; }

        bool UpdateStats(Stats stat) override;
        void UpdateMaxHealth() override;
        void UpdateMaxPower(Powers power) override;
        void UpdateAttackPowerAndDamage(bool ranged = false) override;
        float GetConditionalTotalPhysicalDamageModifier(WeaponAttackType type) const override;

        bool   CanTakeMoreActiveSpells(uint32 spellid) const;
        void   ToggleAutocast(uint32 spellid, bool apply);
        bool   HasTPForSpell(uint32 spellid) const;
        int32  GetTPForSpell(uint32 spellid) const;

        void SetModeFlags(PetModeFlags mode);
        PetModeFlags GetModeFlags() const { return m_petModeFlags; }

        bool HasSpell(uint32 spell) const override;
        void AddTeachSpell(uint32 learned_id, uint32 source_id) { m_teachspells[learned_id] = source_id; }

        void LearnPetPassives();
        void CastPetAuras(bool current);
        void CastOwnerTalentAuras();
        void CastPetAura(PetAura const* aura);

        void _LoadSpellCooldowns();
        void _SaveSpellCooldowns();
        void _LoadAuras(uint32 timediff);
        void _SaveAuras();
        void _LoadSpells();
        void _SaveSpells();

        bool addSpell(uint32 spell_id, ActiveStates active = ACT_DECIDE, PetSpellState state = PETSPELL_NEW, PetSpellType type = PETSPELL_NORMAL);
        bool learnSpell(uint32 spell_id);
        bool unlearnSpell(uint32 spell_id, bool learn_prev, bool clear_ab = true);
        bool removeSpell(uint32 spell_id, bool learn_prev, bool clear_ab = true);
        void CleanupActionBar();

        PetSpellMap     m_spells;
        TeachSpellMap   m_teachspells;
        AutoSpellList   m_autospells;

        void InitPetCreateSpells();
        void CheckLearning(uint32 spellid);
        uint32 resetTalentsCost() const;

        virtual CharmInfo* InitCharmInfo(Unit* charm) override;
        virtual void DeleteCharmInfo() override;

        void  SetTP(int32 TP);
        int32 GetDispTP() const;

        int32   m_TrainingPoints;
        uint32  m_resetTalentsCost;
        time_t  m_resetTalentsTime;

        // overwrite Creature function for name localization back to WorldObject version without localization
        const char* GetNameForLocaleIdx(int32 locale_idx) const override { return WorldObject::GetNameForLocaleIdx(locale_idx); }

        DeclinedName const* GetDeclinedNames() const { return m_declinedname; }

        void SetRequiredXpForNextLoyaltyLevel();
        void UpdateRequireXpForNextLoyaltyLevel(uint32 xp);

        bool    m_removed;                                  // prevent overwrite pet state in DB at next Pet::Update if pet already removed(saved)

        // return charminfo ai only when this pet is possessed. (eye of the beast case for ex.)
        virtual UnitAI* AI() override
        {
            if (hasUnitState(UNIT_STAT_POSSESSED) && m_charmInfo->GetAI()) return m_charmInfo->GetAI();
            return m_ai.get();
        }

        virtual CombatData* GetCombatData() override { return m_combatData; }
        virtual CombatData const* GetCombatData() const override { return m_combatData; }

        virtual void RegenerateHealth() override;

        void ResetCorpseRespawn();

        void ForcedDespawn(uint32 timeMSToDespawn = 0, bool onlyAlive = false) override;

        bool IgnoresOwnersDeath() const;
    protected:
        uint32  m_happinessTimer;
        uint32  m_loyaltyTimer;
        PetType m_petType;
        int32   m_duration;                                 // time until unsummon (used mostly for summoned guardians and not used for controlled pets)
        int32   m_loyaltyPoints;
        bool    m_loading;
        uint32  m_xpRequiredForNextLoyaltyLevel;
        DeclinedName* m_declinedname;

    private:
        PetModeFlags m_petModeFlags;
        CharmInfo*   m_originalCharminfo;
        bool m_inStatsUpdate;

        void SaveToDB(uint32, uint8) override               // overwrited of Creature::SaveToDB     - don't must be called
        {
            MANGOS_ASSERT(false);
        }
        void DeleteFromDB() override                        // overwrite of Creature::DeleteFromDB - don't must be called
        {
            MANGOS_ASSERT(false);
        }
};
#endif
