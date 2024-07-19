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
