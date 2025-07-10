-- Execute script in WORLD database

-- =====================================================
-- Global Settings/Toggles
-- =====================================================

SET @CGUID := 9999000;
SET @OGUID := 9999000;

SET @LEVEL_BOOST_GAME_EVENT_ID := 9000;
SET @LEVEL_BOOST_RESTRICT_TBC_RACES_EVENT_ID := 9001;
SET @LEVEL_BOOST_RESTRICT_HORDE_RACES_EVENT_ID := 9002;
SET @LEVEL_BOOST_RESTRICT_ALLIANCE_RACES_EVENT_ID := 9003;

SET @DUAL_SPECIALIZATION_EVENT_ID := 9004;

SET @PTR_FUNSERVER_GAME_EVENT_ID := 9005;

/*
-- Start location change to Quel'thalas mall
UPDATE `playercreateinfo` SET `map`=0, `zone`=139, `position_x`=4272.49, `position_y`=-2777.97, `position_z`=5.55995, `orientation`=0.523871;

-- No action bars
TRUNCATE playercreateinfo_action;

-- Start items
DELETE FROM playercreateinfo_item WHERE itemid IN (5176, 5177, 5178, 5175, 21876, 21967, 18714);
INSERT INTO `playercreateinfo_item` (`race`, `class`, `itemid`, `amount`) VALUES
(0, 7, 5176, 1), -- Fire Totem
(0, 7, 5177, 1), -- Water Totem
(0, 7, 5178, 1), -- Air Totem
(0, 7, 5175, 1), -- Earth Totem
(0, 0, 21876, 4), -- Primal Mooncloth Bag
(0, 0, 21967, 1), -- Magic Talent Switch Gemstone
(0, 3, 18714, 1); -- Ancient Sinew Wrapped Lamina

-- Start spells
DELETE FROM playercreateinfo_spell WHERE Spell IN (34091,27028);
INSERT INTO `playercreateinfo_spell` (`race`, `class`, `Spell`, `Note`) VALUES
(0, 0, 34091, 'Riding (rank 4)'),
(0, 0, 27028, 'First Aid (rank 5)');

-- Remove gold cost from all items (looks cleaner on vendors)
UPDATE `item_template` SET `BuyPrice` = 0 WHERE `BuyPrice` > 0;

-- Consumes to 250 stack
UPDATE `item_template` SET `stackable`=250, `BuyCount`=250 WHERE `entry` IN (32453, 29450, 22832, 29450, 22829, 22148, 22147, 22055, 22054, 21991, 21927, 21177, 17058, 17057, 17056, 17033, 17032, 17031, 17030, 17029, 17028, 17026, 17020, 9186, 6265, 5565, 5140, 3776);

-- Remove all attunements
UPDATE `areatrigger_teleport` SET required_item=0,required_item2=0,heroic_key=0,heroic_key2=0,required_quest_done=0,required_quest_done_heroic=0,condition_id=0;

-- WARNING: Enables all commands for all players
UPDATE `command` SET `security`=0;
*/

DROP TABLE IF EXISTS `spell_disabled`;
CREATE TABLE `spell_disabled` (
	`entry` INT(10) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Spell entry',
	`disable_mask` INT(10) UNSIGNED NOT NULL DEFAULT '0',
	`comment` VARCHAR(64) NOT NULL DEFAULT '' COLLATE 'utf8_general_ci',
	PRIMARY KEY (`entry`) USING BTREE
)
COMMENT='Disabled Spell System'
COLLATE='utf8_general_ci'
ENGINE=MyISAM
ROW_FORMAT=FIXED;

DROP TABLE IF EXISTS `mail_external`;
CREATE TABLE `mail_external` (
	`id` INT(10) UNSIGNED NOT NULL,
	`receiver` INT(10) UNSIGNED NOT NULL,
	`subject` VARCHAR(64) NOT NULL DEFAULT 'Default subject' COLLATE 'utf8_general_ci',
	`message` VARCHAR(500) NOT NULL DEFAULT 'Default message' COLLATE 'utf8_general_ci',
	`money` INT(10) UNSIGNED NOT NULL DEFAULT '0',
	`item` INT(10) UNSIGNED NOT NULL DEFAULT '0',
	`item_count` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0',
	PRIMARY KEY (`id`) USING BTREE
)
COLLATE='utf8_general_ci'
ENGINE=InnoDB;

REPLACE INTO `game_event` (`entry`, `schedule_type`, `occurence`, `length`, `holiday`, `description`) VALUES
(@PTR_FUNSERVER_GAME_EVENT_ID, 1, '201600', '201600', '0', 'Custom: MantisLord''s Funland'),
(@DUAL_SPECIALIZATION_EVENT_ID, 1, '201600', '201600', '0', 'Custom: Dual Specialization'),
(@LEVEL_BOOST_GAME_EVENT_ID, 1, '201600', '201600', '0', 'Custom: Level Boost');
REPLACE INTO `game_event_time` (`entry`, `start_time`, `end_time`) VALUES
(@PTR_FUNSERVER_GAME_EVENT_ID, '2030-12-31 00:00:00', '2030-12-31 00:00:00'),
(@DUAL_SPECIALIZATION_EVENT_ID, '2030-12-31 00:00:00', '2030-12-31 00:00:00'),
(@LEVEL_BOOST_GAME_EVENT_ID, '2030-12-31 00:00:00', '2030-12-31 00:00:00');
-- Use worldstate command "boostrestriction" - don't start these child events manually
REPLACE INTO `game_event` (`entry`, `schedule_type`, `occurence`, `length`, `holiday`, `description`) VALUES
(@LEVEL_BOOST_RESTRICT_TBC_RACES_EVENT_ID, 0, '201600', '201600', '0', 'Custom: Level Boost - TBC Race Restriction'), -- GAME_EVENT_LEVEL_BOOST_RESTRICTION_TBC_RACES 9001
(@LEVEL_BOOST_RESTRICT_HORDE_RACES_EVENT_ID, 0, '201600', '201600', '0', 'Custom: Level Boost - Horde Race Restriction'), -- GAME_EVENT_LEVEL_BOOST_RESTRICTION_HORDE_RACES 9002
(@LEVEL_BOOST_RESTRICT_ALLIANCE_RACES_EVENT_ID, 0, '201600', '201600', '0', 'Custom: Level Boost - Alliance Race Restriction'); -- GAME_EVENT_LEVEL_BOOST_RESTRICTION_ALLIANCE_RACES 9003

DELETE FROM `game_tele` WHERE `id` IN (99998,99997,99996);
INSERT INTO `game_tele` (`id`, `position_x`, `position_y`, `position_z`, `orientation`, `map`, `name`) VALUES
(99998, 4271.622, -2778.375, 5.554479, 0.539123, 0, 'Mall'),
(99997, 16303.5, -16173.5, 40.4365, 0.539123, 451, 'DesignerIsland'), -- WARNING: Must install client_patch/patch-3.MPQ in game client's Data directory or you will receive a WoWError upon porting here!
(99996, 16304, 16318, 69.446, 0.539123, 451, 'ProgrammerIsle'); -- WARNING: Must install client_patch/patch-3.MPQ in game client's Data directory or you will receive a WoWError upon porting here!

DELETE FROM instance_template WHERE map IN (37,44);
INSERT INTO instance_template (map, parent, levelMin, levelMax, maxPlayers, reset_delay, ScriptName, mountAllowed) VALUES
(37, 0, 0, 0, 0, 0, 'instance_azshara_crater', 1),
(44, 0, 0, 0, 0, 0, '', 1); -- Old Scarlet Monastery

-- =====================================================
-- End Global Settings/Toggles
-- =====================================================

REPLACE INTO `item_template` (`entry`,`class`,`subclass`,`unk0`,`name`,`displayid`,`Quality`,`Flags`,`BuyCount`,`BuyPrice`,`SellPrice`,`InventoryType`,`AllowableClass`,`AllowableRace`,`ItemLevel`,`RequiredLevel`,`RequiredSkill`,`RequiredSkillRank`,`requiredspell`,`requiredhonorrank`,`RequiredCityRank`,`RequiredReputationFaction`,`RequiredReputationRank`,`maxcount`,`stackable`,`ContainerSlots`,`stat_type1`,`stat_value1`,`stat_type2`,`stat_value2`,`stat_type3`,`stat_value3`,`stat_type4`,`stat_value4`,`stat_type5`,`stat_value5`,`stat_type6`,`stat_value6`,`stat_type7`,`stat_value7`,`stat_type8`,`stat_value8`,`stat_type9`,`stat_value9`,`stat_type10`,`stat_value10`,`dmg_min1`,`dmg_max1`,`dmg_type1`,`dmg_min2`,`dmg_max2`,`dmg_type2`,`dmg_min3`,`dmg_max3`,`dmg_type3`,`dmg_min4`,`dmg_max4`,`dmg_type4`,`dmg_min5`,`dmg_max5`,`dmg_type5`,`armor`,`holy_res`,`fire_res`,`nature_res`,`frost_res`,`shadow_res`,`arcane_res`,`delay`,`ammo_type`,`RangedModRange`,`spellid_1`,`spelltrigger_1`,`spellcharges_1`,`spellppmRate_1`,`spellcooldown_1`,`spellcategory_1`,`spellcategorycooldown_1`,`spellid_2`,`spelltrigger_2`,`spellcharges_2`,`spellppmRate_2`,`spellcooldown_2`,`spellcategory_2`,`spellcategorycooldown_2`,`spellid_3`,`spelltrigger_3`,`spellcharges_3`,`spellppmRate_3`,`spellcooldown_3`,`spellcategory_3`,`spellcategorycooldown_3`,`spellid_4`,`spelltrigger_4`,`spellcharges_4`,`spellppmRate_4`,`spellcooldown_4`,`spellcategory_4`,`spellcategorycooldown_4`,`spellid_5`,`spelltrigger_5`,`spellcharges_5`,`spellppmRate_5`,`spellcooldown_5`,`spellcategory_5`,`spellcategorycooldown_5`,`bonding`,`description`,`PageText`,`LanguageID`,`PageMaterial`,`startquest`,`lockid`,`Material`,`sheath`,`RandomProperty`,`RandomSuffix`,`block`,`itemset`,`MaxDurability`,`area`,`Map`,`BagFamily`,`TotemCategory`,`socketColor_1`,`socketContent_1`,`socketColor_2`,`socketContent_2`,`socketColor_3`,`socketContent_3`,`socketBonus`,`GemProperties`,`RequiredDisenchantSkill`,`ArmorDamageModifier`,`ScriptName`,`DisenchantID`,`FoodType`,`minMoneyLoot`,`maxMoneyLoot`,`Duration`,`ExtraFlags`) VALUES
('7168', '0', '0', '-1', 'Infinity Coin', '13849', '6', '0', '1', '0', '0', '0', '-1', '-1', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1000', '0', '0', '18282', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '*WARNING* Use with caution.', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', 'item_infinity_coin', '0', '0', '0', '0', '0', '4'),
('21967', '0', '0', '-1', 'Magic Talent Switch Gemstone', '12310', '4', '524289', '1', '0', '0', '0', '-1', '-1', '70', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '18282', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '1', 'Forced logout after talent switch.', '0', '0', '0', '0', '0', '4', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', 'item_custom_dualspec', '0', '0', '0', '0', '0', '4');

DROP TABLE IF EXISTS `autobroadcast`;
CREATE TABLE `autobroadcast` (
    `id` INT(11) NOT NULL AUTO_INCREMENT,
    `text` LONGTEXT NOT NULL,
    PRIMARY KEY (`id`) USING BTREE
);
INSERT INTO `autobroadcast` (`id`, `text`) VALUES
(1, '|c0059ca49[INFO]|cffffffff: Use |c00a5ff57.xp available|cffffffff to view all possible experience boost modifiers, then |c00a5ff57.xp set [modifer]|cffffffff to toggle personal rate.'),
(2, '|c0059ca49[TIP]|cffffffff: Customize these texts by editing data in the autobroadcast table.'),
(3, '|c0059ca49[INFO]|cffffffff: CMaNGOS Discord: |c00a5ff57discord.gg/Dgzerzb|cffffffff'),
(4, '|c0059ca49[TIP]|cffffffff: |c00a5ff57/wave|cffffffff at a combat dummy to see your DPS and total damage.'),
(5, '|c0059ca49[INFO]|cffffffff: These tips and information announcements are automated.'),
(6, '|c0059ca49[TIP]|cffffffff: Dual specialization allows storage of an additional talent spec and set of action bars with it. Access this feature via Universal Class Trainer, Dual Spec Crystal, or Magic Talent Switch Gemstone.'),
(7, '|c0059ca49[INFO]|cffffffff: Please report in-game bugs: |c00a5ff57github.com/cmangos/issues/issues|cffffffff'),
(8, '|c0059ca49[TIP]|cffffffff: There is a |c00a5ff57secret cow level|cffffffff.'),
(9, '|c0059ca49[INFO]|cffffffff: VengeanceWoW Discord: |c00a5ff57discord.gg/6RZmRFA|cffffffff');

DROP TABLE IF EXISTS `chat_filtered`;
CREATE TABLE `chat_filtered` (
    `text` LONGTEXT NOT NULL COLLATE 'utf8mb4_general_ci',
    UNIQUE INDEX `text` (`text`(100)) USING BTREE
) COLLATE='utf8mb4_general_ci'
ENGINE=MyISAM;

DELETE FROM `command` WHERE `name` IN ('xp','debug listitemids','debug gemenchantid','reload autobroadcast','reload chat_filtered','faceme','serverfirst','deserter', 'undeserter');
INSERT INTO `command` VALUES
-- XP Boost system
('xp', '0', 'Syntax: .xp #subcommand XP boost commands'),
-- Misc commands
('debug listitemids', '0', ''),
('debug gemenchantid', '0', ''),
('reload autobroadcast', '0', ''),
('reload chat_filtered', '0', ''),
('faceme', '0', ''),
('serverfirst', '0', ''),
('deserter', 3, 'Syntax: .deserter #punishLevel\r\n\r\nGives your target character the Deserter debuff for duration depending on #punishLevel.\r\n\r\nPunishment levels may range from 0-5: 0 - 15 minutes, 1 - 30 minutes, 2 - 1 hour, 3 - 2 hours, 4 - 18 hours, 5 - 1 week\r\nIf #punishLevel is omitted, the duration will be 15 minutes.'),
('undeserter', 3, 'Syntax: .undeserter #punishLevel\r\n\r\nRemoves target character''s Deserter debuffs of the specified #punishLevel.\r\n\r\n#punishLevel may range from 0-5. If no #punishLevel is supplied, Deserter debuffs of all durations will be removed.');

DELETE FROM `npc_text` WHERE `ID` BETWEEN 42300 AND 42312;
DELETE FROM `npc_text` WHERE `ID` IN (50700,50701);
DELETE FROM `npc_text` WHERE `ID` IN (50000,50100,50200,50300,50201,50202,50203,50204,50205);
DELETE FROM `npc_text` WHERE `ID` IN (50409,50410,50411,50412,50413,50414,50415);
DELETE FROM `npc_text` WHERE `ID` IN (51000,51001,51002,51003,51004,51005,51006,51007,51008,51009,51100,50500,50600,50601,50602,50603,50604,50605,50606,50607,50608,50609,50610,50611,50612,50613,50614,50615);
DELETE FROM `npc_text` WHERE `ID` IN (50399,50400,50401,50402,50403,50404,50405,50406);
INSERT INTO `npc_text` (`ID`, `text0_0`, `text0_1`, `lang0`, `prob0`, `em0_0`, `em0_1`, `em0_2`, `em0_3`, `em0_4`, `em0_5`, `text1_0`, `text1_1`, `lang1`, `prob1`, `em1_0`, `em1_1`, `em1_2`, `em1_3`, `em1_4`, `em1_5`, `text2_0`, `text2_1`, `lang2`, `prob2`, `em2_0`, `em2_1`, `em2_2`, `em2_3`, `em2_4`, `em2_5`, `text3_0`, `text3_1`, `lang3`, `prob3`, `em3_0`, `em3_1`, `em3_2`, `em3_3`, `em3_4`, `em3_5`, `text4_0`, `text4_1`, `lang4`, `prob4`, `em4_0`, `em4_1`, `em4_2`, `em4_3`, `em4_4`, `em4_5`, `text5_0`, `text5_1`, `lang5`, `prob5`, `em5_0`, `em5_1`, `em5_2`, `em5_3`, `em5_4`, `em5_5`, `text6_0`, `text6_1`, `lang6`, `prob6`, `em6_0`, `em6_1`, `em6_2`, `em6_3`, `em6_4`, `em6_5`, `text7_0`, `text7_1`, `lang7`, `prob7`, `em7_0`, `em7_1`, `em7_2`, `em7_3`, `em7_4`, `em7_5`) VALUES
-- Universal Class Trainer
(42300, 'Welcome, $c.\r\n\r\nI am a universal class trainer and may train you, despite whatever class you may be. Would you like to train your abilities?\r\n\r\nOr perhaps you would like to activate a secondary specialization?', 'Welcome, $c.\r\n\r\nI am a universal class trainer and may train you, despite whatever class you may be. Would you like to train your abilities?\r\n\r\nOr perhaps you would like to activate a secondary specialization?', 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0),
(42301, 'Changing specs will replace your talents and action bars. You may also assign names to each specialization.', 'Changing specs will replace your talents and action bars. You may also assign names to each specialization.', 0, 0, 1, 1, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0),
-- Beastmaster
(42302, 'Select a beast below you''d like to tame.', 'Select a beast below you''d like to tame.', 0, 0, 1, 1, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0),
-- 42303-42308 REUSE
-- Race Changer
(42309, 'Hello $r,\r\n\r\nI can change your race and/or gender if you provide me the character name of whose appearance to copy.\r\n\r\nThe character name you enter must be an eligible race for your class.', 'Hello $r,\r\n\r\nI can change your race and/or gender if you provide me the character name of whose appearance to copy.\r\n\r\nThe character name you enter must be an eligible race for your class.', 0, 0, 1, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0),
-- Guild Creator
(42310, 'Greetings $r,\r\n\r\nI can create a personalized guild name for you. Give me a unique name that is under 50 characters and it''s yours.', 'Greetings $r,\r\n\r\nI can create a personalized guild name for you. Give me a unique name that is under 50 characters and it''s yours.', 0, 1, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0),
-- Misc
(42311, 'I''m not sure how to help you, $c.', 'I''m not sure how to help you, $c.', 0, 1, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0),
(42312, 'Hello $N. I''ll help you manage your pets.', 'Hello $N. I''ll help you manage your pets.', 0, 1, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0),

-- Dual Spec Crystals & Magic Talent Switch Gemstone
(50700,'<The crystal hums with energy.>','','0','1','0','1','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(50701,'<The gem hums with energy.>','','0','1','0','1','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),

-- Greeter/Realm Information - Primary
(50000,'I am a VengeanceWoW Raid Messenger.$B$BI have been sent here to advise all travellers that there has been a catastrophic series of events inside this raid. Due to the eminent danger from the creatures lurking on the other side of this portal we have magically sealed it for the safety and security of Azeroth.$B$BEvery effort is being made to contain the situation and we will advise you when the portal is opened again.$B$BThank you for your understanding!','','0','1','0','274','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(50100,'I am a VengeanceWoW Dungeon Messenger.$B$BI have been sent here to advise all travellers that the safety of Azeroth is in jeopardy! We have magically sealed this dungeon to protect Azeroth from the creatures within.$B$BEvery effort is being made to resolve the situation and we will advise you when the portal is opened again.$B$BThank you for your understanding!','','0','1','0','274','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(50200,'Welcome, $N!$B$BI am stationed here to help adventurers like you get started on the realm. The staff hopes you enjoy your stay on VengeanceWoW and looks forward to crossing paths with you on your journey. How may I assist you?','','0','1','0','3','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(50300,'Greetings, $N!$B$BI am an emissary from VengeanceWoW. I provide information to assist travellers on their adventures. How may I help you?','','0','1','0','3','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),

-- Greeter/Realm Information - Secondary
(50201,'VengeanceWoW is a community of nostalgic/oldschool WoW enthusiasts. We strive to provide a safe and friendly environment for all of our players to enjoy.$B$BOur realms are under continual development as we work on improving the game and providing the very best experience possible to our players. We are a highly dedicated team that has been working on WoW emulation since early in the game''s history.$B$BCommunity Discord:$B$B|cFF0008E8discord.gg/6RZmRFA|r$B$B','','0','1','0','1','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
-- (50202,'Thank you for your interest in donating!$B$BAt this point, we are not accepting donations as server operation costs are already covered for the foreseeable future.$B$BRegardless of the status of donations, we will ALWAYS be a free-to-play server with absolutely zero "pay-to-win" elements.','','0','1','0','66','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(50202,'Thank you for your interest in donating!$B$BPlease see the #info channel in our Discord server for more information regarding this:$B$B|cFF0008E8discord.gg/6RZmRFA|r$B$BYour contributions help cover hosting costs of our website and game server.$B$BRegardless of the status of donations, we will ALWAYS be a free-to-play server with absolutely zero "pay-to-win" elements.','','0','1','0','66','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(50203,'|cFF0008E8Reporting Game Problems or Bugs|r$B$BWe have a bug tracker system available for members. You can find a link on our website, or you can use:$B$B|cFF0008E8github.com/cmangos/issues/issues|r$B$BIf you require immediate in-game support, feel free to contact staff on Discord to discuss the issue. We will do our best to help you as soon as possible (it may take us 15-20 minutes to respond if we are AFK).','','0','1','0','1','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(50204,'|cFF0008E8VengeanceWoW Cheating Policy|r$B$BVengeanceWoW has a zero-tolerance policy for cheating, and is equipped with an active anti-cheat detection system. $B$BWe aim to create an equal opportunity, respectful, and enjoyable environment for all of our players. In return, we expect players to uphold a certain level of integrity.$B$BIf you suspect another player of cheating, we request that you report the incident to the development team on Discord.$B$B','','0','1','0','1','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(50205,'|cFF0008E8Contact Us|r$B$BWe have an official Discord channel that is the center of our community outside of the forums:$B$B|cFF0008E8discord.gg/6RZmRFA|r$B$BYou can communicate there with the staff and other players. This is the fastest way to reach a staff member if you have any questions or concerns.$B$BWe look forward to meeting you, and wish you luck on your journey through VengeanceWoW!','','0','1','0','1','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),

-- Test Realm Overlord
(50409,'I have just what you need, $C. Which specialization should I equip you for?','','0','1','0','1','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(50410,'To view some statistics regarding your damage done, /wave to the dummy after you leave combat!','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(50411,'Choose the modifier for your PRIMARY gear. This includes slots:$B$BHead$BShoulders$BChest$BHands$BLegs','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(50412,'Choose the modifier for your SECONDARY gear. This includes slots:$B$BBack$BRings$BNeck$BWrist$BWaist$BBoots','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(50413,'Choose the modifier for your TERTIARY gear. This includes slots:$B$BWeapons$BRanged','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(50414,'Hi there, $N. Make your choice...','','0','1','0','1','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(50415,'Which starting zone should I take you to?','','0','1','0','1','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),

-- Enchantment Crystal
(50399,'Select an enchant.','','0','1','0','1','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(50400,'Choose an item slot.','','0','1','0','1','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),

-- Greeter - deny Level Boost
(50401,'Sorry, you must have attained at least level 20 before you''re ready for my training.','Sorry, you must have attained at least level 20 before you''re ready for my training.','0','1','0','1','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
-- Greeter - confirm Level Boost
(50402,'You must understand there is no turning back once you''ve made this decision.$B$BAre you absolutely certain you wish to embark on this path, $C?$B$B','','0','1','0','1','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),

-- Horde/Alliance Enlistment Officer
(50403,'Greetings, new recruit.$B$BIs there anything I can assist you with before you deploy to the Dark Portal?','','0','1','0','1','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(50404,'Keep up the training, $C. You are not prepared for Outland right now. If you''re looking for a bit of a boost, you should visit a greeter located within any of your faction''s starting areas.','','0','1','0','1','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(50405,'You look hardy enough, $C. What are you waiting for? You don''t need my help!$B$BHead to Outland and help us drive back the Legion!','','0','1','0','1','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(50406,'We happen to have quite a selection of dangerous and exotic creatures. Take your pick.','','0','1','0','1','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),

-- Portals
(51000,'Where would you like to go?\r\nYou must be in a raid group to travel to raids or have GM mode turned on.','','0','1','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(51001,'TBC Dungeons','','0','1','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(51002,'Classic Dungeons','','0','1','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(51003,'Classic Dungeons (cont.)','','0','1','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(51004,'Cities','','0','1','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(51005,'TBC Dungeons (cont.)','','0','1','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(51006,'Unknown Wonders','','0','1','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(51007,'Unknown Wonders (Page 2)','','0','1','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(51008,'Unknown Wonders (Page 3)','','0','1','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(51009,'Unknown Wonders (Page 4)','','0','1','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(51100,'Teleport To Raid (you must be in a raid group)','','0','1','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(50500,'TBC Raids','','0','1','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(50600,'Classic Raids','','0','1','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
-- Portal to Funland
(50601,'<You approach the shimmering portal.>','','0','1','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
-- Model Viewer
(50602,'Hi there. I''m having an identity crisis.','','0','1','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
-- Captain/Legionnaire - Azshara Crater
(50603,'Our forces are standing by. The Battle for the Crater is imminent!','','0','1','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(50604,'Make it quick, $C. Push the enemy back and claim the Crater!','','0','5','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(50605,'This cursed place is Azshara Crater.$B$BPlease assist our forces. Slay the enemy commander to win!','','0','1','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
-- Titler
(50606,'Hey lucky stranger! Are you feeling entitled to something?','','0','1','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(50607,'City Protector$B$BPlease note that you must also be displaying a regular title for these to appear.','','0','1','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(50608,'Arena','','0','1','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(50609,'PvP Ranks - Alliance','','0','1','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(50610,'PvP Ranks - Horde','','0','1','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(50611,'Misc','','0','1','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),

-- Infinity Coin
(50612,'The golden token glistens in your palm. As you peer deeply into its many cryptic inscriptions, you''re overcome with terror and dread.$B$BAny further interaction could destabilize this entire realm...','','0','1','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(50613,'Creature Display Info & Model Data','','0','1','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(50614,'Movement & Positioning','','0','1','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0'),
(50615,'UNIT_BYTES_1_OFFSET_STAND_STATE (UnitStandStateType)','','0','1','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0');

DELETE FROM `npc_text_broadcast_text` WHERE `Id` IN (99999,99998);
INSERT INTO `npc_text_broadcast_text` (`Id`, `Prob0`, `BroadcastTextId0`) VALUES
(99999,1,13644), -- "Need something?"
(99998,1,15251); -- "Which profession do you seek to learn?"

DELETE FROM `script_texts` WHERE `entry` IN (-1800997,-1800998,-1800999);
INSERT INTO `script_texts` (`entry`,`content_default`,`sound`,`type`,`language`,`emote`,`comment`) VALUES
(-1800997, 'Welcome, $N! There''s an important matter we must discuss.', '0', '0', '0', '3', 'Booster greeting SAY_GREET_BOOSTER'),
(-1800998, 'Hey $N, over here! I have everything you need to be prepared!', '0', '0', '0', '25', 'enlistment officer SAY_GREET_70'),
(-1800999, '$N! Come talk to me before you go onward!', '0', '0', '0', '25', 'enlistment officer SAY_GREET');

REPLACE INTO `mangos_string` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`) VALUES 
(11100, 'Transmogrifications removed from equipped items', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(11101, 'You have no transmogrified items equipped', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(11102, '%s transmogrification removed', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(11103, 'No transmogrification on %s slot', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(11104, '%s transmogrified', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(11105, 'Selected items are not suitable', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(11106, 'Selected item does not exist', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(11107, 'Equipment slot is empty', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(11108, 'Head', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(11109, 'Shoulders', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(11110, 'Shirt', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(11111, 'Chest', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(11112, 'Waist', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(11113, 'Legs', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(11114, 'Feet', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(11115, 'Wrists', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(11116, 'Hands', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(11117, 'Back', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(11118, 'Main hand', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(11119, 'Off hand', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(11120, 'Ranged', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(11121, 'Tabard', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(11122, 'Back..', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(11123, 'Remove all transmogrifications', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(11124, 'Remove transmogrifications from all equipped items?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(11125, 'Update menu', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(11126, 'Remove transmogrification', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(11127, 'Remove transmogrification from %s?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(11128, 'Using this item for transmogrify will bind it to you and make it non-refundable and non-tradeable.\r\nDo you wish to continue?\r\n\r\n', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

DELETE FROM `gossip_menu` WHERE `entry`=50404;
DELETE FROM `gossip_menu` WHERE `entry` IN (50000,50100,50200,50300,50201,50202,50203,50204,50205,50402,50401);
DELETE FROM `gossip_menu` WHERE `entry` IN (50409,50411,50412,50413,50414,50415);
DELETE FROM `gossip_menu` WHERE `entry`=50700;
DELETE FROM `gossip_menu` WHERE `entry` IN (51000,51001,51002,51003,51004,51005,51006,51007,51008,51009,51100,51200,51201,50400,50500,50600,50601);
INSERT INTO `gossip_menu` (`entry`, `text_id`, `script_id`, `condition_id`) VALUES
('50404','50410','0','0'), -- Training Dummy Information

('51000','51000','0','0'), -- Portal Main Menu
('51001','51001','0','0'), -- TBC Dungeons
('51005','51005','0','0'), -- TBC Dungeons (cont.)
('51002','51002','0','0'), -- Classic Dungeons
('51003','51003','0','0'), -- Classic Dungeons (cont.)
('51004','51004','0','0'), -- Cities
('51100','51100','0','0'), -- Raid Portal
('50500','50500','0','0'), -- TBC Raids
('50600','50600','0','0'), -- Classic Raids
('51006','51006','0','0'), -- Unknown Wonders
('51007','51007','0','0'), -- Unknown Wonders (Page 2)
('51008','51008','0','0'), -- Unknown Wonders (Page 3)
('51009','51009','0','0'), -- Unknown Wonders (Page 4)
('50601','50601','51200','0'), -- Portal to Funland (only 1 gossip menu option)

-- Primary
('50000','50000','0','0'), -- Raid Messenger
('50100','50100','0','0'), -- Dungeon Messenger
('50200','50200','0','0'), -- Greeter
('50300','50300','0','0'), -- Realm Information
-- Secondary
('50201','50201','0','0'),
('50202','50202','0','0'),
('50203','50203','0','0'),
('50204','50204','0','0'),
('50205','50205','0','0'),
('50402','50402','0','0'), -- Greeter - confirm level boost
('50401','50401','0','0'), -- Greeter - level boost denied due to restrictions

-- Test Realm Overlord
('50409','50409','0','0'),
('50411','50411','0','0'),
('50412','50412','0','0'),
('50413','50413','0','0'),
('50414','50414','0','0'),
('50415','50415','0','0'),

-- Dual Spec Crystal / Magic Talent Switch Gemstone
('50700','50700','0','0');

DELETE FROM `gossip_menu_option` WHERE `menu_id` IN (51000,51001,51002,51003,51004,51005,51006,51007,51008,51009,50500,50600,50601);
DELETE FROM `gossip_menu_option` WHERE `menu_id` IN (50200,50300);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `action_menu_id`, `action_poi_id`, `action_script_id`, `box_coded`, `box_money`, `box_text`, `condition_id`) VALUES
-- Portal to Funland
('50601','1','0','Step through.','100','1','0','0','51200','0','0',NULL,'0'),
-- Greeter
('50200','1','0','Contact Us','100','1','50205','0','0','0','0',NULL,'0'),
('50200','2','7','What is VengeanceWoW?','101','1','50201','0','0','0','0',NULL,'0'),
('50200','3','6','Donating to VengeanceWoW','102','1','50202','0','0','0','0',NULL,'0'),
('50200','4','4','Reporting Game Problems or Bugs','103','1','50203','0','0','0','0',NULL,'0'),
('50200','5','8','Cheating Policy','104','1','50204','0','0','0','0',NULL,'0'),
-- Realm Information
('50300','1','0','Contact Us','1','1','50205','0','0','0','0',NULL,'0'),
('50300','2','7','What is VengeanceWoW?','1','1','50201','0','0','0','0',NULL,'0'),
('50300','3','6','Donating to VengeanceWoW','1','1','50202','0','0','0','0',NULL,'0'),
('50300','4','4','Reporting Game Problems or Bugs','1','1','50203','0','0','0','0',NULL,'0'),
('50300','5','8','Cheating Policy','1','1','50204','0','0','0','0',NULL,'0'),
-- Portal - Main Menu
('51000','0','9','TBC Dungeons','1','1','51001','0','0','0','0',NULL,'0'),
('51000','1','9','Classic Dungeons','1','1','51002','0','0','0','0',NULL,'0'),
('51000','2','9','TBC Raids','1','1','50500','0','0','0','0',NULL,'0'),
('51000','3','9','Classic Raids','1','1','50600','0','0','0','0',NULL,'0'),
('51000','4','8','Cities','1','1','51004','0','0','0','0',NULL,'0'),
('51000','5','6','Mall','1','1','0','0','51200','0','0',NULL,'0'),
('51000','6','3','Unknown Wonders','1','1','51006','0','0','0','0',NULL,'0'),
-- Portal - TBC Dungeons
('51001','0','2','Hellfire Ramparts','1','1','0','0','51019','0','0',NULL,'0'),
('51001','1','2','The Blood Furnace','1','1','0','0','51020','0','0',NULL,'0'),
('51001','2','2','The Shattered Halls','1','1','0','0','51021','0','0',NULL,'0'),
('51001','3','2','The Slave Pens','1','1','0','0','51022','0','0',NULL,'0'),
('51001','4','2','The Underbog','1','1','0','0','51023','0','0',NULL,'0'),
('51001','5','2','The Steamvault','1','1','0','0','51024','0','0',NULL,'0'),
('51001','6','2','Mana-Tombs','1','1','0','0','51025','0','0',NULL,'0'),
('51001','7','4','Page 2 ->','1','1','51005','0','0','0','0',NULL,'0'),
('51001','8','4','Return to Main Menu','1','1','51000','0','0','0','0',NULL,'0'),
-- Portal - TBC Dungeons (cont.)
('51005','0','2','Sethekk Halls','1','1','0','0','51026','0','0',NULL,'0'),
('51005','1','2','Auchenai Crypts','1','1','0','0','51027','0','0',NULL,'0'),
('51005','2','2','Shadow Labyrinth','1','1','0','0','51028','0','0',NULL,'0'),
('51005','3','2','Old Hillsbrad Foothills','1','1','0','0','51029','0','0',NULL,'0'),
('51005','4','2','The Black Morass','1','1','0','0','51030','0','0',NULL,'0'),
('51005','5','2','The Mechanar','1','1','0','0','51031','0','0',NULL,'0'),
('51005','6','2','The Botanica','1','1','0','0','51032','0','0',NULL,'0'),
('51005','7','2','The Arcatraz','1','1','0','0','51033','0','0',NULL,'0'),
('51005','8','4','<- Page 1','1','1','51001','0','0','0','0',NULL,'0'),
('51005','9','4','Return to Main Menu','1','1','51000','0','0','0','0',NULL,'0'),
-- Portal - Classic Dungeons
('51002','11','4','Return to Main Menu','1','1','51000','0','0','0','0',NULL,'0'),
('51002','10','4','Page 2 ->','1','1','51003','0','0','0','0',NULL,'0'),
('51002','9','2','Razorfen Downs','1','1','0','0','51008','0','0',NULL,'0'),
('51002','8','2','Scarlet Monastery','1','1','0','0','51009','0','0',NULL,'0'),
('51002','7','2','Gnomeregan','1','1','0','0','51010','0','0',NULL,'0'),
('51002','6','2','Razorfen Kraul','1','1','0','0','51011','0','0',NULL,'0'),
('51002','5','2','The Stockade','1','1','0','0','51012','0','0',NULL,'0'),
('51002','4','2','Blackfathom Deeps','1','1','0','0','51013','0','0',NULL,'0'),
('51002','3','2','Shadowfang Keep','1','1','0','0','51014','0','0',NULL,'0'),
('51002','2','2','The Deadmines','1','1','0','0','51015','0','0',NULL,'0'),
('51002','1','2','Wailing Caverns','1','1','0','0','51016','0','0',NULL,'0'),
('51002','0','2','Ragefire Chasm','1','1','0','0','51017','0','0',NULL,'0'),
-- Portal - Classic Dungeons (cont.)
('51003','10','4','Return to Main Menu','1','1','51000','0','0','0','0',NULL,'0'),
('51003','9','4','<- Page 1','1','1','51002','0','0','0','0',NULL,'0'),
('51003','8','2','Dire Maul','1','1','0','0','51000','0','0',NULL,'0'),
('51003','7','2','Stratholme','1','1','0','0','51001','0','0',NULL,'0'),
('51003','6','2','Scholomance','1','1','0','0','51002','0','0',NULL,'0'),
('51003','5','2','Lower Blackrock Spire','1','1','0','0','51018','0','0',NULL,'0'),
('51003','4','2','Blackrock Depths','1','1','0','0','51003','0','0',NULL,'0'),
('51003','3','2','Sunken Temple','1','1','0','0','51004','0','0',NULL,'0'),
('51003','2','2','Zul''Farrak','1','1','0','0','51005','0','0',NULL,'0'),
('51003','1','2','Maraudon','1','1','0','0','51006','0','0',NULL,'0'),
('51003','0','2','Uldaman','1','1','0','0','51007','0','0',NULL,'0'),
-- Portal - Cities
('51004','0','2','Stormwind','1','1','0','0','51034','0','0',NULL,'4'),
('51004','1','2','Ironforge','1','1','0','0','51035','0','0',NULL,'4'),
('51004','2','2','Darnassus','1','1','0','0','51036','0','0',NULL,'4'),
('51004','3','2','Exodar','1','1','0','0','51037','0','0',NULL,'4'),
('51004','4','2','Orgrimmar','1','1','0','0','51038','0','0',NULL,'3'),
('51004','5','2','Undercity','1','1','0','0','51039','0','0',NULL,'3'),
('51004','6','2','Thunder Bluff','1','1','0','0','51040','0','0',NULL,'3'),
('51004','7','2','Silvermoon City','1','1','0','0','51041','0','0',NULL,'3'),
('51004','8','2','Shattrath City','1','1','0','0','51042','0','0',NULL,'0'),
('51004','9','2','Booty Bay','1','1','0','0','51043','0','0',NULL,'0'),
('51004','10','2','Gadgetzan','1','1','0','0','51044','0','0',NULL,'0'),
('51004','11','2','Everlook','1','1','0','0','51045','0','0',NULL,'0'),
('51004','12','2','Area 52','1','1','0','0','51046','0','0',NULL,'0'),
('51004','13','4','Return to Main Menu','1','1','51000','0','0','0','0',NULL,'0'),
-- Portal - TBC Raids
('50500','0','2','Karazhan','1','1','0','0','51108','0','0',NULL,'0'),
('50500','1','2','Gruul''s Lair','1','1','0','0','51109','0','0',NULL,'0'),
('50500','2','2','Magtheridon''s Lair','1','1','0','0','51110','0','0',NULL,'0'),
('50500','3','2','Serpentshrine Cavern','1','1','0','0','51111','0','0',NULL,'0'),
('50500','4','2','The Eye','1','1','0','0','51112','0','0',NULL,'0'),
('50500','5','2','Hyjal Summit','1','1','0','0','51113','0','0',NULL,'0'),
('50500','6','2','Black Temple','1','1','0','0','51114','0','0',NULL,'0'),
('50500','7','2','Zul''Aman','1','1','0','0','51115','0','0',NULL,'0'),
('50500','8','2','Sunwell Plateau','1','1','0','0','51116','0','0',NULL,'0'),
('50500','9','4','Return to Main Menu','1','1','51000','0','0','0','0',NULL,'0'),
-- Portal - Classic Raids
('50600','0','2','Upper Blackrock Spire','1','1','0','0','51100','0','0',NULL,'0'),
('50600','1','2','Molten Core','1','1','0','0','51101','0','0',NULL,'0'),
('50600','2','2','Onyxia''s Lair','1','1','0','0','51102','0','0',NULL,'0'),
('50600','3','2','Blackwing Lair','1','1','0','0','51103','0','0',NULL,'0'),
('50600','4','2','Zul''Gurub','1','1','0','0','51104','0','0',NULL,'0'),
('50600','5','2','Ruins of Ahn''Qiraj','1','1','0','0','51105','0','0',NULL,'0'),
('50600','6','2','Temple of Ahn''Qiraj','1','1','0','0','51106','0','0',NULL,'0'),
('50600','7','2','Naxxramas','1','1','0','0','51107','0','0',NULL,'0'),
('50600','8','4','Return to Main Menu','1','1','51000','0','0','0','0',NULL,'0'),
-- Portal - Unknown Wonders (Page 1)
('51006','0','2','Grim Batol','1','1','0','0','51201','0','0',NULL,'0'),
('51006','1','2','Newman''s Landing','1','1','0','0','51202','0','0',NULL,'0'),
('51006','2','2','GM Island','1','1','0','0','51203','0','0',NULL,'0'),
('51006','3','2','GM Prison','1','1','0','0','51204','0','0',NULL,'0'),
('51006','4','2','Testing','1','1','0','0','51205','0','0',NULL,'0'),
('51006','5','2','Stormwind Vault','1','1','0','0','51206','0','0',NULL,'0'),
('51006','6','2','Old Scarlet Monastery','1','1','0','0','51207','0','0',NULL,'0'),
('51006','7','2','Karazhan Crypts','1','1','0','0','51208','0','0',NULL,'0'),
('51006','8','2','Old Ironforge','1','1','0','0','51209','0','0',NULL,'0'),
('51006','9','2','Elwynn Falls','1','1','0','0','51210','0','0',NULL,'0'),
('51006','10','2','Northern Plaguelands','1','1','0','0','51211','0','0',NULL,'0'),
('51006','11','2','Greymane Area','1','1','0','0','51212','0','0',NULL,'0'),
('51006','12','4','<- Page 4','1','1','51009','0','0','0','0',NULL,'0'),
('51006','13','4','Page 2 ->','1','1','51007','0','0','0','0',NULL,'0'),
('51006','14','4','Return to Main Menu','1','1','51000','0','0','0','0',NULL,'0'),
-- Portal - Unknown Wonders (Page 2)
('51007','0','2','Arathi Highlands Farm','1','1','0','0','51214','0','0',NULL,'0'),
('51007','1','2','Sunspire Top','1','1','0','0','51215','0','0',NULL,'0'),
('51007','2','2','Stranglethorn Bengal Tiger Cave','1','1','0','0','51216','0','0',NULL,'0'),
('51007','3','2','Above Undercity Glitch','1','1','0','0','51217','0','0',NULL,'0'),
('51007','4','2','Blackchar Cave','1','1','0','0','51218','0','0',NULL,'0'),
('51007','5','2','Ortell''s Hideout','1','1','0','0','51219','0','0',NULL,'0'),
('51007','6','2','Wetlands Help Mountain','1','1','0','0','51220','0','0',NULL,'0'),
('51007','7','2','Azshara Crater','1','1','0','0','51221','0','0',NULL,'0'),
('51007','8','2','Isolated Wetlands Town','1','1','0','0','51222','0','0',NULL,'0'),
('51007','9','2','Isolated Dun Morogh','1','1','0','0','51223','0','0',NULL,'0'),
('51007','10','2','Isolated Burning Steppes','1','1','0','0','51224','0','0',NULL,'0'),
('51007','11','2','Tanaris Underwater Village','1','1','0','0','51225','0','0',NULL,'0'),
('51007','12','4','<- Page 1','1','1','51006','0','0','0','0',NULL,'0'),
('51007','13','4','Page 3 ->','1','1','51008','0','0','0','0',NULL,'0'),
('51007','14','4','Return to Main Menu','1','1','51000','0','0','0','0',NULL,'0'),
-- Portal - Unknown Wonders (Page 3)
('51008','0','2','Isolated Tirisfal Glades','1','1','0','0','51226','0','0',NULL,'0'),
('51008','1','2','Deadmines - Outland Alpha','1','1','0','0','51227','0','0',NULL,'0'),
('51008','2','2','Shadowfang Keep - Silverpine Forest','1','1','0','0','51228','0','0',NULL,'0'),
('51008','3','2','Zul''Farrak - Tanaris','1','1','0','0','51229','0','0',NULL,'0'),
('51008','4','2','Zul''Gurub - Altar Of Storms','1','1','0','0','51230','0','0',NULL,'0'),
('51008','5','2','Naxxramas - Northrend','1','1','0','0','51231','0','0',NULL,'0'),
('51008','6','2','Emerald Dream','1','1','0','0','51232','0','0',NULL,'0'),
('51008','7','2','Emerald Dream - Statue','1','1','0','0','51233','0','0',NULL,'0'),
('51008','8','2','Emerald Dream - Verdant Fields','1','1','0','0','51234','0','0',NULL,'0'),
('51008','9','2','Emerald Dream - Forest','1','1','0','0','51235','0','0',NULL,'0'),
('51008','10','2','Emerald Dream - Plains','1','1','0','0','51236','0','0',NULL,'0'),
('51008','11','2','Emerald Dream - Mountains','1','1','0','0','51237','0','0',NULL,'0'),
('51008','12','4','<- Page 2','1','1','51007','0','0','0','0',NULL,'0'),
('51008','13','4','Page 4 ->','1','1','51009','0','0','0','0',NULL,'0'),
('51008','14','4','Return to Main Menu','1','1','51000','0','0','0','0',NULL,'0'),
-- Portal - Unknown Wonders (Page 4)
('51009','0','2','Uninstanced Ahn''Qiraj','1','1','0','0','51213','0','0',NULL,'0'),
('51009','1','2','Hyjal','1','1','0','0','51240','0','0',NULL,'0'),
('51009','2','2','Silithus Farm','1','1','0','0','51241','0','0',NULL,'0'),
('51009','3','2','Stonetalon Logging Camp','1','1','0','0','51242','0','0',NULL,'0'),
('51009','4','2','South Seas Islands','1','1','0','0','51243','0','0',NULL,'0'),
('51009','5','2','Forgotten Gnome Camp','1','1','0','0','51244','0','0',NULL,'0'),
('51009','6','2','Deeprun Tram Aquarium','1','1','0','0','51245','0','0',NULL,'0'),
('51009','7','2','Gurubashi Arena','1','1','0','0','51246','0','0',NULL,'0'),
('51009','8','2','PLACEHOLDER1','1','1','0','0','-1','0','0',NULL,'0'),
('51009','9','2','PLACEHOLDER2','1','1','0','0','-1','0','0',NULL,'0'),
('51009','10','2','Designer Island','1','1','0','0','51238','0','0',"WARNING: Must install patch-3.MPQ in game client's Data directory or you will receive a WoWError upon porting here!",'0'),
('51009','11','2','Programmer Isle','1','1','0','0','51239','0','0',"WARNING: Must install patch-3.MPQ in game client's Data directory or you will receive a WoWError upon porting here!",'0'),
('51009','12','4','<- Page 3','1','1','51008','0','0','0','0',NULL,'0'),
('51009','13','4','Page 1 ->','1','1','51006','0','0','0','0',NULL,'0'),
('51009','14','4','Return to Main Menu','1','1','51000','0','0','0','0',NULL,'0');

-- Portal Scripts
DELETE FROM `dbscripts_on_gossip` WHERE `id` BETWEEN 51000 AND 51046;
DELETE FROM `dbscripts_on_gossip` WHERE `id` BETWEEN 51100 AND 51116;
DELETE FROM `dbscripts_on_gossip` WHERE `id` BETWEEN 51200 AND 51246;
INSERT INTO `dbscripts_on_gossip` (`id`, `delay`, `command`, `datalong`, `datalong2`, `buddy_entry`, `search_radius`, `data_flags`, `dataint`, `dataint2`, `dataint3`, `dataint4`, `x`, `y`, `z`, `o`, `comments`) VALUES
-- Dungeons (Classic)
(51000, 0, 6, 1,   0,0,0,0,0,0,0,0,-3528.903,1125.048,161.02308,3.1416,  'Teleport Player to Dire Maul'),
(51001, 0, 6, 329, 0,0,0,0,0,0,0,0,3395.09,-3380.25,142.702,0.1,         'Teleport Player to Stratholme'),
(51002, 0, 6, 289, 0,0,0,0,0,0,0,0,199.4204,123.858,134.9103,4.76032,    'Teleport Player to Scholomance'),
(51018, 0, 6, 229, 0,0,0,0,0,0,0,0,78.8333,-238.0234,54.6281,4.745,      'Teleport Player to Lower Blackrock Spire'),
(51003, 0, 6, 230, 0,0,0,0,0,0,0,0,458.32,26.52,-70.67346,4.95,          'Teleport Player to Blackrock Depths'),
(51004, 0, 6, 109, 0,0,0,0,0,0,0,0,-319.24,99.9,-131.8501,3.19,          'Teleport Player to Sunken Temple'),
(51005, 0, 6, 209, 0,0,0,0,0,0,0,0,1222.613,841.325,9.01058,6.2,         'Teleport Player to ZulFarrak'),
(51006, 0, 6, 349, 0,0,0,0,0,0,0,0,751.79,-602.024,-33.2435,6.25698,     'Teleport Player to Maraudon'),
(51007, 0, 6, 70,  0,0,0,0,0,0,0,0,-227.188,58.49,-46.0383,1.49839,      'Teleport Player to Uldaman'),
(51008, 0, 6, 129, 0,0,0,0,0,0,0,0,2592.711,1094.69,52.64732,4.81304,    'Teleport Player to Razorfen Downs'),
(51009, 0, 6, 0,   0,0,0,0,0,0,0,0,2844.341,-695.083,139.32993,5.06599,  'Teleport Player to Scarlet Monastery'),
(51010, 0, 6, 90,  0,0,0,0,0,0,0,0,-339.867,1.035,-152.8455,2.89488,     'Teleport Player to Gnomeragen'),
(51011, 0, 6, 47,  0,0,0,0,0,0,0,0,1944.7256,1552.7025,81.85492,1.24413, 'Teleport Player to Razorfen Kraul'),
(51012, 0, 6, 47,  0,0,0,0,0,0,0,0,57.215,0.691,-19.4861,6.26,           'Teleport Player to The Stockades'),
(51013, 0, 6, 48,  0,0,0,0,0,0,0,0,-155.268,91.362,-43.5681,4.27396,     'Teleport Player to Blackfathom Deeps'),
(51014, 0, 6, 33,  0,0,0,0,0,0,0,0,-222.864,2125.124,80.43445,1.41152,   'Teleport Player to Shadowfang Keep'),
(51015, 0, 6, 36,  0,0,0,0,0,0,0,0,-20.677,-377.139,60.83653,2.70823,    'Teleport Player to The Deadmines'),
(51016, 0, 6, 43,  0,0,0,0,0,0,0,0,-157.87,131.442,-74.3203,5.97608,     'Teleport Player to Wailing Caverns'),
(51017, 0, 6, 389, 0,0,0,0,0,0,0,0,-0.1478,-24.0,-20.5947,4.23292,       'Teleport Player to Ragefire Chasm'),
-- Dungeons (TBC)
(51019, 0, 6, 543, 0, 0, 0, 0, 0, 0, 0, 0, -1355.24, 1641.12, 68.2491, 1.12531,         'Teleport Player to Hellfire Ramparts'),
(51020, 0, 6, 542, 0, 0, 0, 0, 0, 0, 0, 0, -3.9967, 14.6363, -44.8009, 4.88748,         'Teleport Player to Blood Furnace'),
(51021, 0, 6, 540, 0, 0, 0, 0, 0, 0, 0, 0, -40.8716, -19.7538, -13.8065, 1.11133,       'Teleport Player to Shattered Halls'),
(51022, 0, 6, 547, 0, 0, 0, 0, 0, 0, 0, 0, 120.101, -131.957, -0.801547, 1.47574,       'Teleport Player to Slave Pens'),
(51023, 0, 6, 546, 0, 0, 0, 0, 0, 0, 0, 0, 9.71391, -16.2008, -2.75334, 5.57082,        'Teleport Player to Underbog'),
(51024, 0, 6, 545, 0, 0, 0, 0, 0, 0, 0, 0, -13.8425, 6.7542, -4.2586, 0,                'Teleport Player to Steamvault'),
(51025, 0, 6, 557, 0, 0, 0, 0, 0, 0, 0, 0, 0.0191, 0.9478, -0.9543, 3.03164,            'Teleport Player to Mana Tombs'),
(51026, 0, 6, 556, 0, 0, 0, 0, 0, 0, 0, 0, -4.6811, -0.0930796, 0.0062, 0.0353424,      'Teleport Player to Sethekk Halls'),
(51027, 0, 6, 558, 0, 0, 0, 0, 0, 0, 0, 0, -21.8975, 0.16, -0.1206, 0.0353412,          'Teleport Player to Auchenai Crypts'),
(51028, 0, 6, 555, 0, 0, 0, 0, 0, 0, 0, 0, 0.488033, -0.215935, -1.12788, 3.15888,      'Teleport Player to Shadow Labyrinth'),
(51029, 0, 6, 560, 0, 0, 0, 0, 0, 0, 0, 0, 2741.68, 1312.64, 14.0413, 2.79253,          'Teleport Player to Old Hillsbrad Foothills'),
(51030, 0, 6, 269, 0, 0, 0, 0, 0, 0, 0, 0, -1491.61, 7056.51, 32.1011, 1.74533,         'Teleport Player to Black Morass'),
(51031, 0, 6, 554, 0, 0, 0, 0, 0, 0, 0, 0, -28.906, 0.680314, -1.81282, 0.0345509,      'Teleport Player to The Mechanar'),
(51032, 0, 6, 553, 0, 0, 0, 0, 0, 0, 0, 0, 40.0395, -28.613, -1.1189, 2.35856,          'Teleport Player to The Botanica'),
(51033, 0, 6, 552, 0, 0, 0, 0, 0, 0, 0, 0, -1.23165, 0.0143459, -0.204293, 0.0157123,   'Teleport Player to The Arcatraz'),
-- Cities
('51034','0','6','0','0','0','0','0','0','0','0','0',-8852.23,639.835,96.3843,0.958539,     'Teleport Player to Stormwind'),
('51035','0','6','0','0','0','0','0','0','0','0','0',-4895.91,-962.374,501.448,3.02565,     'Teleport Player to Ironforge'),
('51036','0','6','1','0','0','0','0','0','0','0','0',9941.17,2500.68,1317.96,4.15784,       'Teleport Player to Darnassus'),
('51037','0','6','530','0','0','0','0','0','0','0','0',-3905.68,-11610,-138.054,3.82175,    'Teleport Player to Exodar'),
('51038','0','6','1','0','0','0','0','0','0','0','0',1588.4,-4394.55,6.46053,4.92048,       'Teleport Player to Orgrimmar'),
('51039','0','6','0','0','0','0','0','0','0','0','0',1563.83,263.1,-43.1027,5.59063,        'Teleport Player to Undercity'),
('51040','0','6','1','0','0','0','0','0','0','0','0',-1265.43,49.056,127.614,0.284943,      'Teleport Player to Thunder Bluff'),
('51041','0','6','530','0','0','0','0','0','0','0','0',9513.37,-7300.74,14.4663,3.22001,    'Teleport Player to Silvermoon City'),
('51042','0','6','530','0','0','0','0','0','0','0','0',-1972.12, 5107.44, 7.5194, 4.3598,   'Teleport Player to Shattrath City'),
('51043','0','6','0','0','0','0','0','0','0','0','0',-14353.142,409.47,6.63197,1.895538,    'Teleport Player to Booty Bay'),
('51044','0','6','1','0','0','0','0','0','0','0','0',-7222.671,-3826.853,9.3328,1.0404,     'Teleport Player to Gadgetzan'),
('51045','0','6','1','0','0','0','0','0','0','0','0',6675.0527,-4648.43,721.5831,6.11709,   'Teleport Player to Everlook'),
('51046','0','6','530','0','0','0','0','0','0','0','0',3084.555,3717.161,142.8485,4.9769,   'Teleport Player to Area 52'),
-- Raids (Classic)
('51100','0','6','229','0','0','0','0','0','0','0','0',131.8227,-319.4949,70.95558,6.21762,         'Teleport Player to Upper Blackrock Spire'),
('51101','0','6','409','0','0','0','0','0','0','0','0',1069.607,-487.516,-108.41582,5.4747,         'Teleport Player to Molten Core'),
('51102','0','6','249','0','0','0','0','0','0','0','0',29.598,-66.9944,-6.25016,4.74729,            'Teleport Player to Onyxia Lair'),
('51103','0','6','469','0','0','0','0','0','0','0','0',-7665.2319,-1101.9293,-399.67914,0.50152,    'Teleport Player to Blackwing Lair'),
('51104','0','6','309','0','0','0','0','0','0','0','0',-11916.952,-1240.7623,92.53341,4.59694,      'Teleport Player to Zul''Gurub'),
('51105','0','6','509','0','0','0','0','0','0','0','0',-8436.53,1519.17,31.907,2.61799,             'Teleport Player to Ruins of Ahn''Qiraj (AQ20)'),
('51106','0','6','531','0','0','0','0','0','0','0','0',-8221.35,2014.34,129.071,0.872665,           'Teleport Player to Temple of Ahn''Qiraj (AQ40)'),
('51107','0','6','533','0','0','0','0','0','0','0','0',3005.87,-3435.01,293.882,0,                  'Teleport Player to Naxxramas'),
-- Raids (TBC)
('51108','0','6','532','0','0','0','0','0','0','0','0',-11101.8,-1998.31,49.8927,0.690367,  'Teleport Player to Karazhan'),
('51109','0','6','565','0','0','0','0','0','0','0','0',62.7842,35.462,-3.9835,1.41844,      'Teleport Player to Gruul''s Lair'),
('51110','0','6','544','0','0','0','0','0','0','0','0',187.843,35.9232,67.9252,4.79879,     'Teleport Player to Magtheridon''s Lair'),
('51111','0','6','548','0','0','0','0','0','0','0','0',2.5343,-0.022318,821.727,0.004512,   'Teleport Player to Serpentshrine Cavern'),
('51112','0','6','550','0','0','0','0','0','0','0','0',-10.8021,-1.15045,-2.42833,6.22821,  'Teleport Player to The Eye'),
('51113','0','6','534','0','0','0','0','0','0','0','0',4252.72,-4226.16,868.222,2.35619,    'Teleport Player to Hyjal Summit'),
('51114','0','6','564','0','0','0','0','0','0','0','0',96.4462,1002.35,-86.9984,6.15675,    'Teleport Player to Black Temple'),
('51115','0','6','568','0','0','0','0','0','0','0','0',120.7,1776,43.455,4.7713,            'Teleport Player to Zul''Aman'),
('51116','0','6','580','0','0','0','0','0','0','0','0',1790.65,925.6699,15.15,3.1,          'Teleport Player to Sunwell Plateau'),
-- Mall
('51200','0','6','0','0','0','0','0','0','0','0','0',4271.622,-2778.375,5.554479,0.539123,  'Teleport Player to Mall'), -- Quel'Thalas
-- Unknown Wonders
('51201','0','6','0','0','0','0','0','0','0','0','0',-4258.8774,-3286.604,240.606,5.158,    'Teleport Player to Grim Batol'),
('51202','0','6','0','0','0','0','0','0','0','0','0',-6166,-772,421,0,                      'Teleport Player to Newman''s Landing'),
('51203','0','6','1','0','0','0','0','0','0','0','0',16226.2,16257,13.2022,1.65007,         'Teleport Player to GM Island'),
('51204','0','6','1','0','0','0','0','0','0','0','0',16226.4,16403.4,-64.38,3.155,          'Teleport Player to GM Prison'),
('51205','0','6','13','0','0','0','0','0','0','0','0',-0.310414,0.107129,-100.538,2.94612,  'Teleport Player to Testing'),
('51206','0','6','35','0','0','0','0','0','0','0','0',-1,52,-27,0,                          'Teleport Player to Stormwind Vault'),
('51207','0','6','44','0','0','0','0','0','0','0','0',77,-1,20,0,                           'Teleport Player to Old Scarlet Monastery'),
('51208','0','6','0','0','0','0','0','0','0','0','0',-11069,-1795,54,0,                     'Teleport Player to Karazhan Crypts'),
('51209','0','6','0','0','0','0','0','0','0','0','0',-4821,-975,464.709,0,                  'Teleport Player to Old Ironforge'),
('51210','0','6','0','0','0','0','0','0','0','0','0',-8322,-340,145,0,                      'Teleport Player to Elwynn Falls'),
('51211','0','6','0','0','0','0','0','0','0','0','0',3852,-3565,45.549,0,                   'Teleport Player to Northern Plaguelands'),
('51212','0','6','0','0','0','0','0','0','0','0','0',-979,1579,52.705,0,                    'Teleport Player to Greymane Area'),
('51213','0','6','1','0','0','0','0','0','0','0','0',-9479,1783,49.945,0,                   'Teleport Player to Uninstanced Ahn''Qiraj'),
('51214','0','6','0','0','0','0','0','0','0','0','0',-1817,-4205,3,0,                       'Teleport Player to Arathi Highlands Farm'),
('51215','0','6','530','0','0','0','0','0','0','0','0',10389.178,-6402.809,161.0528,0,      'Teleport Player to Sunspire Top'),
('51216','0','6','0','0','0','0','0','0','0','0','0',-12878,-1408,120,0,                    'Teleport Player to Stranglethorn Bengal Tiger Cave'),
('51217','0','6','0','0','0','0','0','0','0','0','0',1545,150,61.591,0,                     'Teleport Player to Above Undercity Glitch'),
('51218','0','6','0','0','0','0','0','0','0','0','0',-7347,-642,294.585,0,                  'Teleport Player to Blackchar Cave'),
('51219','0','6','0','0','0','0','0','0','0','0','0',-5314,-2512,484,0,                     'Teleport Player to Ortell''s Hideout'),
('51220','0','6','0','0','0','0','0','0','0','0','0',-3857,-3485,579.25,0,                  'Teleport Player to Wetlands Help Mountain'),
('51221','0','6','37','0','0','0','0','0','0','0','0',128.205,135.136,236.11,4.59132,       'Teleport Player to Azshara Crater'), -- (1003, 281, 327)
('51222','0','6','0','0','0','0','0','0','0','0','0',-4033.5546,-1466.882,169.8629,1.288,   'Teleport Player to Isolated Wetlands Town'),
('51223','0','6','0','0','0','0','0','0','0','0','0',-4897,836,390.7,0,                     'Teleport Player to Isolated Dun Morogh'),
('51224','0','6','0','0','0','0','0','0','0','0','0',-7815,-4266,131.5,0,                   'Teleport Player to Isolated Burning Steppes'),
('51225','0','6','1','0','0','0','0','0','0','0','0',-9619,-5594,-496,0,                    'Teleport Player to Tanaris Underwater Village'),
('51226','0','6','0','0','0','0','0','0','0','0','0',2074,2306,131.5,0,                     'Teleport Player to Isolated Tirisfal Glades'),
('51227','0','6','36','0','0','0','0','0','0','0','0',-1639.989,542.354,7.937,2.764,        'Teleport Player to Deadmines - Outland Alpha'),
('51228','0','6','33','0','0','0','0','0','0','0','0',-588,1087,108,0,                      'Teleport Player to Shadowfang Keep - Silverpine Forest'),
('51229','0','6','209','0','0','0','0','0','0','0','0',1075,860,9.6865,0,                   'Teleport Player to Zul''Farrak - Tanaris'),
('51230','0','6','309','0','0','0','0','0','0','0','0',-12029,-2584,-29,0,                  'Teleport Player to Zul''Gurub - Altar Of Storms'),
('51231','0','6','533','0','0','0','0','0','0','0','0',3598,-4523,198.864,0,                'Teleport Player to Naxxramas - Northrend'),
('51232','0','6','169','0','0','0','0','0','0','0','0',-366.091,3097.86,92.317,0.0487625,   'Teleport Player to Emerald Dream'), -- (458, -377, 92)
('51233','0','6','169','0','0','0','0','0','0','0','0',2781.57,3006.76,23.2219,0.5,         'Teleport Player to Emerald Dream - Statue'),
('51234','0','6','169','0','0','0','0','0','0','0','0',-2128.12,-1005.89,132.213,0.5,       'Teleport Player to Emerald Dream - Verdant Fields'), -- (-1650, 577, 125)
('51235','0','6','169','0','0','0','0','0','0','0','0',2732.93,-3319.63,101.284,0.5,        'Teleport Player to Emerald Dream - Forest'), -- (2965, -3040, 98)
('51236','0','6','169','0','0','0','0','0','0','0','0',-3824,3367,133,0.5,                  'Teleport Player to Emerald Dream - Plains'),
('51237','0','6','169','0','0','0','0','0','0','0','0',3225,3046,23,0.5,                    'Teleport Player to Emerald Dream - Mountains'),
('51238','0','6','451','0','0','0','0','0','0','0','0',16303.5,-16173.5,40.4365,0.539123,   'Teleport Player to Designer Island'),
('51239','0','6','451','0','0','0','0','0','0','0','0',16304,16318,69.446,0.539123,         'Teleport Player to Programmer Isle'),
('51240','0','6','1','0','0','0','0','0','0','0','0',4654,-3772,944,0.5,                    'Teleport Player to Hyjal'),
('51241','0','6','1','0','0','0','0','0','0','0','0',-10706.474,2483.685,7.921,3.4365,      'Teleport Player to Silithus Farm'),
('51242','0','6','1','0','0','0','0','0','0','0','0',1951.512,1530.4755,247.2881,0.5,       'Teleport Player to Stonetalon Logging Camp'),
('51243','0','6','1','0','0','0','0','0','0','0','0',-11649.1387,-4698.392,10.5532,4.7795,  'Teleport Player to South Seas Islands'),
('51244','0','6','0','0','0','0','0','0','0','0','0',-5933,452,509,4.7795,                  'Teleport Player to Forgotten Gnome Camp'),
('51245','0','6','369','0','0','0','0','0','0','0','0',3.297,1382.282,-100.303,4.667675,    'Teleport Player to Deeprun Tram Aquarium'),
('51246','0','6','0','0','0','0','0','0','0','0','0',-13181.8,339.356,42.9805,1.18013,      'Teleport Player to Gurubashi Arena');

-- completely custom Deserter spells used as GM tools
DELETE FROM `spell_template` WHERE `Id` IN (90000,90001,90002,90003,90004);
INSERT INTO `spell_template` (Id, Category, CastUI, Dispel, Mechanic, Attributes, AttributesEx, AttributesEx2, AttributesEx3, AttributesEx4, AttributesEx5, AttributesEx6, Stances, StancesNot, Targets, TargetCreatureType, RequiresSpellFocus, FacingCasterFlags, CasterAuraState, TargetAuraState, CasterAuraStateNot, TargetAuraStateNot, CastingTimeIndex, RecoveryTime, CategoryRecoveryTime, InterruptFlags, AuraInterruptFlags, ChannelInterruptFlags, ProcFlags, ProcChance, ProcCharges, MaxLevel, BaseLevel, SpellLevel, DurationIndex, PowerType, ManaCost, ManaCostPerlevel, ManaPerSecond, ManaPerSecondPerLevel, RangeIndex, Speed, ModalNextSpell, StackAmount, Totem1, Totem2, Reagent1, Reagent2, Reagent3, Reagent4, Reagent5, Reagent6, Reagent7, Reagent8, ReagentCount1, ReagentCount2, ReagentCount3, ReagentCount4, ReagentCount5, ReagentCount6, ReagentCount7, ReagentCount8, EquippedItemClass, EquippedItemSubClassMask, EquippedItemInventoryTypeMask, Effect1, Effect2, Effect3, EffectDieSides1, EffectDieSides2, EffectDieSides3, EffectBaseDice1, EffectBaseDice2, EffectBaseDice3, EffectDicePerLevel1, EffectDicePerLevel2, EffectDicePerLevel3, EffectRealPointsPerLevel1, EffectRealPointsPerLevel2, EffectRealPointsPerLevel3, EffectBasePoints1, EffectBasePoints2, EffectBasePoints3, EffectMechanic1, EffectMechanic2, EffectMechanic3, EffectImplicitTargetA1, EffectImplicitTargetA2, EffectImplicitTargetA3, EffectImplicitTargetB1, EffectImplicitTargetB2, EffectImplicitTargetB3, EffectRadiusIndex1, EffectRadiusIndex2, EffectRadiusIndex3, EffectApplyAuraName1, EffectApplyAuraName2, EffectApplyAuraName3, EffectAmplitude1, EffectAmplitude2, EffectAmplitude3, EffectMultipleValue1, EffectMultipleValue2, EffectMultipleValue3, EffectChainTarget1, EffectChainTarget2, EffectChainTarget3, EffectItemType1, EffectItemType2, EffectItemType3, EffectMiscValue1, EffectMiscValue2, EffectMiscValue3, EffectMiscValueB1, EffectMiscValueB2, EffectMiscValueB3, EffectTriggerSpell1, EffectTriggerSpell2, EffectTriggerSpell3, EffectPointsPerComboPoint1, EffectPointsPerComboPoint2, EffectPointsPerComboPoint3, SpellVisual, SpellIconID, ActiveIconID, SpellPriority, SpellName, SpellName2, SpellName3, SpellName4, SpellName5, SpellName6, SpellName7, SpellName8, SpellName9, SpellName10, SpellName11, SpellName12, SpellName13, SpellName14, SpellName15, SpellName16, Rank1, Rank2, Rank3, Rank4, Rank5, Rank6, Rank7, Rank8, Rank9, Rank10, Rank11, Rank12, Rank13, Rank14, Rank15, Rank16, ManaCostPercentage, StartRecoveryCategory, StartRecoveryTime, MaxTargetLevel, SpellFamilyName, SpellFamilyFlags, MaxAffectedTargets, DmgClass, PreventionType, StanceBarOrder, DmgMultiplier1, DmgMultiplier2, DmgMultiplier3, MinFactionId, MinReputation, RequiredAuraVision, TotemCategory1, TotemCategory2, AreaId, SchoolMask, IsServerSide, AttributesServerside) VALUES
-- 26013 Deserter - 15 minutes
-- 30 minutes
('90000', '0', '0', '0', '0', '612368640', '136', '5', '269484032', '4', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '101', '0', '0', '0', '0', '30', '0', '0', '0', '0', '0', '6', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '0', '6', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '25', '0', '0', '0', '0', '0', '0', '0', '0', '4', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '958', '0', '0', 'Deserter 30 minutes', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '1', '1', '0', '0', '0', '0', '0', '0', '64', '0', '0'),
-- 1 hour
('90001', '0', '0', '0', '0', '612368640', '136', '5', '269484032', '4', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '101', '0', '0', '0', '0', '42', '0', '0', '0', '0', '0', '6', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '0', '6', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '25', '0', '0', '0', '0', '0', '0', '0', '0', '4', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '958', '0', '0', 'Deserter 1 hour', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '1', '1', '0', '0', '0', '0', '0', '0', '64', '0', '0'),
-- 2 hours
('90002', '0', '0', '0', '0', '612368640', '136', '5', '269484032', '4', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '101', '0', '0', '0', '0', '367', '0', '0', '0', '0', '0', '6', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '0', '6', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '25', '0', '0', '0', '0', '0', '0', '0', '0', '4', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '958', '0', '0', 'Deserter 2 hours', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '1', '1', '0', '0', '0', '0', '0', '0', '64', '0', '0'),
-- 18 hours
('90003', '0', '0', '0', '0', '612368640', '136', '5', '269484032', '4', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '101', '0', '0', '0', '0', '580', '0', '0', '0', '0', '0', '6', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '0', '6', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '25', '0', '0', '0', '0', '0', '0', '0', '0', '4', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '958', '0', '0', 'Deserter 18 hours', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '1', '1', '0', '0', '0', '0', '0', '0', '64', '0', '0'),
-- 1 week
('90004', '0', '0', '0', '0', '612368640', '136', '5', '269484032', '4', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '101', '0', '0', '0', '0', '225', '0', '0', '0', '0', '0', '6', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '0', '6', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '25', '0', '0', '0', '0', '0', '0', '0', '0', '4', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '958', '0', '0', 'Deserter 1 week', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '1', '1', '0', '0', '0', '0', '0', '0', '64', '0', '0');

DELETE FROM `conditions` WHERE `condition_entry`=15033;
DELETE FROM `conditions` WHERE `condition_entry`=15035;
DELETE FROM `conditions` WHERE `condition_entry` BETWEEN 15046 AND 15053;
INSERT INTO `conditions` (`condition_entry`, `type`, `value1`, `value2`, `flags`) VALUES
-- 515 (Level <= 57)
-- 986 (Level >= 20)
(15033, 14, 0, 2+4+16+64+128+256+1024, 0), -- mana users only
(15035, 2, 21967, 1, 1), -- Does not have 1 Magic Talent Switch Gemstone
(15046, 15, 58, 0, 0), -- Player Level == 58
(15047, -1, 15046, 19, 0), -- Warlock AND Player Level == 58
(15048, 14, 0, 1+4+8, 0), -- Ranged users (Warrior, Rogue, Hunter)
(15049, 15, 60, 2, 0), -- Player Level <= 60
(15050, -1, 15046, 16, 0), -- Player Level == 58 AND Rogue
(15051, -1, 15048, 15049, 0), -- Ranged users AND Player Level <= 60
(15052, -1, 15033, 15049, 0), -- Mana users AND Player Level <= 60
(15053, -1, 15, 15049, 0); -- Hunter AND Player Level <= 60

REPLACE INTO `creature_template` (`Entry`, `Name`, `SubName`, `IconName`, `MinLevel`, `MaxLevel`, `HeroicEntry`, `DisplayId1`, `DisplayId2`, `DisplayId3`, `DisplayId4`, `Faction`, `Scale`, `Family`, `CreatureType`, `InhabitType`, `RegenerateStats`, `RacialLeader`, `NpcFlags`, `UnitFlags`, `DynamicFlags`, `ExtraFlags`, `CreatureTypeFlags`, `SpeedWalk`, `SpeedRun`, `Detection`, `CallForHelp`, `Pursuit`, `Leash`, `Timeout`, `UnitClass`, `Rank`, `Expansion`, `HealthMultiplier`, `PowerMultiplier`, `DamageMultiplier`, `DamageVariance`, `ArmorMultiplier`, `ExperienceMultiplier`, `MinLevelHealth`, `MaxLevelHealth`, `MinLevelMana`, `MaxLevelMana`, `MinMeleeDmg`, `MaxMeleeDmg`, `MinRangedDmg`, `MaxRangedDmg`, `Armor`, `MeleeAttackPower`, `RangedAttackPower`, `MeleeBaseAttackTime`, `RangedBaseAttackTime`, `DamageSchool`, `MinLootGold`, `MaxLootGold`, `LootId`, `PickpocketLootId`, `SkinningLootId`, `KillCredit1`, `KillCredit2`, `MechanicImmuneMask`, `SchoolImmuneMask`, `ResistanceHoly`, `ResistanceFire`, `ResistanceNature`, `ResistanceFrost`, `ResistanceShadow`, `ResistanceArcane`, `PetSpellDataId`, `MovementType`, `TrainerType`, `TrainerSpell`, `TrainerClass`, `TrainerRace`, `TrainerTemplateId`, `VendorTemplateId`, `EquipmentTemplateId`, `GossipMenuId`, `AIName`, `ScriptName`) VALUES
(980049, 'Model Daemon', '', '', '70', '70', '0', '16269', '0', '0', '0', '35', '1', '0', '3', '1', '14', '0', '1', '0', '0', '0', '8', '1', '1.14286', '18', '0', '15000', '0', '0', '1', '1', '1', '11.4515', '1', '1', '1', '1', '1', '1', '1', '0', '0', '0', '0', '0', '0', '15', '0', '0', '2000', '2000', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '', 'modelviewer_daemon'),
(980048, 'Titler', 'Giver of Names', '', 69, 70, 0, 20764, 0, 0, 0, 35, 0.33, 0, 7, 3, 14, 0, 1, 0, 0, 2, 8, 0.666668, 0.857143, 30, 0, 0, 0, 0, 1, 1, 1, 1, 1, 15, 1, 1, 1, 10000, 19000, 0, 0, 4611, 5311, 0, 0, 6792, 2290, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', 'titler'),
(980047, 'Legionnaire Gurak', NULL, NULL, '72', '72', '0', '14732', '0', '0', '0', '29', '1', '0', '7', '3', '14', '0', '1', '36864', '0', 64+4096, '0', '1', '1.14286', '20', '0', '0', '0', '0', '1', '1', '1', '10', '1', '2', '1', '1', '1', '73800', '73800', '0', '0', '559', '720', '61.152', '84.084', '7387', '295', '100', '2000', '2000', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '14720', '0', '', 'crater_reinforcements_spawner'),
(980046, 'Captain Benedict', NULL, NULL, '72', '72', '0', '1688', '0', '0', '0', '11', '1', '0', '7', '3', '14', '0', '1', '36864', '0', 64+4096, '0', '1', '1.14286', '20', '0', '0', '0', '0', '1', '1', '1', '10', '1', '2', '1', '1', '1', '73800', '73800', '0', '0', '559', '720', '61.152', '84.084', '7387', '295', '100', '2000', '2000', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '8891', '0', '', 'crater_reinforcements_spawner'),
(980045, 'Grunt', NULL, NULL, '65', '65', '0', '4259', '4260', '4602', '4601', '29', '1', '0', '7', '3', '14', '0', '0', '36864', '0', 64+4096, '0', '1', '1.14286', '20', '0', '0', '0', '0', '1', '0', '1', '2', '1', '1.6', '1', '1', '1', '11828', '11828', '0', '0', '351', '412', '301.34', '422.17', '5291', '176', '100', '2000', '2000', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '2', '0', '0', '0', '0', '0', '0', '3296', '0', '', ''),
(980044, 'Footman', NULL, NULL, '65', '65', '0', '3167', '5446', '0', '0', '11', '1', '0', '7', '3', '14', '0', '0', '36864', '0', 64+4096, '0', '1', '1.14286', '20', '0', '0', '0', '0', '1', '0', '1', '2', '1', '1.6', '1', '1', '1', '11828', '11828', '0', '0', '351', '412', '301.34', '422.17', '5291', '176', '100', '2000', '2000', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '2', '0', '0', '0', '0', '0', '0', '68', '0', '', ''),
(980043, 'Zip-a-dee-doo-dah', 'Model Viewer', '', '69', '69', '0', '23766', '0', '0', '0', '35', '1', '0', '7', '3', '3', '0', '1', '0', '0', '0', '0', '1', '1.14286', '20', '0', '0', '0', '0', '1', '0', '0', '1', '1', '1', '1', '1', '1', '42', '42', '0', '0', '2', '2', '0', '0', '7', '11', '0', '2000', '2000', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '', 'modelviewer'),
(980042, 'Beginner Training Dummy', '', '', 55, 55, 0, 3019, 0, 0, 0, 7, 1, 0, 9, 3, 3, 0, 0, 0, 0, 64, 0, 1, 1.14286, 20, 0, 0, 0, 0, 1, 0, 1, 200, 1, 1, 1, 1, 1, 1397200, 1397200, 0, 0, 2, 2, 0, 0, 3136, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 8585235, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 'npc_target_dummy'),
(980041, 'Enchantment Crystal', '', '', '1', '1', '0', '22669', '22669', '22669', '22669', '35', '0.25', '0', '7', '3', '3', '0', '1', '0', '0', '0', '0', '1', '1.14286', '20', '0', '0', '0', '0', '1', '0', '0', '1', '1', '1', '1', '1', '1', '42', '42', '0', '0', '2', '2', '0', '0', '7', '11', '0', '2000', '2000', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '', 'enchantment_crystal'),
(980040, 'Paul', 'Stylist', '', '70', '70', '0', '23766', '0', '0', '0', '35', '1', '0', '7', '3', '3', '0', '1', '0', '0', '0', '0', '1', '1.14286', '20', '0', '0', '0', '0', '1', '0', '0', '1', '1', '1', '1', '1', '1', '42', '42', '0', '0', '2', '2', '0', '0', '7', '11', '0', '2000', '2000', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '', 'barber'),
(980039, 'Spirit Healer', '', '', '60', '60', '0', '5233', '0', '0', '0', '35', '1', '0', '7', '1', '14', '0', '16385', '768', '0', '64', '2', '1', '1', '20', '0', '0', '0', '0', '1', '0', '0', '1.35', '1', '1', '1', '1', '1', '4900', '4900', '0', '0', '164', '212', '74.448', '102.366', '3791', '87', '100', '2000', '2000', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '83', 'EventAI', ''),
(980038, 'Arena Battlemaster', '', NULL, '70', '70', '0', '23691', '0', '0', '0', '35', '1', '0', '7', '3', '14', '0', '1048577', '33024', '0', '64', '0', '1', '1.14286', '20', '0', '0', '0', '0', '1', '0', '1', '1', '1', '1', '1', '1', '1', '7200', '7200', '0', '0', '77', '117', '328.696', '458.231', '6792', '45', '100', '2000', '2000', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '8218', '', ''),
(980037, 'Arena Organizer', '', '', '70', '70', '0', '24023', '0', '0', '0', '35', '1', '0', '7', '1', '14', '0', '262145', '768', '0', '0', '0', '1.1', '1.14286', '20', '0', '0', '0', '0', '1', '0', '1', '1', '1', '1', '1', '1', '1', '8300', '8300', '0', '0', '0', '0', '0', '0', '6792', '0', '0', '2000', '2000', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '', ''),
(980036, 'Auctioneer Grizzlin', NULL, NULL, '50', '50', '0', '9132', '0', '0', '0', '855', '1', '0', '7', '1', '14', '0', '2097154', '4096', '0', '65600', '0', '1', '1.14286', '20', '0', '0', '0', '0', '1', '0', '0', '3', '1', '1', '1', '1', '1', '3600', '3600', '2103', '2103', '72', '93', '66.44', '91.355', '2999', '38', '100', '2000', '2000', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '', ''),
(980035, 'Izzy Coppergrab', 'Banker', NULL, '55', '55', '0', '13969', '0', '0', '0', '855', '1', '0', '7', '1', '14', '0', '131073', '0', '0', '65600', '0', '1', '1.14286', '20', '0', '0', '0', '0', '1', '0', '0', '3', '1', '1', '1', '1', '1', '3600', '3600', '0', '0', '96', '125', '70.664', '97.163', '3271', '51', '100', '2000', '2000', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '13917', '0', '', ''),
(980034, 'Paymaster', 'Escape From Wageslavery', '', '70', '70', '0', '23780', '0', '0', '0', '35', '1', '0', '7', '1', '14', '0', '1', '768', '0', '0', '0', '1.1', '1.14286', '20', '0', '0', '0', '0', '8', '0', '1', '1', '1', '1', '1', '1', '1', '1', '1', '0', '0', '0', '0', '0', '0', '3427', '0', '0', '2000', '2000', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '980034', '0', '', 'paymaster'),
(980033, 'Beastmaster', '', '', '70', '70', '0', '24028', '0', '0', '0', '35', '1', '0', '7', '1', '14', '0', '1', '768', '0', '0', '0', '1.1', '1.14286', '20', '0', '0', '0', '0', '2', '0', '1', '1', '1', '1', '1', '1', '1', '1', '1', '0', '0', '0', '0', '0', '0', '5474', '0', '0', '2000', '2000', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '980033', '0', '', 'beastmaster'),
(980032, 'Transmogrification Beacon','','','1','1','0','16946','16946','16946','16946','35','0.5','0','7','3','3','0','1','0','0','0','0','1','1.14286','20','0','0','0','0','1','2','0','1','1','1','1','1','1','42','42','0','0','2','2','0','0','7','11','0','2000','2000','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','','npc_transmogrify'),
(980031, 'Raid Messenger','VengeanceWoW','','73','73','0','24735','24735','24735','24735','35','1','0','7','3','3','0','1','0','0','0','0','1','1.14286','20','0','0','0','0','1','3','1','1','1','1','1','1','1','7588','7588','0','0','2','2','0','0','7','11','0','2000','2000','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','980031','50000','',''),
(980030, 'Dungeon Messenger','VengeanceWoW','','73','73','0','22968','22968','22968','22968','35','1','0','7','3','3','0','1','0','0','0','0','1','1.14286','20','0','0','0','0','1','3','1','1','1','1','1','1','1','7588','7588','0','0','2','2','0','0','7','11','0','2000','2000','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','25167','50100','',''),
(980029, 'Test Realm Overlord', 'Bow Before Your God', '', '1', '1', '0', '134', '0', '0', '0', '35', '5', '0', '7', '3', '3', '0', '1', '0', '0', '0', '0', '1', '1.14286', '20', '0', '0', '0', '0', '1', '2', '0', '1', '1', '1', '1', '1', '1', '42', '42', '0', '0', '2', '2', '0', '0', '7', '11', '0', '2000', '2000', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '50414', '', 'npc_test_realm_overlord'),
(980028, 'Alliance Greeter','Level Booster','','1','1','0','2968','10214','22775','2968','534','1.25','0','7','3','3','0','1','0','0','0','0','1','1.14286','20','0','0','0','0','1','2','0','1','1','1','1','1','1','42','42','0','0','2','2','0','0','7','11','0','2000','2000','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','50200','','npc_booster'),
(980027, 'Horde Greeter','Level Booster','','1','1','0','15730','4059','4523','4526','714','1.25','0','7','3','3','0','1','0','0','0','0','1','1.14286','20','0','0','0','0','1','2','0','1','1','1','1','1','1','42','42','0','0','2','2','0','0','7','11','0','2000','2000','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','50200','','npc_booster'),
(980026, 'Alliance Realm Information','VengeanceWoW','','1','1','0','24755','24755','24755','24755','35','1','0','7','3','3','0','1','0','0','0','0','1','1.14286','20','0','0','0','0','1','2','0','1','1','1','1','1','1','42','42','0','0','2','2','0','0','7','11','0','2000','2000','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','50300','',''),
(980025, 'Horde Realm Information','VengeanceWoW','','1','1','0','11803','11803','11803','11803','35','1','0','7','3','3','0','1','0','0','0','0','1','1.14286','20','0','0','0','0','1','2','0','1','1','1','1','1','1','42','42','0','0','2','2','0','0','7','11','0','2000','2000','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','50300','',''),
(980024, 'Alliance Enlistment Officer', 'Fresh Meat Trainer', '', 1, 1, 0, 12075, 0, 0, 0, 534, 1.25, 0, 7, 3, 3, 0, 945, 768, 0, 0, 0, 1.2, 1.14286, 20, 0, 0, 0, 0, 1, 0, 1, 15, 1, 1, 1, 1, 1, 630, 630, 0, 0, 2, 2, 0, 0, 15, 1, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 980024, 980025, 980024, 0, '', 'npc_enlistment_officer'),
(980023, 'Horde Enlistment Officer', 'Fresh Meat Trainer', '', 1, 1, 0, 12080, 0, 0, 0, 714, 1.25, 0, 7, 3, 3, 0, 945, 768, 0, 0, 0, 1.2, 1.14286, 20, 0, 0, 0, 0, 1, 0, 1, 15, 1, 1, 1, 1, 1, 630, 630, 0, 0, 2, 2, 0, 0, 15, 1, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 980024, 980025, 980023, 0, '', 'npc_enlistment_officer'),
(980022, 'Dual Specialization Crystal', '', '', '1', '1', '0', '11659', '11659', '11659', '11659', '35', '0.25', '0', '7', '3', '3', '0', '1', '0', '0', '0', '0', '1', '1.14286', '20', '0', '0', '0', '0', '1', '0', '0', '1', '1', '1', '1', '1', '1', '42', '42', '0', '0', '2', '2', '0', '0', '7', '11', '0', '2000', '2000', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '50700', '', 'npc_custom_dualspec'),
(980021, 'Grandmaster Training Dummy', '', '', 73, 73, 0, 16074, 0, 0, 0, 7, 1.5, 0, 9, 3, 0, 0, 0, 32768, 0, 64, 4, 1, 1.14286, 20, 0, 0, 0, 0, 1, 1, 1, 1000, 1, 1, 1, 1, 1, 7888000, 7888000, 0, 0, 0, 0, 0, 0, 7684, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8585235, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 'npc_target_dummy'),
(980020, 'Journeyman Training Dummy', '', '', 70, 70, 0, 3019, 0, 0, 0, 7, 1.5, 0, 9, 3, 3, 0, 0, 0, 0, 64, 0, 1, 1.14286, 20, 0, 0, 0, 0, 1, 0, 1, 200, 1, 1, 1, 1, 1, 1397200, 1397200, 0, 0, 2, 2, 0, 0, 4691, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 8585235, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 'npc_target_dummy'),
(980019, 'Novice Training Dummy', '', '', 63, 63, 0, 3019, 0, 0, 0, 7, 1.25, 0, 9, 3, 3, 0, 0, 0, 0, 64, 0, 1, 1.14286, 20, 0, 0, 0, 0, 1, 0, 1, 200, 1, 1, 1, 1, 1, 1397200, 1397200, 0, 0, 2, 2, 0, 0, 4691, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 8585235, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 'npc_target_dummy'),
(980018, 'Lockout Nullifier', '', '', 70, 70, 0, 20984, 0, 0, 0, 35, 1, 0, 7, 3, 3, 0, 1, 139394, 0, 2, 0, 0, 0, 20, 0, 0, 0, 0, 1, 1, -1, 1, 1, 1, 1, 1, 1, 1337, 1337, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 980018, 0, '', 'lockoutnullifier'),
(980017, 'Master Provisioner', '', '', 70, 70, 0, 23765, 0, 0, 0, 35, 1, 0, 7, 1, 14, 0, 128+1, 768, 0, 33554432, 0, 1, 1.14286, 20, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 6986, 6986, 0, 0, 252, 357, 215, 320, 6792, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 980017, 0, '', 'multivendor'),
(980016, 'Tier 5', '', '', 70, 70, 0, 16112, 0, 0, 0, 35, 1, 0, 7, 3, 3, 0, 128, 139394, 0, 2, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 1337, 1337, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 980016, 0, '', ''),
(980015, 'Gems', '', '', 70, 70, 0, 23758, 0, 0, 0, 35, 1, 0, 7, 3, 3, 0, 128+1, 139394, 0, 2+33554432, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 1337, 1337, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 26303, 0, '', 'multivendor'),
(980014, 'Cat', '', '', 1, 1, 0, 5585, 5555, 5556, 5586, 188, 1, 0, 8, 1, 14, 0, 0, 0, 0, 64, 0, 1, 0.857143, 20, 0, 0, 0, 0, 1, 0, 0, 0.01, 1, 1, 1, 1.33333, 1, 8, 8, 0, 0, 2, 2, 1.76, 2.42, 20, 1, 100, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(980013, 'Arena Gear', 'Season 2', '', 70, 70, 0, 22393, 0, 0, 0, 35, 1, 0, 7, 3, 3, 0, 128+1, 139394, 0, 2+33554432, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 1337, 1337, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 980013, 0, '', 'multivendor'),
(980012, 'Race Changer', '', '', 70, 70, 0, 15771, 15767, 15886, 15887, 35, 1, 0, 7, 3, 3, 0, 1, 139394, 0, 2, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 1337, 1337, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 'racechanger'),
(980011, 'Guild Creator', '', '', 70, 70, 0, 103, 0, 0, 0, 35, 1, 0, 7, 3, 3, 0, 129+524288, 139394, 0, 2, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 1337, 1337, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 'guildcreator'),
(980010, 'Avatar of the Elements', '', '', 70, 70, 0, 4607, 0, 0, 0, 35, 1, 0, 7, 3, 3, 0, 128, 139394, 0, 2, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 1337, 1337, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'EventAI', ''),
(980009, 'Impy', 'Demon Trainer', '', 70, 70, 0, 911, 0, 0, 0, 35, 1, 0, 7, 3, 3, 0, 128+1, 139394, 0, 2+33554432, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 1337, 1337, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 980009, 0, '', 'multivendor'),
(980008, 'Mounts', '', '', 70, 70, 0, 6868, 0, 0, 0, 35, 0.5, 0, 7, 3, 3, 0, 128, 139394, 0, 2, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 1337, 1337, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 980008, 0, '', ''),
(980007, 'Poisons', '', '', 70, 70, 0, 7138, 0, 0, 0, 35, 1, 0, 7, 3, 3, 0, 1152, 139394, 0, 2, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 1337, 1337, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 980007, 0, '', ''),
(980006, 'Jewelry', '', '', 70, 70, 0, 7176, 0, 0, 0, 35, 1, 0, 7, 3, 3, 0, 128+1, 139394, 0, 2+33554432, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 1337, 1337, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 'multivendor'),
(980005, 'Off-sets', '', '', 70, 70, 0, 7206, 0, 0, 0, 35, 1, 0, 7, 3, 3, 0, 128+1, 139394, 0, 2+33554432, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 1337, 1337, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 'multivendor'),
(980004, 'Off-hands', '', '', 70, 70, 0, 7171, 0, 0, 0, 35, 1, 0, 7, 3, 3, 0, 128+1, 139394, 0, 2+33554432, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 1337, 1337, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 980004, 0, '', 'multivendor'),
(980003, 'Weapons', '', '', 70, 70, 0, 7211, 0, 0, 0, 35, 1, 0, 7, 3, 3, 0, 128+1, 139394, 0, 2+33554432, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 1337, 1337, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 26309, 0, '', 'multivendor'),
(980002, 'Universal Class Trainer', '', '', 70, 70, 0, 24036, 24032, 24030, 24031, 35, 1, 0, 7, 3, 3, 0, 16+1, 139394, 0, 2+67108864, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 12280, 12280, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 'multitrainer'),
(980001, 'Peasant', '', '', 69, 70, 0, 17330, 0, 0, 0, 1716, 1, 0, 7, 3, 14, 0, 0, 0, 0, 2, 8, 0.666668, 0.857143, 30, 0, 0, 0, 0, 1, 1, 1, 1, 1, 15, 1, 1, 1, 10000, 19000, 0, 0, 4611, 5311, 0, 0, 6792, 2290, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 17931, 0, '', ''),

(185039, 'MV - 185039', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185038, 'MV - 185038', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185037, 'MV - 185037', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185036, 'MV - 185036', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185035, 'MV - 185035', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185034, 'MV - 185034', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185033, 'MV - 185033', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185032, 'MV - 185032', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185031, 'MV - 185031', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185030, 'MV - 185030', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185029, 'MV - 185029', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185028, 'MV - 185028', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185027, 'MV - 185027', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185026, 'MV - 185026', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185025, 'MV - 185025', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185024, 'MV - 185024', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185023, 'MV - 185023', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185022, 'MV - 185022', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185021, 'MV - 185021', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185020, 'MV - 185020', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185019, 'MV - 185019', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185018, 'MV - 185018', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185017, 'MV - 185017', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185016, 'MV - 185016', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185015, 'MV - 185015', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185014, 'MV - 185014', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185013, 'MV - 185013', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185012, 'MV - 185012', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185011, 'MV - 185011', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185010, 'MV - 185010', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185009, 'MV - 185009', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185008, 'MV - 185008', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185007, 'MV - 185007', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185006, 'MV - 185006', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185005, 'MV - 185005', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185004, 'MV - 185004', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185003, 'MV - 185003', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185002, 'MV - 185002', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185001, 'MV - 185001', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', ''),
(185000, 'MV - 185000', NULL, NULL, 70, 70, 0, 93, 0, 0, 0, 35, 1, 0, 0, 3, 3, 0, 128, 0, 0, 128, 0, 0, 0, 20, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '');

UPDATE `creature_template` SET `DisplayIdProbability1`=100 WHERE entry BETWEEN 980001 AND 980049;

-- Set up data for battlemasters (repurposing Blizzlike templates)
UPDATE `creature_template` SET `NpcFlags`=`NpcFlags`|1048576 WHERE `Entry` IN (14623,14879,15972,22516);
UPDATE `creature_template` SET `DisplayId1`=1122, `EquipmentTemplateId`=14623 WHERE `Entry`=14623;
UPDATE `creature_template` SET `DisplayId1`=19924, `EquipmentTemplateId`=14879 WHERE `Entry`=14879;
UPDATE `creature_template` SET `DisplayId1`=10706, `EquipmentTemplateId`=15972 WHERE `Entry`=15972;
UPDATE `creature_template` SET `DisplayId1`=19936, `EquipmentTemplateId`=22516 WHERE `Entry`=22516;
DELETE FROM `battlemaster_entry` WHERE `entry` IN (14623,14879,15972,22516);
INSERT INTO `battlemaster_entry` (`entry`, `bg_template`) VALUES
(14623, 2), -- Warsong Gulch
(14879, 3), -- Arathi Basin
(15972, 1), -- Alterac Valley
(22516, 7); -- Eye of the Storm

REPLACE INTO `creature_template_addon` (`entry`, `mount`, `stand_state`, `sheath_state`, `emote`, `moveflags`, `auras`) VALUES
(980008, 15289, 0, 1, 0, 0, ''), -- Mounts - raptor mount
(980018, 0, 0, 1, 0, 0, ''), -- Lockout Nullifier - unsheath weapons
(980029, 0, 0, 0, 0, 0, '31332 32582'), -- Test Realm Overlord
(980031, 0, 0, 1, 375, 0, '35838'), -- STATE_READY2H, Ghost Visual
(980030, 0, 0, 1, 333, 0, '35838'), -- STATE_READY1H, Ghost Visual
(980028, 0, 0, 0, 0, 0, '35838'), -- Ghost Visual
(980027, 0, 0, 0, 0, 0, '35838'), -- Ghost Visual
(980024, 0, 0, 1, 333, 0, '35838'), -- Ghost Visual, STATE_READY1H
(980023, 0, 0, 1, 333, 0, '35838'), -- Ghost Visual, STATE_READY1H
(980032, 0, 0, 0, 0, 0, '32368'); -- Ethereal Becon Visual

REPLACE INTO `creature_equip_template` (`entry`, `equipentry1`, `equipentry2`, `equipentry3`) VALUES
(980007, 31669, 3696, 0), -- Poisons
(980016, 29996, 30103, 0), -- Tier 5
(980013, 32963, 33313, 0), -- Arena Gear
(980033, 18713, 0, 0), -- Beastmaster
(980034, 12854, 12855, 0), -- Paymaster
(980004, 0, 1168, 0), -- Off-hands
(980008, 19865, 19866, 0), -- Mounts
(980009, 20536, 0, 0), -- Impy
(14623, 20440, 20430, 0), -- Warsong Gulch Battlemaster
(14879, 20220, 0, 0), -- Arathi Basin Battlemaster
(15972, 19323, 0, 0), -- Alterac Valley Battlemaster
(22516, 31318, 0, 0), -- Eye of the Storm Battlemaster
(980018, 32837, 19019, 0), -- Lockout Nullifier
(980024, 12584, 18825, 0), -- Grand Marshal's Longsword, Grand Marshal's Aegis
(980023, 16345, 18826, 0), -- High Warlord's Blade, High Warlord's Shield Wall
(980017, 28487, 2081, 0), -- Master Provisioner: Monster - Work Wrench, Ethereal (Purple Glow), Monster - Torch, Offhand
(980999, 19292, 0, 0), -- Master Provisioner: Last Month's Mutton
(980031, 13706, 35073, 0); -- Monster - Axe, 2H Horde Black Tombstone, Brutal Gladiator's Redoubt

REPLACE INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `faction`, `flags`, `ExtraFlags`, `size`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `CustomData1`, `mingold`, `maxgold`, `ScriptName`) VALUES
(400000, 2, 1327, 'Portal', '', '', 0, 0, 0, 1.4, 0, 0, 0, 51000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400001, 2, 210, 'Training Dummy Information', '', '', 0, 0, 0, 0.75, 0, 0, 2, 50404, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400002, 5, 7735, 'Collision PC Size [x4]', '', '', 35, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400003, 5, 343, 'Teleporter Circle', '', '', 0, 0, 0, 0.75, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''), -- Copy of Dave's Circle 184908 (visual on ground for where players come out of teleport to Blasted Lands)
(400004, 8, 233, 'Forge', '', '', 0, 0, 0, 1, 543, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400005, 8, 166, 'Anvil', '', '', 0, 0, 0, 1, 1, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400006, 7, 39, 'Wooden Chair', '', '', 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400007, 5, 130, 'Weapon Rack', '', '', 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400008, 19, 2190, 'Mailbox', '', '', 855, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400009, 5, 6707, 'PVP HOLIDAY HORDE CTF', '', '', 1375, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400010, 5, 6387, 'PVP HOLIDAY HORDE AV', '', '', 1375, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400011, 5, 6388, 'PVP HOLIDAY HORDE ARATHI', '', '', 1375, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400012, 5, 6135, 'PVP HOLIDAY GENERIC SIGNPOST', '', '', 1375, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400013, 5, 5651, 'PVP HOLIDAY ALLIANCE ARATHI', '', '', 1375, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400014, 5, 6389, 'PVP HOLIDAY ALLIANCE AV', '', '', 114, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400015, 5, 6390, 'PVP HOLIDAY ALLIANCE CTF', '', '', 1375, 0, 0, 0.6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400016, 5, 6508, 'Party Table', '', '', 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400017, 8, 233, 'Forge (invisible focus)', '', '', 0, 0, 0, 1, 3, 10, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400018, 5, 6796, 'Blood Elf Banner', '', '', 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400019, 5, 7268, 'Draenei Banner', '', '', 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400020, 5, 1310, 'Simon Game Aura Yellow', '', '', 0, 0, 0, 1.5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400021, 8, 192, 'Campfire', '', '', 0, 0, 0, 1, 4, 10, 2061, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400022, 34, 7605, 'Guild Vault', '', '', 35, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400023, 5, 6391, 'Ghost Gate', '', '', 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400024, 5, 7735, 'Collision PC Size', '', '', 35, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400025, 3, 36, 'Research Equipment', '', '', 0, 4, 0, 1, 43, 10101, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400026, 10, 335, 'Supply Crate', '', '', 114, 16, 0, 1, 0, 0, 0, 196608, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400027, 2, 7628, 'Portal to Funland', '', '', 0, 0, 0, 1, 0, 0, 0, 50601, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400028, 19, 1907, 'Alliance Mailbox', '', '', 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400029, 19, 2128, 'Horde Mailbox', '', '', 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400030, 19, 2128, 'Horde Gamemaster Mailbox', '', '', 29, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400031, 0, 411, 'Portcullis', '', '', 114, 32, 0, 1.596, 0, 0, 196608, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400032, 19, 1947, 'Alliance Gamemaster Mailbox', '', '', 1732, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400033, 7, 6997, 'Chair', '', '', 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400034, 34, 7605, 'Gamemaster Guild Vault', '', '', 35, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400035, 5, 7681, 'Sunwell Plateau', '', '', 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400036, 5, 1248, 'Peasant Woodpile', '', '', 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400037, 5, 1248, 'Peasant Woodpile (large)', '', '', 114, 0, 0, 1.5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''), 
(400038, 3, 1108, 'Lumber Pile', '', '', 0, 4, 0, 1, 57, 18451, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''), 
(400039, 5, 6738, 'Excavation Tent Pavillion', '', '', 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(400040, 5, 6370, 'Torch', '', '', 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '');

SELECT MAX(`entry`) INTO @max_custom_gameobject_entry FROM `gameobject_template`;
DELETE FROM `gameobject` WHERE `id` BETWEEN 400000 AND @max_custom_gameobject_entry;
DELETE FROM `gameobject` WHERE `guid`=@OGUID+092 AND id=152614; -- special case for Elevator 152614 (uses Blizzlike template)
INSERT INTO `gameobject` (`guid`, `id`, `map`, `spawnMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecsmin`, `spawntimesecsmax`) VALUES
(@OGUID+072, 400040, 0, 1, 4344.26, -2813.37, 4.91783, 2.92274, 0, 0, 0.994019, 0.109207, 25, 25), -- Torch
(@OGUID+073, 400037, 0, 1, 4341.63, -2805.61, 4.80295, 1.33592, 0, 0, 0.619384, 0.785088, 25, 25), -- Peasant Woodpile (large)
(@OGUID+074, 400037, 0, 1, 4340.46, -2805.37, 4.81599, 1.25344, 0, 0, 0.58649, 0.809956, 25, 25), -- Peasant Woodpile (large)
(@OGUID+075, 400036, 0, 1, 4343.19, -2804.62, 4.77377, 6.17004, 0, 0, 0.0565436, -0.9984, 25, 25), -- Peasant Woodpile
(@OGUID+076, 400038, 0, 1, 4344.39, -2807.59, 4.77619, 1.41838, 0, 0, 0.651218, 0.758891, 25, 25), -- Lumber Pile

(@OGUID+077, 400029, 0, 1, -11789, -3183.53, -29.9811, 0.390403, 0, 0, 0.193964, 0.981009, 25, 25), -- Horde Mailbox - Blasted Lands/Dark Portal
(@OGUID+078, 400028, 0, 1, -11806.4, -3216.66, -30.8785, 5.32662, 0, 0, 0.460254, -0.887787, 25, 25), -- Alliance Mailbox - Blasted Lands/Dark Portal
(@OGUID+079, 400003, 0, 1, -11792.108398, -3226.608154, -29.721224, 2.613495, 0, 0, 0, 0, 25, 25), -- Teleporter Circle (Alliance)
(@OGUID+080, 400003, 0, 1, -11774.694336, -3184.537598, -28.923182, 2.749808, 0, 0, 0, 0, 25, 25), -- Teleporter Circle (Horde)

(@OGUID+081, 400004, 0, 1, 4295.62, -2774.97, 7.54217, 3.64235, 0, 0, 0.968818, -0.247772, 25, 25), -- Forge
(@OGUID+082, 400009, 0, 1, 4300.81, -2756.01, 7.70891, 4.34825, 0, 0, 0.823452, -0.567386, 25, 25), -- PVP HOLIDAY HORDE CTF
(@OGUID+083, 400012, 0, 1, 4303.49, -2757.14, 7.2153, 3.88085, 0, 0, 0.932462, -0.361269, 25, 25), -- PVP HOLIDAY GENERIC SIGNPOST
(@OGUID+084, 400015, 0, 1, 4303.16, -2757.51, 8.10565, 3.93192, 0, 0, 0.922933, -0.384961, 25, 25), -- PVP HOLIDAY ALLIANCE CTF
(@OGUID+085, 400014, 0, 1, 4305.55, -2759.83, 8.92634, 4.68075, 0, 0, 0.963883, -0.266325, 25, 25), -- PVP HOLIDAY ALLIANCE AV
(@OGUID+086, 400010, 0, 1, 4305.23, -2763.54, 6.37095, 2.99352, 0, 0, 0.997261, 0.0739667, 25, 25), -- PVP HOLIDAY HORDE AV
(@OGUID+087, 400018, 0, 1, 4304.46, -2766.78, 6.89103, 3.03512, 0, 0, 0.998583, 0.0532114, 25, 25), -- Blood Elf Banner
(@OGUID+088, 400019, 0, 1, 4305.14, -2770.49, 7.18839, 3.4946, 0, 0, 0.984464, -0.175587, 25, 25), -- Draenei Banner
(@OGUID+089, 400011, 0, 1, 4294.09, -2752.54, 7.29416, 4.10485, 0, 0, 0.886243, -0.463221, 25, 25), -- PVP HOLIDAY HORDE ARATHI
(@OGUID+090, 400013, 0, 1, 4298, -2755.31, 7.44461, 4.30906, 0, 0, 0.834411, -0.551143, 25, 25), -- PVP HOLIDAY ALLIANCE ARATHI
(@OGUID+091, 400005, 0, 1, 4292.01, -2773.86, 7.29045, 3.12373, 0, 0, 0.99996, 0.00893159, 25, 25), -- Anvil
(@OGUID+092, 152614, 0, 1, 4348.48, -2754.15, 46.1418, 4.34539, 0, 0, 0.824263, -0.566207, 25, 25), -- Elevator
(@OGUID+093, 400021, 0, 1, 4298.05, -2779.19, 7.49992, 0.854188, 0, 0, 0.414227, 0.910173, 25, 25), -- Campfire
(@OGUID+094, 400017, 0, 1, 4295.62, -2774.97, 7.54217, 3.64235, 0, 0, 0.968818, -0.247772, 25, 25), -- Forge (invisible focus)
(@OGUID+095, 400016, 0, 1, 4222.16, -2765.33, 5.68186, 5.25091, 0, 0, 0.493526, -0.869731, 25, 25), -- Party Table
(@OGUID+096, 400006, 0, 1, 4223.77, -2763.92, 5.76153, 4.16312, 0, 0, 0.872371, -0.488845, 25, 25), -- Wooden Chair
(@OGUID+097, 400022, 0, 1, 4306.6, -2756.06, 16.5814, 3.64598, 0, 0, 0.968367, -0.249528, 25, 25),  -- Guild Vault
(@OGUID+098, 400008, 0, 1, 4308.04, -2758.31, 16.5987, 3.44508, 0, 0, 0.988509, -0.151163, 25, 25), -- Mailbox
(@OGUID+099, 400001, 0, 1, 4222.14, -2765.38, 6.80969, 4.91318, 0, 0, 0.632677, -0.774416, 25, 25), -- Training Dummy Information
(@OGUID+100, 400020, 0, 1, 4273.55, -2772.84, 5.73722, 4.17713, 0, 0, 0.868924, -0.494945, 25, 25), -- Simon Game Aura Yellow
-- (@OGUID+101, 400023, 0, 1, -4104.62, -3737.61, 196.4, 2.26066, 0, 0, 0.904553, 0.426361, 25, 25), -- Ghost Gate
(@OGUID+102, 400025, 0, 1, 4342.46, -2806.98, 4.81289, 1.37558, 0, 0, 0.634829, 0.772652, 25, 25), -- Research Equipment
(@OGUID+103, 400039, 0, 1, 4342.46, -2806.98, 4.81289, 1.37558, 0, 0, 0.634829, 0.772652, 25, 25), -- Excavation Tent Pavillion
(@OGUID+104, 400037, 0, 1, 4339.98, -2808.09, 4.89407, 1.61473, 0, 0, 0.722468, 0.691404, 25, 25), -- Peasant Woodpile (large)
(@OGUID+105, 400027, 0, 1, -11786.2, -3160.42, -28.6667, 4.63473, 0, 0, 0.734024, -0.679124, 25, 25), -- Portal to Funland #1 from Blasted Lands/Dark Portal
(@OGUID+106, 400027, 0, 1, -11782.8, -3212.9, -29.3573, 3.43693, 0, 0, 0.989117, -0.147132, 25, 25), -- Portal to Funland #2 from Blasted Lands/Dark Portal
-- (@OGUID+107, 400023, 0, 1, -4219.81, -3309.01, 231, 0.601225, 0, 0, 0.296105, 0.955155, 25, 25), -- Ghost Gate
-- (@OGUID+108, 400023, 0, 1, -4224.75, -3311.77, 231.272, 0.442301, 0, 0, 0.219352, 0.975646, 25, 25), -- Ghost Gate
(@OGUID+109, 400025, 0, 1, -4221.95, -3307.5, 232.232, 3.10478, 0, 0, 0.999831, 0.0184044, 25, 25), -- Research Equipment
(@OGUID+110, 400025, 0, 1, -4220.38, -3310.6, 232.323, 2.58721, 0, 0, 0.961827, 0.273658, 25, 25), -- Research Equipment
(@OGUID+111, 400026, 0, 1, -4222.57, -3308.31, 232.287, 4.77236, 0, 0, 0.685588, -0.727989, 25, 25), -- Supply Crate
(@OGUID+112, 400007, 0, 1, -4220.72, -3313.1, 232.312, 4.77236, 0, 0, 0.685588, -0.727989, 25, 25), -- Weapon Rack
(@OGUID+113, 400007, 0, 1, -4224.73, -3307.55, 232.214, 5.80261, 0, 0, 0.23798, -0.97127, 25, 25), -- Weapon Rack
-- @OGUID+114 reuse
(@OGUID+115, 400000, 0, 1, 4305.04, -2802.02, 5.03549, 3.98812, 0, 0, 0.911754, -0.410736, 25, 25), -- Portal (Mall Area #1)
(@OGUID+116, 400000, 0, 1, 4272.85, -2701.42, 5.4303, 3.6394, 0, 0, 0.969183, -0.246342, 25, 25), -- Portal (Mall Area #2)
-- (@OGUID+117, 400002, 0, 1, 4274.88, -2785.68, 5.47708, 5.6859, 0, 0, 0.294223, -0.955737, 25, 25), -- Collision PC Size [x4]
-- Classic Dungeon Portals
(@OGUID+118, 400000, 1, 1, -3514.7, 1123.7, 161.026, 3.18951, 0, 0, 0, 0, 1, 1), -- Dire Maul
(@OGUID+119, 400000, 329, 1, 3388.34, -3379.91, 142.758, 3.0908, 0, 0, 0, 0, 1, 1), -- Stratholme
(@OGUID+120, 400000, 289, 1, 191.748, 126.756, 136.697, 3.14476, 0, 0, 0, 0, 1, 1), -- Scholomance
(@OGUID+121, 400000, 230, 1, 457.177, 34.6375, -69.451, 1.56179, 0, 0, 0, 0, 1, 1), -- Blackrock Depths
(@OGUID+122, 400000, 109, 1, -312.912, 99.8684, -131.85, 6.27819, 0, 0, 0, 0, 1, 1), -- The Sunken Temple
(@OGUID+123, 400000, 209, 1, 1210.88, 842.603, 8.92341, 3.16518, 0, 0, 0, 0, 1, 1), -- Zul'Farrak
(@OGUID+124, 400000, 349, 1, 753.632, -621.518, -32.9648, 4.7498, 0, 0, 0, 0, 1, 1), -- Maraudon
(@OGUID+125, 400000, 70, 1, -228.742, 46.1557, -46.0197, 4.68553, 0, 0, 0, 0, 1, 1), -- Uldaman
(@OGUID+126, 400000, 129, 1, 2593.14, 1110.8, 50.9636, 1.60626, 0, 0, 0, 0, 1, 1), -- Razorfen Downs
(@OGUID+127, 400000, 0, 1, 2841.7, -688.601, 139.33, 2.00294, 0, 0, 0, 0, 1, 1),  -- Scarlet Monastery
(@OGUID+128, 400000, 90, 1, -328.649, -3.68268, -152.846, 5.85897, 0, 0, 0, 0, 1, 1), -- Gnomeregan
(@OGUID+129, 400000, 47, 1, 1940.62, 1540.8, 81.8905, 3.98674, 0, 0, 0, 0, 1, 1), -- Razorfen Kraul
(@OGUID+130, 400000, 34, 1, 48.5276, 0.624905, -16.2335, 3.13019, 0, 0, 0, 0, 1, 1), -- Stormwind Stockades
(@OGUID+131, 400000, 48, 1, -150.698, 104.815, -40.1936, 1.4214, 0, 0, 0, 0, 1, 1), -- Blackfathom Deeps
(@OGUID+132, 400000, 33, 1, -228.972, 2109.7, 76.8898, 1.267, 0, 0, 0, 0, 1, 1), -- Shadowfang Keep
(@OGUID+133, 400000, 36, 1, -14.7696, -386.369, 62.7103, 4.84216, 0, 0, 0, 0, 1, 1), -- Deadmines
(@OGUID+134, 400000, 43, 1, -162.999, 132.661, -73.7355, 5.83, 0, 0, 0, 0, 1, 1), -- Wailing Caverns
(@OGUID+135, 400000, 389, 1, 2.30476, -8.69099, -15.7848, 1.60026, 0, 0, 0, 0, 1, 1), -- Ragefire Chasm
(@OGUID+136, 400000, 229, 1, 78.071, -226.853, 49.7684, 1.69922, 0, 0, 0, 0, 1, 1), -- Blackrock Spire (Lower)
-- Classic Raid Portals
(@OGUID+137, 400000, 229, 1, 129.03, -319.003, 70.9556, 3.2763, 0, 0, 0, 0, 1, 1), -- Blackrock Spire (Upper)
(@OGUID+138, 400000, 409, 1, 1085.72, -476.182, -107.314, 0.492129, 0, 0, 0, 0, 1, 1), -- Molten Core
(@OGUID+139, 400000, 249, 1, 30.8192, -56.0592, -5.19956, 1.58057, 0, 0, 0, 0, 1, 1), -- Onyxia's Lair
(@OGUID+140, 400000, 469, 1, -7672.67, -1107.07, 396.652, 3.74521, 0, 0, 0, 0, 1, 1), -- Blackwing Lair
(@OGUID+141, 400000, 309, 1, -11916.5, -1231.92, 92.5334, 1.64541, 0, 0, 0, 0, 1, 1), -- Zul'Gurub
(@OGUID+142, 400000, 509, 1, -8436.53, 1519.17, 31.907, 2.61799, 0, 0, 0.965925, 0.258821, 25, 25), -- AQ20
(@OGUID+143, 400000, 531, 1, -8221.35, 2014.34, 129.071, 0.872665, 0, 0, 0.422618, 0.906308, 25, 25), -- AQ40
(@OGUID+144, 400000, 533, 1, 3005.87, -3435.01, 293.882, 0, 0, 0, 0, 1, 25, 25), -- Naxx
-- TBC Dungeon Portals
(@OGUID+145, 400000, 543, 3, -1355.24, 1641.12, 68.2491, 1.12531, 0, 0, 0.533434, 0.845842, 25, 25), -- Hellfire Ramparts
(@OGUID+146, 400000, 542, 3, -3.9967, 14.6363, -44.8009, 4.88748, 0, 0, 0.642574, -0.766224, 25, 25), -- Blood Furnace
(@OGUID+147, 400000, 540, 3, -40.8716, -19.7538, -13.8065, 1.11133, 0, 0, 0.527508, 0.84955, 25, 25), -- Shattered Halls
(@OGUID+148, 400000, 547, 3, 120.101, -131.957, -0.801547, 1.47574, 0, 0, 0.672713, 0.739903, 25, 25), -- Slave Pens
(@OGUID+149, 400000, 546, 3, 9.71391, -16.2008, -2.75334, 5.57082, 0, 0, 0.348699, -0.937235, 25, 25), -- Underbog
(@OGUID+150, 400000, 545, 3, -13.8425, 6.7542, -4.2586, 0, 0, 0, 0, 1, 25, 25), -- Steamvault
(@OGUID+151, 400000, 557, 3, 0.0191, 0.9478, -0.9543, 3.03164, 0, 0, 0.998489, 0.0549486, 25, 25), -- Mana Tombs
(@OGUID+152, 400000, 558, 3, -21.8975, 0.16, -0.1206, 0.0353412, 0, 0, 0.0176697, 0.999844, 25, 25), -- Auchenai Crypts
(@OGUID+153, 400000, 556, 3, -4.6811, -0.0930796, 0.0062, 0.0353424, 0, 0, 0.0176703, 0.999844, 25, 25), -- Sethekk Halls
(@OGUID+154, 400000, 555, 3, 0.488033, -0.215935, -1.12788, 3.15888, 0, 0, 0.999963, -0.00864356, 25, 25), -- Shadow Labyrinth
(@OGUID+155, 400000, 560, 3, 2741.68, 1312.64, 14.0413, 2.79253, 0, 0, 0.984808, 0.173647, 25, 25), -- Old Hillsbrad Foothills
(@OGUID+156, 400000, 269, 3, -1491.61, 7056.51, 32.1011, 1.74533, 0, 0, 0.766045, 0.642787, 25, 25), -- Black Morass
(@OGUID+157, 400000, 553, 3, 40.0395, -28.613, -1.1189, 2.35856, 0, 0, 0.924332, 0.38159, 25, 25), -- The Botanica
(@OGUID+158, 400000, 552, 3, -1.23165, 0.0143459, -0.204293, 0.0157123, 0, 0, 0.00785607, 0.999969, 25, 25), -- The Arcatraz
(@OGUID+159, 400000, 554, 3, -28.906, 0.680314, -1.81282, 0.0345509, 0, 0, 0.0172746, 0.999851, 25, 25), -- The Mechanar
-- TBC Raid Portals
(@OGUID+160, 400000, 532, 3, -11101.8, -1998.31, 49.8927, 0.690367, 0, 0, 0.338369, 0.941013, 25, 25), -- Karazhan
(@OGUID+161, 400000, 565, 3, 62.7842, 35.462, -3.9835, 1.41844, 0, 0, 0.651242, 0.75887, 25, 25), -- Gruul's Lair
(@OGUID+162, 400000, 544, 3, 187.843, 35.9232, 67.9252, 4.79879, 0, 0, 0.675909, -0.736985, 25, 25), -- Magtheridon's Lair
(@OGUID+163, 400000, 548, 3, 2.5343, -0.022318, 821.727, 0.004512, 0, 0, 0.002256, 0.999997, 25, 25), -- Serpentshrine Cavern
(@OGUID+164, 400000, 550, 3, -10.8021, -1.15045, -2.42833, 6.22821, 0, 0, 0.0274842, -0.999622, 25, 25), -- The Eye
(@OGUID+165, 400000, 534, 3, 4252.72, -4226.16, 868.222, 2.35619, 0, 0, 0.923879, 0.382686, 25, 25), -- Hyjal Summit
(@OGUID+166, 400000, 564, 3, 96.4462, 1002.35, -86.9984, 6.15675, 0, 0, 0.0631755, -0.998002, 25, 25), -- Black Temple
(@OGUID+167, 400000, 568, 3, 120.7, 1776, 43.455, 4.7713, 0, 0, 0.685975, -0.727625, 25, 25), -- Zul'Aman
-- City Portals
(@OGUID+168, 400000, 0, 1, -8852.23, 639.835, 96.3843, 0.958539, 0, 0, 0, 0, 1, 1), -- Stormwind
(@OGUID+169, 400000, 0, 1, -4895.91, -962.374, 501.448, 3.02565, 0, 0, 0, 0, 1, 1), -- Ironforge
(@OGUID+170, 400000, 1, 1, 9941.17, 2500.68, 1317.96, 4.15784, 0, 0, 0, 0, 1, 1), -- Darnassus
(@OGUID+171, 400000, 530, 1, -3905.68, -11610, -138.054, 3.82175, 0, 0, 0, 0, 1, 1), -- Exodar
(@OGUID+172, 400000, 1, 1, 1588.4, -4394.55, 6.46053, 4.92048, 0, 0, 0, 0, 1, 1), -- Orgrimmar
(@OGUID+173, 400000, 0, 1, 1563.83, 263.1, -43.1027, 5.59063, 0, 0, 0, 0, 1, 1), -- Undercity
(@OGUID+174, 400000, 1, 1, -1265.43, 49.056, 127.614, 0.284943, 0, 0, 0, 0, 1, 1), -- Thunder Bluff
(@OGUID+175, 400000, 530, 1, 9513.37, -7300.74, 14.4663, 3.22001, 0, 0, 0, 0, 1, 1), -- Silvermoon City
(@OGUID+176, 400000, 530, 1, -1972.12, 5107.44, 7.5194, 4.3598, 0, 0, 0, 0, 1, 1), -- Shattrath City
-- Portals to Funland from starting zones
(@OGUID+177, 400027, 530, 1,10361.8, -6368.95, 36.1571, 2.15173, 0, 0, 0.880002, 0.47497, 25, 25),
(@OGUID+178, 400027, 0, 1,-6217.15, 325.326, 383.257, 2.61592, 0, 0, 0.965656, 0.259822, 25, 25),
(@OGUID+179, 400027, 0, 1,-8924.21, -136.003, 81.0789, 1.89346, 0, 0, 0.81151, 0.584339, 25, 25),
(@OGUID+180, 400027, 1, 1,10325.7, 821.585, 1326.42, 2.27174, 0, 0, 0.9069, 0.421345, 25, 25),
(@OGUID+181, 400027, 530, 1,-3999.23, -13894.7, 94.6012, 6.13674, 0, 0, 0.0731583, -0.99732, 25, 25),
(@OGUID+182, 400027, 1, 1,-607.305, -4255.03, 38.95, 3.20717, 0, 0, 0.999462, -0.0327824, 25, 25),
(@OGUID+183, 400027, 0, 1,1656.98, 1673.35, 120.719, 6.27098, 0, 0, 0.00610101, -0.999981, 25, 25),
(@OGUID+184, 400027, 1, 1,-2910.22, -255.039, 52.9412, 3.48011, 0, 0, 0.98571, -0.168453, 25, 25),
-- GM Island
(@OGUID+185, 400034, 1, 1, 16240.796875, 16311.089844, 20.842548, 4.638500, 0, 0, 0.679358, 0.733807, 25, 25), -- Gamemaster Guild Vault
(@OGUID+186, 400032, 1, 1, 16243, 16310.9, 20.8446, 4.63616, 0, 0, 0.733539, -0.679647, 25, 25), -- Alliance Gamemaster Mailbox
(@OGUID+187, 400030, 1, 1, 16238.678711, 16311.349609, 20.844852, 4.630649, 0, 0, 0.971915, -0.23533, 25, 25), -- Horde Gamemaster Mailbox
(@OGUID+188, 400031, 1, 1, 16226.4, 16287, 20.8433, 1.48933, 0, 0, 0.677726, 0.735314, 25, 25), -- Portcullis
(@OGUID+189, 400031, 1, 1, 16224, 16274.9, 13.1756, 1.45524, 0, 0, 0.665095, 0.746758, 25, 25), -- Portcullis
(@OGUID+190, 400031, 1, 1, 16232.6, 16298, 13.1743, 0, 0, 0, -0.05, 0.9, 25, 25), -- Portcullis
(@OGUID+191, 400033, 1, 1, 16231.6, 16258.9, 13.7686, 3.2364, 0, 0, 0, 0, 25, 25), -- Chair
(@OGUID+192, 400033, 1, 1, 16221.8, 16257.1, 13.1844, 1.1354, 0, 0, 0, 0, 25, 25), -- Chair
(@OGUID+193, 400033, 1, 1, 16231.2, 16263.2, 13.4947, 3.264, 0, 0, 0, 0, 25, 25), -- Chair
(@OGUID+194, 400033, 1, 1, 16219.3, 16261, 13.3842, 0.1538, 0, 0, 0, 0, 25, 25), -- Chair
(@OGUID+195, 400033, 1, 1, 16230.2, 16267.2, 13.2607, 3.527, 0, 0, 0, 0, 25, 25), -- Chair
(@OGUID+196, 400033, 1, 1, 16219.8, 16266, 13.2733, 5.6005, 0, 0, 0, 0, 25, 25), -- Chair
(@OGUID+197, 400033, 1, 1, 16229.9, 16254.9, 13.4824, 2.3175, 0, 0, 0, 0, 25, 25), -- Chair
(@OGUID+198, 400035, 1, 1, 16224.1, 16262, 13.2639, 4.8466, 0, 0, 0, 0, 0, 0), -- Sunwell Plateau
-- Unknown Wonders Portals
(@OGUID+199, 400000, 0, 1, -4240.79, -3300.99, 232.78, 4.84855, 0, 0, 0.657366, -0.753572, 25, 25), -- Grim Batol
(@OGUID+200, 400000, 0,1,-6166,-772,421,0, 0, 0, 0, 0, 1, 1), -- Newman's Landing
(@OGUID+201, 400000, 1,1,16226.2,16257,13.2022,1.65007, 0, 0, 0, 0, 1, 1), -- GM Island
(@OGUID+202, 400000, 1,1,16209.844,16386.154,-64.379,3.155, 0, 0, 0, 0, 1, 1), -- GM Prison
(@OGUID+203, 400000, 13,1,-0.310414,0.107129,-144.709,2.94612, 0, 0, 0, 0, 1, 1), -- Testing
(@OGUID+204, 400000, 35,1,-1,52,-27,0, 0, 0, 0, 0, 1, 1), -- Stormwind Vault
(@OGUID+205, 400000, 44,1,74,-1,18.676,0, 0, 0, 0, 0, 1, 1), -- Old Scarlet Monastery
(@OGUID+206, 400000, 0,1,-11069,-1795,54,0, 0, 0, 0, 0, 1, 1), -- Karazhan Crypts
(@OGUID+207, 400000, 0,1,-4821,-975,464.709,0, 0, 0, 0, 0, 1, 1), -- Old Ironforge
(@OGUID+208, 400000, 0,1,-8322,-340,145,0, 0, 0, 0, 0, 1, 1), -- Elwynn Falls
(@OGUID+209, 400000, 0,1,3852,-3565,45.549,0, 0, 0, 0, 0, 1, 1), -- Northern Plaguelands
(@OGUID+210, 400000, 0,1,-979,1579,52.705,0, 0, 0, 0, 0, 1, 1), -- Greymane Area
(@OGUID+211, 400000, 1,1,-9479,1783,49.945,0, 0, 0, 0, 0, 1, 1), -- Uninstanced AhnQiraj
(@OGUID+212, 400000, 0,1,-1817,-4205,3,0, 0, 0, 0, 0, 1, 1), -- Arathi Highlands Farm
(@OGUID+213, 400000, 530,1,10389.178,-6402.809,161.0528,0, 0, 0, 0, 0, 1, 1), -- Sunspire Top
(@OGUID+214, 400000, 0,1,-12878,-1408,120,0, 0, 0, 0, 0, 1, 1), -- Stranglethorn Bengal Tiger Cave
(@OGUID+215, 400000, 0,1,1545,150,61.591,0, 0, 0, 0, 0, 1, 1), -- Above Undercity Glitch
(@OGUID+216, 400000, 0,1,-7347,-642,294.585, 0, 0, 0, 0, 0, 1, 1), -- Blackchar Cave
(@OGUID+217, 400000, 0,1,-5314,-2512,484,0, 0, 0, 0, 0, 1, 1), -- Ortell's Hideout
(@OGUID+218, 400000, 0,1,-3857,-3485,579,0, 0, 0, 0, 0, 1, 1), -- Wetlands Help Mountain
(@OGUID+219, 400000, 37,1,128.205,135.136,236.11,4.59132, 0, 0, 0, 0, 1, 1), -- Azshara Crater
(@OGUID+220, 400000, 0,1,-4033.5546,-1466.882,169.8629,1.288, 0, 0, 0, 0, 1, 1), -- Isolated Wetlands Town
(@OGUID+221, 400000, 0,1,-4897,836,390,0, 0, 0, 0, 0, 1, 1), -- Isolated Dun Morogh
(@OGUID+222, 400000, 0,1,-7815,-4266,131,0, 0, 0, 0, 0, 1, 1), -- Isolated Burning Steppes
(@OGUID+223, 400000, 1,1,-9619,-5594,-496,0, 0, 0, 0, 0, 1, 1), -- Tanaris Underwater Village
(@OGUID+224, 400000, 0,1,2074,2306,131,0, 0, 0, 0, 0, 1, 1), -- Isolated Tirisfal Glades
(@OGUID+225, 400000, 36,1,-1650,549,7,0, 0, 0, 0, 0, 1, 1), -- Deadmines - Outland Alpha
(@OGUID+226, 400000, 33,1,-588,1087,108,0, 0, 0, 0, 0, 1, 1), -- Shadowfang Keep - Silverpine Forest
(@OGUID+227, 400000, 209,1,1075,860,9,0, 0, 0, 0, 0, 1, 1), -- ZulFarrak - Tanaris
(@OGUID+228, 400000, 309,1,-12029,-2584,-29,0, 0, 0, 0, 0, 1, 1), -- ZulGurub - Altar Of Storms
(@OGUID+229, 400000, 533,1,3598,-4523,198,0, 0, 0, 0, 0, 1, 1), -- Naxxramas - Northrend
(@OGUID+230, 400000, 169,1,-366.091,3097.86,92.317,0.0487625, 0, 0, 0, 0, 1, 1), -- Emerald Dream
(@OGUID+231, 400000, 169,1,2781.57,3006.76,23.2219,0.5, 0, 0, 0, 0, 1, 1), -- Emerald Dream - Statue
(@OGUID+232, 400000, 169,1,-2128.12,-1005.89,132.213,0.5, 0, 0, 0, 0, 1, 1), -- Emerald Dream - Verdant Fields
(@OGUID+233, 400000, 169,1,2732.93,-3319.63,101.284,0.5, 0, 0, 0, 0, 1, 1), -- Emerald Dream - Forest
(@OGUID+234, 400000, 169,1,-3824,3367,133,0.5, 0, 0, 0, 0, 1, 1), -- Emerald Dream - Plains
(@OGUID+235, 400000, 169,1,3225,3046,23,0.5, 0, 0, 0, 0, 1, 1), -- Emerald Dream - Mountains
(@OGUID+236, 400000, 451,1,16303.5,-16173.5,40.4365,0.539123, 0, 0, 0, 0, 1, 1), -- Designer Island
(@OGUID+237, 400000, 451,1,16304,16318,69,0.539123, 0, 0, 0, 0, 1, 1), -- Programmer Isle
(@OGUID+238, 400000, 1,1,4654,-3772,944,0.5, 0, 0, 0, 0, 1, 1), -- Hyjal
(@OGUID+239, 400000, 1,1,-10706.474,2483.685,7.921,3.4365, 0, 0, 0, 0, 1, 1), -- Silithus Farm
(@OGUID+240, 400000, 1,1,1951.512,1530.4755,247.2881,0.5, 0, 0, 0, 0, 1, 1), -- Stonetalon Logging Camp
(@OGUID+241, 400000, 1,1,-11649.1387,-4698.392,10.5532,4.7795, 0, 0, 0, 0, 1, 1), -- South Seas Islands
(@OGUID+242, 400000, 0,1,-5933,452,509,4.7795, 0, 0, 0, 0, 1, 1), -- Forgotten Gnome Camp
(@OGUID+243, 400000, 369,1,3.297,1382.282,-100.303,4.667675, 0, 0, 0, 0, 1, 1), -- Deeprun Tram Aquarium
(@OGUID+244, 400000, 0,1,-13181.8,339.356,42.9805,1.18013, 0, 0, 0, 0, 1, 1), -- Gurubashi Arena
-- City Portals (Neutral)
(@OGUID+245,400000,0,1,-14353.142,409.47,6.63197,1.895538, 0, 0, 0, 0, 1, 1), -- Booty Bay
(@OGUID+246,400000,1,1,-7222.671,-3826.853,9.3328,1.0404, 0, 0, 0, 0, 1, 1), -- Gadgetzan
(@OGUID+247,400000,1,1,6675.0527,-4648.43,721.5831,6.11709, 0, 0, 0, 0, 1, 1), -- Everlook
(@OGUID+248,400000,530,1,3084.555,3717.161,142.8485,4.9769, 0, 0, 0, 0, 1, 1); -- Area 52

SELECT MAX(`entry`) INTO @max_custom_creature_entry FROM `creature_template`;
DELETE FROM `creature` WHERE `id` BETWEEN 980000 AND @max_custom_creature_entry;
DELETE FROM `creature` WHERE `id` IN (14623,14879,15972,22516); -- Battlemasters
DELETE FROM `creature` WHERE `id` BETWEEN 185000 AND 185100; -- Multivendors
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecsmin`, `spawntimesecsmax`, `spawndist`, `MovementType`) VALUES
-- Multivendor spawns
(@CGUID+012, 185021, 1, 1, -8560.34, 1981.54, 100.351, 1.94848, 25, 25, 0, 0),
(@CGUID+013, 185020, 1, 1, -8564.03, 1980.08, 100.351, 1.94848, 25, 25, 0, 0),
(@CGUID+014, 185019, 1, 1, -8567.71, 1978.62, 100.351, 1.94848, 25, 25, 0, 0),
(@CGUID+015, 185018, 1, 1, -8570.85, 1977.37, 100.351, 1.94848, 25, 25, 0, 0),
(@CGUID+016, 185017, 1, 1, -8573.99, 1976.12, 100.351, 1.94848, 25, 25, 0, 0),
(@CGUID+017, 185016, 1, 1, -8577.03, 1974.92, 100.351, 1.94848, 25, 25, 0, 0),
(@CGUID+018, 185015, 1, 1, -8580.38, 1973.59, 100.351, 1.94848, 25, 25, 0, 0),
(@CGUID+019, 185014, 1, 1, -8583.64, 1972.3, 100.351, 1.94848, 25, 25, 0, 0),
(@CGUID+020, 185013, 1, 1, -8586.67, 1971.09, 100.351, 1.94848, 25, 25, 0, 0),
(@CGUID+021, 185012, 1, 1, -8590.14, 1969.72, 100.351, 1.94848, 25, 25, 0, 0),
(@CGUID+022, 185011, 1, 1, -8593.17, 1968.51, 100.351, 1.94848, 25, 25, 0, 0),
(@CGUID+023, 185010, 1, 1, -8595.99, 1967.39, 100.351, 1.94848, 25, 25, 0, 0),
(@CGUID+024, 185009, 1, 1, -8598.7, 1966.32, 100.351, 1.94848, 25, 25, 0, 0),
(@CGUID+025, 185008, 1, 1, -8601.62, 1965.16, 100.351, 1.94848, 25, 25, 0, 0),
(@CGUID+026, 185007, 1, 1, -8604.33, 1964.09, 100.351, 1.94848, 25, 25, 0, 0),
(@CGUID+027, 185006, 1, 1, -8606.82, 1963.1, 100.351, 1.94848, 25, 25, 0, 0),
(@CGUID+028, 185005, 1, 1, -8609.42, 1962.07, 100.351, 1.94848, 25, 25, 0, 0),
(@CGUID+029, 185004, 1, 1, -8612.05, 1961.02, 100.351, 1.94848, 25, 25, 0, 0),
(@CGUID+030, 185003, 1, 1, -8614.35, 1960.11, 100.351, 1.94848, 25, 25, 0, 0),
(@CGUID+031, 185002, 1, 1, -8616.84, 1959.12, 100.351, 1.94848, 25, 25, 0, 0),
(@CGUID+032, 185001, 1, 1, -8619.66, 1958.01, 100.351, 1.94848, 25, 25, 0, 0),
(@CGUID+033, 185000, 1, 1, -8621.82, 1957.15, 100.351, 1.94848, 25, 25, 0, 0),

(@CGUID+034,980042,0,1,-11797.620117,-3182.739014,-30.257090,1.07644,25,25,0,0), -- Beginner Training Dummy
(@CGUID+035,980042,0,1,-11809,-3219.39,-30.9315,5.12947,25,25,0,0), -- Beginner Training Dummy
(@CGUID+036,980040,1,1,16239.99,16305.12,20.84,6.1818,25,25,0,0), -- Paul <Stylist>
(@CGUID+037,980032,0,1,4286.77,-2779.733,6.724,4.72,25,25,0,0), -- Transmogrification Beacon
(@CGUID+038,980024,0,1,-11802.8135,-3215.177,-30.7,5.234,60,60,0,0), -- Alliance Enlistment Officer
(@CGUID+039,980023,0,1,-11787.7,-3178.85,-29.7356,5.88474,60,60,0,0), -- Horde Enlistment Officer
-- Raid & Dungeon Messengers
(@CGUID+040,980031,530,0,-3647.07,315.873,35.3595,5.96742,25,25,0,0),    -- Black Temple
(@CGUID+041,980031,530,0,6850.89,-7995.011,192.36881,1.433062,25,25,0,0),-- Zul'Aman
(@CGUID+042,980030,530,0,12884.34,-7337.6,65.4872,1.392411,25,25,0,0),   -- Magisters' Terrace
(@CGUID+043,980031,530,0,12556.3,-6774.45,15.0784,6.24727,25,25,0,0),    -- Sunwell Plateau
-- Alliance Greeter NPC Spawns
(@CGUID+044,980028,0,1,-8931.93, -132.83, 82.88, 3.26, 25,25,0,0),      -- Northshire (Human)
(@CGUID+045,980028,0,1,-6213.47, 330.64, 383.68, 3.15, 25,25,0,0),     -- Coldridge Valley (Dwarf and Gnome)
(@CGUID+046,980028,1,1,10317.40, 821.00, 1326.37, 2.15, 25,25,0,0),    -- Shadowglen (Night Elf)
(@CGUID+047,980028,530,1,-1968.69, -11876.47, 48.80, 4.48, 25,25,0,0), -- Blood Watch (Draenei 20+)
(@CGUID+048,980028,530,1,-3983.02, -13898.48, 96.30, 5.32, 25,25,0,0), -- Ammen Vale (Draenei)
(@CGUID+049,980028,1,1,2757.02,-398.38,111.461,5.83229,25,25,0,0),         -- Astranaar, Ashenvale (Alliance 20+)
(@CGUID+050,980028,0,1,-10530.1,-1174.96,28.104,1.69525,25,25,0,0),        -- Darkshire, Duskwood (Alliance 20+)
(@CGUID+051,980028,0,1,-9248.25,-2166.63,64.0582,3.07986,25,25,0,0),       -- Lakeshire, Redridge Mountains (Alliance 20+)
(@CGUID+052,980028,0,1,-5362.45,-2956.97,323.848,0.857575,25,25,0,0),      -- Thelsamar, Loch Modan (Alliance 20+)
(@CGUID+053,980028,1,1,2715.53,1493.84,236.82,2.36479,25,25,0,0),          -- Stonetalon Peak, Stonetalon Mountains (Alliance 20+)
(@CGUID+054,980028,0,1,-846.075,-546.52,11.0796,1.53876,25,25,0,0),        -- Southshore, Hillsbrad Foothills (Alliance 20+)
(@CGUID+055,980028,0,1,-3794.24,-832.551,9.65552,6.19475,25,25,0,0),       -- Menethil Harbor, Wetlands (Alliance 20+)
-- Horde Greeter NPC Spawns
(@CGUID+056,980027,1,1,-607.47, -4248.13, 38.95, 3.27, 25,25,0,0),      -- Valley of Trials (Orc and Troll)
(@CGUID+057,980027,0,1,1656.367,1682.592,120.78681,0.06632,25,25,0,0),  -- Deathknell (Undead)
(@CGUID+058,980027,1,1,-2913.71, -254.67, 52.94, 3.70, 25,25,0,0),      -- Camp Narache (Tauren)
(@CGUID+059,980027,530,1,7572.59, -6819.23, 86.64, 0.09, 25,25,0,0),   -- Tranquillien (Blood Elf 20+)
(@CGUID+060,980027,530,1,10354.33, -6370.34, 36.04, 1.92, 25,25,0,0),  -- Sunstrider Isle (Blood Elf)
(@CGUID+061,980027,1,1,913.12,921.155,106.258,4.09974,25,25,0,0),          -- Sun Rock Retreat, Stonetalon Mountains (Horde 20+)
(@CGUID+062,980027,1,1,-2363.12,-1963.87,96.0848,0.17341,25,25,0,0),       -- Camp Taurajo, The Barrens (Horde 20+)
(@CGUID+063,980027,1,1,2333.13,-2543.09,101.565,2.5894,25,25,0,0),         -- Splintertree Post, Ashenvale (Horde 20+)
(@CGUID+064,980027,0,1,505.088,1639.41,125.942,4.90182,25,25,0,0),         -- The Sepulcher, Silverpine Forest (Horde 20+)
(@CGUID+065,980027,0,1,-29.2284,-915.915,54.8742,4.301,25,25,0,0),         -- Tarren Mill, Hillsbrad Foothills (Horde 20+)
(@CGUID+066,980027,1,1,-428.479,-2657.02,96.3332,3.16669,25,25,0,0),       -- The Crossroads, The Barrens (Horde 20+)
-- Alliance Realm Information NPC Spawns
(@CGUID+067,980026,0,1,-8852.23,639.835,96.3843,0.958539,25,25,0,0),       -- Stormwind
(@CGUID+068,980026,0,1,-4895.91,-962.374,501.448,3.02565,25,25,0,0),       -- Ironforge
(@CGUID+069,980026,1,1,9941.17,2500.68,1317.96,4.15784,25,25,0,0),         -- Darnassus
(@CGUID+070,980026,530,1,-3905.68,-11610,-138.054,3.82175,25,25,0,0),      -- Exodar
-- Horde Realm Information NPC Spawns
(@CGUID+071,980025,1,1,1584.2746,-4396.12793,5.6854,5.523732,25,25,0,0),   -- Orgrimmar
(@CGUID+072,980025,1,1,-1265.43,49.056,127.614,0.284943,25,25,0,0),        -- Thunder Bluff
(@CGUID+073,980025,0,1,1563.83,263.1,-43.1027,5.59063,25,25,0,0),          -- Undercity
(@CGUID+074,980025,530,1,9513.37,-7300.74,14.4663,3.22001,25,25,0,0),      -- Silvermoon City
-- Dual Spec Crystals
(@CGUID+075, 980022, 1, 1, -1385.57, -106.832, 158.935, 3.38884, 25, 25, 0, 0),
(@CGUID+076, 980022, 530, 1, -3723.79, -11702.9, -105.649, 2.86278, 25, 25, 0, 0),
(@CGUID+077, 980022, 0, 1, 1324.66, 297.563, -63.6997, 2.40582, 25, 25, 0, 0),
(@CGUID+078, 980022, 1, 1, 1990.97, -4810.55, 56.7646, 1.94642, 25, 25, 0, 0),
(@CGUID+079, 980022, 1, 1, 9975.38, 2316.57, 1330.79, 0.00543642, 25, 25, 0, 0),
(@CGUID+080, 980022, 0, 1, -5046.49, -1276.33, 510.325, 1.31583, 25, 25, 0, 0),
(@CGUID+081, 980022, 0, 1, -8381.34, 281.29, 120.887, 2.98957, 25, 25, 0, 0),
(@CGUID+082, 980022, 530, 1, 9854.07, -7569.61, 19.2509, 2.41089, 25, 25, 0, 0),
(@CGUID+083, 980022, 530, 1, -1973.52, 5168.77, -38.4893, 0.446048, 25, 25, 0, 0),
(@CGUID+084,980029,0,1,4273.432617,-2772.893311,5.738295,4.285234,25,25,0,0), -- Test Realm Overlord
(@CGUID+085,980018,0,1,4269.492,-2697.313,5.543248,4.057,25,25,0,0), -- Lockout Nullifier
(@CGUID+086,980010,0,1,4288.75,-2774.64,7.0212,1.545,25,25,0,0), -- Avatar of the Elements
(@CGUID+087,980034,0,1,4288.88,-2774.68,13.2698,3.18993,25,25,0,0), -- Paymaster
(@CGUID+088,980035,0,1,4307.63,-2762.16,16.6499,3.36021,25,25,0,0), -- Izzy Coppergrab <Banker>
(@CGUID+089,980036,0,1,4308.07,-2763.44,17.1091,3.1927,25,25,0,0), -- Auctioneer Grizzlin
(@CGUID+090,14623,0,1,4302.001953,-2757.451416,7.188628,4.021297,25,25,0,0), -- Warsong Gulch Battlemaster
(@CGUID+091,14879,0,1,4295.618164,-2754.676270,7.084821,4.468958,25,25,0,0), -- Arathi Basin Battlemaster
(@CGUID+092,15972,0,1,4304.412109,-2761.713135,6.458610,3.392922,25,25,0,0), -- Alterac Valley Battlemaster
(@CGUID+093,22516,0,1,4304.012695,-2768.903076,7.125793,3.040650,25,25,0,0), -- Eye of the Storm Battlemaster
(@CGUID+094,980021,0,1,4216.61,-2787.04,6.16398,0.63109,25,25,0,0), -- Grandmaster Training Dummy
(@CGUID+095,980020,0,1,4220.55,-2790.81,5.74598,0.69784,25,25,0,0), -- Journeyman Training Dummy
(@CGUID+096,980020,0,1,4224.75,-2793.08,6.10642,1.20791,25,25,0,0), -- Journeyman Training Dummy
(@CGUID+097,980019,0,1,4213.26,-2775.78,6.75641,6.27024,25,25,0,0), -- Novice Training Dummy
(@CGUID+098,980019,0,1,4215.76,-2770.42,6.31771,5.58258,25,25,0,0), -- Novice Training Dummy
(@CGUID+099,980021,0,1,4213.68,-2782.25,6.92849,0.09700,25,25,0,0), -- Grandmaster Training Dummy
(@CGUID+100, 980013, 0, 1, 4302.177734, -2753.798828, 17.025675, 4.222574, 25, 25, 0, 0), -- Arena Gear <Season 2>
(@CGUID+101, 980012, 0, 1, 4274.16, -2767.12, 6.15323, 1.53996, 25, 25, 0, 0), -- Race Changer
(@CGUID+102, 980011, 0, 1, 4275.74, -2766.39, 6.13101, 2.43988, 25, 25, 0, 0), -- Guild Creator
(@CGUID+103, 980033, 0, 1, 4284.32, -2763.95, 13.6069, 5.28552, 25, 25, 0, 0), -- Beastmaster
(@CGUID+104, 980009, 0, 1, 4287.55, -2761.86, 15.003, 5.28552, 25, 25, 0, 0), -- Impy <Demon Trainer>
(@CGUID+105, 980008, 0, 1, 4291.89, -2772.97, 14.7869, 2.11331, 25, 25, 0, 0), -- Mounts
(@CGUID+106, 980007, 0, 1, 4290, -2760.28, 15.8434, 5.28552, 25, 25, 0, 0), -- Poisons
(@CGUID+107, 980006, 0, 1, 4295.72, -2770.66, 15.8998, 2.09282, 25, 25, 0, 0), -- Jewelry
(@CGUID+108, 980005, 0, 1, 4298.35, -2769.15, 16.484, 2.09282, 25, 25, 0, 0), -- Off-sets
(@CGUID+109, 980004, 0, 1, 4301.17, -2767.54, 16.8459, 2.09282, 25, 25, 0, 0), -- Off-hands
(@CGUID+110, 980003, 0, 1, 4304.57, -2765.59, 17.1739, 2.09282, 25, 25, 0, 0), -- Weapons
(@CGUID+111, 980019, 0, 1, 4283.47, -2723.07, 7.77183, 3.3746, 25, 25, 0, 0), -- Novice Training Dummy
(@CGUID+112, 980020, 0, 1, 4288.2, -2745.66, 9.30311, 3.3746, 25, 25, 0, 0), -- Journeyman Training Dummy
(@CGUID+113, 980021, 0, 1, 4283.35, -2734.11, 9.79338, 3.3746, 25, 25, 0, 0), -- Grandmaster Training Dummy
(@CGUID+114, 980017, 0, 1, 4292.660645, -2758.832275, 16.459620, 5.165036, 25, 25, 0, 2), -- Master Provisioner
(@CGUID+115, 980002, 0, 1, 4281.89, -2765.37, 12.0998, 4.34304, 25, 25, 0, 0), -- Universal Class Trainer
(@CGUID+116,980014,0,1,4341.72,-2807.69,6.29622,3.16024,25,25,0,0), -- Cat
(@CGUID+117, 980015, 0, 1, 4299.353516, -2755.156494, 17.022539, 5.172901, 25, 25, 0, 0), -- Gems Vendor
(@CGUID+118, 980041, 0, 1, 4296.085938, -2756.722412, 16.953691, 5.145411, 25, 25, 0, 0), -- Enchantment Crystal - Mall
(@CGUID+119, 980038, 0, 1, 4274.98, -2785.63, 7.148, 2.74164, 25, 25, 0, 2), -- Arena Battlemaster
(@CGUID+120, 980037, 0, 1, 4276.35, -2784.37, 6.22007, 2.78125, 25, 25, 0, 0), -- Arena Organizer
(@CGUID+121, 980039, 0, 1, 4250.73, -2828.26, 8.11386, 0.832699, 25, 25, 0, 0), -- Spirit Healer
(@CGUID+122, 980016, 0, 1, 4303.87, -2754.78, 16.6079, 4.3252, 25, 25, 0, 0), -- Tier 5
(@CGUID+123, 980020, 0, 1, -4190.93, -3315.54, 232.745, 4.36238, 25, 25, 0, 0), -- Journeyman Training Dummy
(@CGUID+124, 980019, 0, 1, -4175.65, -3321.83, 231.778, 4.36238, 25, 25, 0, 0), -- Novice Training Dummy
(@CGUID+125, 980021, 0, 1, -4183.24, -3318.7, 231.886, 4.36238, 25, 25, 0, 0), -- Grandmaster Training Dummy
(@CGUID+126, 980037, 0, 1, -4221.89, -3310.94, 232.324, 3.86664, 25, 25, 0, 0), -- Arena Organizer
(@CGUID+127, 14623, 0, 1, -4224.65, -3309.01, 232.278, 4.07435, 25, 25, 0, 0), -- Warsong Gulch Battlemaster
(@CGUID+128, 980038, 0, 1, -4222.1, -3313.38, 232.289, 2.99757, 25, 25, 0, 0), -- Arena Battlemaster
(@CGUID+129,980022,1,1,16254.7,16306.4,20.8435,2.97028,25,25,0,0), -- Dual Spec Crystal - GM Island
(@CGUID+130,980041,1,1,16254.1,16301.9,20.8434,2.97028,25,25,0,0), -- Enchantment Crystal - GM Island
(@CGUID+131,980022,0,1,4283.85,-2781.47,6.34654,5.2628,25,25,0,0), -- Dual Spec Crystal - Mall
(@CGUID+132,980001,0,1,4337.48,-2807.04,4.924,3.13878,25,25,0,2), -- Peasant
(@CGUID+133,980001,0,1,4342.26,-2802.32,4.8062,2.40444,25,25,0,2), -- Peasant
(@CGUID+134,980043,0,1,4223.45,-2730.8,5.20624,4.56957,25,25,0,2), -- Zip-a-dee-doo-dah <Model Viewer>
(@CGUID+135,980046,37,1,1044.1,294.203,336.121,3.26387,25,25,0,0), -- Captain Benedict 
(@CGUID+136,980047,37,1,75.2764,1134.73,367.176,5.46401,25,25,0,0), -- Legionnaire Gurak
(@CGUID+137,980048,0,1,4258.983887,-2849.688965,12.222013,1.838977,25,25,5,1); -- Titler

-- Grunt/Footman movement
DELETE FROM creature_movement_template WHERE `entry` IN (980045,980044);
INSERT INTO creature_movement_template (`entry`,`PathId`,`Point`,PositionX,PositionY,PositionZ,Orientation) VALUES
(980044,0,1,1026.134033,290.634613,332.553802,100),
(980044,0,2,1007.708679,283.990417,328.392395,100),
(980044,0,3,996.031677,278.255219,325.199341,100),
(980044,0,4,980.790466,274.075775,320.927063,100),
(980044,0,5,970.790161,272.355164,317.553894,100),
(980044,0,6,959.926392,270.041168,314.351044,100),
(980044,0,7,948.159729,260.909119,309.685333,100),
(980044,0,8,940.018372,253.273819,306.896515,100),
(980044,0,9,931.635132,243.834641,303.956787,100),
(980044,0,10,927.673096,235.234863,301.831787,100),
(980044,0,11,927.147644,219.621613,298.333252,100),
(980044,0,12,926.284851,202.292953,294.174286,100),
(980044,0,13,924.742981,189.476456,291.281494,100),
(980044,0,14,916.468140,173.299545,288.019775,100),
(980044,0,15,905.642456,157.362350,284.697876,100),
(980044,0,16,900.244263,146.690109,285.287689,100),
(980044,0,17,893.747437,132.156845,285.373932,100),
(980044,0,18,889.651611,123.506332,286.512634,100),
(980044,0,19,884.596313,110.844521,290.684174,100),
(980044,0,20,882.926331,87.068565,296.293549,100),
(980044,0,21,878.264587,69.746353,298.651367,100),
(980044,0,22,867.942383,55.216946,299.462646,100),
(980044,0,23,849.839600,50.537601,298.233337,100),
(980044,0,24,828.001831,52.519707,296.715698,100),
(980044,0,25,812.587891,54.696636,294.916321,100),
(980044,0,26,798.576599,54.391621,293.036621,100),
(980044,0,27,782.522766,50.863091,291.059143,100),
(980044,0,28,768.868042,51.628017,288.955658,100),
(980044,0,29,752.728516,57.071819,286.131897,100),
(980044,0,30,737.755127,64.534203,283.356476,100),
(980044,0,31,722.433228,72.080849,281.423920,100),
(980044,0,32,703.138489,81.779175,279.606354,100),
(980044,0,33,691.545166,94.900238,279.012939,100),
(980044,0,34,680.580017,107.237534,279.231995,100),
(980044,0,35,658.862122,109.020554,279.194916,100),
(980044,0,36,638.577576,109.200432,279.238190,100),
(980044,0,37,621.028381,108.345490,279.011536,100),
(980044,0,38,604.105286,106.311249,277.327881,100),
(980044,0,39,590.054443,99.957024,275.142365,100),
(980044,0,40,577.062744,92.326508,273.277222,100),
(980044,0,41,561.408081,83.607529,270.988342,100),
(980044,0,42,549.403015,78.985397,269.232269,100),
(980044,0,43,533.828796,79.830086,266.567505,100),
(980044,0,44,522.961060,80.325630,264.767456,100),
(980044,0,45,509.037994,76.066818,262.587189,100),
(980044,0,46,493.970642,74.845665,260.912262,100),
(980044,0,47,474.937653,76.550774,259.099487,100),
(980044,0,48,461.872833,79.383087,257.742279,100),
(980044,0,49,451.809662,88.161583,256.774872,100),
(980044,0,50,443.020325,97.906960,256.041046,100),
(980044,0,51,434.653107,107.326126,254.619370,100),
(980044,0,52,425.703247,112.356506,252.933456,100),
(980044,0,53,414.952972,118.227081,250.664673,100),
(980044,0,54,402.666321,124.251633,248.434647,100),
(980044,0,55,391.280762,130.693329,246.058502,100),
(980044,0,56,378.576202,141.273483,242.457245,100),
(980044,0,57,364.108276,147.404526,240.051666,100),
(980044,0,58,348.286926,152.030029,236.355362,100),
(980044,0,59,339.595734,157.320084,234.985260,100),
(980044,0,60,328.263245,165.933578,234.993408,100),
(980044,0,61,313.371429,178.251144,235.049881,100),
(980044,0,62,298.596832,188.541702,237.670746,100),
(980044,0,63,289.105743,197.283600,241.535568,100),
(980044,0,64,281.778290,212.966339,245.946625,100),
(980044,0,65,270.515472,223.841888,248.371613,100),
(980044,0,66,256.981476,230.807907,250.967133,100),
(980044,0,67,241.698547,245.391174,254.275101,100),
(980044,0,68,227.806274,257.607697,257.249084,100),
(980044,0,69,212.766205,271.098877,260.178040,100),
(980044,0,70,195.172287,279.023956,264.103912,100),
(980044,0,71,174.514145,286.676788,269.256958,100),
(980044,0,72,158.946808,291.938873,272.544067,100),
(980044,0,73,144.048782,293.087555,275.087219,100),
(980044,0,74,130.468842,297.095062,277.993134,100),
(980044,0,75,119.335228,305.922577,282.061951,100),
(980044,0,76,110.364708,309.419250,284.892090,100),
(980044,0,77,96.541710,310.836182,288.501678,100),
(980044,0,78,80.115700,314.120789,291.640930,100),
(980044,0,79,68.293381,331.675873,294.748199,100),
(980044,0,80,57.047859,353.106201,297.293121,100),
(980044,0,81,49.801083,366.092224,299.185638,100),
(980044,0,82,37.231995,379.875671,301.036499,100),
(980044,0,83,26.950632,390.485352,303.231140,100),
(980044,0,84,23.778778,400.335144,304.927612,100),
(980044,0,85,22.994734,412.899200,306.010529,100),
(980044,0,86,23.418188,427.199738,306.123444,100),
(980044,0,87,24.081280,441.919586,306.123444,100),
(980044,0,88,24.683798,455.661011,306.032684,100),
(980044,0,89,24.901884,469.099152,304.052032,100),
(980044,0,90,27.212715,477.208496,302.187622,100),
(980044,0,91,28.830856,487.440247,300.407928,100),
(980044,0,92,32.842308,502.418274,295.821564,100),
(980044,0,93,40.684326,512.940613,291.814758,100),
(980044,0,94,49.940472,525.461853,286.520050,100),
(980044,0,95,48.196640,534.493042,283.842987,100),
(980044,0,96,33.374950,544.537903,281.611786,100),
(980044,0,97,22.199127,556.684570,279.023651,100),
(980044,0,98,18.910872,569.968262,275.898315,100),
(980044,0,99,17.708551,588.932068,272.132324,100),
(980044,0,100,17.132387,610.158081,269.384247,100),
(980044,0,101,17.775631,631.629883,269.080627,100),
(980044,0,102,15.631619,646.680603,271.115082,100),
(980044,0,103,9.020606,664.774902,273.488007,100),
(980044,0,104,3.919050,674.797913,274.454712,100),
(980044,0,105,1.358302,683.089600,273.273560,100),
(980044,0,106,-4.225438,703.724365,272.215912,100),
(980044,0,107,-8.095167,718.771851,273.067719,100),
(980044,0,108,-13.276932,737.035706,275.615875,100),
(980044,0,109,-17.490086,751.437744,278.988525,100),
(980044,0,110,-19.884523,758.522644,281.298462,100),
(980044,0,111,-23.311052,767.328857,280.632965,100),
(980044,0,112,-12.117568,770.899109,279.442322,100),
(980044,0,113,1.729707,772.897095,278.168396,100),
(980044,0,114,21.334959,775.916016,274.553833,100),
(980044,0,115,39.369770,778.335266,271.262421,100),
(980044,0,116,55.006588,784.364685,268.549896,100),
(980044,0,117,66.125053,798.636414,268.597595,100),
(980044,0,118,77.677032,811.037659,269.491852,100),
(980044,0,119,90.884827,818.847046,271.781982,100),
(980044,0,120,103.629944,831.142334,272.164429,100),
(980044,0,121,123.056961,845.771179,272.164429,100),
(980044,0,122,141.738556,859.924683,270.396973,100),
(980044,0,123,157.353516,876.227600,268.061127,100),
(980044,0,124,165.322388,893.687195,266.652832,100),
(980044,0,125,173.457550,912.395752,263.412445,100),
(980044,0,126,182.116180,933.217834,264.878235,100),
(980044,0,127,188.102783,950.405701,270.036163,100),
(980044,0,128,183.523911,959.629456,276.214325,100),
(980044,0,129,172.660736,966.573181,283.795380,100),
(980044,0,130,158.505142,978.121704,293.509460,100),
(980044,0,131,151.020996,988.377991,295.071747,100),
(980044,0,132,139.199982,993.286316,295.071747,100),
(980044,0,133,136.218872,1009.130798,295.181366,100),
(980044,0,134,146.970642,1023.959351,295.233704,100),
(980044,0,135,157.865997,1027.474487,296.136475,100),
(980044,0,136,171.367264,1040.999512,301.284882,100),
(980044,0,137,165.872223,1054.150024,308.965851,100),
(980044,0,138,164.649857,1066.390625,317.522949,100),
(980044,0,139,162.842133,1077.405396,321.912994,100),
(980044,0,140,150.648956,1086.625610,323.171570,100),
(980044,0,141,140.546173,1099.066772,326.514832,100),
(980044,0,142,129.663803,1104.193359,332.989990,100),
(980044,0,143,117.371361,1109.164062,343.253845,100),
(980044,0,144,106.712486,1111.796021,352.794739,100),
(980044,0,145,100.562592,1117.755981,358.684296,100),
(980044,0,146,94.313591,1125.515747,364.500061,100),
(980044,0,147,85.367973,1129.325806,367.095764,100);
SET @Point := 0;
INSERT INTO creature_movement_template (`entry`,`PathId`,`Point`,PositionX,PositionY,PositionZ,Orientation)
SELECT 980045 AS `entry`,`PathId`,(@Point := @Point + 1),PositionX,PositionY,PositionZ,Orientation
FROM creature_movement_template
WHERE `entry`=980044
ORDER BY `point` DESC;

DELETE FROM `creature_spawn_data_template` WHERE `entry`=980021;
INSERT INTO `creature_spawn_data_template` (`Entry`, `UnitFlags`, `Faction`, `ModelId`, `EquipmentId`, `CurHealth`, `CurMana`, `SpawnFlags`, `RelayId`, `Name`) VALUES
(980021, 0, 0, 0, 0, 1441720, 0, 0, 0, 'Grandmaster Training Dummy, set to 19% HP for execute testing');
DELETE FROM `creature_spawn_data` WHERE `Guid` IN (@CGUID+095,@CGUID+099,@CGUID+113);
INSERT INTO `creature_spawn_data` (`Guid`, `Id`) VALUES
(@CGUID+095,980021),
(@CGUID+099,980021),
(@CGUID+113,980021);

-- Arena Organizer
DELETE FROM `creature_addon` WHERE `guid` IN (@CGUID+120);
INSERT INTO `creature_addon` (`guid`, `mount`, `stand_state`, `sheath_state`, `emote`, `moveflags`, `auras`) VALUES
(@CGUID+120, 0, 1, 0, 0, 0, '');

DELETE FROM `dbscripts_on_relay` WHERE `id` BETWEEN 9999981 AND 9999999;
INSERT INTO `dbscripts_on_relay` (`id`, `delay`, priority, command, datalong, datalong2, datalong3, buddy_entry, search_radius, data_flags, dataint, dataint2, dataint3, dataint4, x, y, z, o, condition_id, comments) VALUES
(9999981, 0, 0, 1, 0, 0, 0, 0, 0, 0, 395, 396, 397, 401, 0, 0, 0, 0, 0, 'Battlemaster - Random Emote'),
(9999982, 0, 0, 1, 0, 0, 0, 0, 0, 0, 381, 387, 389, 390, 0, 0, 0, 0, 0, 'Battlemaster - Random Emote'),
(9999983, 0, 0, 1, 0, 0, 0, 0, 0, 0, 71, 113, 153, 213, 0, 0, 0, 0, 0, 'Battlemaster - Random Emote'),
(9999984, 0, 0, 1, 0, 0, 0, 0, 0, 0, 60, 61, 66, 70, 0, 0, 0, 0, 0, 'Battlemaster - Random Emote'),
(9999985, 0, 0, 1, 0, 0, 0, 0, 0, 0, 48, 51, 53, 54, 0, 0, 0, 0, 0, 'Battlemaster - Random Emote'),
(9999986, 0, 0, 1, 0, 0, 0, 0, 0, 0, 39, 43, 44, 45, 0, 0, 0, 0, 0, 'Battlemaster - Random Emote'),
(9999987, 0, 0, 1, 0, 0, 0, 0, 0, 0, 35, 36, 37, 38, 0, 0, 0, 0, 0, 'Battlemaster - Random Emote'),
(9999988, 0, 0, 1, 0, 0, 0, 0, 0, 0, 24, 25, 33, 34, 0, 0, 0, 0, 0, 'Battlemaster - Random Emote'),
(9999989, 0, 0, 1, 0, 0, 0, 0, 0, 0, 20, 21, 22, 23, 0, 0, 0, 0, 0, 'Battlemaster - Random Emote'),
(9999990, 0, 0, 1, 0, 0, 0, 0, 0, 0, 15, 17, 18, 19, 0, 0, 0, 0, 0, 'Battlemaster - Random Emote'),
(9999991, 0, 0, 1, 0, 0, 0, 0, 0, 0, 5, 6, 11, 14, 0, 0, 0, 0, 0, 'Battlemaster - Random Emote'),
(9999992, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 0, 0, 0, 0, 0, 'Battlemaster - Random Emote'),
(9999993, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Battlemaster - movement changed to 0:idle'),
(9999993, 0, 1, 1, 0, 0, 0, 0, 0, 0, 399, 400, 412, 415, 0, 0, 0, 0, 0, 'Battlemaster - Random EmoteState'),
(9999993, 20000, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Battlemaster - EmoteState 0'),
(9999993, 20000, 1, 20, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Battlemaster - movement changed to 2:waypoint'),
(9999994, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Battlemaster - movement changed to 0:idle'),
(9999994, 0, 1, 1, 0, 0, 0, 0, 0, 0, 386, 391, 392, 398, 0, 0, 0, 0, 0, 'Battlemaster - Random EmoteState'),
(9999994, 20000, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Battlemaster - EmoteState 0'),
(9999994, 20000, 1, 20, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Battlemaster - movement changed to 2:waypoint'),
(9999995, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Battlemaster - movement changed to 0:idle'),
(9999995, 0, 1, 1, 0, 0, 0, 0, 0, 0, 378, 383, 384, 385, 0, 0, 0, 0, 0, 'Battlemaster - Random EmoteState'),
(9999995, 20000, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Battlemaster - EmoteState 0'),
(9999995, 20000, 1, 20, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Battlemaster - movement changed to 2:waypoint'),
(9999996, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Battlemaster - movement changed to 0:idle'),
(9999996, 0, 1, 1, 0, 0, 0, 0, 0, 0, 253, 333, 375, 376, 0, 0, 0, 0, 0, 'Battlemaster - Random EmoteState'),
(9999996, 20000, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Battlemaster - EmoteState 0'),
(9999996, 20000, 1, 20, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Battlemaster - movement changed to 2:waypoint'),
(9999997, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Battlemaster - movement changed to 0:idle'),
(9999997, 0, 1, 1, 0, 0, 0, 0, 0, 0, 173, 214, 233, 234, 0, 0, 0, 0, 0, 'Battlemaster - Random EmoteState'),
(9999997, 20000, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Battlemaster - EmoteState 0'),
(9999997, 20000, 1, 20, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Battlemaster - movement changed to 2:waypoint'),
(9999998, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Battlemaster - movement changed to 0:idle'),
(9999998, 0, 1, 1, 0, 0, 0, 0, 0, 0, 64, 93, 94, 133, 0, 0, 0, 0, 0, 'Battlemaster - Random EmoteState'),
(9999998, 20000, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Battlemaster - EmoteState 0'),
(9999998, 20000, 1, 20, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Battlemaster - movement changed to 2:waypoint'),
(9999999, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Battlemaster - movement changed to 0:idle'),
(9999999, 0, 1, 1, 0, 0, 0, 0, 0, 0, 10, 27, 28, 29, 0, 0, 0, 0, 0, 'Battlemaster - Random EmoteState'),
(9999999, 20000, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Battlemaster - EmoteState 0'),
(9999999, 20000, 1, 20, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Battlemaster - movement changed to 2:waypoint');
DELETE FROM `dbscript_random_templates` WHERE `id`=9999999;
INSERT INTO `dbscript_random_templates` (`id`, `type`, `target_id`, `chance`, `comments`) VALUES
('9999999', '1', '9999981', '0', 'Battlemaster - Random OneShot Emote'),
('9999999', '1', '9999982', '0', 'Battlemaster - Random OneShot Emote'),
('9999999', '1', '9999983', '0', 'Battlemaster - Random OneShot Emote'),
('9999999', '1', '9999984', '0', 'Battlemaster - Random OneShot Emote'),
('9999999', '1', '9999985', '0', 'Battlemaster - Random OneShot Emote'),
('9999999', '1', '9999986', '0', 'Battlemaster - Random OneShot Emote'),
('9999999', '1', '9999987', '0', 'Battlemaster - Random OneShot Emote'),
('9999999', '1', '9999988', '0', 'Battlemaster - Random OneShot Emote'),
('9999999', '1', '9999989', '0', 'Battlemaster - Random OneShot Emote'),
('9999999', '1', '9999990', '0', 'Battlemaster - Random OneShot Emote'),
('9999999', '1', '9999991', '0', 'Battlemaster - Random OneShot Emote'),
('9999999', '1', '9999992', '0', 'Battlemaster - Random OneShot Emote'),
('9999999', '1', '9999993', '0', 'Battlemaster - Random State Emote'),
('9999999', '1', '9999994', '0', 'Battlemaster - Random State Emote'),
('9999999', '1', '9999995', '0', 'Battlemaster - Random State Emote'),
('9999999', '1', '9999996', '0', 'Battlemaster - Random State Emote'),
('9999999', '1', '9999997', '0', 'Battlemaster - Random State Emote'),
('9999999', '1', '9999998', '0', 'Battlemaster - Random State Emote'),
('9999999', '1', '9999999', '0', 'Battlemaster - Random State Emote');
DELETE FROM dbscripts_on_creature_movement WHERE id=9999999;
INSERT INTO dbscripts_on_creature_movement (id, `delay`, priority, command, datalong, datalong2, datalong3, buddy_entry, search_radius, data_flags, dataint, dataint2, dataint3, dataint4, x, y, z, o, condition_id, comments) VALUES
(9999999, 0, 0, 45, 0, 9999999, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Battlemaster - Start Random Script');
DELETE FROM `creature_movement` WHERE `Id`=@CGUID+119;
INSERT INTO `creature_movement` (`Id`, `Point`, `PositionX`, `PositionY`, `PositionZ`, `Orientation`, `WaitTime`, `ScriptId`, `Comment`) VALUES
(@CGUID+119, 1, 4274.98, -2785.63, 7.148, 2.74164, 5000, 9999999, 'Battlemaster');

DELETE FROM dbscripts_on_relay WHERE `id` BETWEEN 9999977 AND 9999980;
INSERT INTO dbscripts_on_relay (id, `delay`, priority, command, datalong, datalong2, datalong3, buddy_entry, search_radius, data_flags, dataint, dataint2, dataint3, dataint4, x, y, z, o, condition_id, comments) VALUES
(9999977, 0, 0, 23, 549, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Avatar of the Elements - Morph'),
(9999978, 0, 0, 23, 21130, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Avatar of the Elements - Morph'),
(9999979, 0, 0, 23, 21131, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Avatar of the Elements - Morph'),
(9999980, 0, 0, 23, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Avatar of the Elements - Demorph');
DELETE FROM `dbscript_random_templates` WHERE `id`=9999998;
INSERT INTO `dbscript_random_templates` (`id`, `type`, `target_id`, `chance`, `comments`) VALUES
('9999998', '1', '9999977', '0', 'Avatar of the Elements - Morph'),
('9999998', '1', '9999978', '0', 'Avatar of the Elements - Morph'),
('9999998', '1', '9999979', '0', 'Avatar of the Elements - Morph'),
('9999998', '1', '9999980', '0', 'Avatar of the Elements - Demorph');
DELETE FROM creature_ai_scripts WHERE creature_id=980010;
INSERT INTO creature_ai_scripts (id, creature_id, event_type, event_inverse_phase_mask, event_chance, event_flags, event_param1, event_param2, event_param3, event_param4, event_param5, event_param6, action1_type, action1_param1, action1_param2, action1_param3, action2_type, action2_param1, action2_param2, action2_param3, action3_type, action3_param1, action3_param2, action3_param3, `comment`) VALUES
('98001001', '980010', '1', '0', '100', '1', '1000', '5000', '5000', '10000', '0', '0', '53', '-9999998', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'Avatar of the Elements - Start Random Relay Script on Timer OOC');

-- Master Provisioner
DELETE FROM `creature_movement_template` WHERE `Entry`=980017;
INSERT INTO `creature_movement_template` (`Entry`,`PathId`,`Point`,`PositionX`,`PositionY`,`PositionZ`,`Orientation`,`WaitTime`,`ScriptId`,`Comment`) VALUES
(980017, 0, 1, 4295.95, -2764.31, 16.2243, 100, 0, 0,''),
(980017, 0, 2, 4302.89, -2761.94, 16.76, 100, 0, 0, ''),
(980017, 0, 3, 4306.65, -2759.31, 16.6333, 0.680156, 10000, 9999998, ''),
(980017, 0, 4, 4301.73, -2757.62, 16.7206, 100, 0, 0, ''),
(980017, 0, 5, 4296.77, -2759.62, 16.577, 100, 0, 0, ''),
(980017, 0, 6, 4292.58, -2762.06, 15.959, 100, 0, 0, ''),
(980017, 0, 7, 4287.65, -2765.55, 14.2883, 100, 0, 0, ''),
(980017, 0, 8, 4280.71, -2768.97, 10.3149, 100, 0, 0, ''),
(980017, 0, 9, 4276.31, -2774.56, 5.74921, 100, 0, 0, ''),
(980017, 0, 10, 4271.87, -2776.67, 5.602, 100, 0, 0, ''),
(980017, 0, 11, 4267.84, -2770.87, 6.14754, 100, 0, 0, ''),
(980017, 0, 12, 4269.84, -2765.49, 6.73516, 100, 0, 0, ''),
(980017, 0, 13, 4276.59, -2762.09, 6.5711, 100, 0, 0, ''),
(980017, 0, 14, 4285.57, -2761.73, 6.60474, 100, 0, 0, ''),
(980017, 0, 15, 4290.82, -2767.08, 7.10317, 100, 0, 0, ''),
(980017, 0, 16, 4300.77, -2774.79, 7.58169, 100, 0, 0, ''),
(980017, 0, 17, 4299.15, -2777.62, 7.55807, 4.11413, 25000, 9999997, ''),
(980017, 0, 18, 4303.08, -2779.85, 7.47154, 100, 0, 0, ''),
(980017, 0, 19, 4300.84, -2786.41, 6.90507, 100, 0, 0, ''),
(980017, 0, 20, 4292.52, -2787.13, 6.61355, 100, 0, 0, ''),
(980017, 0, 21, 4282.77, -2789.27, 5.82883, 100, 0, 0, ''),
(980017, 0, 22, 4273.55, -2790.45, 5.42234, 100, 0, 0, ''),
(980017, 0, 23, 4269.53, -2786.97, 5.6453, 100, 0, 0, ''),
(980017, 0, 24, 4270.41, -2782.25, 5.55202, 100, 0, 0, ''),
(980017, 0, 25, 4277.72, -2776.76, 5.82516, 100, 0, 0, ''),
(980017, 0, 26, 4284.018066, -2771.373291, 11.447682, 100, 0, 0, ''),
(980017, 0, 27, 4290.791016, -2765.192139, 15.153986, 100, 0, 0, ''),
(980017, 0, 28, 4292.58, -2762.06, 15.959, 100, 0, 0, ''),
(980017, 0, 29, 4292.660645, -2758.832275, 16.459620, 5.165036, 180000, 0, '');

DELETE FROM `dbscripts_on_creature_movement` WHERE `id` IN (9999998,9999997);
INSERT INTO `dbscripts_on_creature_movement` (id, `delay`, priority, command, datalong, datalong2, datalong3, buddy_entry, search_radius, data_flags, dataint, dataint2, dataint3, dataint4, x, y, z, o, condition_id, comments) VALUES
(9999998, 0, 0, 42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Master Provisioner - Set Equipment Slots - Empty handed'),
(9999998, 1000, 0, 1, 133, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Master Provisioner - Use EmoteState'),
(9999998, 8000, 0, 42, 0, 0, 0, 0, 0, 0, 19292, 0, 0, 0, 0, 0, 0, 0, 0, 'Master Provisioner - Set Equipment Slots - Last Month''s Mutton'),
(9999998, 9000, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Master Provisioner - Reset Emote'),

(9999997, 1000, 0, 1, 133, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Master Provisioner - Use EmoteState'),
(9999997, 24000, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Master Provisioner - Reset Emote'),
(9999997, 70000, 0, 42, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Master Provisioner - Reset Equipment - Default');

-- Peasants
DELETE FROM `creature_movement` WHERE `Id` IN (@CGUID+132,@CGUID+133);
INSERT INTO `creature_movement` (`Id`, `Point`, `PositionX`, `PositionY`, `PositionZ`, `Orientation`, `WaitTime`, `ScriptId`, `Comment`) VALUES
(@CGUID+132, 1, 4312.328125, -2802.904541, 4.864039, 3.032768, 20000, 9999996, ''),
(@CGUID+132, 2, 4339.327637, -2806.349609, 4.853236, 0.444877, 500, 9999995, ''),
(@CGUID+133, 1, 4327.838379, -2789.811523, 6.834820, 3.904552, 20000, 9999996, ''),
(@CGUID+133, 2, 4342.054199, -2802.174316, 4.810671, 4.572153, 500, 9999995, '');
DELETE FROM `dbscripts_on_creature_movement` WHERE `id` BETWEEN 9999996 AND 9999995;
INSERT INTO `dbscripts_on_creature_movement` (`id`,`delay`,`command`,`datalong`,`datalong2`,`datalong3`,`dataint`,`dataint2`,`dataint3`,`dataint4`,`buddy_entry`,`search_radius`,`data_flags`,`comments`,`x`,`y`,`z`,`o`) VALUES
(9999996,0,25,1,0,0,0,0,0,0,0,0,0,'Peasant - Run On',0,0,0,0),
(9999996,0,1,234,0,0,0,0,0,0,0,0,0,'Peasant - STATE_WORK_CHOPWOOD',0,0,0,0),
(9999996,20000,1,0,0,0,0,0,0,0,0,0,0,'Peasant - ONESHOT_NONE',0,0,0,0),
(9999996,20000,15,34450,0,0,0,0,0,0,0,0,0,'Peasant - Cast Transform: Peasant w/ wood 1.5 scale',0,0,0,0),
(9999995,0,14,34450,0,0,0,0,0,0,0,0,0,'Peasant - Remove Aura 34450',0,0,0,0);
-- Cat
DELETE FROM `creature_addon` WHERE `guid`=@CGUID+116;
INSERT INTO `creature_addon` (`guid`, `mount`, `stand_state`, `sheath_state`, `emote`, `moveflags`, `auras`) VALUES
(@CGUID+116, 0, 1, 0, 0, 0, NULL);

-- Zip-a-dee-doo-dah <Model Viewer>
DELETE FROM `creature_movement` WHERE `Id`=@CGUID+134;
INSERT INTO `creature_movement` (`Id`, `Point`, `PositionX`, `PositionY`, `PositionZ`, `Orientation`, `WaitTime`, `ScriptId`, `Comment`) VALUES
(@CGUID+134, 1, 4223.45, -2730.8, 5.20624, 100, 1000, 9999994, ''),
(@CGUID+134, 2, 4291.79, -2854.883, 5.2294, 100, 1000, 9999994, '');
DELETE FROM `dbscripts_on_creature_movement` WHERE `id`=9999994;
INSERT INTO `dbscripts_on_creature_movement` (id, `delay`, priority, command, datalong, datalong2, datalong3, buddy_entry, search_radius, data_flags, dataint, dataint2, dataint3, dataint4, x, y, z, o, condition_id, comments) VALUES
(9999994, 0, 0, 20, 1, 5, 60000, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Zip-a-dee-doo-dah - Change movement to random around point for 60 seconds');

-- -----------------------
-- Instant 58 Starter Gear
-- -----------------------
-- use official blizzard unused IDs so that item icon in player inventory does not appear as '?'
REPLACE INTO `item_template` (`entry`, `class`, `subclass`, `unk0`, `name`, `displayid`, `Quality`, `Flags`, `BuyCount`, `BuyPrice`, `SellPrice`, `InventoryType`, `AllowableClass`, `AllowableRace`, `ItemLevel`, `RequiredLevel`, `RequiredSkill`, `RequiredSkillRank`, `requiredspell`, `requiredhonorrank`, `RequiredCityRank`, `RequiredReputationFaction`, `RequiredReputationRank`, `maxcount`, `stackable`, `ContainerSlots`, `stat_type1`, `stat_value1`, `stat_type2`, `stat_value2`, `stat_type3`, `stat_value3`, `stat_type4`, `stat_value4`, `stat_type5`, `stat_value5`, `stat_type6`, `stat_value6`, `stat_type7`, `stat_value7`, `stat_type8`, `stat_value8`, `stat_type9`, `stat_value9`, `stat_type10`, `stat_value10`, `dmg_min1`, `dmg_max1`, `dmg_type1`, `dmg_min2`, `dmg_max2`, `dmg_type2`, `dmg_min3`, `dmg_max3`, `dmg_type3`, `dmg_min4`, `dmg_max4`, `dmg_type4`, `dmg_min5`, `dmg_max5`, `dmg_type5`, `armor`, `holy_res`, `fire_res`, `nature_res`, `frost_res`, `shadow_res`, `arcane_res`, `delay`, `ammo_type`, `RangedModRange`, `spellid_1`, `spelltrigger_1`, `spellcharges_1`, `spellppmRate_1`, `spellcooldown_1`, `spellcategory_1`, `spellcategorycooldown_1`, `spellid_2`, `spelltrigger_2`, `spellcharges_2`, `spellppmRate_2`, `spellcooldown_2`, `spellcategory_2`, `spellcategorycooldown_2`, `spellid_3`, `spelltrigger_3`, `spellcharges_3`, `spellppmRate_3`, `spellcooldown_3`, `spellcategory_3`, `spellcategorycooldown_3`, `spellid_4`, `spelltrigger_4`, `spellcharges_4`, `spellppmRate_4`, `spellcooldown_4`, `spellcategory_4`, `spellcategorycooldown_4`, `spellid_5`, `spelltrigger_5`, `spellcharges_5`, `spellppmRate_5`, `spellcooldown_5`, `spellcategory_5`, `spellcategorycooldown_5`, `bonding`, `description`, `PageText`, `LanguageID`, `PageMaterial`, `startquest`, `lockid`, `Material`, `sheath`, `RandomProperty`, `RandomSuffix`, `block`, `itemset`, `MaxDurability`, `area`, `Map`, `BagFamily`, `TotemCategory`, `socketColor_1`, `socketContent_1`, `socketColor_2`, `socketContent_2`, `socketColor_3`, `socketContent_3`, `socketBonus`, `GemProperties`, `RequiredDisenchantSkill`, `ArmorDamageModifier`, `ScriptName`, `DisenchantID`, `FoodType`, `minMoneyLoot`, `maxMoneyLoot`, `Duration`, `ExtraFlags`) VALUES
-- ------------
-- SHARED ITEMS
-- ------------
-- food/drink
('1072','0','5','-1','Vengeance Starter Water','18080','1','0','5','0','0','0','-1','-1','70','58','0','0','0','0','0','0','0','0','20','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','22734','0','-1','-1','0','59','1000','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','','0','0','0','0','0','3','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('33822','0','5','-1','Vengeance Starter Fish','1208','1','0','5','0','0','0','-1','-1','65','58','0','0','0','0','0','0','0','0','20','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','27094','0','-1','-1','0','11','1000','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','','0','0','0','0','0','-1','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','2','0','0','0','0'),
('12211','0','5','-1','Vengeance Starter Meat','22196','1','0','5','0','0','0','-1','-1','65','58','0','0','0','0','0','0','0','0','20','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','27094','0','-1','-1','0','11','1000','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','','0','0','0','0','0','-1','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','1','0','0','0','0'),
('1165','0','5','-1','Vengeance Starter Cheese','6422','1','0','5','0','0','0','-1','-1','65','58','0','0','0','0','0','0','0','0','20','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','27094','0','-1','-1','0','11','1000','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','','0','0','0','0','0','-1','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','3','0','0','0','0'),
('21236','0','5','-1','Vengeance Starter Bread','6343','1','0','5','0','0','0','-1','-1','65','58','0','0','0','0','0','0','0','0','20','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','27094','0','-1','-1','0','11','1000','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','','0','0','0','0','0','-1','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','4','0','0','0','0'),
('24541','0','5','-1','Vengeance Starter Fungus','3427','1','0','5','0','0','0','-1','-1','65','58','0','0','0','0','0','0','0','0','20','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','27094','0','-1','-1','0','11','1000','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','','0','0','0','0','0','-1','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','5','0','0','0','0'),
('12763','0','5','-1','Vengeance Starter Fruit','26735','1','0','5','0','0','0','-1','-1','65','58','0','0','0','0','0','0','0','0','20','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','27094','0','-1','-1','0','11','1000','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','','0','0','0','0','0','-1','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','6','0','0','0','0'),
('37460','0','7','-1','Vengeance Starter Bandage','11907','1','2097216','5','0','0','0','-1','-1','58','0','129','225','0','0','0','0','0','0','20','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','18610','0','-1','-1','0','150','1000','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','','0','0','0','0','0','7','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),

-- ammo
('2514','6','2','-1','Vengeance Starter Arrow','5996','1','0','200','0','0','24','-1','-1','80','58','0','0','0','0','0','0','0','0','200','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','22','22','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','3000','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','','0','0','0','0','0','2','0','0','0','0','0','0','0','0','1','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('19286','6','3','-1','Vengeance Starter Bullet','26613','1','0','200','0','0','24','-1','-1','80','58','0','0','0','0','0','0','0','0','200','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','22','22','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','3000','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','','0','0','0','0','0','2','0','0','0','0','0','0','0','0','2','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),

-- all classes
('6711','4','0','-1','Vengeance Starter Band','9834','2','1','1','0','0','11','-1','-1','60','58','0','0','0','0','0','0','0','0','1','0','7','13','6','4','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','5','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','200','0','','0','0','0','0','0','0'),
('1122','4','0','-1','Vengeance Starter Talisman','6494','2','1','1','0','0','2','-1','-1','47','58','0','0','0','0','0','0','0','0','1','0','7','10','6','2','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','3','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('2115','1','0','-1','Vengeance Starter Bag','3237','1','0','1','0','0','18','-1','-1','15','0','0','0','0','0','0','0','0','0','1','8','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','','0','0','0','0','0','8','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),

-- rogue, hunter, warrior, shaman, paladin, druid
('22230','4','1','-1','Vengeance Starter Cape','23081','3','1','1','0','0','16','-1','-1','52','58','0','0','0','0','0','0','0','0','1','0','7','10','3','10','4','5','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','37','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','7','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),

-- warlock, mage, priest, shaman, paladin, druid
('4011','4','1','-1','Vengeance Starter Cloak','15239','3','1','1','0','0','16','-1','-1','59','58','0','0','0','0','0','0','0','0','1','0','5','16','7','7','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','42','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','7','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),

-- hunter, warrior
('5548','2','2','-1','Vengeance Starter Bow','30683','3','1','1','0','0','15','-1','-1','58','58','0','0','0','0','0','0','0','0','1','0','3','3','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','64','119','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','2400','2','100','15464','1','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','2','0','0','0','0','0','75','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),

-- druid, rogue
('20269','4','2','-1','Vengeance Starter Wristguards','34549','3','1','1','0','0','9','-1','-1','60','58','0','0','0','0','0','0','0','0','1','0','4','9','7','10','3','10','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','74','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','8','0','0','0','0','0','35','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('20267','4','2','-1','Vengeance Starter Belt','14389','3','1','1','0','0','6','-1','-1','61','58','0','0','0','0','0','0','0','0','1','0','3','22','7','9','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','97','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','8','0','0','0','0','0','35','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('3533','4','2','-1','Vengeance Starter Pads','23765','3','1','1','0','0','8','-1','-1','60','58','0','0','0','0','0','0','0','0','1','0','7','14','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','116','0','0','0','0','0','0','0','0','0','14049','1','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','8','0','0','0','0','0','50','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),

-- warlock, mage, priest, druid
('2487','2','10','-1','Vengeance Starter Staff','28511','3','1','1','0','0','17','-1','-1','58','58','0','0','0','0','0','0','0','0','1','0','5','18','7','8','6','8','0','0','0','0','0','0','0','0','0','0','0','0','0','0','111','167','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','2800','0','0','21629','1','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','2','2','0','0','0','0','100','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),

-- warlock, mage, priest
('7186','2','19','-1','Vengeance Starter Wand','30660','3','0','1','0','0','26','-1','-1','62','57','0','0','0','0','0','0','0','0','1','0','5','4','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','83','155','6','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','1900','0','100','9416','1','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','2','0','0','0','0','0','65','0','0','0','0','0','0','0','0','0','0','0','0','225','0','','48','0','0','0','0','0'),
('7169','2','15','-1','Vengeance Starter Spellblade','29872','3','1','1','0','0','13','-1','-1','54','58','0','0','0','0','0','0','0','0','1','0','5','11','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','40','75','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','1600','0','0','21623','1','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','1','3','0','0','0','0','65','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('12863','4','0','-1','Vengeance Starter Spellbook','23321','3','1','1','0','0','23','-1','-1','47','58','0','0','0','0','0','0','0','0','1','0','6','13','5','4','7','5','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','1','','0','0','0','0','0','-1','2','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('21088','4','1','-1','Vengeance Starter Slippers','17256','3','1','1','0','0','8','-1','-1','56','58','0','0','0','0','0','0','0','0','1','0','5','14','6','13','7','11','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','55','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','7','0','0','0','0','0','40','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('26056','4','1','-1','Vengeance Starter Noose','24113','3','1','1','0','0','6','-1','-1','59','54','0','0','0','0','0','0','0','0','1','0','6','10','7','9','5','17','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','47','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','7','0','0','0','0','0','30','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('16119','4','1','-1','Vengeance Starter Wristwraps','34793','3','1','1','0','0','9','-1','-1','61','56','0','0','0','0','0','0','0','0','1','0','7','14','5','8','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','108','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','7','0','0','0','0','0','30','0','0','0','0','0','0','0','0','0','0','0','0','-1','70','','0','0','0','0','0','0'),

-- shaman, hunter
('1174','4','3','-1','Vengeance Starter Greaves','19898','3','1','1','0','0','8','-1','-1','54','58','0','0','0','0','0','0','0','0','1','0','7','20','3','6','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','221','0','0','0','0','0','0','0','0','0','9140','1','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','5','0','0','0','0','0','60','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('3525','4','3','-1','Vengeance Starter Bracers','29890','3','1','1','0','0','9','-1','-1','54','58','0','0','0','0','0','0','0','0','1','0','7','6','5','5','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','141','0','0','0','0','0','0','0','0','0','9336','1','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','5','0','0','0','0','0','40','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('1969','4','3','-1','Vengeance Starter Girdle','22907','3','1','1','0','0','6','-1','-1','60','58','0','0','0','0','0','0','0','0','1','0','3','6','7','6','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','199','0','0','0','0','0','0','0','0','0','15810','1','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','5','0','0','0','0','0','40','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),

-- shaman, paladin
('14609','4','6','-1','Vengeance Starter Buckler','18488','3','0','1','0','0','14','-1','-1','61','58','0','0','0','0','0','0','0','0','1','0','5','14','6','5','7','9','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','2089','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','6','4','0','0','38','0','100','0','0','0','0','0','0','0','0','0','0','0','0','225','0','','48','0','0','0','0','0'),
('1312','2','4','-1','Vengeance Starter Gavel','30440','3','0','1','0','0','21','-1','-1','60','55','0','0','0','0','0','0','0','0','1','0','5','5','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','66','123','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','2400','0','0','9343','1','0','0','0','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','2','3','0','0','0','0','90','0','0','0','0','0','0','0','0','0','0','0','0','200','0','','48','0','0','0','0','0'),

-- shaman, warrior
('25799','2','0','-1','Vengeance Starter Chopper','19841','3','1','1','0','0','13','-1','-1','56','50','0','0','0','0','0','0','0','0','1','0','4','6','31','10','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','62','116','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','2400','0','0','0','1','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','1','3','0','0','0','0','90','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('7940','2','1','-1','Vengeance Starter Battle Axe','13360','3','1','1','0','0','17','-1','-1','58','53','0','0','0','0','0','0','0','0','1','0','4','26','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','123','185','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','3100','0','0','7597','1','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','6','1','0','0','0','0','100','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),

-- paladin, warrior
('18431','4','4','-1','Vengeance Starter Shackles','29925','3','1','1','0','0','9','-1','-1','51','46','0','0','0','0','0','0','0','0','1','0','7','5','4','15','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','235','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','6','0','0','0','0','0','45','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('3952','4','4','-1','Vengeance Starter Girdle','28640','3','1','1','0','0','6','-1','-1','51','46','0','0','0','0','0','0','0','0','1','0','4','18','7','8','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','302','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','5','0','0','0','0','0','45','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('14689','4','4','-1','Vengeance Starter Footguards','23856','3','1','1','0','0','8','-1','-1','61','56','0','0','0','0','0','0','0','0','1','0','4','10','7','17','3','10','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','438','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','6','0','0','0','0','0','65','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),

-- --------------
-- SPECIFIC CLASS
-- --------------
-- hunter
('3479','4','3','-1','Vengeance Starter Hunter Tunic','31402','3','1','1','0','0','5','-1','-1','63','58','0','0','0','0','0','0','0','0','1','0','3','14','7','16','5','13','6','6','4','5','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','370','0','0','0','0','0','0','0','0','0','9335','1','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','5','0','0','0','0','0','120','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('3051','4','3','-1','Vengeance Starter Hunter Gloves','31406','3','1','1','0','0','10','-1','-1','59','54','0','0','0','0','0','0','0','0','1','0','3','10','6','10','7','9','5','9','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','218','0','0','0','0','0','0','0','0','0','9330','1','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','5','0','0','0','0','0','40','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('13795','4','3','-1','Vengeance Starter Hunter Cap','31410','3','1','1','0','0','1','-1','-1','62','57','0','0','0','0','0','0','0','0','1','0','3','14','7','20','5','10','6','6','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','297','0','0','0','0','0','0','0','0','0','9334','1','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','5','0','0','0','0','0','70','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('3050','4','3','-1','Vengeance Starter Hunter Pants','31403','3','1','1','0','0','7','-1','-1','61','56','0','0','0','0','0','0','0','0','1','0','3','18','6','12','7','6','4','6','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','315','0','0','0','0','0','0','0','0','0','15806','1','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','5','0','0','0','0','0','90','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('24534','4','3','-1','Vengeance Starter Hunter Mantle','31409','3','1','1','0','0','3','-1','-1','60','55','0','0','0','0','0','0','0','0','1','0','7','17','3','8','5','7','6','4','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','266','0','0','0','0','0','0','0','0','0','9142','1','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','5','0','0','0','0','0','70','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('5545','2','6','-1','Vengeance Starter Hunter Polearm','5290','3','1','1','0','0','17','-1','-1','59','54','0','0','0','0','0','0','0','0','1','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','137','206','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','3400','0','0','7597','1','0','0','-1','0','-1','15814','1','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','2','2','0','0','0','0','100','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('1281','11','2','-1','Vengeance Starter Hunter Quiver','21321','1','0','1','0','0','18','-1','-1','15','0','0','0','0','0','0','0','0','1','1','10','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','29418','1','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','-1','0','0','0','0','0','0','0','0','1','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('29889','11','3','-1','Vengeance Starter Hunter Ammo Pouch','1816','1','0','1','0','0','18','-1','-1','15','0','0','0','0','0','0','0','0','0','1','10','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','14824','1','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','-1','0','0','0','0','0','0','0','0','2','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('1047','2','3','-1','Vengeance Starter Hunter Gun','6595','3','0','1','0','0','26','-1','-1','58','53','0','0','0','0','0','0','0','0','1','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','61','114','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','2300','3','100','29635','1','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','2','','0','0','0','0','0','1','0','0','0','0','0','75','0','0','0','0','0','0','0','0','0','0','0','0','200','0','','48','0','0','0','0','0'),

-- rogue
('23652','2','7','-1','Vengeance Starter Rogue Scimitar','25641','3','0','1','0','0','13','-1','-1','57','52','0','0','0','0','0','0','0','0','1','0','4','3','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','71','132','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','2700','0','0','7597','1','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','2','','0','0','0','0','0','1','3','0','0','0','0','90','0','0','0','0','0','0','0','0','0','0','0','0','200','0','','48','0','0','0','0','0'),
('1018','2','7','-1','Vengeance Starter Rogue Cutlass','22733','3','1','1','0','0','13','-1','-1','54','49','0','0','0','0','0','0','0','0','1','0','4','10','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','47','89','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','1900','0','0','9141','1','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','1','3','0','0','0','0','90','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('1313','2','15','-1','Vengeance Starter Rogue Dirk','22721','3','1','1','0','0','13','-1','-1','51','46','0','0','0','0','0','0','0','1','1','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','42','80','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','1800','0','0','16454','2','0','2','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','1','3','0','0','0','0','65','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('7167','2','15','-1','Vengeance Starter Rogue Dagger','6443','3','1','1','0','0','22','-1','-1','62','57','0','0','0','0','0','0','0','0','1','0','37','14','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','42','64','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','1300','0','0','0','1','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','1','3','0','0','0','0','65','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('20270','4','2','-1','Vengeance Starter Rogue Cap','28180','3','1','1','0','0','1','-1','-1','62','57','0','0','0','0','0','0','0','0','1','0','3','20','7','18','4','13','6','5','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','141','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','8','0','0','0','0','0','60','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('20308','4','2','-1','Vengeance Starter Rogue Spaulders','28179','3','1','1','0','0','3','-1','-1','60','55','0','0','0','0','0','0','0','0','1','0','3','22','7','9','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','127','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','8','0','0','0','0','0','60','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('3532','4','2','-1','Vengeance Starter Rogue Pants','28161','3','1','1','0','0','7','-1','-1','61','56','0','0','0','0','0','0','0','0','1','0','3','25','7','12','4','12','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','150','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','8','0','0','0','0','0','75','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('20271','4','2','-1','Vengeance Starter Rogue Gloves','28166','3','1','1','0','0','10','-1','-1','59','54','0','0','0','0','0','0','0','0','1','0','3','14','6','10','7','9','4','9','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','105','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','8','0','0','0','0','0','35','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('20274','4','2','-1','Vengeance Starter Rogue Tunic','28160','3','1','1','0','0','5','-1','-1','63','58','0','0','0','0','0','0','0','0','1','0','3','26','7','13','6','12','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','176','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','8','0','0','0','0','0','100','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('25877','2','16','-1','Vengeance Starter Rogue Throwing Knife','26361','3','4194304','1','0','0','25','-1','-1','60','58','0','0','0','0','0','0','0','0','1','0','3','6','7','5','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','54','102','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','2200','0','100','9141','1','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','1','0','0','0','0','0','240','0','0','0','0','0','0','0','0','0','0','0','0','200','0','','48','0','0','0','0','0'),
('1128','0','8','-1','Vengeance Starter Rogue Instant Poison V','1150','1','2097216','1','0','0','0','8','-1','52','52','0','0','0','0','0','0','0','0','20','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','11339','0','-1','-1','0','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','','0','0','0','0','0','3','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('1851','0','8','-1','Vengeance Starter Rogue Deadly Poison IV','6340','1','2097216','1','0','0','0','8','-1','54','54','0','0','0','0','0','0','0','0','20','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','11356','0','-1','-1','0','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','','0','0','0','0','0','3','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),

-- druid
('5554','2','5','-1','Vengeance Starter Druid Mace','25625','3','1','1','0','0','17','-1','-1','57','52','0','0','0','0','0','0','0','0','1','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','105','159','0','0','0','0','0','0','0','0','0','0','0','0','0','30','0','0','0','0','0','0','2700','0','0','15464','1','0','0','-1','0','-1','7598','1','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','2','1','0','0','0','0','100','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('1535','4','2','-1','Vengeance Starter Druid Vest','29974','3','1','1','0','0','5','-1','-1','63','58','0','0','0','0','0','0','0','0','1','0','5','20','6','20','7','13','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','176','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','8','0','0','0','0','0','100','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('3549','4','2','-1','Vengeance Starter Druid Gloves','29979','3','1','1','0','0','10','-1','-1','59','54','0','0','0','0','0','0','0','0','1','0','6','21','5','9','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','105','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','8','0','0','0','0','0','35','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('1424','4','2','-1','Vengeance Starter Druid Spaulders','30412','3','1','1','0','0','3','-1','-1','60','55','0','0','0','0','0','0','0','0','1','0','5','18','7','9','6','8','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','127','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','8','0','0','0','0','0','60','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('14388','4','2','-1','Vengeance Starter Druid Kilt','29975','3','1','1','0','0','7','-1','-1','61','56','0','0','0','0','0','0','0','0','1','0','5','14','6','14','7','14','4','13','3','12','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','150','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','8','0','0','0','0','0','75','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('1023','4','2','-1','Vengeance Starter Druid Cowl','31228','3','1','1','0','0','1','-1','-1','62','57','0','0','0','0','0','0','0','0','1','0','5','20','6','20','7','10','4','6','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','141','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','8','0','0','0','0','0','60','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('25667','4','8','-1','Vengeance Starter Druid Idol','40160','3','1','1','0','0','28','-1','-1','57','52','0','0','0','0','0','0','0','1','1','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','27851','1','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','2','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),

-- mage
('20328','4','1','-1','Vengeance Starter Mage Gloves','29593','3','1','1','0','0','10','-1','-1','59','54','0','0','0','0','0','0','0','0','1','0','6','14','5','14','7','9','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','52','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','7','0','0','0','0','0','30','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('20327','4','1','-1','Vengeance Starter Mage Crown','31087','3','1','1','0','0','1','-1','-1','62','57','0','0','0','0','0','0','0','0','1','0','5','30','6','5','7','11','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','71','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','7','0','0','0','0','0','50','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('20343','4','1','-1','Vengeance Starter Mage Leggings','29273','3','1','1','0','0','7','-1','-1','61','56','0','0','0','0','0','0','0','0','1','0','6','21','5','20','7','12','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','76','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','7','0','0','0','0','0','65','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('20331','4','1','-1','Vengeance Starter Mage Robes','29591','3','1','1','0','0','20','-1','-1','63','58','0','0','0','0','0','0','0','0','1','0','5','31','6','8','7','9','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','89','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','7','0','0','0','0','0','80','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('20330','4','1','-1','Vengeance Starter Mage Mantle','30211','3','1','1','0','0','3','-1','-1','60','55','0','0','0','0','0','0','0','0','1','0','5','22','6','6','7','6','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','64','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','7','0','0','0','0','0','50','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),

-- paladin
('32110','4','4','-1','Vengeance Starter Paladin Gauntlets','29970','3','1','1','0','0','10','-1','-1','59','54','0','0','0','0','0','0','0','0','1','0','6','14','4','14','7','9','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','386','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','6','0','0','0','0','0','45','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('12615','4','4','-1','Vengeance Starter Paladin Breastplate','29969','3','1','1','0','0','5','-1','-1','63','58','0','0','0','0','0','0','0','0','1','0','7','21','5','16','4','13','6','8','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','657','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','6','0','0','0','0','0','135','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('2275','4','4','-1','Vengeance Starter Paladin Helm','31207','3','1','1','0','0','1','-1','-1','62','57','0','0','0','0','0','0','0','0','1','0','7','20','4','13','5','14','6','10','3','6','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','526','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','6','0','0','0','0','0','80','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('12413','4','4','-1','Vengeance Starter Paladin Legplates','29972','3','1','1','0','0','7','-1','-1','61','56','0','0','0','0','0','0','0','0','1','0','4','20','7','14','5','12','6','9','3','8','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','557','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','6','0','0','0','0','0','100','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('16031','4','4','-1','Vengeance Starter Paladin Spaulders','29971','3','1','1','0','0','3','-1','-1','60','55','0','0','0','0','0','0','0','0','1','0','7','15','5','11','4','9','6','5','3','4','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','470','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','6','0','0','0','0','0','80','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('23202','4','7','-1','Vengeance Starter Paladin Libram','34960','3','1','1','0','0','28','-1','-1','62','57','0','0','0','0','0','0','0','1','1','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','27848','1','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','2','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),

-- priest
('19844','4','1','-1','Vengeance Starter Priest Robe','30422','3','1','1','0','0','20','-1','-1','63','58','0','0','0','0','0','0','0','0','1','0','5','24','6','15','7','13','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','89','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','7','0','0','0','0','0','80','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('2573','4','1','-1','Vengeance Starter Priest Gloves','30427','3','1','1','0','0','10','-1','-1','59','54','0','0','0','0','0','0','0','0','1','0','6','17','5','10','7','9','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','52','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','7','0','0','0','0','0','30','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('16141','4','1','-1','Vengeance Starter Priest Crown','31104','3','1','1','0','0','1','-1','-1','62','57','0','0','0','0','0','0','0','0','1','0','5','24','6','15','7','13','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','71','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','7','0','0','0','0','0','50','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('25800','4','1','-1','Vengeance Starter Priest Skirt','30424','3','1','1','0','0','7','-1','-1','61','56','0','0','0','0','0','0','0','0','1','0','6','23','5','15','7','12','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','76','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','7','0','0','0','0','0','65','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('18747','4','1','-1','Vengeance Starter Priest Mantle','31103','3','1','1','0','0','3','-1','-1','60','55','0','0','0','0','0','0','0','0','1','0','5','21','6','9','7','4','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','64','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','7','0','0','0','0','0','50','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),

-- shaman
('20775','4','3','-1','Vengeance Starter Shaman Vest','31416','3','1','1','0','0','5','-1','-1','63','58','0','0','0','0','0','0','0','0','1','0','5','20','6','20','7','13','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','370','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','5','0','0','0','0','0','120','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('3529','4','3','-1','Vengeance Starter Shaman Coif','45174','3','1','1','0','0','1','-1','-1','62','57','0','0','0','0','0','0','0','0','1','0','5','23','7','13','6','12','4','7','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','297','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','5','0','0','0','0','0','70','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('3232','4','3','-1','Vengeance Starter Shaman Kilt','31415','3','1','1','0','0','7','-1','-1','61','56','0','0','0','0','0','0','0','0','1','0','6','20','5','15','4','12','7','7','3','6','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','315','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','5','0','0','0','0','0','90','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('20793','4','3','-1','Vengeance Starter Shaman Pauldrons','30925','3','1','1','0','0','3','-1','-1','60','55','0','0','0','0','0','0','0','0','1','0','5','15','7','14','6','6','4','6','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','266','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','5','0','0','0','0','0','70','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('20777','4','3','-1','Vengeance Starter Shaman Gauntlets','31414','3','1','1','0','0','10','-1','-1','59','54','0','0','0','0','0','0','0','0','1','0','6','16','5','10','4','9','7','4','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','218','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','5','0','0','0','0','0','40','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('6213','4','9','-1','Vengeance Starter Shaman Totem','4717','3','1','1','0','0','28','-1','-1','57','52','0','0','0','0','0','0','0','1','1','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','27859','1','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','2','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),

-- warlock
('24563','4','1','-1','Vengeance Starter Warlock Mask','31263','3','1','1','0','0','1','-1','-1','62','57','0','0','0','0','0','0','0','0','1','0','5','23','7','15','6','12','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','71','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','7','0','0','0','0','0','50','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('26165','4','1','-1','Vengeance Starter Warlock Leggings','29797','3','1','1','0','0','7','-1','-1','61','56','0','0','0','0','0','0','0','0','1','0','6','21','7','15','5','14','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','76','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','7','0','0','0','0','0','65','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('24564','4','1','-1','Vengeance Starter Warlock Robe','29792','3','1','1','0','0','20','-1','-1','63','58','0','0','0','0','0','0','0','0','1','0','5','21','7','20','6','13','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','89','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','7','0','0','0','0','0','80','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('24562','4','1','-1','Vengeance Starter Warlock Mantle','29798','3','1','1','0','0','3','-1','-1','60','55','0','0','0','0','0','0','0','0','1','0','5','15','7','14','6','9','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','64','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','7','0','0','0','0','0','50','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('24566','4','1','-1','Vengeance Starter Warlock Wraps','29800','3','1','1','0','0','10','-1','-1','59','54','0','0','0','0','0','0','0','0','1','0','6','14','5','9','7','13','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','52','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','7','0','0','0','0','0','30','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),

-- warrior
('20136','4','4','-1','Vengeance Starter Warrior Breastplate','29958','3','1','1','0','0','5','-1','-1','63','58','0','0','0','0','0','0','0','0','1','0','7','24','4','15','3','10','6','6','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','657','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','6','0','0','0','0','0','135','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('1028','4','4','-1','Vengeance Starter Warrior Helm','42241','3','1','1','0','0','1','-1','-1','62','57','0','0','0','0','0','0','0','0','1','0','7','23','4','15','3','9','6','8','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','526','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','6','0','0','0','0','0','80','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('3243','4','4','-1','Vengeance Starter Warrior Legplates','29963','3','1','1','0','0','7','-1','-1','61','56','0','0','0','0','0','0','0','0','1','0','4','23','7','15','3','11','6','4','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','557','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','6','0','0','0','0','0','100','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('33772','4','4','-1','Vengeance Starter Warrior Spaulders','29964','3','1','1','0','0','3','-1','-1','60','55','0','0','0','0','0','0','0','0','1','0','7','17','4','11','3','9','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','470','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','6','0','0','0','0','0','80','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('20284','4','4','-1','Vengeance Starter Warrior Gauntlets','29962','3','1','1','0','0','10','-1','-1','59','54','0','0','0','0','0','0','0','0','1','0','4','17','7','10','6','8','3','3','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','386','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','6','0','0','0','0','0','45','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('28961','4','6','-1','Vengeance Starter Warrior Shield','23825','3','1','1','0','0','14','-1','-1','60','55','0','0','0','0','0','0','0','0','1','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','2057','0','0','0','0','0','0','0','0','0','17495','1','0','0','0','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','1','4','0','0','38','0','100','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0'),
('31822','2','7','-1','Vengeance Starter Warrior Longsword','23734','3','1','1','0','0','13','-1','-1','57','52','0','0','0','0','0','0','0','0','1','0','7','12','4','7','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','55','103','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','2100','0','0','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','0','0','0','0','-1','0','-1','1','','0','0','0','0','0','1','3','0','0','0','0','90','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','','0','0','0','0','0','0');

-- all starter items are BoP and flagged as custom
UPDATE `item_template` SET `bonding`=1, `ExtraFlags`=`ExtraFlags`|4, `DisenchantID`=0, `RequiredDisenchantSkill`=-1, `BuyPrice`=0, `SellPrice`=0, `Quality`=2 WHERE `name` LIKE 'Vengeance Starter%';

-- food/water consumables max 60
UPDATE `item_template` SET `maxcount`=60 WHERE `entry` IN (1072,33822,12211,1165,21236,24541,12763,37460);

-- ammo consumables max 4000
UPDATE `item_template` SET `maxcount`=4000 WHERE `entry` IN (2514,19286);

-- rogue poisons
UPDATE `item_template` SET `maxcount`=5 WHERE `entry` IN (1128,1851);

-- Items used for user-specified suffix level 70 green gear set
REPLACE INTO `item_template` (`entry`, class, subclass, unk0, `name`, displayid, Quality, Flags, BuyCount, BuyPrice, SellPrice, InventoryType, AllowableClass, AllowableRace, ItemLevel, RequiredLevel, RequiredSkill, RequiredSkillRank, requiredspell, requiredhonorrank, RequiredCityRank, RequiredReputationFaction, RequiredReputationRank, maxcount, stackable, ContainerSlots, stat_type1, stat_value1, stat_type2, stat_value2, stat_type3, stat_value3, stat_type4, stat_value4, stat_type5, stat_value5, stat_type6, stat_value6, stat_type7, stat_value7, stat_type8, stat_value8, stat_type9, stat_value9, stat_type10, stat_value10, dmg_min1, dmg_max1, dmg_type1, dmg_min2, dmg_max2, dmg_type2, dmg_min3, dmg_max3, dmg_type3, dmg_min4, dmg_max4, dmg_type4, dmg_min5, dmg_max5, dmg_type5, armor, holy_res, fire_res, nature_res, frost_res, shadow_res, arcane_res, `delay`, ammo_type, RangedModRange, spellid_1, spelltrigger_1, spellcharges_1, spellppmRate_1, spellcooldown_1, spellcategory_1, spellcategorycooldown_1, spellid_2, spelltrigger_2, spellcharges_2, spellppmRate_2, spellcooldown_2, spellcategory_2, spellcategorycooldown_2, spellid_3, spelltrigger_3, spellcharges_3, spellppmRate_3, spellcooldown_3, spellcategory_3, spellcategorycooldown_3, spellid_4, spelltrigger_4, spellcharges_4, spellppmRate_4, spellcooldown_4, spellcategory_4, spellcategorycooldown_4, spellid_5, spelltrigger_5, spellcharges_5, spellppmRate_5, spellcooldown_5, spellcategory_5, spellcategorycooldown_5, bonding, `description`, PageText, LanguageID, PageMaterial, startquest, lockid, Material, sheath, RandomProperty, RandomSuffix, `block`, itemset, MaxDurability, area, Map, BagFamily, TotemCategory, socketColor_1, socketContent_1, socketColor_2, socketContent_2, socketColor_3, socketContent_3, socketBonus, GemProperties, RequiredDisenchantSkill, ArmorDamageModifier, ScriptName, DisenchantID, FoodType, minMoneyLoot, maxMoneyLoot, Duration, ExtraFlags) VALUES
-- plate
('12423', '4', '4', '-1', 'Vengeance Starter Helmet', '45183', '2', '0', '1', '0', '0', '1', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '831', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '6', '0', '0', '0', '0', '0', '70', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '14', '0', '0', '0', '0', '0'),
('20324', '4', '4', '-1', 'Vengeance Starter Girdle', '39643', '2', '0', '1', '0', '0', '6', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '575', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '6', '0', '0', '0', '0', '0', '40', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '14', '0', '0', '0', '0', '0'),
('16030', '4', '4', '-1', 'Vengeance Starter Greaves', '39646', '2', '0', '1', '0', '0', '8', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '703', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '6', '0', '0', '0', '0', '0', '55', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '14', '0', '0', '0', '0', '0'),
('15141', '4', '4', '-1', 'Vengeance Starter Breastplate', '39641', '2', '0', '1', '0', '0', '5', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1023', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '6', '0', '0', '0', '0', '0', '115', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '14', '0', '0', '0', '0', '0'),
('14694', '4', '4', '-1', 'Vengeance Starter Gauntlets', '39644', '2', '0', '1', '0', '0', '10', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '639', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '6', '0', '0', '0', '0', '0', '40', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '14', '0', '0', '0', '0', '0'),
('32171', '4', '4', '-1', 'Vengeance Starter Legplates', '39647', '2', '0', '1', '0', '0', '7', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '895', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '6', '0', '0', '0', '0', '0', '85', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '14', '0', '0', '0', '0', '0'), -- scuffed icon
('12407', '4', '4', '-1', 'Vengeance Starter Epaulets', '39651', '2', '0', '1', '0', '0', '3', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '767', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '6', '0', '0', '0', '0', '0', '70', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '14', '0', '0', '0', '0', '0'),
('16512', '4', '4', '-1', 'Vengeance Starter Vambraces', '39648', '2', '0', '1', '0', '0', '9', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '448', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '6', '0', '0', '0', '0', '0', '40', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '14', '0', '0', '0', '0', '0'),

-- shared
('7466', '4', '0', '-1', 'Vengeance Starter Ring', '9832', '2', '0', '1', '0', '0', '11', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '3', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '14', '0', '0', '0', '0', '0'),
('27228', '4', '0', '-1', 'Vengeance Starter Necklace', '4841', '2', '0', '1', '0', '0', '2', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '3', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '14', '0', '0', '0', '0', '0'),
('14701', '4', '1', '-1', 'Vengeance Starter Cape', '22691', '2', '0', '1', '0', '0', '16', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '69', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '7', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '14', '0', '0', '0', '0', '0'),
('4031', '4', '0', '-1', 'Vengeance Starter Insignia', '6515', '2', '32768', '1', '0', '0', '12', '-1', '-1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '42292', '0', '0', '0', '300000', '1182', '300000', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '1', '', '0', '0', '0', '0', '0', '-1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '', '0', '0', '0', '0', '0', '0'),

-- weapons
('32177', '2', '15', '-1', 'Vengeance Starter Mageblade', '39769', '2', '0', '1', '0', '0', '21', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '39.89', '100.89', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1700', '0', '0', '33140', '1', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '1', '3', '0', '0', '0', '0', '55', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '-18.3', '', '33', '0', '0', '0', '0', '0'),
('19359', '2', '4', '-1', 'Vengeance Starter Cudgel', '39774', '2', '0', '1', '0', '0', '21', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '39.89', '100.89', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1700', '0', '0', '33166', '1', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '2', '3', '0', '0', '0', '0', '75', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '-18.3', '', '33', '0', '0', '0', '0', '0'),
('12767', '2', '15', '-1', 'Vengeance Starter Knife', '40711', '2', '0', '1', '0', '0', '13', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '71', '132', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1700', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '1', '3', '0', '0', '0', '0', '55', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '33', '0', '0', '0', '0', '0'),
('26797', '2', '4', '-1', 'Vengeance Starter Club', '39750', '2', '0', '1', '0', '0', '13', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '108', '202', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '2600', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '2', '3', '0', '0', '0', '0', '75', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '33', '0', '0', '0', '0', '0'),
('1311', '2', '1', '-1', 'Vengeance Starter Battleaxe', '40302', '2', '0', '1', '0', '0', '17', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '217', '327', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '3500', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '1', '1', '0', '0', '0', '0', '85', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '33', '0', '0', '0', '0', '0'),
('26559', '4', '6', '-1', 'Vengeance Starter Shield', '28449', '2', '0', '1', '0', '0', '14', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '3278', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '1', '4', '0', '0', '77', '0', '85', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '33', '0', '0', '0', '0', '0'),
('20522', '2', '10', '-1', 'Vengeance Starter Greatstaff', '39753', '2', '0', '1', '0', '0', '17', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '152.95', '262.95', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '3500', '0', '0', '44900', '1', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '2', '2', '0', '0', '0', '0', '85', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '-18.3', '', '33', '0', '0', '0', '0', '0'),
('20313', '2', '2', '-1', 'Vengeance Starter Longbow', '28827', '2', '0', '1', '0', '0', '15', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '107', '200', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '2700', '2', '100', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '2', '0', '0', '0', '0', '0', '65', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '33', '0', '0', '0', '0', '0'),
('35770', '4', '0', '-1', 'Vengeance Starter Caster Trinket', '43359', '2', '64', '1', '0', '0', '12', '-1', '-1', '88', '0', '0', '0', '0', '0', '0', '0', '0', '1', '1', '0', '21', '26', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '33662', '0', '0', '0', '90000', '1141', '15000', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '1', '', '0', '0', '0', '0', '0', '3', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '225', '0', '', '50', '0', '0', '0', '0', '0'),
('35771', '4', '0', '-1', 'Vengeance Starter Physical Trinket', '43359', '2', '64', '1', '0', '0', '12', '-1', '-1', '88', '0', '0', '0', '0', '0', '0', '0', '0', '1', '1', '0', '32', '26', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '33667', '0', '0', '0', '90000', '1141', '15000', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '1', '', '0', '0', '0', '0', '0', '3', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '225', '0', '', '50', '0', '0', '0', '0', '0'),
('35772', '4', '0', '-1', 'Vengeance Starter Tank Trinket', '43359', '2', '64', '1', '0', '0', '12', '-1', '-1', '88', '0', '0', '0', '0', '0', '0', '0', '0', '1', '1', '0', '12', '26', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '33668', '0', '0', '0', '300000', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '1', '', '0', '0', '0', '0', '0', '3', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '225', '0', '', '50', '0', '0', '0', '0', '0'),
('35773', '4', '0', '-1', 'Vengeance Starter Healer Trinket', '43359', '2', '524288', '1', '0', '0', '12', '-1', '-1', '88', '61', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '18038', '1', '0', '0', '-1', '0', '-1', '31794', '0', '0', '0', '180000', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '1', '', '0', '0', '0', '0', '0', '3', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '225', '0', '', '50', '0', '0', '0', '0', '0'),
('32482', '2', '19', '-1', 'Vengeance Starter Wand', '41367', '2', '0', '1', '0', '0', '26', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '125', '234', '5', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1700', '0', '100', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '2', '0', '0', '0', '0', '0', '55', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '33', '0', '0', '0', '0', '0'),
('26573', '4', '0', '-1', 'Vengeance Starter Orb', '22923', '2', '0', '1', '0', '0', '23', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '3', '7', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '33', '0', '0', '0', '0', '0'),
('29210', '2', '16', '-1', 'Vengeance Starter Throwing Axe', '26358', '2', '4194304', '1', '0', '0', '25', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '64', '119', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1700', '0', '100', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '2', '0', '0', '0', '0', '0', '200', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '33', '0', '0', '0', '0', '0'),
('20811', '2', '7', '-1', 'Vengeance Starter Spellblade', '33220', '2', '0', '1', '0', '0', '21', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '39.89', '100.89', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1700', '0', '0', '33140', '1', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '1', '3', '0', '0', '0', '0', '75', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '-18.3', '', '33', '0', '0', '0', '0', '0'),
('23232', '2', '7', '-1', 'Vengeance Starter Sword', '25646', '2', '0', '1', '0', '0', '13', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '108', '202', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '2600', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '1', '3', '0', '0', '0', '0', '75', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '33', '0', '0', '0', '0', '0'),

-- cloth
('32129', '4', '1', '-1', 'Vengeance Starter Cowl', '39783', '2', '0', '1', '0', '0', '1', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '111', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '7', '0', '0', '0', '0', '0', '45', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '14', '0', '0', '0', '0', '0'),
('28016', '4', '1', '-1', 'Vengeance Starter Mantle', '39782', '2', '0', '1', '0', '0', '3', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '103', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '7', '0', '0', '0', '0', '0', '45', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '14', '0', '0', '0', '0', '0'),
('26082', '4', '1', '-1', 'Vengeance Starter Tunic', '39781', '2', '0', '1', '0', '0', '5', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '137', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '7', '0', '0', '0', '0', '0', '70', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '14', '0', '0', '0', '0', '0'), -- scuffed icon
('18341', '4', '1', '-1', 'Vengeance Starter Sash', '39778', '2', '0', '1', '0', '0', '6', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '77', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '7', '0', '0', '0', '0', '0', '25', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '14', '0', '0', '0', '0', '0'), -- scuffed icon
('30921', '4', '1', '-1', 'Vengeance Starter Pants', '39780', '2', '0', '1', '0', '0', '7', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '120', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '7', '0', '0', '0', '0', '0', '55', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '14', '0', '0', '0', '0', '0'), -- scuffed icon
('26097', '4', '1', '-1', 'Vengeance Starter Covers', '39777', '2', '0', '1', '0', '0', '8', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '94', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '7', '0', '0', '0', '0', '0', '35', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '14', '0', '0', '0', '0', '0'), -- scuffed icon
('32977', '4', '1', '-1', 'Vengeance Starter Bracers', '16892', '2', '0', '1', '0', '0', '9', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '60', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '7', '0', '0', '0', '0', '0', '25', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '14', '0', '0', '0', '0', '0'),
('30288', '4', '1', '-1', 'Vengeance Starter Handwraps', '39779', '2', '0', '1', '0', '0', '10', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '86', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '7', '0', '0', '0', '0', '0', '25', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '14', '0', '0', '0', '0', '0'),

-- leather
('16156', '4', '2', '-1', 'Vengeance Starter Skullcap', '35629', '2', '0', '1', '0', '0', '1', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '209', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '8', '0', '0', '0', '0', '0', '50', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '14', '0', '0', '0', '0', '0'),
('4728', '4', '2', '-1', 'Vengeance Starter Shoulderguards', '26035', '2', '0', '1', '0', '0', '3', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '193', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '8', '0', '0', '0', '0', '0', '50', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '14', '0', '0', '0', '0', '0'),
('1170', '4', '2', '-1', 'Vengeance Starter Jerkin', '39746', '2', '0', '1', '0', '0', '5', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '257', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '8', '0', '0', '0', '0', '0', '85', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '14', '0', '0', '0', '0', '0'), -- scuffed icon
('20789', '4', '2', '-1', 'Vengeance Starter Waistband', '39736', '2', '0', '1', '0', '0', '6', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '144', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '8', '0', '0', '0', '0', '0', '30', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '14', '0', '0', '0', '0', '0'),
('13776', '4', '2', '-1', 'Vengeance Starter Britches', '39738', '2', '0', '1', '0', '0', '7', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '225', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '8', '0', '0', '0', '0', '0', '65', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '14', '0', '0', '0', '0', '0'),
('26225', '4', '2', '-1', 'Vengeance Starter Boots', '39737', '2', '0', '1', '0', '0', '8', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '177', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '8', '0', '0', '0', '0', '0', '45', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '14', '0', '0', '0', '0', '0'), -- scuffed icon
('26015', '4', '2', '-1', 'Vengeance Starter Wristguards', '3652', '2', '0', '1', '0', '0', '9', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '112', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '8', '0', '0', '0', '0', '0', '30', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '14', '0', '0', '0', '0', '0'),
('26227', '4', '2', '-1', 'Vengeance Starter Gloves', '39744', '2', '0', '1', '0', '0', '10', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '160', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '8', '0', '0', '0', '0', '0', '30', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '14', '0', '0', '0', '0', '0'), -- scuffed icon

-- mail
('26388', '4', '3', '-1', 'Vengeance Starter Helm', '39793', '2', '0', '1', '0', '0', '1', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '465', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '5', '0', '0', '0', '0', '0', '60', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '14', '0', '0', '0', '0', '0'), -- scuffed icon
('32137', '4', '3', '-1', 'Vengeance Starter Spaulders', '39794', '2', '0', '1', '0', '0', '3', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '430', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '5', '0', '0', '0', '0', '0', '60', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '14', '0', '0', '0', '0', '0'), -- scuffed icon
('29899', '4', '3', '-1', 'Vengeance Starter Chestpiece', '39788', '2', '0', '1', '0', '0', '5', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '573', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '5', '0', '0', '0', '0', '0', '100', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '14', '0', '0', '0', '0', '0'), -- scuffed icon
('25794', '4', '3', '-1', 'Vengeance Starter Belt', '39786', '2', '0', '1', '0', '0', '6', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '322', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '5', '0', '0', '0', '0', '0', '35', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '14', '0', '0', '0', '0', '0'), -- scuffed icon
('25551', '4', '3', '-1', 'Vengeance Starter Legguards', '39792', '2', '0', '1', '0', '0', '7', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '501', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '5', '0', '0', '0', '0', '0', '75', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '14', '0', '0', '0', '0', '0'), -- scuffed icon
('28011', '4', '3', '-1', 'Vengeance Starter Footgear', '39789', '2', '0', '1', '0', '0', '8', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '394', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '5', '0', '0', '0', '0', '0', '50', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '14', '0', '0', '0', '0', '0'),
('32968', '4', '3', '-1', 'Vengeance Starter Armbands', '39787', '2', '0', '1', '0', '0', '9', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '251', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '5', '0', '0', '0', '0', '0', '35', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '14', '0', '0', '0', '0', '0'),
('32149', '4', '3', '-1', 'Vengeance Starter Fists', '39790', '2', '0', '1', '0', '0', '10', '-1', '-1', '111', '67', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '358', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '0', '0', '0', '0', '-1', '0', '-1', '2', '', '0', '0', '0', '0', '0', '5', '0', '0', '0', '0', '0', '35', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '275', '0', '', '14', '0', '0', '0', '0', '0');

UPDATE `item_template` SET `bonding`=1, `ExtraFlags`=`ExtraFlags`|4, `DisenchantID`=0, `RequiredDisenchantSkill`=-1, `BuyPrice`=0, `SellPrice`=0, `Quality`=2 WHERE `name` LIKE 'Vengeance Starter%';

-- Librams, Totems, and Idols
-- unfortunately, the real item is used here - not enough unused ids to make proper customs
UPDATE `item_template` SET `bonding`=1, `ExtraFlags`=`ExtraFlags`|4, `DisenchantID`=0, `RequiredDisenchantSkill`=-1, `BuyPrice`=0, `SellPrice`=0 WHERE `entry` IN (23201,22401,23203,23199,22395,23200,23197,28064,22398);


-- --------------------------------------------------------------------------------------------------------------------------------------------------------
-- Assigning all custom NPCs and objects to game corresponding game events so they may be easily toggled on/off

-- ======================================
-- Custom Event #1 - MantisLord's Funland
-- ======================================
REPLACE INTO `game_event_creature` (`guid`, `event`) SELECT `guid`, @PTR_FUNSERVER_GAME_EVENT_ID FROM `creature` WHERE `id` IN (980048,980047,980046,980043,980041,980040,980039,980038,980037,980036,980035,980034,980033,980032,980029,980021,980020,980019,980018,980017,980016,980015,980014,980013,980012,980011,980010,980009,980008,980007,980006,980005,980004,980003,980002,980001);
REPLACE INTO `game_event_creature` (`guid`, `event`) SELECT `guid`, @PTR_FUNSERVER_GAME_EVENT_ID FROM `creature` WHERE `id` BETWEEN 185000 AND 185100; -- Multivendors
REPLACE INTO `game_event_creature` (`guid`, `event`) SELECT `guid`, @PTR_FUNSERVER_GAME_EVENT_ID FROM `creature` WHERE `id` IN (14623,14879,15972,22516); -- Battlemasters
REPLACE INTO `game_event_gameobject` (`guid`, `event`) SELECT `guid`, @PTR_FUNSERVER_GAME_EVENT_ID FROM `gameobject` WHERE `id` IN (400000,400001,400002,400004,400005,400006,400007,400008,400009,400010,400011,400012,400013,400014,400015,400016,400017,400018,400019,400020,400021,400022,400023,400024,400025,400026,400027,400030,400031,400032,400033,400034,400035,400036,400037,400038,400039,400040);
REPLACE INTO `game_event_gameobject` (`guid`, `event`) VALUES (@OGUID+092, @PTR_FUNSERVER_GAME_EVENT_ID); -- Elevator 152614
-- --------- CREATURES ---------
-- Titler 980048
-- Legionnaire Gurak 980047
-- Captain Benedict 980046
-- Zip-a-dee-doo-dah <Model Viewer> 980043
-- Enchantment Crystal 980041
-- Paul <Stylist> 980040
-- Spirit Healer 980039
-- Arena Battlemaster 980038
-- Arena Organizer 980037
-- Auctioneer Grizzlin 980036
-- Izzy Coppergrab <Banker> 980035
-- Paymaster 980034
-- Beastmaster 980033
-- Transmogrification Beacon 980032
-- Test Realm Overlord 980029
-- Grandmaster Training Dummy 980021
-- Journeyman Training Dummy 980020
-- Novice Training Dummy 980019
-- Lockout Nullifier 980018
-- Master Provisioner 980017
-- Tier 5 980016
-- Gems 980015
-- Cat 980014
-- Arena Gear <Season 2> 980013
-- Race Changer 980012
-- Guild Creator 980011
-- Avatar of the Elements 980010
-- Impy <Demon Trainer> 980009
-- Mounts 980008
-- Poisons 980007
-- Jewelry 980006
-- Off-sets 980005
-- Off-hands 980004
-- Weapons 980003
-- Universal Class Trainer 980002
-- Peasant 980001
-- Warsong Gulch Battlemaster 14623
-- Arathi Basin Battlemaster 14879
-- Alterac Valley Battlemaster 15972
-- Eye of the Storm Battlemaster 22516
-- Multivendors (185000-185100)
-- --------- GAMEOBJECTS ---------
-- Elevator 152614
-- Portal 400000
-- Training Dummy Information 400001
-- Collision PC Size [x4] 400002
-- Forge 400004
-- Anvil 400005
-- Wooden Chair 400006
-- Weapon Rack 400007
-- Mailbox 400008
-- PVP HOLIDAY HORDE CTF 400009
-- PVP HOLIDAY HORDE AV 400010
-- PVP HOLIDAY HORDE ARATHI 400011
-- PVP HOLIDAY GENERIC SIGNPOST 400012
-- PVP HOLIDAY ALLIANCE ARATHI 400013
-- PVP HOLIDAY ALLIANCE AV 400014
-- PVP HOLIDAY ALLIANCE CTF 400015
-- Party Table 400016
-- Forge (invisible focus) 400017
-- Blood Elf Banner 400018
-- Draenei Banner 400019
-- Simon Game Aura Yellow 400020
-- Campfire 400021
-- Guild Vault 400022
-- Ghost Gate 400023
-- Collision PC Size 400024
-- Research Equipment 400025
-- Supply Crate 400026
-- Portal to Funland 400027
-- Horde Gamemaster Mailbox 400030
-- Portcullis 400031
-- Alliance Gamemaster Mailbox 400032
-- Chair 400033
-- Gamemaster Guild Vault 400034
-- Sunwell Plateau 400035
-- Peasant Woodpile 400036
-- Peasant Woodpile (large) 400037
-- Lumber Pile 400038
-- Excavation Tent Pavillion 400039
-- Torch 400040

-- =============================
-- Custom Event #2 - Level Boost
-- =============================
REPLACE INTO `game_event_creature` SELECT `guid`, @LEVEL_BOOST_GAME_EVENT_ID FROM `creature` WHERE `id` IN (980042,980028,980027,980026,980025,980024,980023);
REPLACE INTO `game_event_gameobject` SELECT `guid`, @LEVEL_BOOST_GAME_EVENT_ID FROM `gameobject` WHERE `id` IN (400003,400028,400029);
-- --------- CREATURES ---------
-- Beginner Training Dummy 980042
-- Alliance Greeter 980028
-- Horde Greeter 980027
-- Alliance Realm Information 980026
-- Horde Realm Information 980025
-- Alliance Enlistment Officer 980024
-- Horde Enlistment Officer 980023
-- --------- GAMEOBJECTS ---------
-- Teleporter Circle 400003
-- Alliance Mailbox 400028
-- Horde Mailbox 400029
DELETE FROM `game_event_creature` WHERE `event` IN (
@LEVEL_BOOST_RESTRICT_TBC_RACES_EVENT_ID,
@LEVEL_BOOST_RESTRICT_HORDE_RACES_EVENT_ID,
@LEVEL_BOOST_RESTRICT_ALLIANCE_RACES_EVENT_ID);
INSERT INTO `game_event_creature` (`guid`, `event`) VALUES
-- spawn Draenei and Blood Elf greeters located at Blood Watch and Tranquillien, since that's where players will likely be when they hit level 20
(@CGUID+047, @LEVEL_BOOST_RESTRICT_TBC_RACES_EVENT_ID),
(@CGUID+059, @LEVEL_BOOST_RESTRICT_TBC_RACES_EVENT_ID),
-- alternate 20+ Horde greeter spawns
(@CGUID+061, @LEVEL_BOOST_RESTRICT_HORDE_RACES_EVENT_ID),
(@CGUID+062, @LEVEL_BOOST_RESTRICT_HORDE_RACES_EVENT_ID),
(@CGUID+063, @LEVEL_BOOST_RESTRICT_HORDE_RACES_EVENT_ID),
(@CGUID+064, @LEVEL_BOOST_RESTRICT_HORDE_RACES_EVENT_ID),
(@CGUID+065, @LEVEL_BOOST_RESTRICT_HORDE_RACES_EVENT_ID),
(@CGUID+066, @LEVEL_BOOST_RESTRICT_HORDE_RACES_EVENT_ID),
-- alternate 20+ Alliance greeter spawns
(@CGUID+049, @LEVEL_BOOST_RESTRICT_ALLIANCE_RACES_EVENT_ID),
(@CGUID+050, @LEVEL_BOOST_RESTRICT_ALLIANCE_RACES_EVENT_ID),
(@CGUID+051, @LEVEL_BOOST_RESTRICT_ALLIANCE_RACES_EVENT_ID),
(@CGUID+052, @LEVEL_BOOST_RESTRICT_ALLIANCE_RACES_EVENT_ID),
(@CGUID+053, @LEVEL_BOOST_RESTRICT_ALLIANCE_RACES_EVENT_ID),
(@CGUID+054, @LEVEL_BOOST_RESTRICT_ALLIANCE_RACES_EVENT_ID),
(@CGUID+055, @LEVEL_BOOST_RESTRICT_ALLIANCE_RACES_EVENT_ID);

-- =====================================
-- Custom Event #3 - Dual Specialization
-- =====================================
REPLACE INTO `game_event_creature` SELECT `guid`, @DUAL_SPECIALIZATION_EVENT_ID FROM `creature` WHERE `id` IN (980022);
-- --------- CREATURES ---------
-- Dual Specialization Crystal 980022

-- --------------------------------------------------------------------------------------------------------------------------------------------------------





-- =================================================================================================================================================
-- =================================================================================================================================================
-- =================================================================================================================================================
-- VENDOR/TRAINER TEMPLATES
-- =================================================================================================================================================
-- =================================================================================================================================================
-- =================================================================================================================================================

-- Level Boost
-- free ranks of talent spells
DELETE FROM `npc_trainer_template` WHERE `entry` IN (980024);
INSERT INTO `npc_trainer_template` (`entry`, `spell`, `spellcost`, `reqskill`, `reqskillvalue`, `reqlevel`, `condition_id`) VALUES
-- Mortal Strike
(980024, 21551, 0, 0, 0, 48, 0),
(980024, 21552, 0, 0, 0, 54, 0),
(980024, 21553, 0, 0, 0, 60, 0),
(980024, 25248, 0, 0, 0, 66, 0),
(980024, 30330, 0, 0, 0, 70, 0),
-- Bloodthirst
(980024, 23892, 0, 0, 0, 48, 0),
(980024, 23893, 0, 0, 0, 54, 0),
(980024, 23894, 0, 0, 0, 60, 0),
(980024, 25251, 0, 0, 0, 66, 0),
(980024, 30335, 0, 0, 0, 70, 0),
-- Shield Slam
(980024, 23923, 0, 0, 0, 48, 0),
(980024, 23924, 0, 0, 0, 54, 0),
(980024, 23925, 0, 0, 0, 60, 0),
(980024, 25258, 0, 0, 0, 66, 0),
(980024, 30356, 0, 0, 0, 70, 0),
-- Devastate
(980024, 30016, 0, 0, 0, 60, 0),
(980024, 30022, 0, 0, 0, 70, 0),

-- Holy Shock
(980024, 20929, 0, 0, 0, 48, 0),
(980024, 20930, 0, 0, 0, 56, 0),
(980024, 27174, 0, 0, 0, 64, 0),
(980024, 33072, 0, 0, 0, 70, 0),
-- Blessing of Sanctuary
(980024, 20912, 0, 0, 0, 40, 0),
(980024, 20913, 0, 0, 0, 50, 0),
(980024, 20914, 0, 0, 0, 60, 0),
(980024, 27168, 0, 0, 0, 70, 0),
-- Greater Blessing of Sanctuary
(980024, 25899, 0, 0, 0, 60, 0),
(980024, 27169, 0, 0, 0, 70, 0),
-- Holy Shield
(980024, 20927, 0, 0, 0, 50, 0),
(980024, 20928, 0, 0, 0, 60, 0),
(980024, 27179, 0, 0, 0, 70, 0),
-- Seal of Command
(980024, 20915, 0, 0, 0, 30, 0),
(980024, 20918, 0, 0, 0, 40, 0),
(980024, 20919, 0, 0, 0, 50, 0),
(980024, 20920, 0, 0, 0, 60, 0),
(980024, 27170, 0, 0, 0, 70, 0),
-- Avenger's Shield
(980024, 32699, 0, 0, 0, 60, 0),
(980024, 32700, 0, 0, 0, 70, 0),
-- Greater Blessing of Kings
(980024, 25898, 0, 0, 0, 60, 0),

-- Nature's Grasp
(980024, 16810, 0, 0, 0, 18, 0),
(980024, 16811, 0, 0, 0, 28, 0),
(980024, 16812, 0, 0, 0, 38, 0),
(980024, 16813, 0, 0, 0, 48, 0),
(980024, 17329, 0, 0, 0, 58, 0),
(980024, 27009, 0, 0, 0, 68, 0),
-- Insect Swarm
(980024, 24974, 0, 0, 0, 30, 0),
(980024, 24975, 0, 0, 0, 40, 0),
(980024, 24976, 0, 0, 0, 50, 0),
(980024, 24977, 0, 0, 0, 60, 0),
(980024, 27013, 0, 0, 0, 70, 0),
-- Faerie Fire (Feral)
(980024, 17390, 0, 0, 0, 30, 0),
(980024, 17391, 0, 0, 0, 42, 0),
(980024, 17392, 0, 0, 0, 54, 0),
(980024, 27011, 0, 0, 0, 66, 0),
-- Mangle (Cat)
(980024, 33982, 0, 0, 0, 58, 0),
(980024, 33983, 0, 0, 0, 68, 0),
-- Mangle (Bear)
(980024, 33986, 0, 0, 0, 58, 0),
(980024, 33987, 0, 0, 0, 68, 0),

-- Divine Spirit
(980024, 14818, 0, 0, 0, 40, 0),
(980024, 14819, 0, 0, 0, 50, 0),
(980024, 27841, 0, 0, 0, 60, 0),
(980024, 25312, 0, 0, 0, 70, 0),
-- Holy Nova
(980024, 15430, 0, 0, 0, 28, 0),
(980024, 15431, 0, 0, 0, 36, 0),
(980024, 25331, 0, 0, 0, 68, 0),
(980024, 27799, 0, 0, 0, 44, 0),
(980024, 27800, 0, 0, 0, 52, 0),
(980024, 27801, 0, 0, 0, 60, 0),
-- Lightwell
(980024, 27870, 0, 0, 0, 50, 0),
(980024, 27871, 0, 0, 0, 60, 0),
(980024, 28275, 0, 0, 0, 70, 0),
-- Circle of Healing
(980024, 34863, 0, 0, 0, 56, 0),
(980024, 34864, 0, 0, 0, 60, 0),
(980024, 34865, 0, 0, 0, 65, 0),
(980024, 34866, 0, 0, 0, 70, 0),
-- Mind Flay
(980024, 17311, 0, 0, 0, 28, 0),
(980024, 17312, 0, 0, 0, 36, 0),
(980024, 17313, 0, 0, 0, 44, 0),
(980024, 17314, 0, 0, 0, 52, 0),
(980024, 18807, 0, 0, 0, 60, 0),
(980024, 25387, 0, 0, 0, 68, 0),
-- Vampiric Touch
(980024, 34916, 0, 0, 0, 60, 0),
(980024, 34917, 0, 0, 0, 70, 0),

-- Mutilate
(980024, 34411, 0, 0, 0, 50, 0),
(980024, 34412, 0, 0, 0, 60, 0),
(980024, 34413, 0, 0, 0, 70, 0),
-- Hemorrhage
(980024, 17347, 0, 0, 0, 46, 0),
(980024, 17348, 0, 0, 0, 58, 0),
(980024, 26864, 0, 0, 0, 70, 0),

-- Pyroblast
(980024, 12505, 0, 0, 0, 24, 0),
(980024, 12522, 0, 0, 0, 30, 0),
(980024, 12523, 0, 0, 0, 36, 0),
(980024, 12524, 0, 0, 0, 42, 0),
(980024, 12525, 0, 0, 0, 48, 0),
(980024, 12526, 0, 0, 0, 54, 0),
(980024, 18809, 0, 0, 0, 60, 0),
(980024, 27132, 0, 0, 0, 66, 0),
(980024, 33938, 0, 0, 0, 70, 0),
-- Blast Wave
(980024, 13018, 0, 0, 0, 36, 0),
(980024, 13019, 0, 0, 0, 44, 0),
(980024, 13020, 0, 0, 0, 52, 0),
(980024, 13021, 0, 0, 0, 60, 0),
(980024, 27133, 0, 0, 0, 65, 0),
(980024, 33933, 0, 0, 0, 70, 0),
-- Dragon's Breath
(980024, 33041, 0, 0, 0, 56, 0),
(980024, 33042, 0, 0, 0, 64, 0),
(980024, 33043, 0, 0, 0, 70, 0),
-- Ice Barrier
(980024, 13031, 0, 0, 0, 46, 0),
(980024, 13032, 0, 0, 0, 52, 0),
(980024, 13033, 0, 0, 0, 58, 0),
(980024, 27134, 0, 0, 0, 64, 0),
(980024, 33405, 0, 0, 0, 70, 0),

-- Siphon Life
(980024, 18879, 0, 0, 0, 38, 0),
(980024, 18880, 0, 0, 0, 48, 0),
(980024, 18881, 0, 0, 0, 58, 0),
(980024, 27264, 0, 0, 0, 63, 0),
(980024, 30911, 0, 0, 0, 70, 0),
-- Dark Pact
(980024, 18937, 0, 0, 0, 50, 0),
(980024, 18938, 0, 0, 0, 60, 0),
(980024, 27265, 0, 0, 0, 70, 0),
-- Shadowburn
(980024, 18867, 0, 0, 0, 24, 0),
(980024, 18868, 0, 0, 0, 32, 0),
(980024, 18869, 0, 0, 0, 40, 0),
(980024, 18870, 0, 0, 0, 48, 0),
(980024, 18871, 0, 0, 0, 56, 0),
(980024, 27263, 0, 0, 0, 63, 0),
(980024, 30546, 0, 0, 0, 70, 0),
-- Conflagrate
(980024, 18930, 0, 0, 0, 48, 0),
(980024, 18931, 0, 0, 0, 54, 0),
(980024, 18932, 0, 0, 0, 60, 0),
(980024, 27266, 0, 0, 0, 65, 0),
(980024, 30912, 0, 0, 0, 70, 0),

-- Aimed Shot
(980024, 20900, 0, 0, 0, 28, 0),
(980024, 20901, 0, 0, 0, 36, 0),
(980024, 20902, 0, 0, 0, 44, 0),
(980024, 20903, 0, 0, 0, 52, 0),
(980024, 20904, 0, 0, 0, 60, 0),
(980024, 27065, 0, 0, 0, 70, 0),
-- Trueshot Aura
(980024, 20905, 0, 0, 0, 50, 0),
(980024, 20906, 0, 0, 0, 60, 0),
(980024, 27066, 0, 0, 0, 70, 0),
-- Counterattack
(980024, 20909, 0, 0, 0, 42, 0),
(980024, 20910, 0, 0, 0, 54, 0),
(980024, 27067, 0, 0, 0, 66, 0),
-- Wyvern Sting
(980024, 24132, 0, 0, 0, 50, 0),
(980024, 24133, 0, 0, 0, 60, 0),
(980024, 27068, 0, 0, 0, 70, 0);
DELETE FROM `npc_trainer_template` WHERE `entry`=980024 AND `reqlevel` BETWEEN 61 AND 70; -- 47 talent spells restricted

-- Level Boost
DELETE FROM `npc_vendor_template` WHERE `entry`=980025;
INSERT INTO `npc_vendor_template` (`entry`, `item`, `maxcount`, `incrtime`, `ExtendedCost`, `condition_id`, `comments`) VALUES
-- 58 only
('980025', '6265', '0', '0', '0', '15047', 'Soul Shard (Warlock Only)'),
('980025', '1128', '0', '0', '0', '15050', 'Vengeance Starter Rogue Instant Poison V'),
('980025', '1851', '0', '0', '0', '15050', 'Vengeance Starter Rogue Deadly Poison IV'),

-- 58-60 only
('980025', '2514', '0', '0', '0', '15051', 'Vengeance Starter Arrow (Hunter, Warrior, Rogue Only)'),
('980025', '19286', '0', '0', '0', '15051', 'Vengeance Starter Bullet (Hunter, Warrior, Rogue Only)'),
('980025', '33822', '0', '0', '0', '15053', 'Vengeance Starter Fish (Hunter Only)'),
('980025', '12211', '0', '0', '0', '15053', 'Vengeance Starter Meat (Hunter Only)'),
('980025', '1165', '0', '0', '0', '15053', 'Vengeance Starter Cheese (Hunter Only)'),
('980025', '21236', '0', '0', '0', '15049', 'Vengeance Starter Bread'),
('980025', '24541', '0', '0', '0', '15053', 'Vengeance Starter Fungus (Hunter Only)'),
('980025', '12763', '0', '0', '0', '15053', 'Vengeance Starter Fruit (Hunter Only)'),
('980025', '1072', '0', '0', '0', '15052', 'Vengeance Starter Water (all classes except Warrior and Rogue)'),
('980025', '37460', '0', '0', '0', '15049', 'Vengeance Starter Bandage'),

-- any level
('980025', '5175', '0', '0', '0', '17', 'Earth Totem (Shaman Only)'),
('980025', '5176', '0', '0', '0', '17', 'Fire Totem (Shaman Only)'),
('980025', '5177', '0', '0', '0', '17', 'Water Totem (Shaman Only)'),
('980025', '5178', '0', '0', '0', '17', 'Air Totem (Shaman Only)'),
('980025', '28071', '0', '0', '0', '19', 'Grimoire of Anguish (Rank 1)'),
('980025', '16325', '0', '0', '0', '19', 'Grimoire of Blood Pact (Rank 5)'),
('980025', '23734', '0', '0', '0', '19', 'Grimoire of Cleave (Rank 1)'),
('980025', '16362', '0', '0', '0', '19', 'Grimoire of Consume Shadows (Rank 6)'),
('980025', '25900', '0', '0', '0', '19', 'Grimoire of Demonic Frenzy'),
('980025', '16383', '0', '0', '0', '19', 'Grimoire of Devour Magic (Rank 4)'),
('980025', '16330', '0', '0', '0', '19', 'Grimoire of Fire Shield (Rank 5)'),
('980025', '16320', '0', '0', '0', '19', 'Grimoire of Firebolt (Rank 7)'),
('980025', '23711', '0', '0', '0', '19', 'Grimoire of Intercept (Rank 1)'),
('980025', '16373', '0', '0', '0', '19', 'Grimoire of Lash of Pain (Rank 5)'),
('980025', '16380', '0', '0', '0', '19', 'Grimoire of Lesser Invisibility'),
('980025', '16390', '0', '0', '0', '19', 'Grimoire of Paranoia'),
('980025', '16331', '0', '0', '0', '19', 'Grimoire of Phase Shift'),
('980025', '16356', '0', '0', '0', '19', 'Grimoire of Sacrifice (Rank 6)'),
('980025', '16379', '0', '0', '0', '19', 'Grimoire of Seduction'),
('980025', '16378', '0', '0', '0', '19', 'Grimoire of Soothing Kiss (Rank 4)'),
('980025', '16389', '0', '0', '0', '19', 'Grimoire of Spell Lock (Rank 2)'),
('980025', '16365', '0', '0', '0', '19', 'Grimoire of Suffering (Rank 3)'),
('980025', '16387', '0', '0', '0', '19', 'Grimoire of Tainted Blood (Rank 4)'),
('980025', '16349', '0', '0', '0', '19', 'Grimoire of Torment (Rank 5)');
UPDATE `item_template` SET `SellPrice`=0, `BuyPrice`=0 WHERE `entry` IN (28071,16325,23734,16362,25900,16383,16330,16320,23711,16373,16380,16390,16331,16356,16379,16378,16389,16365,16387,16349);

-- Avatar of the Elements
REPLACE INTO `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`, `ExtendedCost`, `condition_id`, `comments`) VALUES
(980010, 31371, 0, 0, 0, 0, NULL),
(980010, 31370, 0, 0, 0, 0, NULL),
(980010, 31369, 0, 0, 0, 0, NULL),
(980010, 24093, 0, 0, 0, 0, NULL),
(980010, 31398, 0, 0, 0, 0, NULL),
(980010, 29486, 0, 0, 0, 0, NULL),
(980010, 29187, 0, 0, 0, 0, NULL),
(980010, 29198, 0, 0, 0, 0, NULL),
(980010, 31368, 0, 0, 0, 0, NULL),
(980010, 31367, 0, 0, 0, 0, NULL),
(980010, 31364, 0, 0, 0, 0, NULL),
(980010, 31399, 0, 0, 0, 0, NULL),
(980010, 24095, 0, 0, 0, 0, NULL),
(980010, 29487, 0, 0, 0, 0, NULL),
(980010, 29194, 0, 0, 0, 0, NULL),
(980010, 31746, 0, 0, 0, 0, NULL),
(980010, 31341, 0, 0, 0, 0, NULL),
(980010, 24092, 0, 0, 0, 0, NULL),
(980010, 30839, 0, 0, 0, 0, NULL),
(980010, 30838, 0, 0, 0, 0, NULL),
(980010, 30837, 0, 0, 0, 0, NULL),
(980010, 30764, 0, 0, 0, 0, NULL),
(980010, 30763, 0, 0, 0, 0, NULL),
(980010, 30762, 0, 0, 0, 0, NULL),
(980010, 30761, 0, 0, 0, 0, NULL),
(980010, 31074, 0, 0, 0, 0, NULL),
(980010, 31936, 0, 0, 0, 0, NULL),
(980010, 29485, 0, 0, 0, 0, NULL),
(980010, 31113, 0, 0, 0, 0, NULL),
(980010, 30831, 0, 0, 0, 0, NULL),
(980010, 30825, 0, 0, 0, 0, NULL),
(980010, 24098, 0, 0, 0, 0, NULL),
(980010, 23512, 0, 0, 0, 0, NULL),
(980010, 23511, 0, 0, 0, 0, NULL),
(980010, 23510, 0, 0, 0, 0, NULL),
(980010, 23509, 0, 0, 0, 0, NULL),
(980010, 21868, 0, 0, 0, 0, NULL),
(980010, 21867, 0, 0, 0, 0, NULL),
(980010, 21866, 0, 0, 0, 0, NULL),
(980010, 21865, 0, 0, 0, 0, NULL),
(980010, 21864, 0, 0, 0, 0, NULL),
(980010, 21863, 0, 0, 0, 0, NULL),
(980010, 29497, 0, 0, 0, 0, NULL),
(980010, 29496, 0, 0, 0, 0, NULL),
(980010, 29495, 0, 0, 0, 0, NULL),
(980010, 29488, 0, 0, 0, 0, NULL),
(980010, 35435, 0, 0, 0, 0, NULL),
(980010, 30845, 0, 0, 0, 0, NULL),
(980010, 29199, 0, 0, 0, 0, NULL),
(980010, 29197, 0, 0, 0, 0, NULL),
(980010, 29196, 0, 0, 0, 0, NULL),
(980010, 29195, 0, 0, 0, 0, NULL),
(980010, 33867, 0, 0, 0, 0, NULL),
(980010, 13513, 0, 0, 0, 0, NULL),
(980010, 33208, 0, 0, 0, 0, NULL),
(980010, 9088, 0, 0, 0, 0, 'Gift of Arthas'),
(980010, 22841, 0, 0, 0, 0, 'Major Fire Protection Potion'),
(980010, 22842, 0, 0, 0, 0, 'Major Frost Protection Potion'),
(980010, 22847, 0, 0, 0, 0, 'Major Holy Protection Potion'),
(980010, 22844, 0, 0, 0, 0, 'Major Nature Protection Potion'),
(980010, 22846, 0, 0, 0, 0, 'Major Shadow Protection Potion'),
(980010, 22845, 0, 0, 0, 0, 'Major Arcane Protection Potion'),
(980010, 13461, 0, 0, 0, 0, 'Greater Arcane Protection Potion'),
(980010, 13467, 0, 0, 0, 0, 'Greater Fire Protection Potion'),
(980010, 13456, 0, 0, 0, 0, 'Greater Frost Protection Potion'),
(980010, 13458, 0, 0, 0, 0, 'Greater Nature Protection Potion'),
(980010, 13459, 0, 0, 0, 0, 'Greater Shadow Protection Potion'),
(980010, 22682, 0, 0, 0, 0, 'Frozen Rune'),
(980010, 32839, 0, 0, 0, 0, 'Cauldron of Major Arcane Protection'),
(980010, 32849, 0, 0, 0, 0, 'Cauldron of Major Fire Protection'),
(980010, 32850, 0, 0, 0, 0, 'Cauldron of Major Frost Protection'),
(980010, 32851, 0, 0, 0, 0, 'Cauldron of Major Nature Protection'),
(980010, 32852, 0, 0, 0, 0, 'Cauldron of Major Shadow Protection');

REPLACE INTO `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`, `ExtendedCost`, `condition_id`, `comments`) VALUES
(185000, 29275, 0, 0, 0, 0, NULL),
(185000, 29924, 0, 0, 0, 0, NULL),
(185000, 29948, 0, 0, 0, 0, NULL),
(185000, 29962, 0, 0, 0, 0, NULL),
(185000, 29996, 0, 0, 0, 0, NULL),
(185000, 30082, 0, 0, 0, 0, NULL),
(185000, 30103, 0, 0, 0, 0, NULL),
(185000, 32944, 0, 0, 0, 0, NULL),
(185001, 28773, 0, 0, 0, 0, NULL),
(185001, 28774, 0, 0, 0, 0, NULL),
(185001, 28794, 0, 0, 0, 0, NULL),
(185001, 29993, 0, 0, 0, 0, NULL),
(185001, 30090, 0, 0, 0, 0, NULL),
(185002, 29949, 0, 0, 0, 0, NULL),
(185002, 30105, 0, 0, 0, 0, NULL),
(185002, 32756, 0, 0, 0, 0, NULL),
(185003, 28783, 0, 0, 0, 0, NULL),
(185003, 29982, 0, 0, 0, 0, NULL),
(185003, 30080, 0, 0, 0, 0, NULL),
(185004, 29267, 0, 0, 0, 0, NULL),
(185004, 29268, 0, 0, 0, 0, NULL),
(185005, 29269, 0, 0, 0, 0, NULL),
(185005, 29270, 0, 0, 0, 0, NULL),
(185005, 29271, 0, 0, 0, 0, NULL),
(185005, 29272, 0, 0, 0, 0, NULL),
(185005, 29273, 0, 0, 0, 0, NULL),
(185005, 29274, 0, 0, 0, 0, NULL),
(185005, 29923, 0, 0, 0, 0, NULL),
(185005, 30049, 0, 0, 0, 0, NULL),
(185006, 29368, 0, 0, 0, 0, NULL),
(185006, 29374, 0, 0, 0, 0, NULL),
(185006, 29381, 0, 0, 0, 0, NULL),
(185006, 30008, 0, 0, 0, 0, NULL),
(185006, 30015, 0, 0, 0, 0, NULL),
(185006, 30017, 0, 0, 0, 0, NULL),
(185006, 30018, 0, 0, 0, 0, NULL),
(185006, 30022, 0, 0, 0, 0, NULL),
(185006, 30059, 0, 0, 0, 0, NULL),
(185007, 24258, 0, 0, 0, 0, NULL),
(185007, 24259, 0, 0, 0, 0, NULL),
(185007, 28379, 0, 0, 0, 0, NULL),
(185007, 28380, 0, 0, 0, 0, NULL),
(185007, 28660, 0, 0, 0, 0, NULL),
(185007, 28672, 0, 0, 0, 0, NULL),
(185007, 28777, 0, 0, 0, 0, NULL),
(185007, 29369, 0, 0, 0, 0, NULL),
(185007, 29375, 0, 0, 0, 0, NULL),
(185007, 29382, 0, 0, 0, 0, NULL),
(185007, 29989, 0, 0, 0, 0, NULL),
(185007, 29992, 0, 0, 0, 0, NULL),
(185007, 29994, 0, 0, 0, 0, NULL),
(185007, 30098, 0, 0, 0, 0, NULL),
(185007, 30735, 0, 0, 0, 0, NULL),
(185008, 28790, 0, 0, 0, 0, NULL),
(185008, 28791, 0, 0, 0, 0, NULL),
(185008, 28793, 0, 0, 0, 0, NULL),
(185008, 29279, 0, 0, 0, 0, NULL),
(185008, 29283, 0, 0, 0, 0, NULL),
(185008, 29287, 0, 0, 0, 0, NULL),
(185008, 29290, 0, 0, 0, 0, NULL),
(185008, 29367, 0, 0, 0, 0, NULL),
(185008, 29373, 0, 0, 0, 0, NULL),
(185008, 29379, 0, 0, 0, 0, NULL),
(185008, 29920, 0, 0, 0, 0, NULL),
(185008, 29922, 0, 0, 0, 0, NULL),
(185008, 29997, 0, 0, 0, 0, NULL),
(185008, 30052, 0, 0, 0, 0, NULL),
(185008, 30061, 0, 0, 0, 0, NULL),
(185008, 30109, 0, 0, 0, 0, NULL),
(185008, 30110, 0, 0, 0, 0, NULL),
(185008, 33054, 0, 0, 0, 0, NULL),
(185008, 33055, 0, 0, 0, 0, NULL),
(185009, 27683, 0, 0, 0, 0, NULL),
(185009, 27770, 0, 0, 0, 0, NULL),
(185009, 27896, 0, 0, 0, 0, NULL),
(185009, 28190, 0, 0, 0, 0, NULL),
(185009, 28234, 0, 0, 0, 0, NULL),
(185009, 28235, 0, 0, 0, 0, NULL),
(185009, 28236, 0, 0, 0, 0, NULL),
(185009, 28237, 0, 0, 0, 0, NULL),
(185009, 28238, 0, 0, 0, 0, NULL),
(185009, 28239, 0, 0, 0, 0, NULL),
(185009, 28240, 0, 0, 0, 0, NULL),
(185009, 28241, 0, 0, 0, 0, NULL),
(185009, 28242, 0, 0, 0, 0, NULL),
(185009, 28243, 0, 0, 0, 0, NULL),
(185009, 28288, 0, 0, 0, 0, NULL),
(185009, 28370, 0, 0, 0, 0, NULL),
(185009, 28785, 0, 0, 0, 0, NULL),
(185009, 28789, 0, 0, 0, 0, NULL),
(185009, 28823, 0, 0, 0, 0, NULL),
(185009, 28830, 0, 0, 0, 0, NULL),
(185009, 29370, 0, 0, 0, 0, NULL),
(185009, 29376, 0, 0, 0, 0, NULL),
(185009, 29383, 0, 0, 0, 0, NULL),
(185009, 30343, 0, 0, 0, 0, NULL),
(185009, 30344, 0, 0, 0, 0, NULL),
(185009, 30345, 0, 0, 0, 0, NULL),
(185009, 30346, 0, 0, 0, 0, NULL),
(185009, 30348, 0, 0, 0, 0, NULL),
(185009, 30349, 0, 0, 0, 0, NULL),
(185009, 30350, 0, 0, 0, 0, NULL),
(185009, 30351, 0, 0, 0, 0, NULL),
(185009, 30619, 0, 0, 0, 0, NULL),
(185009, 30620, 0, 0, 0, 0, NULL),
(185009, 30626, 0, 0, 0, 0, NULL),
(185009, 30627, 0, 0, 0, 0, NULL),
(185009, 30720, 0, 0, 0, 0, NULL),
(185010, 28803, 0, 0, 0, 0, NULL),
(185010, 29983, 0, 0, 0, 0, NULL),
(185010, 29986, 0, 0, 0, 0, NULL),
(185010, 29990, 0, 0, 0, 0, NULL),
(185010, 30048, 0, 0, 0, 0, NULL),
(185010, 32084, 0, 0, 0, 0, NULL),
(185010, 32085, 0, 0, 0, 0, NULL),
(185010, 32086, 0, 0, 0, 0, NULL),
(185010, 32088, 0, 0, 0, 0, NULL),
(185010, 32089, 0, 0, 0, 0, NULL),
(185010, 32090, 0, 0, 0, 0, NULL),
(185011, 30053, 0, 0, 0, 0, NULL),
(185011, 30055, 0, 0, 0, 0, NULL),
(185011, 30079, 0, 0, 0, 0, NULL),
(185011, 30084, 0, 0, 0, 0, NULL),
(185011, 30085, 0, 0, 0, 0, NULL),
(185011, 30097, 0, 0, 0, 0, NULL),
(185011, 30111, 0, 0, 0, 0, NULL),
(185012, 29921, 0, 0, 0, 0, NULL),
(185012, 30054, 0, 0, 0, 0, NULL),
(185012, 30056, 0, 0, 0, 0, NULL),
(185012, 30065, 0, 0, 0, 0, NULL),
(185012, 30075, 0, 0, 0, 0, NULL),
(185012, 30101, 0, 0, 0, 0, NULL),
(185012, 30102, 0, 0, 0, 0, NULL),
(185012, 30107, 0, 0, 0, 0, NULL),
(185013, 28795, 0, 0, 0, 0, NULL),
(185013, 29918, 0, 0, 0, 0, NULL),
(185013, 29966, 0, 0, 0, 0, NULL),
(185013, 30026, 0, 0, 0, 0, NULL),
(185013, 30047, 0, 0, 0, 0, NULL),
(185013, 30057, 0, 0, 0, 0, NULL),
(185013, 30062, 0, 0, 0, 0, NULL),
(185013, 30091, 0, 0, 0, 0, NULL),
(185013, 32515, 0, 0, 0, 0, NULL),
(185013, 32516, 0, 0, 0, 0, NULL),
(185014, 28824, 0, 0, 0, 0, NULL),
(185014, 28827, 0, 0, 0, 0, NULL),
(185014, 29947, 0, 0, 0, 0, NULL),
(185014, 29976, 0, 0, 0, 0, NULL),
(185014, 29987, 0, 0, 0, 0, NULL),
(185014, 29998, 0, 0, 0, 0, NULL),
(185014, 30029, 0, 0, 0, 0, NULL),
(185014, 30112, 0, 0, 0, 0, NULL),
(185015, 28779, 0, 0, 0, 0, NULL),
(185015, 28828, 0, 0, 0, 0, NULL),
(185015, 29965, 0, 0, 0, 0, NULL),
(185015, 29984, 0, 0, 0, 0, NULL),
(185015, 30020, 0, 0, 0, 0, NULL),
(185015, 30030, 0, 0, 0, 0, NULL),
(185015, 30064, 0, 0, 0, 0, NULL),
(185015, 30068, 0, 0, 0, 0, NULL),
(185015, 30096, 0, 0, 0, 0, NULL),
(185015, 30106, 0, 0, 0, 0, NULL),
(185016, 29950, 0, 0, 0, 0, NULL),
(185016, 29972, 0, 0, 0, 0, NULL),
(185016, 29977, 0, 0, 0, 0, NULL),
(185016, 29985, 0, 0, 0, 0, NULL),
(185016, 29991, 0, 0, 0, 0, NULL),
(185016, 29995, 0, 0, 0, 0, NULL),
(185017, 29951, 0, 0, 0, 0, NULL),
(185017, 30027, 0, 0, 0, 0, NULL),
(185017, 30050, 0, 0, 0, 0, NULL),
(185017, 30060, 0, 0, 0, 0, NULL),
(185017, 30066, 0, 0, 0, 0, NULL),
(185017, 30067, 0, 0, 0, 0, NULL),
(185017, 30081, 0, 0, 0, 0, NULL),
(185017, 30092, 0, 0, 0, 0, NULL),
(185017, 30100, 0, 0, 0, 0, NULL),
(185017, 30104, 0, 0, 0, 0, NULL),
(185017, 32267, 0, 0, 0, 0, NULL),
(185018, 28782, 0, 0, 0, 0, NULL),
(185018, 29981, 0, 0, 0, 0, NULL),
(185018, 29988, 0, 0, 0, 0, NULL),
(185018, 30058, 0, 0, 0, 0, NULL),
(185018, 30108, 0, 0, 0, 0, NULL),
(185019, 30486, 0, 0, 0, 0, NULL),
(185019, 30487, 0, 0, 0, 0, NULL),
(185019, 30488, 0, 0, 0, 0, NULL),
(185019, 30489, 0, 0, 0, 0, NULL),
(185019, 30490, 0, 0, 0, 0, NULL),
(185019, 31960, 0, 0, 0, 0, NULL),
(185019, 31961, 0, 0, 0, 0, NULL),
(185019, 31962, 0, 0, 0, 0, NULL),
(185019, 31963, 0, 0, 0, 0, NULL),
(185019, 31964, 0, 0, 0, 0, NULL),
(185019, 31967, 0, 0, 0, 0, NULL),
(185019, 31968, 0, 0, 0, 0, NULL),
(185019, 31969, 0, 0, 0, 0, NULL),
(185019, 31971, 0, 0, 0, 0, NULL),
(185019, 31972, 0, 0, 0, 0, NULL),
(185019, 31973, 0, 0, 0, 0, NULL),
(185019, 31974, 0, 0, 0, 0, NULL),
(185019, 31975, 0, 0, 0, 0, NULL),
(185019, 31976, 0, 0, 0, 0, NULL),
(185019, 31977, 0, 0, 0, 0, NULL),
(185019, 31979, 0, 0, 0, 0, NULL),
(185019, 31980, 0, 0, 0, 0, NULL),
(185019, 31981, 0, 0, 0, 0, NULL),
(185019, 31982, 0, 0, 0, 0, NULL),
(185019, 31983, 0, 0, 0, 0, NULL),
(185019, 31987, 0, 0, 0, 0, NULL),
(185019, 31988, 0, 0, 0, 0, NULL),
(185019, 31989, 0, 0, 0, 0, NULL),
(185019, 31990, 0, 0, 0, 0, NULL),
(185019, 31991, 0, 0, 0, 0, NULL),
(185019, 31992, 0, 0, 0, 0, NULL),
(185019, 31993, 0, 0, 0, 0, NULL),
(185019, 31995, 0, 0, 0, 0, NULL),
(185019, 31996, 0, 0, 0, 0, NULL),
(185019, 31997, 0, 0, 0, 0, NULL),
(185019, 31998, 0, 0, 0, 0, NULL),
(185019, 31999, 0, 0, 0, 0, NULL),
(185019, 32000, 0, 0, 0, 0, NULL),
(185019, 32001, 0, 0, 0, 0, NULL),
(185019, 32002, 0, 0, 0, 0, NULL),
(185019, 32004, 0, 0, 0, 0, NULL),
(185019, 32005, 0, 0, 0, 0, NULL),
(185019, 32006, 0, 0, 0, 0, NULL),
(185019, 32007, 0, 0, 0, 0, NULL),
(185019, 32008, 0, 0, 0, 0, NULL),
(185019, 32009, 0, 0, 0, 0, NULL),
(185019, 32010, 0, 0, 0, 0, NULL),
(185019, 32011, 0, 0, 0, 0, NULL),
(185019, 32012, 0, 0, 0, 0, NULL),
(185019, 32013, 0, 0, 0, 0, NULL),
(185019, 32015, 0, 0, 0, 0, NULL),
(185019, 32016, 0, 0, 0, 0, NULL),
(185019, 32017, 0, 0, 0, 0, NULL),
(185019, 32018, 0, 0, 0, 0, NULL),
(185019, 32019, 0, 0, 0, 0, NULL),
(185019, 32020, 0, 0, 0, 0, NULL),
(185019, 32021, 0, 0, 0, 0, NULL),
(185019, 32022, 0, 0, 0, 0, NULL),
(185019, 32023, 0, 0, 0, 0, NULL),
(185019, 32024, 0, 0, 0, 0, NULL),
(185019, 32029, 0, 0, 0, 0, NULL),
(185019, 32030, 0, 0, 0, 0, NULL),
(185019, 32031, 0, 0, 0, 0, NULL),
(185019, 32032, 0, 0, 0, 0, NULL),
(185019, 32033, 0, 0, 0, 0, NULL),
(185019, 32034, 0, 0, 0, 0, NULL),
(185019, 32035, 0, 0, 0, 0, NULL),
(185019, 32036, 0, 0, 0, 0, NULL),
(185019, 32037, 0, 0, 0, 0, NULL),
(185019, 32038, 0, 0, 0, 0, NULL),
(185019, 32039, 0, 0, 0, 0, NULL),
(185019, 32040, 0, 0, 0, 0, NULL),
(185019, 32041, 0, 0, 0, 0, NULL),
(185019, 32042, 0, 0, 0, 0, NULL),
(185019, 32043, 0, 0, 0, 0, NULL),
(185019, 32047, 0, 0, 0, 0, NULL),
(185019, 32048, 0, 0, 0, 0, NULL),
(185019, 32049, 0, 0, 0, 0, NULL),
(185019, 32050, 0, 0, 0, 0, NULL),
(185019, 32051, 0, 0, 0, 0, NULL),
(185019, 32056, 0, 0, 0, 0, NULL),
(185019, 32057, 0, 0, 0, 0, NULL),
(185019, 32058, 0, 0, 0, 0, NULL),
(185019, 32059, 0, 0, 0, 0, NULL),
(185019, 32060, 0, 0, 0, 0, NULL),
(185020, 32785, 0, 0, 0, 0, NULL),
(185020, 32786, 0, 0, 0, 0, NULL),
(185020, 32787, 0, 0, 0, 0, NULL),
(185020, 32788, 0, 0, 0, 0, NULL),
(185020, 32789, 0, 0, 0, 0, NULL),
(185020, 32790, 0, 0, 0, 0, NULL),
(185020, 32791, 0, 0, 0, 0, NULL),
(185020, 32792, 0, 0, 0, 0, NULL),
(185020, 32793, 0, 0, 0, 0, NULL),
(185020, 32794, 0, 0, 0, 0, NULL),
(185020, 32795, 0, 0, 0, 0, NULL),
(185020, 32796, 0, 0, 0, 0, NULL),
(185020, 32797, 0, 0, 0, 0, NULL),
(185020, 32798, 0, 0, 0, 0, NULL),
(185020, 32799, 0, 0, 0, 0, NULL),
(185020, 32800, 0, 0, 0, 0, NULL),
(185020, 32801, 0, 0, 0, 0, NULL),
(185020, 32802, 0, 0, 0, 0, NULL),
(185020, 32803, 0, 0, 0, 0, NULL),
(185020, 32804, 0, 0, 0, 0, NULL),
(185020, 32805, 0, 0, 0, 0, NULL),
(185020, 32806, 0, 0, 0, 0, NULL),
(185020, 32807, 0, 0, 0, 0, NULL),
(185020, 32808, 0, 0, 0, 0, NULL),
(185020, 32809, 0, 0, 0, 0, NULL),
(185020, 32810, 0, 0, 0, 0, NULL),
(185020, 32811, 0, 0, 0, 0, NULL),
(185020, 32812, 0, 0, 0, 0, NULL),
(185020, 32813, 0, 0, 0, 0, NULL),
(185020, 32814, 0, 0, 0, 0, NULL),
(185020, 32816, 0, 0, 0, 0, NULL),
(185020, 32817, 0, 0, 0, 0, NULL),
(185020, 32818, 0, 0, 0, 0, NULL),
(185020, 32819, 0, 0, 0, 0, NULL),
(185020, 32820, 0, 0, 0, 0, NULL),
(185020, 32821, 0, 0, 0, 0, NULL),
(185020, 32979, 0, 0, 0, 0, NULL),
(185020, 32980, 0, 0, 0, 0, NULL),
(185020, 32981, 0, 0, 0, 0, NULL),
(185020, 32988, 0, 0, 0, 0, NULL),
(185020, 32989, 0, 0, 0, 0, NULL),
(185020, 32990, 0, 0, 0, 0, NULL),
(185020, 32997, 0, 0, 0, 0, NULL),
(185020, 32998, 0, 0, 0, 0, NULL),
(185020, 32999, 0, 0, 0, 0, NULL),
(185020, 33056, 0, 0, 0, 0, NULL),
(185020, 33057, 0, 0, 0, 0, NULL),
(185020, 33064, 0, 0, 0, 0, NULL),
(185020, 33065, 0, 0, 0, 0, NULL),
(185020, 33066, 0, 0, 0, 0, NULL),
(185020, 33067, 0, 0, 0, 0, NULL),
(185020, 33068, 0, 0, 0, 0, NULL),
(185021, 31958, 0, 0, 0, 0, NULL),
(185021, 31959, 0, 0, 0, 0, NULL),
(185021, 31965, 0, 0, 0, 0, NULL),
(185021, 31966, 0, 0, 0, 0, NULL),
(185021, 31978, 0, 0, 0, 0, NULL),
(185021, 31984, 0, 0, 0, 0, NULL),
(185021, 31985, 0, 0, 0, 0, NULL),
(185021, 31986, 0, 0, 0, 0, NULL),
(185021, 32003, 0, 0, 0, 0, NULL),
(185021, 32014, 0, 0, 0, 0, NULL),
(185021, 32025, 0, 0, 0, 0, NULL),
(185021, 32026, 0, 0, 0, 0, NULL),
(185021, 32027, 0, 0, 0, 0, NULL),
(185021, 32028, 0, 0, 0, 0, NULL),
(185021, 32044, 0, 0, 0, 0, NULL),
(185021, 32045, 0, 0, 0, 0, NULL),
(185021, 32046, 0, 0, 0, 0, NULL),
(185021, 32052, 0, 0, 0, 0, NULL),
(185021, 32053, 0, 0, 0, 0, NULL),
(185021, 32054, 0, 0, 0, 0, NULL),
(185021, 32055, 0, 0, 0, 0, NULL),
(185021, 32961, 0, 0, 0, 0, NULL),
(185021, 32962, 0, 0, 0, 0, NULL),
(185021, 32963, 0, 0, 0, 0, NULL),
(185021, 32964, 0, 0, 0, 0, NULL),
(185021, 33076, 0, 0, 0, 0, NULL),
(185021, 33077, 0, 0, 0, 0, NULL),
(185021, 33078, 0, 0, 0, 0, NULL),
(185021, 33309, 0, 0, 0, 0, NULL),
(185021, 33313, 0, 0, 0, 0, NULL),
(185021, 33937, 0, 0, 0, 0, NULL),
(185021, 33940, 0, 0, 0, 0, NULL),
(185021, 33943, 0, 0, 0, 0, NULL),
(185021, 33946, 0, 0, 0, 0, NULL),
(185021, 33949, 0, 0, 0, 0, NULL),
(185021, 33952, 0, 0, 0, 0, NULL),
(185022, 25890, 0, 0, 0, 0, NULL),
(185022, 25893, 0, 0, 0, 0, NULL),
(185022, 25894, 0, 0, 0, 0, NULL),
(185022, 25895, 0, 0, 0, 0, NULL),
(185022, 25896, 0, 0, 0, 0, NULL),
(185022, 25897, 0, 0, 0, 0, NULL),
(185022, 25898, 0, 0, 0, 0, NULL),
(185022, 25899, 0, 0, 0, 0, NULL),
(185022, 25901, 0, 0, 0, 0, NULL),
(185022, 28557, 0, 0, 0, 0, NULL),
(185022, 32409, 0, 0, 0, 0, NULL),
(185022, 32410, 0, 0, 0, 0, NULL),
(185022, 32640, 0, 0, 0, 0, NULL),
(185022, 32641, 0, 0, 0, 0, NULL),
(185022, 34220, 0, 0, 0, 0, NULL),
(185023, 24027, 0, 0, 0, 0, NULL),
(185023, 24028, 0, 0, 0, 0, NULL),
(185023, 24029, 0, 0, 0, 0, NULL),
(185023, 24030, 0, 0, 0, 0, NULL),
(185023, 24031, 0, 0, 0, 0, NULL),
(185023, 24032, 0, 0, 0, 0, NULL),
(185023, 24036, 0, 0, 0, 0, NULL),
(185024, 24047, 0, 0, 0, 0, NULL),
(185024, 24048, 0, 0, 0, 0, NULL),
(185024, 24050, 0, 0, 0, 0, NULL),
(185024, 24051, 0, 0, 0, 0, NULL),
(185024, 24053, 0, 0, 0, 0, NULL),
(185024, 31861, 0, 0, 0, 0, NULL),
(185025, 24033, 0, 0, 0, 0, NULL),
(185025, 24035, 0, 0, 0, 0, NULL),
(185025, 24037, 0, 0, 0, 0, NULL),
(185025, 24039, 0, 0, 0, 0, NULL),
(185026, 24058, 0, 0, 0, 0, NULL),
(185026, 24059, 0, 0, 0, 0, NULL),
(185026, 24060, 0, 0, 0, 0, NULL),
(185026, 24061, 0, 0, 0, 0, NULL),
(185026, 31867, 0, 0, 0, 0, NULL),
(185026, 31868, 0, 0, 0, 0, NULL),
(185026, 32637, 0, 0, 0, 0, NULL),
(185026, 32638, 0, 0, 0, 0, NULL),
(185027, 24065, 0, 0, 0, 0, NULL),
(185027, 24066, 0, 0, 0, 0, NULL),
(185027, 24067, 0, 0, 0, 0, NULL),
(185027, 32635, 0, 0, 0, 0, NULL),
(185027, 32639, 0, 0, 0, 0, NULL),
(185027, 33782, 0, 0, 0, 0, NULL),
(185028, 24054, 0, 0, 0, 0, NULL),
(185028, 24055, 0, 0, 0, 0, NULL),
(185028, 24056, 0, 0, 0, 0, NULL),
(185028, 24057, 0, 0, 0, 0, NULL),
(185028, 31863, 0, 0, 0, 0, NULL),
(185028, 31865, 0, 0, 0, 0, NULL),
(185028, 32634, 0, 0, 0, 0, NULL),
(185028, 32636, 0, 0, 0, 0, NULL),
(185029, 16331, 0, 0, 0, 0, NULL),
(185029, 22179, 0, 0, 0, 0, NULL),
(185029, 22180, 0, 0, 0, 0, NULL),
(185029, 22181, 0, 0, 0, 0, NULL),
(185030, 22182, 0, 0, 0, 0, NULL),
(185030, 22184, 0, 0, 0, 0, NULL),
(185030, 22185, 0, 0, 0, 0, NULL),
(185030, 28068, 0, 0, 0, 0, NULL),
(185031, 16379, 0, 0, 0, 0, NULL),
(185031, 16380, 0, 0, 0, 0, NULL),
(185031, 22186, 0, 0, 0, 0, NULL),
(185031, 22187, 0, 0, 0, 0, NULL),
(185032, 16389, 0, 0, 0, 0, NULL),
(185032, 16390, 0, 0, 0, 0, NULL),
(185032, 22189, 0, 0, 0, 0, NULL),
(185032, 22190, 0, 0, 0, 0, NULL),
(185033, 23731, 0, 0, 0, 0, NULL),
(185033, 23755, 0, 0, 0, 0, NULL),
(185033, 25469, 0, 0, 0, 0, NULL),
(185033, 25900, 0, 0, 0, 0, NULL),
(185033, 28073, 0, 0, 0, 0, NULL),
(185034, 27886, 0, 0, 0, 0, NULL),
(185034, 29388, 0, 0, 0, 0, NULL),
(185034, 29389, 0, 0, 0, 0, NULL),
(185034, 29390, 0, 0, 0, 0, NULL),
(185034, 30051, 0, 0, 0, 0, NULL),
(185034, 30063, 0, 0, 0, 0, NULL),
(980007, 3776, 0, 0, 0, 0, NULL),
(980007, 9186, 0, 0, 0, 0, NULL),
(980007, 21927, 0, 0, 0, 0, NULL),
(980007, 22054, 0, 0, 0, 0, NULL),
(980007, 22055, 0, 0, 0, 0, NULL),
(980008, 29466, 0, 0, 0, 0, NULL),
(980008, 29467, 0, 0, 0, 0, NULL),
(980008, 29468, 0, 0, 0, 0, NULL),
(980008, 29469, 0, 0, 0, 0, NULL),
(980008, 29471, 0, 0, 0, 0, NULL),
(980008, 29472, 0, 0, 0, 0, NULL),
(980008, 34129, 0, 0, 0, 0, NULL),
(980008, 35906, 0, 0, 0, 0, NULL),
(980011, 5976, 0, 0, 0, 0, NULL),
(980016, 30113, 0, 0, 0, 0, NULL),
(980016, 30114, 0, 0, 0, 0, NULL),
(980016, 30115, 0, 0, 0, 0, NULL),
(980016, 30116, 0, 0, 0, 0, NULL),
(980016, 30117, 0, 0, 0, 0, NULL),
(980016, 30118, 0, 0, 0, 0, NULL),
(980016, 30119, 0, 0, 0, 0, NULL),
(980016, 30120, 0, 0, 0, 0, NULL),
(980016, 30121, 0, 0, 0, 0, NULL),
(980016, 30122, 0, 0, 0, 0, NULL),
(980016, 30123, 0, 0, 0, 0, NULL),
(980016, 30124, 0, 0, 0, 0, NULL),
(980016, 30125, 0, 0, 0, 0, NULL),
(980016, 30126, 0, 0, 0, 0, NULL),
(980016, 30127, 0, 0, 0, 0, NULL),
(980016, 30129, 0, 0, 0, 0, NULL),
(980016, 30130, 0, 0, 0, 0, NULL),
(980016, 30131, 0, 0, 0, 0, NULL),
(980016, 30132, 0, 0, 0, 0, NULL),
(980016, 30133, 0, 0, 0, 0, NULL),
(980016, 30134, 0, 0, 0, 0, NULL),
(980016, 30135, 0, 0, 0, 0, NULL),
(980016, 30136, 0, 0, 0, 0, NULL),
(980016, 30137, 0, 0, 0, 0, NULL),
(980016, 30138, 0, 0, 0, 0, NULL),
(980016, 30139, 0, 0, 0, 0, NULL),
(980016, 30140, 0, 0, 0, 0, NULL),
(980016, 30141, 0, 0, 0, 0, NULL),
(980016, 30142, 0, 0, 0, 0, NULL),
(980016, 30143, 0, 0, 0, 0, NULL),
(980016, 30144, 0, 0, 0, 0, NULL),
(980016, 30145, 0, 0, 0, 0, NULL),
(980016, 30146, 0, 0, 0, 0, NULL),
(980016, 30148, 0, 0, 0, 0, NULL),
(980016, 30149, 0, 0, 0, 0, NULL),
(980016, 30150, 0, 0, 0, 0, NULL),
(980016, 30151, 0, 0, 0, 0, NULL),
(980016, 30152, 0, 0, 0, 0, NULL),
(980016, 30153, 0, 0, 0, 0, NULL),
(980016, 30154, 0, 0, 0, 0, NULL),
(980016, 30159, 0, 0, 0, 0, NULL),
(980016, 30160, 0, 0, 0, 0, NULL),
(980016, 30161, 0, 0, 0, 0, NULL),
(980016, 30162, 0, 0, 0, 0, NULL),
(980016, 30163, 0, 0, 0, 0, NULL),
(980016, 30164, 0, 0, 0, 0, NULL),
(980016, 30165, 0, 0, 0, 0, NULL),
(980016, 30166, 0, 0, 0, 0, NULL),
(980016, 30167, 0, 0, 0, 0, NULL),
(980016, 30168, 0, 0, 0, 0, NULL),
(980016, 30169, 0, 0, 0, 0, NULL),
(980016, 30170, 0, 0, 0, 0, NULL),
(980016, 30171, 0, 0, 0, 0, NULL),
(980016, 30172, 0, 0, 0, 0, NULL),
(980016, 30173, 0, 0, 0, 0, NULL),
(980016, 30185, 0, 0, 0, 0, NULL),
(980016, 30189, 0, 0, 0, 0, NULL),
(980016, 30190, 0, 0, 0, 0, NULL),
(980016, 30192, 0, 0, 0, 0, NULL),
(980016, 30194, 0, 0, 0, 0, NULL),
(980016, 30196, 0, 0, 0, 0, NULL),
(980016, 30205, 0, 0, 0, 0, NULL),
(980016, 30206, 0, 0, 0, 0, NULL),
(980016, 30207, 0, 0, 0, 0, NULL),
(980016, 30210, 0, 0, 0, 0, NULL),
(980016, 30211, 0, 0, 0, 0, NULL),
(980016, 30212, 0, 0, 0, 0, NULL),
(980016, 30213, 0, 0, 0, 0, NULL),
(980016, 30214, 0, 0, 0, 0, NULL),
(980016, 30215, 0, 0, 0, 0, NULL),
(980016, 30216, 0, 0, 0, 0, NULL),
(980016, 30217, 0, 0, 0, 0, NULL),
(980016, 30219, 0, 0, 0, 0, NULL),
(980016, 30220, 0, 0, 0, 0, NULL),
(980016, 30221, 0, 0, 0, 0, NULL),
(980016, 30222, 0, 0, 0, 0, NULL),
(980016, 30223, 0, 0, 0, 0, NULL),
(980016, 30228, 0, 0, 0, 0, NULL),
(980016, 30229, 0, 0, 0, 0, NULL),
(980016, 30230, 0, 0, 0, 0, NULL),
(980016, 30231, 0, 0, 0, 0, NULL),
(980016, 30232, 0, 0, 0, 0, NULL),
(980016, 30233, 0, 0, 0, 0, NULL),
(980016, 30234, 0, 0, 0, 0, NULL),
(980016, 30235, 0, 0, 0, 0, NULL),
-- Master Provisioner - Basic Necessities
(185035, 21967, 0, 0, 0, 15035, 'Magic Talent Switch Gemstone'),
(185035, 20520, 0, 0, 0, 0, 'Dark Rune'),
(185035, 17029, 0, 0, 0, 0, 'Sacred Candle'),
(185035, 17030, 0, 0, 0, 0, 'Ankh'),
(185035, 17033, 0, 0, 0, 0, 'Symbol of Divinity'),
(185035, 21177, 0, 0, 0, 0, 'Symbol of Kings'),
(185035, 22147, 0, 0, 0, 0, 'Flintweed Seed'),
(185035, 22148, 0, 0, 0, 0, 'Wild Quillvine'),
(185035, 31737, 0, 0, 0, 0, 'Timeless Arrow'),
(185035, 31735, 0, 0, 0, 0, 'Timeless Shell'),
(185035, 5140, 0, 0, 0, 0, 'Flash Powder'),
(185035, 5565, 0, 0, 0, 0, 'Infernal Stone'),
(185035, 6265, 0, 0, 0, 0, 'Soul Shard'),
(185035, 17020, 0, 0, 0, 0, 'Arcane Powder'),
(185035, 17056, 0, 0, 0, 0, 'Light Feather'),
(185035, 17057, 0, 0, 0, 0, 'Shiny Fish Scales'),
(185035, 17058, 0, 0, 0, 0, 'Fish Oil'),
(185035, 21991, 0, 0, 0, 0, 'Heavy Netherweave Bandage'),
(185035, 22829, 0, 0, 0, 0, 'Super Healing Potion'),
(185035, 22832, 0, 0, 0, 0, 'Super Mana Potion'),
(185035, 29450, 0, 0, 0, 0, 'Telaari Grapes'),
(185035, 32453, 0, 0, 0, 0, 'Star''s Tears'),
(185035, 34581, 0, 0, 0, 0, 'Mysterious Arrow'),
(185035, 34582, 0, 0, 0, 0, 'Mysterious Shell'),
-- Master Provisioner - Stat Buff Food
(185036, 33052, 0, 0, 0, 0, 'Fisherman''s Feast'),
(185036, 33872, 0, 0, 0, 0, 'Spicy Hot Talbuk'),
(185036, 27658, 0, 0, 0, 0, 'Roasted Clefthoof'),
(185036, 27655, 0, 0, 0, 0, 'Ravager Dog'),
(185036, 27657, 0, 0, 0, 0, 'Blackened Basilisk'),
(185036, 27666, 0, 0, 0, 0, 'Golden Fish Sticks'),
(185036, 27664, 0, 0, 0, 0, 'Grilled Mudfish'),
(185036, 33874, 0, 0, 0, 0, 'Kibler''s Bits'),
(185036, 33825, 0, 0, 0, 0, 'Skullfish Soup'),
(185036, 27663, 0, 0, 0, 0, 'Blackened Sporefish'),
(185036, 33866, 0, 0, 0, 0, 'Stormchops'),
(185036, 22237, 0, 0, 0, 0, 'Dark Desire'),
(185036, 22238, 0, 0, 0, 0, 'Very Berry Cream'),
(185036, 22236, 0, 0, 0, 0, 'Buttermilk Delight'),
(185036, 22239, 0, 0, 0, 0, 'Sweet Surprise'),
-- Master Provisioner - Flasks, Elixirs, & Potions
(185037, 13511, 0, 0, 0, 0, 'Flask of Distilled Wisdom'),
(185037, 22851, 0, 0, 0, 0, 'Flask of Fortification'),
(185037, 22853, 0, 0, 0, 0, 'Flask of Mighty Restoration'),
(185037, 22866, 0, 0, 0, 0, 'Flask of Pure Death'),
(185037, 22861, 0, 0, 0, 0, 'Flask of Blinding Light'),
(185037, 22854, 0, 0, 0, 0, 'Flask of Relentless Assault'),
(185037, 13506, 0, 0, 0, 0, 'Flask of Petrification'),
(185037, 28104, 0, 0, 0, 0, 'Elixir of Mastery'),
(185037, 22835, 0, 0, 0, 0, 'Elixir of Major Shadow Power'),
(185037, 22827, 0, 0, 0, 0, 'Elixir of Major Frost Power'),
(185037, 22833, 0, 0, 0, 0, 'Elixir of Major Firepower'),
(185037, 22824, 0, 0, 0, 0, 'Elixir of Major Strength'),
(185037, 9224, 0, 0, 0, 0, 'Elixir of Demonslaying'),
(185037, 31679, 0, 0, 0, 0, 'Fel Strength Elixir'),
(185037, 28103, 0, 0, 0, 0, 'Adept''s Elixir'),
(185037, 28102, 0, 0, 0, 0, 'Onslaught Elixir'),
(185037, 13454, 0, 0, 0, 0, 'Greater Arcane Elixir'),
(185037, 22825, 0, 0, 0, 0, 'Elixir of Healing Power'),
(185037, 22831, 0, 0, 0, 0, 'Elixir of Major Agility'),
(185037, 22834, 0, 0, 0, 0, 'Elixir of Major Defense'),
(185037, 32062, 0, 0, 0, 0, 'Elixir of Major Fortitude'),
(185037, 32063, 0, 0, 0, 0, 'Earthen Elixir'),
(185037, 22848, 0, 0, 0, 0, 'Elixir of Empowerment'),
(185037, 22840, 0, 0, 0, 0, 'Elixir of Major Mageblood'),
(185037, 32067, 0, 0, 0, 0, 'Elixir of Draenic Wisdom'),
(185037, 31676, 0, 0, 0, 0, 'Fel Regeneration Potion'),
(185037, 31677, 0, 0, 0, 0, 'Fel Mana Potion'),
(185037, 34440, 0, 0, 0, 0, 'Mad Alchemist''s Potion'),
(185037, 22849, 0, 0, 0, 0, 'Ironshield Potion'),
(185037, 22837, 0, 0, 0, 0, 'Heroic Potion'),
(185037, 22839, 0, 0, 0, 0, 'Destruction Potion'),
(185037, 22838, 0, 0, 0, 0, 'Haste Potion'),
(185037, 22828, 0, 0, 0, 0, 'Insane Strength Potion'),
(185037, 22836, 0, 0, 0, 0, 'Major Dreamless Sleep Potion'),
(185037, 22826, 0, 0, 0, 0, 'Sneaking Potion'),
(185037, 22871, 0, 0, 0, 0, 'Shrouding Potion'),
(185037, 28100, 0, 0, 0, 0, 'Volatile Healing Potion'),
(185037, 28101, 0, 0, 0, 0, 'Unstable Mana Potion'),
(185037, 13442, 0, 0, 0, 0, 'Mighty Rage Potion'),
(185037, 9172, 0, 0, 0, 0, 'Invisibility Potion'),
(185037, 20008, 0, 0, 0, 0, 'Living Action Potion'),
(185037, 5634, 0, 0, 0, 0, 'Free Action Potion'),
(185037, 9030, 0, 0, 0, 0, 'Restorative Potion'),
(185037, 25539, 0, 0, 0, 0, 'Potion of Water Breathing'),
-- Master Provisioner - Equipment Enhancers, Gadgets, & Misc Consumables
(185038, 22522, 0, 0, 0, 0, 'Superior Wizard Oil'),
(185038, 22521, 0, 0, 0, 0, 'Superior Mana Oil'),
(185038, 23529, 0, 0, 0, 0, 'Adamantite Sharpening Stone'),
(185038, 28421, 0, 0, 0, 0, 'Adamantite Weightstone'),
(185038, 18262, 0, 0, 0, 0, 'Elemental Sharpening Stone'),
(185038, 20748, 0, 0, 0, 0, 'Brilliant Mana Oil'),
(185038, 20749, 0, 0, 0, 0, 'Brilliant Wizard Oil'),
(185038, 23123, 0, 0, 0, 0, 'Blessed Wizard Oil'),
(185038, 23122, 0, 0, 0, 0, 'Consecrated Sharpening Stone'),
(185038, 29529, 0, 0, 0, 0, 'Drums of Battle'),
(185038, 29532, 0, 0, 0, 0, 'Drums of Panic'),
(185038, 29531, 0, 0, 0, 0, 'Drums of Restoration'),
(185038, 29530, 0, 0, 0, 0, 'Drums of Speed'),
(185038, 29528, 0, 0, 0, 0, 'Drums of War'),
(185038, 25550, 0, 0, 0, 0, 'Redcap Toadstool'),
(185038, 8956, 0, 0, 0, 0, 'Oil of Immolation'),
(185038, 23559, 0, 0, 0, 0, 'Lesser Rune of Warding'),
(185038, 23575, 0, 0, 0, 0, 'Lesser Rune of Shielding'),
(185038, 23841, 0, 0, 0, 0, 'Gnomish Flame Turret'),
(185038, 23827, 0, 0, 0, 0, 'Super Sapper Charge'),
(185038, 23737, 0, 0, 0, 0, 'Adamantite Grenade'),
(185038, 32413, 0, 0, 0, 0, 'Frost Grenade'),
(185038, 23826, 0, 0, 0, 0, 'The Bigger One'),
(185038, 23819, 0, 0, 0, 0, 'Elemental Seaforium Charge'),
(185038, 22788, 0, 0, 0, 0, 'Flame Cap'),
(185038, 22797, 0, 0, 0, 0, 'Nightmare Seed'),
(185038, 22795, 0, 0, 0, 0, 'Fel Blossom'),
(185038, 22710, 0, 0, 0, 0, 'Bloodthistle'),
(185038, 25883, 0, 0, 0, 0, 'Dense Stone Statue'),
(185038, 4397, 0, 0, 0, 0, 'Gnomish Cloaking Device'),
-- Master Provisioner - Crafting Materials 
(185039, 22794, 0, 0, 0, 0, 'Fel Lotus'),
(185039, 22451, 0, 0, 0, 0, 'Primal Air'),
(185039, 22452, 0, 0, 0, 0, 'Primal Earth'),
(185039, 22457, 0, 0, 0, 0, 'Primal Mana'),
(185039, 21885, 0, 0, 0, 0, 'Primal Water'),
(185039, 21886, 0, 0, 0, 0, 'Primal Life'),
(185039, 21884, 0, 0, 0, 0, 'Primal Fire'),
(185039, 22456, 0, 0, 0, 0, 'Primal Shadow'),
(185039, 23571, 0, 0, 0, 0, 'Primal Might'),
(185039, 23572, 0, 0, 0, 0, 'Primal Nether'),
(185039, 30183, 0, 0, 0, 0, 'Nether Vortex'),
(185039, 32428, 0, 0, 0, 0, 'Heart of Darkness'),
(185039, 22462, 0, 0, 0, 0, 'Runed Adamantite Rod'),
(185039, 22450, 0, 0, 0, 0, 'Void Crystal'),
(185039, 22448, 0, 0, 0, 0, 'Large Prismatic Shard'),
(185039, 22449, 0, 0, 0, 0, 'Small Prismatic Shard'),
(185039, 22446, 0, 0, 0, 0, 'Greater Planar Essence'),
(185039, 22445, 0, 0, 0, 0, 'Arcane Dust'),
(185039, 16204, 0, 0, 0, 0, 'Illusion Dust'),
(185039, 23446, 0, 0, 0, 0, 'Adamantite Bar'),
(185039, 23573, 0, 0, 0, 0, 'Hardened Adamantite Bar'),
(185039, 23445, 0, 0, 0, 0, 'Fel Iron Bar'),
(185039, 23448, 0, 0, 0, 0, 'Felsteel Bar'),
(185039, 23449, 0, 0, 0, 0, 'Khorium Bar'),
(185039, 23447, 0, 0, 0, 0, 'Eternium Bar'),
(185039, 16203, 0, 0, 0, 0, 'Greater Eternal Essence'),
(185039, 14344, 0, 0, 0, 0, 'Large Brilliant Shard'),
(185039, 12811, 0, 0, 0, 0, 'Righteous Orb'),
(185039, 13468, 0, 0, 0, 0, 'Black Lotus'),
(185039, 5956, 0, 0, 0, 0, 'Blacksmith Hammer'),
(185039, 6219, 0, 0, 0, 0, 'Arclight Spanner'),
(185039, 4470, 0, 0, 0, 0, 'Simple Wood'),
(185039, 4471, 0, 0, 0, 0, 'Flint and Tinder'),
(185039, 7005, 0, 0, 0, 0, 'Skinning Knife'),
(185039, 2901, 0, 0, 0, 0, 'Mining Pick'),
(185039, 18567, 0, 0, 0, 0, 'Elemental Flux'),
(185039, 24288, 0, 0, 0, 0, 'Rune Thread'),
(185039, 14256, 0, 0, 0, 0, 'Felcloth'),
(185039, 14342, 0, 0, 0, 0, 'Mooncloth'),
(185039, 24272, 0, 0, 0, 0, 'Shadowcloth'),
(185039, 24271, 0, 0, 0, 0, 'Spellcloth'),
(185039, 21845, 0, 0, 0, 0, 'Primal Mooncloth'),
(185039, 21840, 0, 0, 0, 0, 'Bolt of Netherweave'),
(185039, 21842, 0, 0, 0, 0, 'Bolt of Imbued Netherweave');


