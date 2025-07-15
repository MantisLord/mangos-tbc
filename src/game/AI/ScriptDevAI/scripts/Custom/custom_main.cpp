#include "AI/ScriptDevAI/include/sc_common.h"
#include "Guilds/Guild.h"
#include "Guilds/GuildMgr.h"
#include "World/World.h"
#include "GameEvents/GameEventMgr.h"
#include "World/WorldState.h"
#include "beastmaster.h"
#include "azshara_crater.h"
#include "custom_main.h"

/*
    RACE_HUMAN              = 1,
    RACE_ORC                = 2,
    RACE_DWARF              = 3,
    RACE_NIGHTELF           = 4,
    RACE_UNDEAD             = 5,
    RACE_TAUREN             = 6,
    RACE_GNOME              = 7,
    RACE_TROLL              = 8,
    RACE_GOBLIN             = 9,
    RACE_BLOODELF           = 10,
    RACE_DRAENEI            = 11,
    RACE_FEL_ORC            = 12,
    RACE_NAGA               = 13,
    RACE_BROKEN             = 14,
    RACE_SKELETON           = 15,
    RACE_VRYKUL             = 16,
    RACE_TUSKARR            = 17,
    RACE_FOREST_TROLL       = 18,

    CLASS_WARRIOR       = 1,
    CLASS_PALADIN       = 2,
    CLASS_HUNTER        = 3,
    CLASS_ROGUE         = 4,
    CLASS_PRIEST        = 5,
    // CLASS_DEATH_KNIGHT = 6,                             // not listed in DBC, will be in 3.0
    CLASS_SHAMAN        = 7,
    CLASS_MAGE          = 8,
    CLASS_WARLOCK       = 9,
    // CLASS_MONK       = 10,                              // not listed in DBC, will be in 5.0
    CLASS_DRUID         = 11,
    // CLASS_DEMON_HUNTER = 12,                            // not listed in DBC, will be in 7.0
*/

enum
{
    NPC_TEXT_PLASTIC_SURGERY_SELECTIONS     = 42303,
    NPC_TEXT_PLASTIC_SURGERY_MAIN_MENU_1    = 42304,
    NPC_TEXT_PLASTIC_SURGERY_MAIN_MENU_2    = 42305,
    NPC_TEXT_PLASTIC_SURGERY_MAIN_MENU_3    = 42306,
    NPC_TEXT_PLASTIC_SURGERY_MAIN_MENU_4    = 42307,
    NPC_TEXT_PLASTIC_SURGERY_RACE_CHANGE    = 42308,
    SPELL_REND                              = 11977,
};

const int surgeonEmotes[4] = { EMOTE_ONESHOT_ATTACKUNARMED, EMOTE_ONESHOT_ATTACK1H, EMOTE_ONESHOT_ATTACK2HTIGHT, EMOTE_ONESHOT_ATTACK2HLOOSE };
const int woundEmotes[2] = { EMOTE_ONESHOT_WOUND, EMOTE_ONESHOT_WOUNDCRITICAL };
const int barberEmotes[3] = { EMOTE_ONESHOT_POINT, EMOTE_ONESHOT_READYBOW, EMOTE_ONESHOT_PARRYUNARMED };
const int surgeonMainMenuTexts[4] = { NPC_TEXT_PLASTIC_SURGERY_MAIN_MENU_1, NPC_TEXT_PLASTIC_SURGERY_MAIN_MENU_2, NPC_TEXT_PLASTIC_SURGERY_MAIN_MENU_3, NPC_TEXT_PLASTIC_SURGERY_MAIN_MENU_4 };

static void PlaySurgeryVisual(Player* player, Creature* creature)
{
    creature->SetFacingToObject(player);
    creature->HandleEmote(surgeonEmotes[urand(0, 3)]);
    creature->CastSpell(player, SPELL_REND, TRIGGERED_OLD_TRIGGERED);
    player->HandleEmote(woundEmotes[urand(0, 1)]);
}

static uint8 GetByteMaxValue(Player* player, uint16 index, uint8 offset)
{
    uint8 max = 255;
    switch (index)
    {
        case PLAYER_BYTES:
        {
            switch (offset)
            {
                case 0:
                    max = maxSkins[player->getRace()].maxMale;
                    if (player->getGender() == GENDER_FEMALE)
                        max = maxSkins[player->getRace()].maxFemale;
                    break;
                case 1:
                    max = maxFaces[player->getRace()].maxMale;
                    if (player->getGender() == GENDER_FEMALE)
                        max = maxFaces[player->getRace()].maxFemale;
                    break;
                case 2:
                    max = maxHairStyles[player->getRace()].maxMale;
                    if (player->getGender() == GENDER_FEMALE)
                        max = maxHairStyles[player->getRace()].maxFemale;
                    break;
                case 3:
                    max = maxHairColor[player->getRace()];
                    break;
            }
            break;
        }
        case PLAYER_BYTES_2:
        {
            if (offset == 0)
            {
                max = maxFacialFeatures[player->getRace()].maxMale;
                if (player->getGender() == GENDER_FEMALE)
                    max = maxFacialFeatures[player->getRace()].maxFemale;
            }
            break;
        }
        case UNIT_FIELD_BYTES_0:
        {
            if (offset == UNIT_BYTES_0_OFFSET_GENDER)
                max = 1;
            break;
        }
    }
    return max;
}

static uint8 GetNextCustomizationIndex(uint8 current, uint8 max)
{
    return current + 1 > max ? 0 : current + 1;
}
static uint8 GetPrevCustomizationIndex(uint8 current, uint8 max)
{
    return current - 1 < 0 ? max : current - 1;
}

static void UpdatePlayerByte(Player* player, Creature* creature, uint16 index, uint8 offset, bool isNext)
{
    uint8 currentVal = player->GetByteValue(index, offset);
    uint8 maxVal = GetByteMaxValue(player, index, offset);
    uint8 newVal = isNext ? GetNextCustomizationIndex(currentVal, maxVal) : GetPrevCustomizationIndex(currentVal, maxVal);

    if (index == UNIT_FIELD_BYTES_0 && offset == UNIT_BYTES_0_OFFSET_GENDER)
        player->SetUInt16Value(PLAYER_BYTES_3, 0, uint16(newVal) | (player->GetDrunkValue() & 0xFFFE));

    player->SetByteValue(index, offset, newVal);

    player->InitDisplayIds();
    player->SendCreateUpdateToPlayer(player);
    WorldPacket data(SMSG_FORCE_DISPLAY_UPDATE, 8);
    data << player->GetObjectGuid();
    player->GetSession()->SendPacket(data, true);
}

static const char* GetOptionText(bool isNext, char const* baseText, uint8 current, uint8 max)
{
    uint8 next = GetNextCustomizationIndex(current, max);
    uint8 prev = GetPrevCustomizationIndex(current, max);
    static char textBuffer[128];
    std::snprintf(textBuffer, sizeof(textBuffer), isNext ? "next %s [%u/%u] -- %u" : "prev %s [%u/%u] -- %u", baseText, current, max, isNext ? next : prev);
    const char* text = textBuffer;
    return text;
}

static bool GossipHello_plastic_surgeon(Player* player, Creature* creature)
{
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Begin character customization", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 100);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Close", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 101);
    player->SEND_GOSSIP_MENU(surgeonMainMenuTexts[urand(0, 3)], creature->GetObjectGuid());
    return true;
}

static bool GossipSelect_plastic_surgeon(Player* player, Creature* creature, uint32 sender, uint32 action)
{
    switch (action)
    {
        case GOSSIP_ACTION_INFO_DEF + 100:
        {
            if (!player->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_HIDE_HELM))
                player->ToggleFlag(PLAYER_FLAGS, PLAYER_FLAGS_HIDE_HELM);

            uint8 skin = player->GetByteValue(PLAYER_BYTES, 0);
            uint8 skinMax = GetByteMaxValue(player, PLAYER_BYTES, 0);
            uint8 face = player->GetByteValue(PLAYER_BYTES, 1);
            uint8 faceMax = GetByteMaxValue(player, PLAYER_BYTES, 1);
            uint8 hair = player->GetByteValue(PLAYER_BYTES, 2);
            uint8 hairMax = GetByteMaxValue(player, PLAYER_BYTES, 2);
            uint8 hairColor = player->GetByteValue(PLAYER_BYTES, 3);
            uint8 hairColorMax = GetByteMaxValue(player, PLAYER_BYTES, 3);
            uint8 faceFeature = player->GetByteValue(PLAYER_BYTES_2, 0);
            uint8 faceFeatureMax = GetByteMaxValue(player, PLAYER_BYTES_2, 0);
            
            char const* hairText = "hair style";
            if (player->getRace() == RACE_TAUREN)
                hairText = "horns";
            
            char const* hairColorText = "hair color";
            if (player->getRace() == RACE_TAUREN)
                hairColorText = "horn color";

            char const* faceFeatureText = "facial hair style";
            switch (player->getRace())
            {
                case RACE_ORC:
                case RACE_HUMAN:
                    if (player->getGender() == GENDER_FEMALE)
                        faceFeatureText = "piercings";
                    break;
                case RACE_DWARF:
                case RACE_GNOME:
                case RACE_BLOODELF:
                    if (player->getGender() == GENDER_FEMALE)
                        faceFeatureText = "earrings";
                    break;
                case RACE_NIGHTELF:
                    if (player->getGender() == GENDER_FEMALE)
                        faceFeatureText = "tattoos";
                    break;
                case RACE_UNDEAD:
                    faceFeatureText = "head accessory";
                    break;
                case RACE_TAUREN:
                    if (player->getGender() == GENDER_FEMALE)
                        faceFeatureText = "hair";
                    break;
                case RACE_TROLL:
                    faceFeatureText = "tusks";
                    break;
                case RACE_DRAENEI:
                    player->getGender() == GENDER_FEMALE ? faceFeatureText = "horns" : faceFeatureText = "tentacles";
                    break;
            }
            if (player->getGender() == GENDER_MALE)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Gender reassignment: Female", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1000);
            else
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Gender reassignment: Male", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1001);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Change race", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1002);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, GetOptionText(true, "skin", skin, skinMax), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1003);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, GetOptionText(false, "skin", skin, skinMax), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1004);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, GetOptionText(true, "face", face, faceMax), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1005);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, GetOptionText(false, "face", face, faceMax), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1006);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, GetOptionText(true, hairText, hair, hairMax), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1007);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, GetOptionText(false, hairText, hair, hairMax), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1008);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, GetOptionText(true, hairColorText, hairColor, hairColorMax), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1009);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, GetOptionText(false, hairColorText, hairColor, hairColorMax), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1010);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, GetOptionText(true, faceFeatureText, faceFeature, faceFeatureMax), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1011);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, GetOptionText(false, faceFeatureText, faceFeature, faceFeatureMax), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1012);
            player->SEND_GOSSIP_MENU(NPC_TEXT_PLASTIC_SURGERY_SELECTIONS, creature->GetObjectGuid());
            return true;
        }
        case GOSSIP_ACTION_INFO_DEF + 101:
        {
            player->CLOSE_GOSSIP_MENU();
            return true;
        }
        case GOSSIP_ACTION_INFO_DEF + 1000: // next gender
        case GOSSIP_ACTION_INFO_DEF + 1001: // prev gender
        {
            PlaySurgeryVisual(player, creature);
            UpdatePlayerByte(player, creature, UNIT_FIELD_BYTES_0, UNIT_BYTES_0_OFFSET_GENDER, action == GOSSIP_ACTION_INFO_DEF + 1000 ? true : false);
            GossipSelect_plastic_surgeon(player, creature, sender, GOSSIP_ACTION_INFO_DEF + 100);
            return true;
        }
        case GOSSIP_ACTION_INFO_DEF + 1002:
        {
            switch (player->getClass())
            {
                case CLASS_WARRIOR:
                    if (player->getRace() != RACE_HUMAN) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Human", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_HUMAN);
                    if (player->getRace() != RACE_ORC) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Orc", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_ORC);
                    if (player->getRace() != RACE_DWARF) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Dwarf", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_DWARF);
                    if (player->getRace() != RACE_NIGHTELF) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Night Elf", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_NIGHTELF);
                    if (player->getRace() != RACE_UNDEAD) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Undead", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_UNDEAD);
                    if (player->getRace() != RACE_TAUREN) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Tauren", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_TAUREN);
                    if (player->getRace() != RACE_GNOME) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Gnome", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_GNOME);
                    if (player->getRace() != RACE_TROLL) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Troll", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_TROLL);
                    if (player->getRace() != RACE_DRAENEI) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Draenei", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_DRAENEI);
                    break;
                case CLASS_PALADIN:
                    if (player->getRace() != RACE_HUMAN) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Human", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_HUMAN);
                    if (player->getRace() != RACE_DWARF) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Dwarf", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_DWARF);
                    if (player->getRace() != RACE_DRAENEI) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Draenei", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_DRAENEI);
                    if (player->getRace() != RACE_BLOODELF) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Blood Elf", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_BLOODELF);
                    break;
                case CLASS_HUNTER:
                    if (player->getRace() != RACE_ORC) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Orc", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_ORC);
                    if (player->getRace() != RACE_DWARF) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Dwarf", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_DWARF);
                    if (player->getRace() != RACE_NIGHTELF) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Night Elf", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_NIGHTELF);
                    if (player->getRace() != RACE_TAUREN) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Tauren", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_TAUREN);
                    if (player->getRace() != RACE_TROLL) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Troll", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_TROLL);
                    if (player->getRace() != RACE_DRAENEI) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Draenei", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_DRAENEI);
                    if (player->getRace() != RACE_BLOODELF) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Blood Elf", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_BLOODELF);
                    break;
                case CLASS_ROGUE:
                    if (player->getRace() != RACE_HUMAN) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Human", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_HUMAN);
                    if (player->getRace() != RACE_ORC) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Orc", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_ORC);
                    if (player->getRace() != RACE_DWARF) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Dwarf", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_DWARF);
                    if (player->getRace() != RACE_NIGHTELF) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Night Elf", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_NIGHTELF);
                    if (player->getRace() != RACE_UNDEAD) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Undead", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_UNDEAD);
                    if (player->getRace() != RACE_GNOME) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Gnome", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_GNOME);
                    if (player->getRace() != RACE_TROLL) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Troll", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_TROLL);
                    if (player->getRace() != RACE_BLOODELF) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Blood Elf", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_BLOODELF);
                    break;
                case CLASS_PRIEST:
                    if (player->getRace() != RACE_HUMAN) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Human", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_HUMAN);
                    if (player->getRace() != RACE_DWARF) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Dwarf", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_DWARF);
                    if (player->getRace() != RACE_NIGHTELF) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Night Elf", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_NIGHTELF);
                    if (player->getRace() != RACE_UNDEAD) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Undead", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_UNDEAD);
                    if (player->getRace() != RACE_BLOODELF) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Blood Elf", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_BLOODELF);
                    break;
                case CLASS_SHAMAN:
                    if (player->getRace() != RACE_ORC) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Orc", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_ORC);
                    if (player->getRace() != RACE_TAUREN) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Tauren", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_TAUREN);
                    if (player->getRace() != RACE_TROLL) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Troll", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_TROLL);
                    if (player->getRace() != RACE_DRAENEI) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Draenei", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_DRAENEI);
                    if (player->getRace() != RACE_BLOODELF) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Blood Elf", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_BLOODELF);
                    break;
                case CLASS_MAGE:
                    if (player->getRace() != RACE_HUMAN) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Human", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_HUMAN);
                    if (player->getRace() != RACE_UNDEAD) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Undead", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_UNDEAD);
                    if (player->getRace() != RACE_GNOME) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Gnome", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_GNOME);
                    if (player->getRace() != RACE_TROLL) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Troll", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_TROLL);
                    if (player->getRace() != RACE_DRAENEI) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Draenei", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_DRAENEI);
                    if (player->getRace() != RACE_BLOODELF) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Blood Elf", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_BLOODELF);
                    break;
                case CLASS_WARLOCK:
                    if (player->getRace() != RACE_HUMAN) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Human", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_HUMAN);
                    if (player->getRace() != RACE_UNDEAD) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Undead", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_UNDEAD);
                    if (player->getRace() != RACE_GNOME) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Gnome", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_GNOME);
                    if (player->getRace() != RACE_TROLL) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Troll", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_TROLL);
                    if (player->getRace() != RACE_DRAENEI) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Draenei", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_DRAENEI);
                    if (player->getRace() != RACE_BLOODELF) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Blood Elf", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_BLOODELF);
                    break;
                case CLASS_DRUID:
                    if (player->getRace() != RACE_NIGHTELF) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Night Elf", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_NIGHTELF);
                    if (player->getRace() != RACE_TAUREN) player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Tauren", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + RACE_TAUREN);
                    break;
            }
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Return to main menu", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 100);
            player->SEND_GOSSIP_MENU(NPC_TEXT_PLASTIC_SURGERY_RACE_CHANGE, creature->GetObjectGuid());
            return true;
        }
        case GOSSIP_ACTION_INFO_DEF + 1003: // next skin
        case GOSSIP_ACTION_INFO_DEF + 1004: // prev skin
        {
            PlaySurgeryVisual(player, creature);
            UpdatePlayerByte(player, creature, PLAYER_BYTES, 0, action == GOSSIP_ACTION_INFO_DEF + 1003 ? true : false);
            GossipSelect_plastic_surgeon(player, creature, sender, GOSSIP_ACTION_INFO_DEF + 100);
            return true;
        }
        case GOSSIP_ACTION_INFO_DEF + 1005: // next face
        case GOSSIP_ACTION_INFO_DEF + 1006: // prev face
        {
            PlaySurgeryVisual(player, creature);
            UpdatePlayerByte(player, creature, PLAYER_BYTES, 1, action == GOSSIP_ACTION_INFO_DEF + 1005 ? true : false);
            GossipSelect_plastic_surgeon(player, creature, sender, GOSSIP_ACTION_INFO_DEF + 100);
            return true;
        }
        case GOSSIP_ACTION_INFO_DEF + 1007: // next hair
        case GOSSIP_ACTION_INFO_DEF + 1008: // prev hair
        {
            creature->HandleEmote(barberEmotes[urand(0, 2)]);
            UpdatePlayerByte(player, creature, PLAYER_BYTES, 2, action == GOSSIP_ACTION_INFO_DEF + 1007 ? true : false);
            GossipSelect_plastic_surgeon(player, creature, sender, GOSSIP_ACTION_INFO_DEF + 100);
            return true;
        }
        case GOSSIP_ACTION_INFO_DEF + 1009: // next hair color
        case GOSSIP_ACTION_INFO_DEF + 1010: // prev hair color
        {
            creature->HandleEmote(barberEmotes[urand(0, 2)]);
            UpdatePlayerByte(player, creature, PLAYER_BYTES, 3, action == GOSSIP_ACTION_INFO_DEF + 1009 ? true : false);
            GossipSelect_plastic_surgeon(player, creature, sender, GOSSIP_ACTION_INFO_DEF + 100);
            return true;
        }
        case GOSSIP_ACTION_INFO_DEF + 1011: // next face feature
        case GOSSIP_ACTION_INFO_DEF + 1012: // prev face facture
        {
            creature->HandleEmote(barberEmotes[urand(0, 2)]);
            UpdatePlayerByte(player, creature, PLAYER_BYTES_2, 0, action == GOSSIP_ACTION_INFO_DEF + 1011 ? true : false);
            GossipSelect_plastic_surgeon(player, creature, sender, GOSSIP_ACTION_INFO_DEF + 100);
            return true;
        }
        default:
            uint8 newRace = action - GOSSIP_ACTION_INFO_DEF;
            if (!player->ChangeRace(newRace))
                player->GetSession()->SendNotification("Invalid race for class.");
            else
                PlaySurgeryVisual(player, creature);
            GossipSelect_plastic_surgeon(player, creature, sender, GOSSIP_ACTION_INFO_DEF + 1002);
            return true;
    }
}

enum
{
    NPC_TEXT_APPEARANCE_COPIER  = 42309,
    SPELL_VANISH_VISUAL         = 24222,
};

const int appearanceCopierDisplayIds[6] = { 15771, 15767, 15886, 15887, 15897, 15891 };

static bool GossipHello_appearance_copier(Player* player, Creature* creature)
{
    creature->SetDisplayId(appearanceCopierDisplayIds[urand(0, 5)]);
    player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, "Copy appearance from player", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1, "Enter player name and click Accept button in next pop-up. Do NOT use the Enter key.", 0, true);
    player->SEND_GOSSIP_MENU(NPC_TEXT_APPEARANCE_COPIER, creature->GetObjectGuid());
    return true;
}

static bool GossipSelectWithCode_appearance_copier(Player* player, Creature* creature, uint32 sender, uint32 action, const char* sCode)
{
    if (sender != GOSSIP_SENDER_MAIN)
        return false;
    if (player->IsInCombat() || player->InBattleGroundQueue())
    {
        player->CLOSE_GOSSIP_MENU();
        player->GetSession()->SendNotification("Cannot complete action while in combat or BG queue.");
        return false;
    }
    if (action == GOSSIP_ACTION_INFO_DEF + 1)
    {
        if (strlen(sCode) == 0)
            return false;

        // prevent SQL injection
        std::string strName = sCode;
        strName.erase(std::remove_if(strName.begin(), strName.end(), &::isspace), strName.end());
        CharacterDatabase.escape_string(strName);

        std::string pNameLower = player->GetName();
        std::string strNameLower = sCode;
        std::transform(pNameLower.begin(), pNameLower.end(), pNameLower.begin(), ::tolower);
        std::transform(strNameLower.begin(), strNameLower.end(), strNameLower.begin(), ::tolower);

        if (strName == pNameLower)
        {
            player->GetSession()->SendNotification("You can't change into \"%s\". That's you! :rolling_eyes:", strName.c_str());
            player->CLOSE_GOSSIP_MENU();
            return false;
        }

        auto result = CharacterDatabase.PQuery("SELECT playerBytes, playerBytes2 & 0xFF, gender, race FROM characters WHERE name='%s'", strName.c_str());
        if (!result)
        {
            player->GetSession()->SendNotification("Player by the name of \"%s\" does not exist.", strName.c_str());
            player->CLOSE_GOSSIP_MENU();
            return false;
        }
        uint32 oldBytes = player->GetUInt32Value(PLAYER_BYTES);
        uint32 oldBytes2 = player->GetUInt32Value(PLAYER_BYTES_2);
        uint8 oldGender = player->getGender();

        Field* fields = result->Fetch();
        uint32 bytes = fields[0].GetUInt32();
        uint32 bytes2 = fields[1].GetUInt32();
        uint8 gender = fields[2].GetUInt8();
        uint8 race = fields[3].GetUInt8();

        bytes2 |= (player->GetUInt32Value(PLAYER_BYTES_2) & 0xFFFFFF00);
        player->SetUInt32Value(PLAYER_BYTES, bytes);
        player->SetUInt32Value(PLAYER_BYTES_2, bytes2);
        player->SetByteValue(UNIT_FIELD_BYTES_0, UNIT_BYTES_0_OFFSET_GENDER, gender);
        player->SetUInt16Value(PLAYER_BYTES_3, 0, uint16(gender) | (player->GetDrunkValue() & 0xFFFE));
        player->InitDisplayIds();

        player->CLOSE_GOSSIP_MENU();

        if (race == player->getRace())
        {
            player->SendCreateUpdateToPlayer(player);

            WorldPacket data(SMSG_FORCE_DISPLAY_UPDATE, 8);
            data << player->GetObjectGuid();
            player->GetSession()->SendPacket(data, true);

            player->CastSpell(player, SPELL_VANISH_VISUAL, TRIGGERED_OLD_TRIGGERED);
            return true;
        }
        else if (!player->ChangeRace(race))
        {
            // revert other changes since race change failed
            player->SetUInt32Value(PLAYER_BYTES, oldBytes);
            player->SetUInt32Value(PLAYER_BYTES_2, oldBytes2);
            player->SetByteValue(UNIT_FIELD_BYTES_0, UNIT_BYTES_0_OFFSET_GENDER, oldGender);
            player->InitDisplayIds();
            player->GetSession()->SendNotification("Failed to change race to match player \"%s\". Incorrect race/class combo?", strName.c_str());
            return false;
        }
        return true;
    }
    return false;
}

enum
{
    NPC_TEXT_GUILD_CREATOR = 42310,
};

static bool GossipHello_guildcreator(Player *player, Creature *creature)
{
    player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, "Create guild", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1, "Enter guild name and click Accept button in next pop-up. Do NOT use the Enter key.", 0, true);
    
    if (creature->isTabardDesigner())
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Design a guild tabard", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TABARD);

    if (creature->isVendor())
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Purchase guild tabard", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

    player->SEND_GOSSIP_MENU(NPC_TEXT_GUILD_CREATOR, creature->GetObjectGuid());

    return true;
}

static bool GossipSelect_guildcreator(Player* player, Creature* creature, uint32 sender, uint32 action)
{
    if (sender != GOSSIP_SENDER_MAIN)
        return false;

    switch (action)
    {
        case GOSSIP_ACTION_TABARD:
            if (!sGuildMgr.GetGuildByLeader(player->GetObjectGuid()))
            {
                player->GetSession()->SendNotification("You're not a guild leader");
                break;
            }
            player->SEND_TABARDLIST(creature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_TRADE:
            player->SEND_VENDORLIST(creature->GetObjectGuid());
            break;
        default:
            player->GetSession()->SendNotification("No guild name provided");
            break;
    }
    player->CLOSE_GOSSIP_MENU();

    return true;
}

static bool GossipSelectWithCode_guildcreator(Player *player, Creature *creature, uint32 sender, uint32 action, const char* sCode)
{
    if (sender != GOSSIP_SENDER_MAIN)
        return false;

    if (player->IsInCombat())
    {
        player->CLOSE_GOSSIP_MENU();
        player->GetSession()->SendNotification("Cannot complete action while in combat");
        return false;
    }

    if (player->GetGuildId())
    {
        player->CLOSE_GOSSIP_MENU();
        player->GetSession()->SendNotification("You are already in a guild");
        return false;
    }

    if (action == GOSSIP_ACTION_INFO_DEF + 1)
    {
        if (strlen(sCode) == 0)
            return false;

        // Prevent SQL injection
        std::string guildname = sCode;
        CharacterDatabase.escape_string(guildname);

        if (guildname.length() > 95)
        {
            player->CLOSE_GOSSIP_MENU();
            player->GetSession()->SendNotification("Name too large. Keep to 95 characters or under so client may display it properly.");
            return false;
        }

        if (player->GetGuildId())
        {
            player->CLOSE_GOSSIP_MENU();
            player->GetSession()->SendNotification("You are already in a guild");
            return false;
        }

        if (sWorld.ChatMessageIsFiltered(guildname) || sObjectMgr.IsReservedName(guildname))
        {
            player->GetSession()->SendNotification("Name is filtered");
            player->CLOSE_GOSSIP_MENU();
            return false;
        }

        Guild* guild = new Guild;
        if (!guild->Create(player, guildname))
        {
            delete guild;
            player->CLOSE_GOSSIP_MENU();
            player->GetSession()->SendNotification("Guild creation failure");
            return false;
        }

        sGuildMgr.AddGuild(guild);

        player->CLOSE_GOSSIP_MENU();
        return true;
    }

    return false;
}

struct npc_target_dummyAI : public Scripted_NoMovementAI
{
    npc_target_dummyAI(Creature* creature) : Scripted_NoMovementAI(creature) { Reset(); }

    std::unordered_map<ObjectGuid, time_t> attackers;
    std::map<ObjectGuid, uint64> m_damageMap;

    time_t m_combatStartTime;
    time_t m_combatEndTime;

    uint32 m_uiCombatTimer;

    void Reset() override
    {
        attackers.clear();

        m_uiCombatTimer = 2000;
        SetCombatMovement(false);
        SetReactState(REACT_PASSIVE);
    }

    void Aggro(Unit* who) override
    {
        m_damageMap.clear();
        m_combatStartTime = time(nullptr);
        m_combatEndTime = 0;
    }

    void AddAttackerToList(Unit* pWho)
    {
        auto itr = attackers.find(pWho->GetObjectGuid());
        if (itr != attackers.end())
            itr->second = std::time(nullptr);
        else
            attackers.emplace(pWho->GetObjectGuid(), std::time(nullptr));
    }

    void EnterEvadeMode() override
    {
        m_combatEndTime = time(nullptr);
        m_uiCombatTimer = 0;
        ScriptedAI::EnterEvadeMode();
    }

    void SpellHit(Unit* caster, const SpellEntry* spellInfo) override
    {
        if (caster)
            AddAttackerToList(caster);
    }

    void DamageTaken(Unit* dealer, uint32& uiDamage, DamageEffectType damagetype, SpellEntry const* spellInfo) override
    {
        if (dealer)
        {
            m_damageMap[dealer->GetObjectGuid()] += uiDamage;
            AddAttackerToList(dealer);
        }
    }

    void ReceiveEmote(Player* player, uint32 uiEmote) override
    {
        if (uiEmote == TEXTEMOTE_WAVE)
        {
            time_t seconds = (m_combatEndTime ? m_combatEndTime : time(nullptr)) - m_combatStartTime;
            ChatHandler(player).PSendSysMessage("Player DPS during last combat that lasted %ld seconds: %ld Total Damage: %lu", seconds, m_damageMap[player->GetObjectGuid()] / seconds, m_damageMap[player->GetObjectGuid()]);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_creature->IsInCombat())
        {
            if (m_uiCombatTimer <= uiDiff)
            {
                for (auto itr = attackers.begin(); itr != attackers.end();)
                {
                    Unit* pAttacker = m_creature->GetMap()->GetUnit(itr->first);

                    if (!pAttacker || !pAttacker->IsInWorld())
                    {
                        itr = attackers.erase(itr);
                        continue;
                    }
                    if (itr->second + 5 < std::time(nullptr))
                    {
                        m_creature->_removeAttacker(pAttacker);
                        m_creature->getThreatManager().modifyThreatPercent(pAttacker, -101.0f);
                        itr = attackers.erase(itr);
                        continue;
                    }
                    ++itr;
                }

                if (m_creature->getThreatManager().isThreatListEmpty())
                    EnterEvadeMode();

                m_uiCombatTimer = 2000;
            }
            else
                m_uiCombatTimer -= uiDiff;
        }
    }
};

enum
{
    FACTION_SCRYERS             = 934,
    FACTION_ALDOR               = 932,
    SPELL_MASTER_JEWELCRAFTING  = 28897,
    SPELL_MASTER_ALCHEMY        = 28596,
    SPELL_MASTER_BLACKSMITHING  = 29844,
    SPELL_MASTER_ENCHANTING     = 28029,
    SPELL_MASTER_ENGINEERING    = 30350,
    SPELL_MASTER_HERB_GATHERING = 28695,
    SPELL_MASTER_LEATHERWORKING = 32549,
    SPELL_MASTER_MINING         = 29354,
    SPELL_MASTER_SKINNING       = 32678,
    SPELL_MASTER_TAILORING      = 26790,
    SPELL_MASTER_FIRST_AID      = 27028,
    SPELL_MASTER_COOKING        = 33359,
    SPELL_MASTER_FISHING        = 33095,
};

void MaxSkillAndLearnAllRecipes(Player* player, uint32 spellId, uint32 skillId)
{
    player->CLOSE_GOSSIP_MENU();
    SkillLineEntry const* skillInfo = sSkillLineStore.LookupEntry(skillId);
    if (!skillInfo)
        return;

    if (skillInfo->categoryId != SKILL_CATEGORY_PROFESSION &&
        skillInfo->categoryId != SKILL_CATEGORY_SECONDARY)
        return;

    if (skillInfo->categoryId == SKILL_CATEGORY_PROFESSION && player->GetFreePrimaryProfessionPoints() == 0)
    {
        player->GetSession()->SendNotification("You already have the max number of primary professions - unlearn one first.");
        return;
    }

    player->learnSpell(spellId, false);

    uint16 maxLevel = player->GetSkillMaxPure(skillInfo->id);
    player->SetSkill(skillInfo->id, maxLevel, maxLevel);

    uint32 classmask = player->getClassMask();
    for (uint32 j = 0; j < sSkillLineAbilityStore.GetNumRows(); ++j)
    {
        SkillLineAbilityEntry const* skillLine = sSkillLineAbilityStore.LookupEntry(j);
        if (!skillLine)
            continue;

        // wrong skill
        if (skillLine->skillId != skillId)
            continue;

        // not high rank
        if (skillLine->forward_spellid)
            continue;

        // skip racial skills
        if (skillLine->racemask != 0)
            continue;

        // skip wrong class skills
        if (skillLine->classmask && (skillLine->classmask & classmask) == 0)
            continue;

        SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(skillLine->spellId);
        if (!spellInfo || !SpellMgr::IsSpellValid(spellInfo, player, false))
            continue;

        player->learnSpell(skillLine->spellId, false);
    }
}

bool GossipHello_paymaster(Player* player, Creature* creature)
{
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "5,000 Gold", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 0);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Profession Trainer", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I want to be Aldor.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Let's go Scryer.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
    player->SEND_GOSSIP_MENU(99999, creature->GetObjectGuid());
    return true;
}

bool GossipSelect_paymaster(Player* player, Creature* creature, uint32 sender, uint32 action)
{
    switch (action)
    {
        case GOSSIP_ACTION_INFO_DEF + 0:
        {
            if (player->GetMoney() < (5000 * GOLD))
            {
                player->SetMoney(5000 * GOLD);
            }
            player->CLOSE_GOSSIP_MENU();
            break;
        }
        case GOSSIP_ACTION_INFO_DEF + 1:
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Jewelcrafting", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Alchemy", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Blacksmithing", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Enchanting", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 13);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Engineering", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 14);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Herbalism", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 15);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Leatherworking", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 16);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Mining", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 17);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Skinning", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 18);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Tailoring", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 19);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "First Aid", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 20);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Cooking", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 21);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Fishing", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 22);
            player->SEND_GOSSIP_MENU(99998, creature->GetObjectGuid());
            break;
        }
        case GOSSIP_ACTION_INFO_DEF + 2:
        {
            FactionEntry const* factionEntryAldor = sFactionStore.LookupEntry<FactionEntry>(FACTION_ALDOR);
            if (!factionEntryAldor)
                return false;

            player->GetReputationMgr().ModifyReputation(factionEntryAldor, 99999999);
            player->CLOSE_GOSSIP_MENU();
            break;
        }
        case GOSSIP_ACTION_INFO_DEF + 3:
        {
            FactionEntry const* factionEntryScryers = sFactionStore.LookupEntry<FactionEntry>(FACTION_SCRYERS);
            if (!factionEntryScryers)
                return false;

            player->GetReputationMgr().ModifyReputation(factionEntryScryers, 99999999);
            player->CLOSE_GOSSIP_MENU();
            break;
        }
        case GOSSIP_ACTION_INFO_DEF + 10:
        {
            MaxSkillAndLearnAllRecipes(player, SPELL_MASTER_JEWELCRAFTING, SKILL_JEWELCRAFTING);
            break;
        }
        case GOSSIP_ACTION_INFO_DEF + 11:
        {
            MaxSkillAndLearnAllRecipes(player, SPELL_MASTER_ALCHEMY, SKILL_ALCHEMY);
            break;
        }
        case GOSSIP_ACTION_INFO_DEF + 12:
        {
            MaxSkillAndLearnAllRecipes(player, SPELL_MASTER_BLACKSMITHING, SKILL_BLACKSMITHING);
            break;
        }
        case GOSSIP_ACTION_INFO_DEF + 13:
        {
            MaxSkillAndLearnAllRecipes(player, SPELL_MASTER_ENCHANTING, SKILL_ENCHANTING);
            break;
        }
        case GOSSIP_ACTION_INFO_DEF + 14:
        {
            MaxSkillAndLearnAllRecipes(player, SPELL_MASTER_ENGINEERING, SKILL_ENGINEERING);
            break;
        }
        case GOSSIP_ACTION_INFO_DEF + 15:
        {
            MaxSkillAndLearnAllRecipes(player, SPELL_MASTER_HERB_GATHERING, SKILL_HERBALISM);
            break;
        }
        case GOSSIP_ACTION_INFO_DEF + 16:
        {
            MaxSkillAndLearnAllRecipes(player, SPELL_MASTER_LEATHERWORKING, SKILL_LEATHERWORKING);
            break;
        }
        case GOSSIP_ACTION_INFO_DEF + 17:
        {
            MaxSkillAndLearnAllRecipes(player, SPELL_MASTER_MINING, SKILL_MINING);
            break;
        }
        case GOSSIP_ACTION_INFO_DEF + 18:
        {
            MaxSkillAndLearnAllRecipes(player, SPELL_MASTER_SKINNING, SKILL_SKINNING);
            break;
        }
        case GOSSIP_ACTION_INFO_DEF + 19:
        {
            MaxSkillAndLearnAllRecipes(player, SPELL_MASTER_TAILORING, SKILL_TAILORING);
            break;
        }
        case GOSSIP_ACTION_INFO_DEF + 20:
        {
            MaxSkillAndLearnAllRecipes(player, SPELL_MASTER_FIRST_AID, SKILL_FIRST_AID);
            break;
        }
        case GOSSIP_ACTION_INFO_DEF + 21:
        {
            MaxSkillAndLearnAllRecipes(player, SPELL_MASTER_COOKING, SKILL_COOKING);
            break;
        }
        case GOSSIP_ACTION_INFO_DEF + 22:
        {
            MaxSkillAndLearnAllRecipes(player, SPELL_MASTER_FISHING, SKILL_FISHING);
            break;
        }
    }
    return true;
}

bool GossipHello_lockoutnullifier(Player* player, Creature* creature)
{
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "List my lockouts", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
    player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, "Reset my lockouts", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1, "Are you sure you wish to proceed? All of your saved raids and dungeons will be lost. This cannot be reversed.", 0, false);
    player->SEND_GOSSIP_MENU(8412, creature->GetObjectGuid()); // "Hello, friend. How may I be of service to you today?"
    return true;
}

void ListOrUnbindInstances(Player* player, bool unbind = false)
{
    uint32 counter = 0;
    for (uint8 i = 0; i < MAX_DIFFICULTY; ++i)
    {
        Player::BoundInstancesMap& binds = player->GetBoundInstances(Difficulty(i));
        for (Player::BoundInstancesMap::const_iterator itr = binds.begin(); itr != binds.end(); ++itr)
        {
            DungeonPersistentState* state = itr->second.state;
            std::string timeleft = secsToTimeString(state->GetResetTime() - time(nullptr), true);
            if (const MapEntry* entry = sMapStore.LookupEntry(itr->first))
            {
                player->GetSession()->SendNotification("map: %d (%s) inst: %d perm: %s diff: %s canReset: %s TTR: %s",
                    itr->first, entry->name[player->GetSession()->GetSessionDbcLocale()], state->GetInstanceId(), itr->second.perm ? "yes" : "no",
                    state->GetDifficulty() == DUNGEON_DIFFICULTY_NORMAL ? "normal" : "heroic", state->CanReset() ? "yes" : "no", timeleft.c_str());

                if (unbind)
                    player->UnbindInstance(itr->first, Difficulty(i));
            }
            else
                player->GetSession()->SendNotification("bound for a nonexistent map %u", itr->first);
            ++counter;
        }
    }
    player->GetSession()->SendNotification(unbind ? "instances unbound: %d" : "player binds: %d", counter);
}

bool GossipSelect_lockoutnullifier(Player* player, Creature* creature, uint32 sender, uint32 action)
{
    switch (action)
    {
        case GOSSIP_ACTION_INFO_DEF:
        {
            player->CLOSE_GOSSIP_MENU();
            ListOrUnbindInstances(player);
            break;
        }
        case GOSSIP_ACTION_INFO_DEF + 1:
        {
            player->CLOSE_GOSSIP_MENU();
            ListOrUnbindInstances(player, true);
            break;
        }
    }
    return true;
}

// CHARACTER LEVEL BOOST SYSTEM
// Start GAME_EVENT_LEVEL_BOOST to enable

enum
{
    SET_ID_PRIMARY      = 0,
    SET_ID_SECONDARY    = 1,
    SET_ID_TERTIARY     = 2,
    SET_ID_INSTANT_58   = 3,

    SPELL_APPRENTICE_RIDING     = 33389,
    SPELL_JOURNEYMAN_RIDING     = 33392,
    SPELL_ARTISAN_RIDING        = 34093,
    SPELL_ARTISAN_FIRST_AID     = 10847,
    SPELL_HEAVY_RUNECLOTH_BAND  = 18632,
    SPELL_HEAVY_NETHER_BAND     = 27033,
};

const std::vector<uint32> Level1StartingGear = { 25,35,36,37,39,40,43,44,47,48,51,52,55,56,57,59,117,120,121,129,139,140,147,153,159,1395,1396,2070,2092,2101,2102,2361,2362,2504,2508,2512,2516,3661,4536,4540,4604,6098,6116,6118,6119,6121,6122,6123,6124,6126,6127,6129,6135,6137,6138,6139,6140,6144,12282,20857,20891,20892,20893,20894,20895,20896,20898,20899,20900,20980,20981,20982,20983,23322,23344,23346,23347,23348,23474,23475,23477,23478,23479,24145,24146,25861,28979 };

struct CustomTeleportLocation
{
    uint32 map;
    float x, y, z, o;
    uint32 area;
};
static const CustomTeleportLocation teleLocs[] =
{
    {0, -11792.108398f, -3226.608154f, -29.721224f, 2.613495f, 72}, // Dark Portal - Alliance
    {0, -11774.694336f, -3184.537598f, -28.923182f, 2.749808f, 72}, // Dark Portal - Horde
    {530, -1982.2641f, 5073.6987f, 7.388187f, 1.227539f, 3703},     // Shattrath City - Alliance
    {530, -1992.693726f, 5078.106934f, 7.165706f, 0.944797f, 3703}, // Shattrath City - Horde

    {0, -8931.93f, -132.83f, 82.88f, 3.26f, 0},                     // Northshire (Human)
    {0, -6213.47f, 330.64f, 383.68f, 3.15f, 0},                     // Coldridge Valley (Dwarf and Gnome)
    {1, 10317.40f, 821.00f, 1326.37f, 2.15f, 0},                    // Shadowglen (Night Elf)
    {530, -3983.02f, -13898.48f, 96.30f, 5.32f, 0},                 // Ammen Vale (Draenei)

    {1, -607.47f, -4248.13f, 38.95f, 3.27f, 0},                     // Valley of Trials (Orc and Troll)
    {0, 1656.367f, 1682.592f, 120.78681f, 0.06632f, 0},             // Deathknell (Undead)
    {1, -2913.71f, -254.67f, 52.94f, 3.70f, 0},                     // Camp Narache (Tauren)
    {530, 10354.33f, -6370.34f, 36.04f, 1.92f, 0},                  // Sunstrider Isle (Blood Elf)

    {0, 4272.49f, -2777.97f, 5.56f, 0.523f, 2037},                  // Mall (Quel'thalas)
};

uint32 GetStarterMountForRace(Player* player)
{
    uint32 mountEntry = 0;
    switch (player->getRace())
    {
        case RACE_HUMAN:
            if (!player->HasItemCount(2411,1) && !player->HasItemCount(2414,1) && !player->HasItemCount(5655,1) && !player->HasItemCount(5656,1))
                mountEntry = PickRandomValue(2411,2414,5655,5656);
            break;
        case RACE_DWARF:
            if (!player->HasItemCount(5864, 1) && !player->HasItemCount(5872, 1) && !player->HasItemCount(5873, 1))
                mountEntry = PickRandomValue(5864,5872,5873);
            break;
        case RACE_NIGHTELF:
            if (!player->HasItemCount(8629, 1) && !player->HasItemCount(8631, 1) && !player->HasItemCount(8632, 1))
                mountEntry = PickRandomValue(8629,8631,8632);
            break;
        case RACE_GNOME:
            if (!player->HasItemCount(8595, 1) && !player->HasItemCount(8563, 1) && !player->HasItemCount(13321, 1) && !player->HasItemCount(13322, 1))
                mountEntry = PickRandomValue(8595,8563,13321,13322);
            break;
        case RACE_DRAENEI:
            if (!player->HasItemCount(29744, 1) && !player->HasItemCount(28481, 1) && !player->HasItemCount(29743, 1))
                mountEntry = PickRandomValue(29744,28481,29743);
            break;
        case RACE_ORC:
            if (!player->HasItemCount(1132, 1) && !player->HasItemCount(5665, 1) && !player->HasItemCount(5668, 1))
                mountEntry = PickRandomValue(1132,5665,5668);
            break;
        case RACE_UNDEAD:
            if (!player->HasItemCount(13331, 1) && !player->HasItemCount(13332, 1) && !player->HasItemCount(13333, 1))
                mountEntry = PickRandomValue(13331,13332,13333);
            break;
        case RACE_TAUREN:
            if (!player->HasItemCount(15277, 1) && !player->HasItemCount(15290, 1))
                mountEntry = PickRandomValue(15277,15290);
            break;
        case RACE_TROLL:
            if (!player->HasItemCount(8588, 1) && !player->HasItemCount(8591, 1) && !player->HasItemCount(8592, 1))
                mountEntry = PickRandomValue(8588,8591,8592);
            break;
        case RACE_BLOODELF:
            if (!player->HasItemCount(28927, 1) && !player->HasItemCount(29220, 1) && !player->HasItemCount(29221, 1) && !player->HasItemCount(29222, 1))
                mountEntry = PickRandomValue(28927,29220,29221,29222);
            break;
    }
    return mountEntry;
}

uint32 GetStarterEpicMountForRace(Player* player)
{
    uint32 mountEntry = 0;
    switch (player->getRace())
    {
        case RACE_HUMAN:
            if (!player->HasItemCount(18776, 1) && !player->HasItemCount(18777, 1) && !player->HasItemCount(18778, 1))
                mountEntry = PickRandomValue(18776,18777,18778);
            break;
        case RACE_DWARF:
            if (!player->HasItemCount(18787, 1) && !player->HasItemCount(18786, 1) && !player->HasItemCount(18785, 1))
                mountEntry = PickRandomValue(18787,18786,18785);
            break;
        case RACE_NIGHTELF:
            if (!player->HasItemCount(18767, 1) && !player->HasItemCount(18766, 1) && !player->HasItemCount(18902, 1))
                mountEntry = PickRandomValue(18767,18766,18902);
            break;
        case RACE_GNOME:
            if (!player->HasItemCount(18772, 1) && !player->HasItemCount(18773, 1) && !player->HasItemCount(18774, 1))
                mountEntry = PickRandomValue(18772,18773,18774);
            break;
        case RACE_DRAENEI:
            if (!player->HasItemCount(29745, 1) && !player->HasItemCount(29746, 1) && !player->HasItemCount(29747, 1))
                mountEntry = PickRandomValue(29745,29746,29747);
            break;
        case RACE_ORC:
            if (!player->HasItemCount(18798, 1) && !player->HasItemCount(18797, 1) && !player->HasItemCount(18796, 1))
                mountEntry = PickRandomValue(18798,18797,18796);
            break;
        case RACE_UNDEAD:
            if (!player->HasItemCount(18791, 1) && !player->HasItemCount(13334, 1))
                mountEntry = PickRandomValue(18791,13334);
            break;
        case RACE_TAUREN:
            if (!player->HasItemCount(18795, 1) && !player->HasItemCount(18794, 1) && !player->HasItemCount(18793, 1))
                mountEntry = PickRandomValue(18795,18794,18793);
            break;
        case RACE_TROLL:
            if (!player->HasItemCount(18790, 1) && !player->HasItemCount(18788, 1) && !player->HasItemCount(18789, 1))
                mountEntry = PickRandomValue(18790,18788,18789);
            break;
        case RACE_BLOODELF:
            if (!player->HasItemCount(28936, 1) && !player->HasItemCount(29223, 1) && !player->HasItemCount(29224, 1))
                mountEntry = PickRandomValue(28936,29223,29224);
            break;
    }
    return mountEntry;
}

uint32 GetStarterFlyingMountForFaction(Player* player)
{
    uint32 mountEntry = 0;
    if (player->GetTeam() == ALLIANCE)
    {
        if (!player->HasItemCount(25529, 1) && !player->HasItemCount(25473, 1) && !player->HasItemCount(25527, 1) && !player->HasItemCount(25528, 1))
            mountEntry = PickRandomValue(25529, 25473, 25527, 25528);
    }
    else
    {
        if (!player->HasItemCount(25477, 1) && !player->HasItemCount(25531, 1) && !player->HasItemCount(25532, 1) && !player->HasItemCount(25533, 1))
            mountEntry = PickRandomValue(25477, 25531, 25532, 25533);
    }
    return mountEntry;
}

const std::vector<uint32> Instant58TaxiNodesAlliance =
{
    // Eastern Kingdoms:
    2,     // Stormwind, Elwynn
    4,     // Sentinel Hill, Westfall
    5,     // Lakeshire, Redridge
    6,     // Ironforge, Dun Morogh
    7,     // Menethil Harbor, Wetlands
    8,     // Thelsamar, Loch Modan
    12,    // Darkshire, Duskwood
    14,    // Southshore, Hillsbrad
    16,    // Refuge Pointe, Arathi
    19,    // Booty Bay, Stranglethorn
    43,    // Aerie Peak, The Hinterlands
    45,    // Nethergarde Keep, Blasted Lands
    66,    // Chillwind Camp, Western Plaguelands
    67,    // Light's Hope Chapel, Eastern Plaguelands
    71,    // Morgan's Vigil, Burning Steppes
    74,    // Thorium Point, Searing Gorge
    // Kalimdor:
    26,    // Auberdine, Darkshore
    27,    // Rut'theran Village, Teldrassil
    28,    // Astranaar, Ashenvale
    31,    // Thalanaar, Feralas
    32,    // Theramore, Dustwallow Marsh
    33,    // Stonetalon Peak, Stonetalon Mountains
    37,    // Nijel's Point, Desolace
    39,    // Gadgetzan, Tanaris
    41,    // Feathermoon, Feralas
    49,    // Moonglade
    52,    // Everlook, Winterspring
    64,    // Talrendis Point, Azshara
    65,    // Talonbranch Glade, Felwood
    73,    // Cenarion Hold, Silithus
    79,    // Marshal's Refuge, Un'Goro Crater
    80     // Ratchet, The Barrens
};
const std::vector<uint32> Instant58TaxiNodesHorde =
{
    // Eastern kingdoms:
    10,    // The Sepulcher, Silverpine Forest
    11,    // Undercity, Tirisfal
    13,    // Tarren Mill, Hillsbrad
    17,    // Hammerfall, Arathi
    18,    // Booty Bay, Stranglethorn
    20,    // Grom'gol, Stranglethorn
    21,    // Kargath, Badlands
    56,    // Stonard, Swamp of Sorrows
    68,    // Light's Hope Chapel, Eastern Plaguelands
    70,    // Flame Crest, Burning Steppes
    75,    // Thorium Point, Searing Gorge
    76,    // Revantusk Village, The Hinterlands
    // Kalimdor:
    22,    // Thunder Bluff, Mulgore
    23,    // Orgrimmar, Durotar
    25,    // Crossroads, The Barrens
    29,    // Sun Rock Retreat, Stonetalon Mountains
    30,    // Freewind Post, Thousand Needles
    38,    // Shadowprey Village, Desolace
    40,    // Gadgetzan, Tanaris
    42,    // Camp Mojache, Feralas
    44,    // Valormok, Azshara
    48,    // Bloodvenom Post, Felwood
    53,    // Everlook, Winterspring
    55,    // Brackenwall Village, Dustwallow Marsh
    58,    // Zoram'gar Outpost, Ashenvale
    61,    // Splintertree Post, Ashenvale
    69,    // Moonglade
    72,    // Cenarion Hold, Silithus
    77,    // Camp Taurajo, The Barrens
    79,    // Marshal's Refuge, Un'Goro Crater
    80     // Ratchet, The Barrens
};

bool TaxiNodesKnown(Player& player)
{
    switch (uint32(player.GetTeam()))
    {
        case ALLIANCE:
            for (uint32 node : Instant58TaxiNodesAlliance)
            {
                if (!player.m_taxi.IsTaximaskNodeKnown(node))
                    return false;
            }
            break;
        case HORDE:
            for (uint32 node : Instant58TaxiNodesHorde)
            {
                if (!player.m_taxi.IsTaximaskNodeKnown(node))
                    return false;
            }
            break;
    }
    return true;
}

void TaxiNodesTeach(Player& player)
{
    // Vanilla taxi nodes up to 2.0.1 progression patch
    switch (uint32(player.GetTeam()))
    {
        case ALLIANCE:
            for (uint32 node : Instant58TaxiNodesAlliance)
                player.m_taxi.SetTaximaskNode(node);
            break;
        case HORDE:
            for (uint32 node : Instant58TaxiNodesHorde)
                player.m_taxi.SetTaximaskNode(node);
            break;
    }
}

void BoostPlayer(Player* player, uint32 targetLevel)
{
    if (player->GetLevel() < targetLevel)
    {
        player->GiveLevel(targetLevel);
        player->SetUInt32Value(PLAYER_XP, 0);
        player->learnClassLevelSpells(false);
        player->UpdateSkillsForLevel(true);

        if (player->getClass() == CLASS_HUNTER)
        {
            if (Pet* pet = player->GetPet())
            {
                if (pet->GetLevel() < targetLevel)
                {
                    pet->GivePetLevel(targetLevel);
                    pet->SavePetToDB(PET_SAVE_AS_CURRENT, player);
                }
            }
        }
    }

    // Learn skills
    player->CastSpell(player, SPELL_APPRENTICE_RIDING, TRIGGERED_OLD_TRIGGERED);
    player->CastSpell(player, SPELL_ARTISAN_FIRST_AID, TRIGGERED_OLD_TRIGGERED);
    player->CastSpell(player, SPELL_HEAVY_RUNECLOTH_BAND, TRIGGERED_OLD_TRIGGERED);
    switch (targetLevel) {
        case 60:
            player->CastSpell(player, SPELL_JOURNEYMAN_RIDING, TRIGGERED_OLD_TRIGGERED);
            break;
        case 70:
            player->CastSpell(player, SPELL_ARTISAN_RIDING, TRIGGERED_OLD_TRIGGERED);
            player->CastSpell(player, SPELL_MASTER_FIRST_AID, TRIGGERED_OLD_TRIGGERED);
            player->learnSpell(SPELL_HEAVY_NETHER_BAND, false);
            break;
    }
    SkillLineEntry const* sl = sSkillLineStore.LookupEntry(SKILL_FIRST_AID);
    if (sl)
    {
        uint32 maxSkill = player->GetSkillMaxPure(SKILL_FIRST_AID);

        if (player->GetSkillValue(SKILL_FIRST_AID) < maxSkill)
            player->SetSkill(SKILL_FIRST_AID, maxSkill, maxSkill);
    }

    // Remove any gear the character still has from initial creation (now useless)
    for (uint32 itemEntry : Level1StartingGear)
        player->DestroyItemCount(itemEntry, 200, true, false);

    if (targetLevel == 58) {
        // LV Starter Bag (8 Slot) x4
        if (!player->HasItemCount(2115, 4)) player->StoreNewItemInBestSlots(2115, 4);

        // Ground Mount
        if (uint32 groundMount = GetStarterMountForRace(player))
            player->StoreNewItemInBestSlots(groundMount, 1);
    }
    else {
        // Clean up LV Starter Bags
        if (player->HasItemCount(2115, 4)) player->DestroyItemCount(2115, 4, true);
        // Onyxia Hide Backpack x4
        if (!player->HasItemCount(17966, 4)) player->StoreNewItemInBestSlots(17966, 4);

        // Epic Ground Mount
        if (uint32 groundMount = GetStarterEpicMountForRace(player))
            player->StoreNewItemInBestSlots(groundMount, 1);

        // Flying Mount
        if (targetLevel == 70)
            if (uint32 flyingMount = GetStarterFlyingMountForFaction(player))
                player->StoreNewItemInBestSlots(flyingMount, 1);
    }

    player->SaveToDB();
}

// todo: make and load new table - custom_gear_set: set_id, item_id, enchant_id/spell_id?, gem_1, gem_2, gem_3, gem_4?, comment
// 
// Instant 70
//                                                          head    shoulders   chest   hands   legs
const std::vector<uint32> PlatePrimaryGear              = { 12423,  12407,      15141,  14694,  32171 };
//                                                          head    shoulders   chest   hands   legs
const std::vector<uint32> LeatherPrimaryGear            = { 16156,  4728,       1170,   26227,  13776 };
//                                                          head    shoulders   chest   hands   legs
const std::vector<uint32> ClothPrimaryGear              = { 32129,  28016,      26082,  30288,  30921 };
//                                                          head    shoulders   chest   hands   legs
const std::vector<uint32> MailPrimaryGear               = { 26388,  32137,      29899,  32149,  25551 };

//                                                          back    ring1   ring2   neck   trinket1
const std::vector<uint32> SharedSecondaryGear           = { 14701,  7466,   7466,   27228, 4031 };
//                                                          wrist   waist   boots
const std::vector<uint32> PlateSecondaryGear            = { 16512,  20324,  16030 };
//                                                          wrist   waist   boots
const std::vector<uint32> ClothSecondaryGear            = { 32977,  18341,  26097 };
//                                                          wrist   waist   boots
const std::vector<uint32> LeatherSecondaryGear          = { 26015,  20789,  26225 };
//                                                          wrist   waist   boots
const std::vector<uint32> MailSecondaryGear             = { 32968,  25794,  28011 };

//                                                          mainhand    shield  trinket2    ranged
const std::vector<uint32> ProtWarriorTertiaryGear       = { 26797,      26559,  35772,      29210 };
//                                                          mainhand    offhand trinket2    ranged
const std::vector<uint32> FuryWarriorTertiaryGear       = { 26797,      23232,  35771,      29210 };
//                                                          2hand   trinket2    ranged
const std::vector<uint32> ArmsWarriorTertiaryGear       = { 1311,   35771,      29210 };
//                                                          mainhand    shield  trinket2    libram
const std::vector<uint32> HolyPaladinTertiaryGear       = { 19359,      26559,  35773,      23201 };
//                                                          2hand   trinket2    libram
const std::vector<uint32> RetPaladinTertiaryGear        = { 1311,   35771,      31033 };
//                                                          mainhand    shield  trinket2    libram
const std::vector<uint32> ProtPaladinTertiaryGear       = { 20811,      26559,  35772,      27917 };
//                                                          mainhand    offhand trinket2    idol
const std::vector<uint32> BalanceDruidTertiaryGear      = { 32177,      26573,  35770,      23197 };
//                                                          2hand   trinket2    idol
const std::vector<uint32> FeralDruidTertiaryGear        = { 20522,  35771,      28064 };
//                                                          mainhand    offhand trinket2    idol
const std::vector<uint32> RestoDruidTertiaryGear        = { 19359,      26573,  35773,      22398 };
//                                                          mainhand    offhand trinket2    wand
const std::vector<uint32> DiscHolyPriestTertiaryGear    = { 19359,      26573,  35773,      32482 };
//                                                          mainhand    offhand trinket2    wand
const std::vector<uint32> CasterDPSTertiaryGear         = { 32177,      26573,  35770,      32482 }; // warlock, shadow priest, mage
//                                                          mainhand    offhand trinket2    totem
const std::vector<uint32> EnhanceShamanTertiaryGear     = { 26797,      26797,  35771,      22395 };
//                                                          mainhand    shield  trinket2    totem
const std::vector<uint32> ElementalShamanTertiaryGear   = { 32177,      26559,  35770,      23199 };
//                                                          mainhand    shield  trinket2    totem
const std::vector<uint32> RestoShamanTertiaryGear       = { 19359,      26559,  35773,      23200 };
//                                                          2hand   trinket2    ranged  quiver
const std::vector<uint32> HunterTertiaryGear            = { 1311,   35771,      20313,  1281 };
//                                                          mainhand    offhand trinket2    ranged
const std::vector<uint32> CombatSubRogueTertiaryGear    = { 23232,      23232,  35771,      29210 };
//                                                          mainhand    offhand trinket2    ranged
const std::vector<uint32> AssassinRogueTertiaryGear     = { 12767,      12767,  35771,      29210 };

// "Best In Slot"
const std::vector<uint32> Lvl60BiS_ShamanResto          = { 22466, 21712, 22467, 21583, 22464, 22471, 22465, 22469, 22470, 22468, 23065, 21620, 23047, 19395, 23056, 22819, 22396 };
const std::vector<uint32> Lvl60BiS_ShamanEnhancement    = { 18817, 18404, 21684, 23045, 21374, 21602, 21624, 21586, 21651, 21705, 17063, 23038, 22321, 19289, 22798, 22395 };
const std::vector<uint32> Lvl60BiS_ShamanElemental      = { 19375, 22943, 21376, 23050, 21671, 21464, 21585, 22730, 21375, 21373, 21707, 21709, 19379, 23046, 22988, 23049, 23199 };
const std::vector<uint32> Lvl60BiS_PriestShadow         = { 23035, 18814, 22983, 22731, 23220, 21611, 19133, 21585, 19400, 19131, 21709, 19434, 19379, 23046, 22988, 23049, 22820 };
const std::vector<uint32> Lvl60BiS_PriestDiscHoly       = { 21615, 21712, 22515, 22960, 22512, 21604, 22513, 22517, 21582, 22516, 23061, 22939, 23027, 23047, 23056, 23048, 23009 };
const std::vector<uint32> Lvl60BiS_PaladinHoly          = { 19375, 23057, 22429, 23050, 22425, 21604, 22427, 20264, 21582, 22430, 23066, 19382, 19395, 23047, 23056, 23075, 23006 };
const std::vector<uint32> Lvl60BiS_PaladinRetribution   = { 21387, 18404, 21391, 23045, 21389, 22936, 21623, 23219, 21390, 21388, 23038, 17063, 22321, 19289, 22691, 23203, 17182 };
const std::vector<uint32> Lvl60BiS_PaladinProtection    = { 21387, 22732, 21639, 22938, 21389, 20616, 21674, 21598, 19855, 21706, 19376, 21601, 19431, 18406, 22988, 23043, 22401, 19019 };
const std::vector<uint32> Lvl60BiS_WarriorFuryArms      = { 12640, 23053, 21330, 23045, 23000, 22936, 21581, 23219, 23068, 19387, 18821, 23038, 22954, 23041, 23054, 23577, 17076, 22812 };
const std::vector<uint32> Lvl60BiS_WarriorProtection    = { 22418, 22732, 22419, 22938, 22416, 22423, 22421, 22422, 22417, 22420, 23059, 21601, 19431, 19406, 19019, 23043, 19368 };
const std::vector<uint32> Lvl60BiS_DruidFeralCat        = { 8345, 19377, 21665, 21710, 23226, 21602, 21672, 21586, 23071, 21493, 23038, 19432, 19406, 23041, 22988, 22632, 22397, 13385 };
const std::vector<uint32> Lvl60BiS_DruidFeralBear       = { 21693, 22732, 19389, 22938, 23226, 21602, 21605, 21675, 20627, 19381, 21601, 23018, 13966, 11811, 943, 23198 };
const std::vector<uint32> Lvl60BiS_DruidBalance         = { 19375, 23057, 22983, 23050, 19682, 23021, 21585, 22730, 19683, 19684, 23025, 21709, 19379, 23046, 22988, 23049, 23197 };
const std::vector<uint32> Lvl60BiS_DruidResto           = { 20628, 21712, 22491, 22960, 22488, 21604, 22493, 21582, 22489, 22492, 22939, 21620, 23047, 23027, 23056, 22632, 22399, 23048 };
const std::vector<uint32> Lvl60BiS_Rogue                = { 22478, 19377, 22479, 23045, 22476, 22483, 22477, 22481, 22482, 22480, 23060, 23038, 23041, 22954, 23054, 22802, 21126, 23577, 22812, 19019 };
const std::vector<uint32> Lvl60BiS_Mage                 = { 22498, 23057, 22983, 23050, 22496, 23021, 23070, 21585, 22730, 22500, 23062, 23237, 19379, 23046, 19339, 22807, 23049, 22821 };
const std::vector<uint32> Lvl60BiS_Hunter               = { 22438, 23053, 22439, 23045, 22436, 22443, 22437, 22441, 22442, 22440, 23067, 22961, 23041, 19406, 22816, 22802, 22812 };
const std::vector<uint32> Lvl60BiS_Warlock              = { 22506, 23057, 22507, 23050, 22504, 21186, 23070, 21585, 22730, 22508, 21709, 23025, 19379, 23046, 22807, 23049, 22820 };

const std::vector<uint32> Lvl70BiS_ShamanResto          = { 32524, 31016, 30873, 32528, 32238, 32275, 31012, 31019, 32370, 31022, 32258, 30869, 32500, 30882, 32496, 30619, 32344, 30023 };
const std::vector<uint32> Lvl70BiS_ShamanEnhancement    = { 32323, 34912, 32510, 32497, 32335, 32234, 32376, 30900, 32260, 31024, 32346, 30864, 32946, 32945, 28830, 32505, 32332, 27815 };
const std::vector<uint32> Lvl70BiS_ShamanElemental      = { 32331, 31017, 32242, 32527, 32247, 31008, 31014, 31020, 32349, 31023, 30044, 32259, 34009, 30872, 32483, 30626, 32374, 32330 };
const std::vector<uint32> Lvl70BiS_PriestShadow         = { 32590, 31065, 32239, 32527, 32247, 31061, 31064, 30916, 30666, 31070, 32256, 30870, 32237, 30872, 32343, 32483, 28789, 32374 };
const std::vector<uint32> Lvl70BiS_PriestDiscHoly       = { 32524, 31066, 32609, 32528, 32238, 31060, 31063, 30912, 32370, 31069, 30895, 30871, 32500, 30911, 32363, 32496, 29376, 32344 };
const std::vector<uint32> Lvl70BiS_PaladinHoly          = { 32524, 30992, 32243, 32528, 32238, 30983, 30988, 30994, 32370, 30996, 30897, 30862, 32500, 30882, 32496, 29376, 28592 };
const std::vector<uint32> Lvl70BiS_PaladinRetribution   = { 32323, 30990, 32345, 32497, 32335, 32278, 32373, 32341, 32591, 30866, 30032, 30057, 33503, 32332, 28830, 32505 };
const std::vector<uint32> Lvl70BiS_PaladinProtection    = { 32331, 30991, 32245, 30083, 29297, 30985, 30987, 30995, 32362, 30998, 32342, 32279, 34009, 30909, 31856, 28789, 30620, 32368 };
const std::vector<uint32> Lvl70BiS_WarriorFuryArms      = { 32323, 30975, 32345, 32497, 32335, 32278, 32373, 32341, 32260, 30979, 30032, 30861, 32369, 30881, 30105, 32505, 28830, 32348 };
const std::vector<uint32> Lvl70BiS_WarriorProtection    = { 34010, 30976, 32268, 32261, 29297, 30970, 32521, 30978, 32362, 30980, 32333, 32232, 32254, 32375, 32325, 30620, 32501 };
const std::vector<uint32> Lvl70BiS_DruidFeralCat        = { 32323, 32252, 32366, 32497, 32266, 32347, 32235, 31044, 32260, 31048, 30106, 32324, 32257, 28830, 32505, 30883 };
const std::vector<uint32> Lvl70BiS_DruidFeralBear       = { 28660, 31042, 32593, 29279, 28792, 31034, 31039, 31044, 32362, 31048, 30879, 32324, 32658, 30620, 30021, 33509 };
const std::vector<uint32> Lvl70BiS_DruidBalance         = { 32331, 31043, 32352, 32527, 32247, 31035, 31040, 31046, 32349, 31049, 30914, 32351, 32237, 30872, 32483, 30626, 32374, 27518 };
const std::vector<uint32> Lvl70BiS_DruidResto           = { 32337, 31041, 30886, 32528, 29309, 31032, 31037, 31045, 32370, 31047, 32339, 30868, 32500, 30911, 32496, 29376, 32344, 30051 };
const std::vector<uint32> Lvl70BiS_Rogue                = { 32323, 31028, 32366, 32497, 32266, 31026, 32235, 31029, 32260, 31030, 30879, 32324, 32369, 30881, 32269, 32326, 28830, 32505 };
const std::vector<uint32> Lvl70BiS_Mage                 = { 32331, 31057, 32239, 32527, 32247, 31055, 32525, 31058, 30015, 31059, 30888, 30870, 30910, 30872, 29982, 32483, 27683, 32374 };
const std::vector<uint32> Lvl70BiS_Hunter               = { 32323, 31004, 30880, 29301, 32497, 31001, 32376, 31005, 32260, 31006, 32346, 32251, 32369, 32236, 30906, 28830, 32505, 32248 };
const std::vector<uint32> Lvl70BiS_Warlock              = { 32590, 31052, 30050, 32527, 32247, 31050, 32525, 31053, 32349, 31054, 32256, 29918, 30910, 30872, 32343, 32483, 27683, 32374 };

// Instant 58
//                                                          ring1   ring2   neck
const std::vector<uint32> Instant58Shared               = { 6711,   6711,   1122 };
//                                                          back
const std::vector<uint32> Instant58PhysicalBack         = { 22230 };
//                                                          back
const std::vector<uint32> Instant58MagicBack            = { 4011 };
//                                                          bow
const std::vector<uint32> Instant58Bow                  = { 5548 };
//                                                          wrist   waist   boots
const std::vector<uint32> Instant58LeatherOffpieces     = { 20269,  20267,  3533 };
//                                                          staff
const std::vector<uint32> Instant58Staff                = { 2487 };
//                                                          wand    mainhand    offhand boots   waist   wrist
const std::vector<uint32> Instant58ClothCaster          = { 7186,   7169,       12863,  21088,  26056,  16119 };
//                                                          wrist   waist   boots
const std::vector<uint32> Instant58MailOffpieces        = { 3525,   1969,   1174 };
//                                                          mainhand    shield
const std::vector<uint32> Instant58GavelAndShield       = { 1312,       14609 };
//                                                          mainhand    offhand
const std::vector<uint32> Instant58DualWield            = { 25799,      25799 };
//                                                          wrist   waist   boots
const std::vector<uint32> Instant58PlateOffpieces       = { 18431,  3952,  14689 };

const std::vector<uint32> Instant58Hunter               = { 3479, 3051, 13795, 3050, 24534, 5545, 29889, 1047, 1281 };
const std::vector<uint32> Instant58Rogue                = { 23652, 1018, 1313, 7167, 20270, 20308, 3532, 20271, 20274, 25877 };
const std::vector<uint32> Instant58Druid                = { 5554, 1535, 3549, 1424, 14388, 1023, 25667 };
const std::vector<uint32> Instant58Mage                 = { 20328, 20327, 20343, 20331, 20330 };
const std::vector<uint32> Instant58Paladin              = { 32110, 12615, 2275, 12413, 16031, 23202, 7940 };
const std::vector<uint32> Instant58Priest               = { 19844, 2573, 16141, 25800, 18747 };
const std::vector<uint32> Instant58Shaman               = { 20775, 3529, 3232, 20793, 20777, 6213, 7940 };
const std::vector<uint32> Instant58Warlock              = { 24563, 26165, 24564, 24562, 24566 };
const std::vector<uint32> Instant58Warrior              = { 20136, 1028, 3243, 33772, 20284, 28961, 31822, 7940 };

std::vector<uint32> GetFullGearListCustom()
{
    std::vector<uint32> gearList;
    gearList.clear();
    gearList.insert(std::end(gearList), std::begin(PlatePrimaryGear), std::end(PlatePrimaryGear));
    gearList.insert(std::end(gearList), std::begin(LeatherPrimaryGear), std::end(LeatherPrimaryGear));
    gearList.insert(std::end(gearList), std::begin(ClothPrimaryGear), std::end(ClothPrimaryGear));
    gearList.insert(std::end(gearList), std::begin(MailPrimaryGear), std::end(MailPrimaryGear));
    gearList.insert(std::end(gearList), std::begin(SharedSecondaryGear), std::end(SharedSecondaryGear));
    gearList.insert(std::end(gearList), std::begin(PlateSecondaryGear), std::end(PlateSecondaryGear));
    gearList.insert(std::end(gearList), std::begin(ClothSecondaryGear), std::end(ClothSecondaryGear));
    gearList.insert(std::end(gearList), std::begin(LeatherSecondaryGear), std::end(LeatherSecondaryGear));
    gearList.insert(std::end(gearList), std::begin(MailSecondaryGear), std::end(MailSecondaryGear));
    gearList.insert(std::end(gearList), std::begin(ProtWarriorTertiaryGear), std::end(ProtWarriorTertiaryGear));
    gearList.insert(std::end(gearList), std::begin(FuryWarriorTertiaryGear), std::end(FuryWarriorTertiaryGear));
    gearList.insert(std::end(gearList), std::begin(ArmsWarriorTertiaryGear), std::end(ArmsWarriorTertiaryGear));
    gearList.insert(std::end(gearList), std::begin(HolyPaladinTertiaryGear), std::end(HolyPaladinTertiaryGear));
    gearList.insert(std::end(gearList), std::begin(RetPaladinTertiaryGear), std::end(RetPaladinTertiaryGear));
    gearList.insert(std::end(gearList), std::begin(ProtPaladinTertiaryGear), std::end(ProtPaladinTertiaryGear));
    gearList.insert(std::end(gearList), std::begin(BalanceDruidTertiaryGear), std::end(BalanceDruidTertiaryGear));
    gearList.insert(std::end(gearList), std::begin(FeralDruidTertiaryGear), std::end(FeralDruidTertiaryGear));
    gearList.insert(std::end(gearList), std::begin(RestoDruidTertiaryGear), std::end(RestoDruidTertiaryGear));
    gearList.insert(std::end(gearList), std::begin(DiscHolyPriestTertiaryGear), std::end(DiscHolyPriestTertiaryGear));
    gearList.insert(std::end(gearList), std::begin(CasterDPSTertiaryGear), std::end(CasterDPSTertiaryGear));
    gearList.insert(std::end(gearList), std::begin(EnhanceShamanTertiaryGear), std::end(EnhanceShamanTertiaryGear));
    gearList.insert(std::end(gearList), std::begin(ElementalShamanTertiaryGear), std::end(ElementalShamanTertiaryGear));
    gearList.insert(std::end(gearList), std::begin(RestoShamanTertiaryGear), std::end(RestoShamanTertiaryGear));
    gearList.insert(std::end(gearList), std::begin(HunterTertiaryGear), std::end(HunterTertiaryGear));
    gearList.insert(std::end(gearList), std::begin(AssassinRogueTertiaryGear), std::end(AssassinRogueTertiaryGear));
    gearList.insert(std::end(gearList), std::begin(CombatSubRogueTertiaryGear), std::end(CombatSubRogueTertiaryGear));
    return gearList;
}

std::vector<uint32> GetFullGearListBiS60()
{
    std::vector<uint32> gearList;
    gearList.clear();
    gearList.insert(std::end(gearList), std::begin(Lvl60BiS_ShamanResto), std::end(Lvl60BiS_ShamanResto));
    gearList.insert(std::end(gearList), std::begin(Lvl60BiS_ShamanEnhancement), std::end(Lvl60BiS_ShamanEnhancement));
    gearList.insert(std::end(gearList), std::begin(Lvl60BiS_ShamanElemental), std::end(Lvl60BiS_ShamanElemental));
    gearList.insert(std::end(gearList), std::begin(Lvl60BiS_PriestShadow), std::end(Lvl60BiS_PriestShadow));
    gearList.insert(std::end(gearList), std::begin(Lvl60BiS_PriestDiscHoly), std::end(Lvl60BiS_PriestDiscHoly));
    gearList.insert(std::end(gearList), std::begin(Lvl60BiS_PaladinHoly), std::end(Lvl60BiS_PaladinHoly));
    gearList.insert(std::end(gearList), std::begin(Lvl60BiS_PaladinRetribution), std::end(Lvl60BiS_PaladinRetribution));
    gearList.insert(std::end(gearList), std::begin(Lvl60BiS_PaladinProtection), std::end(Lvl60BiS_PaladinProtection));
    gearList.insert(std::end(gearList), std::begin(Lvl60BiS_WarriorFuryArms), std::end(Lvl60BiS_WarriorFuryArms));
    gearList.insert(std::end(gearList), std::begin(Lvl60BiS_WarriorProtection), std::end(Lvl60BiS_WarriorProtection));
    gearList.insert(std::end(gearList), std::begin(Lvl60BiS_DruidFeralCat), std::end(Lvl60BiS_DruidFeralCat));
    gearList.insert(std::end(gearList), std::begin(Lvl60BiS_DruidFeralBear), std::end(Lvl60BiS_DruidFeralBear));
    gearList.insert(std::end(gearList), std::begin(Lvl60BiS_DruidBalance), std::end(Lvl60BiS_DruidBalance));
    gearList.insert(std::end(gearList), std::begin(Lvl60BiS_DruidResto), std::end(Lvl60BiS_DruidResto));
    gearList.insert(std::end(gearList), std::begin(Lvl60BiS_Rogue), std::end(Lvl60BiS_Rogue));
    gearList.insert(std::end(gearList), std::begin(Lvl60BiS_Mage), std::end(Lvl60BiS_Mage));
    gearList.insert(std::end(gearList), std::begin(Lvl60BiS_Hunter), std::end(Lvl60BiS_Hunter));
    gearList.insert(std::end(gearList), std::begin(Lvl60BiS_Warlock), std::end(Lvl60BiS_Warlock));
    return gearList;
}

std::vector<uint32> GetFullGearListBiS70()
{
    std::vector<uint32> gearList;
    gearList.clear();
    gearList.insert(std::end(gearList), std::begin(Lvl70BiS_ShamanResto), std::end(Lvl70BiS_ShamanResto));
    gearList.insert(std::end(gearList), std::begin(Lvl70BiS_ShamanEnhancement), std::end(Lvl70BiS_ShamanEnhancement));
    gearList.insert(std::end(gearList), std::begin(Lvl70BiS_ShamanElemental), std::end(Lvl70BiS_ShamanElemental));
    gearList.insert(std::end(gearList), std::begin(Lvl70BiS_PriestShadow), std::end(Lvl70BiS_PriestShadow));
    gearList.insert(std::end(gearList), std::begin(Lvl70BiS_PriestDiscHoly), std::end(Lvl70BiS_PriestDiscHoly));
    gearList.insert(std::end(gearList), std::begin(Lvl70BiS_PaladinHoly), std::end(Lvl70BiS_PaladinHoly));
    gearList.insert(std::end(gearList), std::begin(Lvl70BiS_PaladinRetribution), std::end(Lvl70BiS_PaladinRetribution));
    gearList.insert(std::end(gearList), std::begin(Lvl70BiS_PaladinProtection), std::end(Lvl70BiS_PaladinProtection));
    gearList.insert(std::end(gearList), std::begin(Lvl70BiS_WarriorFuryArms), std::end(Lvl70BiS_WarriorFuryArms));
    gearList.insert(std::end(gearList), std::begin(Lvl70BiS_WarriorProtection), std::end(Lvl70BiS_WarriorProtection));
    gearList.insert(std::end(gearList), std::begin(Lvl70BiS_DruidFeralCat), std::end(Lvl70BiS_DruidFeralCat));
    gearList.insert(std::end(gearList), std::begin(Lvl70BiS_DruidFeralBear), std::end(Lvl70BiS_DruidFeralBear));
    gearList.insert(std::end(gearList), std::begin(Lvl70BiS_DruidBalance), std::end(Lvl70BiS_DruidBalance));
    gearList.insert(std::end(gearList), std::begin(Lvl70BiS_DruidResto), std::end(Lvl70BiS_DruidResto));
    gearList.insert(std::end(gearList), std::begin(Lvl70BiS_Rogue), std::end(Lvl70BiS_Rogue));
    gearList.insert(std::end(gearList), std::begin(Lvl70BiS_Mage), std::end(Lvl70BiS_Mage));
    gearList.insert(std::end(gearList), std::begin(Lvl70BiS_Hunter), std::end(Lvl70BiS_Hunter));
    gearList.insert(std::end(gearList), std::begin(Lvl70BiS_Warlock), std::end(Lvl70BiS_Warlock));
    return gearList;
}

void GivePlayerItems(Player* recipient, std::vector<uint32> gearList, uint32 setId = 0, int32 suffixOverride = 0)
{
    bool allSuccess = true;
    bool alreadyHave = false;
    for (auto item : gearList)
    {
        ItemPrototype const* proto = ObjectMgr::GetItemPrototype(item);
        if (!proto)
            continue;

        if (recipient->HasItemCount(item, (proto->InventoryType != INVTYPE_FINGER && proto->InventoryType != INVTYPE_WEAPON) ? 1 : 2))
        {
            alreadyHave = true;
            continue;
        }

        if (!recipient->StoreNewItemInBestSlots(item, 1, (proto->InventoryType == INVTYPE_TRINKET || proto->InventoryType == INVTYPE_RELIC || proto->InventoryType == INVTYPE_BAG) ? 0 : -suffixOverride))
            allSuccess = false;
    }

    if (alreadyHave)
        recipient->GetSession()->SendNotification("There are already some pieces of starter gear in your possession. You must destroy it first before I can give you more!");

    if (!allSuccess)
        recipient->GetSession()->SendNotification("You can't hold some of the gear I'm trying to give you! Make room for it first and speak to me again.");
    else if (!setId || setId == SET_ID_TERTIARY || setId == SET_ID_INSTANT_58)
        recipient->GetSession()->SendNotification("This equipment will serve you well in battle.");
}

bool HasStarterSet(Player* player, std::vector<uint32> gearList)
{
    for (auto item : gearList)
    {
        ItemPrototype const* proto = ObjectMgr::GetItemPrototype(item);
        if (!proto)
            continue;

        // don't check for quiver/ammo pouch
        if (proto->InventoryType == INVTYPE_BAG || item == 23197)
            continue;

        if (player->HasItemCount(item, 1))
            return true;
    }
    return false;
}

void RemoveStarterSet(Player* player, std::vector<uint32> gearList)
{
    for (auto item : gearList)
    {
        ItemPrototype const* proto = ObjectMgr::GetItemPrototype(item);
        if (!proto)
            continue;

        // don't check for quiver/ammo pouch
        if (proto->InventoryType == INVTYPE_BAG)
            continue;

        player->DestroyItemCount(item, 2, true, false);
    }
}

void AddStarterSet(Player* player, Creature* creature, uint32 setId, int32 suffixOverride = 0, uint32 specId = 0)
{
    std::vector<uint32> gearList;

    if (setId == SET_ID_INSTANT_58)
    {
        gearList.insert(std::end(gearList), std::begin(Instant58Shared), std::end(Instant58Shared));
        switch (player->getClass())
        {
            case CLASS_WARRIOR:
                gearList.insert(std::end(gearList), std::begin(Instant58PhysicalBack), std::end(Instant58PhysicalBack));
                gearList.insert(std::end(gearList), std::begin(Instant58Bow), std::end(Instant58Bow));
                gearList.insert(std::end(gearList), std::begin(Instant58DualWield), std::end(Instant58DualWield));
                gearList.insert(std::end(gearList), std::begin(Instant58PlateOffpieces), std::end(Instant58PlateOffpieces));
                gearList.insert(std::end(gearList), std::begin(Instant58Warrior), std::end(Instant58Warrior));
                break;
            case CLASS_PALADIN:
                gearList.insert(std::end(gearList), std::begin(Instant58PhysicalBack), std::end(Instant58PhysicalBack));
                gearList.insert(std::end(gearList), std::begin(Instant58MagicBack), std::end(Instant58MagicBack));
                gearList.insert(std::end(gearList), std::begin(Instant58GavelAndShield), std::end(Instant58GavelAndShield));
                gearList.insert(std::end(gearList), std::begin(Instant58PlateOffpieces), std::end(Instant58PlateOffpieces));
                gearList.insert(std::end(gearList), std::begin(Instant58Paladin), std::end(Instant58Paladin));
                break;
            case CLASS_PRIEST:
                gearList.insert(std::end(gearList), std::begin(Instant58MagicBack), std::end(Instant58MagicBack));
                gearList.insert(std::end(gearList), std::begin(Instant58Staff), std::end(Instant58Staff));
                gearList.insert(std::end(gearList), std::begin(Instant58ClothCaster), std::end(Instant58ClothCaster));
                gearList.insert(std::end(gearList), std::begin(Instant58Priest), std::end(Instant58Priest));
                break;
            case CLASS_WARLOCK:
                gearList.insert(std::end(gearList), std::begin(Instant58MagicBack), std::end(Instant58MagicBack));
                gearList.insert(std::end(gearList), std::begin(Instant58Staff), std::end(Instant58Staff));
                gearList.insert(std::end(gearList), std::begin(Instant58ClothCaster), std::end(Instant58ClothCaster));
                gearList.insert(std::end(gearList), std::begin(Instant58Warlock), std::end(Instant58Warlock));
                break;
            case CLASS_MAGE:
                gearList.insert(std::end(gearList), std::begin(Instant58MagicBack), std::end(Instant58MagicBack));
                gearList.insert(std::end(gearList), std::begin(Instant58Staff), std::end(Instant58Staff));
                gearList.insert(std::end(gearList), std::begin(Instant58ClothCaster), std::end(Instant58ClothCaster));
                gearList.insert(std::end(gearList), std::begin(Instant58Mage), std::end(Instant58Mage));
                break;
            case CLASS_DRUID:
                gearList.insert(std::end(gearList), std::begin(Instant58PhysicalBack), std::end(Instant58PhysicalBack));
                gearList.insert(std::end(gearList), std::begin(Instant58MagicBack), std::end(Instant58MagicBack));
                gearList.insert(std::end(gearList), std::begin(Instant58LeatherOffpieces), std::end(Instant58LeatherOffpieces));
                gearList.insert(std::end(gearList), std::begin(Instant58Staff), std::end(Instant58Staff));
                gearList.insert(std::end(gearList), std::begin(Instant58Druid), std::end(Instant58Druid));
                break;
            case CLASS_ROGUE:
                gearList.insert(std::end(gearList), std::begin(Instant58Bow), std::end(Instant58Bow));
                gearList.insert(std::end(gearList), std::begin(Instant58PhysicalBack), std::end(Instant58PhysicalBack));
                gearList.insert(std::end(gearList), std::begin(Instant58LeatherOffpieces), std::end(Instant58LeatherOffpieces));
                gearList.insert(std::end(gearList), std::begin(Instant58Rogue), std::end(Instant58Rogue));
                break;
            case CLASS_SHAMAN:
                gearList.insert(std::end(gearList), std::begin(Instant58PhysicalBack), std::end(Instant58PhysicalBack));
                gearList.insert(std::end(gearList), std::begin(Instant58MagicBack), std::end(Instant58MagicBack));
                gearList.insert(std::end(gearList), std::begin(Instant58MailOffpieces), std::end(Instant58MailOffpieces));
                gearList.insert(std::end(gearList), std::begin(Instant58GavelAndShield), std::end(Instant58GavelAndShield));
                gearList.insert(std::end(gearList), std::begin(Instant58DualWield), std::end(Instant58DualWield));
                gearList.insert(std::end(gearList), std::begin(Instant58Shaman), std::end(Instant58Shaman));
                break;
            case CLASS_HUNTER:
                gearList.insert(std::end(gearList), std::begin(Instant58PhysicalBack), std::end(Instant58PhysicalBack));
                gearList.insert(std::end(gearList), std::begin(Instant58Bow), std::end(Instant58Bow));
                gearList.insert(std::end(gearList), std::begin(Instant58MailOffpieces), std::end(Instant58MailOffpieces));
                gearList.insert(std::end(gearList), std::begin(Instant58Hunter), std::end(Instant58Hunter));
                break;
        }
    }
    else if (setId == SET_ID_PRIMARY)
    {
        switch (player->getClass())
        {
            case CLASS_WARRIOR:
            case CLASS_PALADIN:
                gearList.insert(std::end(gearList), std::begin(PlatePrimaryGear), std::end(PlatePrimaryGear));
                break;
            case CLASS_PRIEST:
            case CLASS_WARLOCK:
            case CLASS_MAGE:
                gearList.insert(std::end(gearList), std::begin(ClothPrimaryGear), std::end(ClothPrimaryGear));
                break;
            case CLASS_DRUID:
            case CLASS_ROGUE:
                gearList.insert(std::end(gearList), std::begin(LeatherPrimaryGear), std::end(LeatherPrimaryGear));
                break;
            case CLASS_SHAMAN:
            case CLASS_HUNTER:
                gearList.insert(std::end(gearList), std::begin(MailPrimaryGear), std::end(MailPrimaryGear));
                break;
        }
    }
    else if (setId == SET_ID_SECONDARY)
    {
        gearList.insert(std::end(gearList), std::begin(SharedSecondaryGear), std::end(SharedSecondaryGear));
        switch (player->getClass())
        {
            case CLASS_WARRIOR:
            case CLASS_PALADIN:
                gearList.insert(std::end(gearList), std::begin(PlateSecondaryGear), std::end(PlateSecondaryGear));
                break;
            case CLASS_PRIEST:
            case CLASS_WARLOCK:
            case CLASS_MAGE:
                gearList.insert(std::end(gearList), std::begin(ClothSecondaryGear), std::end(ClothSecondaryGear));
                break;
            case CLASS_DRUID:
            case CLASS_ROGUE:
                gearList.insert(std::end(gearList), std::begin(LeatherSecondaryGear), std::end(LeatherSecondaryGear));
                break;
            case CLASS_SHAMAN:
            case CLASS_HUNTER:
                gearList.insert(std::end(gearList), std::begin(MailSecondaryGear), std::end(MailSecondaryGear));
                break;
        }
    }
    else if (setId == SET_ID_TERTIARY)
    {
        switch (player->getClass())
        {
            case CLASS_WARRIOR:
                if (specId == 0) gearList.insert(std::end(gearList), std::begin(ArmsWarriorTertiaryGear), std::end(ArmsWarriorTertiaryGear));
                else if (specId == 1) gearList.insert(std::end(gearList), std::begin(FuryWarriorTertiaryGear), std::end(FuryWarriorTertiaryGear));
                else if (specId == 2) gearList.insert(std::end(gearList), std::begin(ProtWarriorTertiaryGear), std::end(ProtWarriorTertiaryGear));
                break;
            case CLASS_PALADIN:
                if (specId == 0) gearList.insert(std::end(gearList), std::begin(HolyPaladinTertiaryGear), std::end(HolyPaladinTertiaryGear));
                else if (specId == 1) gearList.insert(std::end(gearList), std::begin(ProtPaladinTertiaryGear), std::end(ProtPaladinTertiaryGear));
                else if (specId == 2) gearList.insert(std::end(gearList), std::begin(RetPaladinTertiaryGear), std::end(RetPaladinTertiaryGear));
                break;
            case CLASS_PRIEST:
                if (specId == 0 || specId == 1) gearList.insert(std::end(gearList), std::begin(DiscHolyPriestTertiaryGear), std::end(DiscHolyPriestTertiaryGear));
                else if (specId == 2) gearList.insert(std::end(gearList), std::begin(CasterDPSTertiaryGear), std::end(CasterDPSTertiaryGear));
                break;
            case CLASS_WARLOCK:
            case CLASS_MAGE:
                gearList.insert(std::end(gearList), std::begin(CasterDPSTertiaryGear), std::end(CasterDPSTertiaryGear));
                break;
            case CLASS_DRUID:
                if (specId == 0) gearList.insert(std::end(gearList), std::begin(BalanceDruidTertiaryGear), std::end(BalanceDruidTertiaryGear));
                else if (specId == 1) gearList.insert(std::end(gearList), std::begin(FeralDruidTertiaryGear), std::end(FeralDruidTertiaryGear));
                else if (specId == 2) gearList.insert(std::end(gearList), std::begin(RestoDruidTertiaryGear), std::end(RestoDruidTertiaryGear));
                break;
            case CLASS_ROGUE:
                if (specId == 0) gearList.insert(std::end(gearList), std::begin(AssassinRogueTertiaryGear), std::end(AssassinRogueTertiaryGear));
                else if (specId == 1 || specId == 2) gearList.insert(std::end(gearList), std::begin(CombatSubRogueTertiaryGear), std::end(CombatSubRogueTertiaryGear));
                break;
            case CLASS_SHAMAN:
                if (specId == 0) gearList.insert(std::end(gearList), std::begin(ElementalShamanTertiaryGear), std::end(ElementalShamanTertiaryGear));
                else if (specId == 1) gearList.insert(std::end(gearList), std::begin(EnhanceShamanTertiaryGear), std::end(EnhanceShamanTertiaryGear));
                else if (specId == 2) gearList.insert(std::end(gearList), std::begin(RestoShamanTertiaryGear), std::end(RestoShamanTertiaryGear));
                break;
            case CLASS_HUNTER:
                gearList.insert(std::end(gearList), std::begin(HunterTertiaryGear), std::end(HunterTertiaryGear));
                break;
        }
    }
    GivePlayerItems(player, gearList, setId, suffixOverride);
}

void OfferPrimarySecondaryModChoices(Player* player, uint32 actionIdBase, uint32 specId)
{
    switch (player->getClass())
    {
        case CLASS_WARRIOR:
            if (specId == 0 || specId == 1) { // Arms/Fury
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Ancestor (Str, Crit, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 54);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Battle (Str, Stam, Crit)", GOSSIP_SENDER_MAIN, actionIdBase + 56);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Beast (Str, Agil, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 41);
            }
            else if (specId == 2) { // Protection
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Blocking (Block, Str)", GOSSIP_SENDER_MAIN, actionIdBase + 47);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Champion (Str, Stam, Def)", GOSSIP_SENDER_MAIN, actionIdBase + 45);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Soldier (Str, Stam, Crit)", GOSSIP_SENDER_MAIN, actionIdBase + 43);
            }
            break;
        case CLASS_PALADIN:
            if (specId == 0) { // Holy
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Elder (Stam, Int, Mana Regen)", GOSSIP_SENDER_MAIN, actionIdBase + 44);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Mind (Spell Dmg/Heal, Spell Crit, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 51);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Physician (Stam, Int, Spell Heal)", GOSSIP_SENDER_MAIN, actionIdBase + 37);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Vision (Spell Dmg/Heal, Int, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 53);
            }
            else if (specId == 1) { // Protection
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Champion (Str, Stam, Def)", GOSSIP_SENDER_MAIN, actionIdBase + 45);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Crusade (Spell Dmg/Heal, Int, Def)", GOSSIP_SENDER_MAIN, actionIdBase + 52);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Knight (Stam, Spell Dmg/Heal, Def)", GOSSIP_SENDER_MAIN, actionIdBase + 66);
            }
            else if (specId == 2) { // Retribution
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Ancestor (Str, Crit, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 54);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Beast (Str, Agil, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 41);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Soldier (Str, Stam, Crit)", GOSSIP_SENDER_MAIN, actionIdBase + 43);
            }
            break;
        case CLASS_PRIEST:
            if (specId == 0 || specId == 1) { // Discipline/Holy
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Elder (Stam, Int, Mana Regen)", GOSSIP_SENDER_MAIN, actionIdBase + 44);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Mind (Spell Dmg/Heal, Spell Crit, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 51);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Physician (Stam, Int, Spell Heal)", GOSSIP_SENDER_MAIN, actionIdBase + 37);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Vision (Spell Dmg/Heal, Int, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 53);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Prophet (Int, Spirit, Spell Heal)", GOSSIP_SENDER_MAIN, actionIdBase + 38);
            }
            else if (specId == 2) { // Shadow
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Nightmare (Shadow Dmg, Stam, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 55);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Elder (Stam, Int, Mana Regen)", GOSSIP_SENDER_MAIN, actionIdBase + 44);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Mind (Spell Dmg/Heal, Spell Crit, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 51);
            }
            break;
        case CLASS_DRUID:
            if (specId == 0) { // Balance
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Elder (Stam, Int, Mana Regen)", GOSSIP_SENDER_MAIN, actionIdBase + 44);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Mind (Spell Dmg/Heal, Spell Crit, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 51);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Vision (Spell Dmg/Heal, Int, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 53);
            }
            else if (specId == 1) { // Feral
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Bandit (Agi, Stam, Attack Power)", GOSSIP_SENDER_MAIN, actionIdBase + 40);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Beast (Str, Agil, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 41);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Champion (Str, Stam, Def)", GOSSIP_SENDER_MAIN, actionIdBase + 45);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Shadow (Attack Power, Agil, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 57);
            }
            else if (specId == 2) { // Restoration
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Elder (Stam, Int, Mana Regen)", GOSSIP_SENDER_MAIN, actionIdBase + 44);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Hierophant (Stam, Spirit, Spell Heal)", GOSSIP_SENDER_MAIN, actionIdBase + 42);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Prophet (Int, Spirit, Spell Heal)", GOSSIP_SENDER_MAIN, actionIdBase + 38);
            }
            break;
        case CLASS_SHAMAN:
            if (specId == 0) // Elemental
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Invoker (Int, Spell Dmg/Heal, Spell Crit)", GOSSIP_SENDER_MAIN, actionIdBase + 39);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Elder (Stam, Int, Mana Regen)", GOSSIP_SENDER_MAIN, actionIdBase + 44);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Mind (Spell Dmg/Heal, Spell Crit, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 51);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Vision (Spell Dmg/Heal, Int, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 53);
            }
            else if (specId == 1) // Enhancement
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Ancestor (Str, Crit, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 54);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Beast (Str, Agil, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 41);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Hunt (Attack Power, Agil, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 50);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Wild (Attack Power, Stam, Agil)", GOSSIP_SENDER_MAIN, actionIdBase + 60);
            }
            else if (specId == 2) // Restoration
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Elder (Stam, Int, Mana Regen)", GOSSIP_SENDER_MAIN, actionIdBase + 44);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Mind (Spell Dmg/Heal, Spell Crit, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 51);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Physician (Stam, Int, Spell Heal)", GOSSIP_SENDER_MAIN, actionIdBase + 37);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Prophet (Int, Spirit, Spell Heal)", GOSSIP_SENDER_MAIN, actionIdBase + 38);
            }
            break;
        case CLASS_WARLOCK: // All specs
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Invoker (Int, Spell Dmg/Heal, Spell Crit)", GOSSIP_SENDER_MAIN, actionIdBase + 39);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Mind (Spell Dmg/Heal, Spell Crit, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 51);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Vision (Spell Dmg/Heal, Int, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 53);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Nightmare (Shadow Dmg, Stam, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 55);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Sorcerer (Stam, Int, Spell Dmg/Heal)", GOSSIP_SENDER_MAIN, actionIdBase + 36);
            break;
        case CLASS_ROGUE:   // All specs
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Bandit (Agi, Stam, Attack Power)", GOSSIP_SENDER_MAIN, actionIdBase + 40);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Shadow (Attack Power, Agil, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 57);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Wild (Attack Power, Stam, Agil)", GOSSIP_SENDER_MAIN, actionIdBase + 60);
            break;
        case CLASS_HUNTER:  // All specs
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Hunt (Attack Power, Agil, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 50);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Bandit (Agil, Stam, Attack Power)", GOSSIP_SENDER_MAIN, actionIdBase + 40);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Shadow (Attack Power, Agil, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 57);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Wild (Attack Power, Stam, Agil)", GOSSIP_SENDER_MAIN, actionIdBase + 60);
            break;
        case CLASS_MAGE:    // All specs
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Invoker (Int, Spell Dmg/Heal, Spell Crit)", GOSSIP_SENDER_MAIN, actionIdBase + 39);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Mind (Spell Dmg/Heal, Spell Crit, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 51);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Vision (Spell Dmg/Heal, Int, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 53);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Sorcerer (Stam, Int, Spell Dmg/Heal)", GOSSIP_SENDER_MAIN, actionIdBase + 36);
            break;
    }
}

void OfferTertiaryModChoices(Player* player, uint32 actionIdBase, uint32 specId)
{
    switch (player->getClass())
    {
        case CLASS_WARRIOR:
            if (specId == 0 || specId == 1) // Arms/Fury
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Tiger (Str, Agil)", GOSSIP_SENDER_MAIN, actionIdBase + 14);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Bear (Str, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 7);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Boar (Str, Spirit)", GOSSIP_SENDER_MAIN, actionIdBase + 12);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Strength", GOSSIP_SENDER_MAIN, actionIdBase + 17);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Agility", GOSSIP_SENDER_MAIN, actionIdBase + 18);
            }
            else if (specId == 2) // Protection
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Tiger (Str, Agil)", GOSSIP_SENDER_MAIN, actionIdBase + 14);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Bear (Str, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 7);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Boar (Str, Spirit)", GOSSIP_SENDER_MAIN, actionIdBase + 12);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Strength", GOSSIP_SENDER_MAIN, actionIdBase + 17);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Agility", GOSSIP_SENDER_MAIN, actionIdBase + 18);
            }
            break;
        case CLASS_PALADIN:
            if (specId == 0) // Holy
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Eagle (Stam, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 6);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Owl (Int, Spirit)", GOSSIP_SENDER_MAIN, actionIdBase + 9);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Intellect", GOSSIP_SENDER_MAIN, actionIdBase + 19);
            }
            else if (specId == 1) // Protection
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Bear (Str, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 7);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Gorilla (Str, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 10);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Eagle (Stam, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 6);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Strength", GOSSIP_SENDER_MAIN, actionIdBase + 17);
            }
            else if (specId == 2) // Retribution
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Bear (Str, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 7);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Gorilla (Str, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 10);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Tiger (Str, Agil)", GOSSIP_SENDER_MAIN, actionIdBase + 14);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Strength", GOSSIP_SENDER_MAIN, actionIdBase + 17);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Agility", GOSSIP_SENDER_MAIN, actionIdBase + 18);
            }
            break;
        case CLASS_PRIEST: // All specs
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Owl (Int, Spirit)", GOSSIP_SENDER_MAIN, actionIdBase + 9);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Whale (Stam, Spirit)", GOSSIP_SENDER_MAIN, actionIdBase + 8);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Spirit", GOSSIP_SENDER_MAIN, actionIdBase + 15);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Intellect", GOSSIP_SENDER_MAIN, actionIdBase + 19);
            break;
        case CLASS_DRUID:
            if (specId == 0) // Balance
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Eagle (Stam, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 6);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Whale (Stam, Spirit)", GOSSIP_SENDER_MAIN, actionIdBase + 8);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Spirit", GOSSIP_SENDER_MAIN, actionIdBase + 15);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Intellect", GOSSIP_SENDER_MAIN, actionIdBase + 19);
            }
            else if (specId == 1) // Feral
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Bear (Str, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 7);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Monkey (Agil, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 5);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Tiger (Str, Agil)", GOSSIP_SENDER_MAIN, actionIdBase + 14);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Agility", GOSSIP_SENDER_MAIN, actionIdBase + 18);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Strength", GOSSIP_SENDER_MAIN, actionIdBase + 17);
            }
            else if (specId == 2) // Restoration
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Eagle (Stam, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 6);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Whale (Stam, Spirit)", GOSSIP_SENDER_MAIN, actionIdBase + 8);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Spirit", GOSSIP_SENDER_MAIN, actionIdBase + 15);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Intellect", GOSSIP_SENDER_MAIN, actionIdBase + 19);
            }
            break;
        case CLASS_SHAMAN:
            if (specId == 0) // Elemental
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Eagle (Stam, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 6);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Owl (Int, Spirit)", GOSSIP_SENDER_MAIN, actionIdBase + 9);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Intellect", GOSSIP_SENDER_MAIN, actionIdBase + 19);
            }
            else if (specId == 1) // Enhancement
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Falcon (Agil, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 11);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Monkey (Agil, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 5);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Tiger (Str, Agil)", GOSSIP_SENDER_MAIN, actionIdBase + 14);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Gorilla (Str, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 10);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Wolf (Agil, Spirit)", GOSSIP_SENDER_MAIN, actionIdBase + 13);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Agility", GOSSIP_SENDER_MAIN, actionIdBase + 18);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Strength", GOSSIP_SENDER_MAIN, actionIdBase + 17);
            }
            else if (specId == 2) // Restoration
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Eagle (Stam, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 6);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Owl (Int, Spirit)", GOSSIP_SENDER_MAIN, actionIdBase + 9);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Spirit", GOSSIP_SENDER_MAIN, actionIdBase + 15);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Intellect", GOSSIP_SENDER_MAIN, actionIdBase + 19);
            }
            break;
        case CLASS_WARLOCK: // All specs
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Eagle (Stam, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 6);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Whale (Stam, Spirit)", GOSSIP_SENDER_MAIN, actionIdBase + 8);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Intellect", GOSSIP_SENDER_MAIN, actionIdBase + 19);
            break;
        case CLASS_ROGUE:   // All specs
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Monkey (Agil, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 5);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Tiger (Str, Agil)", GOSSIP_SENDER_MAIN, actionIdBase + 14);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Agility", GOSSIP_SENDER_MAIN, actionIdBase + 18);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Strength", GOSSIP_SENDER_MAIN, actionIdBase + 17);
            break;
        case CLASS_HUNTER:  // All specs
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Monkey (Agil, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 5);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Falcon (Agil, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 11);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Wolf (Agil, Spirit)", GOSSIP_SENDER_MAIN, actionIdBase + 13);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Intellect", GOSSIP_SENDER_MAIN, actionIdBase + 19);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Agility", GOSSIP_SENDER_MAIN, actionIdBase + 18);
            break;
        case CLASS_MAGE:    // All specs
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Eagle (Stam, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 6);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Owl (Int, Spirit)", GOSSIP_SENDER_MAIN, actionIdBase + 9);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Spirit", GOSSIP_SENDER_MAIN, actionIdBase + 15);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Intellect", GOSSIP_SENDER_MAIN, actionIdBase + 19);
            break;
    }
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Stamina", GOSSIP_SENDER_MAIN, actionIdBase + 16); // all classes/specs may opt for this here
}

enum
{
    NPC_TEST_REALM_OVERLORD     = 980029,
    NPC_ALLIANCE_OFFICER        = 980024,
    NPC_HORDE_OFFICER           = 980023,
    NPC_GREETER_ALLIANCE        = 980028,
    NPC_GREETER_HORDE           = 980027,

    SAY_GREET_OFFICER           = -1800999,
    SAY_GREET_OVERLORD          = -1800998,
    SAY_GREET_BOOSTER           = -1800997,

    GAME_EVENT_LEVEL_BOOST      = 9000,
};

bool CanBoost(Player* player)
{
    // should be prevented by first gossip option condition - just checking for redundancy here
    if (!sGameEventMgr.IsActiveEvent(GAME_EVENT_LEVEL_BOOST))
        return false;

    uint32 boostMinLevelRestricted = sWorld.getConfig(CONFIG_UINT32_BOOST_MIN_LEVEL_RESTRICTED);
    uint32 boostLevel = sWorld.getConfig(CONFIG_UINT32_BOOST_LEVEL);
    bool isRaceRestricted = false;
    if (uint32 restrictionStateFlags = sWorldState.IsTbcRaceBoostRestricted())
    {
        if (restrictionStateFlags & BOOST_FLAG_TBC_RACES)
        {
            if (player->getRace() == RACE_DRAENEI || player->getRace() == RACE_BLOODELF)
                if (player->GetLevel() < boostMinLevelRestricted)
                    isRaceRestricted = true;
        }
        if (restrictionStateFlags & BOOST_FLAG_HORDE_RACES)
        {
            if (player->getRace() == RACE_ORC || player->getRace() == RACE_UNDEAD || player->getRace() == RACE_TAUREN || player->getRace() == RACE_TROLL)
                if (player->GetLevel() < boostMinLevelRestricted)
                    isRaceRestricted = true;
        }
        if (restrictionStateFlags & BOOST_FLAG_ALLIANCE_RACES)
        {
            if (player->getRace() == RACE_HUMAN || player->getRace() == RACE_DWARF || player->getRace() == RACE_NIGHTELF || player->getRace() == RACE_GNOME)
                if (player->GetLevel() < boostMinLevelRestricted)
                    isRaceRestricted = true;
        }
    }
    if (player->GetLevel() < boostLevel && !isRaceRestricted)
        return true;
    else
        return false;
}

struct GreeterAI : public ScriptedAI
{
    GreeterAI(Creature* creature) : ScriptedAI(creature)
    {
        Reset();
    }

    bool m_bCanGreet;
    uint32 m_uiGreetTimer;
    GuidList m_lPlayerGuids;
    uint32 m_uiBoostLevel;

    void Reset() override
    {
        m_uiGreetTimer = 2000;
        m_bCanGreet = true;
        m_lPlayerGuids.clear();
        m_uiBoostLevel = sWorld.getConfig(CONFIG_UINT32_BOOST_LEVEL);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiGreetTimer < uiDiff)
        {
            m_bCanGreet = true;
        }
        else
            m_uiGreetTimer -= uiDiff;
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (m_bCanGreet && pWho->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(pWho, 15.0f) && m_creature->IsWithinLOSInMap(pWho))
        {
            if (((Player*)pWho)->IsGameMaster())
                return;

            // check to see if player has been greeted already
            for (GuidList::const_iterator itr = m_lPlayerGuids.begin(); itr != m_lPlayerGuids.end(); ++itr)
            {
                if ((*itr) == pWho->GetObjectGuid())
                    return;
            }

            m_lPlayerGuids.push_back(pWho->GetObjectGuid());

            if (pWho->GetLevel() == m_uiBoostLevel && ((((Player*)pWho)->GetTeam() == HORDE && m_creature->GetEntry() == NPC_HORDE_OFFICER) || (((Player*)pWho)->GetTeam() == ALLIANCE && m_creature->GetEntry() == NPC_ALLIANCE_OFFICER)))
            {
                m_creature->SetFacingToObject(pWho);
                DoScriptText(SAY_GREET_OFFICER, m_creature, pWho);
            }
            else if (m_creature->GetEntry() == NPC_TEST_REALM_OVERLORD)
            {
                m_creature->SetFacingToObject(pWho);
                DoScriptText(SAY_GREET_OVERLORD, m_creature, pWho);
            }
            else if ((m_creature->GetEntry() == NPC_GREETER_ALLIANCE || m_creature->GetEntry() == NPC_GREETER_HORDE) && CanBoost((Player*)pWho))
            {
                m_creature->SetFacingToObject(pWho);
                DoScriptText(SAY_GREET_BOOSTER, m_creature, pWho);
            }

            m_bCanGreet = false;
            m_uiGreetTimer = urand(5000, 10000);
        }
        ScriptedAI::MoveInLineOfSight(pWho);
    }
};

enum
{
    SPELL_TELEPORT_VISUAL = 41236,

    GOSSIP_TEXT_PICK_SPEC       = 50409,
    GOSSIP_TEXT_PICK_GEAR_1     = 50411,
    GOSSIP_TEXT_PICK_GEAR_2     = 50412,
    GOSSIP_TEXT_PICK_GEAR_3     = 50413,
    GOSSIP_TEXT_OVERLORD        = 50414,
    GOSSIP_TEXT_START_ZONE      = 50415,
};

bool GossipHello_npc_test_realm_overlord(Player* player, Creature* creature)
{
    player->PrepareGossipMenu(creature, GOSSIP_TEXT_OVERLORD);

    if (player->GetLevel() < 70)
    {
        if (player->GetLevel() < 60)
            player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, "|cFF00008BBoost to level 60|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3, "Are you sure?", 0, false);

        player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, "|cFF006400Boost to level 70|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4, "Are you sure?", 0, false);
    }

    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Teleport to Starting Zone", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);

    if (player->GetLevel() >= 60)
    {
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Teleport to Shattrath City", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Teleport to Blasted Lands (Dark Portal)", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Teleport to Mall (Quel'thalas)", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "|cFF0008E8Full \"best in slot\" gear - Classic|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2000);
    }
    if (player->GetLevel() >= 70)
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "|cFF0008E8Full \"best in slot\" gear - TBC|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3000);

    if (HasStarterSet(player, GetFullGearListBiS60()))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "|cFF8B0000Remove all level 60 BiS gear.|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);
    if (HasStarterSet(player, GetFullGearListBiS70()))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "|cFF8B0000Remove all level 70 BiS gear.|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);

    if (HasStarterSet(player, GetFullGearListCustom()))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "|cFF8B0000Remove all custom uncommon gear.|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
    else if (player->GetLevel() >= 70)
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Build starter set of uncommon quality level 70 gear with custom stat modifiers", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

    uint32 groundMount = GetStarterEpicMountForRace(player);
    uint32 flyingMount = GetStarterFlyingMountForFaction(player);
    if ((groundMount || flyingMount) && player->GetLevel() >= 70)
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "I seem to be missing my mount. Can you give me one?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    player->SendPreparedGossip(creature);
    return true;
}

bool GossipSelect_npc_test_realm_overlord(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
{
    player->GetPlayerMenu()->ClearMenus();
    
    switch (action)
    {
        // Main Menu
        case GOSSIP_ACTION_INFO_DEF:
            GossipHello_npc_test_realm_overlord(player, creature);
            break;
        // Add Mount
        case GOSSIP_ACTION_INFO_DEF + 1:
        {
            player->CLOSE_GOSSIP_MENU();

            if (uint32 groundMount = GetStarterEpicMountForRace(player))
                player->StoreNewItemInBestSlots(groundMount, 1);

            if (player->GetLevel() == 70)
                if (uint32 flyingMount = GetStarterFlyingMountForFaction(player))
                    player->StoreNewItemInBestSlots(flyingMount, 1);
            break;
        }
        // Choose Instant 70 Starter Set
        case GOSSIP_ACTION_INFO_DEF + 2:
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "<- Back to Main Menu", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
            switch (player->getClass())
            {
                case CLASS_WARRIOR:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Arms", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 100);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Fury", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 101);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Protection", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 102);
                    break;
                case CLASS_PALADIN:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Holy", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 100);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Protection", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 101);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Retribution", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 102);
                    break;
                case CLASS_PRIEST:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Discipline", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 100);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Holy", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 101);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Shadow", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 102);
                    break;
                case CLASS_DRUID:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Balance", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 100);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Feral", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 101);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Restoration", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 102);
                    break;
                case CLASS_SHAMAN:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Elemental", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 100);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Enhancement", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 101);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Restoration", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 102);
                    break;
                case CLASS_ROGUE:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Assassination", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 100);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Combat", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 101);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Subtlety", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 102);
                    break;

                // these only give the illusion of choice
                case CLASS_WARLOCK:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Affliction", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 100);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Demonology", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 100);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Destruction", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 100);
                    break;
                case CLASS_HUNTER:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Beast Mastery", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 100);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Marksmanship", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 100);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Survival", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 100);
                    break;
                case CLASS_MAGE:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Arcane", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 100);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Fire", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 100);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Frost", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 100);
                    break;
            }
            player->SEND_GOSSIP_MENU(GOSSIP_TEXT_PICK_SPEC, creature->GetObjectGuid());
            break;
        }
        // Level 60 Boost
        case GOSSIP_ACTION_INFO_DEF + 3:
        {
            BoostPlayer(player, 60);
            GossipHello_npc_test_realm_overlord(player, creature);
            break;
        }
        // Level 70 Boost
        case GOSSIP_ACTION_INFO_DEF + 4:
        {
            BoostPlayer(player, 70);
            GossipHello_npc_test_realm_overlord(player, creature);
            break;
        }
        // Teleport - Shattrath City
        case GOSSIP_ACTION_INFO_DEF + 5:
        {
            player->CLOSE_GOSSIP_MENU();
            player->CastSpell(player, SPELL_TELEPORT_VISUAL, TRIGGERED_OLD_TRIGGERED);
            if (player->GetTeam() == ALLIANCE)
                player->TeleportTo(teleLocs[2].map, teleLocs[2].x, teleLocs[2].y, teleLocs[2].z, teleLocs[2].o);
            else
                player->TeleportTo(teleLocs[3].map, teleLocs[3].x, teleLocs[3].y, teleLocs[3].z, teleLocs[3].o);
            break;
        }
        // Teleport - Blasted Lands (Dark Portal)
        case GOSSIP_ACTION_INFO_DEF + 6:
        {
            player->CLOSE_GOSSIP_MENU();
            player->CastSpell(player, SPELL_TELEPORT_VISUAL, TRIGGERED_OLD_TRIGGERED);
            if (player->GetTeam() == ALLIANCE)
                player->TeleportTo(teleLocs[0].map, teleLocs[0].x, teleLocs[0].y, teleLocs[0].z, teleLocs[0].o);
            else
                player->TeleportTo(teleLocs[1].map, teleLocs[1].x, teleLocs[1].y, teleLocs[1].z, teleLocs[1].o);
            break;
        }
        // Teleport - Mall
        case GOSSIP_ACTION_INFO_DEF + 7:
        {
            player->CLOSE_GOSSIP_MENU();
            player->CastSpell(player, SPELL_TELEPORT_VISUAL, TRIGGERED_OLD_TRIGGERED);
            player->TeleportTo(teleLocs[12].map, teleLocs[12].x, teleLocs[12].y, teleLocs[12].z, teleLocs[12].o);
            break;
        }
        // Teleport - Starting Zones (submenu)
        case GOSSIP_ACTION_INFO_DEF + 8:
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "<- Back to Main Menu", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Gnome/Dwarf - Coldridge Valley", GOSSIP_SENDER_MAIN, 8000 + RACE_DWARF);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Human - Northshire Valley", GOSSIP_SENDER_MAIN, 8000 + RACE_HUMAN);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Night Elf - Shadowglen", GOSSIP_SENDER_MAIN, 8000 + RACE_NIGHTELF);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Draenei - Ammen Vale", GOSSIP_SENDER_MAIN, 8000 + RACE_DRAENEI);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Orc/Troll - Valley of Trials", GOSSIP_SENDER_MAIN, 8000 + RACE_ORC);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Undead - Deathknell", GOSSIP_SENDER_MAIN, 8000 + RACE_UNDEAD);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Tauren - Red Cloud Mesa", GOSSIP_SENDER_MAIN, 8000 + RACE_TAUREN);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Blood Elf - Sunstrider Isle", GOSSIP_SENDER_MAIN, 8000 + RACE_BLOODELF);
            player->SEND_GOSSIP_MENU(GOSSIP_TEXT_START_ZONE, creature->GetObjectGuid());
            break;
        }
        // Remove Starter Set - BiS 60
        case GOSSIP_ACTION_INFO_DEF + 9:
        {
            RemoveStarterSet(player, GetFullGearListBiS60());
            player->CLOSE_GOSSIP_MENU();
            break;
        }
        // Remove Starter Set - BiS 70
        case GOSSIP_ACTION_INFO_DEF + 10:
        {
            RemoveStarterSet(player, GetFullGearListBiS70());
            player->CLOSE_GOSSIP_MENU();
            break;
        }
        // Remove Starter Set - Custom 70 Uncommon
        case GOSSIP_ACTION_INFO_DEF + 11:
        {
            RemoveStarterSet(player, GetFullGearListCustom());
            player->CLOSE_GOSSIP_MENU();
            break;
        }

        // Primary - Specialization 1
        case GOSSIP_ACTION_INFO_DEF + 100:
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "<- Back to Spec Selection", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            OfferPrimarySecondaryModChoices(player, 200, 0);
            player->SEND_GOSSIP_MENU(GOSSIP_TEXT_PICK_GEAR_1, creature->GetObjectGuid());
            break;
        }
        // Primary - Specialization 2
        case GOSSIP_ACTION_INFO_DEF + 101:
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "<- Back to Spec Selection", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            OfferPrimarySecondaryModChoices(player, 700, 1);
            player->SEND_GOSSIP_MENU(GOSSIP_TEXT_PICK_GEAR_1, creature->GetObjectGuid());
            break;
        }
        // Primary - Specialization 3
        case GOSSIP_ACTION_INFO_DEF + 102:
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "<- Back to Spec Selection", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            OfferPrimarySecondaryModChoices(player, 1000, 2);
            player->SEND_GOSSIP_MENU(GOSSIP_TEXT_PICK_GEAR_1, creature->GetObjectGuid());
            break;
        }

        // Classic Full Best in Slot
        // Pure DPS classes Hunter, Rogue, Mage, and Warlock don't require a secondary selection
        case GOSSIP_ACTION_INFO_DEF + 2000:
        {
            switch (player->getClass())
            {
                case CLASS_ROGUE:
                    GivePlayerItems(player, Lvl60BiS_Rogue);
                    player->CLOSE_GOSSIP_MENU();
                    break;
                case CLASS_MAGE:
                    GivePlayerItems(player, Lvl60BiS_Mage);
                    player->CLOSE_GOSSIP_MENU();
                    break;
                case CLASS_HUNTER:
                    GivePlayerItems(player, Lvl60BiS_Hunter);
                    player->CLOSE_GOSSIP_MENU();
                    break;
                case CLASS_WARLOCK:
                    GivePlayerItems(player, Lvl60BiS_Warlock);
                    player->CLOSE_GOSSIP_MENU();
                    break;
                case CLASS_SHAMAN:
                    player->PrepareGossipMenu(creature, GOSSIP_TEXT_PICK_SPEC);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Restoration", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2001);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Enhancement", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2002);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Elemental", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2003);
                    player->SendPreparedGossip(creature);
                    break;
                case CLASS_PRIEST:
                    player->PrepareGossipMenu(creature, GOSSIP_TEXT_PICK_SPEC);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Shadow", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2004);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Discipline/Holy", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2005);
                    player->SendPreparedGossip(creature);
                    break;
                case CLASS_PALADIN:
                    player->PrepareGossipMenu(creature, GOSSIP_TEXT_PICK_SPEC);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Holy", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2006);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Retribution", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2007);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Protection", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2008);
                    player->SendPreparedGossip(creature);
                    break;
                case CLASS_WARRIOR:
                    player->PrepareGossipMenu(creature, GOSSIP_TEXT_PICK_SPEC);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Fury/Arms", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2009);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Protection", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2010);
                    player->SendPreparedGossip(creature);
                    break;
                case CLASS_DRUID:
                    player->PrepareGossipMenu(creature, GOSSIP_TEXT_PICK_SPEC);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Feral (Cat/DPS)", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2011);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Feral (Bear/Tank)", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2012);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Balance", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2013);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Restoration", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2014);
                    player->SendPreparedGossip(creature);
                    break;
            }
            break;
        }

        // Classic Full Best in Slot (Spec Selected)
        // Shaman - Restoration
        case GOSSIP_ACTION_INFO_DEF + 2001: GivePlayerItems(player, Lvl60BiS_ShamanResto); player->CLOSE_GOSSIP_MENU(); break;
        // Shaman - Enhancement
        case GOSSIP_ACTION_INFO_DEF + 2002: GivePlayerItems(player, Lvl60BiS_ShamanEnhancement); player->CLOSE_GOSSIP_MENU(); break;
        // Shaman - Elemental
        case GOSSIP_ACTION_INFO_DEF + 2003: GivePlayerItems(player, Lvl60BiS_ShamanElemental); player->CLOSE_GOSSIP_MENU(); break;
        // Priest - Shadow
        case GOSSIP_ACTION_INFO_DEF + 2004: GivePlayerItems(player, Lvl60BiS_PriestShadow); player->CLOSE_GOSSIP_MENU(); break;
        // Priest - Discipline/Holy
        case GOSSIP_ACTION_INFO_DEF + 2005: GivePlayerItems(player, Lvl60BiS_PriestDiscHoly); player->CLOSE_GOSSIP_MENU(); break;
        // Paladin - Holy
        case GOSSIP_ACTION_INFO_DEF + 2006: GivePlayerItems(player, Lvl60BiS_PaladinHoly); player->CLOSE_GOSSIP_MENU(); break;
        // Paladin - Retribution
        case GOSSIP_ACTION_INFO_DEF + 2007: GivePlayerItems(player, Lvl60BiS_PaladinRetribution); player->CLOSE_GOSSIP_MENU(); break;
        // Paladin - Protection
        case GOSSIP_ACTION_INFO_DEF + 2008: GivePlayerItems(player, Lvl60BiS_PaladinProtection); player->CLOSE_GOSSIP_MENU(); break;
        // Warrior - Fury/Arms
        case GOSSIP_ACTION_INFO_DEF + 2009: GivePlayerItems(player, Lvl60BiS_WarriorFuryArms); player->CLOSE_GOSSIP_MENU(); break;
        // Warrior - Protection
        case GOSSIP_ACTION_INFO_DEF + 2010: GivePlayerItems(player, Lvl60BiS_WarriorProtection); player->CLOSE_GOSSIP_MENU(); break;
        // Druid - Feral (Cat/DPS)
        case GOSSIP_ACTION_INFO_DEF + 2011: GivePlayerItems(player, Lvl60BiS_DruidFeralCat); player->CLOSE_GOSSIP_MENU(); break;
        // Druid - Feral (Bear/Tank)
        case GOSSIP_ACTION_INFO_DEF + 2012: GivePlayerItems(player, Lvl60BiS_DruidFeralBear); player->CLOSE_GOSSIP_MENU(); break;
        // Druid - Balance
        case GOSSIP_ACTION_INFO_DEF + 2013: GivePlayerItems(player, Lvl60BiS_DruidBalance); player->CLOSE_GOSSIP_MENU(); break;
        // Druid - Restoration
        case GOSSIP_ACTION_INFO_DEF + 2014: GivePlayerItems(player, Lvl60BiS_DruidResto); player->CLOSE_GOSSIP_MENU(); break;

        // TBC Full Best in Slot
        case GOSSIP_ACTION_INFO_DEF + 3000:
        {
            switch (player->getClass())
            {
                case CLASS_ROGUE:
                    GivePlayerItems(player, Lvl70BiS_Rogue);
                    player->CLOSE_GOSSIP_MENU();
                    break;
                case CLASS_MAGE:
                    GivePlayerItems(player, Lvl70BiS_Mage);
                    player->CLOSE_GOSSIP_MENU();
                    break;
                case CLASS_HUNTER:
                    GivePlayerItems(player, Lvl70BiS_Hunter);
                    player->CLOSE_GOSSIP_MENU();
                    break;
                case CLASS_WARLOCK:
                    GivePlayerItems(player, Lvl70BiS_Warlock);
                    player->CLOSE_GOSSIP_MENU();
                    break;
                case CLASS_SHAMAN:
                    player->PrepareGossipMenu(creature, GOSSIP_TEXT_PICK_SPEC);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Restoration", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3001);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Enhancement", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3002);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Elemental", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3003);
                    player->SendPreparedGossip(creature);
                    break;
                case CLASS_PRIEST:
                    player->PrepareGossipMenu(creature, GOSSIP_TEXT_PICK_SPEC);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Shadow", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3004);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Discipline/Holy", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3005);
                    player->SendPreparedGossip(creature);
                    break;
                case CLASS_PALADIN:
                    player->PrepareGossipMenu(creature, GOSSIP_TEXT_PICK_SPEC);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Holy", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3006);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Retribution", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3007);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Protection", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3008);
                    player->SendPreparedGossip(creature);
                    break;
                case CLASS_WARRIOR:
                    player->PrepareGossipMenu(creature, GOSSIP_TEXT_PICK_SPEC);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Fury/Arms", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3009);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Protection", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3010);
                    player->SendPreparedGossip(creature);
                    break;
                case CLASS_DRUID:
                    player->PrepareGossipMenu(creature, GOSSIP_TEXT_PICK_SPEC);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Feral (Cat/DPS)", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3011);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Feral (Bear/Tank)", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3012);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Balance", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3013);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Restoration", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3014);
                    player->SendPreparedGossip(creature);
                    break;
            }
            break;
        }

        // TBC Full Best in Slot (Spec Selected)
        // Shaman - Restoration
        case GOSSIP_ACTION_INFO_DEF + 3001: GivePlayerItems(player, Lvl70BiS_ShamanResto); player->CLOSE_GOSSIP_MENU(); break;
        // Shaman - Enhancement
        case GOSSIP_ACTION_INFO_DEF + 3002: GivePlayerItems(player, Lvl70BiS_ShamanEnhancement); player->CLOSE_GOSSIP_MENU(); break;
        // Shaman - Elemental
        case GOSSIP_ACTION_INFO_DEF + 3003: GivePlayerItems(player, Lvl70BiS_ShamanElemental); player->CLOSE_GOSSIP_MENU(); break;
        // Priest - Shadow
        case GOSSIP_ACTION_INFO_DEF + 3004: GivePlayerItems(player, Lvl70BiS_PriestShadow); player->CLOSE_GOSSIP_MENU(); break;
        // Priest - Discipline/Holy
        case GOSSIP_ACTION_INFO_DEF + 3005: GivePlayerItems(player, Lvl70BiS_PriestDiscHoly); player->CLOSE_GOSSIP_MENU(); break;
        // Paladin - Holy
        case GOSSIP_ACTION_INFO_DEF + 3006: GivePlayerItems(player, Lvl70BiS_PaladinHoly); player->CLOSE_GOSSIP_MENU(); break;
        // Paladin - Retribution
        case GOSSIP_ACTION_INFO_DEF + 3007: GivePlayerItems(player, Lvl70BiS_PaladinRetribution); player->CLOSE_GOSSIP_MENU(); break;
        // Paladin - Protection
        case GOSSIP_ACTION_INFO_DEF + 3008: GivePlayerItems(player, Lvl70BiS_PaladinProtection); player->CLOSE_GOSSIP_MENU(); break;
        // Warrior - Fury/Arms
        case GOSSIP_ACTION_INFO_DEF + 3009: GivePlayerItems(player, Lvl70BiS_WarriorFuryArms); player->CLOSE_GOSSIP_MENU(); break;
        // Warrior - Protection
        case GOSSIP_ACTION_INFO_DEF + 3010: GivePlayerItems(player, Lvl70BiS_WarriorProtection); player->CLOSE_GOSSIP_MENU(); break;
        // Druid - Feral (Cat/DPS)
        case GOSSIP_ACTION_INFO_DEF + 3011: GivePlayerItems(player, Lvl70BiS_DruidFeralCat); player->CLOSE_GOSSIP_MENU(); break;
        // Druid - Feral (Bear/Tank)
        case GOSSIP_ACTION_INFO_DEF + 3012: GivePlayerItems(player, Lvl70BiS_DruidFeralBear); player->CLOSE_GOSSIP_MENU(); break;
        // Druid - Balance
        case GOSSIP_ACTION_INFO_DEF + 3013: GivePlayerItems(player, Lvl70BiS_DruidBalance); player->CLOSE_GOSSIP_MENU(); break;
        // Druid - Restoration
        case GOSSIP_ACTION_INFO_DEF + 3014: GivePlayerItems(player, Lvl70BiS_DruidResto); player->CLOSE_GOSSIP_MENU(); break;
    }

    // Secondary - Specialization 1
    if (action > 200 && action < 300)
    {
        AddStarterSet(player, creature, SET_ID_PRIMARY, action - 200);
        OfferPrimarySecondaryModChoices(player, 300, 0);
        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_PICK_GEAR_2, creature->GetObjectGuid());
    }
    // Tertiary - Specialization 1
    else if (action > 300 && action < 400)
    {
        AddStarterSet(player, creature, SET_ID_SECONDARY, action - 300);
        OfferTertiaryModChoices(player, 600, 0);
        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_PICK_GEAR_3, creature->GetObjectGuid());
    }
    // Tertiary - Specialization 1 (cont.)
    else if (action > 600 && action < 700)
    {
        AddStarterSet(player, creature, SET_ID_TERTIARY, action - 600, 0);
        player->CLOSE_GOSSIP_MENU();
    }

    // Secondary - Specialization 2
    else if (action > 700 && action < 800)
    {
        AddStarterSet(player, creature, SET_ID_PRIMARY, action - 700);
        OfferPrimarySecondaryModChoices(player, 800, 1);
        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_PICK_GEAR_2, creature->GetObjectGuid());
    }
    // Tertiary - Specialization 2
    else if (action > 800 && action < 900)
    {
        AddStarterSet(player, creature, SET_ID_SECONDARY, action - 800);
        OfferTertiaryModChoices(player, 900, 1);
        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_PICK_GEAR_3, creature->GetObjectGuid());
    }
    // Tertiary - Specialization 2 (cont.)
    else if (action > 900 && action < 1000)
    {
        AddStarterSet(player, creature, SET_ID_TERTIARY, action - 900, 1);
        player->CLOSE_GOSSIP_MENU();
    }
    //
    // skip 1000-1999 range because it's used above
    // 
    // Secondary - Specialization 3
    else if (action > 2000 && action < 2100)
    {
        AddStarterSet(player, creature, SET_ID_PRIMARY, action - 2000);
        OfferPrimarySecondaryModChoices(player, 2100, 2);
        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_PICK_GEAR_2, creature->GetObjectGuid());
    }
    // Tertiary - Specialization 3
    else if (action > 2100 && action < 2200)
    {
        AddStarterSet(player, creature, SET_ID_SECONDARY, action - 2100);
        OfferTertiaryModChoices(player, 2200, 2);
        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_PICK_GEAR_3, creature->GetObjectGuid());
    }
    // Tertiary - Specialization 3 (cont.)
    else if (action > 2200 && action < 2300)
    {
        AddStarterSet(player, creature, SET_ID_TERTIARY, action - 2200, 2);
        player->CLOSE_GOSSIP_MENU();
    }
    // Teleport - Starting Zones
    else if (action > 8000 && action < 8019)
    {
        player->CLOSE_GOSSIP_MENU();
        player->CastSpell(player, SPELL_TELEPORT_VISUAL, TRIGGERED_OLD_TRIGGERED);
        switch (action - 8000)
        {
            case RACE_HUMAN:
                player->TeleportTo(teleLocs[4].map, teleLocs[4].x, teleLocs[4].y, teleLocs[4].z, teleLocs[4].o);
                break;
            case RACE_DWARF:
            case RACE_GNOME:
                player->TeleportTo(teleLocs[5].map, teleLocs[5].x, teleLocs[5].y, teleLocs[5].z, teleLocs[5].o);
                break;
            case RACE_NIGHTELF:
                player->TeleportTo(teleLocs[6].map, teleLocs[6].x, teleLocs[6].y, teleLocs[6].z, teleLocs[6].o);
                break;
            case RACE_DRAENEI:
                player->TeleportTo(teleLocs[7].map, teleLocs[7].x, teleLocs[7].y, teleLocs[7].z, teleLocs[7].o);
                break;
            case RACE_TROLL:
            case RACE_ORC:
                player->TeleportTo(teleLocs[8].map, teleLocs[8].x, teleLocs[8].y, teleLocs[8].z, teleLocs[8].o);
                break;
            case RACE_UNDEAD:
                player->TeleportTo(teleLocs[9].map, teleLocs[9].x, teleLocs[9].y, teleLocs[9].z, teleLocs[9].o);
                break;
            case RACE_TAUREN:
                player->TeleportTo(teleLocs[10].map, teleLocs[10].x, teleLocs[10].y, teleLocs[10].z, teleLocs[10].o);
                break;
            case RACE_BLOODELF:
                player->TeleportTo(teleLocs[11].map, teleLocs[11].x, teleLocs[11].y, teleLocs[11].z, teleLocs[11].o);
                break;
        }
    }

    return false;
}

enum
{
    GOSSIP_MENU_MAIN            = 50200,
    GOSSIP_MENU_BOOST_CONFIRM   = 50402,
    GOSSIP_MENU_BOOST_DENIED    = 50401,
    GOSSIP_MENU_CONTACT         = 50205,
    GOSSIP_MENU_WHAT_IS_LV      = 50201,
    GOSSIP_MENU_DONATING        = 50202,
    GOSSIP_MENU_BUG_REPORT      = 50203,
    GOSSIP_MENU_CHEATING        = 50204,
};

bool GossipHello_npc_booster(Player* player, Creature* creature)
{
    player->PrepareGossipMenu(creature, GOSSIP_MENU_MAIN); // additional gossip_menu_options in DB
    uint32 boostLevel = sWorld.getConfig(CONFIG_UINT32_BOOST_LEVEL);
    std::stringstream boostOptionText;
    boostOptionText << "I want to jump straight into the action. |cFF006400[INSTANT BOOST - LEVEL ";
    boostOptionText << boostLevel;
    boostOptionText << "]|r";
    if (sGameEventMgr.IsActiveEvent(GAME_EVENT_LEVEL_BOOST))
    {
        if (player->GetLevel() < boostLevel)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, boostOptionText.str(), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    }
    player->SendPreparedGossip(creature);
    return true;
}

bool GossipSelect_npc_booster(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
{
    switch (action)
    {
        case 100:
            player->PrepareGossipMenu(creature, GOSSIP_MENU_CONTACT);
            player->SendPreparedGossip(creature);
            break;
        case 101:
            player->PrepareGossipMenu(creature, GOSSIP_MENU_WHAT_IS_LV);
            player->SendPreparedGossip(creature);
            break;
        case 102:
            player->PrepareGossipMenu(creature, GOSSIP_MENU_DONATING);
            player->SendPreparedGossip(creature);
            break;
        case 103:
            player->PrepareGossipMenu(creature, GOSSIP_MENU_BUG_REPORT);
            player->SendPreparedGossip(creature);
            break;
        case 104:
            player->PrepareGossipMenu(creature, GOSSIP_MENU_CHEATING);
            player->SendPreparedGossip(creature);
            break;
        case GOSSIP_ACTION_INFO_DEF + 1: // Confirmation
        {
            // check additional restrictions before allowing final option for boost
            if (CanBoost(player))
            {
                uint32 boostLevel = sWorld.getConfig(CONFIG_UINT32_BOOST_LEVEL);
                std::stringstream boostOptionText;
                std::stringstream boostPopupText;
                boostOptionText << "I'm ready. Take me to the front! |cFF006400[INSTANT BOOST - LEVEL ";
                boostPopupText << "Boost character to level ";
                boostOptionText << boostLevel;
                boostPopupText << boostLevel;
                boostOptionText << "]|r";
                boostPopupText << "?";
                player->PrepareGossipMenu(creature, GOSSIP_MENU_BOOST_CONFIRM);
                player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, boostOptionText.str(), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2, boostPopupText.str(), 0, false);
            }
            else
                player->PrepareGossipMenu(creature, GOSSIP_MENU_BOOST_DENIED);

            player->SendPreparedGossip(creature);

            break;
        }
        case GOSSIP_ACTION_INFO_DEF + 2: // Boost
        {
            uint32 boostLevel = sWorld.getConfig(CONFIG_UINT32_BOOST_LEVEL);
            player->CLOSE_GOSSIP_MENU();
            BoostPlayer(player, boostLevel);
            player->CastSpell(player, SPELL_TELEPORT_VISUAL, TRIGGERED_OLD_TRIGGERED);
            // Teleport Player to Dark Portal
            if (player->GetTeam() == ALLIANCE)
            {
                player->TeleportTo(teleLocs[0].map, teleLocs[0].x, teleLocs[0].y, teleLocs[0].z, teleLocs[0].o);
                player->SetHomebindToLocation(WorldLocation(teleLocs[0].map, teleLocs[0].x, teleLocs[0].y, teleLocs[0].z, teleLocs[0].o), teleLocs[0].area);
            }
            else
            {
                player->TeleportTo(teleLocs[1].map, teleLocs[1].x, teleLocs[1].y, teleLocs[1].z, teleLocs[1].o);
                player->SetHomebindToLocation(WorldLocation(teleLocs[1].map, teleLocs[1].x, teleLocs[1].y, teleLocs[1].z, teleLocs[1].o), teleLocs[1].area);
            }
            break;
        }
    }
    return false;
}

enum
{
    SPELL_CYCLONE_VISUAL_SPAWN      = 8609,

    GOSSIP_TEXT_ENLIST_GREET        = 50403,
    GOSSIP_TEXT_ENLIST_REFUSE_LOW   = 50404,
    GOSSIP_TEXT_ENLIST_REFUSE_HIGH  = 50405,
    GOSSIP_TEXT_ENLIST_PET_MENU     = 50406,

    NPC_PET_BAT     = 8600, // Plaguebat
    NPC_PET_BEAR    = 7443, // Shardtooth Mauler
    NPC_PET_BOAR    = 5992, // Ashmane Boar
    NPC_PET_CARRION = 1809, // Carrion Vulture
    NPC_PET_CAT     = 1713, // Elder Shadowmaw Panther
    NPC_PET_CRAB    = 1088, // Monstrous Crawler
    NPC_PET_CROC    = 1087, // Sawtooth Snapper
    NPC_PET_GORILLA = 6516, // Un'Goro Thunderer
    NPC_PET_HYENA   = 5986, // Rabid Snickerfang
    NPC_PET_OWL     = 7455, // Winterspring Owl
    NPC_PET_RAPTOR  = 687,  // Jungle Stalker
    NPC_PET_STRIDER = 4725, // Crazed Sandstrider
    NPC_PET_SCORPID = 9695, // Deathlash Scorpid
    NPC_PET_SPIDER  = 8933, // Cave Creeper
    NPC_PET_SERPENT = 5308, // Rogue Vale Screecher
    NPC_PET_TURTLE  = 6369, // Coralshell Tortoise
    NPC_PET_WOLF    = 9697, // Giant Ember Worg
    NPC_PET_RAVAGER = 16932 // Razorfang Hatchling
};

std::vector<uint32> GetFullGearListInstant58()
{
    std::vector<uint32> gearList;
    gearList.clear();
    gearList.insert(std::end(gearList), std::begin(Instant58Shared), std::end(Instant58Shared));
    gearList.insert(std::end(gearList), std::begin(Instant58PhysicalBack), std::end(Instant58PhysicalBack));
    gearList.insert(std::end(gearList), std::begin(Instant58MagicBack), std::end(Instant58MagicBack));
    gearList.insert(std::end(gearList), std::begin(Instant58Bow), std::end(Instant58Bow));
    gearList.insert(std::end(gearList), std::begin(Instant58LeatherOffpieces), std::end(Instant58LeatherOffpieces));
    gearList.insert(std::end(gearList), std::begin(Instant58Staff), std::end(Instant58Staff));
    gearList.insert(std::end(gearList), std::begin(Instant58ClothCaster), std::end(Instant58ClothCaster));
    gearList.insert(std::end(gearList), std::begin(Instant58MailOffpieces), std::end(Instant58MailOffpieces));
    gearList.insert(std::end(gearList), std::begin(Instant58GavelAndShield), std::end(Instant58GavelAndShield));
    gearList.insert(std::end(gearList), std::begin(Instant58DualWield), std::end(Instant58DualWield));
    gearList.insert(std::end(gearList), std::begin(Instant58PlateOffpieces), std::end(Instant58PlateOffpieces));
    gearList.insert(std::end(gearList), std::begin(Instant58Hunter), std::end(Instant58Hunter));
    gearList.insert(std::end(gearList), std::begin(Instant58Rogue), std::end(Instant58Rogue));
    gearList.insert(std::end(gearList), std::begin(Instant58Druid), std::end(Instant58Druid));
    gearList.insert(std::end(gearList), std::begin(Instant58Mage), std::end(Instant58Mage));
    gearList.insert(std::end(gearList), std::begin(Instant58Paladin), std::end(Instant58Paladin));
    gearList.insert(std::end(gearList), std::begin(Instant58Priest), std::end(Instant58Priest));
    gearList.insert(std::end(gearList), std::begin(Instant58Shaman), std::end(Instant58Shaman));
    gearList.insert(std::end(gearList), std::begin(Instant58Warlock), std::end(Instant58Warlock));
    gearList.insert(std::end(gearList), std::begin(Instant58Warrior), std::end(Instant58Warrior));
    return gearList;
}

bool GossipHello_npc_enlistment_officer(Player* player, Creature* creature)
{
    if (player->GetLevel() < 58)
    {
        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_ENLIST_REFUSE_LOW, creature->GetObjectGuid());
        return true;
    }

    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Train talent spell ranks", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);

    if (HasStarterSet(player, GetFullGearListInstant58()))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "|cFF8B0000Remove all level 58 starter gear.|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
    else
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "I need my starter gear set!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);

    if (GetStarterMountForRace(player))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "I seem to be missing my mount. Can you give me one?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);

    if (!TaxiNodesKnown(*player))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Learn Azeroth flight paths", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);

    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Basic Supplies", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);

    if (player->GetLevel() <= 60) // only allow these services for levels 58-60
    {
        player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, "Unlearn Talents", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8, "Are you sure?", 0, false);

        if (player->getClass() == CLASS_HUNTER)
        {
            if (!player->GetPet())
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Pet Menu", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            else if (player->GetPet()->m_spells.size() >= 1)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Untrain Pet", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
        }
    }
    
    player->SEND_GOSSIP_MENU(player->GetLevel() > 58 ? GOSSIP_TEXT_ENLIST_REFUSE_HIGH : GOSSIP_TEXT_ENLIST_GREET, creature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_enlistment_officer(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 action)
{
    player->GetPlayerMenu()->ClearMenus();
    Pet* pPet = player->GetPet();

    switch (action)
    {
        // Main Menu
        case GOSSIP_ACTION_INFO_DEF:
            GossipHello_npc_enlistment_officer(player, creature);
            break;

        // Pets - Page 1
        case GOSSIP_ACTION_INFO_DEF + 1:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "<- Back to Main Menu", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Next Page ->", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Bat", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 501);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Bear", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 502);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Boar", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 503);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Carrion Bird", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 504);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Cat", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 505);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Crab", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 506);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Crocolisk", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 507);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Gorilla", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 508);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Hyena", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 509);
            player->SEND_GOSSIP_MENU(GOSSIP_TEXT_ENLIST_PET_MENU, creature->GetObjectGuid());
            break;
        // Pets - Page 2
        case GOSSIP_ACTION_INFO_DEF + 2:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "<- Back to Main Menu", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "<- Previous Page", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Owl", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 510);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Raptor", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 511);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Ravager", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 512);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Strider", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 513);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Scorpid", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 514);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Serpent", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 515);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Spider", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 516);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Turtle", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 517);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Wolf", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 518);
            player->SEND_GOSSIP_MENU(GOSSIP_TEXT_ENLIST_PET_MENU, creature->GetObjectGuid());
            break;

        // Remove Starter Set - Instant 58
        case GOSSIP_ACTION_INFO_DEF + 3:
            RemoveStarterSet(player, GetFullGearListInstant58());
            player->CLOSE_GOSSIP_MENU();
            break;
            
        // Add Starter Mount
        case GOSSIP_ACTION_INFO_DEF + 4:
        {
            player->StoreNewItemInBestSlots(GetStarterMountForRace(player), 1);
            player->CLOSE_GOSSIP_MENU();
            break;
        }

        // Learn Flight Paths
        case GOSSIP_ACTION_INFO_DEF + 5:
            TaxiNodesTeach(*player);
            player->SetFacingToObject(creature);
            player->HandleEmote(EMOTE_ONESHOT_SALUTE);
            player->CastSpell(player, SPELL_CYCLONE_VISUAL_SPAWN, TRIGGERED_OLD_TRIGGERED);
            player->CLOSE_GOSSIP_MENU();
            break;
        
        // Give Instant 58 Starter Set
        case GOSSIP_ACTION_INFO_DEF + 6:
            AddStarterSet(player, creature, SET_ID_INSTANT_58);
            player->CLOSE_GOSSIP_MENU();
            break;

        // Untrain Pet
        case GOSSIP_ACTION_INFO_DEF + 7:
            player->CLOSE_GOSSIP_MENU();
            if (pPet && pPet->m_spells.size() >= 1)
            {
                CharmInfo* charmInfo = pPet->GetCharmInfo();
                if (charmInfo)
                {
                    for (PetSpellMap::iterator itr = pPet->m_spells.begin(); itr != pPet->m_spells.end();)
                    {
                        uint32 spell_id = itr->first;
                        ++itr;
                        pPet->unlearnSpell(spell_id, false);
                    }

                    uint32 cost = pPet->resetTalentsCost();

                    pPet->SetTP(pPet->GetLevel() * (pPet->GetLoyaltyLevel() - 1));

                    for (int i = 0; i < MAX_UNIT_ACTION_BAR_INDEX; ++i)
                        if (UnitActionBarEntry const* ab = charmInfo->GetActionBarEntry(i))
                            if (ab->GetAction() && ab->IsActionBarForSpell())
                                charmInfo->SetActionBar(i, 0, ACT_DISABLED);

                    // relearn pet passives
                    pPet->LearnPetPassives();

                    pPet->m_resetTalentsTime = time(nullptr);
                    pPet->m_resetTalentsCost = cost;

                    player->PetSpellInitialize();
                }
            }
            break;

        // Unlearn Talents
        case GOSSIP_ACTION_INFO_DEF + 8:
            player->CLOSE_GOSSIP_MENU();
            player->resetTalents(true);
            break;

        case GOSSIP_ACTION_INFO_DEF + 9: player->GetSession()->SendListInventory(creature->GetObjectGuid()); break;
        case GOSSIP_ACTION_INFO_DEF + 10: player->GetSession()->SendTrainerList(creature->GetObjectGuid()); break;

        case GOSSIP_ACTION_INFO_DEF + 501: CreatePet(player, creature, NPC_PET_BAT); break;
        case GOSSIP_ACTION_INFO_DEF + 502: CreatePet(player, creature, NPC_PET_BEAR); break;
        case GOSSIP_ACTION_INFO_DEF + 503: CreatePet(player, creature, NPC_PET_BOAR); break;
        case GOSSIP_ACTION_INFO_DEF + 504: CreatePet(player, creature, NPC_PET_CARRION); break;
        case GOSSIP_ACTION_INFO_DEF + 505: CreatePet(player, creature, NPC_PET_CAT); break;
        case GOSSIP_ACTION_INFO_DEF + 506: CreatePet(player, creature, NPC_PET_CRAB); break;
        case GOSSIP_ACTION_INFO_DEF + 507: CreatePet(player, creature, NPC_PET_CROC); break;
        case GOSSIP_ACTION_INFO_DEF + 508: CreatePet(player, creature, NPC_PET_GORILLA); break;
        case GOSSIP_ACTION_INFO_DEF + 509: CreatePet(player, creature, NPC_PET_HYENA); break;
        case GOSSIP_ACTION_INFO_DEF + 510: CreatePet(player, creature, NPC_PET_OWL); break;
        case GOSSIP_ACTION_INFO_DEF + 511: CreatePet(player, creature, NPC_PET_RAPTOR); break;
        case GOSSIP_ACTION_INFO_DEF + 512: CreatePet(player, creature, NPC_PET_RAVAGER); break;
        case GOSSIP_ACTION_INFO_DEF + 513: CreatePet(player, creature, NPC_PET_STRIDER); break;
        case GOSSIP_ACTION_INFO_DEF + 514: CreatePet(player, creature, NPC_PET_SCORPID); break;
        case GOSSIP_ACTION_INFO_DEF + 515: CreatePet(player, creature, NPC_PET_SERPENT); break;
        case GOSSIP_ACTION_INFO_DEF + 516: CreatePet(player, creature, NPC_PET_SPIDER); break;
        case GOSSIP_ACTION_INFO_DEF + 517: CreatePet(player, creature, NPC_PET_TURTLE); break;
        case GOSSIP_ACTION_INFO_DEF + 518: CreatePet(player, creature, NPC_PET_WOLF); break;
    }
    return true;
}

const uint32 numDisplayIds = 17001;
const uint32 AllCreatureDisplayIds[numDisplayIds] = {4,13,14,15,16,17,18,19,20,21,22,23,26,30,31,33,38,39,40,42,49,50,51,52,53,54,55,56,57,58,59,60,62,64,65,68,69,71,73,74,79,80,86,87,88,89,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,112,113,115,123,125,126,127,128,129,130,134,137,138,139,140,141,143,145,146,148,149,150,151,152,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,184,186,188,190,191,193,194,195,196,197,198,200,201,202,203,204,207,208,210,211,212,213,214,215,218,221,224,225,226,227,228,229,231,232,235,236,237,238,239,240,242,243,244,245,246,247,251,252,257,258,262,263,267,268,269,275,277,278,280,282,283,285,286,295,298,299,300,301,302,304,308,309,310,312,314,316,317,320,321,322,326,327,328,330,332,336,337,338,341,342,344,346,347,348,351,352,355,358,360,363,365,366,367,368,369,370,371,372,373,374,375,376,377,378,379,380,381,382,383,384,385,386,387,388,389,390,391,397,399,400,410,413,414,415,416,418,423,424,425,427,428,429,430,433,441,447,448,453,456,457,459,467,470,471,473,478,479,480,481,482,483,486,487,488,489,490,491,492,493,494,495,496,497,498,499,500,501,502,503,504,505,506,507,508,511,512,513,514,515,516,517,518,519,520,522,523,524,525,527,528,529,531,533,534,535,536,538,539,540,543,544,545,546,547,548,549,550,551,553,555,556,557,558,559,560,561,563,564,565,566,567,568,569,570,571,574,575,576,578,580,584,585,586,588,589,590,591,592,593,597,598,599,601,603,604,605,606,607,609,610,611,612,613,614,615,616,617,621,622,623,624,625,627,628,631,632,633,634,636,637,638,641,643,644,646,647,648,649,651,652,654,655,656,657,658,659,660,661,662,664,665,666,667,668,670,671,672,673,674,675,676,677,679,680,681,682,691,692,693,694,695,696,697,698,699,703,704,705,706,707,709,711,713,714,715,716,719,720,721,722,723,726,729,730,732,733,734,735,736,741,742,743,744,748,749,751,752,753,754,755,757,759,760,762,763,764,765,767,768,774,775,776,780,781,782,783,784,785,787,788,790,791,792,793,794,795,796,797,798,800,801,802,803,806,807,809,810,814,815,816,820,821,822,824,825,827,828,829,830,831,832,833,834,835,836,837,838,839,840,841,842,843,844,845,846,847,848,850,852,853,855,856,857,858,859,860,862,863,864,865,867,868,869,870,876,883,892,897,900,901,902,903,904,907,908,909,910,911,913,915,917,918,924,925,927,929,931,933,934,935,936,937,945,947,948,949,950,953,954,955,956,957,958,959,960,961,963,964,965,967,968,969,970,971,972,974,975,976,979,981,982,983,984,985,986,987,990,991,992,993,994,995,996,997,998,999,1000,1001,1004,1005,1006,1007,1008,1010,1011,1012,1013,1014,1015,1016,1017,1018,1019,1021,1023,1025,1027,1029,1030,1031,1032,1034,1035,1036,1037,1038,1039,1040,1041,1042,1043,1045,1048,1049,1050,1051,1052,1054,1055,1056,1057,1058,1059,1060,1061,1062,1063,1064,1065,1067,1068,1070,1072,1073,1074,1075,1076,1077,1078,1079,1080,1081,1082,1083,1084,1086,1087,1088,1089,1090,1091,1092,1094,1095,1097,1098,1099,1100,1101,1102,1103,1104,1105,1106,1107,1108,1109,1110,1114,1115,1120,1122,1125,1126,1127,1128,1129,1130,1131,1132,1133,1134,1135,1136,1137,1138,1139,1140,1141,1142,1143,1144,1145,1146,1147,1148,1149,1150,1151,1152,1153,1154,1155,1156,1157,1159,1160,1161,1162,1163,1164,1165,1166,1167,1170,1176,1185,1192,1193,1194,1196,1197,1198,1200,1201,1202,1204,1206,1207,1208,1210,1213,1214,1216,1217,1218,1219,1220,1221,1222,1223,1225,1226,1228,1229,1230,1231,1232,1233,1236,1237,1241,1244,1245,1246,1247,1248,1250,1251,1252,1253,1254,1255,1256,1258,1259,1260,1261,1262,1267,1269,1274,1275,1276,1277,1278,1279,1280,1281,1282,1283,1284,1285,1286,1287,1288,1289,1290,1291,1292,1293,1294,1295,1296,1297,1298,1299,1300,1302,1303,1304,1305,1306,1307,1308,1309,1310,1311,1312,1313,1314,1315,1316,1317,1318,1319,1320,1321,1322,1323,1324,1325,1326,1327,1328,1329,1330,1331,1332,1333,1334,1335,1336,1337,1338,1339,1340,1341,1342,1343,1344,1345,1346,1348,1349,1350,1351,1352,1354,1355,1356,1357,1358,1359,1360,1361,1362,1363,1364,1366,1367,1368,1369,1370,1371,1372,1373,1374,1375,1376,1377,1378,1379,1380,1381,1382,1383,1384,1385,1386,1387,1388,1389,1390,1391,1392,1393,1394,1395,1396,1397,1398,1399,1400,1401,1402,1403,1404,1405,1406,1407,1408,1409,1410,1411,1412,1413,1415,1416,1417,1418,1420,1421,1423,1424,1425,1426,1427,1428,1429,1430,1431,1432,1433,1434,1435,1436,1437,1438,1439,1440,1441,1442,1443,1444,1445,1446,1447,1448,1449,1450,1451,1452,1453,1454,1455,1460,1461,1466,1467,1468,1469,1470,1471,1472,1473,1474,1477,1478,1479,1480,1481,1482,1483,1484,1485,1486,1487,1488,1489,1490,1491,1492,1493,1494,1495,1496,1497,1498,1499,1500,1501,1502,1503,1504,1505,1506,1507,1508,1509,1510,1511,1512,1513,1514,1515,1516,1517,1518,1519,1520,1521,1522,1523,1524,1525,1526,1527,1528,1529,1531,1532,1533,1534,1535,1536,1537,1538,1539,1540,1541,1542,1543,1544,1545,1546,1547,1548,1549,1550,1551,1552,1555,1556,1557,1558,1559,1560,1561,1562,1563,1564,1565,1566,1567,1568,1569,1570,1571,1572,1573,1574,1575,1576,1577,1578,1579,1580,1581,1582,1583,1584,1585,1586,1587,1588,1589,1590,1591,1592,1593,1594,1595,1596,1597,1598,1599,1600,1601,1602,1603,1604,1605,1606,1607,1608,1609,1610,1611,1612,1613,1614,1615,1616,1617,1618,1619,1620,1621,1622,1623,1624,1625,1626,1627,1628,1629,1630,1631,1632,1633,1634,1635,1636,1637,1638,1639,1640,1641,1643,1644,1645,1646,1647,1648,1649,1650,1651,1652,1653,1654,1655,1656,1657,1658,1659,1660,1661,1662,1663,1664,1665,1666,1667,1668,1669,1670,1671,1672,1673,1674,1675,1676,1677,1678,1679,1680,1681,1682,1683,1684,1685,1686,1687,1688,1689,1690,1691,1692,1693,1694,1695,1696,1697,1698,1699,1700,1701,1702,1703,1704,1705,1706,1707,1708,1709,1710,1711,1712,1713,1714,1715,1716,1717,1718,1719,1720,1721,1722,1723,1724,1725,1726,1727,1728,1729,1730,1731,1732,1733,1734,1735,1736,1737,1738,1739,1740,1741,1742,1743,1744,1745,1746,1747,1748,1749,1750,1751,1752,1753,1754,1755,1756,1757,1758,1759,1760,1761,1762,1763,1764,1765,1766,1767,1768,1769,1770,1771,1772,1773,1774,1775,1776,1777,1778,1779,1780,1781,1782,1783,1784,1785,1786,1787,1788,1789,1790,1791,1792,1793,1794,1795,1796,1797,1798,1799,1800,1801,1802,1803,1804,1805,1806,1807,1808,1809,1810,1811,1812,1813,1814,1815,1816,1817,1818,1819,1820,1821,1822,1823,1824,1825,1826,1827,1828,1829,1830,1831,1832,1833,1834,1835,1836,1837,1838,1839,1840,1841,1842,1843,1844,1845,1846,1847,1848,1849,1850,1851,1852,1853,1854,1855,1856,1857,1858,1859,1860,1861,1862,1863,1864,1865,1866,1867,1868,1869,1870,1871,1872,1873,1874,1875,1876,1877,1878,1879,1880,1881,1882,1883,1884,1885,1886,1887,1888,1889,1890,1891,1892,1893,1894,1895,1896,1897,1898,1899,1900,1901,1902,1903,1904,1905,1906,1907,1908,1909,1910,1911,1912,1913,1914,1915,1916,1917,1918,1921,1923,1924,1925,1926,1927,1928,1929,1930,1931,1932,1933,1934,1935,1936,1937,1938,1939,1940,1941,1942,1943,1944,1945,1947,1948,1950,1951,1952,1954,1955,1956,1957,1958,1959,1960,1961,1962,1963,1964,1965,1967,1968,1969,1970,1971,1972,1973,1974,1975,1976,1977,1978,1979,1980,1981,1982,1983,1984,1985,1986,1987,1988,1989,1990,1991,1992,1993,1994,1995,1996,1997,1998,1999,2000,2001,2002,2003,2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015,2016,2017,2018,2019,2020,2021,2022,2023,2024,2025,2026,2027,2028,2029,2030,2031,2032,2033,2034,2035,2036,2037,2038,2039,2040,2041,2042,2043,2044,2045,2046,2047,2048,2049,2050,2051,2052,2053,2054,2055,2056,2057,2058,2059,2060,2061,2062,2063,2064,2065,2066,2067,2068,2069,2070,2071,2072,2073,2074,2075,2076,2077,2078,2079,2080,2082,2083,2084,2085,2086,2087,2088,2089,2090,2091,2092,2093,2094,2095,2096,2097,2098,2099,2100,2101,2102,2103,2104,2105,2106,2107,2108,2109,2110,2111,2112,2113,2114,2115,2116,2117,2118,2119,2120,2121,2122,2123,2124,2125,2126,2127,2128,2129,2130,2131,2132,2133,2134,2135,2136,2137,2138,2139,2140,2141,2142,2143,2144,2145,2146,2147,2148,2149,2150,2151,2152,2153,2154,2155,2156,2157,2158,2160,2161,2162,2163,2164,2165,2166,2167,2168,2169,2170,2172,2173,2174,2175,2176,2177,2178,2179,2180,2181,2182,2183,2184,2185,2186,2187,2188,2189,2190,2191,2192,2193,2194,2195,2196,2197,2198,2199,2200,2201,2202,2203,2204,2205,2206,2207,2208,2209,2210,2211,2212,2213,2214,2215,2216,2217,2218,2219,2220,2221,2222,2223,2224,2225,2226,2227,2228,2229,2230,2231,2232,2233,2234,2236,2237,2238,2239,2240,2241,2242,2243,2244,2245,2246,2247,2248,2249,2250,2251,2252,2253,2254,2255,2256,2257,2258,2259,2260,2261,2262,2263,2264,2265,2266,2267,2268,2269,2270,2271,2272,2273,2274,2275,2276,2277,2278,2279,2280,2281,2283,2284,2285,2286,2287,2288,2289,2290,2292,2293,2294,2295,2296,2297,2298,2299,2300,2301,2302,2303,2305,2306,2307,2308,2309,2311,2312,2313,2314,2315,2316,2317,2318,2319,2320,2321,2322,2323,2324,2325,2326,2327,2328,2329,2330,2331,2332,2333,2335,2336,2337,2338,2339,2340,2341,2342,2343,2344,2345,2346,2347,2348,2349,2350,2352,2353,2354,2355,2357,2358,2359,2360,2361,2362,2363,2364,2365,2366,2367,2368,2369,2370,2371,2372,2373,2374,2375,2377,2378,2379,2380,2381,2382,2383,2384,2386,2387,2388,2389,2390,2391,2392,2393,2394,2395,2396,2397,2398,2399,2400,2401,2402,2403,2404,2405,2407,2408,2409,2410,2411,2412,2413,2414,2415,2416,2417,2418,2419,2420,2421,2422,2423,2424,2425,2426,2427,2428,2429,2432,2433,2434,2435,2436,2437,2438,2439,2440,2441,2442,2443,2444,2445,2446,2447,2448,2449,2450,2451,2452,2453,2454,2455,2456,2457,2458,2459,2460,2461,2462,2463,2464,2465,2466,2467,2468,2469,2470,2471,2472,2473,2474,2475,2476,2477,2478,2479,2480,2481,2482,2483,2484,2485,2486,2487,2488,2489,2490,2491,2492,2493,2494,2495,2496,2497,2498,2499,2500,2501,2502,2503,2504,2505,2506,2507,2508,2509,2510,2511,2512,2513,2514,2515,2516,2517,2518,2519,2520,2521,2522,2523,2524,2525,2526,2528,2529,2530,2531,2532,2533,2534,2535,2536,2537,2538,2539,2540,2541,2542,2543,2544,2545,2546,2548,2549,2550,2552,2554,2557,2558,2561,2562,2563,2564,2565,2567,2568,2569,2570,2571,2572,2573,2574,2575,2576,2577,2578,2579,2581,2582,2583,2584,2585,2586,2587,2588,2589,2590,2591,2592,2593,2594,2595,2596,2597,2598,2599,2600,2601,2602,2603,2604,2605,2606,2607,2608,2609,2610,2611,2612,2613,2614,2615,2616,2617,2618,2619,2620,2621,2622,2623,2624,2625,2626,2627,2628,2629,2630,2631,2632,2633,2634,2635,2636,2637,2638,2639,2640,2641,2642,2643,2644,2645,2646,2647,2648,2649,2650,2651,2652,2653,2654,2655,2656,2657,2658,2659,2660,2661,2662,2663,2664,2665,2666,2667,2668,2669,2670,2671,2672,2673,2674,2675,2676,2677,2678,2679,2680,2681,2682,2683,2684,2685,2686,2687,2688,2689,2690,2691,2692,2693,2694,2695,2696,2697,2698,2699,2700,2702,2703,2704,2705,2706,2707,2708,2709,2710,2711,2712,2713,2714,2715,2716,2717,2718,2719,2720,2721,2722,2723,2725,2726,2727,2728,2729,2730,2731,2732,2733,2734,2735,2736,2737,2738,2739,2740,2741,2742,2743,2764,2765,2766,2767,2768,2784,2785,2786,2787,2788,2789,2790,2810,2811,2830,2831,2832,2833,2834,2835,2836,2837,2838,2850,2851,2852,2853,2854,2855,2856,2857,2858,2859,2860,2861,2862,2863,2864,2865,2866,2867,2869,2870,2871,2872,2873,2874,2875,2876,2877,2878,2879,2880,2882,2883,2884,2885,2886,2887,2888,2889,2890,2891,2892,2893,2894,2895,2896,2897,2898,2899,2900,2901,2902,2903,2910,2930,2950,2951,2952,2953,2954,2955,2956,2957,2958,2959,2960,2961,2962,2963,2964,2965,2966,2967,2968,2969,2970,2971,2972,2973,2974,2975,2976,2977,2978,2979,2980,2981,2982,2983,2984,2985,2986,2987,2988,2989,2990,2991,2992,2993,2994,2995,2996,2997,2998,2999,3000,3001,3002,3003,3004,3005,3006,3007,3010,3011,3012,3013,3014,3015,3016,3017,3018,3019,3020,3021,3022,3023,3024,3026,3027,3028,3029,3030,3031,3032,3033,3034,3035,3036,3037,3038,3039,3040,3041,3042,3043,3044,3045,3046,3047,3048,3049,3050,3051,3052,3053,3054,3055,3056,3057,3058,3059,3060,3061,3062,3063,3064,3065,3066,3067,3068,3069,3070,3071,3072,3073,3074,3075,3076,3077,3078,3079,3080,3081,3082,3083,3084,3085,3086,3087,3088,3089,3090,3091,3092,3093,3094,3095,3096,3097,3098,3099,3100,3101,3102,3103,3104,3105,3106,3107,3108,3109,3110,3111,3112,3113,3114,3115,3116,3117,3118,3119,3120,3121,3122,3123,3124,3125,3126,3127,3128,3129,3130,3131,3132,3133,3134,3135,3136,3137,3138,3139,3140,3141,3142,3143,3144,3145,3146,3166,3167,3186,3187,3188,3189,3190,3192,3193,3194,3195,3196,3197,3198,3199,3200,3201,3202,3203,3204,3205,3206,3207,3208,3209,3210,3211,3212,3213,3214,3215,3216,3217,3218,3219,3220,3222,3223,3224,3225,3226,3227,3228,3229,3230,3231,3232,3233,3234,3235,3236,3237,3238,3246,3247,3248,3249,3250,3251,3252,3253,3254,3255,3256,3257,3258,3259,3260,3261,3262,3263,3264,3265,3266,3267,3268,3269,3270,3271,3272,3273,3274,3275,3276,3277,3278,3279,3280,3281,3282,3283,3284,3285,3286,3287,3288,3289,3290,3291,3292,3293,3295,3296,3297,3298,3299,3300,3301,3302,3303,3304,3305,3306,3307,3308,3309,3310,3311,3312,3313,3314,3315,3316,3317,3318,3319,3320,3321,3322,3323,3324,3325,3326,3327,3328,3329,3330,3331,3332,3333,3334,3335,3336,3337,3338,3339,3340,3341,3342,3343,3344,3345,3346,3347,3348,3349,3350,3351,3352,3353,3354,3355,3356,3357,3358,3359,3360,3361,3362,3363,3364,3365,3366,3367,3368,3369,3370,3371,3372,3373,3374,3375,3376,3377,3378,3379,3380,3381,3382,3383,3384,3386,3387,3388,3389,3390,3391,3392,3393,3394,3395,3396,3397,3398,3399,3400,3401,3403,3405,3406,3407,3408,3409,3410,3411,3412,3413,3414,3415,3416,3417,3418,3419,3420,3421,3422,3423,3424,3425,3426,3427,3428,3429,3430,3431,3432,3433,3434,3435,3436,3437,3438,3439,3440,3441,3442,3443,3444,3445,3446,3447,3448,3449,3450,3451,3452,3453,3454,3455,3456,3457,3458,3459,3460,3461,3462,3463,3464,3465,3466,3467,3468,3469,3470,3471,3472,3473,3474,3475,3476,3477,3478,3479,3480,3481,3482,3483,3484,3485,3486,3487,3488,3489,3490,3491,3492,3493,3494,3505,3506,3507,3508,3509,3510,3511,3512,3513,3514,3515,3516,3517,3518,3519,3520,3521,3522,3523,3524,3525,3526,3527,3528,3529,3530,3531,3532,3533,3534,3535,3536,3537,3538,3539,3540,3541,3542,3543,3544,3545,3546,3547,3548,3549,3550,3551,3552,3553,3554,3555,3556,3557,3558,3559,3560,3561,3562,3563,3564,3565,3566,3567,3568,3569,3570,3571,3572,3573,3574,3575,3576,3577,3578,3579,3580,3581,3582,3583,3584,3585,3586,3587,3588,3589,3590,3591,3592,3593,3594,3595,3596,3597,3598,3599,3600,3601,3602,3603,3604,3605,3606,3607,3608,3609,3610,3611,3612,3613,3614,3615,3616,3617,3618,3619,3620,3621,3622,3623,3624,3625,3626,3627,3628,3629,3630,3631,3632,3633,3634,3635,3636,3637,3638,3639,3640,3641,3642,3643,3644,3645,3646,3647,3648,3649,3650,3651,3652,3653,3654,3655,3656,3657,3658,3659,3660,3661,3662,3663,3664,3665,3666,3667,3668,3669,3670,3671,3672,3673,3674,3675,3676,3677,3678,3679,3680,3681,3682,3683,3684,3685,3686,3687,3688,3689,3690,3691,3692,3693,3694,3695,3696,3697,3698,3699,3701,3702,3703,3704,3705,3706,3707,3708,3709,3710,3711,3712,3713,3714,3715,3716,3717,3718,3719,3720,3721,3722,3723,3724,3725,3726,3727,3728,3729,3730,3731,3732,3733,3734,3735,3736,3737,3738,3739,3740,3741,3742,3743,3744,3745,3746,3747,3748,3749,3750,3751,3752,3753,3754,3755,3756,3757,3758,3759,3760,3761,3762,3763,3764,3765,3766,3767,3768,3769,3770,3771,3772,3773,3774,3775,3776,3777,3778,3779,3780,3781,3782,3783,3784,3785,3786,3787,3788,3789,3790,3791,3792,3793,3794,3795,3796,3797,3798,3799,3800,3801,3802,3803,3804,3806,3807,3808,3809,3810,3811,3812,3813,3814,3815,3816,3817,3818,3819,3820,3821,3822,3823,3824,3826,3827,3828,3829,3830,3832,3833,3834,3835,3836,3837,3838,3839,3840,3841,3842,3843,3844,3845,3846,3847,3848,3849,3850,3851,3852,3853,3854,3855,3856,3857,3858,3859,3860,3861,3862,3863,3864,3865,3866,3867,3868,3869,3870,3871,3872,3873,3874,3875,3876,3877,3878,3879,3880,3881,3882,3883,3884,3885,3886,3887,3888,3889,3890,3892,3893,3894,3895,3896,3897,3898,3899,3900,3901,3902,3903,3904,3905,3906,3907,3908,3909,3910,3911,3912,3913,3914,3915,3916,3917,3918,3919,3920,3921,3922,3923,3924,3925,3926,3927,3928,3929,3930,3931,3932,3933,3934,3935,3936,3937,3938,3939,3940,3941,3942,3943,3944,3945,3946,3947,3948,3949,3950,3951,3952,3953,3954,3956,3957,3958,3959,3960,3961,3962,3963,3964,3965,3966,3967,3968,3969,3970,3971,3972,3973,3974,3975,3976,3977,3978,3979,3980,3981,3982,3983,3984,3985,3986,3987,3988,3989,3990,3991,3992,3993,3994,3995,3996,3997,3998,3999,4000,4001,4002,4003,4004,4005,4006,4007,4008,4009,4010,4011,4012,4013,4014,4015,4016,4017,4018,4019,4020,4021,4022,4023,4024,4025,4026,4027,4028,4029,4030,4031,4032,4033,4034,4035,4036,4037,4038,4039,4040,4041,4042,4043,4044,4045,4046,4047,4048,4049,4050,4051,4052,4053,4054,4055,4056,4057,4058,4059,4060,4061,4062,4063,4064,4065,4066,4067,4068,4069,4070,4071,4072,4073,4074,4075,4076,4077,4078,4079,4080,4081,4082,4083,4084,4085,4086,4087,4088,4089,4090,4091,4092,4093,4094,4095,4096,4097,4098,4099,4100,4101,4102,4103,4104,4105,4106,4107,4108,4109,4110,4111,4112,4113,4114,4115,4116,4117,4118,4119,4120,4121,4122,4123,4124,4125,4126,4127,4128,4129,4130,4131,4132,4133,4134,4135,4136,4137,4138,4139,4140,4141,4143,4144,4145,4146,4147,4148,4149,4150,4151,4152,4153,4154,4155,4156,4157,4158,4159,4160,4161,4162,4163,4164,4165,4166,4167,4168,4169,4170,4171,4172,4173,4175,4176,4177,4178,4179,4180,4181,4182,4183,4185,4186,4187,4188,4189,4190,4191,4192,4193,4194,4195,4196,4197,4198,4199,4200,4201,4202,4203,4204,4205,4206,4207,4208,4209,4210,4211,4212,4213,4214,4215,4216,4217,4218,4219,4220,4221,4222,4223,4224,4225,4226,4227,4228,4229,4230,4231,4232,4233,4234,4235,4236,4237,4238,4239,4240,4241,4242,4243,4244,4246,4247,4248,4249,4250,4251,4252,4253,4254,4255,4256,4259,4260,4261,4262,4263,4264,4265,4266,4267,4268,4269,4270,4271,4272,4273,4274,4275,4276,4277,4278,4279,4280,4281,4282,4283,4284,4285,4286,4287,4288,4289,4290,4291,4292,4293,4294,4295,4296,4297,4298,4299,4300,4301,4302,4303,4304,4305,4306,4307,4308,4309,4310,4311,4312,4313,4314,4315,4316,4317,4318,4319,4320,4321,4322,4323,4324,4325,4326,4327,4328,4329,4330,4331,4332,4333,4334,4335,4336,4337,4338,4339,4340,4341,4342,4343,4345,4346,4347,4348,4349,4350,4351,4352,4353,4354,4355,4356,4357,4358,4359,4360,4361,4362,4363,4364,4365,4366,4367,4368,4369,4370,4371,4372,4373,4374,4375,4376,4377,4378,4379,4380,4381,4382,4383,4384,4385,4386,4387,4388,4389,4390,4391,4392,4393,4394,4395,4396,4397,4398,4399,4400,4401,4402,4403,4404,4405,4406,4407,4408,4409,4410,4411,4412,4413,4414,4415,4416,4417,4418,4419,4420,4421,4422,4423,4424,4426,4427,4428,4429,4430,4431,4432,4433,4434,4435,4436,4437,4438,4439,4440,4441,4442,4443,4444,4445,4446,4447,4448,4449,4450,4451,4452,4453,4454,4455,4456,4457,4458,4459,4460,4461,4462,4463,4464,4465,4466,4467,4468,4469,4470,4471,4472,4473,4474,4475,4476,4477,4478,4479,4480,4481,4482,4483,4484,4485,4486,4487,4488,4489,4490,4491,4492,4493,4494,4495,4496,4497,4498,4499,4500,4501,4502,4503,4504,4506,4508,4510,4511,4512,4513,4514,4515,4516,4517,4518,4519,4520,4521,4522,4523,4524,4525,4526,4527,4528,4529,4530,4531,4532,4533,4534,4535,4536,4537,4538,4539,4540,4541,4542,4543,4544,4545,4546,4547,4548,4549,4550,4551,4552,4553,4554,4555,4556,4557,4558,4559,4560,4561,4562,4563,4564,4565,4566,4567,4568,4569,4570,4571,4572,4573,4574,4575,4576,4577,4578,4579,4580,4581,4582,4583,4584,4585,4587,4588,4589,4590,4591,4592,4593,4594,4595,4596,4597,4598,4599,4600,4601,4602,4603,4604,4605,4606,4607,4608,4609,4610,4611,4612,4613,4614,4615,4616,4617,4618,4619,4620,4621,4622,4623,4624,4625,4626,4627,4628,4629,4631,4632,4633,4634,4635,4636,4637,4638,4639,4640,4641,4642,4643,4644,4645,4646,4647,4648,4649,4652,4653,4656,4657,4658,4660,4661,4663,4664,4665,4666,4667,4668,4669,4670,4671,4672,4673,4674,4675,4676,4677,4678,4679,4680,4681,4682,4683,4684,4685,4686,4688,4689,4690,4691,4692,4693,4694,4695,4696,4697,4698,4699,4700,4701,4702,4703,4704,4705,4706,4707,4708,4709,4710,4711,4712,4713,4714,4715,4716,4717,4718,4720,4721,4725,4726,4727,4728,4729,4730,4731,4732,4733,4734,4735,4745,4746,4747,4748,4749,4750,4751,4752,4753,4754,4755,4756,4757,4758,4759,4761,4762,4763,4764,4765,4766,4767,4768,4769,4770,4771,4772,4773,4774,4775,4776,4777,4778,4779,4780,4781,4782,4785,4805,4806,4807,4825,4826,4827,4828,4829,4830,4831,4832,4833,4834,4835,4836,4837,4838,4839,4840,4841,4842,4843,4844,4845,4846,4847,4848,4849,4850,4851,4852,4853,4854,4855,4856,4857,4858,4859,4860,4861,4862,4864,4865,4866,4867,4868,4869,4870,4871,4872,4873,4874,4875,4877,4878,4885,4886,4887,4888,4889,4890,4891,4892,4893,4894,4895,4896,4897,4898,4899,4900,4901,4902,4903,4905,4906,4907,4908,4909,4910,4911,4912,4913,4914,4915,4916,4917,4918,4919,4920,4921,4922,4923,4924,4925,4926,4927,4928,4929,4930,4931,4932,4933,4934,4935,4936,4937,4938,4939,4940,4941,4942,4943,4944,4945,4946,4947,4948,4949,4950,4951,4952,4953,4954,4955,4956,4957,4958,4959,4960,4961,4962,4963,4964,4965,4966,4967,4968,4969,4970,4971,4972,4973,4974,4975,4976,4977,4978,4979,4980,4981,4982,4983,4985,4986,4987,4988,4989,4990,4991,4992,4993,4994,4995,4996,4997,4998,4999,5000,5001,5003,5004,5005,5006,5007,5008,5009,5010,5012,5013,5014,5015,5016,5017,5018,5019,5020,5021,5022,5023,5025,5026,5027,5028,5029,5030,5032,5033,5034,5035,5036,5037,5038,5039,5040,5041,5042,5043,5044,5047,5048,5049,5050,5051,5052,5065,5066,5067,5068,5069,5070,5071,5072,5073,5074,5075,5076,5077,5078,5079,5080,5081,5082,5083,5084,5085,5086,5087,5088,5089,5090,5091,5092,5093,5105,5106,5107,5108,5125,5126,5127,5128,5129,5130,5131,5145,5146,5165,5166,5167,5186,5187,5190,5205,5206,5207,5225,5226,5227,5228,5229,5230,5231,5232,5233,5234,5235,5236,5237,5238,5239,5240,5241,5242,5243,5244,5245,5246,5265,5266,5285,5286,5287,5288,5289,5290,5291,5292,5293,5294,5295,5296,5297,5299,5305,5306,5307,5325,5326,5327,5345,5346,5365,5366,5367,5368,5369,5370,5371,5372,5373,5374,5375,5376,5377,5378,5379,5385,5386,5405,5406,5407,5408,5409,5410,5425,5426,5427,5428,5429,5430,5431,5432,5433,5434,5435,5436,5437,5438,5439,5440,5441,5442,5443,5444,5445,5446,5447,5448,5449,5450,5451,5465,5485,5486,5487,5488,5489,5490,5491,5492,5493,5494,5497,5498,5499,5505,5506,5507,5508,5509,5510,5525,5526,5527,5528,5529,5530,5531,5532,5533,5534,5535,5545,5546,5547,5548,5549,5550,5551,5552,5553,5554,5555,5556,5557,5558,5559,5560,5561,5562,5563,5564,5565,5566,5567,5568,5569,5570,5571,5572,5573,5574,5575,5576,5577,5585,5586,5605,5606,5607,5608,5609,5610,5611,5612,5613,5625,5626,5645,5646,5647,5648,5665,5685,5705,5706,5707,5708,5709,5710,5711,5712,5725,5726,5727,5728,5729,5730,5745,5746,5747,5748,5749,5751,5752,5753,5754,5765,5766,5767,5768,5769,5770,5771,5772,5773,5774,5775,5776,5777,5778,5779,5780,5781,5782,5783,5784,5785,5786,5805,5806,5807,5808,5809,5810,5811,5812,5813,5814,5815,5816,5817,5818,5819,5820,5821,5822,5823,5824,5825,5826,5827,5828,5829,5830,5831,5832,5845,5846,5847,5848,5849,5850,5851,5865,5866,5885,5905,5906,5907,5908,5909,5910,5925,5926,5927,5945,5965,5966,5985,5986,5987,5988,5989,5990,5991,6005,6006,6007,6009,6010,6025,6026,6028,6029,6030,6031,6032,6033,6034,6035,6036,6037,6038,6039,6040,6041,6042,6043,6044,6045,6046,6047,6048,6049,6050,6051,6052,6053,6054,6055,6056,6057,6058,6059,6060,6061,6062,6063,6064,6065,6066,6067,6068,6069,6070,6071,6072,6073,6074,6075,6076,6077,6078,6079,6080,6081,6082,6083,6084,6085,6086,6087,6088,6089,6090,6091,6092,6093,6094,6095,6096,6097,6098,6099,6100,6101,6102,6103,6104,6105,6106,6108,6109,6110,6111,6112,6113,6114,6115,6116,6117,6118,6119,6120,6121,6122,6123,6124,6126,6127,6128,6148,6168,6170,6171,6172,6173,6189,6190,6191,6192,6193,6194,6195,6196,6197,6198,6208,6209,6210,6211,6212,6213,6214,6228,6229,6230,6231,6232,6233,6234,6248,6268,6269,6270,6271,6288,6290,6291,6292,6293,6294,6295,6296,6297,6298,6299,6300,6301,6302,6303,6308,6328,6348,6349,6350,6351,6352,6353,6368,6369,6370,6371,6372,6373,6374,6375,6376,6377,6378,6379,6380,6388,6389,6390,6391,6392,6393,6394,6395,6396,6397,6398,6399,6400,6401,6402,6403,6404,6405,6406,6407,6408,6409,6410,6411,6412,6413,6414,6415,6416,6417,6418,6419,6420,6421,6422,6423,6424,6425,6426,6427,6428,6429,6430,6431,6432,6433,6434,6435,6436,6437,6438,6439,6440,6441,6442,6443,6444,6446,6447,6448,6449,6468,6469,6470,6471,6472,6473,6474,6475,6476,6477,6478,6479,6480,6481,6482,6483,6484,6485,6486,6487,6488,6489,6490,6491,6492,6493,6494,6495,6496,6497,6498,6508,6509,6510,6511,6512,6513,6514,6515,6528,6529,6530,6531,6532,6533,6534,6535,6536,6537,6538,6539,6540,6541,6542,6543,6544,6545,6546,6547,6548,6549,6550,6551,6552,6553,6554,6568,6569,6570,6571,6572,6573,6574,6575,6588,6589,6590,6608,6609,6610,6611,6628,6629,6630,6631,6632,6633,6649,6668,6669,6670,6671,6672,6673,6674,6675,6676,6677,6678,6679,6680,6681,6682,6684,6685,6686,6687,6688,6690,6692,6693,6694,6695,6696,6697,6698,6699,6700,6701,6702,6703,6704,6705,6707,6708,6709,6710,6711,6712,6713,6714,6715,6716,6717,6718,6719,6720,6721,6722,6723,6724,6725,6726,6727,6728,6729,6730,6731,6732,6733,6734,6735,6736,6737,6738,6739,6740,6741,6742,6743,6744,6745,6746,6747,6748,6749,6750,6751,6752,6753,6754,6755,6756,6757,6758,6759,6760,6761,6762,6763,6764,6765,6766,6767,6768,6769,6770,6771,6772,6773,6774,6775,6776,6777,6778,6779,6780,6781,6782,6783,6784,6785,6786,6787,6788,6789,6790,6791,6792,6793,6794,6795,6796,6797,6798,6799,6800,6801,6802,6803,6804,6805,6806,6807,6808,6809,6810,6811,6812,6813,6814,6815,6817,6818,6819,6820,6821,6822,6823,6824,6825,6826,6827,6828,6829,6830,6831,6832,6833,6834,6835,6836,6837,6838,6839,6840,6841,6842,6843,6844,6845,6846,6847,6848,6849,6850,6851,6852,6853,6854,6855,6856,6857,6858,6868,6869,6870,6871,6872,6873,6874,6875,6876,6877,6878,6879,6880,6881,6882,6883,6884,6888,6889,6890,6891,6892,6893,6894,6895,6908,6909,6910,6912,6913,6914,6915,6916,6917,6918,6919,6920,6921,6922,6923,6924,6925,6926,6927,6928,6929,6930,6931,6932,6933,6934,6935,6936,6937,6938,6939,6940,6941,6942,6943,6944,6945,6946,6947,6948,6949,6950,6951,6952,6953,6954,6955,6956,6957,6958,6959,6960,6961,6962,6963,6964,6965,6966,6967,6968,6973,6974,6975,6976,6977,6978,6979,6980,6981,6982,6983,6984,6985,6986,6987,6988,6989,6990,6991,6992,6993,6994,6995,6996,6997,6998,6999,7000,7001,7002,7003,7004,7005,7006,7007,7008,7009,7011,7012,7013,7014,7015,7016,7017,7018,7019,7020,7021,7022,7023,7024,7025,7026,7027,7028,7029,7030,7031,7032,7033,7034,7035,7036,7037,7038,7039,7040,7041,7042,7043,7044,7045,7046,7047,7048,7049,7050,7051,7052,7053,7054,7055,7056,7057,7058,7059,7060,7061,7073,7074,7093,7094,7095,7096,7097,7098,7099,7100,7101,7102,7103,7104,7105,7106,7107,7108,7109,7110,7111,7112,7113,7114,7115,7116,7117,7118,7119,7120,7121,7122,7123,7124,7125,7126,7127,7128,7129,7130,7131,7132,7133,7134,7135,7136,7137,7138,7153,7154,7155,7156,7157,7158,7159,7160,7161,7162,7163,7164,7165,7166,7167,7168,7169,7170,7171,7172,7173,7174,7175,7176,7177,7178,7179,7180,7181,7182,7183,7184,7185,7186,7187,7188,7189,7190,7191,7192,7193,7194,7195,7196,7197,7198,7199,7200,7201,7202,7203,7204,7205,7206,7207,7208,7209,7210,7211,7212,7213,7214,7215,7216,7217,7218,7219,7220,7221,7222,7223,7224,7225,7226,7227,7228,7229,7230,7231,7232,7233,7234,7235,7236,7237,7238,7239,7240,7241,7242,7243,7244,7245,7246,7247,7248,7249,7250,7268,7269,7270,7271,7272,7273,7274,7275,7288,7289,7308,7309,7310,7311,7328,7329,7330,7331,7332,7333,7334,7335,7336,7337,7338,7339,7340,7341,7342,7343,7344,7345,7346,7347,7348,7349,7350,7351,7352,7353,7354,7355,7356,7357,7358,7359,7360,7361,7362,7363,7364,7365,7366,7367,7368,7369,7370,7371,7372,7373,7374,7375,7376,7377,7378,7379,7380,7381,7382,7383,7384,7389,7390,7409,7429,7449,7469,7470,7489,7509,7510,7511,7529,7530,7531,7532,7533,7534,7535,7536,7537,7538,7549,7550,7552,7553,7554,7555,7569,7570,7571,7572,7589,7590,7591,7592,7593,7594,7595,7596,7597,7598,7599,7600,7601,7602,7603,7604,7605,7606,7607,7608,7609,7610,7611,7612,7613,7614,7615,7616,7617,7618,7619,7620,7621,7622,7623,7624,7625,7626,7627,7628,7629,7631,7632,7633,7649,7669,7670,7671,7672,7673,7689,7690,7691,7709,7729,7730,7749,7750,7751,7752,7753,7754,7755,7756,7757,7758,7759,7760,7761,7762,7763,7764,7765,7766,7769,7789,7790,7791,7792,7793,7794,7795,7796,7797,7798,7799,7800,7801,7802,7803,7804,7805,7806,7807,7808,7809,7810,7811,7812,7813,7814,7815,7816,7817,7818,7819,7820,7821,7822,7823,7824,7825,7826,7827,7828,7829,7830,7831,7832,7833,7834,7835,7836,7837,7838,7839,7840,7841,7842,7844,7845,7846,7847,7848,7849,7850,7851,7852,7853,7854,7855,7856,7857,7858,7859,7860,7861,7862,7863,7864,7865,7866,7867,7868,7869,7870,7871,7872,7873,7874,7875,7876,7889,7890,7891,7892,7893,7894,7895,7896,7897,7898,7899,7900,7901,7902,7903,7904,7905,7906,7907,7908,7909,7910,7911,7912,7913,7914,7915,7916,7917,7918,7919,7920,7921,7922,7923,7929,7930,7931,7932,7933,7934,7935,7936,7937,7949,7950,7951,7952,7969,7970,7971,7972,7974,7975,7976,7989,7990,7991,7992,7993,7994,7995,7996,7997,7998,7999,8000,8001,8009,8010,8011,8012,8013,8014,8015,8029,8049,8050,8053,8054,8055,8069,8089,8109,8129,8149,8150,8151,8152,8169,8170,8171,8172,8173,8174,8175,8176,8177,8178,8179,8180,8181,8182,8183,8184,8185,8186,8189,8190,8191,8209,8229,8249,8269,8270,8271,8289,8309,8310,8311,8312,8313,8314,8315,8316,8317,8318,8329,8330,8331,8332,8333,8334,8335,8349,8350,8351,8352,8353,8354,8355,8356,8369,8389,8390,8391,8392,8395,8409,8410,8411,8412,8413,8429,8430,8449,8469,8470,8471,8472,8473,8474,8475,8476,8477,8478,8479,8489,8509,8510,8511,8512,8529,8549,8550,8569,8570,8571,8572,8573,8574,8575,8589,8590,8591,8592,8593,8594,8595,8596,8597,8598,8609,8610,8611,8612,8629,8630,8631,8632,8633,8634,8649,8650,8651,8652,8653,8654,8655,8656,8657,8658,8659,8660,8661,8662,8663,8664,8665,8666,8667,8668,8669,8670,8671,8672,8673,8674,8675,8676,8677,8678,8679,8680,8681,8682,8683,8684,8685,8686,8687,8688,8689,8690,8691,8692,8693,8694,8695,8696,8697,8698,8699,8700,8701,8702,8703,8704,8705,8706,8707,8708,8709,8710,8711,8712,8713,8714,8715,8716,8717,8718,8729,8749,8750,8751,8752,8753,8754,8755,8756,8757,8758,8759,8760,8761,8762,8763,8764,8765,8766,8767,8768,8769,8772,8773,8774,8776,8777,8778,8779,8780,8781,8782,8783,8789,8790,8791,8792,8793,8794,8795,8796,8797,8798,8799,8800,8801,8802,8803,8804,8805,8806,8807,8808,8809,8810,8811,8812,8813,8814,8815,8816,8817,8818,8819,8820,8821,8822,8823,8824,8825,8826,8827,8828,8829,8830,8831,8832,8833,8834,8837,8838,8839,8840,8841,8842,8843,8844,8845,8846,8847,8848,8849,8869,8870,8871,8872,8889,8890,8891,8892,8893,8894,8895,8896,8897,8898,8899,8900,8901,8909,8910,8929,8949,8969,8970,8971,8972,8989,9009,9010,9011,9012,9013,9014,9015,9016,9017,9018,9019,9020,9021,9022,9023,9024,9025,9026,9027,9028,9029,9030,9031,9032,9033,9049,9050,9051,9052,9053,9054,9055,9069,9070,9071,9072,9073,9074,9089,9109,9129,9130,9131,9132,9133,9134,9135,9136,9149,9150,9151,9169,9189,9209,9210,9211,9212,9213,9229,9230,9231,9232,9233,9234,9235,9236,9249,9250,9251,9252,9253,9254,9255,9256,9257,9258,9259,9260,9261,9262,9263,9264,9265,9266,9267,9268,9269,9270,9271,9272,9273,9274,9275,9276,9277,9278,9279,9280,9281,9282,9283,9284,9285,9286,9287,9288,9289,9290,9291,9292,9293,9294,9295,9309,9329,9330,9331,9332,9333,9334,9335,9336,9337,9338,9339,9340,9341,9342,9343,9344,9345,9346,9347,9348,9349,9350,9351,9352,9353,9354,9369,9370,9371,9372,9389,9390,9391,9392,9409,9410,9411,9412,9413,9414,9415,9416,9417,9418,9419,9420,9421,9422,9423,9424,9425,9426,9427,9428,9429,9430,9431,9432,9433,9434,9435,9436,9437,9438,9439,9440,9441,9442,9443,9444,9445,9446,9447,9448,9449,9469,9470,9471,9472,9473,9474,9475,9476,9477,9489,9490,9491,9492,9509,9510,9511,9529,9530,9531,9532,9533,9534,9535,9549,9550,9551,9552,9553,9554,9555,9556,9557,9558,9560,9561,9562,9563,9564,9565,9566,9567,9568,9569,9570,9571,9572,9573,9574,9575,9576,9577,9578,9579,9580,9581,9582,9583,9584,9585,9586,9587,9588,9589,9590,9591,9592,9593,9594,9595,9596,9599,9602,9603,9604,9605,9606,9609,9610,9613,9614,9615,9616,9617,9618,9619,9620,9621,9623,9625,9626,9627,9628,9629,9630,9633,9634,9635,9636,9637,9638,9641,9642,9643,9644,9645,9648,9649,9652,9653,9656,9657,9660,9661,9664,9665,9668,9669,9670,9671,9672,9673,9674,9675,9676,9677,9678,9679,9680,9681,9682,9683,9684,9685,9686,9687,9688,9689,9690,9691,9692,9693,9694,9695,9696,9697,9698,9699,9709,9710,9711,9712,9714,9715,9716,9717,9718,9719,9720,9721,9722,9723,9724,9725,9726,9727,9728,9729,9730,9731,9732,9733,9734,9735,9736,9737,9738,9739,9740,9741,9742,9749,9750,9751,9752,9753,9754,9755,9756,9757,9758,9760,9761,9762,9763,9764,9765,9766,9767,9768,9769,9770,9771,9772,9773,9774,9775,9776,9777,9778,9779,9780,9781,9782,9783,9784,9785,9786,9787,9788,9789,9790,9791,9792,9793,9794,9795,9796,9797,9798,9799,9800,9801,9802,9803,9804,9805,9806,9807,9808,9809,9810,9811,9812,9813,9814,9815,9816,9817,9818,9819,9820,9821,9822,9823,9824,9825,9826,9829,9832,9849,9850,9851,9852,9853,9854,9855,9856,9857,9858,9859,9860,9861,9862,9863,9864,9865,9866,9867,9868,9869,9870,9871,9872,9873,9874,9875,9876,9877,9878,9879,9880,9881,9882,9883,9884,9885,9886,9887,9888,9889,9890,9891,9892,9893,9894,9895,9896,9897,9898,9899,9900,9901,9902,9903,9904,9905,9906,9909,9910,9911,9912,9913,9929,9934,9935,9936,9937,9938,9939,9940,9941,9942,9943,9944,9946,9947,9949,9950,9951,9952,9953,9954,9955,9956,9957,9958,9969,9989,9990,9991,9992,9993,9994,9995,9996,9997,9999,10000,10001,10002,10003,10004,10005,10006,10007,10008,10009,10010,10015,10029,10030,10031,10032,10033,10034,10035,10036,10037,10038,10039,10040,10041,10042,10043,10044,10045,10046,10049,10050,10051,10052,10054,10055,10056,10057,10069,10070,10071,10089,10090,10091,10092,10093,10094,10095,10109,10110,10111,10113,10114,10115,10116,10129,10130,10131,10132,10133,10134,10135,10136,10137,10138,10139,10140,10141,10142,10143,10144,10145,10146,10147,10148,10149,10150,10151,10152,10153,10169,10170,10171,10172,10173,10174,10175,10176,10177,10178,10179,10180,10181,10182,10183,10184,10185,10186,10187,10188,10189,10190,10191,10192,10193,10194,10195,10196,10197,10198,10199,10209,10210,10211,10212,10213,10214,10215,10216,10217,10218,10219,10220,10221,10222,10223,10224,10225,10226,10227,10228,10229,10230,10231,10232,10233,10234,10235,10236,10237,10238,10239,10240,10241,10242,10243,10244,10245,10246,10247,10248,10249,10250,10251,10252,10253,10254,10255,10256,10269,10270,10271,10273,10274,10275,10277,10278,10279,10280,10281,10282,10283,10284,10285,10286,10287,10288,10289,10290,10291,10292,10293,10294,10295,10296,10297,10298,10299,10300,10301,10302,10303,10304,10305,10306,10307,10308,10309,10310,10311,10312,10313,10314,10315,10316,10317,10318,10319,10320,10321,10322,10323,10324,10325,10326,10327,10328,10329,10330,10331,10332,10333,10334,10335,10336,10337,10338,10339,10340,10341,10342,10343,10344,10345,10346,10347,10348,10349,10350,10351,10352,10353,10354,10355,10356,10357,10358,10359,10360,10361,10362,10363,10364,10365,10366,10367,10368,10369,10370,10371,10372,10373,10374,10375,10376,10377,10378,10379,10380,10381,10382,10383,10384,10385,10386,10387,10388,10389,10390,10391,10392,10393,10394,10395,10396,10397,10398,10399,10400,10401,10402,10403,10404,10405,10406,10407,10408,10409,10410,10411,10412,10413,10414,10415,10416,10417,10418,10419,10420,10421,10422,10423,10424,10425,10426,10427,10428,10429,10430,10431,10432,10433,10434,10435,10436,10437,10438,10439,10440,10441,10442,10443,10444,10445,10446,10447,10448,10449,10450,10451,10452,10453,10454,10455,10456,10457,10458,10459,10460,10461,10462,10463,10464,10465,10466,10468,10469,10470,10471,10472,10473,10474,10475,10476,10477,10478,10479,10480,10481,10482,10483,10484,10485,10486,10487,10488,10489,10490,10491,10492,10493,10494,10495,10496,10497,10498,10499,10500,10501,10502,10503,10504,10505,10506,10507,10508,10509,10510,10511,10512,10513,10514,10515,10516,10517,10518,10519,10529,10530,10531,10532,10533,10534,10535,10536,10537,10538,10539,10540,10541,10542,10543,10544,10545,10546,10547,10548,10549,10550,10551,10552,10553,10554,10557,10558,10559,10560,10561,10562,10563,10564,10565,10566,10567,10569,10570,10571,10572,10573,10574,10575,10576,10577,10578,10579,10580,10581,10582,10583,10584,10585,10586,10587,10588,10589,10590,10591,10592,10609,10610,10611,10612,10613,10614,10615,10616,10617,10618,10619,10620,10621,10622,10623,10624,10625,10626,10627,10628,10629,10630,10631,10632,10633,10634,10635,10636,10637,10638,10639,10640,10641,10642,10643,10644,10645,10646,10647,10648,10649,10650,10651,10652,10653,10654,10655,10656,10657,10658,10659,10660,10661,10662,10663,10664,10665,10666,10667,10668,10669,10670,10671,10672,10673,10674,10689,10690,10691,10692,10693,10694,10695,10696,10697,10698,10699,10700,10701,10702,10703,10704,10705,10706,10707,10708,10709,10710,10714,10718,10719,10720,10721,10722,10723,10724,10725,10726,10727,10728,10729,10730,10731,10732,10733,10734,10735,10736,10737,10738,10739,10740,10741,10742,10743,10744,10745,10746,10747,10748,10749,10750,10751,10752,10753,10754,10771,10772,10789,10790,10791,10792,10793,10794,10795,10796,10797,10798,10799,10800,10801,10802,10803,10804,10805,10806,10807,10808,10809,10810,10811,10812,10813,10814,10815,10816,10817,10818,10819,10820,10821,10822,10823,10824,10825,10826,10827,10828,10829,10830,10831,10832,10833,10834,10849,10850,10851,10852,10853,10854,10855,10856,10857,10858,10869,10870,10871,10872,10873,10874,10875,10876,10877,10878,10889,10890,10891,10892,10893,10894,10895,10896,10897,10898,10899,10900,10901,10902,10903,10904,10905,10906,10907,10908,10909,10910,10911,10912,10913,10914,10915,10916,10917,10918,10919,10920,10921,10922,10923,10924,10925,10926,10927,10928,10929,10930,10931,10932,10933,10934,10935,10936,10937,10938,10939,10940,10941,10942,10943,10944,10945,10946,10947,10948,10949,10950,10951,10952,10953,10954,10955,10956,10957,10958,10969,10970,10971,10972,10973,10974,10975,10976,10977,10978,10979,10980,10981,10982,10983,10984,10985,10986,10987,10988,10989,10990,10991,10992,10993,10994,10995,10996,11009,11010,11011,11012,11013,11014,11015,11016,11017,11018,11019,11020,11021,11022,11023,11024,11025,11026,11027,11028,11029,11031,11032,11033,11034,11035,11036,11037,11038,11039,11040,11041,11042,11043,11044,11045,11046,11047,11048,11049,11050,11051,11052,11053,11054,11055,11056,11057,11058,11059,11060,11061,11062,11063,11069,11070,11071,11072,11073,11074,11075,11076,11077,11078,11079,11080,11081,11082,11083,11084,11085,11086,11087,11088,11089,11090,11091,11092,11093,11094,11095,11096,11097,11098,11099,11100,11101,11102,11103,11104,11105,11106,11107,11108,11109,11110,11111,11112,11113,11114,11115,11116,11117,11118,11119,11120,11121,11122,11123,11124,11125,11126,11127,11128,11129,11130,11131,11132,11133,11134,11135,11136,11137,11138,11139,11140,11141,11142,11143,11144,11145,11146,11147,11148,11149,11150,11151,11152,11153,11154,11155,11156,11157,11158,11159,11160,11161,11162,11163,11164,11165,11166,11167,11168,11169,11170,11171,11172,11173,11174,11175,11176,11177,11178,11179,11180,11181,11182,11183,11184,11185,11189,11190,11191,11192,11193,11194,11195,11196,11197,11198,11199,11200,11201,11202,11203,11204,11205,11206,11207,11208,11209,11210,11211,11212,11213,11214,11215,11216,11217,11218,11219,11220,11221,11222,11223,11224,11225,11226,11227,11228,11229,11230,11231,11232,11233,11234,11235,11236,11237,11238,11239,11240,11241,11242,11243,11244,11245,11246,11247,11248,11249,11250,11251,11252,11253,11254,11255,11256,11257,11258,11259,11260,11261,11262,11263,11264,11265,11266,11267,11268,11269,11270,11271,11272,11273,11274,11275,11276,11277,11278,11279,11280,11281,11282,11283,11284,11285,11286,11287,11288,11289,11290,11291,11292,11293,11294,11295,11296,11297,11298,11299,11300,11301,11302,11303,11304,11305,11306,11307,11308,11309,11310,11311,11312,11313,11314,11315,11316,11317,11318,11319,11320,11321,11322,11323,11324,11325,11326,11327,11328,11329,11330,11331,11332,11333,11334,11335,11336,11337,11338,11339,11340,11341,11342,11343,11344,11345,11346,11347,11348,11349,11350,11351,11352,11353,11354,11355,11356,11357,11358,11359,11360,11361,11362,11363,11369,11370,11371,11372,11373,11374,11375,11376,11377,11379,11380,11381,11382,11383,11384,11385,11386,11387,11388,11389,11390,11391,11392,11393,11394,11395,11396,11397,11398,11399,11400,11401,11402,11403,11404,11405,11406,11407,11408,11409,11410,11411,11412,11413,11414,11415,11416,11417,11418,11419,11420,11421,11422,11423,11424,11425,11426,11427,11428,11429,11430,11431,11432,11433,11434,11435,11436,11437,11438,11439,11440,11441,11442,11443,11444,11445,11446,11447,11448,11449,11450,11451,11452,11453,11454,11455,11456,11457,11458,11459,11460,11461,11462,11463,11464,11465,11466,11467,11468,11469,11470,11471,11472,11473,11474,11475,11476,11477,11478,11479,11489,11490,11491,11492,11509,11510,11511,11512,11513,11532,11533,11534,11536,11537,11538,11539,11542,11543,11544,11545,11546,11548,11549,11550,11551,11552,11553,11554,11555,11556,11557,11558,11559,11560,11561,11562,11563,11564,11565,11566,11567,11568,11569,11570,11571,11572,11573,11574,11576,11577,11578,11579,11581,11582,11583,11584,11585,11586,11589,11590,11591,11592,11593,11594,11595,11596,11609,11610,11611,11629,11630,11631,11632,11633,11634,11635,11636,11637,11638,11639,11640,11641,11649,11650,11651,11652,11653,11654,11655,11656,11657,11658,11659,11660,11661,11662,11663,11664,11665,11666,11667,11668,11669,11670,11671,11672,11673,11674,11675,11676,11679,11680,11681,11682,11683,11684,11685,11686,11687,11688,11689,11690,11691,11692,11693,11694,11709,11710,11711,11712,11713,11714,11729,11730,11731,11733,11734,11735,11736,11737,11738,11739,11740,11741,11742,11743,11744,11745,11746,11747,11748,11749,11750,11751,11752,11753,11754,11755,11756,11757,11758,11759,11760,11761,11762,11763,11764,11765,11766,11767,11768,11769,11770,11771,11772,11773,11774,11775,11776,11777,11789,11790,11791,11792,11793,11794,11795,11796,11797,11798,11799,11800,11801,11802,11803,11804,11805,11806,11809,11810,11811,11812,11813,11814,11815,11816,11817,11818,11819,11820,11821,11822,11823,11824,11825,11826,11827,11828,11829,11830,11831,11832,11833,11834,11835,11836,11849,11850,11851,11852,11853,11854,11855,11856,11857,11858,11859,11860,11861,11862,11863,11864,11865,11866,11867,11868,11869,11870,11871,11872,11873,11874,11875,11876,11877,11878,11879,11880,11881,11882,11883,11884,11885,11886,11887,11888,11889,11890,11892,11893,11894,11895,11896,11898,11899,11900,11901,11902,11903,11904,11905,11906,11907,11908,11909,11910,11911,11912,11913,11914,11929,11930,11949,11950,11951,11952,11953,11954,11955,11956,11957,11958,11959,11960,11961,11962,11963,11964,11965,11966,11967,11968,11969,11970,11971,11972,11973,11974,11975,11976,11977,11978,11979,11980,11981,11983,11984,11985,11986,11987,11988,11989,11990,11991,11992,11993,11994,11995,11996,11997,11998,11999,12000,12001,12002,12003,12004,12005,12006,12007,12008,12009,12010,12011,12012,12013,12014,12015,12016,12017,12019,12020,12021,12022,12023,12024,12025,12026,12027,12028,12029,12030,12031,12032,12033,12034,12035,12036,12037,12038,12039,12040,12041,12042,12043,12044,12045,12046,12047,12048,12049,12050,12051,12052,12053,12054,12055,12056,12057,12058,12059,12060,12061,12062,12063,12064,12065,12066,12067,12068,12069,12070,12071,12072,12073,12074,12075,12076,12077,12078,12079,12080,12081,12082,12083,12084,12089,12109,12110,12129,12149,12150,12151,12152,12153,12154,12155,12156,12157,12158,12159,12160,12161,12162,12163,12164,12165,12166,12167,12168,12169,12170,12171,12189,12190,12191,12192,12193,12194,12195,12196,12197,12198,12199,12200,12201,12202,12203,12204,12205,12206,12207,12208,12209,12210,12229,12230,12231,12232,12233,12234,12235,12236,12237,12238,12239,12240,12241,12242,12243,12244,12245,12246,12247,12249,12269,12270,12271,12272,12289,12290,12291,12292,12293,12309,12310,12329,12330,12331,12332,12333,12334,12335,12336,12337,12338,12339,12340,12341,12342,12343,12344,12345,12346,12347,12348,12349,12350,12369,12370,12371,12372,12373,12374,12389,12390,12391,12392,12409,12429,12430,12431,12432,12433,12434,12435,12436,12449,12469,12470,12471,12472,12473,12474,12475,12476,12477,12489,12509,12529,12530,12549,12569,12589,12609,12610,12629,12649,12669,12670,12671,12672,12673,12674,12675,12676,12677,12678,12679,12680,12681,12682,12683,12689,12709,12729,12749,12750,12769,12789,12790,12809,12810,12811,12812,12813,12814,12815,12816,12817,12818,12819,12820,12821,12822,12823,12824,12829,12830,12831,12832,12849,12869,12889,12890,12891,12892,12893,12894,12909,12910,12911,12912,12913,12914,12915,12916,12917,12918,12919,12920,12921,12922,12923,12924,12925,12926,12927,12928,12929,12930,12931,12932,12933,12934,12935,12936,12938,12940,12941,12942,12943,12944,12945,12946,12948,12949,12950,12951,12952,12953,12954,12955,12956,12957,12958,12959,12960,12961,12962,12963,12964,12965,12966,12967,12968,12969,12970,12971,12972,12973,12974,12975,12976,12977,12989,12990,12991,12992,13009,13029,13030,13031,13032,13049,13050,13069,13089,13090,13091,13093,13095,13096,13097,13098,13099,13109,13110,13111,13129,13130,13132,13149,13151,13152,13169,13170,13171,13172,13173,13174,13189,13190,13209,13210,13211,13212,13229,13230,13231,13232,13249,13250,13251,13252,13253,13254,13255,13256,13257,13258,13259,13260,13261,13262,13263,13264,13265,13266,13267,13268,13269,13270,13271,13272,13273,13274,13275,13276,13277,13278,13279,13280,13281,13282,13283,13284,13285,13286,13287,13288,13289,13290,13291,13292,13293,13294,13295,13296,13297,13298,13299,13300,13301,13302,13303,13304,13305,13306,13307,13308,13309,13310,13311,13312,13313,13314,13315,13316,13317,13318,13319,13320,13321,13322,13323,13325,13326,13328,13330,13331,13334,13335,13336,13337,13338,13339,13340,13341,13342,13343,13344,13345,13346,13347,13348,13349,13350,13351,13352,13353,13354,13355,13356,13357,13358,13359,13360,13361,13362,13363,13364,13365,13366,13367,13368,13369,13370,13371,13372,13373,13374,13375,13376,13377,13378,13379,13380,13381,13382,13383,13384,13385,13386,13387,13388,13389,13390,13391,13392,13393,13394,13395,13398,13399,13400,13401,13402,13403,13409,13410,13411,13412,13413,13414,13415,13429,13430,13431,13432,13433,13434,13435,13436,13437,13438,13439,13440,13441,13442,13443,13444,13445,13446,13449,13450,13451,13452,13453,13454,13455,13456,13457,13458,13459,13460,13461,13462,13463,13464,13465,13466,13467,13468,13469,13470,13471,13472,13473,13474,13475,13476,13477,13478,13489,13490,13509,13529,13530,13531,13532,13533,13534,13536,13537,13538,13539,13540,13541,13542,13543,13544,13545,13546,13547,13548,13549,13550,13551,13552,13553,13554,13555,13556,13557,13558,13559,13560,13561,13562,13563,13564,13565,13566,13567,13568,13569,13570,13571,13572,13573,13574,13589,13590,13591,13592,13593,13594,13595,13596,13597,13598,13599,13600,13601,13609,13610,13629,13630,13631,13632,13633,13634,13635,13636,13637,13638,13639,13640,13641,13642,13643,13644,13645,13646,13647,13648,13649,13650,13651,13652,13653,13654,13655,13656,13657,13658,13659,13660,13661,13662,13669,13670,13671,13672,13673,13674,13675,13676,13677,13678,13679,13680,13681,13682,13683,13684,13685,13686,13687,13688,13689,13709,13710,13711,13712,13713,13714,13715,13729,13730,13731,13732,13749,13769,13770,13771,13772,13773,13774,13775,13776,13777,13778,13779,13780,13781,13782,13783,13784,13785,13786,13787,13788,13789,13790,13791,13792,13793,13794,13795,13796,13797,13798,13799,13800,13801,13802,13803,13804,13805,13806,13807,13808,13809,13810,13811,13812,13813,13814,13815,13816,13817,13818,13819,13820,13821,13822,13823,13824,13825,13826,13827,13828,13829,13830,13831,13832,13833,13834,13835,13836,13837,13838,13839,13840,13841,13842,13843,13849,13850,13851,13852,13853,13854,13855,13856,13869,13889,13890,13891,13892,13893,13894,13895,13896,13909,13929,13949,13950,13951,13952,13953,13954,13969,13970,13971,13972,13989,13990,13991,13992,13993,14012,14013,14014,14015,14016,14017,14018,14019,14020,14021,14022,14023,14024,14025,14026,14027,14028,14029,14030,14031,14032,14033,14034,14035,14036,14037,14052,14053,14054,14055,14056,14057,14058,14059,14060,14061,14062,14063,14064,14072,14092,14093,14112,14132,14152,14172,14173,14192,14193,14212,14213,14214,14215,14216,14232,14233,14234,14235,14236,14252,14253,14254,14255,14256,14257,14272,14273,14292,14293,14294,14295,14296,14298,14299,14300,14301,14302,14303,14304,14305,14306,14307,14308,14309,14310,14312,14313,14314,14315,14316,14317,14318,14319,14320,14321,14322,14323,14324,14325,14326,14327,14328,14329,14330,14331,14332,14333,14334,14335,14336,14337,14338,14339,14340,14341,14342,14343,14344,14345,14346,14347,14348,14349,14350,14352,14353,14354,14355,14356,14357,14358,14359,14360,14361,14362,14363,14364,14365,14366,14367,14368,14369,14370,14371,14372,14373,14374,14375,14376,14377,14378,14379,14380,14381,14382,14383,14384,14385,14386,14387,14388,14389,14390,14391,14392,14393,14394,14395,14396,14397,14398,14399,14400,14401,14402,14403,14404,14405,14406,14407,14408,14409,14410,14411,14412,14413,14414,14415,14416,14417,14418,14419,14420,14421,14422,14423,14424,14425,14426,14427,14428,14429,14430,14431,14452,14453,14454,14472,14473,14492,14493,14494,14495,14496,14497,14498,14499,14500,14501,14502,14503,14504,14505,14506,14507,14508,14509,14510,14511,14512,14513,14514,14515,14516,14517,14518,14519,14520,14521,14522,14523,14524,14525,14526,14527,14528,14529,14530,14531,14532,14533,14534,14535,14536,14537,14538,14539,14540,14541,14542,14544,14545,14546,14547,14548,14549,14550,14551,14552,14553,14554,14555,14556,14557,14558,14559,14560,14561,14562,14563,14564,14565,14566,14567,14572,14573,14574,14575,14576,14577,14578,14579,14580,14581,14582,14583,14584,14585,14586,14587,14588,14589,14590,14591,14592,14593,14594,14612,14613,14614,14615,14616,14632,14633,14634,14652,14653,14654,14655,14656,14657,14658,14659,14660,14661,14662,14663,14664,14665,14666,14668,14669,14670,14671,14672,14673,14674,14675,14692,14693,14694,14695,14696,14697,14698,14699,14700,14701,14702,14703,14704,14705,14706,14707,14708,14709,14710,14712,14713,14714,14715,14716,14717,14718,14719,14720,14732,14733,14752,14753,14754,14755,14756,14757,14758,14759,14760,14761,14762,14763,14764,14765,14766,14767,14768,14769,14770,14771,14772,14773,14774,14775,14776,14777,14778,14779,14780,14781,14782,14783,14784,14785,14786,14792,14793,14794,14795,14796,14797,14798,14799,14800,14812,14813,14832,14852,14853,14854,14855,14856,14872,14873,14874,14875,14876,14877,14878,14879,14880,14881,14882,14883,14884,14885,14886,14887,14888,14889,14890,14894,14912,14913,14914,14932,14933,14934,14935,14936,14937,14938,14939,14940,14941,14942,14943,14944,14945,14946,14947,14948,14949,14950,14951,14952,14953,14954,14972,14973,14974,14975,14976,14977,14978,14979,14980,14981,14982,14983,14984,14985,14992,14993,14994,14995,14996,14997,14998,14999,15000,15001,15002,15003,15004,15012,15013,15032,15033,15072,15092,15093,15094,15095,15096,15097,15098,15099,15100,15101,15102,15112,15113,15114,15115,15116,15132,15133,15134,15135,15136,15137,15138,15139,15140,15141,15142,15143,15144,15145,15146,15147,15148,15149,15150,15151,15152,15153,15154,15155,15172,15173,15174,15175,15176,15177,15178,15179,15180,15181,15182,15183,15184,15185,15186,15187,15188,15189,15190,15191,15192,15193,15194,15195,15196,15197,15198,15199,15200,15201,15202,15203,15204,15205,15206,15207,15208,15209,15210,15211,15212,15213,15214,15215,15216,15217,15218,15219,15220,15221,15222,15223,15224,15225,15226,15227,15228,15229,15230,15231,15232,15233,15234,15235,15236,15237,15238,15239,15240,15241,15242,15243,15244,15245,15246,15247,15248,15249,15250,15251,15252,15253,15254,15255,15256,15257,15258,15259,15260,15262,15263,15264,15265,15266,15267,15268,15269,15271,15272,15273,15274,15275,15276,15277,15278,15279,15280,15281,15282,15283,15284,15285,15286,15287,15288,15289,15290,15291,15292,15293,15294,15295,15296,15297,15298,15300,15301,15302,15303,15304,15305,15306,15307,15308,15309,15310,15311,15312,15313,15314,15315,15316,15317,15318,15319,15320,15321,15322,15323,15324,15325,15326,15327,15328,15329,15330,15331,15332,15333,15334,15335,15336,15337,15338,15339,15340,15341,15342,15343,15344,15345,15346,15347,15349,15350,15351,15352,15353,15354,15355,15356,15357,15358,15359,15360,15361,15362,15363,15364,15365,15366,15367,15368,15369,15370,15371,15372,15373,15374,15375,15376,15377,15378,15379,15380,15381,15382,15383,15384,15385,15386,15387,15388,15389,15390,15391,15392,15393,15394,15395,15396,15397,15398,15399,15400,15401,15402,15403,15404,15405,15406,15407,15408,15409,15410,15411,15412,15413,15414,15415,15416,15417,15418,15419,15420,15421,15422,15423,15424,15425,15426,15427,15428,15429,15430,15431,15432,15433,15434,15435,15436,15437,15438,15439,15441,15442,15443,15444,15445,15446,15447,15448,15449,15450,15451,15452,15453,15454,15455,15456,15457,15458,15459,15460,15461,15462,15463,15464,15465,15466,15467,15468,15469,15470,15471,15472,15473,15474,15475,15476,15477,15478,15479,15480,15481,15482,15483,15484,15485,15486,15487,15488,15489,15490,15491,15492,15493,15494,15495,15496,15497,15498,15499,15500,15501,15502,15503,15504,15505,15506,15507,15508,15509,15510,15511,15512,15513,15514,15515,15516,15517,15518,15519,15520,15521,15522,15523,15524,15525,15526,15527,15528,15529,15530,15531,15532,15533,15534,15535,15536,15537,15538,15539,15540,15541,15542,15543,15544,15545,15546,15547,15548,15549,15550,15551,15552,15553,15554,15555,15556,15557,15558,15559,15560,15561,15562,15563,15564,15565,15566,15567,15568,15569,15570,15571,15572,15573,15574,15575,15576,15577,15578,15579,15580,15581,15582,15583,15584,15585,15586,15587,15588,15589,15590,15591,15592,15593,15594,15595,15596,15597,15598,15599,15600,15601,15602,15603,15604,15605,15606,15607,15608,15609,15610,15611,15612,15613,15614,15615,15616,15617,15618,15619,15620,15621,15622,15623,15624,15625,15626,15627,15628,15629,15630,15631,15632,15633,15634,15635,15636,15637,15638,15639,15640,15641,15642,15643,15644,15645,15646,15647,15648,15649,15650,15651,15652,15653,15654,15655,15656,15657,15658,15659,15660,15661,15662,15663,15664,15665,15666,15667,15668,15669,15670,15671,15672,15673,15674,15675,15676,15677,15678,15679,15680,15681,15682,15683,15684,15685,15686,15687,15688,15689,15690,15691,15692,15693,15694,15695,15696,15697,15698,15699,15700,15701,15702,15703,15704,15705,15706,15707,15708,15709,15710,15711,15712,15713,15714,15715,15716,15717,15718,15719,15720,15721,15722,15723,15724,15725,15726,15727,15728,15729,15730,15731,15732,15733,15734,15735,15736,15737,15738,15739,15740,15741,15742,15743,15744,15745,15746,15747,15748,15749,15750,15751,15752,15753,15754,15755,15756,15757,15758,15759,15760,15761,15762,15763,15764,15765,15766,15767,15768,15769,15770,15771,15772,15773,15774,15775,15776,15777,15778,15779,15780,15781,15782,15783,15784,15785,15786,15787,15788,15789,15790,15791,15792,15793,15794,15795,15796,15797,15798,15799,15800,15801,15802,15803,15804,15805,15806,15807,15808,15809,15810,15811,15812,15813,15814,15815,15816,15817,15818,15819,15820,15821,15822,15823,15824,15825,15826,15827,15828,15829,15830,15831,15832,15833,15834,15835,15836,15837,15838,15839,15840,15841,15842,15843,15844,15845,15846,15847,15848,15849,15850,15851,15852,15853,15854,15855,15856,15857,15858,15859,15860,15861,15862,15863,15864,15865,15866,15867,15868,15869,15870,15871,15872,15873,15874,15875,15876,15877,15878,15879,15880,15881,15882,15883,15884,15885,15886,15887,15888,15889,15890,15891,15892,15893,15894,15895,15896,15897,15898,15899,15900,15901,15902,15903,15904,15905,15906,15907,15908,15909,15910,15911,15912,15913,15914,15915,15916,15917,15918,15919,15920,15921,15922,15923,15924,15925,15926,15927,15928,15929,15930,15931,15932,15933,15934,15935,15936,15937,15938,15939,15940,15941,15942,15945,15946,15947,15948,15949,15950,15951,15952,15953,15954,15955,15956,15957,15958,15959,15960,15961,15962,15963,15964,15965,15966,15967,15968,15969,15970,15971,15972,15974,15975,15976,15977,15978,15979,15980,15981,15982,15983,15984,15985,15986,15987,15988,15989,15990,15991,15992,15993,15994,15995,15996,15997,15998,15999,16000,16003,16004,16005,16006,16007,16008,16009,16010,16011,16012,16013,16014,16015,16016,16017,16018,16019,16020,16021,16022,16023,16024,16025,16026,16027,16028,16029,16030,16031,16032,16033,16034,16035,16036,16037,16038,16039,16040,16041,16042,16043,16044,16045,16046,16047,16048,16049,16050,16051,16052,16053,16054,16055,16057,16058,16059,16060,16062,16063,16064,16065,16066,16067,16068,16069,16070,16071,16072,16073,16074,16075,16076,16077,16078,16079,16080,16081,16082,16083,16084,16085,16086,16087,16088,16090,16091,16092,16093,16094,16095,16096,16099,16100,16102,16103,16104,16105,16106,16107,16108,16109,16110,16111,16112,16113,16115,16116,16117,16118,16119,16120,16121,16122,16123,16124,16125,16126,16127,16128,16129,16130,16131,16132,16133,16134,16135,16136,16137,16138,16139,16143,16144,16145,16146,16147,16148,16149,16150,16151,16152,16153,16154,16155,16156,16157,16158,16159,16160,16161,16162,16163,16164,16165,16166,16167,16168,16169,16170,16171,16172,16173,16174,16175,16176,16177,16178,16179,16180,16181,16182,16183,16184,16185,16186,16187,16188,16189,16191,16192,16193,16195,16196,16197,16198,16199,16200,16201,16202,16203,16204,16205,16206,16207,16208,16209,16210,16211,16212,16213,16214,16215,16216,16217,16218,16219,16220,16221,16222,16223,16224,16225,16226,16227,16228,16229,16230,16231,16232,16233,16234,16235,16236,16237,16238,16239,16240,16241,16242,16243,16244,16245,16246,16247,16248,16249,16250,16251,16252,16253,16254,16255,16256,16257,16258,16259,16260,16262,16263,16264,16265,16266,16267,16268,16269,16270,16271,16272,16273,16274,16275,16276,16277,16278,16279,16280,16281,16282,16283,16284,16285,16286,16287,16288,16289,16290,16291,16292,16293,16294,16295,16296,16297,16298,16299,16300,16301,16302,16303,16304,16305,16306,16307,16308,16309,16310,16311,16312,16313,16314,16315,16316,16317,16318,16319,16320,16321,16322,16323,16324,16325,16326,16327,16328,16329,16331,16332,16333,16334,16335,16336,16337,16338,16339,16340,16341,16342,16343,16344,16345,16346,16347,16348,16349,16350,16351,16352,16353,16354,16355,16356,16357,16358,16359,16360,16361,16362,16363,16364,16365,16366,16367,16368,16369,16370,16371,16372,16373,16374,16375,16376,16377,16378,16379,16380,16381,16382,16383,16384,16385,16386,16387,16388,16389,16390,16391,16392,16393,16394,16395,16396,16397,16398,16399,16400,16401,16402,16403,16404,16405,16406,16407,16408,16409,16410,16411,16412,16413,16414,16415,16416,16417,16418,16419,16420,16421,16422,16423,16424,16425,16426,16427,16428,16429,16430,16431,16432,16433,16434,16435,16436,16437,16438,16440,16441,16442,16443,16444,16445,16446,16447,16448,16449,16450,16451,16452,16453,16454,16455,16456,16457,16458,16459,16460,16461,16462,16463,16464,16465,16466,16467,16468,16469,16470,16471,16472,16473,16474,16475,16476,16477,16478,16479,16480,16481,16482,16483,16484,16485,16486,16487,16488,16489,16490,16491,16492,16493,16494,16495,16496,16497,16498,16499,16500,16501,16502,16503,16504,16505,16506,16507,16508,16509,16510,16511,16512,16513,16514,16515,16516,16517,16518,16519,16520,16521,16522,16523,16524,16525,16526,16527,16528,16529,16530,16531,16532,16533,16534,16535,16536,16537,16538,16539,16540,16541,16543,16544,16545,16546,16547,16548,16549,16550,16551,16552,16553,16554,16555,16556,16557,16558,16559,16560,16561,16562,16563,16564,16565,16566,16567,16568,16569,16571,16572,16573,16574,16575,16576,16577,16578,16579,16580,16581,16582,16583,16584,16585,16586,16587,16588,16589,16590,16591,16592,16593,16594,16595,16596,16597,16598,16599,16600,16601,16602,16603,16604,16605,16606,16607,16608,16609,16610,16611,16612,16613,16614,16615,16616,16617,16618,16620,16621,16622,16623,16624,16625,16626,16627,16628,16629,16630,16631,16632,16633,16634,16635,16636,16637,16638,16639,16640,16641,16642,16643,16644,16645,16646,16647,16648,16649,16650,16651,16652,16653,16654,16655,16656,16657,16658,16659,16660,16661,16662,16663,16664,16665,16666,16667,16668,16669,16670,16671,16672,16673,16674,16675,16676,16677,16678,16679,16680,16681,16682,16683,16684,16685,16686,16687,16688,16689,16690,16691,16692,16693,16694,16695,16696,16697,16698,16699,16700,16701,16702,16703,16704,16705,16706,16707,16708,16709,16710,16711,16712,16713,16714,16715,16716,16717,16718,16719,16720,16721,16722,16723,16724,16725,16726,16727,16728,16729,16730,16731,16732,16733,16734,16735,16736,16737,16738,16739,16740,16741,16742,16743,16744,16745,16746,16747,16748,16749,16750,16751,16752,16753,16754,16755,16756,16757,16758,16759,16760,16761,16762,16763,16764,16765,16766,16767,16768,16769,16770,16771,16772,16773,16774,16775,16776,16777,16778,16779,16780,16781,16782,16783,16784,16785,16786,16787,16788,16789,16790,16791,16792,16793,16794,16795,16796,16797,16798,16799,16800,16801,16802,16803,16804,16805,16806,16807,16808,16809,16810,16811,16812,16813,16814,16815,16816,16817,16818,16819,16820,16821,16822,16823,16824,16825,16826,16827,16828,16829,16830,16831,16832,16833,16834,16835,16836,16837,16838,16839,16840,16841,16842,16843,16845,16846,16847,16848,16849,16850,16851,16852,16853,16854,16855,16856,16857,16858,16859,16860,16861,16862,16863,16864,16865,16866,16867,16868,16869,16870,16871,16872,16873,16874,16875,16876,16877,16878,16879,16880,16881,16882,16883,16884,16885,16886,16887,16888,16889,16890,16891,16892,16893,16894,16895,16896,16897,16898,16899,16900,16901,16902,16903,16905,16906,16907,16908,16910,16911,16912,16913,16914,16915,16916,16917,16918,16919,16920,16921,16922,16923,16924,16925,16926,16927,16928,16929,16930,16931,16932,16933,16934,16935,16936,16937,16938,16939,16940,16941,16942,16943,16944,16945,16946,16947,16948,16949,16950,16951,16952,16953,16954,16955,16956,16957,16958,16959,16960,16961,16962,16963,16964,16965,16966,16967,16968,16969,16970,16971,16972,16973,16974,16975,16976,16977,16978,16979,16980,16981,16982,16983,16984,16985,16986,16987,16988,16989,16990,16991,16992,16993,16995,16996,16997,16998,16999,17000,17001,17002,17003,17004,17005,17006,17007,17008,17009,17010,17011,17012,17013,17014,17015,17016,17017,17018,17019,17020,17021,17022,17023,17024,17025,17026,17027,17028,17029,17030,17031,17032,17033,17034,17035,17036,17037,17038,17039,17040,17041,17042,17043,17044,17045,17046,17047,17048,17049,17050,17051,17052,17053,17054,17055,17056,17057,17058,17059,17060,17061,17062,17063,17064,17065,17066,17067,17068,17069,17070,17071,17072,17073,17074,17075,17076,17077,17078,17079,17080,17081,17082,17083,17084,17085,17086,17087,17088,17089,17090,17091,17092,17093,17094,17095,17096,17097,17098,17099,17100,17101,17102,17103,17104,17105,17106,17107,17108,17109,17110,17111,17112,17113,17114,17115,17116,17117,17118,17119,17120,17121,17122,17123,17124,17136,17137,17138,17141,17142,17143,17144,17145,17146,17147,17148,17149,17150,17151,17152,17153,17154,17155,17156,17158,17159,17160,17161,17162,17163,17164,17165,17166,17167,17168,17169,17170,17171,17172,17173,17174,17175,17176,17177,17178,17179,17180,17181,17182,17183,17184,17185,17186,17187,17188,17189,17190,17191,17192,17193,17194,17195,17196,17197,17198,17199,17200,17201,17202,17203,17204,17205,17206,17207,17208,17209,17210,17211,17212,17213,17214,17215,17216,17217,17218,17219,17220,17221,17222,17223,17224,17225,17226,17227,17228,17229,17230,17231,17232,17233,17234,17235,17236,17237,17238,17239,17240,17241,17242,17243,17244,17245,17246,17247,17248,17249,17250,17251,17252,17253,17254,17255,17256,17257,17258,17259,17260,17261,17262,17263,17264,17265,17266,17267,17268,17269,17270,17271,17272,17273,17274,17275,17276,17277,17278,17279,17280,17281,17282,17283,17284,17285,17286,17287,17288,17289,17290,17291,17292,17293,17294,17295,17296,17297,17298,17300,17301,17302,17303,17304,17305,17306,17307,17308,17309,17310,17311,17312,17313,17314,17315,17316,17317,17318,17319,17320,17321,17322,17323,17324,17325,17326,17327,17328,17329,17330,17331,17332,17333,17334,17335,17336,17337,17338,17339,17340,17341,17342,17343,17344,17345,17346,17348,17349,17350,17351,17352,17353,17354,17355,17356,17357,17358,17359,17360,17361,17362,17363,17364,17365,17366,17367,17368,17369,17370,17371,17372,17373,17374,17375,17376,17377,17378,17380,17381,17384,17385,17386,17387,17388,17389,17402,17403,17404,17405,17406,17407,17408,17409,17410,17411,17412,17413,17414,17415,17416,17417,17418,17419,17420,17421,17422,17423,17424,17425,17426,17427,17428,17429,17430,17431,17432,17433,17434,17435,17436,17437,17438,17439,17440,17441,17442,17443,17444,17445,17446,17447,17448,17449,17450,17452,17453,17454,17455,17456,17457,17458,17459,17460,17461,17462,17463,17464,17465,17466,17467,17468,17469,17470,17471,17472,17473,17474,17475,17476,17477,17478,17479,17480,17481,17482,17483,17484,17485,17486,17487,17488,17489,17490,17491,17492,17493,17494,17495,17496,17497,17498,17499,17500,17501,17502,17503,17504,17505,17506,17507,17508,17509,17510,17511,17512,17513,17514,17515,17516,17517,17518,17519,17520,17521,17522,17523,17524,17525,17526,17527,17528,17529,17530,17531,17532,17533,17534,17535,17536,17537,17538,17539,17540,17541,17542,17543,17544,17545,17546,17547,17548,17549,17550,17551,17552,17553,17554,17555,17556,17557,17558,17559,17560,17561,17562,17563,17564,17565,17566,17567,17568,17569,17570,17571,17572,17573,17574,17575,17576,17577,17578,17579,17580,17581,17582,17583,17584,17585,17586,17587,17588,17589,17590,17591,17592,17593,17594,17595,17596,17597,17598,17599,17600,17601,17602,17603,17604,17605,17606,17607,17608,17609,17610,17611,17612,17613,17614,17615,17616,17617,17618,17619,17620,17621,17622,17623,17624,17625,17626,17627,17628,17629,17630,17631,17632,17633,17634,17635,17636,17637,17638,17639,17640,17641,17642,17643,17644,17645,17646,17647,17648,17650,17651,17652,17653,17654,17655,17656,17657,17658,17659,17660,17661,17662,17663,17664,17665,17666,17667,17668,17669,17670,17671,17672,17673,17674,17675,17676,17677,17678,17679,17680,17681,17682,17683,17684,17685,17686,17687,17688,17689,17690,17691,17693,17694,17695,17696,17697,17698,17699,17700,17701,17702,17703,17704,17705,17707,17709,17711,17712,17713,17714,17715,17716,17717,17718,17719,17720,17721,17722,17723,17724,17725,17726,17727,17728,17729,17730,17731,17732,17733,17734,17735,17736,17737,17739,17740,17741,17742,17743,17744,17745,17746,17747,17748,17749,17750,17751,17752,17753,17754,17755,17756,17757,17758,17759,17760,17761,17762,17763,17764,17765,17766,17767,17768,17769,17770,17771,17772,17773,17774,17775,17776,17777,17778,17779,17780,17781,17782,17783,17784,17785,17787,17788,17789,17790,17791,17792,17793,17794,17795,17796,17797,17798,17800,17801,17802,17803,17804,17805,17806,17807,17808,17809,17810,17811,17812,17813,17814,17815,17816,17817,17818,17819,17820,17821,17822,17823,17824,17825,17826,17827,17828,17829,17830,17831,17832,17833,17834,17835,17836,17837,17838,17839,17840,17841,17842,17843,17844,17845,17846,17847,17848,17849,17850,17851,17852,17853,17854,17855,17856,17857,17858,17859,17860,17864,17865,17866,17867,17868,17869,17870,17871,17872,17873,17874,17875,17876,17877,17878,17879,17880,17881,17882,17883,17884,17886,17887,17888,17889,17890,17891,17892,17893,17894,17895,17896,17897,17898,17899,17900,17901,17902,17903,17904,17905,17906,17907,17908,17909,17910,17911,17912,17913,17914,17915,17916,17917,17918,17919,17920,17921,17922,17923,17924,17925,17926,17927,17928,17929,17930,17931,17932,17933,17934,17935,17936,17937,17938,17939,17940,17941,17942,17943,17944,17945,17946,17947,17948,17949,17950,17951,17952,17953,17954,17955,17956,17957,17958,17959,17960,17961,17962,17963,17964,17965,17966,17967,17968,17969,17970,17971,17972,17973,17974,17976,17977,17978,17979,17980,17981,17982,17983,17985,17987,17988,17990,17991,17992,17999,18000,18001,18002,18003,18004,18005,18006,18007,18008,18009,18010,18011,18012,18013,18014,18015,18016,18017,18018,18019,18020,18021,18022,18023,18024,18025,18026,18027,18028,18029,18030,18031,18032,18033,18034,18035,18036,18037,18038,18039,18040,18041,18042,18043,18044,18045,18046,18047,18048,18049,18050,18051,18052,18053,18054,18055,18056,18057,18058,18059,18060,18061,18062,18063,18064,18065,18066,18067,18069,18070,18071,18072,18073,18074,18075,18077,18078,18079,18080,18081,18082,18083,18084,18085,18086,18087,18088,18089,18090,18091,18092,18093,18094,18095,18096,18097,18099,18100,18102,18103,18104,18105,18106,18107,18108,18109,18110,18111,18112,18113,18114,18115,18116,18117,18118,18119,18120,18121,18122,18123,18124,18125,18126,18127,18128,18129,18130,18132,18133,18134,18135,18136,18137,18138,18139,18140,18141,18142,18143,18144,18145,18146,18147,18148,18149,18150,18151,18153,18154,18155,18156,18157,18158,18159,18160,18161,18162,18163,18164,18165,18166,18167,18168,18169,18170,18171,18172,18173,18174,18175,18176,18177,18178,18179,18180,18181,18182,18183,18184,18185,18186,18187,18188,18189,18190,18191,18192,18193,18194,18195,18196,18197,18198,18199,18200,18201,18202,18203,18204,18205,18206,18207,18208,18209,18210,18211,18212,18213,18214,18215,18216,18217,18218,18219,18220,18221,18222,18223,18224,18227,18228,18229,18231,18232,18233,18234,18235,18236,18237,18238,18239,18240,18241,18242,18243,18244,18245,18246,18247,18248,18249,18250,18251,18252,18253,18254,18255,18256,18257,18258,18259,18260,18261,18262,18263,18264,18265,18266,18267,18268,18269,18270,18271,18272,18273,18274,18275,18276,18277,18278,18279,18280,18281,18282,18284,18285,18286,18287,18288,18289,18290,18291,18292,18293,18295,18296,18297,18298,18299,18300,18301,18302,18303,18304,18305,18306,18307,18308,18309,18310,18311,18312,18313,18314,18315,18316,18317,18318,18319,18320,18321,18322,18323,18324,18325,18326,18327,18328,18329,18330,18331,18332,18333,18334,18335,18336,18337,18338,18339,18340,18341,18342,18343,18344,18345,18346,18347,18348,18349,18350,18351,18352,18353,18354,18355,18356,18357,18358,18359,18360,18361,18362,18363,18364,18365,18366,18368,18369,18370,18371,18372,18373,18374,18375,18376,18377,18378,18379,18380,18381,18382,18383,18384,18386,18387,18388,18389,18390,18391,18392,18393,18394,18395,18396,18397,18398,18399,18400,18401,18402,18403,18404,18405,18406,18407,18408,18409,18410,18411,18412,18413,18414,18415,18416,18417,18418,18419,18420,18421,18422,18423,18424,18425,18426,18427,18428,18429,18430,18431,18432,18433,18434,18435,18436,18437,18438,18439,18440,18441,18442,18443,18444,18445,18446,18447,18448,18449,18450,18451,18452,18453,18454,18455,18456,18457,18458,18459,18460,18461,18462,18463,18464,18465,18466,18467,18468,18469,18470,18471,18472,18473,18474,18475,18476,18477,18478,18479,18480,18481,18482,18483,18484,18485,18486,18487,18488,18489,18491,18492,18493,18494,18495,18496,18497,18498,18499,18500,18501,18502,18503,18504,18505,18506,18507,18508,18509,18510,18511,18512,18513,18514,18515,18516,18517,18518,18519,18520,18521,18522,18523,18524,18525,18526,18527,18528,18529,18530,18531,18532,18533,18534,18535,18536,18537,18538,18539,18540,18541,18542,18543,18544,18545,18546,18547,18548,18549,18550,18551,18552,18553,18554,18555,18556,18557,18558,18559,18560,18561,18562,18563,18564,18565,18566,18567,18568,18569,18570,18571,18572,18573,18574,18575,18576,18577,18578,18579,18580,18581,18582,18583,18584,18585,18586,18587,18588,18589,18590,18591,18592,18593,18594,18595,18596,18597,18598,18599,18600,18601,18602,18603,18604,18605,18606,18607,18608,18609,18610,18611,18612,18613,18614,18615,18616,18617,18618,18619,18620,18621,18622,18623,18624,18625,18626,18627,18628,18629,18630,18631,18632,18633,18634,18635,18636,18637,18638,18639,18640,18641,18642,18643,18644,18645,18646,18647,18648,18649,18650,18651,18652,18653,18654,18655,18656,18657,18658,18659,18660,18661,18662,18663,18664,18665,18666,18667,18668,18669,18670,18671,18672,18673,18674,18675,18676,18677,18678,18679,18680,18681,18682,18683,18684,18685,18686,18687,18688,18689,18690,18691,18692,18693,18694,18695,18696,18697,18698,18699,18700,18701,18702,18703,18704,18705,18706,18707,18708,18709,18711,18712,18713,18714,18715,18716,18717,18718,18719,18720,18721,18722,18723,18724,18725,18726,18728,18729,18730,18731,18732,18733,18734,18735,18736,18737,18738,18739,18740,18741,18742,18743,18744,18745,18746,18747,18748,18749,18750,18751,18752,18753,18754,18755,18756,18757,18758,18759,18760,18761,18762,18763,18764,18765,18766,18767,18768,18769,18770,18771,18772,18773,18774,18775,18776,18777,18778,18779,18780,18781,18782,18783,18784,18785,18786,18787,18788,18789,18790,18791,18792,18793,18794,18795,18796,18797,18798,18799,18800,18801,18802,18803,18804,18805,18806,18807,18808,18809,18810,18811,18812,18813,18814,18815,18816,18817,18818,18819,18820,18821,18822,18823,18824,18825,18826,18827,18828,18829,18830,18831,18832,18833,18834,18835,18836,18837,18838,18839,18840,18841,18842,18843,18844,18845,18846,18847,18848,18849,18850,18851,18852,18853,18854,18855,18856,18857,18858,18859,18860,18861,18862,18863,18864,18865,18866,18867,18868,18869,18870,18871,18872,18873,18874,18875,18876,18877,18878,18879,18880,18881,18882,18883,18884,18885,18886,18887,18888,18889,18890,18891,18892,18893,18894,18895,18896,18897,18898,18899,18900,18901,18902,18903,18904,18905,18906,18907,18908,18909,18910,18911,18912,18913,18914,18915,18916,18917,18918,18919,18920,18921,18922,18923,18924,18925,18926,18927,18928,18929,18930,18931,18932,18933,18938,18939,18940,18941,18942,18943,18944,18945,18946,18947,18948,18949,18950,18951,18952,18953,18954,18955,18956,18957,18958,18959,18960,18961,18962,18963,18964,18965,18966,18967,18968,18969,18970,18971,18972,18973,18978,18979,18980,18981,18982,18983,18984,18985,18986,18987,18988,18989,18990,18991,18992,18993,18994,18995,18996,18997,18998,18999,19000,19001,19002,19003,19004,19005,19006,19007,19008,19009,19010,19011,19012,19013,19014,19015,19016,19017,19018,19019,19020,19021,19022,19023,19024,19025,19026,19027,19028,19029,19030,19031,19033,19034,19035,19036,19037,19038,19039,19040,19041,19042,19043,19044,19045,19046,19047,19048,19049,19050,19051,19052,19053,19054,19055,19056,19057,19058,19059,19060,19061,19062,19063,19064,19065,19066,19067,19069,19070,19071,19072,19073,19074,19075,19076,19077,19078,19079,19080,19081,19082,19083,19084,19085,19086,19087,19088,19089,19090,19091,19092,19093,19094,19095,19096,19097,19098,19099,19100,19101,19102,19103,19104,19105,19106,19107,19108,19109,19110,19111,19112,19113,19114,19115,19116,19117,19118,19119,19120,19121,19122,19123,19124,19125,19126,19127,19128,19129,19130,19131,19132,19134,19135,19136,19137,19138,19139,19140,19141,19142,19143,19144,19145,19147,19148,19149,19150,19151,19152,19153,19154,19155,19156,19157,19158,19159,19160,19161,19162,19163,19164,19165,19166,19167,19168,19169,19170,19171,19172,19173,19174,19175,19176,19177,19178,19179,19180,19181,19182,19183,19184,19185,19186,19187,19188,19189,19190,19191,19192,19193,19194,19195,19196,19197,19198,19199,19200,19201,19202,19203,19204,19205,19206,19207,19208,19209,19210,19211,19212,19213,19214,19215,19216,19217,19218,19219,19220,19221,19222,19223,19224,19225,19226,19227,19228,19229,19230,19231,19232,19233,19234,19235,19236,19237,19238,19239,19240,19241,19242,19243,19244,19245,19246,19247,19248,19249,19250,19251,19252,19253,19254,19255,19256,19257,19258,19259,19260,19261,19262,19263,19264,19265,19266,19267,19268,19269,19270,19271,19272,19273,19274,19275,19276,19277,19278,19279,19280,19281,19282,19283,19284,19285,19286,19287,19288,19289,19290,19291,19292,19293,19294,19295,19296,19297,19298,19299,19300,19301,19302,19303,19304,19305,19306,19307,19308,19309,19310,19311,19312,19313,19314,19315,19316,19317,19318,19319,19320,19321,19322,19323,19324,19325,19326,19327,19328,19329,19330,19331,19332,19333,19334,19335,19336,19337,19338,19339,19340,19341,19342,19343,19344,19345,19346,19347,19348,19349,19350,19351,19353,19354,19355,19356,19357,19358,19359,19360,19361,19362,19363,19364,19365,19366,19367,19368,19369,19370,19371,19372,19373,19374,19375,19376,19377,19378,19379,19380,19381,19382,19383,19384,19385,19386,19387,19388,19390,19391,19392,19393,19394,19396,19397,19398,19399,19400,19401,19402,19403,19404,19405,19406,19407,19408,19409,19410,19411,19412,19413,19414,19415,19416,19417,19418,19419,19420,19421,19422,19423,19424,19425,19426,19427,19428,19429,19430,19431,19432,19433,19434,19435,19436,19437,19438,19439,19440,19441,19442,19443,19444,19445,19446,19447,19448,19449,19450,19451,19452,19453,19454,19455,19456,19458,19459,19460,19461,19462,19463,19464,19465,19466,19467,19468,19469,19470,19471,19472,19473,19474,19475,19476,19477,19478,19479,19480,19481,19482,19483,19484,19485,19486,19487,19488,19489,19490,19491,19492,19493,19494,19495,19496,19497,19498,19499,19500,19501,19502,19503,19504,19505,19506,19507,19508,19509,19510,19511,19512,19513,19514,19515,19516,19517,19518,19519,19520,19521,19522,19523,19524,19525,19526,19527,19528,19529,19530,19531,19532,19533,19534,19535,19536,19537,19538,19539,19540,19541,19542,19543,19544,19545,19546,19547,19548,19549,19550,19551,19552,19553,19554,19555,19556,19557,19558,19559,19560,19561,19562,19563,19564,19565,19566,19567,19568,19569,19570,19571,19572,19573,19574,19575,19576,19577,19578,19579,19580,19581,19582,19583,19584,19585,19586,19587,19588,19589,19590,19591,19592,19593,19594,19595,19596,19597,19598,19599,19600,19601,19602,19603,19604,19605,19606,19607,19608,19609,19610,19611,19612,19613,19614,19615,19616,19617,19618,19619,19620,19621,19622,19623,19624,19625,19626,19627,19628,19629,19630,19631,19632,19633,19634,19635,19636,19637,19638,19639,19640,19641,19642,19643,19644,19645,19646,19647,19648,19649,19650,19651,19652,19653,19654,19655,19656,19657,19658,19659,19660,19661,19662,19663,19664,19665,19666,19667,19668,19669,19670,19671,19672,19673,19674,19675,19676,19677,19678,19679,19680,19681,19682,19683,19684,19685,19686,19687,19688,19689,19690,19691,19692,19693,19694,19695,19696,19697,19698,19699,19701,19702,19703,19704,19705,19706,19707,19708,19709,19710,19711,19712,19713,19714,19715,19716,19717,19718,19719,19720,19721,19722,19723,19724,19725,19726,19727,19728,19729,19730,19732,19733,19734,19735,19736,19737,19738,19739,19740,19742,19743,19744,19745,19746,19747,19748,19749,19750,19751,19752,19753,19754,19755,19756,19757,19758,19759,19760,19762,19763,19764,19765,19766,19767,19768,19769,19770,19771,19772,19773,19774,19775,19776,19777,19778,19779,19780,19781,19782,19783,19784,19785,19786,19787,19788,19789,19790,19791,19792,19793,19794,19795,19796,19797,19798,19799,19800,19801,19802,19803,19804,19805,19806,19807,19808,19809,19810,19811,19812,19813,19814,19815,19816,19817,19818,19819,19820,19821,19822,19823,19824,19825,19826,19827,19828,19829,19830,19831,19832,19833,19834,19835,19836,19837,19838,19839,19840,19841,19842,19843,19844,19845,19846,19847,19848,19849,19850,19851,19852,19853,19854,19855,19856,19857,19858,19859,19860,19861,19862,19863,19864,19865,19866,19867,19868,19869,19870,19871,19872,19873,19874,19875,19876,19877,19878,19879,19880,19881,19882,19883,19884,19885,19886,19887,19888,19889,19890,19891,19892,19893,19894,19895,19896,19897,19898,19899,19900,19901,19902,19903,19904,19905,19906,19907,19908,19909,19910,19911,19912,19913,19914,19915,19916,19917,19918,19919,19920,19921,19922,19923,19924,19925,19926,19927,19928,19929,19930,19931,19932,19933,19934,19935,19936,19937,19938,19939,19940,19941,19942,19943,19944,19945,19946,19947,19948,19949,19950,19951,19952,19953,19954,19955,19956,19957,19958,19959,19960,19961,19962,19963,19964,19965,19966,19967,19968,19970,19971,19972,19973,19974,19975,19976,19977,19978,19979,19980,19981,19982,19983,19984,19985,19986,19987,19988,19989,19990,19991,19992,19993,19994,19995,19996,19997,19998,19999,20000,20001,20002,20003,20004,20005,20006,20007,20008,20009,20010,20011,20012,20013,20014,20015,20016,20017,20018,20019,20020,20021,20022,20023,20024,20025,20026,20027,20028,20029,20030,20031,20032,20033,20034,20035,20036,20037,20038,20039,20040,20041,20042,20043,20044,20045,20046,20047,20048,20049,20050,20051,20052,20053,20054,20055,20056,20057,20058,20059,20060,20061,20062,20063,20064,20065,20066,20067,20068,20069,20070,20071,20072,20073,20074,20075,20076,20077,20078,20079,20080,20081,20082,20083,20084,20085,20086,20087,20088,20089,20090,20091,20092,20093,20094,20095,20096,20097,20098,20099,20100,20101,20102,20103,20104,20105,20106,20107,20108,20109,20110,20111,20112,20113,20114,20115,20116,20117,20118,20119,20120,20121,20122,20123,20124,20125,20126,20127,20128,20129,20130,20131,20132,20133,20134,20135,20136,20137,20138,20139,20140,20141,20142,20143,20144,20145,20146,20147,20148,20149,20150,20151,20152,20153,20154,20155,20156,20157,20158,20160,20161,20162,20163,20164,20165,20166,20167,20168,20169,20170,20171,20172,20173,20174,20175,20176,20177,20178,20179,20180,20181,20182,20183,20184,20185,20186,20187,20188,20189,20190,20191,20192,20193,20194,20195,20196,20197,20198,20199,20200,20201,20202,20203,20204,20205,20206,20207,20208,20209,20210,20211,20212,20213,20214,20215,20216,20217,20218,20219,20220,20221,20222,20223,20224,20225,20226,20227,20228,20229,20230,20231,20232,20233,20234,20235,20236,20237,20238,20239,20240,20241,20242,20243,20244,20245,20247,20249,20250,20251,20252,20253,20254,20255,20256,20257,20258,20259,20260,20261,20262,20263,20264,20265,20266,20267,20268,20269,20270,20271,20272,20273,20274,20275,20276,20277,20278,20279,20280,20281,20282,20283,20284,20285,20286,20287,20288,20289,20290,20291,20292,20293,20294,20295,20296,20297,20298,20299,20300,20301,20302,20303,20304,20305,20306,20307,20308,20309,20310,20311,20312,20313,20314,20315,20316,20317,20318,20319,20320,20321,20322,20323,20324,20325,20326,20327,20328,20329,20330,20331,20332,20333,20334,20335,20336,20337,20338,20339,20340,20341,20342,20343,20344,20345,20346,20347,20348,20349,20350,20351,20352,20353,20354,20355,20356,20357,20358,20359,20360,20361,20362,20363,20364,20365,20366,20367,20368,20369,20370,20371,20372,20373,20374,20375,20376,20377,20378,20379,20380,20381,20382,20383,20384,20385,20386,20387,20388,20389,20390,20391,20392,20393,20394,20395,20396,20397,20398,20399,20400,20401,20402,20403,20404,20405,20406,20407,20408,20409,20410,20411,20412,20413,20414,20415,20416,20417,20418,20419,20420,20421,20422,20423,20424,20425,20426,20427,20428,20429,20430,20431,20432,20433,20434,20435,20436,20437,20438,20439,20440,20441,20442,20443,20444,20445,20446,20447,20448,20449,20450,20451,20452,20453,20454,20455,20456,20457,20458,20459,20460,20461,20462,20463,20464,20465,20466,20467,20468,20469,20470,20471,20472,20473,20474,20475,20476,20477,20478,20479,20480,20481,20482,20483,20484,20485,20486,20487,20488,20489,20490,20491,20492,20493,20494,20495,20496,20497,20498,20499,20500,20501,20502,20503,20504,20505,20506,20507,20508,20509,20510,20511,20512,20513,20514,20515,20516,20517,20518,20519,20520,20521,20522,20523,20524,20525,20526,20527,20528,20529,20530,20531,20532,20533,20534,20535,20536,20537,20538,20539,20540,20541,20542,20543,20544,20545,20546,20547,20548,20550,20551,20552,20553,20554,20555,20556,20557,20558,20559,20560,20561,20562,20563,20564,20565,20566,20567,20568,20569,20570,20571,20572,20573,20574,20575,20576,20577,20578,20579,20580,20581,20582,20583,20584,20585,20586,20587,20588,20589,20590,20591,20592,20593,20594,20595,20596,20597,20598,20599,20600,20601,20602,20603,20604,20605,20606,20607,20608,20609,20610,20611,20612,20613,20614,20615,20616,20617,20618,20619,20620,20621,20622,20623,20624,20625,20626,20627,20628,20630,20631,20632,20633,20634,20635,20636,20637,20638,20639,20640,20641,20642,20643,20644,20645,20646,20647,20648,20649,20650,20651,20652,20653,20654,20655,20656,20657,20658,20659,20660,20661,20662,20663,20664,20665,20666,20667,20668,20669,20670,20671,20672,20673,20674,20675,20676,20677,20678,20679,20680,20681,20682,20683,20684,20685,20686,20687,20688,20689,20690,20691,20692,20693,20694,20695,20696,20698,20699,20700,20701,20702,20703,20704,20705,20706,20707,20708,20709,20710,20711,20712,20713,20714,20715,20716,20717,20718,20719,20720,20721,20722,20723,20724,20725,20726,20727,20728,20729,20730,20731,20732,20733,20734,20735,20736,20737,20738,20739,20740,20741,20742,20743,20744,20745,20746,20747,20748,20749,20750,20751,20752,20753,20754,20755,20756,20757,20758,20759,20760,20761,20762,20763,20764,20765,20766,20767,20768,20769,20770,20771,20772,20773,20774,20775,20776,20777,20778,20779,20780,20781,20782,20783,20784,20785,20786,20787,20788,20789,20790,20791,20792,20793,20794,20795,20796,20797,20798,20799,20800,20801,20802,20803,20804,20805,20806,20807,20808,20809,20810,20811,20812,20813,20814,20815,20816,20817,20818,20819,20820,20821,20822,20823,20824,20825,20826,20827,20828,20829,20830,20831,20832,20833,20834,20835,20836,20837,20838,20839,20840,20841,20842,20843,20844,20845,20846,20847,20848,20849,20850,20851,20852,20853,20854,20855,20856,20857,20858,20860,20861,20862,20863,20864,20865,20866,20867,20868,20869,20870,20871,20872,20873,20874,20875,20876,20877,20878,20879,20880,20881,20882,20883,20884,20885,20886,20887,20888,20889,20890,20891,20892,20893,20894,20895,20896,20897,20898,20899,20900,20901,20902,20903,20904,20905,20906,20907,20908,20909,20910,20911,20912,20913,20914,20915,20916,20917,20918,20919,20920,20921,20922,20923,20924,20925,20926,20927,20928,20929,20930,20931,20932,20933,20934,20935,20936,20937,20938,20939,20940,20941,20942,20943,20944,20945,20946,20947,20948,20949,20950,20951,20952,20953,20954,20955,20956,20957,20958,20959,20960,20961,20962,20963,20964,20965,20966,20967,20968,20969,20970,20971,20972,20973,20974,20975,20976,20977,20978,20979,20981,20982,20983,20984,20985,20986,20987,20988,20989,20990,20991,20992,20993,20994,20995,20996,20997,20998,20999,21000,21001,21002,21003,21004,21005,21006,21007,21008,21009,21010,21011,21012,21013,21014,21015,21016,21017,21018,21019,21020,21021,21022,21023,21024,21025,21026,21027,21028,21029,21030,21031,21032,21033,21034,21035,21036,21037,21038,21039,21040,21041,21042,21043,21044,21045,21046,21047,21048,21049,21050,21051,21052,21053,21054,21055,21056,21057,21058,21059,21060,21061,21062,21063,21064,21065,21066,21067,21068,21069,21070,21072,21073,21074,21075,21076,21077,21078,21079,21080,21081,21082,21083,21084,21085,21086,21087,21088,21089,21090,21091,21092,21093,21094,21095,21096,21097,21098,21099,21100,21101,21102,21103,21104,21105,21106,21107,21108,21109,21110,21111,21112,21113,21114,21115,21116,21117,21118,21119,21120,21121,21122,21123,21124,21125,21126,21127,21128,21129,21130,21131,21132,21133,21134,21135,21136,21137,21138,21139,21140,21141,21142,21143,21144,21145,21146,21147,21148,21150,21151,21152,21153,21154,21155,21156,21157,21158,21159,21160,21161,21162,21163,21164,21165,21166,21167,21168,21169,21170,21171,21172,21173,21174,21175,21176,21177,21178,21179,21180,21181,21182,21183,21184,21185,21186,21187,21188,21189,21190,21191,21193,21194,21195,21196,21197,21198,21199,21200,21201,21202,21203,21204,21205,21206,21207,21208,21209,21210,21211,21212,21213,21214,21215,21216,21217,21218,21219,21220,21221,21222,21223,21227,21228,21229,21230,21231,21232,21233,21234,21235,21236,21237,21238,21239,21240,21241,21242,21243,21244,21245,21246,21247,21252,21253,21254,21255,21256,21257,21258,21259,21260,21261,21262,21263,21264,21265,21266,21267,21268,21269,21270,21271,21272,21273,21274,21275,21276,21277,21278,21279,21280,21281,21282,21283,21284,21285,21286,21287,21288,21289,21290,21291,21292,21293,21294,21295,21296,21297,21298,21299,21300,21301,21302,21303,21304,21305,21306,21307,21308,21309,21310,21311,21312,21313,21314,21315,21316,21317,21318,21319,21320,21321,21322,21323,21324,21325,21326,21327,21328,21329,21330,21331,21332,21333,21334,21335,21336,21337,21338,21339,21340,21341,21342,21343,21344,21345,21346,21347,21348,21349,21350,21351,21352,21353,21354,21355,21356,21357,21358,21359,21360,21361,21362,21363,21364,21365,21366,21367,21368,21369,21370,21371,21372,21373,21374,21375,21376,21377,21378,21379,21380,21381,21382,21383,21384,21385,21386,21387,21388,21389,21390,21391,21392,21393,21394,21395,21396,21397,21398,21399,21400,21401,21402,21403,21404,21405,21406,21407,21408,21409,21410,21411,21412,21413,21414,21415,21416,21417,21418,21419,21420,21421,21422,21423,21424,21425,21426,21427,21428,21429,21430,21431,21432,21433,21434,21435,21436,21437,21438,21439,21440,21441,21442,21443,21444,21445,21446,21447,21448,21449,21450,21451,21452,21453,21454,21455,21456,21457,21458,21459,21460,21461,21462,21463,21464,21465,21466,21467,21468,21469,21470,21471,21472,21473,21474,21475,21476,21478,21479,21480,21481,21482,21483,21484,21485,21486,21487,21488,21489,21490,21491,21492,21493,21494,21495,21496,21497,21498,21499,21500,21501,21502,21503,21504,21505,21506,21507,21508,21509,21510,21511,21512,21513,21514,21515,21516,21517,21518,21519,21520,21521,21522,21523,21524,21525,21526,21527,21528,21529,21530,21531,21532,21533,21534,21535,21536,21537,21538,21539,21540,21541,21542,21543,21544,21545,21546,21547,21548,21549,21550,21551,21552,21553,21554,21555,21556,21557,21558,21559,21560,21561,21562,21563,21564,21565,21566,21567,21568,21569,21570,21571,21572,21573,21574,21575,21576,21582,21583,21584,21585,21586,21587,21588,21589,21590,21591,21592,21593,21594,21595,21596,21604,21605,21606,21607,21618,21619,21620,21621,21622,21623,21624,21625,21626,21627,21629,21630,21631,21632,21633,21634,21635,21636,21637,21638,21639,21640,21641,21642,21643,21644,21645,21646,21647,21648,21649,21650,21651,21652,21653,21654,21655,21656,21657,21658,21659,21660,21661,21662,21663,21664,21665,21666,21667,21668,21669,21670,21671,21672,21673,21674,21675,21676,21677,21678,21679,21680,21681,21682,21683,21684,21687,21689,21691,21692,21693,21694,21695,21696,21697,21698,21699,21700,21701,21702,21703,21704,21705,21706,21719,21720,21723,21724,21725,21726,21727,21728,21729,21732,21733,21734,21735,21736,21738,21739,21740,21741,21742,21743,21744,21747,21764,21766,21767,21769,21770,21793,21796,21805,21818,21820,21821,21822,21824,21825,21826,21827,21828,21829,21830,21831,21832,21836,21837,21838,21839,21840,21841,21842,21843,21844,21845,21846,21847,21848,21849,21850,21851,21852,21853,21854,21855,21856,21857,21858,21859,21860,21861,21862,21863,21864,21865,21866,21867,21868,21869,21870,21871,21872,21873,21874,21875,21876,21877,21878,21879,21880,21881,21882,21887,21888,21891,21893,21895,21896,21899,21900,21901,21906,21907,21908,21925,21934,21939,21950,21955,21959,21963,21964,21967,21969,21970,21971,21972,21973,21974,22002,22004,22101,22118,22119,22128,22129,22130,22161,22162,22167,22176,22204,22211,22230,22231,22253,22255,22256,22257,22259,22260,22261,22262,22263,22264,22265,22271,22272,22273,22274,22275,22276,22277,22278,22301,22302,22303,22304,22305,22306,22307,22308,22309,22310,22313,22314,22315,22316,22322,22323,22324,22325,22327,22332,22335,22336,22337,22340,22347,22348,22349,22350,22351,22352,22353,22354,22355,22358,22360,22361,22362,22363,22364,22365,22366,22367,22368,22369,22370,22371,22372,22373,22374,22375,22376,22377,22378,22379,22380,22381,22384,22385,22386,22387,22388,22389,22390,22391,22393,22394,22395,22396,22397,22398,22399,22400,22401,22402,22403,22404,22405,22406,22407,22408,22409,22410,22411,22412,22413,22414,22415,22416,22417,22418,22419,22420,22421,22422,22423,22424,22425,22426,22427,22428,22429,22430,22431,22432,22433,22434,22435,22436,22437,22438,22439,22445,22446,22449,22450,22451,22452,22454,22455,22459,22460,22461,22462,22463,22464,22465,22466,22467,22468,22469,22470,22471,22472,22473,22474,22476,22480,22481,22482,22483,22484,22487,22488,22489,22490,22491,22492,22493,22494,22495,22496,22497,22498,22499,22504,22507,22508,22511,22512,22513,22517,22523,22531,22538,22539,22540,22541,22542,22545,22552,22553,22554,22555,22556,22557,22558,22559,22560,22561,22562,22563,22564,22565,22566,22567,22568,22569,22570,22571,22572,22573,22574,22580,22583,22584,22585,22586,22587,22588,22589,22590,22596,22597,22598,22602,22603,22605,22609,22612,22613,22614,22615,22616,22617,22620,22621,22627,22628,22629,22630,22631,22642,22646,22653,22659,22660,22661,22662,22663,22664,22665,22666,22667,22669,22671,22676,22679,22680,22681,22682,22683,22684,22685,22686,22687,22688,22689,22690,22691,22692,22693,22694,22695,22696,22697,22698,22699,22709,22710,22711,22716,22717,22719,22720,22723,22724,22729,22730,22731,22732,22733,22734,22735,22736,22737,22738,22739,22740,22741,22742,22743,22744,22745,22746,22747,22748,22749,22750,22751,22752,22753,22754,22755,22757,22758,22759,22760,22761,22762,22763,22764,22765,22766,22767,22768,22769,22770,22774,22775,22776,22777,22778,22779,22780,22781,22782,22783,22784,22785,22786,22787,22788,22789,22790,22791,22792,22793,22794,22795,22797,22800,22801,22802,22803,22804,22805,22806,22807,22808,22809,22810,22811,22812,22814,22815,22816,22817,22818,22819,22820,22821,22822,22824,22825,22826,22827,22828,22829,22830,22831,22832,22833,22834,22835,22836,22837,22838,22839,22840,22841,22842,22843,22844,22845,22846,22847,22848,22849,22850,22851,22852,22853,22854,22855,22856,22857,22858,22859,22860,22861,22862,22863,22864,22865,22866,22867,22868,22869,22870,22871,22872,22873,22874,22875,22876,22877,22879,22880,22881,22882,22883,22884,22885,22886,22887,22888,22889,22890,22891,22892,22893,22894,22895,22896,22897,22898,22899,22900,22901,22902,22903,22904,22905,22906,22907,22908,22909,22910,22911,22912,22913,22914,22915,22916,22917,22918,22919,22920,22921,22922,22923,22924,22925,22926,22927,22928,22929,22930,22931,22932,22933,22934,22935,22936,22937,22938,22939,22940,22941,22942,22943,22944,22945,22946,22947,22948,22949,22950,22951,22952,22953,22954,22955,22956,22957,22958,22959,22960,22961,22962,22963,22964,22965,22966,22967,22968,22969,22970,22971,22972,22973,22974,22975,22976,22977,22978,22979,22980,22981,22982,22983,22990,22991,22992,22993,22994,23002,23003,23020,23067,23084,23085,23086,23087,23088,23089,23090,23091,23092,23093,23094,23095,23096,23097,23098,23099,23100,23114,23128,23129,23130,23131,23132,23133,23134,23153,23154,23156,23157,23158,23159,23161,23162,23177,23178,23200,23224,23225,23226,23227,23228,23240,23243,23244,23266,23267,23268,23269,23290,23291,23292,23293,23295,23296,23297,23306,23334,23344,23345,23350,23370,23372,23377,23404,23422,23428,23431,23438,23439,23440,23441,23442,23443,23444,23446,23447,23448,23449,23450,23451,23452,23453,23458,23459,23466,23467,23468,23469,23470,23472,23473,23474,23475,23476,23477,23478,23479,23480,23487,23489,23494,23500,23503,23504,23506,23507,23513,23514,23515,23516,23517,23518,23519,23520,23521,23523,23525,23526,23527,23528,23529,23530,23531,23532,23533,23534,23535,23536,23537,23538,23539,23540,23541,23542,23543,23544,23545,23546,23547,23548,23557,23558,23559,23560,23574,23587,23588,23589,23590,23591,23592,23593,23594,23595,23596,23647,23648,23656,23659,23661,23662,23663,23664,23665,23666,23667,23668,23669,23670,23671,23672,23673,23674,23675,23676,23677,23678,23684,23687,23688,23689,23691,23692,23693,23707,23708,23709,23711,23714,23715,23716,23717,23718,23719,23720,23725,23726,23727,23728,23729,23730,23732,23735,23737,23738,23740,23748,23749,23750,23757,23758,23759,23760,23761,23762,23763,23764,23765,23766,23767,23768,23769,23775,23776,23777,23778,23779,23780,23781,23789,23790,23791,23792,23830,23834,23842,23870,23872,23899,23928,24010,24013,24019,24023,24028,24029,24030,24031,24032,24033,24034,24035,24036,24043,24058,24368,24371,24392,24393,24419,24420,24508,24509,24510,24511,24512,24513,24514,24515,24516,24517,24518,24519,24520,24521,24522,24523,24524,24525,24526,24527,24528,24529,24569,24615,24616,24620,24693,24720,24721,24725,24730,24731,24735,24745,24751,24752,24754,24755,24756,24757,24758,24761,24764,24785,24810,24811,24860,24890,24895,24897,24898,24913,24918,24919,24920,24921,24939,24942,24950,24959,24975,24979,24980,24981,24982,24983,24984,24985,24986,24987,24988,24989,24990,25002,25003,25004,25017,25032,25033,25034,25035,25036,25037,25038,25039,25040,25041,25042,25043,25044,25045,25046,25047,25048,25049,25050,25051,25052,25053,25054,25055,25069,25070,25073,25074,25075,25076,25081,25118,25122,25123,25124,25125,25159,25175,25182,25183,25184,25187,25207,25209,25210,25212,25219,25221,25229,25230,25231,25232,25233,25234,25235,25236,25265,25332,25335,25457,25519,25520,25521,25522,25523,25525,25544,25900,25901,25902,25958};

enum
{
    GOSSIP_TEXT_MODEL_VIEWER_MAIN = 50602
};

struct ModelViewerAI : public ScriptedAI
{
    ModelViewerAI(Creature* creature) : ScriptedAI(creature)
    {
        Reset();
    }

    bool autoCycle;
    uint32 savedAutoCycleTimer;
    uint32 autoCycleTimer;
    uint32 displayIdIndex;
    char* currentModelPath;

    void Reset() override
    {
        savedAutoCycleTimer = 5000;
        autoCycleTimer = 5000;
        autoCycle = true;
        SetModelPath(m_creature->GetDisplayId());
        displayIdIndex = 0;
        while (displayIdIndex < numDisplayIds && AllCreatureDisplayIds[displayIdIndex] != m_creature->GetDisplayId()) ++displayIdIndex;
    }
    
    void ToggleAutoCycle(bool enable)
    {
        autoCycle = enable;
        autoCycleTimer = enable ? savedAutoCycleTimer : 0;
    }

    void SetModelPath(uint32 displayId)
    {
        if (CreatureDisplayInfoEntry const* displayInfo = sCreatureDisplayInfoStore.LookupEntry(displayId))
            if (CreatureModelDataEntry const* modelData = sCreatureModelDataStore.LookupEntry(displayInfo->ModelId))
                currentModelPath = modelData->ModelPath;
    }

    void UpdateModel(uint32 displayId)
    {
        SetModelPath(displayId);
        m_creature->SetDisplayId(displayId);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (autoCycle)
        {
            if (autoCycleTimer < uiDiff)
            {
                displayIdIndex++;
                if (displayIdIndex == numDisplayIds)
                    displayIdIndex = 0;
                UpdateModel(AllCreatureDisplayIds[displayIdIndex]);
                autoCycleTimer = savedAutoCycleTimer;
            }
            else
                autoCycleTimer -= uiDiff;
        }
    }
};

bool GossipHello_modelviewer(Player* player, Creature* creature)
{
    if (ModelViewerAI* modelviewerAI = dynamic_cast<ModelViewerAI*>(creature->AI()))
    {
        std::string statusStr = "Current display ID - " + std::to_string(creature->GetDisplayId()) + " | " + modelviewerAI->currentModelPath;
        char const* status = statusStr.c_str();
        creature->MonsterWhisper(status, player);
        std::stringstream gossipOptionNext;
        gossipOptionNext << "Next display ID. (" << AllCreatureDisplayIds[modelviewerAI->displayIdIndex == numDisplayIds ? 0 : modelviewerAI->displayIdIndex + 1] << ")";
        std::stringstream gossipOptionPrev;
        gossipOptionPrev << "Previous display ID. (" << AllCreatureDisplayIds[modelviewerAI->displayIdIndex == 0 ? numDisplayIds - 1 : modelviewerAI->displayIdIndex - 1] << ")";
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, gossipOptionNext.str(), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, gossipOptionPrev.str(), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Random display ID.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
        if (!modelviewerAI->autoCycle)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Start display ID auto-cycle.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
        else
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Stop display ID auto-cycle.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, "Set display ID.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6, "", 0, true);
        player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, "Set auto-cycle timer (milliseconds).", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7, "", 0, true);
        player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, "Set scale.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8, "", 0, true);
        player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_BATTLE, "Set equip template entry.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9, "", 0, true);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Random equipment.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
        if (creature->GetMotionMaster()->GetCurrentMovementGeneratorType() != FOLLOW_MOTION_TYPE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Follow me.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
        if (creature->GetMotionMaster()->GetCurrentMovementGeneratorType() != IDLE_MOTION_TYPE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Hold still.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_MODEL_VIEWER_MAIN, creature->GetObjectGuid());
    }
    return true;
}

bool GossipSelectWithCode_modelviewer(Player* player, Creature* creature, uint32 sender, uint32 action, const char* sCode)
{
    if (ModelViewerAI* modelviewerAI = dynamic_cast<ModelViewerAI*>(creature->AI()))
    {
        uint32 code = std::stoi(sCode);
        std::stringstream responseMsg;
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF + 6:
            {
                uint32 index = 0;
                while (index < numDisplayIds && AllCreatureDisplayIds[index] != code) ++index;
                modelviewerAI->displayIdIndex = (index == numDisplayIds ? 0 : index);
                modelviewerAI->UpdateModel(AllCreatureDisplayIds[index]);
                break;
            }
            case GOSSIP_ACTION_INFO_DEF + 7:
            {
                modelviewerAI->savedAutoCycleTimer = code;
                responseMsg << "Set auto-cycle timer to " << code << " milliseconds. Auto-cycle is currently " << (modelviewerAI->autoCycle ? "enabled." : "disabled.");
                std::string responseMsgStr = responseMsg.str();
                const char* responseMsgChar = responseMsgStr.c_str();
                creature->MonsterWhisper(responseMsgChar, player);
                break;
            }
            case GOSSIP_ACTION_INFO_DEF + 8:
            {
                responseMsg << "Set scale to " << code << ".";
                std::string responseMsgStr = responseMsg.str();
                const char* responseMsgChar = responseMsgStr.c_str();
                creature->MonsterWhisper(responseMsgChar, player);
                creature->SetObjectScale(code);
                break;
            }
            case GOSSIP_ACTION_INFO_DEF + 9:
            {
                responseMsg << "Set equipment template to " << code << ".";
                std::string responseMsgStr = responseMsg.str();
                const char* responseMsgChar = responseMsgStr.c_str();
                creature->MonsterWhisper(responseMsgChar, player);
                creature->LoadEquipment(code);
                break;
            }
        }
    }
    player->CLOSE_GOSSIP_MENU();
    GossipHello_modelviewer(player, creature);
    return true;
}

bool GossipSelect_modelviewer(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 action)
{
    if (ModelViewerAI* modelviewerAI = dynamic_cast<ModelViewerAI*>(creature->AI()))
    {
        std::stringstream responseMsg;
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1: // Next
            {
                modelviewerAI->displayIdIndex = modelviewerAI->displayIdIndex == numDisplayIds ? 0 : modelviewerAI->displayIdIndex + 1;
                modelviewerAI->UpdateModel(AllCreatureDisplayIds[modelviewerAI->displayIdIndex]);
                break;
            }
            case GOSSIP_ACTION_INFO_DEF + 2: // Previous
            {
                modelviewerAI->displayIdIndex = modelviewerAI->displayIdIndex == 0 ? numDisplayIds - 1 : modelviewerAI->displayIdIndex - 1;
                modelviewerAI->UpdateModel(AllCreatureDisplayIds[modelviewerAI->displayIdIndex]);
                break;
            }
            case GOSSIP_ACTION_INFO_DEF + 3: // Random
            {
                uint32 newIndex = urand(0, numDisplayIds);
                modelviewerAI->UpdateModel(AllCreatureDisplayIds[newIndex]);
                modelviewerAI->displayIdIndex = newIndex;
                break;
            }
            case GOSSIP_ACTION_INFO_DEF + 4:
            {
                responseMsg << "Enabling auto-cycle. Timer is currently set to " << modelviewerAI->savedAutoCycleTimer << " milliseconds.";
                std::string responseMsgStr = responseMsg.str();
                const char* responseMsgChar = responseMsgStr.c_str();
                creature->MonsterWhisper(responseMsgChar, player);
                modelviewerAI->ToggleAutoCycle(true);
                break;
            }
            case GOSSIP_ACTION_INFO_DEF + 5:
            {
                creature->MonsterWhisper("Disabling auto-cycle.", player);
                modelviewerAI->ToggleAutoCycle(false);
                break;
            }
            case GOSSIP_ACTION_INFO_DEF + 10:
            {
                std::set<uint32> const& equipmentTemplates = sObjectMgr.GetEquipmentTemplateEntrySet();
                int idx = rand() % equipmentTemplates.size();
                auto it = equipmentTemplates.begin();
                for (int i = 0; i < idx; i++) it++;
                creature->LoadEquipment(*it);
                responseMsg << "Set equipment template to " << *it << ".";
                std::string responseMsgStr = responseMsg.str();
                const char* responseMsgChar = responseMsgStr.c_str();
                creature->MonsterWhisper(responseMsgChar, player);
                break;
            }
            case GOSSIP_ACTION_INFO_DEF + 11:
            {
                creature->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
                creature->MonsterSay("Okay, following $N.", LANG_UNIVERSAL, player);
                break;
            }
            case GOSSIP_ACTION_INFO_DEF + 12:
            {
                creature->GetMotionMaster()->MoveIdle();
                creature->MonsterSay("I'll stay put here.", LANG_UNIVERSAL);
                break;
            }
        }
    }
    player->CLOSE_GOSSIP_MENU();
    GossipHello_modelviewer(player, creature);
    return true;
}

enum
{
    TEXT_ID_COIN                    = 50612,
    TEXT_ID_MODEL_DAEMON            = 50613,
    TEXT_ID_COIN2                   = 50614,
    TEXT_ID_COIN3                   = 50615,
    NPC_MODEL_DAEMON                = 980049,
};

struct Location2DPoint
{
    float x, y;
};
static const Location2DPoint southWestCorner = { -145.6f, 145.6f };
static const Location2DPoint northEastCorner = { 145.6f, -145.6f };
GuidList infinityCoinSummons;
bool creatureModelMuseumEnabled = false;
uint32 leftOffDisplayId = 0;
float spacing = 10.f;
float minSpacing = 4.f;
float testMapFloorZ = -144.7f;

bool GossipItemUse_infinity_coin(Player* player, Item* item, const SpellCastTargets& /*pTargets*/)
{
    player->GetPlayerMenu()->ClearMenus();
    if (player->GetMapId() == 13)
    {
        if (!creatureModelMuseumEnabled)
            player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, "Spawn creature model museum", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1, "WARNING - Incurs heavy load, only use this if you know what you're doing!", 0, false);
        else
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, "Despawn creature model museum", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, "Perform emote", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3, "Input EmoteId to perform on next popup and click Accept. Max 333 Vanilla | 423 TBC | 476 WotLK", 0, true);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Set stand state", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Movement & positioning", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
        }
        std::stringstream msg;
        msg << "Set spacing on spawn (Current: " << spacing << " yds)";
        player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_BATTLE, msg.str(), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4, "Input space in yards on next popup and click Accept. Minimum value is 4 for client stability.", 0, true);
    }
    player->SEND_GOSSIP_MENU(TEXT_ID_COIN, item->GetObjectGuid());
    return true;
}

void CleanUpSpawns(Player* player)
{
    for (GuidList::const_iterator itr = infinityCoinSummons.begin(); itr != infinityCoinSummons.end(); ++itr)
    {
        if (Creature* summoned = player->GetMap()->GetCreature(*itr))
        {
            summoned->ForcedDespawn();
            summoned->RemoveFromWorld();
        }
    }
    infinityCoinSummons.clear();
}

void MoveSpawns(Player* player, Item* item, uint32 sender, uint32 action)
{
    for (GuidList::const_iterator itr = infinityCoinSummons.begin(); itr != infinityCoinSummons.end(); ++itr)
    {
        if (Creature* summoned = player->GetMap()->GetCreature(*itr))
        {
            switch (action)
            {
                case GOSSIP_ACTION_INFO_DEF + 6:
                {
                    summoned->GetMotionMaster()->Clear();
                    summoned->GetMotionMaster()->MovePoint(0, summoned->GetRespawnPosition(), FORCED_MOVEMENT_RUN, 0.f, true);
                    break;
                }
                case GOSSIP_ACTION_INFO_DEF + 7:
                {
                    summoned->GetMotionMaster()->MovePoint(1, 0, 0, testMapFloorZ);
                    break;
                }
                case GOSSIP_ACTION_INFO_DEF + 8:
                {
                    summoned->GetMotionMaster()->MoveRandomAroundPoint(summoned->GetPositionX(), summoned->GetPositionY(), testMapFloorZ, 5.f);
                    break;
                }
                case GOSSIP_ACTION_INFO_DEF + 9:
                {
                    summoned->GetMotionMaster()->MoveRandomAroundPoint(summoned->GetPositionX(), summoned->GetPositionY(), testMapFloorZ, 30.f);
                    break;
                }
                case GOSSIP_ACTION_INFO_DEF + 10:
                {
                    summoned->GetMotionMaster()->MoveFollow(player, frand(0.f, 40.f), frand(-3.14159f, 3.14159f));
                    break;
                }
                case GOSSIP_ACTION_INFO_DEF + 11:
                {
                    summoned->GetMotionMaster()->MoveIdle();
                    break;
                }
            }
        }
    }
}

bool GossipSelectItem_infinity_coin(Player* player, Item* item, uint32 sender, uint32 action)
{
    switch (action)
    {
        // Enable Creature Model Museum
        case GOSSIP_ACTION_INFO_DEF + 1:
        {
            CleanUpSpawns(player);
            creatureModelMuseumEnabled = true;
            float currX = southWestCorner.x + spacing;
            float currY = southWestCorner.y - spacing;
            bool ranOutOfSpace = false;
            uint32 counter = 0;

            for (auto displayId : AllCreatureDisplayIds)
            {
                if (leftOffDisplayId > 0 && displayId <= leftOffDisplayId)
                    continue;
                counter++;
                if (Creature* spawn = player->SummonCreature(NPC_MODEL_DAEMON, currX, currY, testMapFloorZ, 0, TEMPSPAWN_MANUAL_DESPAWN, 60000, true, true, 0, 0, displayId))
                {
                    infinityCoinSummons.push_back(spawn->GetObjectGuid());
                    spawn->SetNativeDisplayId(displayId);
                }
                currX += spacing;
                if (currX >= northEastCorner.x - spacing)
                {
                    currX = southWestCorner.x + spacing;
                    currY -= spacing;
                }
                if (currY <= northEastCorner.y + spacing)
                {
                    std::stringstream msg;
                    msg << "Spawned " << counter << " before runnning out of space! Left off on DisplayId " << displayId << ".";
                    player->Say(msg.str(), LANG_UNIVERSAL);
                    leftOffDisplayId = displayId;
                    ranOutOfSpace = true;
                    break;
                }
            }
            if (!ranOutOfSpace)
            {
                std::stringstream msg;
                msg << "Spawned " << counter << "! Reached end of available models.";
                player->Say(msg.str(), LANG_UNIVERSAL);
                leftOffDisplayId = 0;
            }

            player->CLOSE_GOSSIP_MENU();
            SpellCastTargets target;
            GossipItemUse_infinity_coin(player, item, target);
            break;
        }
        // Disable Creature Model Museum
        case GOSSIP_ACTION_INFO_DEF + 2:
        {
            CleanUpSpawns(player);
            creatureModelMuseumEnabled = false;
            player->CLOSE_GOSSIP_MENU();
            SpellCastTargets target;
            GossipItemUse_infinity_coin(player, item, target);
            break;
        }
        // Movement & Positioning
        case GOSSIP_ACTION_INFO_DEF + 5:
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Main Menu", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 99);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Go home", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
            player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_BATTLE, "Singularity", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7, "WARNING: Frame rate may drop significantly if you stare too deeply into the abyss. Continue?", 0, false);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Random 5 yds", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Random 30 yds", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Follow", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Idle", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
            player->SEND_GOSSIP_MENU(TEXT_ID_COIN2, item->GetObjectGuid());
            break;
        }
        // Main Menu
        case GOSSIP_ACTION_INFO_DEF + 99:
        {
            player->CLOSE_GOSSIP_MENU();
            SpellCastTargets target;
            GossipItemUse_infinity_coin(player, item, target);
            break;
        }
    }

    if (action >= GOSSIP_ACTION_INFO_DEF + 6 && action <= GOSSIP_ACTION_INFO_DEF + 11)
    {
        MoveSpawns(player, item, sender, action);
        player->CLOSE_GOSSIP_MENU();
        bool retVal = GossipSelectItem_infinity_coin(player, item, sender, GOSSIP_ACTION_INFO_DEF + 5);
    }
    else if (action == GOSSIP_ACTION_INFO_DEF + 12)
    {
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Main Menu", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 99);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Stand", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 13);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Sit", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 14);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Sit Chair", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 15);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Sleep", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 16);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Sit Low Chair", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 17);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Sit Medium Chair", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 18);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Sit High Chair", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 19);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Dead", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 20);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Kneel", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 21);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Custom", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 22);
        player->SEND_GOSSIP_MENU(TEXT_ID_COIN3, item->GetObjectGuid());
    }
    else if (action >= GOSSIP_ACTION_INFO_DEF + 13 && action <= GOSSIP_ACTION_INFO_DEF + 22)
    {
        for (GuidList::const_iterator itr = infinityCoinSummons.begin(); itr != infinityCoinSummons.end(); ++itr)
        {
            if (Creature* summoned = player->GetMap()->GetCreature(*itr))
                summoned->SetStandState(UnitStandStateType(action - GOSSIP_ACTION_INFO_DEF - 13));
        }
        player->CLOSE_GOSSIP_MENU();
        bool retVal = GossipSelectItem_infinity_coin(player, item, sender, GOSSIP_ACTION_INFO_DEF + 12);
    }

    return true;
}

bool GossipSelectItemWithCode_infinity_coin(Player* player, Item* item, uint32 sender, uint32 action, const char* sCode)
{
    switch (action)
    {
        case GOSSIP_ACTION_INFO_DEF + 3:
        {
            uint32 code = std::stoi(sCode);
            if (code <= 0 || code > 476)
                code = EMOTE_STATE_DANCE;
            for (GuidList::const_iterator itr = infinityCoinSummons.begin(); itr != infinityCoinSummons.end(); ++itr)
            {
                if (Creature* summoned = player->GetMap()->GetCreature(*itr))
                    summoned->HandleEmote(code);
            }
            player->CLOSE_GOSSIP_MENU();
            SpellCastTargets target;
            GossipItemUse_infinity_coin(player, item, target);
            break;
        }
        case GOSSIP_ACTION_INFO_DEF + 4:
        {
            float code = std::stof(sCode);
            if (code < minSpacing)
                code = minSpacing;
            spacing = code;
            player->CLOSE_GOSSIP_MENU();
            SpellCastTargets target;
            GossipItemUse_infinity_coin(player, item, target);
            break;
        }
    }
    return true;
}

bool GossipHello_modelviewer_daemon(Player* player, Creature* creature)
{
    if (CreatureDisplayInfoEntry const* displayInfo = sCreatureDisplayInfoStore.LookupEntry(creature->GetDisplayId()))
    {
        if (CreatureModelDataEntry const* modelData = sCreatureModelDataStore.LookupEntry(displayInfo->ModelId))
        {
            std::stringstream showData1;
            showData1 << "DisplayId: " << displayInfo->Displayid;
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, showData1.str(), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            std::stringstream showData2;
            showData2 << "ModelId: " << displayInfo->ModelId;
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, showData2.str(), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, modelData->ModelPath, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            std::stringstream showData3;
            showData3 << "DispScale: " << displayInfo->scale << " | ModelScale: " << modelData->Scale;
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, showData3.str(), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            std::stringstream showData4;
            showData4 << "ColHeight: " << modelData->CollisionHeight << " | ColWidth: " << modelData->CollisionWidth;
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, showData4.str(), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            std::stringstream showData5;
            showData5 << "Flags: " << modelData->Flags;
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, showData5.str(), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
            std::stringstream showData6;
            showData6 << "MountHeight: " << modelData->MountHeight;
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, showData6.str(), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
            player->SEND_GOSSIP_MENU(TEXT_ID_MODEL_DAEMON, creature->GetObjectGuid());
        }
    }
    return true;
}

enum
{
    GOSSIP_TEXT_REINFORCEMENTS_SPAWNER_MAIN     = 50603,
    GOSSIP_TEXT_REINFORCEMENTS_SPAWNER_STARTED  = 50604,
    GOSSIP_TEXT_REINFORCEMENTS_SPAWNER_EXPLAIN  = 50605,
};

bool GossipHello_crater_reinforcements_spawner(Player* player, Creature* creature)
{
    if (instance_azshara_crater* instance = (instance_azshara_crater*)creature->GetInstanceData())
    {
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "What is this place? How do I help?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        if (instance->GetData(TYPE_BATTLE_FOR_THE_CRATER) == IN_PROGRESS)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Stop Battle for the Crater", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            player->SEND_GOSSIP_MENU(GOSSIP_TEXT_REINFORCEMENTS_SPAWNER_STARTED, creature->GetObjectGuid());
        }
        else
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Begin Battle for the Crater", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            player->SEND_GOSSIP_MENU(GOSSIP_TEXT_REINFORCEMENTS_SPAWNER_MAIN, creature->GetObjectGuid());
        }
    }
    return true;
}

bool GossipSelect_crater_reinforcements_spawner(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 action)
{
    instance_azshara_crater* instance = (instance_azshara_crater*)creature->GetInstanceData();
    if (instance)
    {
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1: // Info
            {
                player->SEND_GOSSIP_MENU(GOSSIP_TEXT_REINFORCEMENTS_SPAWNER_EXPLAIN, creature->GetObjectGuid());
                break;
            }
            case GOSSIP_ACTION_INFO_DEF + 2: // Stop Battle
            {
                instance->SetData(TYPE_BATTLE_FOR_THE_CRATER, FAIL);
                player->CLOSE_GOSSIP_MENU();
                break;
            }
            case GOSSIP_ACTION_INFO_DEF + 3: // Start Battle
            {
                instance->SetData(TYPE_BATTLE_FOR_THE_CRATER, IN_PROGRESS);
                player->CLOSE_GOSSIP_MENU();
                break;
            }
        }
    }
    return true;
}

void AddSC_custom_main()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "plastic_surgeon";
    newscript->pGossipHello = &GossipHello_plastic_surgeon;
    newscript->pGossipSelect = &GossipSelect_plastic_surgeon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "appearance_copier";
    newscript->pGossipHello = &GossipHello_appearance_copier;
    newscript->pGossipSelectWithCode = &GossipSelectWithCode_appearance_copier;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "guildcreator";
    newscript->pGossipHello = &GossipHello_guildcreator;
    newscript->pGossipSelect = &GossipSelect_guildcreator;
    newscript->pGossipSelectWithCode = &GossipSelectWithCode_guildcreator;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_target_dummy";
    newscript->GetAI = &GetNewAIInstance<npc_target_dummyAI>;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "paymaster";
    newscript->pGossipHello = &GossipHello_paymaster;
    newscript->pGossipSelect = &GossipSelect_paymaster;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "lockoutnullifier";
    newscript->pGossipHello = &GossipHello_lockoutnullifier;
    newscript->pGossipSelect = &GossipSelect_lockoutnullifier;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_enlistment_officer";
    newscript->GetAI = &GetNewAIInstance<GreeterAI>;
    newscript->pGossipHello = &GossipHello_npc_enlistment_officer;
    newscript->pGossipSelect = &GossipSelect_npc_enlistment_officer;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_test_realm_overlord";
    newscript->GetAI = &GetNewAIInstance<GreeterAI>;
    newscript->pGossipHello = &GossipHello_npc_test_realm_overlord;
    newscript->pGossipSelect = &GossipSelect_npc_test_realm_overlord;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_booster";
    newscript->GetAI = &GetNewAIInstance<GreeterAI>;
    newscript->pGossipHello = &GossipHello_npc_booster;
    newscript->pGossipSelect = &GossipSelect_npc_booster;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "modelviewer";
    newscript->GetAI = &GetNewAIInstance<ModelViewerAI>;
    newscript->pGossipHello = &GossipHello_modelviewer;
    newscript->pGossipSelect = &GossipSelect_modelviewer;
    newscript->pGossipSelectWithCode = &GossipSelectWithCode_modelviewer;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "item_infinity_coin";
    newscript->pItemUse = &GossipItemUse_infinity_coin;
    newscript->pGossipSelectItem = &GossipSelectItem_infinity_coin;
    newscript->pGossipSelectItemWithCode = &GossipSelectItemWithCode_infinity_coin;
    newscript->RegisterSelf(false);

    newscript = new Script;
    newscript->Name = "crater_reinforcements_spawner";
    newscript->pGossipHello = &GossipHello_crater_reinforcements_spawner;
    newscript->pGossipSelect = &GossipSelect_crater_reinforcements_spawner;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "modelviewer_daemon";
    newscript->pGossipHello = &GossipHello_modelviewer_daemon;
    newscript->RegisterSelf();
}
