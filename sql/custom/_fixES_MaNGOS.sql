-- aldaron the reckless loc fix Blood elf starting area fix
DELETE FROM `creature` WHERE `id`=16294;
INSERT INTO `creature` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`modelid`,`equipment_id`,`position_x`,`position_y`,`position_z`,`orientation`,`spawntimesecs`,`spawndist`,`currentwaypoint`,`curhealth`,`curmana`,`DeathState`,`MovementType`) VALUES
(4457369, 16294, 530, 1, 1, 0, 160, 8780.36, -6101.31, 72.6879, 3.2764, 25, 0, 0, 148, 825, 0, 0);

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
