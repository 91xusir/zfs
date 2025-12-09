# MySQL-Front 5.1  (Build 3.57)

/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE */;
/*!40101 SET SQL_MODE='' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES */;
/*!40103 SET SQL_NOTES='ON' */;


# Host: 192.168.15.214    Database: fso
# ------------------------------------------------------
# Server version 5.0.16

CREATE DATABASE `fso` /*!40100 DEFAULT CHARACTER SET gb2312 */;
USE `fso`;

#
# Source for table community
#

CREATE TABLE `community` (
  `UserID` int(11) NOT NULL default '0',
  `Friend` text,
  `GuildID` int(11) default '0',
  PRIMARY KEY  (`UserID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table mail
#

CREATE TABLE `mail` (
  `ID` int(11) NOT NULL auto_increment,
  `ReceiverID` int(11) NOT NULL default '0',
  `SenderID` int(11) NOT NULL default '0',
  `StrItemID` longtext NOT NULL,
  `Money` int(11) NOT NULL default '0',
  `IsOpen` int(11) NOT NULL default '0',
  `ReceivedDate` datetime NOT NULL default '0000-00-00 00:00:00',
  `Title` longtext NOT NULL,
  `Content` longtext NOT NULL,
  `ReceiverName` varchar(32) NOT NULL default '',
  `SenderName` varchar(32) NOT NULL default '',
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312 ROW_FORMAT=COMPACT;

#
# Source for table py_game_op
#

CREATE TABLE `py_game_op` (
  `cmdid` int(11) NOT NULL auto_increment,
  `cmd` varchar(50) NOT NULL default '',
  `gm_account` int(11) NOT NULL,
  `result` int(1) NOT NULL,
  `toUserName` varchar(32) NOT NULL default '',
  PRIMARY KEY  (`cmdid`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table t_auction
#

CREATE TABLE `t_auction` (
  `Index` int(10) unsigned NOT NULL auto_increment,
  `ItemUID` int(11) NOT NULL default '0',
  `ItemIDStr` longtext NOT NULL,
  `ItemVer` int(11) NOT NULL default '0',
  `BargainorAcc` int(11) NOT NULL default '0',
  `BargainorChrID` int(11) NOT NULL default '0',
  `BargainorChr` varchar(45) NOT NULL default '',
  `PurchaserAcc` int(11) NOT NULL default '0',
  `PurchaserChrID` int(11) NOT NULL default '0',
  `PurchaserChr` varchar(45) NOT NULL default '',
  `PassTime` int(11) NOT NULL default '0',
  `Duration` int(11) NOT NULL default '0',
  `HandlingCharge` int(11) NOT NULL default '0',
  `BasePrice` int(11) NOT NULL default '0',
  `BidPrice` int(11) NOT NULL default '0',
  `UniquePrice` int(11) NOT NULL default '0',
  PRIMARY KEY  (`Index`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table t_auction_dealing
#

CREATE TABLE `t_auction_dealing` (
  `Index` int(10) unsigned NOT NULL auto_increment,
  `ItemID` longtext NOT NULL,
  `ItemVer` int(11) NOT NULL default '0',
  `BargainorAcc` int(11) NOT NULL default '0',
  `BargainorChrID` int(11) NOT NULL default '0',
  `BargainorChr` varchar(45) NOT NULL default '',
  `PurchaserAcc` int(11) NOT NULL default '0',
  `PurchaserChrID` int(11) NOT NULL default '0',
  `PurchaserChr` varchar(45) NOT NULL default '',
  `PassTime` int(11) NOT NULL default '0',
  `Duration` int(11) NOT NULL default '0',
  `HandlingCharge` int(11) NOT NULL default '0',
  `BasePrice` int(11) NOT NULL default '0',
  `BidPrice` int(11) NOT NULL default '0',
  `UniquePrice` int(11) NOT NULL default '0',
  PRIMARY KEY  (`Index`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table t_auction_log
#

CREATE TABLE `t_auction_log` (
  `Index` int(10) unsigned NOT NULL auto_increment,
  `Type` int(11) NOT NULL default '0',
  `ItemID` longtext NOT NULL,
  `ItemVer` int(11) NOT NULL default '0',
  `BargainorAcc` int(11) NOT NULL default '0',
  `BargainorChrID` int(11) NOT NULL default '0',
  `BargainorChr` varchar(45) NOT NULL default '',
  `PurchaserAcc` int(11) NOT NULL default '0',
  `PurchaserChrID` int(11) NOT NULL default '0',
  `PurchaserChr` varchar(45) NOT NULL default '',
  `PassTime` int(11) NOT NULL default '0',
  `Duration` int(11) NOT NULL default '0',
  `HandlingCharge` int(11) NOT NULL default '0',
  `BasePrice` int(11) NOT NULL default '0',
  `BidPrice` int(11) NOT NULL default '0',
  `UniquePrice` int(11) NOT NULL default '0',
  `Time` int(11) NOT NULL default '0',
  PRIMARY KEY  (`Index`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table t_auction_result
#

CREATE TABLE `t_auction_result` (
  `Index` int(10) unsigned NOT NULL auto_increment,
  `Type` int(11) NOT NULL default '0',
  `ItemID` varchar(255) NOT NULL default '',
  `ItemVer` longtext NOT NULL,
  `Money` int(11) NOT NULL default '0',
  `ChrID` int(11) NOT NULL default '0',
  `Time` int(11) NOT NULL default '0',
  PRIMARY KEY  (`Index`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table t_bank
#

CREATE TABLE `t_bank` (
  `UserID` int(11) NOT NULL auto_increment,
  `AccountID` int(10) NOT NULL default '0',
  `Account` varchar(45) NOT NULL default '',
  `Items` longtext NOT NULL,
  `BankMoney` int(11) NOT NULL default '0',
  `Pets` longtext NOT NULL COMMENT '宠物仓库',
  `Password` varchar(45) NOT NULL default '',
  PRIMARY KEY  (`UserID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table t_cardtrade
#

CREATE TABLE `t_cardtrade` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `CardType` int(11) NOT NULL default '0',
  `GroupType` int(11) NOT NULL default '0',
  `CardID` varchar(45) NOT NULL default '',
  `CardPwd` varchar(45) NOT NULL default '',
  `CardInfo` varchar(45) NOT NULL default '',
  `UserID` int(11) NOT NULL default '0',
  `UserName` varchar(45) NOT NULL default '',
  `AccountID` int(11) NOT NULL default '0',
  `AccountName` varchar(45) NOT NULL default '',
  `Price` varchar(45) NOT NULL default '',
  `StartTime` datetime NOT NULL default '0000-00-00 00:00:00',
  `EndTime` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table t_cardtradelog
#

CREATE TABLE `t_cardtradelog` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `OpType` int(11) NOT NULL default '0',
  `CardID` varchar(45) NOT NULL default '',
  `UserID` int(11) NOT NULL default '0',
  `Price` int(11) NOT NULL default '0',
  `opTime` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table t_castlewar_info
#

CREATE TABLE `t_castlewar_info` (
  `ID` int(11) NOT NULL auto_increment,
  `FeodName` varchar(45) NOT NULL default '',
  `UnionID` int(11) NOT NULL default '0',
  `StartTime` int(11) NOT NULL default '0',
  `UnionName` varchar(45) NOT NULL default '',
  `VassalName` varchar(45) NOT NULL default '',
  `FlagID` int(11) NOT NULL default '0',
  `IsAttacker` int(11) NOT NULL default '0',
  `IsAllowed` int(11) NOT NULL default '0',
  `AnimalDead` int(11) NOT NULL default '0',
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table t_castlewar_time_set
#

CREATE TABLE `t_castlewar_time_set` (
  `FeodName` varchar(32) NOT NULL default '',
  `DefWeekDay` int(11) NOT NULL,
  `DefHour` int(11) NOT NULL,
  `DefMinute` int(11) NOT NULL,
  `DefDuration` int(11) NOT NULL,
  `LatestTime` int(11) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=gb2312 ROW_FORMAT=COMPACT;

#
# Source for table t_createvote
#

CREATE TABLE `t_createvote` (
  `VoteID` int(11) NOT NULL auto_increment,
  `c_Creater` int(4) unsigned NOT NULL default '0',
  `Recall` int(4) unsigned NOT NULL default '0',
  `EndTime` datetime NOT NULL default '0000-00-00 00:00:00',
  `UnionID` int(8) NOT NULL default '0',
  PRIMARY KEY  (`VoteID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table t_dungeonid
#

CREATE TABLE `t_dungeonid` (
  `LastID` int(10) unsigned NOT NULL default '1',
  PRIMARY KEY  (`LastID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312 ROW_FORMAT=COMPACT;

#
# Source for table t_feod
#

CREATE TABLE `t_feod` (
  `Name` varchar(32) character set latin1 NOT NULL default '',
  `CurrentUnion` int(11) NOT NULL default '0',
  `TaxRate` float NOT NULL default '0',
  `Bank` int(11) NOT NULL default '0',
  `LastDayIncome` int(11) NOT NULL default '0',
  `LastDay` int(11) NOT NULL default '0',
  PRIMARY KEY  (`Name`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table t_gift
#

CREATE TABLE `t_gift` (
  `idx` int(10) unsigned NOT NULL auto_increment,
  `charID` int(11) NOT NULL default '0',
  `level` int(11) NOT NULL default '0',
  `type` int(11) NOT NULL default '0',
  `attribute` varchar(300) NOT NULL default '',
  `addTime` datetime NOT NULL default '0000-00-00 00:00:00',
  `limitTime` datetime NOT NULL default '0000-00-00 00:00:00',
  `msg` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`idx`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table t_itemid
#

CREATE TABLE `t_itemid` (
  `RegionID` int(11) NOT NULL default '0',
  `ItemID` int(15) NOT NULL default '0',
  `BeginID` int(11) NOT NULL default '0',
  `MaxID` int(15) NOT NULL default '0',
  PRIMARY KEY  (`RegionID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table t_masterprentice
#

CREATE TABLE `t_masterprentice` (
  `Idx` int(10) unsigned NOT NULL auto_increment,
  `MasterID` int(11) NOT NULL default '0',
  `MasterName` varchar(45) NOT NULL default '',
  `PrenticeID` int(11) NOT NULL default '0',
  `PrenticeName` varchar(45) NOT NULL default '',
  `FinishTime` datetime NOT NULL default '0000-00-00 00:00:00',
  `CreditToAdd` int(11) NOT NULL default '0',
  `FinishOnlineTime` int(11) NOT NULL default '0',
  PRIMARY KEY  (`Idx`),
  KEY `MasterID` (`MasterID`),
  KEY `PrenticeID` (`PrenticeID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table t_pet
#

CREATE TABLE `t_pet` (
  `UserID` int(11) NOT NULL auto_increment,
  `AccountID` int(11) NOT NULL default '0',
  `Name` varchar(45) NOT NULL default '',
  `Pets` longtext NOT NULL COMMENT '宠物背包',
  PRIMARY KEY  (`UserID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312 ROW_FORMAT=COMPACT;

#
# Source for table t_union
#

CREATE TABLE `t_union` (
  `ID` int(11) NOT NULL auto_increment,
  `Name` varchar(32) NOT NULL default '',
  `Level` int(4) unsigned NOT NULL default '0',
  `Enounce` longtext,
  `Bulletin` longtext,
  `Icon` varchar(32) NOT NULL default '0',
  `CaptainID` int(11) NOT NULL default '0',
  `IconData` longblob,
  `ToBeDeletedBegin` datetime default NULL,
  `Score` int(11) NOT NULL default '0',
  `Money` int(15) NOT NULL default '0',
  `Item1` int(8) NOT NULL default '0',
  `Item2` int(8) NOT NULL default '0',
  `Item3` int(8) NOT NULL default '0',
  `Item4` int(8) NOT NULL default '0',
  PRIMARY KEY  (`ID`),
  UNIQUE KEY `Name` (`Name`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312 ROW_FORMAT=COMPACT;

#
# Source for table t_union_citizen
#

CREATE TABLE `t_union_citizen` (
  `userID` int(11) NOT NULL default '0',
  `official` int(4) NOT NULL default '0',
  `name` varchar(32) NOT NULL default '',
  `nickname` varchar(32) default NULL,
  `UnionID` int(11) NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table t_union_citizen_back
#

CREATE TABLE `t_union_citizen_back` (
  `userID` int(11) NOT NULL default '0' COMMENT '用户id',
  `official` int(4) NOT NULL default '0' COMMENT '官职',
  `UnionID` int(11) NOT NULL default '0' COMMENT '帮派id',
  `Contribution` int(11) NOT NULL default '0' COMMENT '帮派贡献',
  `Fighting` int(11) NOT NULL default '0' COMMENT '战斗力'
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table t_vote
#

CREATE TABLE `t_vote` (
  `VID` int(11) NOT NULL auto_increment,
  `VoteID` int(11) NOT NULL,
  `VoterID` int(4) unsigned NOT NULL default '0',
  `VState` int(4) unsigned NOT NULL default '0',
  `VoteTime` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`VID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table t_votelog
#

CREATE TABLE `t_votelog` (
  `LogID` int(11) NOT NULL auto_increment,
  `LogType` int(11) NOT NULL,
  `LogData` varchar(45) NOT NULL default '',
  `LogTime` datetime NOT NULL default '0000-00-00 00:00:00',
  `UnionID` int(8) NOT NULL default '0',
  PRIMARY KEY  (`LogID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table user
#

CREATE TABLE `user` (
  `ID` int(11) NOT NULL auto_increment,
  `UniqueName` varchar(32) NOT NULL default '',
  `Name` varchar(32) NOT NULL default '',
  `ActorID` int(11) NOT NULL default '0',
  `AccountID` int(11) NOT NULL default '0',
  `Account` varchar(20) NOT NULL default '',
  `Scene` varchar(32) NOT NULL default '',
  `HP` int(11) NOT NULL default '0',
  `MP` int(11) NOT NULL default '0',
  `EXP` int(11) NOT NULL default '0',
  `AP` int(11) NOT NULL default '0',
  `Level` int(11) NOT NULL default '0',
  `DeleteDate` datetime default '0000-00-00 00:00:00',
  `CreateDate` datetime NOT NULL default '0000-00-00 00:00:00',
  `LastUpdateDate` datetime NOT NULL default '0000-00-00 00:00:00',
  `Metier` int(11) NOT NULL default '0',
  `MetierLevel` int(11) NOT NULL default '0',
  `X` int(11) NOT NULL default '0',
  `Y` int(11) NOT NULL default '0',
  `Gold` int(11) NOT NULL default '0',
  `SP` int(11) default '0',
  `Str` smallint(6) NOT NULL default '0',
  `Con` smallint(6) NOT NULL default '0',
  `Dex` smallint(6) NOT NULL default '0',
  `Int` smallint(6) NOT NULL default '0',
  `Hit` smallint(6) NOT NULL default '0',
  `Skills` longtext,
  `Items` longtext,
  `Shortcut` longtext,
  `Config` longtext,
  `PK` int(11) NOT NULL default '0',
  `Task` longtext,
  `Score` int(11) NOT NULL default '0',
  `HeadModelID` int(11) NOT NULL default '0',
  `Credit` int(11) NOT NULL default '0',
  `OnlineTime` int(11) NOT NULL default '0',
  `usertype` smallint(4) NOT NULL default '0',
  `LastDelUnionTime` datetime NOT NULL default '0000-00-00 00:00:00',
  `LastRemoveMPTime` datetime NOT NULL default '0000-00-00 00:00:00',
  `Password` varchar(45) NOT NULL default '',
  `deleted` int(11) NOT NULL default '0',
  `Faction` smallint(5) NOT NULL default '0',
  `ClearCnt` int(11) default '0',
  `DungeonTID` int(11) default '0',
  `DungeonUID` int(11) default '0',
  `DungeonQuit` int(11) default '0',
  `DungeonTask` longtext,
  `InWarfield` int(11) default '0',
  `WarfieldQuit` int(11) default '0',
  `UsData` longtext,
  `WithdrawMPGift` int(10) unsigned NOT NULL default '0',
  `MyData` longblob,
  `IsOnline` tinyint(1) NOT NULL default '0',
  `c_Cul` int(16) default '0',
  `c_Soul` int(11) default '0',
  `c_Contribute` int(16) default '0',
  `c_Point` int(16) default '0',
  `c_Record` int(16) default '0',
  `c_UnionRecord` int(16) default '0',
  `c_JobRecord` int(16) default '0',
  `c_BornTag` int(4) default '0' COMMENT '生肖',
  `Sex` int(4) NOT NULL default '0' COMMENT '角色性别 默认为男',
  `TeamFree` int(2) default '1',
  `AreaHonors` longtext NOT NULL COMMENT '区域的声望预留128个',
  `Stage` int(4) default '0' COMMENT '阶段',
  `RelivePostion` longtext NOT NULL COMMENT '绑定复活点',
  `Vigour` int(16) default '0' COMMENT '???',
  `VigourPoint` int(16) default '0' COMMENT '???',
  `HasPassword` int(1) default '0',
  `LineSysGroupID` int(11) NOT NULL default '0',
  `LineSysLineID` int(11) NOT NULL default '0',
  `nRelivePostion` int(4) default '0',
  `GradeLev` int(11) NOT NULL default '0',
  `PlayerOnlineTime` int(11) NOT NULL default '0',
  `CulExp` int(11) NOT NULL default '0',
  `Exploit` int(11) NOT NULL default '0',
  `EnP` int(11) NOT NULL default '0',
  `HeadImageID` int(11) NOT NULL default '0',
  `petItemID` int(11) NOT NULL default '0',
  `Popularity` longtext NOT NULL,
  `isFirstLogin` tinyint(3) NOT NULL default '0',
  `IsDead` tinyint(3) default '0',
  `EffectSkill` longtext NOT NULL,
  `ChangeModelID` int(11) NOT NULL default '0',
  PRIMARY KEY  (`ID`),
  UNIQUE KEY `UniqueName` (`UniqueName`),
  KEY `ID` (`ID`),
  KEY `Account` (`Account`),
  KEY `AccountID` (`AccountID`),
  KEY `LastUpdateDate` (`LastUpdateDate`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
