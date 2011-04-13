-- aldaron the reckless loc fix Blood elf starting area fix
DELETE FROM `creature` WHERE `id`=16294;
INSERT INTO `creature` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`modelid`,`equipment_id`,`position_x`,`position_y`,`position_z`,`orientation`,`spawntimesecs`,`spawndist`,`currentwaypoint`,`curhealth`,`curmana`,`DeathState`,`MovementType`) VALUES
(67521, 16294, 530, 1, 1, 0, 0, 8747.7, -6116.85, 68.5645, 5.28401, 300, 0, 0, 148, 825, 0, 0);

-- Quest Fix Noth Special Brew 
UPDATE `quest_template` SET `SpecialFlags` = 1 WHERE `entry` = 12717; 
 
DELETE FROM `creature_questrelation` WHERE `quest` = 12716; 
DELETE FROM `gameobject_questrelation` WHERE `quest` = 12716; 
UPDATE `item_template` SET `StartQuest`=0 WHERE `StartQuest` = 12716; 
INSERT INTO `creature_questrelation` (`id`, `quest`) VALUES (28919, 12716); 
UPDATE `creature_template` SET `npcflag`=`npcflag`|2 WHERE `entry` = 28919; 
DELETE FROM `creature_involvedrelation` WHERE `quest` = 12716; 
DELETE FROM `gameobject_involvedrelation` WHERE `quest` = 12716; 
INSERT INTO `creature_involvedrelation` (`id`, `quest`) VALUES (28919, 12716); 
UPDATE `creature_template` SET `npcflag`=`npcflag`|2 WHERE `entry`=28919; 
UPDATE `quest_template` SET `ExclusiveGroup` = 12716 WHERE `entry` = 12716; 

-- faction fix for durotar starting area
UPDATE `creature_template` SET `faction_A` = 16, `faction_H` = 16 WHERE `entry` = 3101;
UPDATE `creature_template` SET `faction_A` = 16, `faction_H` = 16 WHERE `entry` = 3124;
UPDATE `creature_template` SET `faction_A` = 16, `faction_H` = 16 WHERE `entry` = 3102;

--faction fix for sunstrider island starting area
UPDATE `creature_template` SET `faction_A` = 16, `faction_H` = 16 WHERE `entry` = 15271;
UPDATE `creature_template` SET `faction_A` = 16, `faction_H` = 16 WHERE `entry` = 15294;
UPDATE `creature_template` SET `faction_A` = 16, `faction_H` = 16 WHERE `entry` = 15273;
UPDATE `creature_template` SET `faction_A` = 16, `faction_H` = 16 WHERE `entry` = 15298;

-- disable spell kill command 34260
INSERT INTO `spell_disabled` (`entry`) VALUES ('34026 ');

-- spell of light  = insight  
UPDATE `spell_proc_event` SET `SchoolMask`='1', `procFlags`='4' WHERE `entry`='20165'; 

