# MySQL-Front 5.1  (Build 3.57)

/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE */;
/*!40101 SET SQL_MODE='' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES */;
/*!40103 SET SQL_NOTES='ON' */;


# Host: 192.168.15.214    Database: account
# ------------------------------------------------------
# Server version 5.0.16

CREATE DATABASE `account` /*!40100 DEFAULT CHARACTER SET gb2312 */;
USE `account`;

#
# Source for table a_log
#

CREATE TABLE `a_log` (
  `id` bigint(20) NOT NULL auto_increment,
  `theType` char(20) NOT NULL,
  `theTypeKind` tinyint(11) NOT NULL default '1',
  `theDate` datetime NOT NULL,
  `theAccount` char(20) NOT NULL,
  `thePasswordOld` char(32) default NULL,
  `thePasswordNew` char(32) default NULL,
  `theNum` bigint(11) NOT NULL default '0',
  `theDepict` varchar(100) default NULL,
  `awflag` int(11) default NULL,
  `theNumBefore` bigint(20) NOT NULL,
  `theNumAfter` bigint(20) NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312 COMMENT='wtLog';

#
# Source for table card_number
#

CREATE TABLE `card_number` (
  `idx` int(11) NOT NULL auto_increment,
  `userid` int(11) NOT NULL default '0',
  `accountid` int(11) NOT NULL default '0',
  `cardid` varchar(16) NOT NULL default '',
  `type` int(11) NOT NULL,
  `attribute` varchar(300) NOT NULL default '',
  `usetime` datetime NOT NULL default '0000-00-00 00:00:00',
  `createtime` datetime NOT NULL default '0000-00-00 00:00:00',
  `userip` varchar(16) NOT NULL default '',
  `cardtrade` int(11) NOT NULL default '0',
  `cardidMD5` varchar(32) NOT NULL default '',
  PRIMARY KEY  (`idx`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table concurrent_log
#

CREATE TABLE `concurrent_log` (
  `idx` int(11) NOT NULL auto_increment,
  `log_time` datetime default '0000-00-00 00:00:00',
  `gw1` mediumint(9) NOT NULL default '0',
  `gw2` mediumint(9) NOT NULL default '0',
  `gw3` mediumint(9) NOT NULL default '0',
  `gw4` mediumint(9) NOT NULL default '0',
  `gw5` mediumint(9) NOT NULL default '0',
  `gw6` mediumint(9) NOT NULL default '0',
  `gw7` mediumint(9) NOT NULL default '0',
  `gw8` mediumint(9) NOT NULL default '0',
  PRIMARY KEY  (`idx`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table consume
#

CREATE TABLE `consume` (
  `idx` int(11) NOT NULL auto_increment,
  `accountID` int(11) NOT NULL default '0',
  `ip` varchar(20) NOT NULL default '',
  `enter_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `leave_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `Point` float NOT NULL default '0',
  `Server` int(11) NOT NULL default '0',
  PRIMARY KEY  (`idx`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table log_charge_card
#

CREATE TABLE `log_charge_card` (
  `idx` int(10) unsigned NOT NULL auto_increment,
  `log_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `cmdSN` int(10) unsigned NOT NULL default '0',
  `op` int(10) unsigned NOT NULL default '0',
  `cardType` int(10) unsigned NOT NULL default '0',
  `server` int(10) unsigned NOT NULL default '0',
  `accountID` int(10) unsigned NOT NULL default '0',
  `userID` int(10) unsigned NOT NULL default '0',
  `cardID` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`idx`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table order
#

CREATE TABLE `order` (
  `billingID` varchar(225) NOT NULL default '',
  `point` varchar(225) default NULL,
  `extrapoint` varchar(225) default NULL,
  `account` varchar(225) default NULL,
  `datetime` datetime NOT NULL default '1970-01-01 00:00:00'
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table user
#

CREATE TABLE `user` (
  `idx` int(11) NOT NULL auto_increment,
  `username` varchar(20) NOT NULL default '',
  `password` varchar(32) NOT NULL default '',
  `identity` varchar(225) NOT NULL default '',
  `deleted` smallint(6) NOT NULL default '0',
  `user_priv` mediumint(9) NOT NULL default '0',
  `locked` smallint(6) NOT NULL default '0',
  `lock_begin` datetime NOT NULL default '1970-01-01 00:00:00',
  `lock_end` datetime NOT NULL default '1970-01-01 00:00:00',
  `create_date` datetime NOT NULL default '1970-01-01 00:00:00',
  `from` int(11) NOT NULL default '0',
  `latest_login_date` datetime NOT NULL default '0000-00-00 00:00:00',
  `latest_login_ip` varchar(45) NOT NULL default '-',
  `datetime` datetime NOT NULL default '1970-01-01 00:00:00',
  `point` int(11) NOT NULL default '0',
  `isVip` int(11) NOT NULL default '0',
  `activation` tinyint(1) NOT NULL default '0',
  `badult` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`idx`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;



# MySQL-Front 5.1  (Build 3.57)

/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE */;
/*!40101 SET SQL_MODE='' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES */;
/*!40103 SET SQL_NOTES='ON' */;


# Host: 192.168.15.214    Database: activity
# ------------------------------------------------------
# Server version 5.0.16

CREATE DATABASE `activity` /*!40100 DEFAULT CHARACTER SET gb2312 */;
USE `activity`;

#
# Source for table enter_activity
#

CREATE TABLE `enter_activity` (
  `UserID` int(11) unsigned NOT NULL,
  `UserName` varchar(64) NOT NULL default '',
  `1` int(4) default '0',
  `2` int(4) default '0',
  `3` int(4) default '0',
  `4` int(4) default '0',
  UNIQUE KEY `UserID` (`UserID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table user_cachot
#

CREATE TABLE `user_cachot` (
  `UserID` int(11) NOT NULL default '0',
  `UserName` varchar(64) default NULL,
  `ActivityType` int(11) NOT NULL default '1',
  `JoinState` int(11) NOT NULL default '2',
  `RunDegree` int(11) NOT NULL default '0',
  `StateTime` varchar(32) default NULL COMMENT '��.��.��.ʱ.��',
  UNIQUE KEY `UserID` (`UserID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table user_hold_treasure
#

CREATE TABLE `user_hold_treasure` (
  `UserID` int(11) NOT NULL default '0',
  `UserName` varchar(64) default NULL,
  `ActivityType` int(11) NOT NULL default '1',
  `JoinState` int(11) NOT NULL default '2',
  `StateTime` varchar(32) default NULL COMMENT '��.��.��.ʱ.��',
  UNIQUE KEY `UserID` (`UserID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;



# MySQL-Front 5.1  (Build 3.57)

/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE */;
/*!40101 SET SQL_MODE='' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES */;
/*!40103 SET SQL_NOTES='ON' */;


# Host: 192.168.15.214    Database: analyse
# ------------------------------------------------------
# Server version 5.0.16

CREATE DATABASE `analyse` /*!40100 DEFAULT CHARACTER SET gb2312 */;
USE `analyse`;

#
# Source for table items
#

CREATE TABLE `items` (
  `Id` int(11) NOT NULL auto_increment,
  `UserID` int(11) NOT NULL default '0',
  `ItemID` int(11) NOT NULL default '0',
  `type` int(11) NOT NULL default '0' COMMENT '������',
  `subtype` int(11) NOT NULL default '0' COMMENT '������',
  `ItemIndex` int(11) NOT NULL default '0' COMMENT '��Ʒ���е�ID',
  `color` tinyint(3) NOT NULL default '0' COMMENT '���ߵ���ɫ(��ɫװ������ɫװ���ͻƽ�װ��)',
  `level` smallint(6) NOT NULL default '0' COMMENT '����ȼ�(���10��,index��0��ʼ)',
  `Bind` tinyint(3) default NULL COMMENT '��',
  `Lock` bit(1) default NULL,
  `Countable` bit(1) default NULL COMMENT '�Ƿ����(������ʱʹ��)',
  `Enchant` bit(1) default NULL COMMENT '�Ƿ�ħ(������ʱʹ��)',
  `TimeLimit` bit(1) default NULL COMMENT '�Ƿ���ʱ������(������ʱʹ��)',
  `attribID1` int(11) default NULL COMMENT '��ɫװ���������ӵ�����ID1',
  `attribID2` int(11) default NULL COMMENT '��ɫװ���������ӵ�����ID2',
  `attribRate1` int(11) default NULL COMMENT '��ɫװ������ID�ļӳɱ���1',
  `attribRate2` int(11) default NULL COMMENT '��ɫװ������ID�ļӳɱ���2',
  `count` int(11) default NULL COMMENT 'ҩƿ��ʣ������',
  `enchantAttrib` int(11) default NULL COMMENT '��ħ������ID',
  `ItemTimeLimit` int(11) default NULL COMMENT '����ʱ��',
  `curWear` int(11) default NULL COMMENT '��ǰ�;�',
  `Valid` int(11) default NULL COMMENT '�Ƿ���Լӳ�����',
  `name` varchar(255) default NULL,
  `paramCount` int(11) default '0' COMMENT '��������',
  `params1` int(11) default NULL,
  `params2` int(11) default NULL,
  `params3` int(11) default NULL,
  `params4` int(11) default NULL,
  `params5` int(11) default NULL,
  `params6` int(11) default NULL,
  `params7` int(11) default NULL,
  `params8` int(11) default NULL,
  `params9` int(11) default NULL,
  `params10` int(11) default NULL,
  `params11` int(11) default NULL,
  `params12` int(11) default NULL,
  `params13` int(11) default NULL,
  `params14` int(11) default NULL,
  `params15` int(11) default NULL,
  `params16` int(11) default NULL,
  `params17` int(11) default NULL,
  `params18` int(11) default NULL,
  `params19` int(11) default NULL,
  `params20` int(11) default NULL,
  `params21` int(11) default NULL,
  `params22` int(11) default NULL,
  `params23` int(11) default NULL,
  `params24` int(11) default NULL,
  `params25` int(11) default NULL,
  `params26` int(11) default NULL,
  `params27` int(11) default NULL,
  `params28` int(11) default NULL,
  `params29` int(11) default NULL,
  `params30` int(11) default NULL,
  `params31` int(11) default NULL,
  `params32` int(11) default NULL,
  `params33` int(11) default NULL,
  `params34` int(11) default NULL,
  `params35` int(11) default NULL,
  `params36` int(11) default NULL,
  `params37` int(11) default NULL,
  `params38` int(11) default NULL,
  `params39` int(11) default NULL,
  `params40` int(11) default NULL,
  `params41` int(11) default NULL,
  `params42` int(11) default NULL,
  `params43` int(11) default NULL,
  `params44` int(11) default NULL,
  `params45` int(11) default NULL,
  `params46` int(11) default NULL,
  `params47` int(11) default NULL,
  `params48` int(11) default NULL,
  `params49` int(11) default NULL,
  `params50` int(11) default NULL,
  PRIMARY KEY  (`Id`),
  KEY `UserID` (`UserID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table lastupdatetime
#

CREATE TABLE `lastupdatetime` (
  `Id` int(11) NOT NULL auto_increment,
  `LastUpdateDate` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`Id`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;



# MySQL-Front 5.1  (Build 3.57)

/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE */;
/*!40101 SET SQL_MODE='' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES */;
/*!40103 SET SQL_NOTES='ON' */;


# Host: 192.168.15.214    Database: fso_log
# ------------------------------------------------------
# Server version 5.0.16

CREATE DATABASE `fso_log` /*!40100 DEFAULT CHARACTER SET gb2312 */;
USE `fso_log`;

#
# Source for table t_log_user_history
#

CREATE TABLE `t_log_user_history` (
  `OperateID` int(11) NOT NULL auto_increment,
  `type` int(11) NOT NULL default '0',
  `userID` int(11) NOT NULL default '0',
  `longParam1` int(11) NOT NULL default '0',
  `strParam1` longtext NOT NULL,
  `longParam2` int(11) NOT NULL default '0',
  `strParam2` mediumtext NOT NULL,
  `longParam3` int(11) NOT NULL default '0',
  `strParam3` longtext NOT NULL,
  `LogTime` datetime NOT NULL default '0000-00-00 00:00:00',
  `userIP` varchar(32) NOT NULL default '',
  PRIMARY KEY  (`OperateID`),
  KEY `type` (`type`),
  KEY `type_longParam1` (`type`,`longParam1`),
  KEY `type_longParam1_userID` (`type`,`longParam1`,`userID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table t_log_user_history1
#

CREATE TABLE `t_log_user_history1` (
  `OperateID` int(11) NOT NULL auto_increment,
  `type` int(11) NOT NULL default '0',
  `userID` int(11) NOT NULL default '0',
  `longParam1` int(11) NOT NULL default '0',
  `strParam1` longtext NOT NULL,
  `longParam2` int(11) NOT NULL default '0',
  `strParam2` longtext NOT NULL,
  `longParam3` int(11) NOT NULL default '0',
  `strParam3` longtext NOT NULL,
  `LogTime` datetime NOT NULL default '0000-00-00 00:00:00',
  `userIP` varchar(32) NOT NULL default '',
  PRIMARY KEY  (`OperateID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table t_log_user_history10
#

CREATE TABLE `t_log_user_history10` (
  `OperateID` int(11) NOT NULL auto_increment,
  `type` int(11) NOT NULL default '0',
  `userID` int(11) NOT NULL default '0',
  `longParam1` int(11) NOT NULL default '0',
  `strParam1` longtext NOT NULL,
  `longParam2` int(11) NOT NULL default '0',
  `strParam2` longtext NOT NULL,
  `longParam3` int(11) NOT NULL default '0',
  `strParam3` longtext NOT NULL,
  `LogTime` datetime NOT NULL default '0000-00-00 00:00:00',
  `userIP` varchar(32) NOT NULL default '',
  PRIMARY KEY  (`OperateID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table t_log_user_history2
#

CREATE TABLE `t_log_user_history2` (
  `OperateID` int(11) NOT NULL auto_increment,
  `type` int(11) NOT NULL default '0',
  `userID` int(11) NOT NULL default '0',
  `longParam1` int(11) NOT NULL default '0',
  `strParam1` longtext NOT NULL,
  `longParam2` int(11) NOT NULL default '0',
  `strParam2` longtext NOT NULL,
  `longParam3` int(11) NOT NULL default '0',
  `strParam3` longtext NOT NULL,
  `LogTime` datetime NOT NULL default '0000-00-00 00:00:00',
  `userIP` varchar(32) NOT NULL default '',
  PRIMARY KEY  (`OperateID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table t_log_user_history3
#

CREATE TABLE `t_log_user_history3` (
  `OperateID` int(11) NOT NULL auto_increment,
  `type` int(11) NOT NULL default '0',
  `userID` int(11) NOT NULL default '0',
  `longParam1` int(11) NOT NULL default '0',
  `strParam1` longtext NOT NULL,
  `longParam2` int(11) NOT NULL default '0',
  `strParam2` longtext NOT NULL,
  `longParam3` int(11) NOT NULL default '0',
  `strParam3` longtext NOT NULL,
  `LogTime` datetime NOT NULL default '0000-00-00 00:00:00',
  `userIP` varchar(32) NOT NULL default '',
  PRIMARY KEY  (`OperateID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table t_log_user_history4
#

CREATE TABLE `t_log_user_history4` (
  `OperateID` int(11) NOT NULL auto_increment,
  `type` int(11) NOT NULL default '0',
  `userID` int(11) NOT NULL default '0',
  `longParam1` int(11) NOT NULL default '0',
  `strParam1` longtext NOT NULL,
  `longParam2` int(11) NOT NULL default '0',
  `strParam2` longtext NOT NULL,
  `longParam3` int(11) NOT NULL default '0',
  `strParam3` mediumtext NOT NULL,
  `LogTime` datetime NOT NULL default '0000-00-00 00:00:00',
  `userIP` varchar(32) NOT NULL default '',
  PRIMARY KEY  (`OperateID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table t_log_user_history5
#

CREATE TABLE `t_log_user_history5` (
  `OperateID` int(11) NOT NULL auto_increment,
  `type` int(11) NOT NULL default '0',
  `userID` int(11) NOT NULL default '0',
  `longParam1` int(11) NOT NULL default '0',
  `strParam1` longtext NOT NULL,
  `longParam2` int(11) NOT NULL default '0',
  `strParam2` mediumtext NOT NULL,
  `longParam3` int(11) NOT NULL default '0',
  `strParam3` mediumtext NOT NULL,
  `LogTime` datetime NOT NULL default '0000-00-00 00:00:00',
  `userIP` varchar(32) NOT NULL default '',
  PRIMARY KEY  (`OperateID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table t_log_user_history6
#

CREATE TABLE `t_log_user_history6` (
  `OperateID` int(11) NOT NULL auto_increment,
  `type` int(11) NOT NULL default '0',
  `userID` int(11) NOT NULL default '0',
  `longParam1` int(11) NOT NULL default '0',
  `strParam1` mediumtext NOT NULL,
  `longParam2` int(11) NOT NULL default '0',
  `strParam2` mediumtext NOT NULL,
  `longParam3` int(11) NOT NULL default '0',
  `strParam3` mediumtext NOT NULL,
  `LogTime` datetime NOT NULL default '0000-00-00 00:00:00',
  `userIP` varchar(32) NOT NULL default '',
  PRIMARY KEY  (`OperateID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table t_log_user_history7
#

CREATE TABLE `t_log_user_history7` (
  `OperateID` int(11) NOT NULL auto_increment,
  `type` int(11) NOT NULL default '0',
  `userID` int(11) NOT NULL default '0',
  `longParam1` int(11) NOT NULL default '0',
  `strParam1` mediumtext NOT NULL,
  `longParam2` int(11) NOT NULL default '0',
  `strParam2` mediumtext NOT NULL,
  `longParam3` int(11) NOT NULL default '0',
  `strParam3` mediumtext NOT NULL,
  `LogTime` datetime NOT NULL default '0000-00-00 00:00:00',
  `userIP` varchar(32) NOT NULL default '',
  PRIMARY KEY  (`OperateID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table t_log_user_history8
#

CREATE TABLE `t_log_user_history8` (
  `OperateID` int(11) NOT NULL auto_increment,
  `type` int(11) NOT NULL default '0',
  `userID` int(11) NOT NULL default '0',
  `longParam1` int(11) NOT NULL default '0',
  `strParam1` mediumtext NOT NULL,
  `longParam2` int(11) NOT NULL default '0',
  `strParam2` mediumtext NOT NULL,
  `longParam3` int(11) NOT NULL default '0',
  `strParam3` mediumtext NOT NULL,
  `LogTime` datetime NOT NULL default '0000-00-00 00:00:00',
  `userIP` varchar(32) NOT NULL default '',
  PRIMARY KEY  (`OperateID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table t_log_user_history9
#

CREATE TABLE `t_log_user_history9` (
  `OperateID` int(11) NOT NULL auto_increment,
  `type` int(11) NOT NULL default '0',
  `userID` int(11) NOT NULL default '0',
  `longParam1` int(11) NOT NULL default '0',
  `strParam1` mediumtext NOT NULL,
  `longParam2` int(11) NOT NULL default '0',
  `strParam2` mediumtext NOT NULL,
  `longParam3` int(11) NOT NULL default '0',
  `strParam3` mediumtext NOT NULL,
  `LogTime` datetime NOT NULL default '0000-00-00 00:00:00',
  `userIP` varchar(32) NOT NULL default '',
  PRIMARY KEY  (`OperateID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

#
# Source for table t_log_user_onlineamount
#

CREATE TABLE `t_log_user_onlineamount` (
  `id` bigint(20) NOT NULL auto_increment,
  `OnlineAmount` int(32) default '0',
  `LogOnlineTime` datetime default '0000-00-00 00:00:00',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `LogOnlineTime` USING BTREE (`LogOnlineTime`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312 ROW_FORMAT=COMPACT;

/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;



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
  `Pets` longtext NOT NULL COMMENT '����ֿ�',
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
  `Pets` longtext NOT NULL COMMENT '���ﱳ��',
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
  `userID` int(11) NOT NULL default '0' COMMENT '�û�id',
  `official` int(4) NOT NULL default '0' COMMENT '��ְ',
  `UnionID` int(11) NOT NULL default '0' COMMENT '����id',
  `Contribution` int(11) NOT NULL default '0' COMMENT '���ɹ���',
  `Fighting` int(11) NOT NULL default '0' COMMENT 'ս����'
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
  `c_BornTag` int(4) default '0' COMMENT '��Ф',
  `Sex` int(4) NOT NULL default '0' COMMENT '��ɫ�Ա� Ĭ��Ϊ��',
  `TeamFree` int(2) default '1',
  `AreaHonors` longtext NOT NULL COMMENT '���������Ԥ��128��',
  `Stage` int(4) default '0' COMMENT '�׶�',
  `RelivePostion` longtext NOT NULL COMMENT '�󶨸����',
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



# MySQL-Front 5.1  (Build 3.57)

/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE */;
/*!40101 SET SQL_MODE='' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES */;
/*!40103 SET SQL_NOTES='ON' */;


# Host: 192.168.15.175    Database: fso
# ------------------------------------------------------
# Server version 5.0.51a-3ubuntu5.4-log

USE `fso`;

#
# Dumping data for table t_castlewar_time_set
#

INSERT INTO `t_castlewar_time_set` VALUES ('ChuanYun',0,0,0,0,0);
INSERT INTO `t_castlewar_time_set` VALUES ('XiQi',0,0,0,0,0);
INSERT INTO `t_castlewar_time_set` VALUES ('ZhaoGe',0,0,0,0,0);

/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;



# MySQL-Front 5.1  (Build 3.57)

/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE */;
/*!40101 SET SQL_MODE='' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES */;
/*!40103 SET SQL_NOTES='ON' */;


# Host: 192.168.15.175    Database: fso
# ------------------------------------------------------
# Server version 5.0.51a-3ubuntu5.4-log

USE `fso`;

#
# Dumping data for table t_dungeonid
#

INSERT INTO `t_dungeonid` VALUES (122);

/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;



# MySQL-Front 5.1  (Build 3.57)

/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE */;
/*!40101 SET SQL_MODE='' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES */;
/*!40103 SET SQL_NOTES='ON' */;


# Host: 192.168.15.175    Database: fso
# ------------------------------------------------------
# Server version 5.0.51a-3ubuntu5.4-log

USE `fso`;

#
# Dumping data for table t_itemid
#

INSERT INTO `t_itemid` VALUES (0,2449001,0,134217727);
INSERT INTO `t_itemid` VALUES (1,134219728,0,268435454);
INSERT INTO `t_itemid` VALUES (2,268435455,0,402653181);
INSERT INTO `t_itemid` VALUES (3,402653182,0,536870908);
INSERT INTO `t_itemid` VALUES (4,536870909,0,671088635);
INSERT INTO `t_itemid` VALUES (5,671088636,0,805306362);
INSERT INTO `t_itemid` VALUES (6,805306363,0,939524089);
INSERT INTO `t_itemid` VALUES (7,939524090,0,1073741816);
INSERT INTO `t_itemid` VALUES (8,1073741817,0,1207959543);
INSERT INTO `t_itemid` VALUES (9,1207959544,0,1342177270);
INSERT INTO `t_itemid` VALUES (10,1342177271,0,1476394997);
INSERT INTO `t_itemid` VALUES (11,1476394998,0,1610612724);
INSERT INTO `t_itemid` VALUES (12,1610612725,0,1744830451);
INSERT INTO `t_itemid` VALUES (13,1744830452,0,1879048178);
INSERT INTO `t_itemid` VALUES (14,1879048179,0,2013265905);
INSERT INTO `t_itemid` VALUES (15,2013265906,0,2147483632);
INSERT INTO `t_itemid` VALUES (16,2147483633,0,-2013265937);
INSERT INTO `t_itemid` VALUES (17,-2013265936,0,-1879048210);
INSERT INTO `t_itemid` VALUES (18,-1879048209,0,-1744830483);
INSERT INTO `t_itemid` VALUES (19,-1744830482,0,-1610612756);
INSERT INTO `t_itemid` VALUES (20,-1610612755,0,-1476395029);
INSERT INTO `t_itemid` VALUES (21,-1476395028,0,-1342177302);
INSERT INTO `t_itemid` VALUES (22,-1342177301,0,-1207959575);
INSERT INTO `t_itemid` VALUES (23,-1207959574,0,-1073741848);
INSERT INTO `t_itemid` VALUES (24,-1073741847,0,-939524121);
INSERT INTO `t_itemid` VALUES (25,-939524120,0,-805306394);
INSERT INTO `t_itemid` VALUES (26,-805306393,0,-671088667);
INSERT INTO `t_itemid` VALUES (27,-671088666,0,-536870940);
INSERT INTO `t_itemid` VALUES (28,-536870939,0,-402653213);
INSERT INTO `t_itemid` VALUES (29,-402653212,0,-268435486);
INSERT INTO `t_itemid` VALUES (30,-268435485,0,-134217759);
INSERT INTO `t_itemid` VALUES (31,-134217758,0,-32);
INSERT INTO `t_itemid` VALUES (254,48165101,0,69675675);
INSERT INTO `t_itemid` VALUES (255,1231314091,0,1312312312);

/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;



