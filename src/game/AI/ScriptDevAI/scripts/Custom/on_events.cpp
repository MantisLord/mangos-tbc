#include "AI/ScriptDevAI/include/sc_common.h"
#include "World/World.h"

void OnLogin(Player* player)
{
    if (sWorld.getConfig(CONFIG_BOOL_START_MAX_FIRST_AID))
    {
        if (SkillLineEntry const* sl = sSkillLineStore.LookupEntry(SKILL_FIRST_AID))
        {
            uint32 maxSkill = player->GetSkillMaxPure(SKILL_FIRST_AID);
            if (player->GetSkillValue(SKILL_FIRST_AID) < maxSkill)
                player->SetSkill(SKILL_FIRST_AID, maxSkill, maxSkill);
        }
    }

    if (sWorld.getConfig(CONFIG_BOOL_START_HUNTER_AMMO_REPUTATIONS) && player->getClass() == CLASS_HUNTER)
    {
        player->GetReputationMgr().SetReputation(sFactionStore.LookupEntry<FactionEntry>(967), 21000); // Violet Eye
        player->GetReputationMgr().SetReputation(sFactionStore.LookupEntry<FactionEntry>(990), 9000); // Scale of the Sands
    }

    player->LoadPlayerSetting(PLAYER_SETTING_CITY_PROTECTOR);
    player->LoadPlayerSetting(PLAYER_SETTING_XP_MODIFIER);
}

void OnLogout(Player* player)
{
    for (int i = 1; i < MAX_PLAYER_SETTING; i++)
        player->SavePlayerSetting((PlayerSettings)i);
}

void OnPVPKill(Player* killer, Player *killed)
{
}

 void AddSC_onevents()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "scripted_on_events";
    newscript->pOnLogin = &OnLogin;
    newscript->pOnLogout = &OnLogout;
    newscript->pOnPVPKill = &OnPVPKill;

    newscript->RegisterSelf();
}
