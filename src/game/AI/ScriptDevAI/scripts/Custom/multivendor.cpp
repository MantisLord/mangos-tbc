// Multivendor NPC
// Smolderforge 2015-2018
// PvP Henhouse 2018-2020
// Author: Henhouse

#include "AI/ScriptDevAI/include/sc_gossip.h"

// Multivendor
bool GossipHello_multivendor(Player* player, Creature* creature)
{
    if (creature->GetCreatureInfo()->ExtraFlags & CREATURE_EXTRA_FLAG_CUSTOM_VENDOR)
    {
        switch (creature->GetEntry())
        {
            case 980003: // Weapons vendor
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "One-Handed Weapons", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Two-Handed Weapons", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Ranged Weapons", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Caster Weapons", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 28);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Wands", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 13);
                player->SEND_GOSSIP_MENU(42305, creature->GetObjectGuid());
                break;
            case 980004: // Off-hands vendor
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Shields", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 14);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Off-hands", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 15);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Relics", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 62);
                player->SEND_GOSSIP_MENU(42305, creature->GetObjectGuid());
                break;
            case 980006: // Jewelry vendor
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Necklaces", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+16);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Rings", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+18);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Trinkets", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+19);
                player->SEND_GOSSIP_MENU(42305, creature->GetObjectGuid());
                break;
            case 980005: // Off-sets
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Helmets", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 20);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Shoulders", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 21);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Cloaks", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 17);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Chest", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 22);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Bracers", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 23);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Gloves", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 24);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Belts", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 25);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Legs", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 26);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Boots", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 27);
                player->SEND_GOSSIP_MENU(42305, creature->GetObjectGuid());
                break;
            case 980009: // Demon Trainer vendor
                if (player->getClass() == CLASS_WARLOCK)
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Imp", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 47);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Voidwalker", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 48);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Succubus", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 49);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Felhunter", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 50);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Felguard", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 51);
                    player->SEND_GOSSIP_MENU(42305, creature->GetObjectGuid());
                }
                else
                {
                    player->SEND_GOSSIP_MENU(42311, creature->GetObjectGuid());
                }
                break;
            case 980013: // Arena gear vendor
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Gear", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 52);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Off-set", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 53);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Weapons & Off-hands", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 54);
                player->SEND_GOSSIP_MENU(42305, creature->GetObjectGuid());
                break;
            case 980015: // Gem Vendor
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Meta Gems", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 55);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Red Gems", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 56);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Yellow Gems", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 57);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Blue Gems", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 58);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Orange Gems", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 59);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Green Gems", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 60);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Purple Gems", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 61);
                player->SEND_GOSSIP_MENU(42305, creature->GetObjectGuid());
                break;
            case 980017: // General Goods Vendor
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Basic Necessities", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 63);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Stat Buff Food", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 64);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Flasks, Elixirs, & Potions", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 65);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Equip Enhancers, Gadgets, & Misc Consumes", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 66);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Crafting Materials", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 67);
                player->SEND_GOSSIP_MENU(42305, creature->GetObjectGuid());
                break;
            default:
                player->SEND_GOSSIP_MENU(42311, creature->GetObjectGuid());
                break;
        }
    }

    return true;
}

bool GossipSelect_multivendor(Player *player, Creature *creature, uint32 sender, uint32 action)
{
    uint32 vendorEntryId = 0;
    switch (action)
    {
        // ***** WEAPONS VENDOR  ****** //
        case GOSSIP_ACTION_INFO_DEF + 10: // 1h
            vendorEntryId = 185000;
            break;
        case GOSSIP_ACTION_INFO_DEF + 11: // 2h
            vendorEntryId = 185001;
            break;
        case GOSSIP_ACTION_INFO_DEF + 12: // ranged
            vendorEntryId = 185002;
            break;
        case GOSSIP_ACTION_INFO_DEF + 28: // caster weapons
            vendorEntryId = 185018;
            break;
        case GOSSIP_ACTION_INFO_DEF + 13: // wands
            vendorEntryId = 185003;
            break;

        // ***** OFF-HANDS VENDOR  ****** //
        case GOSSIP_ACTION_INFO_DEF + 14: // shields
            vendorEntryId = 185004;
            break;
        case GOSSIP_ACTION_INFO_DEF + 15: // off-hands
            vendorEntryId = 185005;
            break;
        case GOSSIP_ACTION_INFO_DEF + 62: // relics
            vendorEntryId = 185034;
            break;

        // ***** JEWELRY VENDOR  ****** //
        case GOSSIP_ACTION_INFO_DEF + 16: // necks
            vendorEntryId = 185006;
            break;
        case GOSSIP_ACTION_INFO_DEF + 17: // cloaks
            vendorEntryId = 185007;
            break;
        case GOSSIP_ACTION_INFO_DEF + 18: // rings
            vendorEntryId = 185008;
            break;
        case GOSSIP_ACTION_INFO_DEF + 19: // trinkets
            vendorEntryId = 185009;
            break;

        // ***** OFFSET VENDOR  ****** //
        case GOSSIP_ACTION_INFO_DEF + 20: // helmets
            vendorEntryId = 185010;
            break;
        case GOSSIP_ACTION_INFO_DEF + 21: // shoulders
            vendorEntryId = 185011;
            break;
        case GOSSIP_ACTION_INFO_DEF + 22: // chest
            vendorEntryId = 185012;
            break;
        case GOSSIP_ACTION_INFO_DEF + 23: // bracers
            vendorEntryId = 185013;
            break;
        case GOSSIP_ACTION_INFO_DEF + 24: // gloves
            vendorEntryId = 185014;
            break;
        case GOSSIP_ACTION_INFO_DEF + 25: // belts
            vendorEntryId = 185015;
            break;
        case GOSSIP_ACTION_INFO_DEF + 26: // legs
            vendorEntryId = 185016;
            break;
        case GOSSIP_ACTION_INFO_DEF + 27: // boots
            vendorEntryId = 185017;
            break;


        // ***** DEMON TRAINER VENDOR  ****** //
        case GOSSIP_ACTION_INFO_DEF+47: // Imp
            vendorEntryId = 185029;
            break;
        case GOSSIP_ACTION_INFO_DEF+48: // Voidwalker
            vendorEntryId = 185030;
            break;
        case GOSSIP_ACTION_INFO_DEF+49: // Succubus
            vendorEntryId = 185031;
            break;
        case GOSSIP_ACTION_INFO_DEF+50: // Felhunter
            vendorEntryId = 185032;
            break;
        case GOSSIP_ACTION_INFO_DEF+51: // Felguard
            vendorEntryId = 185033;
            break;

        // ***** ARENA GEAR VENDOR  ****** //
        case GOSSIP_ACTION_INFO_DEF + 52: // Gear
            vendorEntryId = 185019;
            break;
        case GOSSIP_ACTION_INFO_DEF + 53: // Off-set
            vendorEntryId = 185020;
            break;
        case GOSSIP_ACTION_INFO_DEF + 54: // Weapons & Off-sets
            vendorEntryId = 185021;
            break;

        // ***** GEM VENDOR  ****** //
        case GOSSIP_ACTION_INFO_DEF + 55: // Meta gems
            vendorEntryId = 185022;
            break;
        case GOSSIP_ACTION_INFO_DEF + 56: // Red gems
            vendorEntryId = 185023;
            break;
        case GOSSIP_ACTION_INFO_DEF + 57: // Yellow gems
            vendorEntryId = 185024;
            break;
        case GOSSIP_ACTION_INFO_DEF + 58: // Blue gems
            vendorEntryId = 185025;
            break;
        case GOSSIP_ACTION_INFO_DEF + 59: // Orange gems
            vendorEntryId = 185026;
            break;
        case GOSSIP_ACTION_INFO_DEF + 60: // Green gems
            vendorEntryId = 185027;
            break;
        case GOSSIP_ACTION_INFO_DEF + 61: // Purple gems
            vendorEntryId = 185028;
            break;

        // ***** GENERAL GOODS VENDOR  ****** //
        case GOSSIP_ACTION_INFO_DEF + 63: // Basic Necessities
            vendorEntryId = 185035;
            break;
        case GOSSIP_ACTION_INFO_DEF + 64: // Stat Buff Food
            vendorEntryId = 185036;
            break;
        case GOSSIP_ACTION_INFO_DEF + 65: // Flasks, Elixirs, & Potions
            vendorEntryId = 185037;
            break;
        case GOSSIP_ACTION_INFO_DEF + 66: // Equip Enhancers, Gadgets, & Misc Consumes
            vendorEntryId = 185038;
            break;
        case GOSSIP_ACTION_INFO_DEF + 67: // Crafting Materials
            vendorEntryId = 185039;
            break;

        case GOSSIP_ACTION_INFO_DEF+100: // Weapons One-Handed submenu
            player->GetPlayerMenu()->ClearMenus();
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Swords", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+18);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Axes", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+19);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Maces", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+20);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Daggers", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+21);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Fist Weapons", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+22);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "« Back", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+0);
            player->SEND_GOSSIP_MENU(42305, creature->GetObjectGuid());
            return true; // return or it'll try to open a menu

        case GOSSIP_ACTION_INFO_DEF+200: // Weapons Two-Handed submenu
            player->GetPlayerMenu()->ClearMenus();
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Swords", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+35);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Axes", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+36);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Maces", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+37);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Polearms", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+38);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Staves", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+39);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "« Back", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+0);
            player->SEND_GOSSIP_MENU(42305, creature->GetObjectGuid());
            return true; // return or it'll try to open a menu

        case GOSSIP_ACTION_TRAIN: // Train
            player->GetSession()->SendTrainerList(creature->GetObjectGuid());
            return true;

        default: // return to main menu
            GossipHello_multivendor(player, creature);
            return true;
    }

    if (vendorEntryId)
    {
        player->m_currentVendorEntry = vendorEntryId;
        player->GetSession()->SendListInventory(creature->GetObjectGuid(), vendorEntryId);
    }

    return true;
}

void AddSC_multivendor()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "multivendor";
    newscript->pGossipHello = &GossipHello_multivendor;
    newscript->pGossipSelect = &GossipSelect_multivendor;
    newscript->RegisterSelf();
}
