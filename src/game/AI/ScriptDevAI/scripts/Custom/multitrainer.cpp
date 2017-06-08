// Multitrainer (Universal Class Trainer)
// Smolderforge 2015-2020
// Author: Henhouse

#include "AI/ScriptDevAI/ScriptDevAIMgr.h"
#include "AI/ScriptDevAI/include/sc_gossip.h"
#include "beastmaster.h"

bool GossipHello_custom_multitrainer(Player* player, Creature* creature)
{
    if (creature->GetCreatureInfo()->ExtraFlags & CREATURE_EXTRA_FLAG_CUSTOM_TRAINER)  // check if multi trainer first
    {
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GOSSIP_TEXT_TRAIN, GOSSIP_SENDER_MAIN, player->getClass());

        // morph based on class
        switch (player->getClass())
        {
        case CLASS_WARRIOR:
            creature->SetDisplayId(24036);
            break;
        case CLASS_PALADIN:
            creature->SetDisplayId(24032);
            break;
        case CLASS_HUNTER:
            creature->SetDisplayId(24030);
            break;
        case CLASS_ROGUE:
            creature->SetDisplayId(23777);
            break;
        case CLASS_PRIEST:
            creature->SetDisplayId(24033);
            break;
        case CLASS_SHAMAN:
            creature->SetDisplayId(24034);
            break;
        case CLASS_MAGE:
            creature->SetDisplayId(24031);
            break;
        case CLASS_WARLOCK:
            creature->SetDisplayId(24035);
            break;
        case CLASS_DRUID:
            creature->SetDisplayId(24029);
            break;
        }
    }
    else if (creature->isTrainer())
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GOSSIP_TEXT_TRAIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1000);

    if (creature->CanTrainAndResetTalentsOf(player))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "I wish to unlearn my talents", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1001);

    if (player->getClass() == CLASS_HUNTER)
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Tame a pet!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);

    uint8 specCount = player->GetSpecsCount();
    if (specCount < MAX_TALENT_SPECS)
    {
        player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, "Enable Dual Talent Specialization", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 0, "Are you sure you would like to activate your second specialization? This will allow you to quickly switch between two different talent builds and action bars.", 0, false);
    }
    else
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Change my specialization", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);

    if (creature->isQuestGiver())
        player->PrepareQuestMenu(creature->GetObjectGuid());

    player->SEND_GOSSIP_MENU(42300, creature->GetObjectGuid());
    return true;
}

bool GossipSelect_custom_multitrainer(Player* player, Creature* creature, uint32 sender, uint32 action)
{
    uint32 trainerEntryId = 0;
    switch (action)
    {
        // multi trainer
        case CLASS_WARRIOR:
            trainerEntryId = 11;
            break;
        case CLASS_PALADIN:
            trainerEntryId = 21;
            break;
        case CLASS_HUNTER:
            trainerEntryId = 31;
            break;
        case CLASS_ROGUE:
            trainerEntryId = 41;
            break;
        case CLASS_PRIEST:
            trainerEntryId = 51;
            break;
        case CLASS_SHAMAN:
            trainerEntryId = 61;
            break;
        case CLASS_MAGE:
            trainerEntryId = 71;
            break;
        case CLASS_WARLOCK:
            trainerEntryId = 81;
            break;
        case CLASS_DRUID:
            trainerEntryId = 91;
            break;

        // dual spec
        case GOSSIP_ACTION_INFO_DEF + 0:
            player->SetSpecsCount(player->GetSpecsCount() + 1);
            GossipSelect_custom_multitrainer(player, creature, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            break;
        case GOSSIP_ACTION_INFO_DEF + 1:
        {
            if (player->GetActiveSpec() == 0)
            {
                player->CLOSE_GOSSIP_MENU();
                player->GetSession()->SendNotification("You are already on that spec.");
                GossipSelect_custom_multitrainer(player, creature, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
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
                GossipSelect_custom_multitrainer(player, creature, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
                break;
            }
            player->ActivateSpec(1);
            break;
        }
        case GOSSIP_ACTION_INFO_DEF + 5: // swap menus & rename
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
            player->SEND_GOSSIP_MENU(42301, creature->GetObjectGuid());
            break;
        }
        case GOSSIP_ACTION_INFO_DEF + 6: // tame pet, switch script
            player->CLOSE_GOSSIP_MENU();
            GossipSelect_beastmaster(player, creature, GOSSIP_SENDER_MAIN, 100);
            break;

        // beastmaster pet cases
        case 5000:
        case 5001:
        case 5002:
        case 5003:
        case 5004:
        case 5005:
        case 5006:
        case 5007:
        case 5008:
        case 5009:
        case 5010:
        case 5011:
        case 5012:
        case 5013:
        case 5014:
        case 5015:
            GossipSelect_beastmaster(player, creature, GOSSIP_SENDER_MAIN, action);
            break;

        case GOSSIP_ACTION_INFO_DEF+1000:
            player->SEND_TRAINERLIST(creature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+1001:
            player->CLOSE_GOSSIP_MENU();
            player->SendTalentWipeConfirm(creature->GetObjectGuid());
            break;
        default:
            sLog.outString("ActionId: %u", action);
            break;
    }

    if (trainerEntryId)
    {
        player->m_currentTrainerTemplate = trainerEntryId;
        player->GetSession()->SendTrainerList(creature->GetObjectGuid(), trainerEntryId);
    }

    return true;
}

bool GossipSelectWithCode_custom_multitrainer(Player* player, Creature* creature, uint32 sender, uint32 action, const char* sCode)
{
    std::string strCode = sCode;
    CharacterDatabase.escape_string(strCode);

    if (action == GOSSIP_ACTION_INFO_DEF + 10)
        player->SetSpecName(0, strCode.c_str());
    else if (action == GOSSIP_ACTION_INFO_DEF + 11)
        player->SetSpecName(1, strCode.c_str());

    player->CLOSE_GOSSIP_MENU();
    GossipSelect_custom_multitrainer(player, creature, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
    return true;
}

void AddSC_multitrainer()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "multitrainer";
    pNewScript->pGossipHello = &GossipHello_custom_multitrainer;
    pNewScript->pGossipSelect = &GossipSelect_custom_multitrainer;
    pNewScript->pGossipSelectWithCode = &GossipSelectWithCode_custom_multitrainer;
    pNewScript->RegisterSelf();
}
