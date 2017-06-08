/*
* ScriptData
* SDName:      item_custom_dualspec
* SD%Complete: 100
* SDComment:
* EndScriptData
*/

#include "AI/ScriptDevAI/ScriptDevAIMgr.h"
#include "AI/ScriptDevAI/include/sc_gossip.h"

enum
{
    TEXT_ID_GEM = 50701,
};

bool GossipItemUse_custom_dualspec(Player* pPlayer, Item* pItem, const SpellCastTargets& /*pTargets*/)
{
    pPlayer->GetPlayerMenu()->ClearMenus();

    if (pPlayer->IsInCombat())
    {
        pPlayer->GetSession()->SendNotification("You are in combat and cannot switch spec at this time.");
        return false;
    }

    if (pPlayer->InBattleGround() || pPlayer->InArena() || pPlayer->GetMap()->IsDungeon() || pPlayer->GetMap()->IsRaid())
    {
        pPlayer->GetSession()->SendNotification("You must exit the instance to re-spec.");
        return false;
    }

    if (pPlayer->IsFlying() || pPlayer->IsTaxiFlying() || pPlayer->IsMounted())
    {
        pPlayer->GetSession()->SendNotification("You are mounted and cannot switch spec at this time.");
        return false;
    }

    if (pPlayer->IsDead())
    {
        pPlayer->GetSession()->SendNotification("You are dead and cannot switch spec at this time.");
        return false;
    }

    if (pPlayer->GetSpecsCount() < MAX_TALENT_SPECS)
    {
        pPlayer->GetSession()->SendNotification("You must unlock the dual talent specialization feature first.");
        return false;
    }

    if (pPlayer->GetLevel() < 10)
    {
        pPlayer->GetSession()->SendNotification("Current level is less than ten - you cannot switch spec at this time.");
        return false;
    }

    uint8 specCount = pPlayer->GetSpecsCount();
    for (uint8 i = 0; i < specCount; ++i)
    {
        std::stringstream specNameString;
        specNameString << "|cFF0041FF[Activate] ";
        if (pPlayer->GetSpecName(i) == "NULL")
            specNameString << " [unnamed] ";
        else
            specNameString << pPlayer->GetSpecName(i);
        if (i == pPlayer->GetActiveSpec())
            specNameString << " (active) ";
        else
            specNameString << "";

        specNameString << "|r";
 
        pPlayer->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_BATTLE, specNameString.str(), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + (1 + i), "Are you sure you wish to switch your talent specialization?", 0, false);
    }

    for (uint8 i = 0; i < specCount; ++i)
    {
        std::stringstream specNameString;
        specNameString << "|cFFCC00CC[Rename] ";
        if (pPlayer->GetSpecName(i) == "NULL")
            specNameString << " [unnamed] ";
        else
            specNameString << pPlayer->GetSpecName(i);

        specNameString << "|r";

        pPlayer->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_BATTLE, specNameString.str(), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + (10 + i), "", 0, true);
    }

    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_GEM, pItem->GetObjectGuid());
    return true;
}

bool GossipSelectItem_custom_dualspec(Player* pPlayer, Item* pItem, uint32 sender, uint32 action)
{
    switch (action)
    {
        case GOSSIP_ACTION_INFO_DEF + 1:
        {
            pPlayer->ActivateSpec(0);
            break;
        }
        case GOSSIP_ACTION_INFO_DEF + 2:
        {
            pPlayer->ActivateSpec(1);
            break;
        }
        case GOSSIP_ACTION_INFO_DEF + 999:
        {
            pPlayer->CLOSE_GOSSIP_MENU();
            break;
        }
        default:
            break;
    }

    return true;
}

bool GossipSelectItemWithCode_custom_dualspec(Player* pPlayer, Item* pItem, uint32 sender, uint32 action, const char* sCode)
{
    if (action == GOSSIP_ACTION_INFO_DEF + 10)
        pPlayer->SetSpecName(0, sCode);
    else if (action == GOSSIP_ACTION_INFO_DEF + 11)
        pPlayer->SetSpecName(1, sCode);

    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}

void AddSC_item_custom_dualspec()
{
    Script* pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "item_custom_dualspec";
    pNewScript->pItemUse = &GossipItemUse_custom_dualspec;
    pNewScript->pGossipSelectItem = &GossipSelectItem_custom_dualspec;
    pNewScript->pGossipSelectItemWithCode = &GossipSelectItemWithCode_custom_dualspec;
    pNewScript->RegisterSelf(false);
}
