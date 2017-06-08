enum
{
    GOSSIP_TEXT_CRYSTAL     = 50400,
    GOSSIP_TEXT_CRYSTAL_2   = 50399,
};

bool GossipHello_enchantment_crystal(Player* pPlayer, Creature* pCreature)
{
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Head", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Shoulder", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Cloak", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Chest", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Wrist", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Hands", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Legs", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Feet", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Ring", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Ring2", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "2H Weapon", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Mainhand Weapon", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Offhand", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 14);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Ranged", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 16);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Shield", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 17);

    pPlayer->GetPlayerMenu()->SendGossipMenu(GOSSIP_TEXT_CRYSTAL, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_enchantment_crystal(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "34 attack power 16 hit", EQUIPMENT_SLOT_HEAD, 35452);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "17 strength 16 intellect", EQUIPMENT_SLOT_HEAD, 37891);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "22 spell damage 14 spell hit", EQUIPMENT_SLOT_HEAD, 35447);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "35 healing 12 spell damage 7 mp5", EQUIPMENT_SLOT_HEAD, 35445);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "18 stamina 20 resilience", EQUIPMENT_SLOT_HEAD, 35453);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "16 defense 17 dodge", EQUIPMENT_SLOT_HEAD, 35443);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "20 Fire resistance", EQUIPMENT_SLOT_HEAD, 35456);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "20 Arcane resistance", EQUIPMENT_SLOT_HEAD, 35455);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "20 Shadow resistance", EQUIPMENT_SLOT_HEAD, 35458);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "20 Nature resistance", EQUIPMENT_SLOT_HEAD, 35454);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "20 Frost resistance", EQUIPMENT_SLOT_HEAD, 35457);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "8 all resistance", EQUIPMENT_SLOT_HEAD, 37889);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Main Menu", 0, 0);
        pPlayer->GetPlayerMenu()->SendGossipMenu(GOSSIP_TEXT_CRYSTAL_2, pCreature->GetObjectGuid());
    }
    else if (uiAction == GOSSIP_ACTION_INFO_DEF + 2)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "30 attack power 10 crit", EQUIPMENT_SLOT_SHOULDERS, 35417);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "26 attack power 14 crit", EQUIPMENT_SLOT_SHOULDERS, 29483);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "20 attack power 15 crit", EQUIPMENT_SLOT_SHOULDERS, 35439);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "18 spell damage 10 spell crit", EQUIPMENT_SLOT_SHOULDERS, 35406);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "15 spell damage 14 spell crit", EQUIPMENT_SLOT_SHOULDERS, 29467);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "12 spell damage 15 spell crit", EQUIPMENT_SLOT_SHOULDERS, 35437);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "33 healing 11 spell damage 4 mp5", EQUIPMENT_SLOT_SHOULDERS, 35404);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "31 healing 11 spell damage 5 mp5", EQUIPMENT_SLOT_SHOULDERS, 29475);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "22 healing 6 mp5", EQUIPMENT_SLOT_SHOULDERS, 35435);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "16 stamina 100 armor", EQUIPMENT_SLOT_SHOULDERS, 29480);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "10 dodge 15 defense", EQUIPMENT_SLOT_SHOULDERS, 35433);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "15 dodge 10 defense", EQUIPMENT_SLOT_SHOULDERS, 35402);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Main Menu", 0, 0);
        pPlayer->GetPlayerMenu()->SendGossipMenu(GOSSIP_TEXT_CRYSTAL_2, pCreature->GetObjectGuid());
    }
    else if (uiAction == GOSSIP_ACTION_INFO_DEF + 3)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "12 agility", EQUIPMENT_SLOT_BACK, 34004);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "20 spell penetration", EQUIPMENT_SLOT_BACK, 34003);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "15 Fire resistance", EQUIPMENT_SLOT_BACK, 25081);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "15 Arcane resistance", EQUIPMENT_SLOT_BACK, 34005);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "15 Shadow resistance", EQUIPMENT_SLOT_BACK, 34006);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "15 Nature resistance", EQUIPMENT_SLOT_BACK, 25082);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "7 all resistance", EQUIPMENT_SLOT_BACK, 27962);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "12 dodge", EQUIPMENT_SLOT_BACK, 25086);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "12 defense", EQUIPMENT_SLOT_BACK, 47051);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "120 armor", EQUIPMENT_SLOT_BACK, 27961);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "increase stealth", EQUIPMENT_SLOT_BACK, 25083);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "decrease threat by 2%", EQUIPMENT_SLOT_BACK, 25084);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Main Menu", 0, 0);
        pPlayer->GetPlayerMenu()->SendGossipMenu(GOSSIP_TEXT_CRYSTAL_2, pCreature->GetObjectGuid());
    }
    else if (uiAction == GOSSIP_ACTION_INFO_DEF + 4)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "15 resilience", EQUIPMENT_SLOT_CHEST, 33992);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "6 to all stats", EQUIPMENT_SLOT_CHEST, 27960);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "15 spirit", EQUIPMENT_SLOT_CHEST, 33990);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "15 defense", EQUIPMENT_SLOT_CHEST, 46594);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "150 hp", EQUIPMENT_SLOT_CHEST, 27957);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "150 Mana", EQUIPMENT_SLOT_CHEST, 27958);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Main Menu", 0, 0);
        pPlayer->GetPlayerMenu()->SendGossipMenu(GOSSIP_TEXT_CRYSTAL_2, pCreature->GetObjectGuid());
    }
    else if (uiAction == GOSSIP_ACTION_INFO_DEF + 5)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "24 attack power", EQUIPMENT_SLOT_WRISTS, 34002);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "15 spell damage", EQUIPMENT_SLOT_WRISTS, 27917);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "30 healing 10 spell damage", EQUIPMENT_SLOT_WRISTS, 27911);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "12 stamina", EQUIPMENT_SLOT_WRISTS, 27914);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "12 defense", EQUIPMENT_SLOT_WRISTS, 27906);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "12 strength", EQUIPMENT_SLOT_WRISTS, 27899);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "12 intellect ", EQUIPMENT_SLOT_WRISTS, 34001);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "4 to all stats", EQUIPMENT_SLOT_WRISTS, 27905);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "6 mp5", EQUIPMENT_SLOT_WRISTS, 27913);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "9 spirit", EQUIPMENT_SLOT_WRISTS, 20009);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Main Menu", 0, 0);
        pPlayer->GetPlayerMenu()->SendGossipMenu(GOSSIP_TEXT_CRYSTAL_2, pCreature->GetObjectGuid());
    }
    else if (uiAction == GOSSIP_ACTION_INFO_DEF + 6)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "26 attack power", EQUIPMENT_SLOT_HANDS, 33996);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "15 strength", EQUIPMENT_SLOT_HANDS, 33995);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "15 agility", EQUIPMENT_SLOT_HANDS, 25080);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "20 spell damage", EQUIPMENT_SLOT_HANDS, 33997);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "35 healing 12 spell damage", EQUIPMENT_SLOT_HANDS, 33999);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "15 spell hit", EQUIPMENT_SLOT_HANDS, 33994);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "10 spell crit", EQUIPMENT_SLOT_HANDS, 33993);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Main Menu", 0, 0);
        pPlayer->GetPlayerMenu()->SendGossipMenu(GOSSIP_TEXT_CRYSTAL_2, pCreature->GetObjectGuid());
    }
    else if (uiAction == GOSSIP_ACTION_INFO_DEF + 7)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "50 attack power 12 crit", EQUIPMENT_SLOT_LEGS, 35490);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "40 stamina 12 agility", EQUIPMENT_SLOT_LEGS, 35495);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "35 spell damage 20 stamina", EQUIPMENT_SLOT_LEGS, 31372);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "66 healing 22 spell damage 20 stamina", EQUIPMENT_SLOT_LEGS, 31370);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Main Menu", 0, 0);
        pPlayer->GetPlayerMenu()->SendGossipMenu(GOSSIP_TEXT_CRYSTAL_2, pCreature->GetObjectGuid());
    }
    else if (uiAction == GOSSIP_ACTION_INFO_DEF + 8)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Movespeed 6 agility", EQUIPMENT_SLOT_FEET, 34007);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Movespeed 9 stamina", EQUIPMENT_SLOT_FEET, 34008);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "5% snare and root resistance 10 hit", EQUIPMENT_SLOT_FEET, 27954);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "12 agility", EQUIPMENT_SLOT_FEET, 27951);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "12 stamina", EQUIPMENT_SLOT_FEET, 27950);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "4 hp/mp5", EQUIPMENT_SLOT_FEET, 27948);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Main Menu", 0, 0);
        pPlayer->GetPlayerMenu()->SendGossipMenu(GOSSIP_TEXT_CRYSTAL_2, pCreature->GetObjectGuid());
    }
    else if (uiAction == GOSSIP_ACTION_INFO_DEF + 9)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "4 to all stats", EQUIPMENT_SLOT_FINGER1, 27927);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "12 spell damage", EQUIPMENT_SLOT_FINGER1, 27924);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "20 healing 7 spell damage", EQUIPMENT_SLOT_FINGER1, 27926);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "2 damage to physical attacks", EQUIPMENT_SLOT_FINGER1, 27920);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Main Menu", 0, 0);
        pPlayer->GetPlayerMenu()->SendGossipMenu(GOSSIP_TEXT_CRYSTAL_2, pCreature->GetObjectGuid());
    }
    else if (uiAction == GOSSIP_ACTION_INFO_DEF + 10)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "4 to all stats", EQUIPMENT_SLOT_FINGER2, 27927);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "12 spell damage", EQUIPMENT_SLOT_FINGER2, 27924);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "20 healing 7 spell damage", EQUIPMENT_SLOT_FINGER2, 27926);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "2 physical damage", EQUIPMENT_SLOT_FINGER2, 27920);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Main Menu", 0, 0);
        pPlayer->GetPlayerMenu()->SendGossipMenu(GOSSIP_TEXT_CRYSTAL_2, pCreature->GetObjectGuid());
    }
    else if (uiAction == GOSSIP_ACTION_INFO_DEF + 11)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "70 attack power", EQUIPMENT_SLOT_MAINHAND, 27971);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "35 agility", EQUIPMENT_SLOT_MAINHAND, 27977);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "9 physical damage", EQUIPMENT_SLOT_MAINHAND, 20030);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Main Menu", 0, 0);
        pPlayer->GetPlayerMenu()->SendGossipMenu(GOSSIP_TEXT_CRYSTAL_2, pCreature->GetObjectGuid());
    }
    else if (uiAction == GOSSIP_ACTION_INFO_DEF + 12)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Mongoose", EQUIPMENT_SLOT_MAINHAND, 27984);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Executioner", EQUIPMENT_SLOT_MAINHAND, 42974);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "20 strength", EQUIPMENT_SLOT_MAINHAND, 27972);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "20 agility", EQUIPMENT_SLOT_MAINHAND, 42620);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "40 spell damage", EQUIPMENT_SLOT_MAINHAND, 27975);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "81 healing 27 spell damage", EQUIPMENT_SLOT_MAINHAND, 34010);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "20 spirit", EQUIPMENT_SLOT_MAINHAND, 23803);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "7 physical damage", EQUIPMENT_SLOT_MAINHAND, 27967);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "50 Arcane/Fire spell damage", EQUIPMENT_SLOT_MAINHAND, 27981);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Next Page ->", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 13);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Main Menu", 0, 0);
        pPlayer->GetPlayerMenu()->SendGossipMenu(GOSSIP_TEXT_CRYSTAL_2, pCreature->GetObjectGuid());
    }
    else if (uiAction == GOSSIP_ACTION_INFO_DEF + 13)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "30 intellect", EQUIPMENT_SLOT_MAINHAND, 27968);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Battlemaster", EQUIPMENT_SLOT_MAINHAND, 28004);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Lifestealing", EQUIPMENT_SLOT_MAINHAND, 20032);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Crusader", EQUIPMENT_SLOT_MAINHAND, 20034);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Deathfrost", EQUIPMENT_SLOT_MAINHAND, 46578);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Fiery Weapon", EQUIPMENT_SLOT_MAINHAND, 13898);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Icy Chill", EQUIPMENT_SLOT_MAINHAND, 20029);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Spellsurge", EQUIPMENT_SLOT_MAINHAND, 28003);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Adamantite Weapon Chain", EQUIPMENT_SLOT_MAINHAND, 42687);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "54 Shadow/Frost spell damage", EQUIPMENT_SLOT_MAINHAND, 27982);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "<- Previous Page", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Main Menu", 0, 0);
        pPlayer->GetPlayerMenu()->SendGossipMenu(GOSSIP_TEXT_CRYSTAL_2, pCreature->GetObjectGuid());
    }
    else if (uiAction == GOSSIP_ACTION_INFO_DEF + 14)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Mongoose", EQUIPMENT_SLOT_OFFHAND, 27984);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Executioner", EQUIPMENT_SLOT_OFFHAND, 42974);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "20 strength", EQUIPMENT_SLOT_OFFHAND, 27972);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "20 agility", EQUIPMENT_SLOT_OFFHAND, 42620);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "7 physical damage", EQUIPMENT_SLOT_OFFHAND, 27967);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Next Page ->", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 15);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Main Menu", 0, 0);
        pPlayer->GetPlayerMenu()->SendGossipMenu(GOSSIP_TEXT_CRYSTAL_2, pCreature->GetObjectGuid());
    }
    else if (uiAction == GOSSIP_ACTION_INFO_DEF + 15)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Battlemaster", EQUIPMENT_SLOT_OFFHAND, 28004);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Lifestealing", EQUIPMENT_SLOT_OFFHAND, 20032);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Crusader", EQUIPMENT_SLOT_OFFHAND, 20034);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Deathfrost", EQUIPMENT_SLOT_OFFHAND, 46578);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Fiery Weapon", EQUIPMENT_SLOT_OFFHAND, 13898);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Icy Chill", EQUIPMENT_SLOT_OFFHAND, 20029);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Adamantite Weapon Chain", EQUIPMENT_SLOT_OFFHAND, 42687);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "<- Previous Page", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 14);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Main Menu ", 0, 0);
        pPlayer->GetPlayerMenu()->SendGossipMenu(GOSSIP_TEXT_CRYSTAL_2, pCreature->GetObjectGuid());
    }
    else if (uiAction == GOSSIP_ACTION_INFO_DEF + 16)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "28 crit", EQUIPMENT_SLOT_RANGED, 30260);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "30 hit", EQUIPMENT_SLOT_RANGED, 22779);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "12 Range damage", EQUIPMENT_SLOT_RANGED, 30252);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Main Menu ", 0, 0);
        pPlayer->GetPlayerMenu()->SendGossipMenu(GOSSIP_TEXT_CRYSTAL_2, pCreature->GetObjectGuid());
    }
    else if (uiAction == GOSSIP_ACTION_INFO_DEF + 17)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "12 resilience", EQUIPMENT_SLOT_OFFHAND, 44383);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "18 stamina", EQUIPMENT_SLOT_OFFHAND, 34009);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "12 intellect", EQUIPMENT_SLOT_OFFHAND, 27945);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "5 to all resistance", EQUIPMENT_SLOT_OFFHAND, 27947);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "15 Shield Block", EQUIPMENT_SLOT_OFFHAND, 27946);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "9 spirit", EQUIPMENT_SLOT_OFFHAND, 20016);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "8 Frost resistance", EQUIPMENT_SLOT_OFFHAND, 11224);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "30 armor", EQUIPMENT_SLOT_OFFHAND, 13464);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "26-38 damage if blocked", EQUIPMENT_SLOT_OFFHAND, 29454);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Main Menu", 0, 0);
        pPlayer->GetPlayerMenu()->SendGossipMenu(GOSSIP_TEXT_CRYSTAL_2, pCreature->GetObjectGuid());
    }
    else if (uiAction == GOSSIP_ACTION_INFO_DEF + 0)
        GossipHello_enchantment_crystal(pPlayer, pCreature);
    else
    {
        pPlayer->EnchantItem(uiAction, uiSender);
        GossipHello_enchantment_crystal(pPlayer, pCreature);
    }
    return true;
}

void AddSC_enchantment_crystal()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "enchantment_crystal";
    newscript->pGossipHello  = &GossipHello_enchantment_crystal;
    newscript->pGossipSelect = &GossipSelect_enchantment_crystal;
    newscript->RegisterSelf();
}
