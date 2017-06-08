/* ScriptData
SDName: Instance_Azshara_Crater
SD%Complete:
SDComment: Custom
SDCategory: Azshara Crater
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "azshara_crater.h"

instance_azshara_crater::instance_azshara_crater(Map* pMap) : ScriptedInstance(pMap),
    m_uiNextMinionWaveTimer(0),
    m_uiWaveCounter(0)
{
    Initialize();
}

void instance_azshara_crater::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_azshara_crater::OnCreatureCreate(Creature* creature)
{
    switch (creature->GetEntry())
    {
        case NPC_LEGIONNAIRE_GURAK:
        case NPC_CAPTAIN_BENEDICT:
            m_npcEntryGuidStore[creature->GetEntry()] = creature->GetObjectGuid();
            break;
        case NPC_FOOTMAN:
        case NPC_GRUNT:
            m_npcEntryGuidCollection[creature->GetEntry()].push_back(creature->GetObjectGuid());
            break;
    }
}

void instance_azshara_crater::OnCreatureDeath(Creature* creature)
{
    switch (creature->GetEntry())
    {
        case NPC_LEGIONNAIRE_GURAK:
        case NPC_CAPTAIN_BENEDICT:
            SetData(TYPE_BATTLE_FOR_THE_CRATER, DONE);
            break;
    }
}

void instance_azshara_crater::SpawnSingleMinion(Creature* spawner, uint32 entry)
{
    float newX, newY, newZ;
    spawner->GetRandomPoint(spawner->GetPositionX(), spawner->GetPositionY(), spawner->GetPositionZ(), frand(0.f, 10.f), newX, newY, newZ);
    spawner->SummonCreature(entry, newX, newY, newZ, spawner->GetOrientation(), TEMPSPAWN_DEAD_DESPAWN, 0, true, true);
}

void instance_azshara_crater::SpawnNextWave()
{
    m_uiWaveCounter++;
    debug_log("SD2: instance_azshara_crater: Spawning minion wave #%u.", m_uiWaveCounter);
    if (Creature* gurak = GetSingleCreatureFromStorage(NPC_LEGIONNAIRE_GURAK))
    {
        for (int i = 0; i < 4; i++)
            SpawnSingleMinion(gurak, NPC_GRUNT);
        gurak->HandleEmote(EMOTE_ONESHOT_POINT_NOSHEATHE);
    }
    if (Creature* benedict = GetSingleCreatureFromStorage(NPC_CAPTAIN_BENEDICT))
    {
        for (int i = 0; i < 4; i++)
            SpawnSingleMinion(benedict, NPC_FOOTMAN);
        benedict->HandleEmote(EMOTE_ONESHOT_POINT_NOSHEATHE);
    }
}

void instance_azshara_crater::EndBattle()
{
    m_uiNextMinionWaveTimer = 0;

    Map::PlayerList const& players = instance->GetPlayers();
    for (const auto& player : players)
        if (Player* pPlayer = player.getSource())
            pPlayer->GetSession()->SendNotification("The battle is over! %u minion waves were spawned.", m_uiWaveCounter);

    if (Creature* gurak = GetSingleCreatureFromStorage(NPC_LEGIONNAIRE_GURAK))
        gurak->MonsterSay("Just when things were heating up...", LANG_ORCISH);
    if (Creature* benedict = GetSingleCreatureFromStorage(NPC_CAPTAIN_BENEDICT))
        benedict->MonsterSay("Just when things were heating up...", LANG_COMMON);

    GuidVector footmen;
    GetCreatureGuidVectorFromStorage(NPC_FOOTMAN, footmen);
    for (auto guid : footmen)
        if (Creature* footman = instance->GetCreature(guid))
            footman->ForcedDespawn();

    GuidVector grunts;
    GetCreatureGuidVectorFromStorage(NPC_GRUNT, grunts);
    for (auto guid : grunts)
        if (Creature* grunt = instance->GetCreature(guid))
            grunt->ForcedDespawn();
}

void instance_azshara_crater::StartBattle()
{
    m_uiNextMinionWaveTimer = 1;

    Map::PlayerList const& players = instance->GetPlayers();
    for (const auto& player : players)
        if (Player* pPlayer = player.getSource())
            pPlayer->GetSession()->SendNotification("The battle has begun! Minion waves are now spawning.");

    if (Creature* gurak = GetSingleCreatureFromStorage(NPC_LEGIONNAIRE_GURAK))
    {
        gurak->MonsterSay("Go now! Destroy the enemy!", LANG_ORCISH);
        gurak->HandleEmote(EMOTE_ONESHOT_EXCLAMATION);
    }
    if (Creature* benedict = GetSingleCreatureFromStorage(NPC_CAPTAIN_BENEDICT))
    {
        benedict->MonsterSay("Go now! Destroy the enemy!", LANG_COMMON);
        benedict->HandleEmote(EMOTE_ONESHOT_EXCLAMATION);
    }
}

void instance_azshara_crater::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_BATTLE_FOR_THE_CRATER:
        {
            if (uiData == IN_PROGRESS)
                StartBattle();
            if (uiData == DONE || uiData == FAIL)
                EndBattle();
            m_auiEncounter[uiType] = uiData;
            break;
        }
    }
    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;
        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0];
        m_strInstData = saveStream.str();
        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

void instance_azshara_crater::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }
    OUT_LOAD_INST_DATA(chrIn);
    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0];
    for (uint32& i : m_auiEncounter)
    {
        if (i == IN_PROGRESS)
            i = NOT_STARTED;
    }
    OUT_LOAD_INST_DATA_COMPLETE;
}

uint32 instance_azshara_crater::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];
    return 0;
}

void instance_azshara_crater::Update(uint32 uiDiff)
{
    if (GetData(TYPE_BATTLE_FOR_THE_CRATER) != IN_PROGRESS)
        return;

    if (m_uiNextMinionWaveTimer)
    {
        if (m_uiNextMinionWaveTimer <= uiDiff)
        {
            SpawnNextWave();
            m_uiNextMinionWaveTimer = 15000;
        }
        else
            m_uiNextMinionWaveTimer -= uiDiff;
    }
}

InstanceData* GetInstanceData_instance_azshara_crater(Map* pMap)
{
    return new instance_azshara_crater(pMap);
}

void AddSC_instance_azshara_crater()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_azshara_crater";
    pNewScript->GetInstanceData = &GetInstanceData_instance_azshara_crater;
    pNewScript->RegisterSelf();
}
