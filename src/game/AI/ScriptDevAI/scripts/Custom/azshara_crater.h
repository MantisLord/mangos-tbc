#ifndef DEF_AZSHARA_CRATER_H
#define DEF_AZSHARA_CRATER_H

enum
{
    MAX_ENCOUNTER               = 1,
    TYPE_BATTLE_FOR_THE_CRATER  = 0,
    NPC_LEGIONNAIRE_GURAK       = 980047,
    NPC_CAPTAIN_BENEDICT        = 980046,
    NPC_GRUNT                   = 980045,
    NPC_FOOTMAN                 = 980044,
};

class instance_azshara_crater : public ScriptedInstance
{
    public:
        instance_azshara_crater(Map* pMap);
        ~instance_azshara_crater() {}

        void Initialize() override;

        void OnCreatureCreate(Creature* pCreature) override;
        void OnCreatureDeath(Creature* pCreature) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) const override;

        const char* Save() const override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

        void Update(const uint32 diff) override;

    protected:
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;
        uint32 m_uiNextMinionWaveTimer;
        uint32 m_uiWaveCounter;
        void SpawnNextWave();
        void SpawnSingleMinion(Creature* spawner, uint32 entry);
        void EndBattle();
        void StartBattle();
};

#endif
