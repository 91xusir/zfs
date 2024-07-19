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
  `type` int(11) NOT NULL default '0' COMMENT '大类型',
  `subtype` int(11) NOT NULL default '0' COMMENT '子类型',
  `ItemIndex` int(11) NOT NULL default '0' COMMENT '物品表中的ID',
  `color` tinyint(3) NOT NULL default '0' COMMENT '道具的颜色(白色装备，蓝色装备和黄金装备)',
  `level` smallint(6) NOT NULL default '0' COMMENT '锻造等级(最多10级,index从0开始)',
  `Bind` tinyint(3) default NULL COMMENT '绑定',
  `Lock` bit(1) default NULL,
  `Countable` bit(1) default NULL COMMENT '是否可数(仅存盘时使用)',
  `Enchant` bit(1) default NULL COMMENT '是否附魔(仅存盘时使用)',
  `TimeLimit` bit(1) default NULL COMMENT '是否有时间限制(仅存盘时使用)',
  `attribID1` int(11) default NULL COMMENT '有色装备额外增加的属性ID1',
  `attribID2` int(11) default NULL COMMENT '有色装备额外增加的属性ID2',
  `attribRate1` int(11) default NULL COMMENT '有色装备属性ID的加成比率1',
  `attribRate2` int(11) default NULL COMMENT '有色装备属性ID的加成比率2',
  `count` int(11) default NULL COMMENT '药瓶的剩余数量',
  `enchantAttrib` int(11) default NULL COMMENT '附魔的属性ID',
  `ItemTimeLimit` int(11) default NULL COMMENT '过期时间',
  `curWear` int(11) default NULL COMMENT '当前耐久',
  `Valid` int(11) default NULL COMMENT '是否可以加成属性',
  `name` varchar(255) default NULL,
  `paramCount` int(11) default '0' COMMENT '参数数量',
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
