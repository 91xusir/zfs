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
  `StateTime` varchar(32) default NULL COMMENT '年.月.日.时.分',
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
  `StateTime` varchar(32) default NULL COMMENT '年.月.日.时.分',
  UNIQUE KEY `UserID` (`UserID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
