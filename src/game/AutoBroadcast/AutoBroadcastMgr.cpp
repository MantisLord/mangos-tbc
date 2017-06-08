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

#include "AutoBroadcastMgr.h"
#include "Database/DatabaseEnv.h"
#include "World/World.h"
#include "Log/Log.h"

INSTANTIATE_SINGLETON_1(AutoBroadCastMgr);

AutoBroadCastMgr::AutoBroadCastMgr() {}

AutoBroadCastMgr::~AutoBroadCastMgr()
{
    entries.clear();
}

void AutoBroadCastMgr::load()
{
    entries.clear();

    auto result = WorldDatabase.Query("SELECT text FROM autobroadcast");
    if (!result)
    {
        sLog.outString(">> Loaded 0 AutoBroadcast messages");
        return;
    }

    uint32 count = 0;

    Field *fields;
    do
    {
        fields = result->Fetch();
        std::string message = fields[0].GetCppString();
        entries.push_back(message);
        ++count;
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u AutoBroadcast messages", count);
}

void AutoBroadCastMgr::SendAutoBroadcast()
{
    if (entries.empty())
        return;

    std::string msg;

    std::vector<std::string>::const_iterator itr = entries.begin();
    std::advance(itr, rand() % entries.size());
    msg = *itr;

    uint32 abcenter = sWorld.getConfig(CONFIG_UINT32_AUTOBROADCAST_CENTER);
    if (abcenter == 0)
        sWorld.SendServerMessage(SERVER_MSG_CUSTOM, msg.c_str(), nullptr);
    else if (abcenter == 1)
    {
        WorldPacket data(SMSG_NOTIFICATION, (msg.size() + 1));
        data << msg;
        sWorld.SendGlobalMessage(data);
    }
    else if (abcenter == 2)
    {
        sWorld.SendServerMessage(SERVER_MSG_CUSTOM, msg.c_str(), nullptr);

        WorldPacket data(SMSG_NOTIFICATION, (msg.size() + 1));
        data << msg;
        sWorld.SendGlobalMessage(data);
    }

    sLog.outDebug("AutoBroadcast: '%s'", msg.c_str());
}
