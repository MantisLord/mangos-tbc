-- Execute script in CHARACTERS database

-- settings table shared by both XP Boost and Dual Spec

-- ==============================================
-- Table structure for table `character_settings`
-- ==============================================

DROP TABLE IF EXISTS `character_settings`;
CREATE TABLE `character_settings` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Player GUID',
  `id` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Setting ID',
  `value` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Setting value',
  PRIMARY KEY (`guid`,`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT COMMENT='Player System';

-- =========
-- Dual Spec
-- =========

-- ============================================
-- Table structure for table `character_talent`
-- ============================================

DROP TABLE IF EXISTS `character_talent`;
CREATE TABLE `character_talent` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0',
  `spell` int(11) unsigned NOT NULL DEFAULT '0',
  `spec` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`,`spell`,`spec`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ============================================
-- Table structure for table `character_action`
-- ============================================
ALTER TABLE `character_action`
  ADD COLUMN `spec` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0' AFTER `guid`,
  DROP PRIMARY KEY,
  ADD PRIMARY KEY (`guid`, `spec`, `button`);
    
-- =================================================
-- Table structure for table `character_talent_name`
-- =================================================

DROP TABLE IF EXISTS `character_talent_name`;
CREATE TABLE `character_talent_name` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0',
  `spec` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`guid`,`spec`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- =============
-- Arena Logging
-- =============

-- =========================================
-- Table structure for table `arena_matches`
-- =========================================

DROP TABLE IF EXISTS arena_matches;
CREATE TABLE `arena_matches` (
	`matchid` INT(11) UNSIGNED NOT NULL AUTO_INCREMENT,
	`winner_teamid` INT(10) UNSIGNED NULL DEFAULT NULL,
	`loser_teamid` INT(10) UNSIGNED NULL DEFAULT NULL,
	`map` INT(11) UNSIGNED NOT NULL DEFAULT '0',
	`time` TIMESTAMP NULL DEFAULT NULL,
	`duration` INT(11) UNSIGNED NOT NULL DEFAULT '0',
	`winner_rating` INT(11) UNSIGNED NOT NULL DEFAULT '0',
	`loser_rating` INT(11) UNSIGNED NOT NULL DEFAULT '0',
	`winner_rating_change` INT(11) NOT NULL DEFAULT '0',
	`loser_rating_change` INT(11) NOT NULL DEFAULT '0',
	PRIMARY KEY (`matchid`) USING BTREE
) ENGINE=InnoDB;

-- ===============================================
-- Table structure for table `arena_match_players`
-- ===============================================

DROP TABLE IF EXISTS arena_match_players;
CREATE TABLE `arena_match_players` (
	`matchid` INT(11) UNSIGNED NOT NULL,
	`character_guid` INT(10) UNSIGNED NOT NULL,
	`teamid` INT(10) UNSIGNED NOT NULL,
	`score_killing_blows` MEDIUMINT(8) UNSIGNED NOT NULL,
	`score_damage_done` MEDIUMINT(8) UNSIGNED NOT NULL,
	`score_healing_done` MEDIUMINT(8) UNSIGNED NOT NULL,
	PRIMARY KEY (`matchid`, `character_guid`) USING BTREE
) ENGINE=InnoDB;

-- ========
-- XP Boost
-- ========

-- ==================================================
-- Table structure for table `experience_bracket_cap`
-- ==================================================

DROP TABLE IF EXISTS `experience_bracket_cap`;
CREATE TABLE `experience_bracket_cap` (
  `low` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Lower bracket bound',
  `high` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Upper bracket bound',
  `team` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Bracket team',
  `value` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Bracket XP cap',
  PRIMARY KEY (`low`,`high`,`team`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT COMMENT='XP System';

-- ========
-- Transmogrification
-- ========

-- ===============================================
-- Table structure for `custom_transmogrification`
-- ===============================================
DROP TABLE IF EXISTS `custom_transmogrification`;
CREATE TABLE `custom_transmogrification` (
  `Guid` int(10) unsigned NOT NULL DEFAULT '0',
  `FakeEntry` int(10) unsigned NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

