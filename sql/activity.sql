/*
Navicat MySQL Data Transfer

Source Server         : 127.0.0.1
Source Server Version : 50156
Source Host           : localhost:3306
Source Database       : activity

Target Server Type    : MYSQL
Target Server Version : 50156
File Encoding         : 65001

Date: 2011-09-02 16:53:47
*/
CREATE DATABASE IF NOT EXISTS activity;

USE activity;

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `enter_activity`
-- ----------------------------
DROP TABLE IF EXISTS `enter_activity`;
CREATE TABLE `enter_activity` (
  `UserID` int(11) unsigned NOT NULL,
  `UserName` varchar(64) NOT NULL DEFAULT '',
  `1` int(4) DEFAULT '0',
  `2` int(4) DEFAULT '0',
  `3` int(4) DEFAULT '0',
  `4` int(4) DEFAULT '0',
  UNIQUE KEY `UserID` (`UserID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of enter_activity
-- ----------------------------
INSERT INTO `enter_activity` VALUES ('1', 'testtest', '1', '1', '1', '1');
INSERT INTO `enter_activity` VALUES ('2', 'testtest2', '1', '1', '1', '1');
INSERT INTO `enter_activity` VALUES ('9', 'estetstes', '0', '0', '0', '0');
INSERT INTO `enter_activity` VALUES ('3', 'tesfesest', '0', '0', '0', '0');

-- ----------------------------
-- Table structure for `user_cachot`
-- ----------------------------
DROP TABLE IF EXISTS `user_cachot`;
CREATE TABLE `user_cachot` (
  `UserID` int(11) NOT NULL DEFAULT '0',
  `UserName` varchar(64) DEFAULT NULL,
  `ActivityType` int(11) NOT NULL DEFAULT '1',
  `JoinState` int(11) NOT NULL DEFAULT '2',
  `RunDegree` int(11) NOT NULL DEFAULT '0',
  `StateTime` varchar(32) DEFAULT NULL COMMENT '...ʱ.',
  UNIQUE KEY `UserID` (`UserID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of user_cachot
-- ----------------------------
INSERT INTO `user_cachot` VALUES ('1', 'testtest', '1', '2', '0', null);
INSERT INTO `user_cachot` VALUES ('2', 'testtest2', '1', '2', '0', null);

-- ----------------------------
-- Table structure for `user_hold_treasure`
-- ----------------------------
DROP TABLE IF EXISTS `user_hold_treasure`;
CREATE TABLE `user_hold_treasure` (
  `UserID` int(11) NOT NULL DEFAULT '0',
  `UserName` varchar(64) DEFAULT NULL,
  `ActivityType` int(11) NOT NULL DEFAULT '1',
  `JoinState` int(11) NOT NULL DEFAULT '2',
  `StateTime` varchar(32) DEFAULT NULL COMMENT '...ʱ.',
  UNIQUE KEY `UserID` (`UserID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of user_hold_treasure
-- ----------------------------
INSERT INTO `user_hold_treasure` VALUES ('1', 'testtest', '1', '2', null);
INSERT INTO `user_hold_treasure` VALUES ('2', 'testtest2', '1', '2', null);
