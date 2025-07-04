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
#include "Database/DatabaseEnv.h"
#include "World/World.h"
#include "Entities/Player.h"
#include "Server/Opcodes.h"
#include "Chat/Chat.h"
#include "Globals/ObjectAccessor.h"
#include "Tools/Language.h"
#include "Accounts/AccountMgr.h"
#include "AI/ScriptDevAI/ScriptDevAIMgr.h"
#include "SystemConfig.h"
#include "revision.h"
#include "Util/Util.h"
#include "World/WorldState.h"

bool ChatHandler::HandleHelpCommand(char* args)
{
    if (!*args)
    {
        ShowHelpForCommand(getCommandTable(), "help");
        ShowHelpForCommand(getCommandTable(), "");
    }
    else
    {
        if (!ShowHelpForCommand(getCommandTable(), args))
            SendSysMessage(LANG_NO_CMD);
    }

    return true;
}

bool ChatHandler::HandleCommandsCommand(char* /*args*/)
{
    ShowHelpForCommand(getCommandTable(), "");
    return true;
}

bool ChatHandler::HandleAccountCommand(char* args)
{
    // let show subcommands at unexpected data in args
    if (*args)
        return false;

    AccountTypes gmlevel = GetAccessLevel();
    PSendSysMessage(LANG_ACCOUNT_LEVEL, uint32(gmlevel));
    return true;
}

bool ChatHandler::HandleStartCommand(char* /*args*/)
{
    Player* chr = m_session->GetPlayer();

    if (chr->IsTaxiFlying())
    {
        SendSysMessage(LANG_YOU_IN_FLIGHT);
        SetSentErrorMessage(true);
        return false;
    }

    if (chr->IsInCombat())
    {
        SendSysMessage(LANG_YOU_IN_COMBAT);
        SetSentErrorMessage(true);
        return false;
    }

    // cast spell Stuck
    chr->CastSpell(chr, 7355, TRIGGERED_NONE);
    return true;
}

bool ChatHandler::HandleServerInfoCommand(char* /*args*/)
{
    uint32 activeClientsNum = sWorld.GetActiveSessionCount();
    uint32 queuedClientsNum = sWorld.GetQueuedSessionCount();
    uint32 maxActiveClientsNum = sWorld.GetMaxActiveSessionCount();
    uint32 maxQueuedClientsNum = sWorld.GetMaxQueuedSessionCount();
    std::string str = secsToTimeString(sWorld.GetUptime());

    char const* full;
    if (m_session)
        full = _FULLVERSION(REVISION_DATE, "|cffffffff|Hurl:" REVISION_ID "|h" REVISION_ID "|h|r");
    else
        full = _FULLVERSION(REVISION_DATE, REVISION_ID);
    SendSysMessage(full);

    PSendSysMessage(LANG_USING_WORLD_DB, sWorld.GetDBVersion());
    PSendSysMessage(LANG_USING_EVENT_AI, sWorld.GetCreatureEventAIVersion());
    PSendSysMessage(LANG_CONNECTED_USERS, activeClientsNum, maxActiveClientsNum, queuedClientsNum, maxQueuedClientsNum);
    PSendSysMessage(LANG_UPTIME, str.c_str());

    return true;
}

bool ChatHandler::HandleDismountCommand(char* /*args*/)
{
    Player* player = m_session->GetPlayer();

    // If player is not mounted, so go out :)
    if (!player->IsMounted())
    {
        SendSysMessage(LANG_CHAR_NON_MOUNTED);
        SetSentErrorMessage(true);
        return false;
    }

    if (player->IsTaxiFlying())
    {
        SendSysMessage(LANG_YOU_IN_FLIGHT);
        SetSentErrorMessage(true);
        return false;
    }

    player->Unmount();
    player->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);
    return true;
}

bool ChatHandler::HandleSaveCommand(char* /*args*/)
{
    Player* player = m_session->GetPlayer();

    // save GM account without delay and output message (testing, etc)
    if (GetAccessLevel() > SEC_PLAYER)
    {
        player->SaveToDB();
        SendSysMessage(LANG_PLAYER_SAVED);
        return true;
    }

    // save or plan save after 20 sec (logout delay) if current next save time more this value and _not_ output any messages to prevent cheat planning
    uint32 save_interval = sWorld.getConfig(CONFIG_UINT32_INTERVAL_SAVE);
    if (save_interval == 0 || (save_interval > 20 * IN_MILLISECONDS && player->GetSaveTimer() <= save_interval - 20 * IN_MILLISECONDS))
        player->SaveToDB();

    return true;
}

bool ChatHandler::HandleTogglePvpMessages(char* args)
{
    Player* player = m_session->GetPlayer();
    if (player->IsIgnoringPvpMessages())
    {
        player->SetIgnoringPvpMessages(false);
        SendSysMessage("You no longer ignore PvP messages");
    }
    else
    {
        player->SetIgnoringPvpMessages(true);
        SendSysMessage("You now ignore PvP messages");
    }
    return true;
}

bool ChatHandler::HandleGMListIngameCommand(char* /*args*/)
{
    std::list< std::pair<std::string, bool> > names;

    {
        HashMapHolder<Player>::ReadGuard g(HashMapHolder<Player>::GetLock());
        HashMapHolder<Player>::MapType& m = sObjectAccessor.GetPlayers();
        for (HashMapHolder<Player>::MapType::const_iterator itr = m.begin(); itr != m.end(); ++itr)
        {
            Player* player = itr->second;
            AccountTypes security = player->GetSession()->GetSecurity();
            if ((player->IsGameMaster() || (security > SEC_PLAYER && security <= (AccountTypes)sWorld.getConfig(CONFIG_UINT32_GM_LEVEL_IN_GM_LIST))) &&
                (!m_session || player->IsVisibleGloballyFor(m_session->GetPlayer())))
                names.push_back(std::make_pair<std::string, bool>(GetNameLink(player), player->isAcceptWhispers()));
        }
    }

    if (!names.empty())
    {
        SendSysMessage(LANG_GMS_ON_SRV);

        char const* accepts = GetMangosString(LANG_GM_ACCEPTS_WHISPER);
        char const* not_accept = GetMangosString(LANG_GM_NO_WHISPER);
        for (std::list<std::pair< std::string, bool> >::const_iterator iter = names.begin(); iter != names.end(); ++iter)
            PSendSysMessage("%s - %s", iter->first.c_str(), iter->second ? accepts : not_accept);
    }
    else
        SendSysMessage(LANG_GMS_NOT_LOGGED);

    return true;
}

bool ChatHandler::HandleAccountPasswordCommand(char* args)
{
    // allow use from RA, but not from console (not have associated account id)
    if (!GetAccountId())
    {
        SendSysMessage(LANG_RA_ONLY_COMMAND);
        SetSentErrorMessage(true);
        return false;
    }

    // allow or quoted string with possible spaces or literal without spaces
    char* old_pass = ExtractQuotedOrLiteralArg(&args);
    char* new_pass = ExtractQuotedOrLiteralArg(&args);
    char* new_pass_c = ExtractQuotedOrLiteralArg(&args);

    if (!old_pass || !new_pass || !new_pass_c)
        return false;

    std::string password_old = old_pass;
    std::string password_new = new_pass;
    std::string password_new_c = new_pass_c;

    if (password_new != password_new_c)
    {
        SendSysMessage(LANG_NEW_PASSWORDS_NOT_MATCH);
        SetSentErrorMessage(true);
        return false;
    }

    if (!sAccountMgr.CheckPassword(GetAccountId(), password_old))
    {
        SendSysMessage(LANG_COMMAND_WRONGOLDPASSWORD);
        SetSentErrorMessage(true);
        return false;
    }

    AccountOpResult result = sAccountMgr.ChangePassword(GetAccountId(), password_new);

    switch (result)
    {
        case AOR_OK:
            SendSysMessage(LANG_COMMAND_PASSWORD);
            break;
        case AOR_PASS_TOO_LONG:
            SendSysMessage(LANG_PASSWORD_TOO_LONG);
            SetSentErrorMessage(true);
            return false;
        case AOR_NAME_NOT_EXIST:                            // not possible case, don't want get account name for output
        default:
            SendSysMessage(LANG_COMMAND_NOTCHANGEPASSWORD);
            SetSentErrorMessage(true);
            return false;
    }

    // OK, but avoid normal report for hide passwords, but log use command for anyone
    LogCommand(".account password *** *** ***");
    SetSentErrorMessage(true);
    return false;
}

bool ChatHandler::HandleAccountLockCommand(char* args)
{
    // allow use from RA, but not from console (not have associated account id)
    if (!GetAccountId())
    {
        SendSysMessage(LANG_RA_ONLY_COMMAND);
        SetSentErrorMessage(true);
        return false;
    }

    bool value;
    if (!ExtractOnOff(&args, value))
    {
        SendSysMessage(LANG_USE_BOL);
        SetSentErrorMessage(true);
        return false;
    }

    if (value)
    {
        LoginDatabase.PExecute("UPDATE account SET locked = '1' WHERE id = '%u'", GetAccountId());
        PSendSysMessage(LANG_COMMAND_ACCLOCKLOCKED);
    }
    else
    {
        LoginDatabase.PExecute("UPDATE account SET locked = '0' WHERE id = '%u'", GetAccountId());
        PSendSysMessage(LANG_COMMAND_ACCLOCKUNLOCKED);
    }

    return true;
}

/// Display the 'Message of the day' for the realm
bool ChatHandler::HandleServerMotdCommand(char* /*args*/)
{
    PSendSysMessage(LANG_MOTD_CURRENT, sWorld.GetMotd());
    return true;
}

bool ChatHandler::HandleWhisperRestrictionCommand(char* args)
{
    if (!*args)
    {
        PSendSysMessage("Whisper restriction is %s.", m_session->GetPlayer()->isEnabledWhisperRestriction() ? "ON" : "OFF");
        return true;
    }

    bool value;
    if (!ExtractOnOff(&args, value))
    {
        SendSysMessage(LANG_USE_BOL);
        SetSentErrorMessage(true);
        return false;
    }

    m_session->GetPlayer()->SetWhisperRestriction(value);
    PSendSysMessage("Whisper restriction is now %s.", value ? "ON. Only friends, group members, or guildmates may whisper you." : "OFF");

    return true;
}

// Custom
bool ChatHandler::HandleXPCommandSet(char* args)
{
    Player* player = m_session->GetPlayer();

    uint32 modifier;
    if (!ExtractOptUInt32(&args, modifier, 10))
    {
        PSendSysMessage("XP modifier not a valid number.");
        return false;
    }

    uint32 boostMinLevelRestricted = sWorld.getConfig(CONFIG_UINT32_BOOST_MIN_LEVEL_RESTRICTED);
    uint32 cap = sWorld.GetExperienceCapForLevel(player->GetLevel(), player->GetTeam());
    if (uint32 restrictionStateFlags = sWorldState.IsTbcRaceBoostRestricted())
    {
        if (restrictionStateFlags & BOOST_FLAG_TBC_RACES)
        {
            if (player->getRace() == RACE_DRAENEI || player->getRace() == RACE_BLOODELF)
                if (player->GetLevel() < boostMinLevelRestricted)
                    cap = 1;
        }
        if (restrictionStateFlags & BOOST_FLAG_HORDE_RACES)
        {
            if (player->getRace() == RACE_ORC || player->getRace() == RACE_UNDEAD || player->getRace() == RACE_TAUREN || player->getRace() == RACE_TROLL)
                if (player->GetLevel() < boostMinLevelRestricted)
                    cap = 1;
        }
        if (restrictionStateFlags & BOOST_FLAG_ALLIANCE_RACES)
        {
            if (player->getRace() == RACE_HUMAN || player->getRace() == RACE_DWARF || player->getRace() == RACE_NIGHTELF || player->getRace() == RACE_GNOME)
                if (player->GetLevel() < boostMinLevelRestricted)
                    cap = 1;
        }
    }
    if (modifier && modifier <= cap)
    {
        player->SetPlayerXPModifier(modifier);
        PSendSysMessage("You have set XP modifier to %u.", modifier);
    }
    else
    {
        PSendSysMessage("XP modifier not valid. Permitted values: 1 - %u.", cap);
        return false;
    }

    return true;
}

bool ChatHandler::HandleXPCommandCurrent(char* /*args*/)
{
    Player* player = m_session->GetPlayer();

    PSendSysMessage("Current XP modifier: %u.", player->GetPlayerXPModifier());

    return true;
}

bool ChatHandler::HandleXPCommandAvailable(char* args)
{
    Player* player = m_session->GetPlayer();

    PSendSysMessage("Current available XP modifiers:");
    std::array<uint32, MAX_PLAYER_LEVEL> caps;
    sWorld.GetExperienceCapArray(player->GetTeam(), caps);
    uint32 boostMinLevelRestricted = sWorld.getConfig(CONFIG_UINT32_BOOST_MIN_LEVEL_RESTRICTED);
    if (uint32 restrictionStateFlags = sWorldState.IsTbcRaceBoostRestricted())
    {
        if (restrictionStateFlags & BOOST_FLAG_TBC_RACES)
        {
            if (player->getRace() == RACE_DRAENEI || player->getRace() == RACE_BLOODELF)
            {
                for (uint32 i = 1; i < boostMinLevelRestricted; ++i)
                    caps[i] = 1;
            }
        }
        if (restrictionStateFlags & BOOST_FLAG_HORDE_RACES)
        {
            if (player->getRace() == RACE_ORC || player->getRace() == RACE_UNDEAD || player->getRace() == RACE_TAUREN || player->getRace() == RACE_TROLL)
            {
                for (uint32 i = 1; i < boostMinLevelRestricted; ++i)
                    caps[i] = 1;
            }
        }
        if (restrictionStateFlags & BOOST_FLAG_ALLIANCE_RACES)
        {
            if (player->getRace() == RACE_HUMAN || player->getRace() == RACE_DWARF || player->getRace() == RACE_NIGHTELF || player->getRace() == RACE_GNOME)
            {
                for (uint32 i = 1; i < boostMinLevelRestricted; ++i)
                    caps[i] = 1;
            }
        }
    }

    for (uint32 i = 1; i < MAX_LEVEL_TBC; i += 10)
    {
        PSendSysMessage("%u: %u, %u: %u, %u: %u, %u: %u, %u: %u, %u: %u, %u: %u, %u: %u, %u: %u, %u: %u",
            i, caps[i], i + 1, caps[i + 1], i + 2, caps[i + 2], i + 3, caps[i + 3], i + 4, caps[i + 4], i + 5, caps[i + 5], i + 6, caps[i + 6], i + 7, caps[i + 7], i + 8, caps[i + 8], i + 9, caps[i + 9]);
    }

    return true;
}

bool ChatHandler::HandleServerFirstPrint(char* args)
{
    std::string output = sWorldState.GetServerFirstString();
    PSendSysMessage("%s", output.data());
    return true;
}
