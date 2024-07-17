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
