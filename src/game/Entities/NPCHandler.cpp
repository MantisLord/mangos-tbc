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

#include "Common.h"
#include "Tools/Language.h"
#include "Database/DatabaseEnv.h"
#include "Server/WorldPacket.h"
#include "Server/WorldSession.h"
#include "Server/Opcodes.h"
#include "Log/Log.h"
#include "Globals/ObjectMgr.h"
#include "AI/ScriptDevAI/ScriptDevAIMgr.h"
#include "Entities/Player.h"
#include "Entities/GossipDef.h"
#include "DBScripts/ScriptMgr.h"
#include "Entities/Creature.h"
#include "Entities/Pet.h"
#include "Guilds/Guild.h"
#include "Guilds/GuildMgr.h"
#include "Chat/Chat.h"

enum StableResultCode
{
    STABLE_ERR_MONEY        = 0x01,                         // "you don't have enough money"
    STABLE_ERR_STABLE       = 0x06,                         // currently used in most fail cases
    STABLE_SUCCESS_STABLE   = 0x08,                         // stable success
    STABLE_SUCCESS_UNSTABLE = 0x09,                         // unstable/swap success
    STABLE_SUCCESS_BUY_SLOT = 0x0A,                         // buy slot success
};

void WorldSession::HandleTabardVendorActivateOpcode(WorldPacket& recv_data)
{
    ObjectGuid guid;
    recv_data >> guid;

    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_TABARDDESIGNER);
    if (!unit)
    {
        DEBUG_LOG("WORLD: HandleTabardVendorActivateOpcode - %s not found or you can't interact with him.", guid.GetString().c_str());
        return;
    }

    SendTabardVendorActivate(guid);
}

void WorldSession::SendTabardVendorActivate(ObjectGuid guid) const
{
    WorldPacket data(MSG_TABARDVENDOR_ACTIVATE, 8);
    data << ObjectGuid(guid);
    SendPacket(data);
}

void WorldSession::HandleBankerActivateOpcode(WorldPacket& recv_data)
{
    ObjectGuid guid;

    DEBUG_LOG("WORLD: Received opcode CMSG_BANKER_ACTIVATE");

    recv_data >> guid;

    if (!CheckBanker(guid))
        return;

    SendShowBank(guid);
}

void WorldSession::SendShowBank(ObjectGuid guid) const
{
    WorldPacket data(SMSG_SHOW_BANK, 8);
    data << ObjectGuid(guid);
    SendPacket(data);
}

void WorldSession::HandleTrainerListOpcode(WorldPacket& recv_data)
{
    ObjectGuid guid;

    recv_data >> guid;

    SendTrainerList(guid);
}


static void SendTrainerSpellHelper(WorldPacket& data, TrainerSpell const* tSpell, TrainerSpellState state, float fDiscountMod, bool /*can_learn_primary_prof*/, uint32 reqLevel)
{
    bool primary_prof_first_rank = sSpellMgr.IsPrimaryProfessionFirstRankSpell(tSpell->learnedSpell);

    SpellChainNode const* chain_node = sSpellMgr.GetSpellChainNode(tSpell->learnedSpell);

    data << uint32(tSpell->spell);                      // learned spell (or cast-spell in profession case)
    data << uint8(state == TRAINER_SPELL_GREEN_DISABLED ? TRAINER_SPELL_GREEN : state);
    data << uint32(floor(tSpell->spellCost * fDiscountMod));

    data << uint32(0); // spells don't cost talent points
    data << uint32(primary_prof_first_rank ? 1 : 0);    // must be equal prev. field to have learn button in enabled state
    data << uint8(reqLevel);
    data << uint32(tSpell->reqSkill);
    data << uint32(tSpell->reqSkillValue);
    if (tSpell->reqAbility[0])
        data << *tSpell->reqAbility[0];
    else
        data << uint32(!tSpell->IsCastable() && chain_node ? (chain_node->prev ? chain_node->prev : chain_node->req) : 0);
    if (tSpell->reqAbility[1])
        data << *tSpell->reqAbility[1];
    else
        data << uint32(!tSpell->IsCastable() && chain_node && chain_node->prev ? chain_node->req : 0);
    if (tSpell->reqAbility[2])
        data << *tSpell->reqAbility[2];
    else
        data << uint32(0);
}

void WorldSession::SendTrainerList(ObjectGuid guid, uint32 entry) const
{
    DEBUG_LOG("WORLD: SendTrainerList");

    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_TRAINER);
    if (!unit)
    {
        DEBUG_LOG("WORLD: SendTrainerList - %s not found or you can't interact with him.", guid.GetString().c_str());
        return;
    }

    // trainer list loaded at check;
    if (!unit->IsTrainerOf(_player, true))
        return;

    CreatureInfo const* ci = unit->GetCreatureInfo();
    if (!ci)
        return;

    TrainerSpellData const* cSpells = unit->GetTrainerSpells();
    TrainerSpellData const* tSpells = entry ? sObjectMgr.GetNpcTrainerTemplateSpells(entry) : unit->GetTrainerTemplateSpells();

    if (!cSpells && !tSpells)
    {
        DEBUG_LOG("WORLD: SendTrainerList - Training spells not found for %s", guid.GetString().c_str());
        return;
    }

    uint32 maxcount = (cSpells ? cSpells->spellList.size() : 0) + (tSpells ? tSpells->spellList.size() : 0);
    uint32 trainer_type = cSpells && cSpells->trainerType ? cSpells->trainerType : (tSpells ? tSpells->trainerType : 0);

    std::string strTitle;
    if (TrainerGreeting const* data = sObjectMgr.GetTrainerGreetingData(guid.GetEntry()))
    {
        strTitle = data->text;
        int loc_idx = GetSessionDbLocaleIndex();
        sObjectMgr.GetTrainerGreetingLocales(guid.GetEntry(), loc_idx, &strTitle);
    }
    else
        strTitle = GetMangosString(LANG_NPC_TAINER_HELLO); // Fallback in case no trainer greeting found

    WorldPacket data(SMSG_TRAINER_LIST, 8 + 4 + 4 + maxcount * 38 + strTitle.size() + 1);
    data << ObjectGuid(guid);
    data << uint32(trainer_type);

    size_t count_pos = data.wpos();
    data << uint32(maxcount);

    // reputation discount
    float fDiscountMod = _player->GetReputationPriceDiscount(unit);
    bool can_learn_primary_prof = GetPlayer()->GetFreePrimaryProfessionPoints() > 0;

    uint32 count = 0;

    if (cSpells)
    {
        for (const auto& itr : cSpells->spellList)
        {
            TrainerSpell const* tSpell = &itr.second;

            uint32 reqLevel = 0;
            if (!_player->IsSpellFitByClassAndRace(tSpell->learnedSpell, &reqLevel))
                continue;

            if (tSpell->conditionId && !sObjectMgr.IsConditionSatisfied(tSpell->conditionId, GetPlayer(), unit->GetMap(), unit, CONDITION_FROM_TRAINER))
                continue;

            reqLevel = tSpell->isProvidedReqLevel ? tSpell->reqLevel : std::max(reqLevel, tSpell->reqLevel);

            TrainerSpellState state = _player->GetTrainerSpellState(tSpell, reqLevel);

            SendTrainerSpellHelper(data, tSpell, state, fDiscountMod, can_learn_primary_prof, reqLevel);

            ++count;
        }
    }

    if (tSpells)
    {
        for (const auto& itr : tSpells->spellList)
        {
            TrainerSpell const* tSpell = &itr.second;

            uint32 reqLevel = 0;
            if (!_player->IsSpellFitByClassAndRace(tSpell->learnedSpell, &reqLevel))
                continue;

            if (tSpell->conditionId && !sObjectMgr.IsConditionSatisfied(tSpell->conditionId, GetPlayer(), unit->GetMap(), unit, CONDITION_FROM_TRAINER))
                continue;

            reqLevel = tSpell->isProvidedReqLevel ? tSpell->reqLevel : std::max(reqLevel, tSpell->reqLevel);

            TrainerSpellState state = _player->GetTrainerSpellState(tSpell, reqLevel);

            SendTrainerSpellHelper(data, tSpell, state, fDiscountMod, can_learn_primary_prof, reqLevel);

            ++count;
        }
    }

    data << strTitle;

    data.put<uint32>(count_pos, count);
    SendPacket(data);
}

void WorldSession::HandleTrainerBuySpellOpcode(WorldPacket& recv_data)
{
    ObjectGuid guid;
    uint32 spellId = 0;

    recv_data >> guid >> spellId;
    DEBUG_LOG("WORLD: Received opcode CMSG_TRAINER_BUY_SPELL Trainer: %s, learn spell id is: %u", guid.GetString().c_str(), spellId);

    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_TRAINER);
    if (!unit)
    {
        DEBUG_LOG("WORLD: HandleTrainerBuySpellOpcode - %s not found or you can't interact with him.", guid.GetString().c_str());
        return;
    }

    if (!unit->IsTrainerOf(_player, true))
        return;

    // check present spell in trainer spell list
    TrainerSpellData const* cSpells = unit->GetTrainerSpells();
    TrainerSpellData const* tSpells;

    // Multigossip Trainer
    if (unit->GetCreatureInfo()->ExtraFlags & CREATURE_EXTRA_FLAG_CUSTOM_TRAINER)
        tSpells = sObjectMgr.GetNpcTrainerTemplateSpells(GetPlayer()->m_currentTrainerTemplate);
    else
        tSpells = unit->GetTrainerTemplateSpells();

    if (!cSpells && !tSpells)
        return;

    // Try find spell in npc_trainer
    TrainerSpell const* trainer_spell = cSpells ? cSpells->Find(spellId) : nullptr;

    // Not found, try find in npc_trainer_template
    if (!trainer_spell && tSpells)
        trainer_spell = tSpells->Find(spellId);

    // Not found anywhere, cheating?
    if (!trainer_spell)
        return;

    // can't be learn, cheat? Or double learn with lags...
    uint32 reqLevel = 0;
    if (!_player->IsSpellFitByClassAndRace(trainer_spell->learnedSpell, &reqLevel))
        return;

    reqLevel = trainer_spell->isProvidedReqLevel ? trainer_spell->reqLevel : std::max(reqLevel, trainer_spell->reqLevel);
    if (_player->GetTrainerSpellState(trainer_spell, reqLevel) != TRAINER_SPELL_GREEN)
        return;

    // apply reputation discount
    uint32 nSpellCost = uint32(floor(trainer_spell->spellCost * _player->GetReputationPriceDiscount(unit)));

    // check money requirement
    if (_player->GetMoney() < nSpellCost)
        return;

    _player->ModifyMoney(-int32(nSpellCost));

    SendPlaySpellVisual(guid, 0xB3);                        // visual effect on trainer

    WorldPacket data(SMSG_PLAY_SPELL_IMPACT, 8 + 4);        // visual effect on player
    data << _player->GetObjectGuid();
    data << uint32(0x016A);                                 // index from SpellVisualKit.dbc
    SendPacket(data);

    // learn explicitly or cast explicitly
    // TODO - Are these spells really cast correctly this way?
    if (trainer_spell->IsCastable())
        _player->CastSpell(_player, trainer_spell->spell, TRIGGERED_OLD_TRIGGERED);
    else
        _player->learnSpell(spellId, false);

    data.Initialize(SMSG_TRAINER_BUY_SUCCEEDED, 12);
    data << ObjectGuid(guid);
    data << uint32(spellId);                                // should be same as in packet from client
    SendPacket(data);
}

void WorldSession::HandleGossipHelloOpcode(WorldPacket& recv_data)
{
    DEBUG_LOG("WORLD: Received opcode CMSG_GOSSIP_HELLO");

    ObjectGuid guid;
    recv_data >> guid;

    Creature* pCreature = GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_NONE);
    if (!pCreature)
    {
        DEBUG_LOG("WORLD: HandleGossipHelloOpcode - %s not found or you can't interact with him.", guid.GetString().c_str());
        return;
    }

    // Stop the npc if moving
    if (uint32 pauseTimer = pCreature->GetInteractionPauseTimer())
        pCreature->GetMotionMaster()->PauseWaypoints(pauseTimer);

    if (pCreature->isSpiritGuide())
        pCreature->SendAreaSpiritHealerQueryOpcode(_player);

    if (!sScriptDevAIMgr.OnGossipHello(_player, pCreature))
    {
        _player->PrepareGossipMenu(pCreature, pCreature->GetDefaultGossipMenuId());
        _player->SendPreparedGossip(pCreature);
    }
}

void WorldSession::HandleGossipSelectOptionOpcode(WorldPacket& recv_data)
{
    DEBUG_LOG("WORLD: CMSG_GOSSIP_SELECT_OPTION");

    uint32 gossipListId;
    uint32 menuId;
    ObjectGuid guid;
    std::string code;

    recv_data >> guid >> menuId >> gossipListId;

    if (_player->GetPlayerMenu()->GossipOptionCoded(gossipListId))
    {
        recv_data >> code;
        DEBUG_LOG("Gossip code: %s", code.c_str());
    }

    uint32 sender = _player->GetPlayerMenu()->GossipOptionSender(gossipListId);
    uint32 action = _player->GetPlayerMenu()->GossipOptionAction(gossipListId);

    if (guid.IsAnyTypeCreature())
    {
        Creature* pCreature = GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_NONE);

        if (!pCreature)
        {
            DEBUG_LOG("WORLD: HandleGossipSelectOptionOpcode - %s not found or you can't interact with it.", guid.GetString().c_str());
            return;
        }

        if (!sScriptDevAIMgr.OnGossipSelect(_player, pCreature, sender, action, code.empty() ? nullptr : code.c_str()))
            _player->OnGossipSelect(pCreature, gossipListId, menuId);
    }
    else if (guid.IsGameObject())
    {
        GameObject* pGo = GetPlayer()->GetGameObjectIfCanInteractWith(guid);

        if (!pGo)
        {
            DEBUG_LOG("WORLD: HandleGossipSelectOptionOpcode - %s not found or you can't interact with it.", guid.GetString().c_str());
            return;
        }

        if (!sScriptDevAIMgr.OnGossipSelect(_player, pGo, sender, action, code.empty() ? nullptr : code.c_str()))
            _player->OnGossipSelect(pGo, gossipListId, menuId);
    }
    else if (guid.IsItem())
    {
        Item* pItem = GetPlayer()->GetItemByGuid(guid);

        if (!pItem)
        {
            DEBUG_LOG("WORLD: HandleGossipSelectOptionOpcode - %s not found or you can't interact with it.", guid.GetString().c_str());
            return;
        }

        sScriptDevAIMgr.OnGossipSelect(_player, pItem, sender, action, code.empty() ? nullptr : code.c_str());
    }
}

void WorldSession::HandleSpiritHealerActivateOpcode(WorldPacket& recv_data)
{
    DEBUG_LOG("WORLD: CMSG_SPIRIT_HEALER_ACTIVATE");

    ObjectGuid guid;

    recv_data >> guid;

    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_SPIRITHEALER);
    if (!unit)
    {
        DEBUG_LOG("WORLD: HandleSpiritHealerActivateOpcode - %s not found or you can't interact with him.", guid.GetString().c_str());
        return;
    }

    SendSpiritResurrect();
}

void WorldSession::SendSpiritResurrect() const
{
    _player->ResurrectPlayer(0.5f, true);

    _player->DurabilityLossAll(0.25f, true);

    // get corpse nearest graveyard
    WorldSafeLocsEntry const* corpseGrave = nullptr;
    Corpse* corpse = _player->GetCorpse();
    if (corpse)
        corpseGrave = _player->GetMap()->GetGraveyardManager().GetClosestGraveYard(corpse->GetPositionX(), corpse->GetPositionY(), corpse->GetPositionZ(), corpse->GetMapId(), _player->GetTeam());

    // now can spawn bones
    _player->SpawnCorpseBones();

    // teleport to nearest from corpse graveyard, if different from nearest to player ghost
    if (corpseGrave)
    {
        WorldSafeLocsEntry const* ghostGrave = _player->GetMap()->GetGraveyardManager().GetClosestGraveYard(_player->GetPositionX(), _player->GetPositionY(), _player->GetPositionZ(), _player->GetMapId(), _player->GetTeam());

        if (corpseGrave != ghostGrave)
            _player->TeleportTo(corpseGrave->map_id, corpseGrave->x, corpseGrave->y, corpseGrave->z, corpseGrave->o);
        // or update at original position
        else
        {
            _player->GetCamera().UpdateVisibilityForOwner();
            _player->UpdateObjectVisibility();
        }
    }
    // or update at original position
    else
    {
        _player->GetCamera().UpdateVisibilityForOwner();
        _player->UpdateObjectVisibility();
    }
}

void WorldSession::HandleBinderActivateOpcode(WorldPacket& recv_data)
{
    ObjectGuid npcGuid;
    recv_data >> npcGuid;

    if (!GetPlayer()->IsInWorld() || !GetPlayer()->IsAlive())
        return;

    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(npcGuid, UNIT_NPC_FLAG_INNKEEPER);
    if (!unit)
    {
        DEBUG_LOG("WORLD: HandleBinderActivateOpcode - %s not found or you can't interact with him.", npcGuid.GetString().c_str());
        return;
    }

    SendBindPoint(unit);
}

void WorldSession::SendBindPoint(Creature* npc) const
{
    // prevent set homebind to instances in any case
    if (GetPlayer()->GetMap()->Instanceable())
        return;

    // send spell for bind 3286 bind magic
    npc->CastSpell(_player, 3286, TRIGGERED_OLD_TRIGGERED);                    // Bind

    WorldPacket data(SMSG_TRAINER_BUY_SUCCEEDED, (8 + 4));
    data << npc->GetObjectGuid();
    data << uint32(3286);                                   // Bind
    SendPacket(data);

    _player->GetPlayerMenu()->CloseGossip();
}

void WorldSession::HandleListStabledPetsOpcode(WorldPacket& recv_data)
{
    DEBUG_LOG("WORLD: Recv MSG_LIST_STABLED_PETS");
    ObjectGuid npcGUID;

    recv_data >> npcGUID;

    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(npcGUID, UNIT_NPC_FLAG_STABLEMASTER);
    if (!unit)
    {
        DEBUG_LOG("WORLD: HandleListStabledPetsOpcode - %s not found or you can't interact with him.", npcGUID.GetString().c_str());
        return;
    }

    SendStablePet(npcGUID);
}

void WorldSession::SendStablePet(ObjectGuid guid) const
{
    DEBUG_LOG("WORLD: Recv MSG_LIST_STABLED_PETS Send.");

    WorldPacket data(MSG_LIST_STABLED_PETS, 200);           // guess size
    data << guid;

    Pet* pet = _player->GetPet();

    size_t wpos = data.wpos();
    data << uint8(0);                                       // place holder for slot show number

    data << uint8(GetPlayer()->m_stableSlots);

    uint8 num = 0;                                          // counter for place holder
    PetSaveMode firstSlot = PET_SAVE_FIRST_STABLE_SLOT;     // have to be changed to PET_SAVE_AS_CURRENT if pet is currently temp unsummoned

    // not let move dead pet in slot
    if (pet && pet->IsAlive() && pet->getPetType() == HUNTER_PET)
    {
        data << uint32(pet->GetCharmInfo()->GetPetNumber());
        data << uint32(pet->GetEntry());
        data << uint32(pet->GetLevel());
        data << pet->GetName();                             // petname
        data << uint32(pet->GetLoyaltyLevel());             // loyalty
        data << uint8(0x01);                                // client slot 1 == current pet (0)
        ++num;
    }
    else
    {
        if(_player->GetTemporaryUnsummonedPetNumber()) // temporary unsummon - mount
            firstSlot = PET_SAVE_AS_CURRENT;
        else
        {
            auto queryResult = CharacterDatabase.PQuery("SELECT owner, id, entry, level, name, loyalty FROM character_pet WHERE owner = '%u' AND slot = '%u' ORDER BY slot",
                _player->GetGUIDLow(), uint32(PET_SAVE_NOT_IN_SLOT));

            if (queryResult) // dismissed pet
            {
                do
                {
                    Field* fields = queryResult->Fetch();

                    data << uint32(fields[1].GetUInt32());          // petnumber
                    data << uint32(fields[2].GetUInt32());          // creature entry
                    data << uint32(fields[3].GetUInt32());          // level
                    data << fields[4].GetString();                  // name
                    data << uint32(fields[5].GetUInt32());          // loyalty
                    data << uint8(0x03);                            // slot

                    ++num;
                } while (queryResult->NextRow());
            }
        }
    }

    //                                                  0      1     2   3      4      5        6
    auto queryResult = CharacterDatabase.PQuery("SELECT owner, slot, id, entry, level, loyalty, name FROM character_pet WHERE owner = '%u' AND slot >= '%u' AND slot <= '%u' ORDER BY slot",
                          _player->GetGUIDLow(), uint32(firstSlot), uint32(PET_SAVE_LAST_STABLE_SLOT));

    if (queryResult)
    {
        do
        {
            Field* fields = queryResult->Fetch();

            data << uint32(fields[2].GetUInt32());          // petnumber
            data << uint32(fields[3].GetUInt32());          // creature entry
            data << uint32(fields[4].GetUInt32());          // level
            data << fields[6].GetString();                  // name
            data << uint32(fields[5].GetUInt32());          // loyalty
            data << uint8(fields[1].GetUInt32() + 1);       // slot

            ++num;
        }
        while (queryResult->NextRow());
    }

    data.put<uint8>(wpos, num);                             // set real data to placeholder
    SendPacket(data);
}

void WorldSession::SendStableResult(uint8 res) const
{
    WorldPacket data(SMSG_STABLE_RESULT, 1);
    data << uint8(res);
    SendPacket(data);
}

bool WorldSession::CheckStableMaster(ObjectGuid guid) const
{
    // spell case or GM
    if (guid == GetPlayer()->GetObjectGuid())
    {
        // command case will return only if player have real access to command
        if (!ChatHandler(GetPlayer()).FindCommand("stable"))
        {
            DEBUG_LOG("%s attempt open stable in cheating way.", guid.GetString().c_str());
            return false;
        }
    }
    // stable master case
    else
    {
        if (!GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_STABLEMASTER))
        {
            DEBUG_LOG("Stablemaster %s not found or you can't interact with him.", guid.GetString().c_str());
            return false;
        }
    }

    return true;
}

void WorldSession::HandleStablePet(WorldPacket& recv_data)
{
    DEBUG_LOG("WORLD: Recv CMSG_STABLE_PET");
    ObjectGuid npcGUID;

    recv_data >> npcGUID;

    if (!GetPlayer()->IsAlive())
    {
        SendStableResult(STABLE_ERR_STABLE);
        return;
    }

    if (!CheckStableMaster(npcGUID))
    {
        SendStableResult(STABLE_ERR_STABLE);
        return;
    }

    Pet* pet = _player->GetPet();

    // can't place in stable dead pet
    if (pet)
    {
        bool stop = false;
        if (!pet->IsAlive())
        {
            _player->SendPetTameFailure(PETTAME_DEAD);
            stop = true;
        }

        if (!stop && pet->getPetType() != HUNTER_PET)
        {
            _player->SendPetTameFailure(PETTAME_INVALIDCREATURE);
            stop = true;
        }

        if (stop)
        {
            SendStableResult(STABLE_ERR_STABLE);
            return;
        }
    }
    else
    {
        SpellCastResult loadResult = Pet::TryLoadFromDB(_player, 0, 0, _player->GetTemporaryUnsummonedPetNumber() != 0, HUNTER_PET);
        if (loadResult != SPELL_CAST_OK)
        {
            if (loadResult == SPELL_FAILED_TARGETS_DEAD)
                _player->SendPetTameFailure(PETTAME_DEAD);

            if (loadResult == SPELL_FAILED_BAD_TARGETS)
                _player->SendPetTameFailure(PETTAME_INVALIDCREATURE);

            SendStableResult(STABLE_ERR_STABLE);
            return;
        }
    }

    uint32 free_slot = 1;

    auto queryResult = CharacterDatabase.PQuery("SELECT owner,slot,id FROM character_pet WHERE owner = '%u'  AND slot >= '%u' AND slot <= '%u' ORDER BY slot ",
                          _player->GetGUIDLow(), PET_SAVE_FIRST_STABLE_SLOT, PET_SAVE_LAST_STABLE_SLOT);
    if (queryResult)
    {
        do
        {
            Field* fields = queryResult->Fetch();

            uint32 slot = fields[1].GetUInt32();

            // slots ordered in query, and if not equal then free
            if (slot != free_slot)
                break;

            // this slot not free, skip
            ++free_slot;
        }
        while (queryResult->NextRow());
    }

    if (free_slot > 0 && free_slot <= GetPlayer()->m_stableSlots)
    {
        if (pet)
            pet->Unsummon(PetSaveMode(free_slot), _player);
        else
        {
            // change pet slot directly in database
            CharacterDatabase.BeginTransaction();
            static SqlStatementID ChangePetSlot_ID;
            SqlStatement ChangePetSlot = CharacterDatabase.CreateStatement(ChangePetSlot_ID, "UPDATE character_pet SET slot = ? WHERE owner = ? AND slot = ? ");
            ChangePetSlot.PExecute(free_slot, _player->GetObjectGuid().GetCounter(), uint32(_player->GetTemporaryUnsummonedPetNumber() ? PET_SAVE_AS_CURRENT : PET_SAVE_NOT_IN_SLOT));
            CharacterDatabase.CommitTransaction();
        }
        SendStableResult(STABLE_SUCCESS_STABLE);
        _player->SetTemporaryUnsummonedPetNumber(0);
    }
    else
        SendStableResult(STABLE_ERR_STABLE);
}

void WorldSession::HandleUnstablePet(WorldPacket& recv_data)
{
    DEBUG_LOG("WORLD: Recv CMSG_UNSTABLE_PET.");
    ObjectGuid npcGUID;
    uint32 petnumber;

    recv_data >> npcGUID >> petnumber;

    if (!CheckStableMaster(npcGUID))
    {
        SendStableResult(STABLE_ERR_STABLE);
        return;
    }

    uint32 creature_id = 0;
    uint32 slot = 0;

    auto queryResult = CharacterDatabase.PQuery("SELECT entry, slot FROM character_pet WHERE owner = '%u' AND id = '%u' AND slot >='%u' AND slot <= '%u'",
                          _player->GetGUIDLow(), petnumber, PET_SAVE_FIRST_STABLE_SLOT, PET_SAVE_LAST_STABLE_SLOT);
    if (queryResult)
    {
        Field* fields = queryResult->Fetch();
        creature_id   = fields[0].GetUInt32();
        slot          = fields[1].GetUInt32();
    }

    if (!creature_id)
    {
        sLog.outError("HandleUnstablePet> %s trying to unstable a pet with invalid creature id!", _player->GetGuidStr().c_str());
        SendStableResult(STABLE_ERR_STABLE);
        return;
    }

    if (slot == uint32(PET_SAVE_AS_CURRENT))
    {
        sLog.outError("HandleUnstablePet> %s trying to unstable current pet!", _player->GetGuidStr().c_str());
        SendStableResult(STABLE_ERR_STABLE);
        return;
    }

    CreatureInfo const* creatureInfo = ObjectMgr::GetCreatureTemplate(creature_id);
    if (!creatureInfo || !creatureInfo->isTameable())
    {
        SendStableResult(STABLE_ERR_STABLE);
        return;
    }

    // found if a pet is actually summoned
    Pet* pet = _player->GetPet();
    if (pet)
    {
        bool stop = false;
        if (!pet->IsAlive())
        {
            _player->SendPetTameFailure(PETTAME_DEAD);
            stop = true;
        }

        if (!stop && pet->getPetType() != HUNTER_PET)
        {
            _player->SendPetTameFailure(PETTAME_ANOTHERSUMMONACTIVE);
            stop = true;
        }

        if (stop)
        {
            SendStableResult(STABLE_ERR_STABLE);
            return;
        }

        pet->Unsummon(PetSaveMode(slot), _player);
    }
    else
    {
        // try to find if pet is actually temporary unsummoned
        SpellCastResult loadResult = Pet::TryLoadFromDB(_player, 0, 0, _player->GetTemporaryUnsummonedPetNumber() != 0, HUNTER_PET);
        if (loadResult != SPELL_CAST_OK)
        {
            if (loadResult == SPELL_FAILED_TARGETS_DEAD)
            {
                _player->SendPetTameFailure(PETTAME_DEAD);

                sLog.outError("UnStablePet> trying to unstable a dead pet. Should not be possible!");
                return;
            }

            if (loadResult == SPELL_FAILED_BAD_TARGETS)
            {
                _player->SendPetTameFailure(PETTAME_ANOTHERSUMMONACTIVE);
                SendStableResult(STABLE_ERR_STABLE);
                return;
            }
        }
        else
        {
            // change pet slot directly in database
            CharacterDatabase.BeginTransaction();
            static SqlStatementID ChangePetSlot_ID;
            SqlStatement ChangePetSlot = CharacterDatabase.CreateStatement(ChangePetSlot_ID, "UPDATE character_pet SET slot = ? WHERE owner = ? AND slot = ? ");
            ChangePetSlot.PExecute(slot, _player->GetObjectGuid().GetCounter(), uint32(_player->GetTemporaryUnsummonedPetNumber() ? PET_SAVE_AS_CURRENT : PET_SAVE_NOT_IN_SLOT));
            CharacterDatabase.CommitTransaction();
            _player->SetTemporaryUnsummonedPetNumber(0);
        }
    }

    Pet* newpet = new Pet(HUNTER_PET);
    if (!newpet->LoadPetFromDB(_player, newpet->GetPetSpawnPosition(_player), creature_id, petnumber))
    {
        delete newpet;
        newpet = nullptr;

        // load failed but it may be normal if the pet is set as temporary unsummoned by any reason
        if (!_player->GetTemporaryUnsummonedPetNumber())
        {
            // load really failed
            SendStableResult(STABLE_ERR_STABLE);
            return;
        }
    }

    SendStableResult(STABLE_SUCCESS_UNSTABLE);
}

void WorldSession::HandleBuyStableSlot(WorldPacket& recv_data)
{
    DEBUG_LOG("WORLD: Recv CMSG_BUY_STABLE_SLOT.");
    ObjectGuid npcGUID;

    recv_data >> npcGUID;

    if (!CheckStableMaster(npcGUID))
    {
        SendStableResult(STABLE_ERR_STABLE);
        return;
    }

    if (GetPlayer()->m_stableSlots < MAX_PET_STABLES)
    {
        StableSlotPricesEntry const* SlotPrice = sStableSlotPricesStore.LookupEntry(GetPlayer()->m_stableSlots + 1);
        if (_player->GetMoney() >= SlotPrice->Price)
        {
            ++GetPlayer()->m_stableSlots;
            _player->ModifyMoney(-int32(SlotPrice->Price));
            SendStableResult(STABLE_SUCCESS_BUY_SLOT);
        }
        else
            SendStableResult(STABLE_ERR_MONEY);
    }
    else
        SendStableResult(STABLE_ERR_STABLE);
}

void WorldSession::HandleStableRevivePet(WorldPacket& /* recv_data */)
{
    DEBUG_LOG("HandleStableRevivePet: Not implemented");
}

void WorldSession::HandleStableSwapPet(WorldPacket& recv_data)
{
    DEBUG_LOG("WORLD: Recv CMSG_STABLE_SWAP_PET.");
    ObjectGuid npcGUID;
    uint32 pet_number;

    recv_data >> npcGUID >> pet_number;

    if (!CheckStableMaster(npcGUID))
    {
        SendStableResult(STABLE_ERR_STABLE);
        return;
    }

    Pet* pet = _player->GetPet();
    if (pet)
    {
        bool stop = false;
        if (!pet->IsAlive())
        {
            _player->SendPetTameFailure(PETTAME_DEAD);
            stop = true;
        }

        if (!stop && pet->getPetType() != HUNTER_PET)
        {
            _player->SendPetTameFailure(PETTAME_INVALIDCREATURE);
            stop = true;
        }

        if (stop)
        {
            SendStableResult(STABLE_ERR_STABLE);
            return;
        }
    }
    else
    {
        SpellCastResult loadResult = Pet::TryLoadFromDB(_player, 0, 0, _player->GetTemporaryUnsummonedPetNumber() != 0, HUNTER_PET);
        if (loadResult != SPELL_CAST_OK)
        {
            if (loadResult == SPELL_FAILED_TARGETS_DEAD)
                _player->SendPetTameFailure(PETTAME_DEAD);

            if (loadResult == SPELL_FAILED_BAD_TARGETS)
                _player->SendPetTameFailure(PETTAME_INVALIDCREATURE);

            SendStableResult(STABLE_ERR_STABLE);
            return;
        }
    }

    // find swapped pet slot in stable
    auto queryResult = CharacterDatabase.PQuery("SELECT slot,entry FROM character_pet WHERE owner = '%u' AND id = '%u'",
                          _player->GetGUIDLow(), pet_number);
    if (!queryResult)
    {
        SendStableResult(STABLE_ERR_STABLE);
        return;
    }

    Field* fields = queryResult->Fetch();

    uint32 slot        = fields[0].GetUInt32();
    uint32 creature_id = fields[1].GetUInt32();

    if (!creature_id)
    {
        SendStableResult(STABLE_ERR_STABLE);
        return;
    }

    CreatureInfo const* creatureInfo = ObjectMgr::GetCreatureTemplate(creature_id);
    if (!creatureInfo || !creatureInfo->isTameable())
    {
        SendStableResult(STABLE_ERR_STABLE);
        return;
    }

    // move alive pet to slot or delete dead pet
    if (pet)
        pet->Unsummon(PetSaveMode(slot), _player);
    else
    {
        // change pet slot directly in memory
        CharacterDatabase.BeginTransaction();
        static SqlStatementID ChangePetSlot_ID;
        SqlStatement ChangePetSlot = CharacterDatabase.CreateStatement(ChangePetSlot_ID, "UPDATE character_pet SET slot = ? WHERE owner = ? AND slot = ? ");
        ChangePetSlot.PExecute(slot, _player->GetObjectGuid().GetCounter(), uint32(_player->GetTemporaryUnsummonedPetNumber() ? PET_SAVE_AS_CURRENT : PET_SAVE_NOT_IN_SLOT));
        CharacterDatabase.CommitTransaction();
    }

    // summon unstabled pet
    Pet* newpet = new Pet;
    if (!newpet->LoadPetFromDB(_player, newpet->GetPetSpawnPosition(_player), creature_id, pet_number))
    {
        delete newpet;

        // load failed but it may be normal if the pet is set as temporary unsummoned by any reason
        if (!_player->GetTemporaryUnsummonedPetNumber())
        {
            // load really failed
            SendStableResult(STABLE_ERR_STABLE);
            return;
        }
    }

    SendStableResult(STABLE_SUCCESS_UNSTABLE);
}

void WorldSession::HandleRepairItemOpcode(WorldPacket& recv_data)
{
    DEBUG_LOG("WORLD: CMSG_REPAIR_ITEM");

    ObjectGuid npcGuid;
    ObjectGuid itemGuid;
    uint8 guildBank;                                        // new in 2.3.2, bool that means from guild bank money

    recv_data >> npcGuid >> itemGuid >> guildBank;

    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(npcGuid, UNIT_NPC_FLAG_REPAIR);
    if (!unit)
    {
        DEBUG_LOG("WORLD: HandleRepairItemOpcode - %s not found or you can't interact with him.", npcGuid.GetString().c_str());
        return;
    }

    // reputation discount
    float discountMod = _player->GetReputationPriceDiscount(unit);

    uint32 TotalCost = 0;
    if (itemGuid)
    {
        DEBUG_LOG("ITEM: %s repair of %s", npcGuid.GetString().c_str(), itemGuid.GetString().c_str());

        Item* item = _player->GetItemByGuid(itemGuid);

        if (item)
            TotalCost = _player->DurabilityRepair(item->GetPos(), true, discountMod, (guildBank > 0));
    }
    else
    {
        DEBUG_LOG("ITEM: %s repair all items", npcGuid.GetString().c_str());

        TotalCost = _player->DurabilityRepairAll(true, discountMod, (guildBank > 0));
    }
    if (guildBank)
    {
        uint32 GuildId = _player->GetGuildId();
        if (!GuildId)
            return;
        Guild* pGuild = sGuildMgr.GetGuildById(GuildId);
        if (!pGuild)
            return;
        pGuild->LogBankEvent(GUILD_BANK_LOG_REPAIR_MONEY, 0, _player->GetGUIDLow(), TotalCost);
        pGuild->SendMoneyInfo(this, _player->GetGUIDLow());
    }
}
