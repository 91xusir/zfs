/*
Navicat MySQL Data Transfer

Source Server         : 127.0.0.1
Source Server Version : 50156
Source Host           : localhost:3306
Source Database       : analyse

Target Server Type    : MYSQL
Target Server Version : 50156
File Encoding         : 65001

Date: 2011-09-02 16:53:40
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `items`
-- ----------------------------
DROP TABLE IF EXISTS `items`;
CREATE TABLE `items` (
  `Id` int(11) NOT NULL AUTO_INCREMENT,
  `UserID` int(11) NOT NULL DEFAULT '0',
  `ItemID` int(11) NOT NULL DEFAULT '0',
  `type` int(11) NOT NULL DEFAULT '0' COMMENT '',
  `subtype` int(11) NOT NULL DEFAULT '0' COMMENT '',
  `ItemIndex` int(11) NOT NULL DEFAULT '0' COMMENT 'ƷеID',
  `color` tinyint(3) NOT NULL DEFAULT '0' COMMENT 'ߵɫ(ɫװɫװͻƽװ)',
  `level` smallint(6) NOT NULL DEFAULT '0' COMMENT 'ȼ(10,index0ʼ)',
  `Bind` tinyint(3) DEFAULT NULL COMMENT '',
  `Lock` bit(1) DEFAULT NULL,
  `Countable` bit(1) DEFAULT NULL COMMENT 'Ƿ(ʱʹ)',
  `Enchant` bit(1) DEFAULT NULL COMMENT 'Ƿħ(ʱʹ)',
  `TimeLimit` bit(1) DEFAULT NULL COMMENT 'Ƿʱ(ʱʹ)',
  `attribID1` int(11) DEFAULT NULL COMMENT 'ɫװӵID1',
  `attribID2` int(11) DEFAULT NULL COMMENT 'ɫװӵID2',
  `attribRate1` int(11) DEFAULT NULL COMMENT 'ɫװIDļӳɱ1',
  `attribRate2` int(11) DEFAULT NULL COMMENT 'ɫװIDļӳɱ2',
  `count` int(11) DEFAULT NULL COMMENT 'ҩƿʣ',
  `enchantAttrib` int(11) DEFAULT NULL COMMENT 'ħID',
  `ItemTimeLimit` int(11) DEFAULT NULL COMMENT 'ʱ',
  `curWear` int(11) DEFAULT NULL COMMENT 'ǰ;',
  `Valid` int(11) DEFAULT NULL COMMENT 'ǷԼӳ',
  `name` varchar(255) DEFAULT NULL,
  `paramCount` int(11) DEFAULT '0' COMMENT '',
  `params1` int(11) DEFAULT NULL,
  `params2` int(11) DEFAULT NULL,
  `params3` int(11) DEFAULT NULL,
  `params4` int(11) DEFAULT NULL,
  `params5` int(11) DEFAULT NULL,
  `params6` int(11) DEFAULT NULL,
  `params7` int(11) DEFAULT NULL,
  `params8` int(11) DEFAULT NULL,
  `params9` int(11) DEFAULT NULL,
  `params10` int(11) DEFAULT NULL,
  `params11` int(11) DEFAULT NULL,
  `params12` int(11) DEFAULT NULL,
  `params13` int(11) DEFAULT NULL,
  `params14` int(11) DEFAULT NULL,
  `params15` int(11) DEFAULT NULL,
  `params16` int(11) DEFAULT NULL,
  `params17` int(11) DEFAULT NULL,
  `params18` int(11) DEFAULT NULL,
  `params19` int(11) DEFAULT NULL,
  `params20` int(11) DEFAULT NULL,
  `params21` int(11) DEFAULT NULL,
  `params22` int(11) DEFAULT NULL,
  `params23` int(11) DEFAULT NULL,
  `params24` int(11) DEFAULT NULL,
  `params25` int(11) DEFAULT NULL,
  `params26` int(11) DEFAULT NULL,
  `params27` int(11) DEFAULT NULL,
  `params28` int(11) DEFAULT NULL,
  `params29` int(11) DEFAULT NULL,
  `params30` int(11) DEFAULT NULL,
  `params31` int(11) DEFAULT NULL,
  `params32` int(11) DEFAULT NULL,
  `params33` int(11) DEFAULT NULL,
  `params34` int(11) DEFAULT NULL,
  `params35` int(11) DEFAULT NULL,
  `params36` int(11) DEFAULT NULL,
  `params37` int(11) DEFAULT NULL,
  `params38` int(11) DEFAULT NULL,
  `params39` int(11) DEFAULT NULL,
  `params40` int(11) DEFAULT NULL,
  `params41` int(11) DEFAULT NULL,
  `params42` int(11) DEFAULT NULL,
  `params43` int(11) DEFAULT NULL,
  `params44` int(11) DEFAULT NULL,
  `params45` int(11) DEFAULT NULL,
  `params46` int(11) DEFAULT NULL,
  `params47` int(11) DEFAULT NULL,
  `params48` int(11) DEFAULT NULL,
  `params49` int(11) DEFAULT NULL,
  `params50` int(11) DEFAULT NULL,
  PRIMARY KEY (`Id`),
  KEY `UserID` (`UserID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of items
-- ----------------------------

-- ----------------------------
-- Table structure for `lastupdatetime`
-- ----------------------------
DROP TABLE IF EXISTS `lastupdatetime`;
CREATE TABLE `lastupdatetime` (
  `Id` int(11) NOT NULL AUTO_INCREMENT,
  `LastUpdateDate` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`Id`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of lastupdatetime
-- ----------------------------
