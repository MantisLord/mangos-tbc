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

#ifndef MANGOSSERVER_CHAT_H
#define MANGOSSERVER_CHAT_H

#include "Common.h"
#include "Globals/Locales.h"
#include "Globals/SharedDefines.h"
#include "Entities/ObjectGuid.h"

#include <functional>
#include <utility>

struct AreaTrigger;
struct AreaTriggerEntry;
struct FactionEntry;
struct FactionState;
struct GameTele;
struct SpellEntry;

class QueryResult;
class ChatHandler;
class WorldSession;
class WorldPacket;
class GMTicket;
class MailDraft;
class Object;
class GameObject;
class Creature;
class Pet;
class Player;
class Unit;

class ChatCommand
{
    public:
        const char*        Name;
        uint32             SecurityLevel;                   // function pointer required correct align (use uint32)
        bool               AllowConsole;
        bool (ChatHandler::*Handler)(char* args);
        std::string        Help;
        ChatCommand*       ChildCommands;
};

enum ChatCommandSearchResult
{
    CHAT_COMMAND_OK,                                        // found accessible command by command string
    CHAT_COMMAND_UNKNOWN,                                   // first level command not found
    CHAT_COMMAND_UNKNOWN_SUBCOMMAND,                        // command found but some level subcommand not find in subcommand list
};

enum PlayerChatTag
{
    CHAT_TAG_NONE               = 0x00,
    CHAT_TAG_AFK                = 0x01,
    CHAT_TAG_DND                = 0x02,
    CHAT_TAG_GM                 = 0x04,
};
typedef uint32 ChatTagFlags;

class ChatHandler
{
    public:
        explicit ChatHandler(WorldSession* session);
        explicit ChatHandler(Player* player);
        virtual ~ChatHandler();

        static char* LineFromMessage(char*& pos) { char* start = strtok(pos, "\n"); pos = nullptr; return start; }

        // function with different implementation for chat/console
        virtual const char* GetMangosString(int32 entry) const;
        const char* GetOnOffStr(bool value) const;

        virtual void SendSysMessage(const char* str);

        void SendSysMessage(int32     entry);
        void PSendSysMessage(const char* format, ...) ATTR_PRINTF(2, 3);
        void PSendSysMessage(int32     entry, ...);

        bool ParseCommands(const char* text);
        ChatCommand const* FindCommand(char const* text);

        static bool HasEscapeSequences(const char* message);
        static bool CheckEscapeSequences(const char* message);

        bool HasSentErrorMessage() const { return sentErrorMessage; }

        WorldSession* GetSession() { return m_session; }

        /**
        * \brief Prepare SMSG_GM_MESSAGECHAT/SMSG_MESSAGECHAT
        *
        * Method:    BuildChatPacket build message chat packet generic way
        * FullName:  ChatHandler::BuildChatPacket
        * Access:    public static
        * Returns:   void
        *
        * \param WorldPacket& data             : Provided packet will be filled with requested info
        * \param ChatMsg msgtype               : Message type from ChatMsg enum from SharedDefines.h
        * \param ChatTagFlags chatTag          : Chat tag from PlayerChatTag in Chat.h
        * \param char const* message           : Message to send
        * \param Language language             : Language from Language enum in SharedDefines.h
        * \param ObjectGuid const& senderGuid  : May be null in some case but often required for ignore list
        * \param char const* senderName        : Required for type *MONSTER* or *BNET*, but also if GM is true
        * \param ObjectGuid const& targetGuid  : Often null, but needed for type *MONSTER* or *BNET* or *BATTLEGROUND* or *ACHIEVEMENT*
        * \param char const* targetName        : Often null, but needed for type *MONSTER* or *BNET* or *BATTLEGROUND*
        * \param char const* channelName       : Required only for CHAT_MSG_CHANNEL
        **/
        static void BuildChatPacket(
            WorldPacket& data, ChatMsg msgtype, char const* message, Language language = LANG_UNIVERSAL, ChatTagFlags chatTag = CHAT_TAG_NONE,
            ObjectGuid const& senderGuid = ObjectGuid(), char const* senderName = nullptr,
            ObjectGuid const& targetGuid = ObjectGuid(), char const* targetName = nullptr,
            char const* channelName = nullptr);

        // extraction different type params from args string, all functions update (char** args) to first unparsed tail symbol at return
        static void  SkipWhiteSpaces(char** args);
        bool  ExtractInt32(char** args, int32& val) const;
        bool  ExtractOptInt32(char** args, int32& val, int32 defVal) const;
        bool  ExtractUInt32Base(char** args, uint32& val, uint32 base) const;
        bool  ExtractUInt32(char** args, uint32& val) const { return ExtractUInt32Base(args, val, 10); }
        bool  ExtractOptUInt32(char** args, uint32& val, uint32 defVal) const;
        static bool  ExtractFloat(char** args, float& val);
        static bool  ExtractOptFloat(char** args, float& val, float defVal);
        static char* ExtractQuotedArg(char** args, bool asis = false);
        // string with " or [] or ' around
        static char* ExtractLiteralArg(char** args, char const* lit = nullptr);
        // literal string (until whitespace and not started from "['|), any or 'lit' if provided
        static char* ExtractQuotedOrLiteralArg(char** args, bool asis = false);
        static bool  ExtractOnOff(char** args, bool& value);
        static char* ExtractLinkArg(char** args, char const* const* linkTypes = nullptr, int* foundIdx = nullptr, char** keyPair = nullptr, char** somethingPair = nullptr);
        // shift-link like arg (with aditional info if need)
        static char* ExtractArg(char** args, bool asis = false);   // any name/number/quote/shift-link strings
        static char* ExtractOptNotLastArg(char** args);            // extract name/number/quote/shift-link arg only if more data in args for parse

        char* ExtractKeyFromLink(char** text, char const* linkType, char** something1 = nullptr);
        static char* ExtractKeyFromLink(char** text, char const* const* linkTypes, int* found_idx = nullptr, char** something1 = nullptr);
        bool  ExtractUint32KeyFromLink(char** text, char const* linkType, uint32& value);

        uint32 ExtractAccountId(char** args, std::string* accountName = nullptr, Player** targetIfNullArg = nullptr);
        uint32 ExtractSpellIdFromLink(char** text);
        ObjectGuid ExtractGuidFromLink(char** text);
        GameTele const* ExtractGameTeleFromLink(char** text);
        bool   ExtractLocationFromLink(char** text, uint32& mapid, float& x, float& y, float& z);
        bool   ExtractRaceMask(char** text, uint32& raceMask, char const** maskName = nullptr);
        std::string ExtractPlayerNameFromLink(char** text);
        bool ExtractPlayerTarget(char** args, Player** player, ObjectGuid* player_guid = nullptr, std::string* player_name = nullptr);

        WorldSession* GetSession() const { return m_session; }
    protected:
        explicit ChatHandler() : m_session(nullptr), sentErrorMessage(false)
        {}      // for CLI subclass

        bool hasStringAbbr(const char* name, const char* part) const;

        // function with different implementation for chat/console
        virtual uint32 GetAccountId() const;
        virtual AccountTypes GetAccessLevel() const;
        virtual bool isAvailable(ChatCommand const& cmd) const;
        virtual std::string GetNameLink() const;
        virtual bool needReportToTarget(Player* chr) const;
        virtual LocaleConstant GetSessionDbcLocale() const;
        virtual int GetSessionDbLocaleIndex() const;

        bool HasLowerSecurity(Player* target, ObjectGuid guid = ObjectGuid(), bool strong = false);
        bool HasLowerSecurityAccount(WorldSession* target, uint32 target_account, bool strong = false);

        void SendGlobalSysMessage(const char* str) const;

        bool SetDataForCommandInTable(ChatCommand* commandTable, const char* text, uint32 security, std::string const& help);
        void ExecuteCommand(const char* text);
        void LogCommand(char const* fullcmd) const;

        bool ShowHelpForCommand(ChatCommand* table, const char* cmd);
        bool ShowHelpForSubCommands(ChatCommand* table, char const* cmd);
        ChatCommandSearchResult FindCommand(ChatCommand* table, char const*& text, ChatCommand*& command, ChatCommand** parentCommand = nullptr, std::string* cmdNamePtr = nullptr, bool allAvailable = false, bool exactlyName = false) const;

        void CheckIntegrity(ChatCommand* table, ChatCommand* parentCommand) const;
        ChatCommand* getCommandTable();

#include "Anticheat/module/AnticheatChatCommandsFunctions.h"

        bool HandleAccountCommand(char* args);
        bool HandleAccountCharactersCommand(char* args);
        bool HandleAccountCreateCommand(char* args);
        bool HandleAccountDeleteCommand(char* args);
        bool HandleAccountLockCommand(char* args);
        bool HandleAccountOnlineListCommand(char* args);
        bool HandleAccountPasswordCommand(char* args);
        bool HandleAccountSetAddonCommand(char* args);
        bool HandleAccountSetGmLevelCommand(char* args);
        bool HandleAccountSetPasswordCommand(char* args);

#ifdef BUILD_AHBOT
        bool HandleAHBotRebuildCommand(char* args);
        bool HandleAHBotReloadCommand(char* args);
        bool HandleAHBotStatusCommand(char* args);
        bool HandleAHBotItemCommand(char* args);
#endif

        bool HandleAuctionAllianceCommand(char* args);
        bool HandleAuctionGoblinCommand(char* args);
        bool HandleAuctionHordeCommand(char* args);
        bool HandleAuctionItemCommand(char* args);
        bool HandleAuctionCommand(char* args);

        bool HandleWarnCharacterCommand(char* args);
        bool HandleAddCharacterNoteCommand(char* args);
        bool HandleBanAccountCommand(char* args);
        bool HandleBanCharacterCommand(char* args);
        bool HandleBanIPCommand(char* args);
        bool HandleBanInfoAccountCommand(char* args);
        bool HandleBanInfoCharacterCommand(char* args);
        bool HandleBanInfoIPCommand(char* args);
        bool HandleBanListAccountCommand(char* args);
        bool HandleBanListCharacterCommand(char* args);
        bool HandleBanListIPCommand(char* args);

        bool HandleCastCommand(char* args);
        bool HandleCastBackCommand(char* args);
        bool HandleCastDistCommand(char* args);
        bool HandleCastSelfCommand(char* args);
        bool HandleCastTargetCommand(char* args);

        bool HandleCharacterDeletedDeleteCommand(char* args);
        bool HandleCharacterDeletedListCommand(char* args);
        bool HandleCharacterDeletedRestoreCommand(char* args);
        bool HandleCharacterDeletedOldCommand(char* args);
        bool HandleCharacterEraseCommand(char* args);
        bool HandleCharacterLevelCommand(char* args);
        bool HandleCharacterRenameCommand(char* args);
        bool HandleCharacterReputationCommand(char* args);
        bool HandleCharacterTitlesCommand(char* args);

        bool HandleChannelListCommand(char* args);
        bool HandleChannelStaticCommand(char* args);

        bool HandleDebugAnimCommand(char* args);
        bool HandleDebugArenaCommand(char* args);
        bool HandleDebugBattlegroundCommand(char* args);
        bool HandleDebugBattlegroundStartCommand(char* args);
        bool HandleDebugListUpdateFieldsCommand(char* args);
        bool HandleDebugGetItemStateCommand(char* args);
        bool HandleDebugGetItemValueCommand(char* args);
        bool HandleDebugGetLootRecipientCommand(char* args);
        bool HandleDebugGetValueByIndexCommand(char* args);
        bool HandleDebugGetValueByNameCommand(char* args);
        bool HandleDebugModItemValueCommand(char* args);
        bool HandleDebugModValueCommand(char* args);
        bool HandleDebugSetAuraStateCommand(char* args);
        bool HandleDebugSetItemValueCommand(char* args);
        bool HandleDebugSetValueByIndexCommand(char* args);
        bool HandleDebugSetValueByNameCommand(char* args);
        bool HandleDebugSpellCheckCommand(char* args);
        bool HandleDebugSpellCoefsCommand(char* args);
        bool HandleDebugSpellModsCommand(char* args);
        bool HandleDebugTaxiCommand(char* /*args*/);
        bool HandleDebugUpdateWorldStateCommand(char* args);
        bool HandleDebugWaypoint(char* args);
        bool HandleDebugByteFields(char* args);
        bool HandleDebugSpellVisual(char* args);
        bool HandleDebugMoveflags(char* args);

        bool LootStatsHelper(char* args, bool full);
        bool HandleLootStatsCommand(char* args);
        bool HandleLootFullStatsCommand(char* args);

        bool HandleDebugOverflowCommand(char* args);
        bool HandleDebugChatFreezeCommand(char* args);

        bool HandleDebugObjectFlags(char* args);
        bool HandleDebugHaveAtClientCommand(char* args);
        bool HandleDebugIsVisibleCommand(char* args);

        bool HandleShowTemporarySpawnList(char* args);
        bool HandleGridsLoadedCount(char* args);

        bool HandleDebugPlayCinematicCommand(char* args);
        bool HandleDebugPlaySoundCommand(char* args);
        bool HandleDebugPlayMusicCommand(char* args);

        bool HandleDebugPetDismissSound(char* args);

        bool HandleDebugAreaTriggersCommand(char* args);

        bool HandleDebugSendBuyErrorCommand(char* args);
        bool HandleDebugSendChannelNotifyCommand(char* args);
        bool HandleDebugSendChatMsgCommand(char* args);
        bool HandleDebugSendEquipErrorCommand(char* args);
        bool HandleDebugSendOpcodeCommand(char* args);
        bool HandleDebugSendPoiCommand(char* args);
        bool HandleDebugSendQuestFailedMsgCommand(char* args);
        bool HandleDebugSendQuestPartyMsgCommand(char* args);
        bool HandleDebugSendQuestInvalidMsgCommand(char* args);
        bool HandleDebugSendSellErrorCommand(char* args);
        bool HandleDebugSendSpellFailCommand(char* args);
        bool HandleDebugSendWorldState(char* args);

        bool HandleDebugFlyCommand(char* args);

        bool HandleDebugOutPacketHistory(char* args);
        bool HandleDebugIncPacketHistory(char* args);

        bool HandleDebugTransports(char* args);

        bool HandleDebugSpawnsList(char* args);
        bool HandleDebugRespawnDynguid(char* args);

        bool HandleDebugPacketLog(char* args);
        bool HandleDebugDbscript(char* args);
        bool HandleDebugDbscriptTargeted(char* args);
        bool HandleDebugDbscriptSourced(char* args);
        bool HandleDebugDbscriptGuided(char* args);

        bool HandleGetItemsAndEnchantIdsCommand(char* args);
        bool HandleGetGemEnchantmentIdCommand(char* args);

        bool HandleSD2HelpCommand(char* args);
        bool HandleSD2ScriptCommand(char* args);

        bool HandleEventListCommand(char* args);
        bool HandleEventStartCommand(char* args);
        bool HandleEventStopCommand(char* args);
        bool HandleEventInfoCommand(char* args);

        bool HandleGameObjectAddCommand(char* args);
        bool HandleGameObjectDeleteCommand(char* args);
        bool HandleGameObjectMoveCommand(char* args);
        bool HandleGameObjectNearCommand(char* args);
        bool HandleGameObjectNearSpawnedCommand(char* args);
        bool HandleGameObjectPhaseCommand(char* args);
        bool HandleGameObjectTargetCommand(char* args);
        bool HandleGameObjectTurnCommand(char* args);
        bool HandleGameObjectActivateCommand(char* args);
        bool HandleGameObjectForcedDespawnCommand(char* args);
        bool HandleGameObjectRespawnCommand(char* args);

        bool HandleGMCommand(char* args);
        bool HandleGMChatCommand(char* args);
        bool HandleGMFlyCommand(char* args);
        bool HandleGMUnkillableCommand(char* args);
        bool HandleGMListFullCommand(char* args);
        bool HandleGMListIngameCommand(char* args);
        bool HandleGMMountUpCommand(char* args);
        bool HandleGMVisibleCommand(char* args);

        bool HandleGoCommand(char* args);
        bool HandleGoCreatureCommand(char* args);
        bool HandleGoGraveyardCommand(char* args);
        bool HandleGoGridCommand(char* args);
        bool HandleGoObjectCommand(char* args);
        bool HandleGoTaxinodeCommand(char* args);
        bool HandleGoTriggerCommand(char* args);
        bool HandleGoWarpCommand(char* args);
        bool HandleGoXYCommand(char* args);
        bool HandleGoXYZCommand(char* args);
        bool HandleGoZoneXYCommand(char* args);
        bool HandleGoNextCommand(char* args);

        bool HandleGuildCreateCommand(char* args);
        bool HandleGuildInviteCommand(char* args);
        bool HandleGuildUninviteCommand(char* args);
        bool HandleGuildRankCommand(char* args);
        bool HandleGuildDeleteCommand(char* args);

        bool HandleHonorAddCommand(char* args);
        bool HandleHonorAddKillCommand(char* args);
        bool HandleHonorUpdateCommand(char* args);

        bool HandleInstanceListBindsCommand(char* args);
        bool HandleInstanceUnbindCommand(char* args);
        bool HandleInstanceStatsCommand(char* args);
        bool HandleInstanceSaveDataCommand(char* args);

        bool HandleLearnCommand(char* args);
        bool HandleLearnAllCommand(char* args);
        bool HandleLearnAllGMCommand(char* args);
        bool HandleLearnAllCraftsCommand(char* args);
        bool HandleLearnAllRecipesCommand(char* args);
        bool HandleLearnAllDefaultCommand(char* args);
        bool HandleLearnAllLangCommand(char* args);
        bool HandleLearnAllMyClassCommand(char* args);
        bool HandleLearnAllMySpellsCommand(char* args);
        bool HandleLearnAllMyTalentsCommand(char* args);

        bool HandleListAreaTriggerCommand(char* args);
        bool HandleListAurasCommand(char* args);
        bool HandleListCreatureCommand(char* args);
        bool HandleListItemCommand(char* args);
        bool HandleListObjectCommand(char* args);
        bool HandleListTalentsCommand(char* args);

        bool HandleLookupAccountEmailCommand(char* args);
        bool HandleLookupAccountIpCommand(char* args);
        bool HandleLookupAccountNameCommand(char* args);
        bool HandleLookupAreaCommand(char* args);
        bool HandleLookupCreatureCommand(char* args);
        bool HandleLookupEventCommand(char* args);
        bool HandleLookupFactionCommand(char* args);
        bool HandleLookupItemCommand(char* args);
        bool HandleLookupItemSetCommand(char* args);
        bool HandleLookupObjectCommand(char* args);
        bool HandleLookupPlayerIpCommand(char* args);
        bool HandleLookupPlayerAccountCommand(char* args);
        bool HandleLookupPlayerEmailCommand(char* args);
        bool HandleLookupPoolCommand(char* args);
        bool HandleLookupQuestCommand(char* args);
        bool HandleLookupSkillCommand(char* args);
        bool HandleLookupSpellCommand(char* args);
        bool HandleLookupTaxiNodeCommand(char* args);
        bool HandleLookupTeleCommand(char* args);
        bool HandleLookupTitleCommand(char* args);

        bool HandleModifyHPCommand(char* args);
        bool HandleModifyManaCommand(char* args);
        bool HandleModifyRageCommand(char* args);
        bool HandleModifyEnergyCommand(char* args);
        bool HandleModifyMoneyCommand(char* args);
        bool HandleModifyASpeedCommand(char* args);
        bool HandleModifySpeedCommand(char* args);
        bool HandleModifyBWalkCommand(char* args);
        bool HandleModifyFlyCommand(char* args);
        bool HandleModifySwimCommand(char* args);
        bool HandleModifyScaleCommand(char* args);
        bool HandleModifyMountCommand(char* args);
        bool HandleModifyFactionCommand(char* args);
        bool HandleModifyTalentCommand(char* args);
        bool HandleModifyHonorCommand(char* args);
        bool HandleModifyRepCommand(char* args);
        bool HandleModifyArenaCommand(char* args);
        bool HandleModifyGenderCommand(char* args);
        bool HandleModifyStrengthCommand(char* args);
        bool HandleModifyAgilityCommand(char* args);
        bool HandleModifyStaminaCommand(char* args);
        bool HandleModifyIntellectCommand(char* args);
        bool HandleModifySpiritCommand(char* args);
        bool HandleModifyArmorCommand(char* args);
        bool HandleModifyHolyCommand(char* args);
        bool HandleModifyFireCommand(char* args);
        bool HandleModifyNatureCommand(char* args);
        bool HandleModifyFrostCommand(char* args);
        bool HandleModifyShadowCommand(char* args);
        bool HandleModifyArcaneCommand(char* args);
        bool HandleModifyMeleeApCommand(char* args);
        bool HandleModifyRangedApCommand(char* args);
        bool HandleModifySpellPowerCommand(char* args);
        bool HandleModifyMeleeCritCommand(char* args);
        bool HandleModifySpellCritCommand(char* args);
        bool HandleModifyMeleeHasteCommand(char* args);
        bool HandleModifyRangedHasteCommand(char* args);
        bool HandleModifySpellHasteCommand(char* args);
        bool HandleModifyBlockCommand(char* args);
        bool HandleModifyDodgeCommand(char* args);
        bool HandleModifyParryCommand(char* args);
        bool ModifyMountCommandHelper(Player* target, char* args);
        bool ModifyStatCommandHelper(char* args, char const* statName, uint32 spellId);

        //-----------------------Npc Commands-----------------------
        bool HandleNpcAddCommand(char* args);
        bool HandleNpcAddVendorItemCommand(char* args);
        bool HandleNpcAIInfoCommand(char* args);
        bool HandleNpcAllowMovementCommand(char* args);
        bool HandleNpcChangeEntryCommand(char* args);
        bool HandleNpcChangeLevelCommand(char* args);
        bool HandleNpcDeleteCommand(char* args);
        bool HandleNpcDelVendorItemCommand(char* args);
        bool HandleNpcFactionIdCommand(char* args);
        bool HandleNpcFlagCommand(char* args);
        bool HandleNpcFollowCommand(char* args);
        bool HandleNpcInfoCommand(char* args);
        bool HandleNpcThreatCommand(char* args);
        bool HandleNpcMoveCommand(char* args);
        bool HandleNpcPlayEmoteCommand(char* args);
        bool HandleNpcSayCommand(char* args);
        bool HandleNpcListSpells(char* args);
        bool HandleNpcShowLootCommand(char* args);
        bool HandleNpcSetModelCommand(char* args);
        bool HandleNpcSetMoveTypeCommand(char* args);
        bool HandleNpcSpawnDistCommand(char* args);
        bool HandleNpcSpawnTimeCommand(char* args);
        bool HandleNpcTameCommand(char* args);
        bool HandleNpcTextEmoteCommand(char* args);
        bool HandleNpcUnFollowCommand(char* args);
        bool HandleNpcWhisperCommand(char* args);
        bool HandleNpcYellCommand(char* args);
        bool HandleNpcTempSpawn(char* args);
        bool HandleNpcEvade(char* args);
        bool HandleNpcDespawn(char* args);
        bool HandleNpcGroupInfoCommand(char* args);
        //bool HandleNpcGroupBehaviorShowCommand(char* args);
        //bool HandleNpcGroupBehaviorSetCommand(char* args);
        bool HandleNpcFormationInfoCommand(char* args);
        bool HandleNpcFormationResetCommand(char* args);
        bool HandleNpcFormationSwitchCommand(char* args);

        // TODO: NpcCommands that needs to be fixed :
        bool HandleNpcAddWeaponCommand(char* args);
        bool HandleNpcNameCommand(char* args);
        bool HandleNpcSubNameCommand(char* args);
        //----------------------------------------------------------

        bool HandlePDumpLoadCommand(char* args);
        bool HandlePDumpWriteCommand(char* args);

        bool HandlePoolListCommand(char* args);
        bool HandlePoolSpawnsCommand(char* args);
        bool HandlePoolInfoCommand(char* args);

        bool HandleQuestAddCommand(char* args);
        bool HandleQuestRemoveCommand(char* args);
        bool HandleQuestCompleteCommand(char* args);

        bool HandleReloadAnticheatCommand(char*);

        bool HandleReloadAllCommand(char* args);
        bool HandleReloadAllAreaCommand(char* args);
        bool HandleReloadAllGossipsCommand(char* args);
        bool HandleReloadAllItemCommand(char* args);
        bool HandleReloadAllLootCommand(char* args);
        bool HandleReloadAllNpcCommand(char* args);
        bool HandleReloadAllQuestCommand(char* args);
        bool HandleReloadAllScriptsCommand(char* args);
        bool HandleReloadAllEventAICommand(char* args);
        bool HandleReloadAllSpellCommand(char* args);
        bool HandleReloadAllLocalesCommand(char* args);

        bool HandleReloadConfigCommand(char* args);

        bool HandleReloadAreaTriggerTavernCommand(char* args);
        bool HandleReloadAreaTriggerTeleportCommand(char* args);
        bool HandleReloadAutoBroadcastCommand(char* args);
        bool HandleReloadChatFilteredCommand(char* args);
        bool HandleReloadBattleEventCommand(char* args);
        bool HandleReloadCommandCommand(char* args);
        bool HandleReloadConditionsCommand(char* args);
        bool HandleReloadCreaturesStatsCommand(char* args);
        bool HandleReloadCreatureQuestRelationsCommand(char* args);
        bool HandleReloadCreatureQuestInvRelationsCommand(char* args);
        bool HandleReloadDBScriptsOnCreatureDeathCommand(char* args);
        bool HandleReloadDBScriptsOnEventCommand(char* args);
        bool HandleReloadDBScriptsOnGossipCommand(char* args);
        bool HandleReloadDBScriptsOnGoUseCommand(char* args);
        bool HandleReloadDBScriptsOnQuestEndCommand(char* args);
        bool HandleReloadDBScriptsOnQuestStartCommand(char* args);
        bool HandleReloadDBScriptsOnSpellCommand(char* args);
        bool HandleReloadDBScriptsOnRelayCommand(char* args);

        bool HandleReloadEventAISummonsCommand(char* args);
        bool HandleReloadEventAIScriptsCommand(char* args);
        bool HandleReloadGameGraveyardZoneCommand(char* args);
        bool HandleReloadGameTeleCommand(char* args);
        bool HandleReloadGossipMenuCommand(char* args);
        bool HandleReloadQuestgiverGreetingCommand(char* args);
        bool HandleReloadTrainerGreetingCommand(char* args);
        bool HandleReloadGOQuestRelationsCommand(char* args);
        bool HandleReloadGOQuestInvRelationsCommand(char* args);
        bool HandleReloadItemEnchantementsCommand(char* args);
        bool HandleReloadItemRequiredTragetCommand(char* args);
        bool HandleReloadLocalesCreatureCommand(char* args);
        bool HandleReloadLocalesGameobjectCommand(char* args);
        bool HandleReloadLocalesGossipMenuOptionCommand(char* args);
        bool HandleReloadLocalesItemCommand(char* args);
        bool HandleReloadLocalesNpcTextCommand(char* args);
        bool HandleReloadLocalesPageTextCommand(char* args);
        bool HandleReloadLocalesPointsOfInterestCommand(char* args);
        bool HandleReloadLocalesQuestCommand(char* args);
        bool HandleReloadLocalesAreaTriggerCommand(char*);
        bool HandleReloadQuestgiverGreetingLocalesCommand(char* args);
        bool HandleReloadLocalesTrainerGreetingCommand(char* args);
        bool HandleReloadLootTemplatesCreatureCommand(char* args);
        bool HandleReloadLootTemplatesDisenchantCommand(char* args);
        bool HandleReloadLootTemplatesFishingCommand(char* args);
        bool HandleReloadLootTemplatesGameobjectCommand(char* args);
        bool HandleReloadLootTemplatesItemCommand(char* args);
        bool HandleReloadLootTemplatesMailCommand(char* args);
        bool HandleReloadLootTemplatesPickpocketingCommand(char* args);
        bool HandleReloadLootTemplatesProspectingCommand(char* args);
        bool HandleReloadLootTemplatesReferenceCommand(char* args);
        bool HandleReloadLootTemplatesSkinningCommand(char* args);
        bool HandleReloadMailLevelRewardCommand(char* args);
        bool HandleReloadMangosStringCommand(char* args);
        bool HandleReloadNpcGossipCommand(char* args);
        bool HandleReloadNpcTextCommand(char* args);
        bool HandleReloadNpcTrainerCommand(char* args);
        bool HandleReloadNpcVendorCommand(char* args);
        bool HandleReloadPageTextsCommand(char* args);
        bool HandleReloadPointsOfInterestCommand(char* args);
        bool HandleReloadQuestAreaTriggersCommand(char* args);
        bool HandleReloadQuestTemplateCommand(char* args);
        bool HandleReloadReservedNameCommand(char* args);
        bool HandleReloadReputationRewardRateCommand(char* args);
        bool HandleReloadReputationSpilloverTemplateCommand(char* args);
        bool HandleReloadSkillDiscoveryTemplateCommand(char* args);
        bool HandleReloadSkillExtraItemTemplateCommand(char* args);
        bool HandleReloadSkillFishingBaseLevelCommand(char* args);
        bool HandleReloadSpellAffectCommand(char* args);
        bool HandleReloadSpellAreaCommand(char* args);
        bool HandleReloadSpellDisabledCommand(char* args);
        bool HandleReloadSpellChainCommand(char* args);
        bool HandleReloadSpellElixirCommand(char* args);
        bool HandleReloadSpellLearnSpellCommand(char* args);
        bool HandleReloadSpellProcEventCommand(char* args);
        bool HandleReloadSpellProcItemEnchantCommand(char* args);
        bool HandleReloadSpellScriptTargetCommand(char* args);
        bool HandleReloadSpellTargetPositionCommand(char* args);
        bool HandleReloadSpellThreatsCommand(char* args);
        bool HandleReloadStringIds(char* args);
        bool HandleReloadTaxiShortcuts(char* args);
        bool HandleReloadSpellPetAurasCommand(char* args);
        bool HandleReloadExpectedSpamRecords(char* args);
        bool HandleReloadCreatureCooldownsCommand(char* args);
        bool HandleReloadCreatureSpellLists(char* args);
        bool HandleReloadSpawnGroupsCommand(char* args);

        bool HandleResetAllCommand(char* args);
        bool HandleResetHonorCommand(char* args);
        bool HandleResetLevelCommand(char* args);
        bool HandleResetSpellsCommand(char* args);
        bool HandleResetStatsCommand(char* args);
        bool HandleResetTalentsCommand(char* args);
        bool HandleResetTaxiNodesCommand(char* args);
        bool HandleResetModsCommand(char* args);

        bool HandleSendItemsCommand(char* args);
        bool HandleSendMailCommand(char* args);
        bool HandleSendMessageCommand(char* args);
        bool HandleSendMoneyCommand(char* args);

        bool HandleSendMassItemsCommand(char* args);
        bool HandleSendMassMailCommand(char* args);
        bool HandleSendMassMoneyCommand(char* args);

        bool HandleServerCorpsesCommand(char* args);
        bool HandleServerExitCommand(char* args);
        bool HandleServerIdleRestartCommand(char* args);
        bool HandleServerIdleShutDownCommand(char* args);
        bool HandleServerInfoCommand(char* args);
        bool HandleServerLogFilterCommand(char* args);
        bool HandleServerLogLevelCommand(char* args);
        bool HandleServerMotdCommand(char* args);
        bool HandleServerPLimitCommand(char* args);
        bool HandleServerResetAllRaidCommand(char* args);
        bool HandleServerRestartCommand(char* args);
        bool HandleServerSetMotdCommand(char* args);
        bool HandleServerShutDownCommand(char* args);
        bool HandleServerShutDownCancelCommand(char* args);

        bool HandleTeleCommand(char* args);
        bool HandleTeleAddCommand(char* args);
        bool HandleTeleDelCommand(char* args);
        bool HandleTeleGroupCommand(char* args);
        bool HandleTeleNameCommand(char* args);

        bool HandleTitlesAddCommand(char* args);
        bool HandleTitlesCurrentCommand(char* args);
        bool HandleTitlesRemoveCommand(char* args);
        bool HandleTitlesSetMaskCommand(char* args);
        bool HandleTitlesSwapCommand(char* args);

        bool HandleTriggerActiveCommand(char* args);
        bool HandleTriggerNearCommand(char* args);
        bool HandleTriggerCommand(char* args);

        bool HandleUnBanAccountCommand(char* args);
        bool HandleUnBanCharacterCommand(char* args);
        bool HandleUnBanIPCommand(char* args);

        bool HandleWpAddCommand(char* args);
        bool HandleWpModifyCommand(char* args);
        bool HandleWpShowCommand(char* args);
        bool HandleWpExportCommand(char* args);

        bool HandleHelpCommand(char* args);
        bool HandleCommandsCommand(char* args);
        bool HandleStartCommand(char* args);
        bool HandleDismountCommand(char* args);
        bool HandleSaveCommand(char* args);
        bool HandleTogglePvpMessages(char* args);

        bool HandleNamegoCommand(char* args);
        bool HandleGonameCommand(char* args);
        bool HandleGroupgoCommand(char* args);
        bool HandleRecallCommand(char* args);
        bool HandleAnnounceCommand(char* args);
        bool HandleNotifyCommand(char* args);
        bool HandleGPSCommand(char* args);
        bool HandleTaxiCheatCommand(char* args);
        bool HandleWhispersCommand(char* args);
        bool HandleWhisperRestrictionCommand(char* args);
        bool HandleModifyDrunkCommand(char* args);
        bool HandleSetViewCommand(char* args);

        bool HandleLoadScriptsCommand(char* args);

        bool HandleGUIDCommand(char* args);
        bool HandleItemMoveCommand(char* args);
        bool HandleDeMorphCommand(char* args);
        bool HandlePInfoCommand(char* args);
        bool HandleMuteCommand(char* args);
        bool HandleUnmuteCommand(char* args);

        bool HandleMovegensCommand(char* args);
        bool HandleComeToMeCommand(char* args);
        bool HandleMovespeedShowCommand(char* args);
        bool HandleDebugMovement(char* args);
        bool HandlePrintMovement(char* args);

        bool HandleCooldownListCommand(char* args);
        bool HandleCooldownClearCommand(char* args);
        bool HandleCooldownClearClientSideCommand(char* args);
        bool HandleCooldownClearArenaCommand(char* args);
        bool HandleUnLearnCommand(char* args);
        bool HandleGetDistanceCommand(char* args);
        bool HandleGetLosCommand(char* args);
        bool HandleModifyStandStateCommand(char* args);
        bool HandleDieCommand(char* args);
        bool HandleDamageCommand(char* args);
        bool HandleReviveCommand(char* args);
        bool HandleModifyMorphCommand(char* args);
        bool HandleAuraCommand(char* args);
        bool HandleUnAuraCommand(char* args);
        bool HandleLinkGraveCommand(char* args);
        bool HandleNearGraveCommand(char* args);
        bool HandleExploreCheatCommand(char* args);
        bool HandleLevelUpCommand(char* args);
        bool HandleShowAreaCommand(char* args);
        bool HandleHideAreaCommand(char* args);
        bool HandleAddItemCommand(char* args);
        bool HandleAddItemSetCommand(char* args);

        bool HandleBankCommand(char* args);
        bool HandleChangeWeatherCommand(char* args);
        bool HandleKickPlayerCommand(char* args);

        bool HandleBagsCommand(char* args);

        bool HandleCombatListCommand(char* args);
        bool HandleCombatStopCommand(char* args);

        bool HandleTicketDiscardCommand(char* args);
        bool HandleTicketEscalateCommand(char* args);
        bool HandleTicketGoCommand(char* args);
        bool HandleTicketGoNameCommand(char* args);
        bool HandleTicketNoteCommand(char* args);
        bool HandleTicketReadCommand(char* args);
        bool HandleTicketResolveCommand(char* args);
        bool HandleTicketSortCommand(char* args);
        bool HandleTicketWhisperCommand(char* args);
        bool HandleTicketCommand(char* args);

        bool HandleTicketsListCommand(char* args);
        bool HandleTicketsQueueCommand(char* args);
        bool HandleTicketsCommand(char* args);

        bool HandleMaxSkillCommand(char* args);
        bool HandleSetSkillCommand(char* args);
        bool HandleRespawnCommand(char* args);
        bool HandleRepairitemsCommand(char* args);
        bool HandleStableCommand(char* args);
        bool HandleWaterwalkCommand(char* args);
        bool HandleQuitCommand(char* args);
#ifdef BUILD_DEPRECATED_PLAYERBOT
        bool HandlePlayerbotCommand(char* args);
#endif

#ifdef ENABLE_PLAYERBOTS
        bool HandlePlayerbotCommand(char* args);
        bool HandleRandomPlayerbotCommand(char* args);
        bool HandleAhBotCommand(char* args);
        bool HandlePerfMonCommand(char* args);
#endif

        bool HandleArenaFlushPointsCommand(char* args);
        bool HandleArenaSeasonRewardsCommand(char* args);
        bool HandleArenaDataReset(char* args);
        bool HandleArenaTeamPointSet(char* args);

        bool HandleMmapPathCommand(char* args);
        bool HandleMmapLocCommand(char* args);
        bool HandleMmapLoadedTilesCommand(char* args);
        bool HandleMmapStatsCommand(char* args);
        bool HandleMmap(char* args);
        bool HandleMmapTestArea(char* args);
        bool HandleMmapTestHeight(char* args);

        bool HandleLinkAddCommand(char* args);
        bool HandleLinkRemoveCommand(char* args);
        bool HandleLinkEditCommand(char* args);
        bool HandleLinkToggleCommand(char* args);
        bool HandleLinkCheckCommand(char* args);

        // worldstate
        bool HandleVariablePrint(char* args);
        bool HandleWarEffortCommand(char* args);
        bool HandleWarEffortPhaseCommand(char* args);
        bool HandleWarEffortCounterCommand(char* args);
        bool HandleScourgeInvasionCommand(char* args);
        bool HandleScourgeInvasionStateCommand(char* args);
        bool HandleScourgeInvasionBattlesWonCommand(char* args);
        bool HandleScourgeInvasionStartZone(char* args);
        bool HandleSunsReachReclamationPhaseCommand(char* args);
        bool HandleSunsReachReclamationSubPhaseCommand(char* args);
        bool HandleSunsReachReclamationCounterCommand(char* args);
        bool HandleSunwellGateCommand(char* args);
        bool HandleSunwellGateCounterCommand(char* args);
        bool HandleExpansionRelease(char* args);
        bool HandleSetVariable(char* args);

        // Battleground
        bool HandleBattlegroundStartCommand(char* args);
        bool HandleBattlegroundStopCommand(char* args);

        //! Development Commands
        bool HandleSaveAllCommand(char* args);

        bool HandlePetLevelLoyaltyCommand(char* args);

        // Custom
        bool HandleXPCommandSet(char* args);
        bool HandleXPCommandCurrent(char* args);
        bool HandleXPCommandAvailable(char* args);
        bool HandleTbcRacesBoostRestriction(char* args); // custom voa
        bool HandleDeserterCommand(char* args);
        bool HandleUnDeserterCommand(char* args);
        bool HandleServerFirstPrint(char* args);

        Player*   getSelectedPlayer() const;
        Unit*     getSelectedUnit(bool self = true) const;
        Creature* getSelectedCreature() const;
        Pet*      getSelectedPet() const;

        std::string petLink(std::string const& name) const { return m_session ? "|cffffffff|Hpet:" + name + "|h[" + name + "]|h|r" : name; }
        std::string playerLink(std::string const& name) const { return m_session ? "|cffffffff|Hplayer:" + name + "|h[" + name + "]|h|r" : name; }
        std::string GetNameLink(Player* chr) const;

        GameObject* GetGameObjectWithGuid(uint32 lowguid, uint32 entry) const;
        
        bool HandleFaceMeCommand(char* args);

        // Utility methods for commands
        bool ShowAccountListHelper(std::unique_ptr<QueryResult> queryResult, uint32* limit = nullptr, bool title = true, bool error = true);
        void ShowFactionListHelper(FactionEntry const* factionEntry, LocaleConstant loc, FactionState const* repState = nullptr, Player* target = nullptr);
        void ShowItemListHelper(uint32 itemId, int loc_idx, Player* target = nullptr);
        void ShowQuestListHelper(uint32 questId, int32 loc_idx, Player* target = nullptr);
        bool ShowPlayerListHelper(std::unique_ptr<QueryResult> queryResult, uint32* limit = nullptr, bool title = true, bool error = true);
        void ShowSpellListHelper(Player* target, SpellEntry const* spellInfo, LocaleConstant loc);
        void ShowPoolListHelper(uint16 pool_id);
        void ShowTriggerListHelper(AreaTriggerEntry const* atEntry);
        void ShowTriggerTargetListHelper(uint32 id, AreaTrigger const* at, bool subpart = false);
        bool LookupPlayerSearchCommand(std::unique_ptr<QueryResult> queryResult, uint32* limit = nullptr);
        bool HandleBanListHelper(std::unique_ptr<QueryResult> queryResult);
        bool HandleBanHelper(BanMode mode, char* args);
        bool HandleBanInfoHelper(uint32 accountid, char const* accountname);
        bool HandleUnBanHelper(BanMode mode, char* args);
        void HandleCharacterLevel(Player* player, ObjectGuid player_guid, uint32 oldlevel, uint32 newlevel);
        void HandleLearnSkillRecipesHelper(Player* player, uint32 skill_id);
        bool HandleGoHelper(Player* _player, uint32 mapid, float x, float y, float const* zPtr = nullptr, float const* ortPtr = nullptr);
        bool HandleGetValueHelper(Object* target, uint32 field, char* typeStr);
        void ShowAllUpdateFieldsHelper(Object const* target);
        void ShowUpdateFieldHelper(Object const* target, uint16 index);
        bool HandlerDebugModValueHelper(Object* target, uint32 field, char* typeStr, char* valStr);
        bool HandleSetValueHelper(Object* target, uint32 field, char* typeStr, char* valStr);

        bool HandleSendItemsHelper(MailDraft& draft, char* args);
        bool HandleSendMailHelper(MailDraft& draft, char* args);
        bool HandleSendMoneyHelper(MailDraft& draft, char* args);

        template<typename T>
        void ShowNpcOrGoSpawnInformation(uint32 guid);
        template <typename T>
        std::string PrepareStringNpcOrGoSpawnInformation(uint32 guid) const;

        /**
         * Stores informations about a deleted character
         */
        struct DeletedInfo
        {
            uint32      lowguid;                            ///< the low GUID from the character
            std::string name;                               ///< the character name
            uint32      accountId;                          ///< the account id
            std::string accountName;                        ///< the account name
            time_t      deleteDate;                         ///< the date at which the character has been deleted
        };

        typedef std::list<DeletedInfo> DeletedInfoList;
        bool GetDeletedCharacterInfoList(DeletedInfoList& foundList, std::string searchString = "");
        void HandleCharacterDeletedListHelper(DeletedInfoList const& foundList);
        void HandleCharacterDeletedRestoreHelper(DeletedInfo const& delInfo);

        void SetSentErrorMessage(bool val) { sentErrorMessage = val;};
    private:
        WorldSession* m_session;                            // != nullptr for chat command call and nullptr for CLI command

        // common global flag
        static bool load_command_table;
        bool sentErrorMessage;
};

class CliHandler : public ChatHandler
{
    private:
        typedef std::function<void(const char*)> Print;
        uint32 m_accountId;
        AccountTypes m_loginAccessLevel;
        Print m_print;

    public:
        CliHandler(uint32 accountId, AccountTypes accessLevel, Print zprint)
            : m_accountId(accountId), m_loginAccessLevel(accessLevel), m_print(std::move(zprint)) {}

        // overwrite functions
        const char* GetMangosString(int32 entry) const override;
        uint32 GetAccountId() const override;
        AccountTypes GetAccessLevel() const override;
        bool isAvailable(ChatCommand const& cmd) const override;
        void SendSysMessage(const char* str) override;
        std::string GetNameLink() const override;
        bool needReportToTarget(Player* chr) const override;
        LocaleConstant GetSessionDbcLocale() const override;
        int GetSessionDbLocaleIndex() const override;
};

#endif
