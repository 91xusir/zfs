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
