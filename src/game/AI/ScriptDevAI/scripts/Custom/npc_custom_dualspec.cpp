/*
* ScriptData
* SDName:      npc_custom_dualspec
* SD%Complete: 100
* SDComment:
* EndScriptData
*/

#include "AI/ScriptDevAI/ScriptDevAIMgr.h"
#include "AI/ScriptDevAI/include/sc_gossip.h"

enum
{
    TEXT_ID_CRYSTAL = 50700,
    DUALSPEC_COST = 10000000,
};

bool GossipHello_custom_dualspec(Player* player, Creature* creature)
{
    uint8 specCount = player->GetSpecsCount();
    if (specCount < MAX_TALENT_SPECS)
    {
        player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, "Purchase Dual Talent Specialization", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 0, "Are you sure you would like to activate your second specialization for 1,000 gold? This will allow you to quickly switch between two different talent builds and action bars.", DUALSPEC_COST, false);
    }
    else
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Change my specialization", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);

    player->SEND_GOSSIP_MENU(TEXT_ID_CRYSTAL, creature->GetObjectGuid());
    return true;
}

bool GossipSelect_custom_dualspec(Player* player, Creature* creature, uint32 sender, uint32 action)
{
    switch (action)
    {
        case GOSSIP_ACTION_INFO_DEF + 0:
        {
            if (player->GetMoney() >= DUALSPEC_COST)
            {
                player->ModifyMoney(-DUALSPEC_COST);
                player->SetSpecsCount(player->GetSpecsCount() + 1);
                player->SavePlayerSetting(PLAYER_SETTING_SPEC_COUNT);
                GossipSelect_custom_dualspec(player, creature, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            }
            else
            {
                player->GetSession()->SendNotification("You don't have enough money to unlock dual specialization.");
            }
            break;
        }
        case GOSSIP_ACTION_INFO_DEF + 1:
        {
            if (player->GetActiveSpec() == 0)
            {
                player->CLOSE_GOSSIP_MENU();
                player->GetSession()->SendNotification("You are already on that spec.");
                GossipSelect_custom_dualspec(player, creature, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
                break;
            }
            player->ActivateSpec(0);
            break;
        }
        case GOSSIP_ACTION_INFO_DEF + 2:
        {
            if (player->GetActiveSpec() == 1)
            {
                player->CLOSE_GOSSIP_MENU();
                player->GetSession()->SendNotification("You are already on that spec.");
                GossipSelect_custom_dualspec(player, creature, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
                break;
            }
            player->ActivateSpec(1);
            break;
        }
        case GOSSIP_ACTION_INFO_DEF + 5:
        {
            uint8 specCount = player->GetSpecsCount();
            for (uint8 i = 0; i < specCount; ++i)
            {
                std::stringstream specNameString;
                specNameString << "[Activate] ";
                if (player->GetSpecName(i) == "NULL")
                    specNameString << "Unnamed";
                else
                    specNameString << player->GetSpecName(i);
                if (i == player->GetActiveSpec())
                    specNameString << " (active)";
                else
                    specNameString << "";
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, specNameString.str(), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + (1 + i));
            }

            for (uint8 i = 0; i < specCount; ++i)
            {
                std::stringstream specNameString;
                specNameString << "[Rename] ";
                if (player->GetSpecName(i) == "NULL")
                    specNameString << "Unnamed";
                else
                    specNameString << player->GetSpecName(i);
                if (i == player->GetActiveSpec())
                    specNameString << " (active)";
                else
                    specNameString << "";
                player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_TALK, specNameString.str(), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + (10 + i), "", 0, true);
            }
            player->SEND_GOSSIP_MENU(TEXT_ID_CRYSTAL, creature->GetObjectGuid());
            break;
        }
    }
    return true;
}

bool GossipSelectWithCode_custom_dualspec(Player* player, Creature* creature, uint32 sender, uint32 action, const char* sCode)
{
    std::string strCode = sCode;
    CharacterDatabase.escape_string(strCode);

    if (action == GOSSIP_ACTION_INFO_DEF + 10)
        player->SetSpecName(0, strCode.c_str());
    else if (action == GOSSIP_ACTION_INFO_DEF + 11)
        player->SetSpecName(1, strCode.c_str());

    player->CLOSE_GOSSIP_MENU();
    GossipSelect_custom_dualspec(player, creature, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
    return true;
}

void AddSC_npc_custom_dualspec()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_custom_dualspec";
    pNewScript->pGossipHello = &GossipHello_custom_dualspec;
    pNewScript->pGossipSelect = &GossipSelect_custom_dualspec;
    pNewScript->pGossipSelectWithCode = &GossipSelectWithCode_custom_dualspec;
    pNewScript->RegisterSelf();
}
