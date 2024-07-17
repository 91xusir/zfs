/*
Navicat MySQL Data Transfer

Source Server         : 127.0.0.1
Source Server Version : 50156
Source Host           : localhost:3306
Source Database       : fso

Target Server Type    : MYSQL
Target Server Version : 50156
File Encoding         : 65001

Date: 2011-09-01 13:43:56
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `community`
-- ----------------------------
DROP TABLE IF EXISTS `community`;
CREATE TABLE `community` (
  `UserID` int(11) NOT NULL DEFAULT '0',
  `Friend` text,
  `GuildID` int(11) DEFAULT '0',
  PRIMARY KEY (`UserID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of community
-- ----------------------------

-- ----------------------------
-- Table structure for `mail`
-- ----------------------------
DROP TABLE IF EXISTS `mail`;
CREATE TABLE `mail` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `ReceiverID` int(11) NOT NULL DEFAULT '0',
  `SenderID` int(11) NOT NULL DEFAULT '0',
  `StrItemID` longtext NOT NULL,
  `Money` int(11) NOT NULL DEFAULT '0',
  `IsOpen` int(11) NOT NULL DEFAULT '0',
  `ReceivedDate` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `Title` longtext NOT NULL,
  `Content` longtext NOT NULL,
  `ReceiverName` varchar(32) NOT NULL DEFAULT '',
  `SenderName` varchar(32) NOT NULL DEFAULT '',
  PRIMARY KEY (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mail
-- ----------------------------

-- ----------------------------
-- Table structure for `py_game_op`
-- ----------------------------
DROP TABLE IF EXISTS `py_game_op`;
CREATE TABLE `py_game_op` (
  `cmdid` int(11) NOT NULL AUTO_INCREMENT,
  `cmd` varchar(50) NOT NULL DEFAULT '',
  `gm_account` int(11) NOT NULL,
  `result` int(1) NOT NULL,
  `toUserName` varchar(32) NOT NULL DEFAULT '',
  PRIMARY KEY (`cmdid`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of py_game_op
-- ----------------------------

-- ----------------------------
-- Table structure for `t_auction`
-- ----------------------------
DROP TABLE IF EXISTS `t_auction`;
CREATE TABLE `t_auction` (
  `Index` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `ItemUID` int(11) NOT NULL DEFAULT '0',
  `ItemIDStr` longtext NOT NULL,
  `ItemVer` int(11) NOT NULL DEFAULT '0',
  `BargainorAcc` int(11) NOT NULL DEFAULT '0',
  `BargainorChrID` int(11) NOT NULL DEFAULT '0',
  `BargainorChr` varchar(45) NOT NULL DEFAULT '',
  `PurchaserAcc` int(11) NOT NULL DEFAULT '0',
  `PurchaserChrID` int(11) NOT NULL DEFAULT '0',
  `PurchaserChr` varchar(45) NOT NULL DEFAULT '',
  `PassTime` int(11) NOT NULL DEFAULT '0',
  `Duration` int(11) NOT NULL DEFAULT '0',
  `HandlingCharge` int(11) NOT NULL DEFAULT '0',
  `BasePrice` int(11) NOT NULL DEFAULT '0',
  `BidPrice` int(11) NOT NULL DEFAULT '0',
  `UniquePrice` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`Index`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of t_auction
-- ----------------------------

-- ----------------------------
-- Table structure for `t_auction_dealing`
-- ----------------------------
DROP TABLE IF EXISTS `t_auction_dealing`;
CREATE TABLE `t_auction_dealing` (
  `Index` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `ItemID` longtext NOT NULL,
  `ItemVer` int(11) NOT NULL DEFAULT '0',
  `BargainorAcc` int(11) NOT NULL DEFAULT '0',
  `BargainorChrID` int(11) NOT NULL DEFAULT '0',
  `BargainorChr` varchar(45) NOT NULL DEFAULT '',
  `PurchaserAcc` int(11) NOT NULL DEFAULT '0',
  `PurchaserChrID` int(11) NOT NULL DEFAULT '0',
  `PurchaserChr` varchar(45) NOT NULL DEFAULT '',
  `PassTime` int(11) NOT NULL DEFAULT '0',
  `Duration` int(11) NOT NULL DEFAULT '0',
  `HandlingCharge` int(11) NOT NULL DEFAULT '0',
  `BasePrice` int(11) NOT NULL DEFAULT '0',
  `BidPrice` int(11) NOT NULL DEFAULT '0',
  `UniquePrice` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`Index`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of t_auction_dealing
-- ----------------------------

-- ----------------------------
-- Table structure for `t_auction_log`
-- ----------------------------
DROP TABLE IF EXISTS `t_auction_log`;
CREATE TABLE `t_auction_log` (
  `Index` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `Type` int(11) NOT NULL DEFAULT '0',
  `ItemID` longtext NOT NULL,
  `ItemVer` int(11) NOT NULL DEFAULT '0',
  `BargainorAcc` int(11) NOT NULL DEFAULT '0',
  `BargainorChrID` int(11) NOT NULL DEFAULT '0',
  `BargainorChr` varchar(45) NOT NULL DEFAULT '',
  `PurchaserAcc` int(11) NOT NULL DEFAULT '0',
  `PurchaserChrID` int(11) NOT NULL DEFAULT '0',
  `PurchaserChr` varchar(45) NOT NULL DEFAULT '',
  `PassTime` int(11) NOT NULL DEFAULT '0',
  `Duration` int(11) NOT NULL DEFAULT '0',
  `HandlingCharge` int(11) NOT NULL DEFAULT '0',
  `BasePrice` int(11) NOT NULL DEFAULT '0',
  `BidPrice` int(11) NOT NULL DEFAULT '0',
  `UniquePrice` int(11) NOT NULL DEFAULT '0',
  `Time` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`Index`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of t_auction_log
-- ----------------------------

-- ----------------------------
-- Table structure for `t_auction_result`
-- ----------------------------
DROP TABLE IF EXISTS `t_auction_result`;
CREATE TABLE `t_auction_result` (
  `Index` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `Type` int(11) NOT NULL DEFAULT '0',
  `ItemID` varchar(255) NOT NULL DEFAULT '',
  `ItemVer` longtext NOT NULL,
  `Money` int(11) NOT NULL DEFAULT '0',
  `ChrID` int(11) NOT NULL DEFAULT '0',
  `Time` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`Index`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of t_auction_result
-- ----------------------------

-- ----------------------------
-- Table structure for `t_bank`
-- ----------------------------
DROP TABLE IF EXISTS `t_bank`;
CREATE TABLE `t_bank` (
  `UserID` int(11) NOT NULL AUTO_INCREMENT,
  `AccountID` int(10) NOT NULL DEFAULT '0',
  `Account` varchar(45) NOT NULL DEFAULT '',
  `Items` longtext NOT NULL,
  `BankMoney` int(11) NOT NULL DEFAULT '0',
  `Pets` longtext NOT NULL COMMENT 'ֿ',
  `Password` varchar(45) NOT NULL DEFAULT '',
  PRIMARY KEY (`UserID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of t_bank
-- ----------------------------

-- ----------------------------
-- Table structure for `t_cardtrade`
-- ----------------------------
DROP TABLE IF EXISTS `t_cardtrade`;
CREATE TABLE `t_cardtrade` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `CardType` int(11) NOT NULL DEFAULT '0',
  `GroupType` int(11) NOT NULL DEFAULT '0',
  `CardID` varchar(45) NOT NULL DEFAULT '',
  `CardPwd` varchar(45) NOT NULL DEFAULT '',
  `CardInfo` varchar(45) NOT NULL DEFAULT '',
  `UserID` int(11) NOT NULL DEFAULT '0',
  `UserName` varchar(45) NOT NULL DEFAULT '',
  `AccountID` int(11) NOT NULL DEFAULT '0',
  `AccountName` varchar(45) NOT NULL DEFAULT '',
  `Price` varchar(45) NOT NULL DEFAULT '',
  `StartTime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `EndTime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of t_cardtrade
-- ----------------------------

-- ----------------------------
-- Table structure for `t_cardtradelog`
-- ----------------------------
DROP TABLE IF EXISTS `t_cardtradelog`;
CREATE TABLE `t_cardtradelog` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `OpType` int(11) NOT NULL DEFAULT '0',
  `CardID` varchar(45) NOT NULL DEFAULT '',
  `UserID` int(11) NOT NULL DEFAULT '0',
  `Price` int(11) NOT NULL DEFAULT '0',
  `opTime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of t_cardtradelog
-- ----------------------------

-- ----------------------------
-- Table structure for `t_castlewar_info`
-- ----------------------------
DROP TABLE IF EXISTS `t_castlewar_info`;
CREATE TABLE `t_castlewar_info` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `FeodName` varchar(45) NOT NULL DEFAULT '',
  `UnionID` int(11) NOT NULL DEFAULT '0',
  `StartTime` int(11) NOT NULL DEFAULT '0',
  `UnionName` varchar(45) NOT NULL DEFAULT '',
  `VassalName` varchar(45) NOT NULL DEFAULT '',
  `FlagID` int(11) NOT NULL DEFAULT '0',
  `IsAttacker` int(11) NOT NULL DEFAULT '0',
  `IsAllowed` int(11) NOT NULL DEFAULT '0',
  `AnimalDead` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of t_castlewar_info
-- ----------------------------

-- ----------------------------
-- Table structure for `t_castlewar_time_set`
-- ----------------------------
DROP TABLE IF EXISTS `t_castlewar_time_set`;
CREATE TABLE `t_castlewar_time_set` (
  `FeodName` varchar(32) NOT NULL DEFAULT '',
  `DefWeekDay` int(11) NOT NULL,
  `DefHour` int(11) NOT NULL,
  `DefMinute` int(11) NOT NULL,
  `DefDuration` int(11) NOT NULL,
  `LatestTime` int(11) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=gb2312 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of t_castlewar_time_set
-- ----------------------------
INSERT INTO `t_castlewar_time_set` VALUES ('ChuanYun', '0', '0', '0', '0', '0');
INSERT INTO `t_castlewar_time_set` VALUES ('XiQi', '0', '0', '0', '0', '0');
INSERT INTO `t_castlewar_time_set` VALUES ('ZhaoGe', '0', '0', '0', '0', '0');

-- ----------------------------
-- Table structure for `t_createvote`
-- ----------------------------
DROP TABLE IF EXISTS `t_createvote`;
CREATE TABLE `t_createvote` (
  `VoteID` int(11) NOT NULL AUTO_INCREMENT,
  `c_Creater` int(4) unsigned NOT NULL DEFAULT '0',
  `Recall` int(4) unsigned NOT NULL DEFAULT '0',
  `EndTime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `UnionID` int(8) NOT NULL DEFAULT '0',
  PRIMARY KEY (`VoteID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of t_createvote
-- ----------------------------

-- ----------------------------
-- Table structure for `t_dungeonid`
-- ----------------------------
DROP TABLE IF EXISTS `t_dungeonid`;
CREATE TABLE `t_dungeonid` (
  `LastID` int(10) unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`LastID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of t_dungeonid
-- ----------------------------
INSERT INTO `t_dungeonid` VALUES ('122');

-- ----------------------------
-- Table structure for `t_feod`
-- ----------------------------
DROP TABLE IF EXISTS `t_feod`;
CREATE TABLE `t_feod` (
  `Name` varchar(32) CHARACTER SET latin1 NOT NULL DEFAULT '',
  `CurrentUnion` int(11) NOT NULL DEFAULT '0',
  `TaxRate` float NOT NULL DEFAULT '0',
  `Bank` int(11) NOT NULL DEFAULT '0',
  `LastDayIncome` int(11) NOT NULL DEFAULT '0',
  `LastDay` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`Name`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of t_feod
-- ----------------------------

-- ----------------------------
-- Table structure for `t_gift`
-- ----------------------------
DROP TABLE IF EXISTS `t_gift`;
CREATE TABLE `t_gift` (
  `idx` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `charID` int(11) NOT NULL DEFAULT '0',
  `level` int(11) NOT NULL DEFAULT '0',
  `type` int(11) NOT NULL DEFAULT '0',
  `attribute` varchar(300) NOT NULL DEFAULT '',
  `addTime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `limitTime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `msg` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`idx`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of t_gift
-- ----------------------------

-- ----------------------------
-- Table structure for `t_itemid`
-- ----------------------------
DROP TABLE IF EXISTS `t_itemid`;
CREATE TABLE `t_itemid` (
  `RegionID` int(11) NOT NULL DEFAULT '0',
  `ItemID` int(15) NOT NULL DEFAULT '0',
  `BeginID` int(11) NOT NULL DEFAULT '0',
  `MaxID` int(15) NOT NULL DEFAULT '0',
  PRIMARY KEY (`RegionID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of t_itemid
-- ----------------------------
INSERT INTO `t_itemid` VALUES ('0', '2484001', '0', '134217727');
INSERT INTO `t_itemid` VALUES ('1', '134219728', '0', '268435454');
INSERT INTO `t_itemid` VALUES ('2', '268435455', '0', '402653181');
INSERT INTO `t_itemid` VALUES ('3', '402653182', '0', '536870908');
INSERT INTO `t_itemid` VALUES ('4', '536870909', '0', '671088635');
INSERT INTO `t_itemid` VALUES ('5', '671088636', '0', '805306362');
INSERT INTO `t_itemid` VALUES ('6', '805306363', '0', '939524089');
INSERT INTO `t_itemid` VALUES ('7', '939524090', '0', '1073741816');
INSERT INTO `t_itemid` VALUES ('8', '1073741817', '0', '1207959543');
INSERT INTO `t_itemid` VALUES ('9', '1207959544', '0', '1342177270');
INSERT INTO `t_itemid` VALUES ('10', '1342177271', '0', '1476394997');
INSERT INTO `t_itemid` VALUES ('11', '1476394998', '0', '1610612724');
INSERT INTO `t_itemid` VALUES ('12', '1610612725', '0', '1744830451');
INSERT INTO `t_itemid` VALUES ('13', '1744830452', '0', '1879048178');
INSERT INTO `t_itemid` VALUES ('14', '1879048179', '0', '2013265905');
INSERT INTO `t_itemid` VALUES ('15', '2013265906', '0', '2147483632');
INSERT INTO `t_itemid` VALUES ('16', '2147483633', '0', '-2013265937');
INSERT INTO `t_itemid` VALUES ('17', '-2013265936', '0', '-1879048210');
INSERT INTO `t_itemid` VALUES ('18', '-1879048209', '0', '-1744830483');
INSERT INTO `t_itemid` VALUES ('19', '-1744830482', '0', '-1610612756');
INSERT INTO `t_itemid` VALUES ('20', '-1610612755', '0', '-1476395029');
INSERT INTO `t_itemid` VALUES ('21', '-1476395028', '0', '-1342177302');
INSERT INTO `t_itemid` VALUES ('22', '-1342177301', '0', '-1207959575');
INSERT INTO `t_itemid` VALUES ('23', '-1207959574', '0', '-1073741848');
INSERT INTO `t_itemid` VALUES ('24', '-1073741847', '0', '-939524121');
INSERT INTO `t_itemid` VALUES ('25', '-939524120', '0', '-805306394');
INSERT INTO `t_itemid` VALUES ('26', '-805306393', '0', '-671088667');
INSERT INTO `t_itemid` VALUES ('27', '-671088666', '0', '-536870940');
INSERT INTO `t_itemid` VALUES ('28', '-536870939', '0', '-402653213');
INSERT INTO `t_itemid` VALUES ('29', '-402653212', '0', '-268435486');
INSERT INTO `t_itemid` VALUES ('30', '-268435485', '0', '-134217759');
INSERT INTO `t_itemid` VALUES ('31', '-134217758', '0', '-32');
INSERT INTO `t_itemid` VALUES ('254', '48177101', '0', '69675675');
INSERT INTO `t_itemid` VALUES ('255', '1231314091', '0', '1312312312');

-- ----------------------------
-- Table structure for `t_masterprentice`
-- ----------------------------
DROP TABLE IF EXISTS `t_masterprentice`;
CREATE TABLE `t_masterprentice` (
  `Idx` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `MasterID` int(11) NOT NULL DEFAULT '0',
  `MasterName` varchar(45) NOT NULL DEFAULT '',
  `PrenticeID` int(11) NOT NULL DEFAULT '0',
  `PrenticeName` varchar(45) NOT NULL DEFAULT '',
  `FinishTime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `CreditToAdd` int(11) NOT NULL DEFAULT '0',
  `FinishOnlineTime` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`Idx`),
  KEY `MasterID` (`MasterID`),
  KEY `PrenticeID` (`PrenticeID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of t_masterprentice
-- ----------------------------

-- ----------------------------
-- Table structure for `t_pet`
-- ----------------------------
DROP TABLE IF EXISTS `t_pet`;
CREATE TABLE `t_pet` (
  `UserID` int(11) NOT NULL AUTO_INCREMENT,
  `AccountID` int(11) NOT NULL DEFAULT '0',
  `Name` varchar(45) NOT NULL DEFAULT '',
  `Pets` longtext NOT NULL COMMENT 'ﱳ',
  PRIMARY KEY (`UserID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of t_pet
-- ----------------------------

-- ----------------------------
-- Table structure for `t_union`
-- ----------------------------
DROP TABLE IF EXISTS `t_union`;
CREATE TABLE `t_union` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `Name` varchar(32) NOT NULL DEFAULT '',
  `Level` int(4) unsigned NOT NULL DEFAULT '0',
  `Enounce` longtext,
  `Bulletin` longtext,
  `Icon` varchar(32) NOT NULL DEFAULT '0',
  `CaptainID` int(11) NOT NULL DEFAULT '0',
  `IconData` longblob,
  `ToBeDeletedBegin` datetime DEFAULT NULL,
  `Score` int(11) NOT NULL DEFAULT '0',
  `Money` int(15) NOT NULL DEFAULT '0',
  `Item1` int(8) NOT NULL DEFAULT '0',
  `Item2` int(8) NOT NULL DEFAULT '0',
  `Item3` int(8) NOT NULL DEFAULT '0',
  `Item4` int(8) NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`),
  UNIQUE KEY `Name` (`Name`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of t_union
-- ----------------------------

-- ----------------------------
-- Table structure for `t_union_citizen`
-- ----------------------------
DROP TABLE IF EXISTS `t_union_citizen`;
CREATE TABLE `t_union_citizen` (
  `userID` int(11) NOT NULL DEFAULT '0',
  `official` int(4) NOT NULL DEFAULT '0',
  `name` varchar(32) NOT NULL DEFAULT '',
  `nickname` varchar(32) DEFAULT NULL,
  `UnionID` int(11) NOT NULL DEFAULT '0'
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of t_union_citizen
-- ----------------------------

-- ----------------------------
-- Table structure for `t_union_citizen_back`
-- ----------------------------
DROP TABLE IF EXISTS `t_union_citizen_back`;
CREATE TABLE `t_union_citizen_back` (
  `userID` int(11) NOT NULL DEFAULT '0' COMMENT 'ûid',
  `official` int(4) NOT NULL DEFAULT '0' COMMENT 'ְ',
  `UnionID` int(11) NOT NULL DEFAULT '0' COMMENT 'id',
  `Contribution` int(11) NOT NULL DEFAULT '0' COMMENT 'ɹ',
  `Fighting` int(11) NOT NULL DEFAULT '0' COMMENT 'ս'
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of t_union_citizen_back
-- ----------------------------

-- ----------------------------
-- Table structure for `t_vote`
-- ----------------------------
DROP TABLE IF EXISTS `t_vote`;
CREATE TABLE `t_vote` (
  `VID` int(11) NOT NULL AUTO_INCREMENT,
  `VoteID` int(11) NOT NULL,
  `VoterID` int(4) unsigned NOT NULL DEFAULT '0',
  `VState` int(4) unsigned NOT NULL DEFAULT '0',
  `VoteTime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`VID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of t_vote
-- ----------------------------

-- ----------------------------
-- Table structure for `t_votelog`
-- ----------------------------
DROP TABLE IF EXISTS `t_votelog`;
CREATE TABLE `t_votelog` (
  `LogID` int(11) NOT NULL AUTO_INCREMENT,
  `LogType` int(11) NOT NULL,
  `LogData` varchar(45) NOT NULL DEFAULT '',
  `LogTime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `UnionID` int(8) NOT NULL DEFAULT '0',
  PRIMARY KEY (`LogID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of t_votelog
-- ----------------------------

-- ----------------------------
-- Table structure for `user`
-- ----------------------------
DROP TABLE IF EXISTS `user`;
CREATE TABLE `user` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `UniqueName` varchar(32) NOT NULL DEFAULT '',
  `Name` varchar(32) NOT NULL DEFAULT '',
  `ActorID` int(11) NOT NULL DEFAULT '0',
  `AccountID` int(11) NOT NULL DEFAULT '0',
  `Account` varchar(20) NOT NULL DEFAULT '',
  `Scene` varchar(32) NOT NULL DEFAULT '',
  `HP` int(11) NOT NULL DEFAULT '0',
  `MP` int(11) NOT NULL DEFAULT '0',
  `EXP` int(11) NOT NULL DEFAULT '0',
  `AP` int(11) NOT NULL DEFAULT '0',
  `Level` int(11) NOT NULL DEFAULT '0',
  `DeleteDate` datetime DEFAULT CURRENT_TIMESTAMP,
  `CreateDate` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `LastUpdateDate` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `Metier` int(11) NOT NULL DEFAULT '0',
  `MetierLevel` int(11) NOT NULL DEFAULT '0',
  `X` int(11) NOT NULL DEFAULT '0',
  `Y` int(11) NOT NULL DEFAULT '0',
  `Gold` int(11) NOT NULL DEFAULT '0',
  `SP` int(11) DEFAULT '0',
  `Str` smallint(6) NOT NULL DEFAULT '0',
  `Con` smallint(6) NOT NULL DEFAULT '0',
  `Dex` smallint(6) NOT NULL DEFAULT '0',
  `Int` smallint(6) NOT NULL DEFAULT '0',
  `Hit` smallint(6) NOT NULL DEFAULT '0',
  `Skills` longtext,
  `Items` longtext,
  `Shortcut` longtext,
  `Config` longtext,
  `PK` int(11) NOT NULL DEFAULT '0',
  `Task` longtext,
  `Score` int(11) NOT NULL DEFAULT '0',
  `HeadModelID` int(11) NOT NULL DEFAULT '0',
  `Credit` int(11) NOT NULL DEFAULT '0',
  `OnlineTime` int(11) NOT NULL DEFAULT '0',
  `usertype` smallint(4) NOT NULL DEFAULT '0',
  `LastDelUnionTime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `LastRemoveMPTime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `Password` varchar(45) NOT NULL DEFAULT '',
  `deleted` int(11) NOT NULL DEFAULT '0',
  `Faction` smallint(5) NOT NULL DEFAULT '0',
  `ClearCnt` int(11) DEFAULT '0',
  `DungeonTID` int(11) DEFAULT '0',
  `DungeonUID` int(11) DEFAULT '0',
  `DungeonQuit` int(11) DEFAULT '0',
  `DungeonTask` longtext,
  `InWarfield` int(11) DEFAULT '0',
  `WarfieldQuit` int(11) DEFAULT '0',
  `UsData` longtext,
  `WithdrawMPGift` int(10) unsigned NOT NULL DEFAULT '0',
  `MyData` longblob,
  `IsOnline` tinyint(1) NOT NULL DEFAULT '0',
  `c_Cul` int(16) DEFAULT '0',
  `c_Soul` int(11) DEFAULT '0',
  `c_Contribute` int(16) DEFAULT '0',
  `c_Point` int(16) DEFAULT '0',
  `c_Record` int(16) DEFAULT '0',
  `c_UnionRecord` int(16) DEFAULT '0',
  `c_JobRecord` int(16) DEFAULT '0',
  `c_BornTag` int(4) DEFAULT '0' COMMENT 'Ф',
  `Sex` int(4) NOT NULL DEFAULT '0' COMMENT 'ɫԱ ĬΪ',
  `TeamFree` int(2) DEFAULT '1',
  `AreaHonors` longtext NOT NULL COMMENT 'Ԥ128',
  `Stage` int(4) DEFAULT '0' COMMENT '׶',
  `RelivePostion` longtext NOT NULL COMMENT '󶨸',
  `Vigour` int(16) DEFAULT '0' COMMENT '???',
  `VigourPoint` int(16) DEFAULT '0' COMMENT '???',
  `HasPassword` int(1) DEFAULT '0',
  `LineSysGroupID` int(11) NOT NULL DEFAULT '0',
  `LineSysLineID` int(11) NOT NULL DEFAULT '0',
  `nRelivePostion` int(4) DEFAULT '0',
  `GradeLev` int(11) NOT NULL DEFAULT '0',
  `PlayerOnlineTime` int(11) NOT NULL DEFAULT '0',
  `CulExp` int(11) NOT NULL DEFAULT '0',
  `Exploit` int(11) NOT NULL DEFAULT '0',
  `EnP` int(11) NOT NULL DEFAULT '0',
  `HeadImageID` int(11) NOT NULL DEFAULT '0',
  `petItemID` int(11) NOT NULL DEFAULT '0',
  `Popularity` longtext NOT NULL,
  `isFirstLogin` tinyint(3) NOT NULL DEFAULT '0',
  `IsDead` tinyint(3) DEFAULT '0',
  `EffectSkill` longtext NOT NULL,
  `ChangeModelID` int(11) NOT NULL DEFAULT '0',
  `HairIndex` int(11) DEFAULT '0',
  `FaceIndex` int(11) DEFAULT '0',
  PRIMARY KEY (`ID`),
  UNIQUE KEY `UniqueName` (`UniqueName`),
  KEY `ID` (`ID`),
  KEY `Account` (`Account`),
  KEY `AccountID` (`AccountID`),
  KEY `LastUpdateDate` (`LastUpdateDate`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of user
-- ----------------------------
