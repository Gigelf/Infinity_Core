UPDATE `creature_template` SET `minhealth` = 15952 WHERE `entry` = 3296;
UPDATE `creature_template` SET `minhealth` = 0 WHERE `entry` = 777002;
UPDATE `creature_template` SET `minhealth` = 0 WHERE `entry` = 777003;
UPDATE `quest_template` SET `SpecialFlags` = 1 WHERE `entry` = 777000;

DELETE FROM `creature` WHERE `id`=777003;
INSERT INTO `creature` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`modelid`,`equipment_id`,`position_x`,`position_y`,`position_z`,`orientation`,`spawntimesecs`,`spawndist`,`currentwaypoint`,`curhealth`,`curmana`,`DeathState`,`MovementType`) VALUES
(7777141, 777003, 0, 1, 1, 0, 3, -8953.5, 518.49, 96.6, 1.735, 360, 0, 0, 0, 0, 0, 0),
(7777142, 777003, 0, 1, 1, 0, 3, -8925.02, 631.136, 99.5229, 4.92578, 300, 0, 0, 1500000, 0, 0, 0);