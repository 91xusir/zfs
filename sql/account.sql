/*
Navicat MySQL Data Transfer

Source Server         : 127.0.0.1
Source Server Version : 50156
Source Host           : localhost:3306
Source Database       : account

Target Server Type    : MYSQL
Target Server Version : 50156
File Encoding         : 65001

Date: 2011-09-02 16:53:09
*/
CREATE DATABASE IF NOT EXISTS account;

USE account;

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `a_log`
-- ----------------------------
DROP TABLE IF EXISTS `a_log`;
CREATE TABLE `a_log` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `theType` char(20) NOT NULL,
  `theTypeKind` tinyint(11) NOT NULL DEFAULT '1',
  `theDate` datetime NOT NULL,
  `theAccount` char(20) NOT NULL,
  `thePasswordOld` char(32) DEFAULT NULL,
  `thePasswordNew` char(32) DEFAULT NULL,
  `theNum` bigint(11) NOT NULL DEFAULT '0',
  `theDepict` varchar(100) DEFAULT NULL,
  `awflag` int(11) DEFAULT NULL,
  `theNumBefore` bigint(20) NOT NULL,
  `theNumAfter` bigint(20) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312 COMMENT='wtLog';

-- ----------------------------
-- Records of a_log
-- ----------------------------

-- ----------------------------
-- Table structure for `card_number`
-- ----------------------------
DROP TABLE IF EXISTS `card_number`;
CREATE TABLE `card_number` (
  `idx` int(11) NOT NULL AUTO_INCREMENT,
  `userid` int(11) NOT NULL DEFAULT '0',
  `accountid` int(11) NOT NULL DEFAULT '0',
  `cardid` varchar(16) NOT NULL DEFAULT '',
  `type` int(11) NOT NULL,
  `attribute` varchar(300) NOT NULL DEFAULT '',
  `usetime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `createtime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `userip` varchar(16) NOT NULL DEFAULT '',
  `cardtrade` int(11) NOT NULL DEFAULT '0',
  `cardidMD5` varchar(32) NOT NULL DEFAULT '',
  PRIMARY KEY (`idx`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of card_number
-- ----------------------------

-- ----------------------------
-- Table structure for `concurrent_log`
-- ----------------------------
DROP TABLE IF EXISTS `concurrent_log`;
CREATE TABLE `concurrent_log` (
  `idx` int(11) NOT NULL AUTO_INCREMENT,
  `log_time` datetime DEFAULT CURRENT_TIMESTAMP,
  `gw1` mediumint(9) NOT NULL DEFAULT '0',
  `gw2` mediumint(9) NOT NULL DEFAULT '0',
  `gw3` mediumint(9) NOT NULL DEFAULT '0',
  `gw4` mediumint(9) NOT NULL DEFAULT '0',
  `gw5` mediumint(9) NOT NULL DEFAULT '0',
  `gw6` mediumint(9) NOT NULL DEFAULT '0',
  `gw7` mediumint(9) NOT NULL DEFAULT '0',
  `gw8` mediumint(9) NOT NULL DEFAULT '0',
  PRIMARY KEY (`idx`)
) ENGINE=MyISAM AUTO_INCREMENT=105 DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of concurrent_log
-- ----------------------------
INSERT INTO `concurrent_log` VALUES ('1', '2011-09-01 06:01:56', '0', '0', '0', '0', '0', '0', '0', '0');


-- ----------------------------
-- Table structure for `consume`
-- ----------------------------
DROP TABLE IF EXISTS `consume`;
CREATE TABLE `consume` (
  `idx` int(11) NOT NULL AUTO_INCREMENT,
  `accountID` int(11) NOT NULL DEFAULT '0',
  `ip` varchar(20) NOT NULL DEFAULT '',
  `enter_time` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `leave_time` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `Point` float NOT NULL DEFAULT '0',
  `Server` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`idx`)
) ENGINE=MyISAM AUTO_INCREMENT=99 DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of consume
-- ----------------------------
INSERT INTO `consume` VALUES ('1', '1', '16777343', '2011-09-01 13:16:28', '2011-09-01 13:16:35', '0', '-1');


-- ----------------------------
-- Table structure for `log_charge_card`
-- ----------------------------
DROP TABLE IF EXISTS `log_charge_card`;
CREATE TABLE `log_charge_card` (
  `idx` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `log_time` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `cmdSN` int(10) unsigned NOT NULL DEFAULT '0',
  `op` int(10) unsigned NOT NULL DEFAULT '0',
  `cardType` int(10) unsigned NOT NULL DEFAULT '0',
  `server` int(10) unsigned NOT NULL DEFAULT '0',
  `accountID` int(10) unsigned NOT NULL DEFAULT '0',
  `userID` int(10) unsigned NOT NULL DEFAULT '0',
  `cardID` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`idx`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of log_charge_card
-- ----------------------------

-- ----------------------------
-- Table structure for `order`
-- ----------------------------
DROP TABLE IF EXISTS `order`;
CREATE TABLE `order` (
  `billingID` varchar(225) NOT NULL DEFAULT '',
  `point` varchar(225) DEFAULT NULL,
  `extrapoint` varchar(225) DEFAULT NULL,
  `account` varchar(225) DEFAULT NULL,
  `datetime` datetime NOT NULL DEFAULT '1970-01-01 00:00:00'
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of order
-- ----------------------------

-- ----------------------------
-- Table structure for `user`
-- ----------------------------
DROP TABLE IF EXISTS `user`;
CREATE TABLE `user` (
  `idx` int(11) NOT NULL AUTO_INCREMENT,
  `username` varchar(20) NOT NULL DEFAULT '',
  `password` varchar(32) NOT NULL DEFAULT '',
  `identity` varchar(225) NOT NULL DEFAULT '',
  `deleted` smallint(6) NOT NULL DEFAULT '0',
  `user_priv` mediumint(9) NOT NULL DEFAULT '0',
  `locked` smallint(6) NOT NULL DEFAULT '0',
  `lock_begin` datetime NOT NULL DEFAULT '1970-01-01 00:00:00',
  `lock_end` datetime NOT NULL DEFAULT '1970-01-01 00:00:00',
  `create_date` datetime NOT NULL DEFAULT '1970-01-01 00:00:00',
  `from` int(11) NOT NULL DEFAULT '0',
  `latest_login_date` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `latest_login_ip` varchar(45) NOT NULL DEFAULT '-',
  `datetime` datetime NOT NULL DEFAULT '1970-01-01 00:00:00',
  `point` int(11) NOT NULL DEFAULT '0',
  `isVip` int(11) NOT NULL DEFAULT '0',
  `activation` tinyint(1) NOT NULL DEFAULT '0',
  `badult` tinyint(1) NOT NULL DEFAULT '0',
  `HairIndex` int(11) DEFAULT '0',
  PRIMARY KEY (`idx`)
) ENGINE=MyISAM AUTO_INCREMENT=3 DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of user
-- ----------------------------
INSERT INTO `user` VALUES ('1', 'admin1', 'e10adc3949ba59abbe56e057f20f883e', '', '0', '1', '0', '1970-01-01 00:00:00', '1970-01-01 00:00:00', '1970-01-01 00:00:00', '0', '2011-09-02 16:33:59', '127.0.0.1', '1970-01-01 00:00:00', '0', '0', '1', '0', null);
INSERT INTO `user` VALUES ('2', 'admin2', 'e10adc3949ba59abbe56e057f20f883e', '', '0', '1', '0', '1970-01-01 00:00:00', '1970-01-01 00:00:00', '1970-01-01 00:00:00', '0', '2011-09-02 13:53:34', '127.0.0.1', '1970-01-01 00:00:00', '0', '0', '1', '0', null);
INSERT INTO `user` VALUES ('3', 'admin3', 'e10adc3949ba59abbe56e057f20f883e', '', '0', '1', '0', '1970-01-01 00:00:00', '1970-01-01 00:00:00', '1970-01-01 00:00:00', '0', '2011-09-02 13:53:34', '127.0.0.1', '1970-01-01 00:00:00', '0', '0', '1', '0', null);
