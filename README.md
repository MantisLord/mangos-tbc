# CMaNGOS-TBC Custom Test Realm

A collection of fun scripts/tweaks for CMaNGOS brought to you by Smolderforge and Vengeance.

## Setup
1. Build `world` DB as you would normally using [InstallFullDB.sh script in TBC-DB master](https://github.com/cmangos/tbc-db/blob/master/InstallFullDB.sh) - set CORE_PATH to [CMaNGOS-TBC master](https://github.com/cmangos/mangos-tbc)
2. Execute [custom_world.sql](CustomTestRealmSQLPatches/custom_world.sql) in `world` DB
3. Execute [custom_characters.sql](CustomTestRealmSQLPatches/custom_characters.sql) in `characters` DB
4. (Optional) Copy [patch-3.MPQ](CustomTestRealmClientPatches/patch-3.MPQ) into game client's Data directory to unlock map 451 and use DesignerIsland/ProgrammerIsle teleports
