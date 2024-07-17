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
INSERT INTO `concurrent_log` VALUES ('2', '2011-09-01 06:11:56', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('3', '2011-09-01 06:22:53', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('4', '2011-09-01 10:11:26', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('5', '2011-09-01 10:16:41', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('6', '2011-09-01 10:26:41', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('7', '2011-09-01 10:36:41', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('8', '2011-09-01 10:45:16', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('9', '2011-09-01 10:55:16', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('10', '2011-09-01 11:02:32', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('11', '2011-09-01 11:30:14', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('12', '2011-09-01 11:40:25', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('13', '2011-09-01 11:50:25', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('14', '2011-09-01 12:00:25', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('15', '2011-09-01 12:21:18', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('16', '2011-09-01 12:43:48', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('17', '2011-09-01 12:53:48', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('18', '2011-09-01 13:03:48', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('19', '2011-09-01 13:15:18', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('20', '2011-09-01 13:25:18', '1', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('21', '2011-09-01 13:35:18', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('22', '2011-09-01 13:45:18', '1', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('23', '2011-09-01 13:55:18', '1', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('24', '2011-09-01 14:05:18', '1', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('25', '2011-09-01 14:15:18', '1', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('26', '2011-09-01 14:25:18', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('27', '2011-09-01 14:35:18', '1', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('28', '2011-09-01 14:45:18', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('29', '2011-09-01 14:55:18', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('30', '2011-09-01 15:05:18', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('31', '2011-09-01 15:15:18', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('32', '2011-09-01 15:25:18', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('33', '2011-09-01 15:35:18', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('34', '2011-09-01 22:38:19', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('35', '2011-09-01 22:48:19', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('36', '2011-09-01 22:58:19', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('37', '2011-09-01 23:08:19', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('38', '2011-09-01 23:18:19', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('39', '2011-09-01 23:28:19', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('40', '2011-09-01 23:38:19', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('41', '2011-09-01 23:43:45', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('42', '2011-09-01 23:53:45', '1', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('43', '2011-09-02 00:03:45', '1', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('44', '2011-09-02 00:13:45', '1', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('45', '2011-09-02 03:42:50', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('46', '2011-09-02 03:52:50', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('47', '2011-09-02 04:02:50', '1', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('48', '2011-09-02 04:12:50', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('49', '2011-09-02 04:22:50', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('50', '2011-09-02 04:32:50', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('51', '2011-09-02 04:38:12', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('52', '2011-09-02 04:48:12', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('53', '2011-09-02 04:58:12', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('54', '2011-09-02 05:08:12', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('55', '2011-09-02 05:18:12', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('56', '2011-09-02 05:28:12', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('57', '2011-09-02 05:38:12', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('58', '2011-09-02 05:48:12', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('59', '2011-09-02 05:58:12', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('60', '2011-09-02 06:08:12', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('61', '2011-09-02 06:18:12', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('62', '2011-09-02 06:28:12', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('63', '2011-09-02 06:38:12', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('64', '2011-09-02 06:40:55', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('65', '2011-09-02 06:50:55', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('66', '2011-09-02 07:00:55', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('67', '2011-09-02 07:10:55', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('68', '2011-09-02 07:20:55', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('69', '2011-09-02 07:30:55', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('70', '2011-09-02 07:40:55', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('71', '2011-09-02 07:50:55', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('72', '2011-09-02 08:00:55', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('73', '2011-09-02 08:10:55', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('74', '2011-09-02 08:20:55', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('75', '2011-09-02 08:30:55', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('76', '2011-09-02 08:40:55', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('77', '2011-09-02 08:50:55', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('78', '2011-09-02 09:00:55', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('79', '2011-09-02 09:10:55', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('80', '2011-09-02 09:20:55', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('81', '2011-09-02 09:30:55', '1', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('82', '2011-09-02 09:40:55', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('83', '2011-09-02 09:50:55', '1', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('84', '2011-09-02 10:00:55', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('85', '2011-09-02 10:10:55', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('86', '2011-09-02 10:20:55', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('87', '2011-09-02 10:30:55', '1', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('88', '2011-09-02 10:40:55', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('89', '2011-09-02 13:35:31', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('90', '2011-09-02 13:45:31', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('91', '2011-09-02 13:55:31', '1', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('92', '2011-09-02 14:01:56', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('93', '2011-09-02 14:11:56', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('94', '2011-09-02 14:21:56', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('95', '2011-09-02 14:31:56', '1', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('96', '2011-09-02 14:41:56', '1', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('97', '2011-09-02 14:51:56', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('98', '2011-09-02 15:01:56', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('99', '2011-09-02 15:11:56', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('100', '2011-09-02 16:05:03', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('101', '2011-09-02 16:15:03', '1', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('102', '2011-09-02 16:24:02', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('103', '2011-09-02 16:34:02', '1', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `concurrent_log` VALUES ('104', '2011-09-02 16:44:02', '1', '0', '0', '0', '0', '0', '0', '0');

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
INSERT INTO `consume` VALUES ('2', '1', '16777343', '2011-09-01 13:16:34', '2011-09-01 13:16:41', '0', '-1');
INSERT INTO `consume` VALUES ('3', '2', '16777343', '2011-09-01 13:19:01', '2011-09-01 13:19:08', '0', '-1');
INSERT INTO `consume` VALUES ('4', '2', '16777343', '2011-09-01 13:20:21', '2011-09-01 13:20:28', '0', '-1');
INSERT INTO `consume` VALUES ('5', '2', '16777343', '2011-09-01 13:21:21', '2011-09-01 13:29:21', '0', '-1');
INSERT INTO `consume` VALUES ('6', '1', '16777343', '2011-09-01 13:37:14', '2011-09-01 13:41:57', '0', '-1');
INSERT INTO `consume` VALUES ('7', '1', '16777343', '2011-09-01 13:41:57', '2011-09-01 13:45:52', '0', '-1');
INSERT INTO `consume` VALUES ('8', '1', '16777343', '2011-09-01 13:46:11', '2011-09-01 13:46:18', '0', '-1');
INSERT INTO `consume` VALUES ('9', '1', '16777343', '2011-09-01 13:47:02', '2011-09-01 13:47:09', '0', '-1');
INSERT INTO `consume` VALUES ('10', '1', '16777343', '2011-09-01 13:47:26', '2011-09-01 13:47:39', '0', '-1');
INSERT INTO `consume` VALUES ('11', '1', '16777343', '2011-09-01 13:47:32', '2011-09-01 13:47:50', '0', '-1');
INSERT INTO `consume` VALUES ('12', '2', '16777343', '2011-09-01 13:47:43', '2011-09-01 13:48:28', '0', '-1');
INSERT INTO `consume` VALUES ('13', '2', '16777343', '2011-09-01 13:48:21', '2011-09-01 13:48:31', '0', '-1');
INSERT INTO `consume` VALUES ('14', '2', '16777343', '2011-09-01 13:48:30', '2011-09-01 13:52:39', '0', '-1');
INSERT INTO `consume` VALUES ('15', '2', '16777343', '2011-09-01 13:53:13', '2011-09-01 13:55:41', '0', '-1');
INSERT INTO `consume` VALUES ('16', '2', '16777343', '2011-09-01 13:57:46', '2011-09-01 13:57:55', '0', '-1');
INSERT INTO `consume` VALUES ('17', '2', '16777343', '2011-09-01 13:57:51', '2011-09-01 13:58:36', '0', '-1');
INSERT INTO `consume` VALUES ('18', '2', '16777343', '2011-09-01 14:04:59', '2011-09-01 14:08:17', '0', '-1');
INSERT INTO `consume` VALUES ('19', '2', '16777343', '2011-09-01 14:12:49', '2011-09-01 14:15:52', '0', '-1');
INSERT INTO `consume` VALUES ('20', '2', '16777343', '2011-09-01 14:24:53', '2011-09-01 14:25:21', '0', '-1');
INSERT INTO `consume` VALUES ('21', '2', '16777343', '2011-09-01 14:33:54', '2011-09-01 14:36:10', '0', '-1');
INSERT INTO `consume` VALUES ('22', '2', '16777343', '2011-09-01 14:43:04', '2011-09-01 14:43:17', '0', '-1');
INSERT INTO `consume` VALUES ('23', '2', '16777343', '2011-09-01 14:43:10', '2011-09-01 14:43:19', '0', '-1');
INSERT INTO `consume` VALUES ('24', '1', '16777343', '2011-09-01 14:48:12', '2011-09-01 14:48:45', '0', '-1');
INSERT INTO `consume` VALUES ('25', '1', '16777343', '2011-09-01 14:49:46', '2011-09-01 14:50:22', '0', '-1');
INSERT INTO `consume` VALUES ('26', '1', '16777343', '2011-09-01 14:50:16', '2011-09-01 14:50:53', '0', '-1');
INSERT INTO `consume` VALUES ('27', '1', '16777343', '2011-09-01 14:50:46', '2011-09-01 14:52:17', '0', '-1');
INSERT INTO `consume` VALUES ('28', '1', '16777343', '2011-09-01 14:52:10', '2011-09-01 14:52:23', '0', '-1');
INSERT INTO `consume` VALUES ('29', '1', '16777343', '2011-09-01 14:52:16', '2011-09-01 14:52:31', '0', '-1');
INSERT INTO `consume` VALUES ('30', '2', '16777343', '2011-09-01 14:52:24', '2011-09-01 14:54:23', '0', '-1');
INSERT INTO `consume` VALUES ('31', '2', '16777343', '2011-09-01 14:55:32', '2011-09-01 14:57:38', '0', '-1');
INSERT INTO `consume` VALUES ('32', '2', '16777343', '2011-09-01 14:57:31', '2011-09-01 15:00:12', '0', '-1');
INSERT INTO `consume` VALUES ('33', '2', '16777343', '2011-09-01 15:00:06', '2011-09-01 15:01:13', '0', '-1');
INSERT INTO `consume` VALUES ('34', '2', '16777343', '2011-09-01 15:01:06', '2011-09-01 15:02:39', '0', '-1');
INSERT INTO `consume` VALUES ('35', '2', '16777343', '2011-09-01 15:03:53', '2011-09-01 15:04:31', '0', '-1');
INSERT INTO `consume` VALUES ('36', '1', '16777343', '2011-09-01 22:40:51', '2011-09-01 22:41:33', '0', '-1');
INSERT INTO `consume` VALUES ('37', '1', '16777343', '2011-09-01 22:42:02', '2011-09-01 22:42:30', '0', '-1');
INSERT INTO `consume` VALUES ('38', '1', '16777343', '2011-09-01 22:45:38', '2011-09-01 22:46:15', '0', '-1');
INSERT INTO `consume` VALUES ('39', '1', '16777343', '2011-09-02 03:43:56', '2011-09-02 03:46:52', '0', '-1');
INSERT INTO `consume` VALUES ('40', '1', '16777343', '2011-09-02 03:48:43', '2011-09-02 03:48:57', '0', '-1');
INSERT INTO `consume` VALUES ('41', '1', '16777343', '2011-09-02 03:48:50', '2011-09-02 03:49:03', '0', '-1');
INSERT INTO `consume` VALUES ('42', '2', '16777343', '2011-09-02 03:48:56', '2011-09-02 03:49:07', '0', '-1');
INSERT INTO `consume` VALUES ('43', '1', '16777343', '2011-09-02 03:49:05', '2011-09-02 03:49:24', '0', '-1');
INSERT INTO `consume` VALUES ('44', '2', '16777343', '2011-09-02 03:49:17', '2011-09-02 03:52:18', '0', '-1');
INSERT INTO `consume` VALUES ('45', '2', '16777343', '2011-09-02 03:54:14', '2011-09-02 04:01:04', '0', '-1');
INSERT INTO `consume` VALUES ('46', '1', '16777343', '2011-09-02 04:01:48', '2011-09-02 04:02:01', '0', '-1');
INSERT INTO `consume` VALUES ('47', '2', '16777343', '2011-09-02 04:01:54', '2011-09-02 04:02:28', '0', '-1');
INSERT INTO `consume` VALUES ('48', '2', '16777343', '2011-09-02 04:02:27', '2011-09-02 04:02:45', '0', '-1');
INSERT INTO `consume` VALUES ('49', '2', '16777343', '2011-09-02 04:02:38', '2011-09-02 04:03:50', '0', '-1');
INSERT INTO `consume` VALUES ('50', '1', '16777343', '2011-09-02 04:06:53', '2011-09-02 04:07:23', '0', '-1');
INSERT INTO `consume` VALUES ('51', '1', '16777343', '2011-09-02 04:09:42', '2011-09-02 04:10:56', '0', '-1');
INSERT INTO `consume` VALUES ('52', '1', '16777343', '2011-09-02 04:30:45', '2011-09-02 04:31:21', '0', '-1');
INSERT INTO `consume` VALUES ('53', '1', '16777343', '2011-09-02 04:34:38', '2011-09-02 04:35:37', '0', '-1');
INSERT INTO `consume` VALUES ('54', '1', '16777343', '2011-09-02 04:38:52', '2011-09-02 04:39:27', '0', '-1');
INSERT INTO `consume` VALUES ('55', '2', '16777343', '2011-09-02 04:39:25', '2011-09-02 04:40:16', '0', '-1');
INSERT INTO `consume` VALUES ('56', '1', '16777343', '2011-09-02 04:42:28', '2011-09-02 04:43:19', '0', '-1');
INSERT INTO `consume` VALUES ('57', '1', '16777343', '2011-09-02 04:46:10', '2011-09-02 04:46:21', '0', '-1');
INSERT INTO `consume` VALUES ('58', '1', '16777343', '2011-09-02 04:47:00', '2011-09-02 04:47:31', '0', '-1');
INSERT INTO `consume` VALUES ('59', '1', '16777343', '2011-09-02 04:54:02', '2011-09-02 04:54:29', '0', '-1');
INSERT INTO `consume` VALUES ('60', '1', '16777343', '2011-09-02 04:56:37', '2011-09-02 04:57:56', '0', '-1');
INSERT INTO `consume` VALUES ('61', '1', '16777343', '2011-09-02 05:00:47', '2011-09-02 05:02:02', '0', '-1');
INSERT INTO `consume` VALUES ('62', '1', '16777343', '2011-09-02 05:02:25', '2011-09-02 05:03:38', '0', '-1');
INSERT INTO `consume` VALUES ('63', '1', '16777343', '2011-09-02 05:03:42', '2011-09-02 05:04:56', '0', '-1');
INSERT INTO `consume` VALUES ('64', '1', '16777343', '2011-09-02 05:04:56', '2011-09-02 05:06:08', '0', '-1');
INSERT INTO `consume` VALUES ('65', '1', '16777343', '2011-09-02 05:06:24', '2011-09-02 05:06:55', '0', '-1');
INSERT INTO `consume` VALUES ('66', '1', '16777343', '2011-09-02 05:08:58', '2011-09-02 05:09:26', '0', '-1');
INSERT INTO `consume` VALUES ('67', '1', '16777343', '2011-09-02 05:14:31', '2011-09-02 05:15:41', '0', '-1');
INSERT INTO `consume` VALUES ('68', '1', '16777343', '2011-09-02 05:30:21', '2011-09-02 05:31:01', '0', '-1');
INSERT INTO `consume` VALUES ('69', '1', '16777343', '2011-09-02 05:39:22', '2011-09-02 05:40:00', '0', '-1');
INSERT INTO `consume` VALUES ('70', '1', '16777343', '2011-09-02 05:45:30', '2011-09-02 05:46:01', '0', '-1');
INSERT INTO `consume` VALUES ('71', '1', '16777343', '2011-09-02 06:11:40', '2011-09-02 06:12:16', '0', '-1');
INSERT INTO `consume` VALUES ('72', '1', '16777343', '2011-09-02 06:22:20', '2011-09-02 06:24:18', '0', '-1');
INSERT INTO `consume` VALUES ('73', '1', '16777343', '2011-09-02 06:38:44', '2011-09-02 06:38:56', '0', '-1');
INSERT INTO `consume` VALUES ('74', '1', '16777343', '2011-09-02 06:41:48', '2011-09-02 06:43:31', '0', '-1');
INSERT INTO `consume` VALUES ('75', '1', '16777343', '2011-09-02 06:48:35', '2011-09-02 06:49:04', '0', '-1');
INSERT INTO `consume` VALUES ('76', '1', '16777343', '2011-09-02 06:49:41', '2011-09-02 06:50:52', '0', '-1');
INSERT INTO `consume` VALUES ('77', '1', '16777343', '2011-09-02 06:55:44', '2011-09-02 06:56:13', '0', '-1');
INSERT INTO `consume` VALUES ('78', '1', '16777343', '2011-09-02 07:04:22', '2011-09-02 07:04:51', '0', '-1');
INSERT INTO `consume` VALUES ('79', '1', '16777343', '2011-09-02 07:14:32', '2011-09-02 07:16:04', '0', '-1');
INSERT INTO `consume` VALUES ('80', '1', '16777343', '2011-09-02 07:20:25', '2011-09-02 07:20:53', '0', '-1');
INSERT INTO `consume` VALUES ('81', '1', '16777343', '2011-09-02 07:22:13', '2011-09-02 07:23:22', '0', '-1');
INSERT INTO `consume` VALUES ('82', '1', '16777343', '2011-09-02 07:34:50', '2011-09-02 07:35:01', '0', '-1');
INSERT INTO `consume` VALUES ('83', '1', '16777343', '2011-09-02 07:45:49', '2011-09-02 07:48:16', '0', '-1');
INSERT INTO `consume` VALUES ('84', '1', '16777343', '2011-09-02 09:28:42', '2011-09-02 09:31:45', '0', '-1');
INSERT INTO `consume` VALUES ('85', '1', '16777343', '2011-09-02 09:49:54', '2011-09-02 09:53:30', '0', '-1');
INSERT INTO `consume` VALUES ('86', '1', '16777343', '2011-09-02 10:30:45', '2011-09-02 10:37:15', '0', '-1');
INSERT INTO `consume` VALUES ('87', '1', '16777343', '2011-09-02 13:51:30', '2011-09-02 13:52:06', '0', '-1');
INSERT INTO `consume` VALUES ('88', '2', '16777343', '2011-09-02 13:53:34', '2011-09-02 13:55:21', '0', '-1');
INSERT INTO `consume` VALUES ('89', '1', '16777343', '2011-09-02 13:55:24', '2011-09-02 13:55:47', '0', '-1');
INSERT INTO `consume` VALUES ('90', '1', '16777343', '2011-09-02 14:24:58', '2011-09-02 14:25:54', '0', '-1');
INSERT INTO `consume` VALUES ('91', '1', '16777343', '2011-09-02 14:25:55', '2011-09-02 14:26:15', '0', '-1');
INSERT INTO `consume` VALUES ('92', '1', '16777343', '2011-09-02 14:30:22', '2011-09-02 14:33:38', '0', '-1');
INSERT INTO `consume` VALUES ('93', '1', '16777343', '2011-09-02 14:36:06', '2011-09-02 14:45:33', '0', '-1');
INSERT INTO `consume` VALUES ('94', '1', '16777343', '2011-09-02 14:45:33', '2011-09-02 14:50:14', '0', '-1');
INSERT INTO `consume` VALUES ('95', '1', '16777343', '2011-09-02 16:06:00', '2011-09-02 16:13:02', '0', '-1');
INSERT INTO `consume` VALUES ('96', '1', '16777343', '2011-09-02 16:25:15', '2011-09-02 16:26:07', '0', '-1');
INSERT INTO `consume` VALUES ('97', '1', '16777343', '2011-09-02 16:26:28', '2011-09-02 16:33:37', '0', '-1');
INSERT INTO `consume` VALUES ('98', '1', '16777343', '2011-09-02 16:33:59', '2011-09-02 16:46:55', '0', '-1');

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
INSERT INTO `user` VALUES ('1', 'testtest', '05a671c66aefea124cc08b76ea6d30bb', '', '0', '1', '0', '1970-01-01 00:00:00', '1970-01-01 00:00:00', '1970-01-01 00:00:00', '0', '2011-09-02 16:33:59', '127.0.0.1', '1970-01-01 00:00:00', '0', '0', '1', '0', null);
INSERT INTO `user` VALUES ('2', 'testtest2', '05a671c66aefea124cc08b76ea6d30bb', '', '0', '1', '0', '1970-01-01 00:00:00', '1970-01-01 00:00:00', '1970-01-01 00:00:00', '0', '2011-09-02 13:53:34', '127.0.0.1', '1970-01-01 00:00:00', '0', '0', '1', '0', null);
