bool GossipHello_titler(Player* player, Creature* creature)
{
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "PvP Ranks - Alliance", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 100);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "PvP Ranks - Horde", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 200);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Arena", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 300);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Misc", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 400);
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "City Protector", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 500);
    player->SEND_GOSSIP_MENU(50606, creature->GetObjectGuid());
    return true;
}

bool GossipSelect_titler(Player* player, Creature* creature, uint32 sender, uint32 action)
{
    if (action >= GOSSIP_ACTION_INFO_DEF + 1 && action <= GOSSIP_ACTION_INFO_DEF + 71)
    {
        player->SetTitle(action - GOSSIP_ACTION_INFO_DEF);
        GossipHello_titler(player, creature);
    }

    switch (action)
    {
        case GOSSIP_ACTION_INFO_DEF:
            GossipHello_titler(player, creature);
            break;
        case GOSSIP_ACTION_INFO_DEF + 100:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Main Menu", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Private", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Corporal", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Sergeant", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Master Sergeant", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Sergeant Major", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Knight", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Knight-Lieutenant", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Knight-Captain", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Knight-Champion", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Lieutenant Commander", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Commander", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Marshal", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Field Marshal", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 13);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Grand Marshal", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 14);
            player->SEND_GOSSIP_MENU(50609, creature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 200:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Main Menu", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Scout", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 15);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Grunt", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 16);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Sergeant", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 17);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Senior Sergeant", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 18);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "First Sergeant", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 19);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Stone Guard", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 20);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Blood Guard", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 21);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Legionnaire", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 22);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Centurion", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 23);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Champion", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 24);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Lieutenant General", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 25);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "General", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 26);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Warlord", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 27);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "High Warlord", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 28);
            player->SEND_GOSSIP_MENU(50610, creature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 300:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Main Menu", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Gladiator", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 42);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Duelist", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 43);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Rival", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 44);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Challenger", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 45);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Merciless Gladiator", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 62);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Vengeful Gladiator", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 71);
            player->SEND_GOSSIP_MENU(50608, creature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 400:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Main Menu", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Scarab Lord", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 46);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Conqueror", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 47);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Justicar", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 48);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Champion of the Naaru", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 53);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "of the Shattered Sun", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 63);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Hand of A'dal", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 64);
            player->SEND_GOSSIP_MENU(50611, creature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 500:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Main Menu", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Protector of Stormwind", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 501);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Overlord of Orgrimmar", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 502);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Thane of Ironforge", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 503);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "High Sentinel of Darnassus", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 504);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Deathlord of the Undercity", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 505);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Chieftain of Thunderbluff", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 506);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Avenger of Gnomeregan", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 507);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Voodoo Boss of Sen'jin", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 508);
            player->SEND_GOSSIP_MENU(50607, creature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 501:
        case GOSSIP_ACTION_INFO_DEF + 502:
        case GOSSIP_ACTION_INFO_DEF + 503:
        case GOSSIP_ACTION_INFO_DEF + 504:
        case GOSSIP_ACTION_INFO_DEF + 505:
        case GOSSIP_ACTION_INFO_DEF + 506:
        case GOSSIP_ACTION_INFO_DEF + 507:
        case GOSSIP_ACTION_INFO_DEF + 508:
            player->SetPlayerSetting(PLAYER_SETTING_CITY_PROTECTOR, action - GOSSIP_ACTION_INFO_DEF - 500);
            GossipHello_titler(player, creature);
            break;
    }
    return true;
}

void AddSC_titler()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "titler";
    newscript->pGossipHello = &GossipHello_titler;
    newscript->pGossipSelect = &GossipSelect_titler;
    newscript->RegisterSelf();
}
