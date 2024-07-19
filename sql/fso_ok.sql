/*
Navicat MySQL Data Transfer

Source Server         : 127.0.0.1
Source Server Version : 50156
Source Host           : localhost:3306
Source Database       : fso

Target Server Type    : MYSQL
Target Server Version : 50156
File Encoding         : 65001

Date: 2011-09-02 16:53:24
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
INSERT INTO `community` VALUES ('1', '0000006c00000000', '0');
INSERT INTO `community` VALUES ('2', '0000006c00000000', '0');
INSERT INTO `community` VALUES ('3', '0000006c00000000', '0');
INSERT INTO `community` VALUES ('4', '0000006c00000000', '0');

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
) ENGINE=MyISAM AUTO_INCREMENT=5 DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of t_bank
-- ----------------------------
INSERT INTO `t_bank` VALUES ('1', '1', 'testesfsefe', '#000b,0,1,8,7,0,', '0', '#000b,0,1,1,5,0,', '');
INSERT INTO `t_bank` VALUES ('2', '1', 'eesegtes', '#000b,0,1,8,7,0,', '0', '#000b,0,1,1,5,0,', '');
INSERT INTO `t_bank` VALUES ('3', '1', 'tesfesest', '#000b,0,1,8,7,0,', '0', '#000b,0,1,1,5,0,', '');
INSERT INTO `t_bank` VALUES ('4', '1', '娇小', '#000b,0,1,8,7,0,', '0', '#000b,0,1,1,5,0,', '');

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
) ENGINE=MyISAM AUTO_INCREMENT=4 DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of t_castlewar_info
-- ----------------------------
INSERT INTO `t_castlewar_info` VALUES ('2', 'XiQi', '1', '1', '', 'XiQi', '1', '1', '1', '0');
INSERT INTO `t_castlewar_info` VALUES ('3', 'XiQi', '1', '1', '', 'XiQi', '1', '1', '1', '0');
INSERT INTO `t_castlewar_info` VALUES ('1', 'XiQi', '1', '1', '', 'XiQi', '1', '1', '1', '0');

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
-- Table structure for `t_item`
-- ----------------------------
DROP TABLE IF EXISTS `t_item`;
CREATE TABLE `t_item` (
  `index` bigint(20) NOT NULL AUTO_INCREMENT COMMENT '物品唯一ID',
  `item_id` int(11) NOT NULL,
  `creature_id` int(11) NOT NULL DEFAULT '0' COMMENT '角色ID',
  `bag_type` tinyint(4) NOT NULL COMMENT '包裹类型',
  `bag_page` tinyint(4) NOT NULL DEFAULT '0' COMMENT '物品所在包裹页码',
  `bag_line` tinyint(4) NOT NULL DEFAULT '0' COMMENT '物品所在包裹行',
  `bag_column` tinyint(4) NOT NULL DEFAULT '0' COMMENT '物品所在包裹列',
  `item_type` int(11) NOT NULL COMMENT '物品种类',
  `count` int(11) NOT NULL DEFAULT '0' COMMENT '药瓶的剩余数量',
  `watt` int(11) NOT NULL DEFAULT '0',
  `attrib_id1` int(11) NOT NULL DEFAULT '0' COMMENT '有色装备额外增加的属性ID1',
  `attrib_rate1` smallint(6) NOT NULL DEFAULT '0' COMMENT '有色装备属性ID的加成比率1',
  `attrib_id2` int(11) NOT NULL DEFAULT '0' COMMENT '有色装备额外增加的属性ID2',
  `attrib_rate2` smallint(6) NOT NULL DEFAULT '0' COMMENT '有色装备属性ID的加成比率2',
  `pet_name` varchar(32) NOT NULL DEFAULT '',
  `param_count` tinyint(4) NOT NULL DEFAULT '0' COMMENT '使用到的参数个数',
  `params` longtext COMMENT '使用到的参数',
  `enchant_attrib` smallint(6) NOT NULL DEFAULT '0' COMMENT '附魔的属性ID',
  `time_limit` int(11) NOT NULL DEFAULT '0' COMMENT '过期时间',
  `cur_wear` int(11) NOT NULL DEFAULT '-1' COMMENT '当前耐久',
  PRIMARY KEY (`index`),
  UNIQUE KEY `index` (`index`),
  KEY `creature_id` (`creature_id`),
  KEY `item_type` (`item_type`),
  KEY `item_id` (`item_id`)
) ENGINE=MyISAM AUTO_INCREMENT=64 DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of t_item
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
INSERT INTO `t_itemid` VALUES ('0', '3812001', '0', '134217727');
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
INSERT INTO `t_itemid` VALUES ('254', '48601101', '0', '69675675');
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
) ENGINE=MyISAM AUTO_INCREMENT=5 DEFAULT CHARSET=gb2312 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of t_pet
-- ----------------------------
INSERT INTO `t_pet` VALUES ('1', '1', 'testesfsefe', '#0001#000b,2,1,1,3,0,0,0,48594103,16502,0,幼年云生兽,36,5,229,353,143,0,0,0,3324,10700,0,11100,11520,9200,0,0,32400,27000,25800,56500,0,0,0,0,0,0,6501,6542,6554,0,0,0,0,0,0,0,0,0,0,1,3804295,16000,0,兔宝宝,36,5,271,312,178,0,0,0,1775,13320,2120,4700,4750,5750,0,0,0,0,0,0,0,0,0,0,0,0,6501,0,0,0,0,0,0,0,0,0,0,0,');
INSERT INTO `t_pet` VALUES ('2', '1', 'eesegtes', '#0001#000b,1,1,1,11,0,0,0,48594111,16501,0,幼年风生兽,36,4,177,325,134,0,0,0,3044,10900,0,11100,12000,9100,28200,0,0,48000,30299,27000,8025,0,0,0,0,0,6501,6538,6598,0,0,0,0,0,0,0,0,0,');
INSERT INTO `t_pet` VALUES ('3', '1', 'tesfesest', '#0001#000b,1,1,1,11,0,0,0,48597103,16501,0,幼年风生兽,36,1,0,285,97,0,0,0,3324,10700,0,11100,11520,9200,27900,0,0,45000,25800,33900,0,0,0,0,0,0,6501,6538,6550,0,0,0,0,0,0,0,0,0,');
INSERT INTO `t_pet` VALUES ('4', '1', '娇小', '#0001#000b,1,1,1,3,0,0,0,48599103,16501,0,幼年风生兽,36,1,0,285,97,0,0,0,3324,10700,0,11100,11520,9200,27900,0,0,45000,25800,33900,0,0,0,0,0,0,6501,6538,6550,0,0,0,0,0,0,0,0,0,');

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
  `AreaHonors` longtext,
  `Stage` int(4) DEFAULT '0' COMMENT '׶',
  `RelivePostion` longtext,
  `Vigour` int(16) DEFAULT '0' COMMENT '???',
  `VigourPoint` int(16) NOT NULL DEFAULT '0' COMMENT '???',
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
  `Popularity` longtext,
  `isFirstLogin` tinyint(3) NOT NULL DEFAULT '0',
  `IsDead` tinyint(3) DEFAULT '0',
  `EffectSkill` longtext,
  `ChangeModelID` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`,`VigourPoint`),
  UNIQUE KEY `UniqueName` (`UniqueName`),
  KEY `ID` (`ID`),
  KEY `Account` (`Account`),
  KEY `AccountID` (`AccountID`),
  KEY `LastUpdateDate` (`LastUpdateDate`)
) ENGINE=MyISAM AUTO_INCREMENT=5 DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of user
-- ----------------------------
INSERT INTO `user` VALUES ('1', 'testesfsefe', 'testesfsefe', '4', '1', '', '', '478', '192', '40', '12', '5', CURRENT_TIMESTAMP, '2011-09-02 14:36:15', '2011-09-02 16:26:00', '2', '0', '590', '1770', '70', '1', '14', '18', '18', '14', '14', '40000090F60000000000070000001000170000000000A60000000000B60000000000C4000000000027000000000092A00000000055B00000000000', '#000b#000b,16,2,6,7,0,0,0,48594104,41,0,0,10000,0,0,1,48594107,2221,0,0,9968,0,0,2,3804289,6054,880,0,93,0,0,3,3804290,6055,880,0,98,0,0,4,3804291,6086,880,0,99,0,0,5,3804292,6087,880,0,99,0,0,6,3804293,4383,82,6005,90,6006,120,0,9989,0,1,0,3808300,2661,0,0,10000,0,1,1,3808301,7501,0,0,0,1,2,3808303,7601,0,0,0,1,3,3808307,41,0,0,10000,0,1,4,3808309,2021,0,0,10000,0,1,5,3808341,2601,0,0,10000,0,1,6,3808343,5522,80,0,0,2,0,3808346,4413,81,3019,62,0,10000,0,2,1,3808345,4398,81,4505,64,6,0,0,0,0,0,0,10000,70,#000b,0,1,6,7,0,48594105,61,0,0,10000,0,0,0,0,3804294,4384,82,6007,108,6008,87,0,9802,0,0,0,0,3808347,4418,81,3026,52,0,10000,3808344,4397,81,4017,83,6,0,0,0,0,0,0,10000,48594106,2201,0,0,9951,48594109,2261,0,0,9959,48594108,2241,0,0,9966,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,', '1;3;111;3;2901;3;2601;0;0;0;0;3;107;3;112;3;113;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;', '1,1,1,2,50,3,7183,6,0,4,0,5,0', '0', '6,2,11000,11030,8388607,769,3,1,0,1,-842150451,3,11001,0,1,1314945617,0,0,0,0,0,-2147483648,11031,0,3,1314945722,0,0,0,0,0,-2147483648,11034,0,3,1314945730,0,0,0,0,0,-2147483648', '0', '6', '0', '1023', '0', CURRENT_TIMESTAMP, CURRENT_TIMESTAMP, 'd41d8cd98f00b204e9800998ecf8427e', '0', '3', '0', '0', '0', '0', '1,0,', '0', '0', '02f7b3622d30', '0', 0x5C00050000000000000000000000F400F40000000000F40000000000CDCDCDCD0000F40000000000F40002000000000000000000F492604E0000000001000000000000000300000006001893604EFF0300007B15000000000000000000000000, '0', '8', '52', '0', '0', '0', '0', '0', '5', '1', '1', null, '0', null, '0', '0', '0', '1', '1', '0', '2', '0', '6', '0', '0', '0', '48594103', '', '1', '0', '3000', '0');
INSERT INTO `user` VALUES ('2', 'eesegtes', 'eesegtes', '1', '1', '', '', '66', '96', '200', '10', '3', CURRENT_TIMESTAMP, '2011-09-02 14:45:56', '2011-09-02 14:50:07', '1', '0', '1692', '2668', '3', '1', '12', '12', '12', '12', '12', '40000080F60000000000070000000000170000000000A60000000000B60000000000C4000000000027000000000008C00000100010CBC00010000000', '#000b#000b,2,2,6,7,0,0,0,48594110,5503,0,0,0,0,1,3804306,8104,0,0,3,#000b,0,1,6,7,0,48594112,81,0,0,9946,0,0,0,0,48594113,101,0,0,10000,0,0,0,0,48594115,2421,0,0,9992,0,0,0,0,48594114,2401,0,0,9986,48594117,2461,0,0,9982,48594116,2441,0,0,9986,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,', '1;3;111;3;3200;0;0;0;0;0;3;107;3;112;3;113;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;', '1,1,1,2,50,3,7183,6,0,4,0,5,0', '0', '6,0,520319,768,0,1,0,1,-842150451,0', '0', '1', '0', '233', '0', CURRENT_TIMESTAMP, CURRENT_TIMESTAMP, 'd41d8cd98f00b204e9800998ecf8427e', '0', '3', '0', '0', '0', '0', '1,0,', '0', '0', '02ed8e6db2c5', '0', 0x5C000500000000000000000000000000000000000000000000000000CDCDCDCD0000000000000000000002000000000000000000B67B604E0000000001000000000000000300000006009F7C604EE90000000000000000000000000000000000, '0', '1', '8', '0', '0', '0', '0', '0', '5', '0', '1', null, '0', null, '0', '0', '0', '1', '1', '0', '1', '0', '1', '0', '0', '0', '0', '', '1', '0', '3000', '0');
INSERT INTO `user` VALUES ('3', 'tesfesest', 'tesfesest', '7', '1', '', '', '60', '62', '106', '5', '2', CURRENT_TIMESTAMP, '2011-09-02 16:06:34', '2011-09-02 16:12:55', '3', '0', '863', '966', '3', '0', '11', '11', '11', '11', '11', '40000090F60000000000070000000000170000000000A60000000000B60000000000C400000000002700000000000D7000005000CF800000400000', '#000b#000b,7,2,6,7,0,0,0,48597104,1,0,0,10000,0,0,1,3808288,5511,80,0,0,0,2,3808289,6054,880,0,99,0,0,3,3808290,6055,880,0,99,0,0,4,3808291,6086,880,0,99,0,0,5,3808292,6087,880,0,99,0,0,6,3808293,4381,82,6001,75,6002,123,0,9989,3,#000b,0,1,6,7,0,3808294,4382,82,6003,90,6004,84,0,9984,0,0,0,0,48597105,21,0,0,10000,0,0,0,0,48597107,2021,0,0,9994,0,0,0,0,48597106,2001,0,0,9996,48597109,2061,0,0,9989,48597108,2041,0,0,9991,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,', '1;3;111;3;2000;3;2300;0;0;0;0;3;107;3;112;3;113;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;', '1,1,1,2,50,3,7183,6,0,4,0,5,0', '0', '6,0,1044607,771,1,1,0,1,0,0', '0', '13', '0', '358', '0', CURRENT_TIMESTAMP, CURRENT_TIMESTAMP, 'd41d8cd98f00b204e9800998ecf8427e', '0', '3', '0', '0', '0', '0', '1,0,', '0', '0', '0270b5ad4c82', '0', 0x5C000500000000000000000000000000000000000000000000000000000000000000000000000000000002000000000000000000A18E604E0000000001000000000000000300000006000790604E660100000000000000000000000000000000, '0', '0', '3', '0', '0', '0', '0', '0', '0', '0', '1', null, '0', null, '0', '0', '0', '1', '1', '1', '1', '0', '0', '0', '0', '1', '0', '', '1', '0', '3000', '0');
INSERT INTO `user` VALUES ('4', '娇小', '娇小', '16', '1', '', '', '383', '276', '165', '20', '5', CURRENT_TIMESTAMP, '2011-09-02 16:26:51', '2011-09-02 16:46:48', '4', '0', '840', '1743', '134', '3', '14', '14', '14', '14', '14', '40000090F600000000000700000000001700C8101000A60000000000B60000000000C400000000002700000000008DE00000500040010000300000', '#000b#000b,11,2,6,7,0,0,0,48599104,121,0,0,10000,0,0,1,3810295,4388,82,6015,82,6016,91,0,9978,0,0,2,3810290,6054,880,0,96,0,0,3,3810291,6055,880,0,98,0,0,4,3810292,6086,880,0,99,0,0,5,3810293,6087,880,0,99,0,0,6,3810308,2601,0,0,10000,0,1,0,3810302,5524,80,0,0,1,2,3810304,4398,81,4526,89,6,0,0,0,0,0,0,10000,0,1,3,3810305,4433,81,3008,88,0,10000,0,1,4,3810306,4438,82,3032,117,3130,91,0,10000,134,#000b,1,1,6,7,0,0,0,3810297,5209,0,0,0,3810294,4387,82,6013,115,6014,98,0,9989,0,0,0,0,48599105,141,0,0,10000,0,0,0,0,48599107,2621,0,0,9985,3810303,4397,82,4067,117,4103,112,6,0,0,0,0,0,0,10000,48599106,2601,0,0,9986,48599109,2661,0,0,9987,48599108,2641,0,0,9988,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,', '1;3;111;3;3800;3;4100;0;0;0;0;3;107;3;112;3;113;2;3810290;1092068986;2;3810291;1108846458;2;3810292;1142408826;2;3810293;1159186298;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;', '1,1,1,2,50,3,7183,6,0,4,0,5,0', '0', '6,7,10001,10002,11000,11001,11002,11030,11042,7864319,769,3,3,0,1,-842150451,6,11003,0,1,1314952248,0,0,0,0,0,-2147483632,11031,0,3,1314952306,0,0,0,0,0,-2147483648,11034,0,3,1314952308,0,0,0,0,0,-2147483648,11037,0,3,1314952366,0,0,0,0,0,-2147483648,10003,0,3,1314952550,0,0,0,0,0,-2147483648,11040,0,1,1314953083,0,0,0,0,0,-2147483648', '0', '10', '0', '1157', '0', CURRENT_TIMESTAMP, CURRENT_TIMESTAMP, 'd41d8cd98f00b204e9800998ecf8427e', '0', '3', '0', '0', '0', '0', '1,0,', '0', '0', '02c68f0bba28', '0', 0x5C00050000000000000000000000F400F40000000000F40000000000CDCDCDCD0000F40000000000F40002000000000000000000FC94604E000000000100000000000000030000000600F897604E850400002200000000000000000000000000, '0', '28', '65', '0', '0', '0', '0', '0', '3', '1', '1', null, '0', null, '0', '0', '0', '1', '1', '1', '3', '5', '14', '0', '0', '0', '3810288', '', '1', '0', '3000', '0');
