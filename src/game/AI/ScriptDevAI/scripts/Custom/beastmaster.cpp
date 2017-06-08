// Beastmaster NPC
// Smolderforge 2013-2020
// Author: Henhouse

#include "beastmaster.h"
#include "Globals/ObjectMgr.h"

void learnAllTrainingSpells(Player* player)
{
    player->learnSpell(27348, false); // bite
    player->learnSpell(28343, false); // charge
    player->learnSpell(27347, false); // claw
    player->learnSpell(27346, false); // cower
    player->learnSpell(23112, false); // dash
    player->learnSpell(23150, false); // dive
    player->learnSpell(35324, false); // fire breath
    player->learnSpell(24599, false); // furious howl
    player->learnSpell(35308, false); // gore
    player->learnSpell(25017, false); // lightning breath
    player->learnSpell(35391, false); // poison spit
    player->learnSpell(24455, false); // prowl
    player->learnSpell(27361, false); // scorpid poison
    player->learnSpell(27349, false); // screech
    player->learnSpell(26065, false); // spell shield
    player->learnSpell(27366, false); // thunderstomp
    player->learnSpell(35348, false); // warp
    player->learnSpell(27350, false); // arcane res
    player->learnSpell(27353, false); // shadow res
    player->learnSpell(27351, false); // fire res
    player->learnSpell(27352, false); // frost res
    player->learnSpell(27354, false); // nature res
    player->learnSpell(27362, false); // natural armor
    player->learnSpell(27364, false); // great stamina
    player->learnSpell(35700, false); // avoidance
    player->learnSpell(25077, false); // cobra reflexes
}

void CreatePet(Player* player, Creature* creature, uint32 entry)
{
    if (player->GetPet())
    {
        player->GetSession()->SendNotification("You already have a pet!");
        player->CLOSE_GOSSIP_MENU();
        return;
    }

    Creature* creatureTarget = creature->SummonCreature(entry, player->GetPositionX(), player->GetPositionY() + 2, player->GetPositionZ(), player->GetOrientation(), TEMPSPAWN_TIMED_DESPAWN, 500);
    if (!creatureTarget)
        return;

    creatureTarget->SetLevel(player->GetLevel());

    Pet* pet = new Pet(HUNTER_PET);

    if (!pet->CreateBaseAtCreature(creatureTarget))
    {
        delete pet;
        return;
    }

    pet->SetUInt32Value(UNIT_CREATED_BY_SPELL, 13481); // tame beast

    pet->SetOwnerGuid(player->GetObjectGuid());
    pet->setFaction(player->GetFaction());

    if (player->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED))
    {
        pet->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);
        pet->SetByteValue(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_PLAYER_CONTROLLED_DEBUFF_LIMIT);
    }
    else
        pet->SetByteValue(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_CREATURE_DEBUFF_LIMIT);

    if (player->IsImmuneToNPC())
        pet->SetImmuneToNPC(true);

    if (player->IsImmuneToPlayer())
        pet->SetImmuneToPlayer(true);

    if (player->IsPvP())
        pet->SetPvP(true);

    pet->GetCharmInfo()->SetPetNumber(sObjectMgr.GeneratePetNumber(), true);

    uint32 level = creatureTarget->GetLevel();
    pet->SetCanModifyStats(true);
    pet->InitStatsForLevel(level);

    pet->SetHealthPercent(creatureTarget->GetHealthPercent());

    // destroy creature object
    creatureTarget->ForcedDespawn();

    // prepare visual effect for levelup
    pet->SetUInt32Value(UNIT_FIELD_LEVEL, level - 1);

    // add pet object to the world
    pet->GetMap()->Add((Creature*)pet);
    pet->AIM_Initialize();

    pet->AI()->SetReactState(REACT_DEFENSIVE);

    // visual effect for levelup
    pet->SetUInt32Value(UNIT_FIELD_LEVEL, level);

    // enable abandon prompt, but not rename (intentional)
    pet->SetByteFlag(UNIT_FIELD_BYTES_2, 2, UNIT_CAN_BE_ABANDONED);

    // this enables pet details window (Shift+P)
    pet->InitPetCreateSpells();

    pet->LearnPetPassives();
    pet->CastPetAuras(true);
    pet->CastOwnerTalentAuras();
    pet->UpdateAllStats();

    // start with maximum loyalty and training points
    pet->SetLoyaltyLevel(BEST_FRIEND);
    pet->ModifyLoyalty(26500);
    pet->SetTP(level * (BEST_FRIEND - 1));
    pet->SetPower(POWER_HAPPINESS, 1050000);

    pet->SetRequiredXpForNextLoyaltyLevel();

    // caster have pet now
    player->SetPet(pet);

    player->PetSpellInitialize();

    pet->SavePetToDB(PET_SAVE_AS_CURRENT, player);

    player->CLOSE_GOSSIP_MENU();
    player->GetSession()->SendAreaTriggerMessage("Pet tamed successfully.");
}

bool GossipHello_beastmaster(Player* player, Creature* creature)
{
    if (creature->isTrainer())
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GOSSIP_TEXT_TRAIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRAIN);

    if (creature->CanTrainAndResetTalentsOf(player))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "I wish to unlearn my talents", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_UNLEARN);

    if (player->getClass() == CLASS_HUNTER)
    {
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Tame a pet!", GOSSIP_SENDER_MAIN, 100);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Access stable", GOSSIP_SENDER_MAIN, 101);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Untrain pet", GOSSIP_SENDER_MAIN, 102);
        player->SEND_GOSSIP_MENU(42312, creature->GetObjectGuid());
    }
    else
    {
        player->SEND_GOSSIP_MENU(42311, creature->GetObjectGuid());
    }

    return true;
}

bool GossipSelect_beastmaster(Player* player, Creature* creature, uint32 sender, uint32 action)
{
    switch (action)
    {
    case 100:
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, GOSSIP_TEXT_BAT, GOSSIP_SENDER_MAIN, 5001);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, GOSSIP_TEXT_BOAR, GOSSIP_SENDER_MAIN, 5003);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, GOSSIP_TEXT_BIRD, GOSSIP_SENDER_MAIN, 5002);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, GOSSIP_TEXT_CAT, GOSSIP_SENDER_MAIN, 5004);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, GOSSIP_TEXT_DRAGONHAWK, GOSSIP_SENDER_MAIN, 5005);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, GOSSIP_TEXT_GORILLA, GOSSIP_SENDER_MAIN, 5006);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, GOSSIP_TEXT_OWL, GOSSIP_SENDER_MAIN, 5007);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, GOSSIP_TEXT_RAPTOR, GOSSIP_SENDER_MAIN, 5008);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, GOSSIP_TEXT_RAVAGER, GOSSIP_SENDER_MAIN, 5009);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, GOSSIP_TEXT_SCORPID, GOSSIP_SENDER_MAIN, 5010);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, GOSSIP_TEXT_SERPENT, GOSSIP_SENDER_MAIN, 5011);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, GOSSIP_TEXT_TURTLE, GOSSIP_SENDER_MAIN, 5012);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, GOSSIP_TEXT_WARPCHASER, GOSSIP_SENDER_MAIN, 5013);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, GOSSIP_TEXT_W_SERPENT, GOSSIP_SENDER_MAIN, 5014);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, GOSSIP_TEXT_WOLF, GOSSIP_SENDER_MAIN, 5015);

        // make sure player has all training spells
        learnAllTrainingSpells(player);

        player->SEND_GOSSIP_MENU(42302, creature->GetObjectGuid());
        break;
    case 101:
        player->GetSession()->SendStablePet(player->GetObjectGuid());
        break;
    case 102: // untrain pet
        player->GetPlayerMenu()->CloseGossip();
        player->SendPetSkillWipeConfirm();
        break;
    case 5001:
        CreatePet(player, creature, PET_BAT);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 5002:
        CreatePet(player, creature, PET_BIRD);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 5003:
        CreatePet(player, creature, PET_BOAR);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 5004:
        CreatePet(player, creature, PET_CAT);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 5005:
        CreatePet(player, creature, PET_DRAGON_HAWK);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 5006:
        CreatePet(player, creature, PET_GORILLA);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 5007:
        CreatePet(player, creature, PET_OWL);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 5008:
        CreatePet(player, creature, PET_RAPTOR);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 5009:
        CreatePet(player, creature, PET_RAVAGER);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 5010:
        CreatePet(player, creature, PET_SCORPID);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 5011:
        CreatePet(player, creature, PET_SERPENT);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 5012:
        CreatePet(player, creature, PET_TURTLE);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 5013:
        CreatePet(player, creature, PET_WARP_CHASER);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 5014:
        CreatePet(player, creature, PET_WIND_SERPENT);
        player->CLOSE_GOSSIP_MENU();
        break;
    case 5015:
        CreatePet(player, creature, PET_WOLF);
        player->CLOSE_GOSSIP_MENU();
        break;

    case GOSSIP_ACTION_TRAIN:
        player->SEND_TRAINERLIST(creature->GetObjectGuid());
        break;
    case GOSSIP_ACTION_UNLEARN:
        player->CLOSE_GOSSIP_MENU();
        player->SendTalentWipeConfirm(creature->GetObjectGuid());
        break;
    }

    return true;
}

void AddSC_beastmaster()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "beastmaster";
    newscript->pGossipHello = &GossipHello_beastmaster;
    newscript->pGossipSelect = &GossipSelect_beastmaster;
    newscript->RegisterSelf();
}
