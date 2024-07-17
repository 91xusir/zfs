/*
Navicat MySQL Data Transfer

Source Server         : 127.0.0.1
Source Server Version : 50156
Source Host           : localhost:3306
Source Database       : fso_log

Target Server Type    : MYSQL
Target Server Version : 50156
File Encoding         : 65001

Date: 2011-09-02 17:28:23
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `t_log_user_history`
-- ----------------------------
DROP TABLE IF EXISTS `t_log_user_history`;
CREATE TABLE `t_log_user_history` (
  `OperateID` int(11) NOT NULL AUTO_INCREMENT,
  `type` int(11) NOT NULL DEFAULT '0',
  `userID` int(11) NOT NULL DEFAULT '0',
  `longParam1` int(11) NOT NULL DEFAULT '0',
  `strParam1` longtext NOT NULL,
  `longParam2` int(11) NOT NULL DEFAULT '0',
  `strParam2` mediumtext NOT NULL,
  `longParam3` int(11) NOT NULL DEFAULT '0',
  `strParam3` longtext NOT NULL,
  `LogTime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `userIP` varchar(32) NOT NULL DEFAULT '',
  PRIMARY KEY (`OperateID`),
  KEY `type` (`type`),
  KEY `type_longParam1` (`type`,`longParam1`),
  KEY `type_longParam1_userID` (`type`,`longParam1`,`userID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of t_log_user_history
-- ----------------------------

-- ----------------------------
-- Table structure for `t_log_user_history1`
-- ----------------------------
DROP TABLE IF EXISTS `t_log_user_history1`;
CREATE TABLE `t_log_user_history1` (
  `OperateID` int(11) NOT NULL AUTO_INCREMENT,
  `type` int(11) NOT NULL DEFAULT '0',
  `userID` int(11) NOT NULL DEFAULT '0',
  `longParam1` int(11) NOT NULL DEFAULT '0',
  `strParam1` longtext NOT NULL,
  `longParam2` int(11) NOT NULL DEFAULT '0',
  `strParam2` longtext NOT NULL,
  `longParam3` int(11) NOT NULL DEFAULT '0',
  `strParam3` longtext NOT NULL,
  `LogTime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `userIP` varchar(32) NOT NULL DEFAULT '',
  PRIMARY KEY (`OperateID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of t_log_user_history1
-- ----------------------------

-- ----------------------------
-- Table structure for `t_log_user_history10`
-- ----------------------------
DROP TABLE IF EXISTS `t_log_user_history10`;
CREATE TABLE `t_log_user_history10` (
  `OperateID` int(11) NOT NULL AUTO_INCREMENT,
  `type` int(11) NOT NULL DEFAULT '0',
  `userID` int(11) NOT NULL DEFAULT '0',
  `longParam1` int(11) NOT NULL DEFAULT '0',
  `strParam1` longtext NOT NULL,
  `longParam2` int(11) NOT NULL DEFAULT '0',
  `strParam2` longtext NOT NULL,
  `longParam3` int(11) NOT NULL DEFAULT '0',
  `strParam3` longtext NOT NULL,
  `LogTime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `userIP` varchar(32) NOT NULL DEFAULT '',
  PRIMARY KEY (`OperateID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of t_log_user_history10
-- ----------------------------

-- ----------------------------
-- Table structure for `t_log_user_history2`
-- ----------------------------
DROP TABLE IF EXISTS `t_log_user_history2`;
CREATE TABLE `t_log_user_history2` (
  `OperateID` int(11) NOT NULL AUTO_INCREMENT,
  `type` int(11) NOT NULL DEFAULT '0',
  `userID` int(11) NOT NULL DEFAULT '0',
  `longParam1` int(11) NOT NULL DEFAULT '0',
  `strParam1` longtext NOT NULL,
  `longParam2` int(11) NOT NULL DEFAULT '0',
  `strParam2` longtext NOT NULL,
  `longParam3` int(11) NOT NULL DEFAULT '0',
  `strParam3` longtext NOT NULL,
  `LogTime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `userIP` varchar(32) NOT NULL DEFAULT '',
  PRIMARY KEY (`OperateID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of t_log_user_history2
-- ----------------------------

-- ----------------------------
-- Table structure for `t_log_user_history3`
-- ----------------------------
DROP TABLE IF EXISTS `t_log_user_history3`;
CREATE TABLE `t_log_user_history3` (
  `OperateID` int(11) NOT NULL AUTO_INCREMENT,
  `type` int(11) NOT NULL DEFAULT '0',
  `userID` int(11) NOT NULL DEFAULT '0',
  `longParam1` int(11) NOT NULL DEFAULT '0',
  `strParam1` longtext NOT NULL,
  `longParam2` int(11) NOT NULL DEFAULT '0',
  `strParam2` longtext NOT NULL,
  `longParam3` int(11) NOT NULL DEFAULT '0',
  `strParam3` longtext NOT NULL,
  `LogTime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `userIP` varchar(32) NOT NULL DEFAULT '',
  PRIMARY KEY (`OperateID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of t_log_user_history3
-- ----------------------------

-- ----------------------------
-- Table structure for `t_log_user_history4`
-- ----------------------------
DROP TABLE IF EXISTS `t_log_user_history4`;
CREATE TABLE `t_log_user_history4` (
  `OperateID` int(11) NOT NULL AUTO_INCREMENT,
  `type` int(11) NOT NULL DEFAULT '0',
  `userID` int(11) NOT NULL DEFAULT '0',
  `longParam1` int(11) NOT NULL DEFAULT '0',
  `strParam1` longtext NOT NULL,
  `longParam2` int(11) NOT NULL DEFAULT '0',
  `strParam2` longtext NOT NULL,
  `longParam3` int(11) NOT NULL DEFAULT '0',
  `strParam3` mediumtext NOT NULL,
  `LogTime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `userIP` varchar(32) NOT NULL DEFAULT '',
  PRIMARY KEY (`OperateID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of t_log_user_history4
-- ----------------------------

-- ----------------------------
-- Table structure for `t_log_user_history5`
-- ----------------------------
DROP TABLE IF EXISTS `t_log_user_history5`;
CREATE TABLE `t_log_user_history5` (
  `OperateID` int(11) NOT NULL AUTO_INCREMENT,
  `type` int(11) NOT NULL DEFAULT '0',
  `userID` int(11) NOT NULL DEFAULT '0',
  `longParam1` int(11) NOT NULL DEFAULT '0',
  `strParam1` longtext NOT NULL,
  `longParam2` int(11) NOT NULL DEFAULT '0',
  `strParam2` mediumtext NOT NULL,
  `longParam3` int(11) NOT NULL DEFAULT '0',
  `strParam3` mediumtext NOT NULL,
  `LogTime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `userIP` varchar(32) NOT NULL DEFAULT '',
  PRIMARY KEY (`OperateID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of t_log_user_history5
-- ----------------------------

-- ----------------------------
-- Table structure for `t_log_user_history6`
-- ----------------------------
DROP TABLE IF EXISTS `t_log_user_history6`;
CREATE TABLE `t_log_user_history6` (
  `OperateID` int(11) NOT NULL AUTO_INCREMENT,
  `type` int(11) NOT NULL DEFAULT '0',
  `userID` int(11) NOT NULL DEFAULT '0',
  `longParam1` int(11) NOT NULL DEFAULT '0',
  `strParam1` mediumtext NOT NULL,
  `longParam2` int(11) NOT NULL DEFAULT '0',
  `strParam2` mediumtext NOT NULL,
  `longParam3` int(11) NOT NULL DEFAULT '0',
  `strParam3` mediumtext NOT NULL,
  `LogTime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `userIP` varchar(32) NOT NULL DEFAULT '',
  PRIMARY KEY (`OperateID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of t_log_user_history6
-- ----------------------------

-- ----------------------------
-- Table structure for `t_log_user_history7`
-- ----------------------------
DROP TABLE IF EXISTS `t_log_user_history7`;
CREATE TABLE `t_log_user_history7` (
  `OperateID` int(11) NOT NULL AUTO_INCREMENT,
  `type` int(11) NOT NULL DEFAULT '0',
  `userID` int(11) NOT NULL DEFAULT '0',
  `longParam1` int(11) NOT NULL DEFAULT '0',
  `strParam1` mediumtext NOT NULL,
  `longParam2` int(11) NOT NULL DEFAULT '0',
  `strParam2` mediumtext NOT NULL,
  `longParam3` int(11) NOT NULL DEFAULT '0',
  `strParam3` mediumtext NOT NULL,
  `LogTime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `userIP` varchar(32) NOT NULL DEFAULT '',
  PRIMARY KEY (`OperateID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of t_log_user_history7
-- ----------------------------

-- ----------------------------
-- Table structure for `t_log_user_history8`
-- ----------------------------
DROP TABLE IF EXISTS `t_log_user_history8`;
CREATE TABLE `t_log_user_history8` (
  `OperateID` int(11) NOT NULL AUTO_INCREMENT,
  `type` int(11) NOT NULL DEFAULT '0',
  `userID` int(11) NOT NULL DEFAULT '0',
  `longParam1` int(11) NOT NULL DEFAULT '0',
  `strParam1` mediumtext NOT NULL,
  `longParam2` int(11) NOT NULL DEFAULT '0',
  `strParam2` mediumtext NOT NULL,
  `longParam3` int(11) NOT NULL DEFAULT '0',
  `strParam3` mediumtext NOT NULL,
  `LogTime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `userIP` varchar(32) NOT NULL DEFAULT '',
  PRIMARY KEY (`OperateID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of t_log_user_history8
-- ----------------------------

-- ----------------------------
-- Table structure for `t_log_user_history9`
-- ----------------------------
DROP TABLE IF EXISTS `t_log_user_history9`;
CREATE TABLE `t_log_user_history9` (
  `OperateID` int(11) NOT NULL AUTO_INCREMENT,
  `type` int(11) NOT NULL DEFAULT '0',
  `userID` int(11) NOT NULL DEFAULT '0',
  `longParam1` int(11) NOT NULL DEFAULT '0',
  `strParam1` mediumtext NOT NULL,
  `longParam2` int(11) NOT NULL DEFAULT '0',
  `strParam2` mediumtext NOT NULL,
  `longParam3` int(11) NOT NULL DEFAULT '0',
  `strParam3` mediumtext NOT NULL,
  `LogTime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `userIP` varchar(32) NOT NULL DEFAULT '',
  PRIMARY KEY (`OperateID`)
) ENGINE=MyISAM DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of t_log_user_history9
-- ----------------------------

-- ----------------------------
-- Table structure for `t_log_user_onlineamount`
-- ----------------------------
DROP TABLE IF EXISTS `t_log_user_onlineamount`;
CREATE TABLE `t_log_user_onlineamount` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `OnlineAmount` int(32) DEFAULT '0',
  `LogOnlineTime` datetime DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  UNIQUE KEY `LogOnlineTime` (`LogOnlineTime`) USING BTREE
) ENGINE=MyISAM AUTO_INCREMENT=814 DEFAULT CHARSET=gb2312 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of t_log_user_onlineamount
-- ----------------------------
INSERT INTO `t_log_user_onlineamount` VALUES ('1', '0', '2011-09-01 06:04:26');
INSERT INTO `t_log_user_onlineamount` VALUES ('2', '0', '2011-09-01 06:05:27');
INSERT INTO `t_log_user_onlineamount` VALUES ('3', '0', '2011-09-01 06:06:28');
INSERT INTO `t_log_user_onlineamount` VALUES ('4', '0', '2011-09-01 06:07:29');
INSERT INTO `t_log_user_onlineamount` VALUES ('5', '0', '2011-09-01 06:08:30');
INSERT INTO `t_log_user_onlineamount` VALUES ('6', '0', '2011-09-01 06:09:31');
INSERT INTO `t_log_user_onlineamount` VALUES ('7', '0', '2011-09-01 06:10:32');
INSERT INTO `t_log_user_onlineamount` VALUES ('8', '0', '2011-09-01 06:11:33');
INSERT INTO `t_log_user_onlineamount` VALUES ('9', '0', '2011-09-01 06:12:34');
INSERT INTO `t_log_user_onlineamount` VALUES ('10', '0', '2011-09-01 10:10:27');
INSERT INTO `t_log_user_onlineamount` VALUES ('11', '0', '2011-09-01 10:11:38');
INSERT INTO `t_log_user_onlineamount` VALUES ('12', '0', '2011-09-01 10:12:39');
INSERT INTO `t_log_user_onlineamount` VALUES ('13', '0', '2011-09-01 10:17:00');
INSERT INTO `t_log_user_onlineamount` VALUES ('14', '0', '2011-09-01 10:18:01');
INSERT INTO `t_log_user_onlineamount` VALUES ('15', '0', '2011-09-01 10:19:02');
INSERT INTO `t_log_user_onlineamount` VALUES ('16', '0', '2011-09-01 10:20:03');
INSERT INTO `t_log_user_onlineamount` VALUES ('17', '0', '2011-09-01 10:21:04');
INSERT INTO `t_log_user_onlineamount` VALUES ('18', '0', '2011-09-01 10:22:05');
INSERT INTO `t_log_user_onlineamount` VALUES ('19', '0', '2011-09-01 10:23:06');
INSERT INTO `t_log_user_onlineamount` VALUES ('20', '0', '2011-09-01 10:24:07');
INSERT INTO `t_log_user_onlineamount` VALUES ('21', '0', '2011-09-01 10:25:08');
INSERT INTO `t_log_user_onlineamount` VALUES ('22', '0', '2011-09-01 10:26:09');
INSERT INTO `t_log_user_onlineamount` VALUES ('23', '0', '2011-09-01 10:27:10');
INSERT INTO `t_log_user_onlineamount` VALUES ('24', '0', '2011-09-01 10:28:11');
INSERT INTO `t_log_user_onlineamount` VALUES ('25', '0', '2011-09-01 10:29:12');
INSERT INTO `t_log_user_onlineamount` VALUES ('26', '0', '2011-09-01 10:30:13');
INSERT INTO `t_log_user_onlineamount` VALUES ('27', '0', '2011-09-01 10:31:14');
INSERT INTO `t_log_user_onlineamount` VALUES ('28', '0', '2011-09-01 10:32:15');
INSERT INTO `t_log_user_onlineamount` VALUES ('29', '0', '2011-09-01 10:33:16');
INSERT INTO `t_log_user_onlineamount` VALUES ('30', '0', '2011-09-01 10:34:17');
INSERT INTO `t_log_user_onlineamount` VALUES ('31', '0', '2011-09-01 10:35:18');
INSERT INTO `t_log_user_onlineamount` VALUES ('32', '0', '2011-09-01 10:36:19');
INSERT INTO `t_log_user_onlineamount` VALUES ('33', '0', '2011-09-01 10:37:20');
INSERT INTO `t_log_user_onlineamount` VALUES ('34', '0', '2011-09-01 10:38:21');
INSERT INTO `t_log_user_onlineamount` VALUES ('35', '0', '2011-09-01 10:39:22');
INSERT INTO `t_log_user_onlineamount` VALUES ('36', '0', '2011-09-01 10:40:23');
INSERT INTO `t_log_user_onlineamount` VALUES ('37', '0', '2011-09-01 10:41:24');
INSERT INTO `t_log_user_onlineamount` VALUES ('38', '0', '2011-09-01 10:45:07');
INSERT INTO `t_log_user_onlineamount` VALUES ('39', '0', '2011-09-01 10:46:08');
INSERT INTO `t_log_user_onlineamount` VALUES ('40', '0', '2011-09-01 10:47:09');
INSERT INTO `t_log_user_onlineamount` VALUES ('41', '0', '2011-09-01 10:48:10');
INSERT INTO `t_log_user_onlineamount` VALUES ('42', '0', '2011-09-01 10:49:11');
INSERT INTO `t_log_user_onlineamount` VALUES ('43', '0', '2011-09-01 10:50:12');
INSERT INTO `t_log_user_onlineamount` VALUES ('44', '0', '2011-09-01 10:51:13');
INSERT INTO `t_log_user_onlineamount` VALUES ('45', '0', '2011-09-01 10:52:14');
INSERT INTO `t_log_user_onlineamount` VALUES ('46', '0', '2011-09-01 10:53:15');
INSERT INTO `t_log_user_onlineamount` VALUES ('47', '0', '2011-09-01 10:54:16');
INSERT INTO `t_log_user_onlineamount` VALUES ('48', '0', '2011-09-01 10:55:17');
INSERT INTO `t_log_user_onlineamount` VALUES ('49', '0', '2011-09-01 10:56:18');
INSERT INTO `t_log_user_onlineamount` VALUES ('50', '0', '2011-09-01 10:57:19');
INSERT INTO `t_log_user_onlineamount` VALUES ('51', '0', '2011-09-01 10:58:20');
INSERT INTO `t_log_user_onlineamount` VALUES ('52', '0', '2011-09-01 10:59:21');
INSERT INTO `t_log_user_onlineamount` VALUES ('53', '0', '2011-09-01 11:00:22');
INSERT INTO `t_log_user_onlineamount` VALUES ('54', '0', '2011-09-01 11:03:47');
INSERT INTO `t_log_user_onlineamount` VALUES ('55', '0', '2011-09-01 11:10:14');
INSERT INTO `t_log_user_onlineamount` VALUES ('56', '0', '2011-09-01 11:11:15');
INSERT INTO `t_log_user_onlineamount` VALUES ('57', '0', '2011-09-01 11:29:46');
INSERT INTO `t_log_user_onlineamount` VALUES ('58', '0', '2011-09-01 11:30:47');
INSERT INTO `t_log_user_onlineamount` VALUES ('59', '0', '2011-09-01 11:31:48');
INSERT INTO `t_log_user_onlineamount` VALUES ('60', '0', '2011-09-01 11:32:49');
INSERT INTO `t_log_user_onlineamount` VALUES ('61', '0', '2011-09-01 11:40:34');
INSERT INTO `t_log_user_onlineamount` VALUES ('62', '0', '2011-09-01 11:41:35');
INSERT INTO `t_log_user_onlineamount` VALUES ('63', '0', '2011-09-01 11:42:36');
INSERT INTO `t_log_user_onlineamount` VALUES ('64', '0', '2011-09-01 11:43:37');
INSERT INTO `t_log_user_onlineamount` VALUES ('65', '0', '2011-09-01 11:44:38');
INSERT INTO `t_log_user_onlineamount` VALUES ('66', '0', '2011-09-01 11:45:39');
INSERT INTO `t_log_user_onlineamount` VALUES ('67', '0', '2011-09-01 11:46:40');
INSERT INTO `t_log_user_onlineamount` VALUES ('68', '0', '2011-09-01 11:47:41');
INSERT INTO `t_log_user_onlineamount` VALUES ('69', '0', '2011-09-01 11:48:42');
INSERT INTO `t_log_user_onlineamount` VALUES ('70', '0', '2011-09-01 11:49:43');
INSERT INTO `t_log_user_onlineamount` VALUES ('71', '0', '2011-09-01 11:50:44');
INSERT INTO `t_log_user_onlineamount` VALUES ('72', '0', '2011-09-01 11:51:45');
INSERT INTO `t_log_user_onlineamount` VALUES ('73', '0', '2011-09-01 11:52:46');
INSERT INTO `t_log_user_onlineamount` VALUES ('74', '0', '2011-09-01 11:53:47');
INSERT INTO `t_log_user_onlineamount` VALUES ('75', '0', '2011-09-01 11:54:48');
INSERT INTO `t_log_user_onlineamount` VALUES ('76', '0', '2011-09-01 11:55:49');
INSERT INTO `t_log_user_onlineamount` VALUES ('77', '0', '2011-09-01 11:56:50');
INSERT INTO `t_log_user_onlineamount` VALUES ('78', '0', '2011-09-01 11:57:51');
INSERT INTO `t_log_user_onlineamount` VALUES ('79', '0', '2011-09-01 11:58:52');
INSERT INTO `t_log_user_onlineamount` VALUES ('80', '0', '2011-09-01 11:59:53');
INSERT INTO `t_log_user_onlineamount` VALUES ('81', '0', '2011-09-01 12:00:54');
INSERT INTO `t_log_user_onlineamount` VALUES ('82', '0', '2011-09-01 12:01:55');
INSERT INTO `t_log_user_onlineamount` VALUES ('83', '0', '2011-09-01 12:02:56');
INSERT INTO `t_log_user_onlineamount` VALUES ('84', '0', '2011-09-01 12:03:57');
INSERT INTO `t_log_user_onlineamount` VALUES ('85', '0', '2011-09-01 12:04:58');
INSERT INTO `t_log_user_onlineamount` VALUES ('86', '0', '2011-09-01 12:05:59');
INSERT INTO `t_log_user_onlineamount` VALUES ('87', '0', '2011-09-01 12:07:00');
INSERT INTO `t_log_user_onlineamount` VALUES ('88', '0', '2011-09-01 12:08:01');
INSERT INTO `t_log_user_onlineamount` VALUES ('89', '0', '2011-09-01 12:09:02');
INSERT INTO `t_log_user_onlineamount` VALUES ('90', '0', '2011-09-01 12:10:03');
INSERT INTO `t_log_user_onlineamount` VALUES ('91', '0', '2011-09-01 12:21:25');
INSERT INTO `t_log_user_onlineamount` VALUES ('92', '0', '2011-09-01 12:22:26');
INSERT INTO `t_log_user_onlineamount` VALUES ('93', '0', '2011-09-01 12:23:27');
INSERT INTO `t_log_user_onlineamount` VALUES ('94', '0', '2011-09-01 12:24:28');
INSERT INTO `t_log_user_onlineamount` VALUES ('95', '0', '2011-09-01 12:25:29');
INSERT INTO `t_log_user_onlineamount` VALUES ('96', '0', '2011-09-01 12:43:31');
INSERT INTO `t_log_user_onlineamount` VALUES ('97', '0', '2011-09-01 12:44:32');
INSERT INTO `t_log_user_onlineamount` VALUES ('98', '0', '2011-09-01 12:45:33');
INSERT INTO `t_log_user_onlineamount` VALUES ('99', '0', '2011-09-01 12:46:34');
INSERT INTO `t_log_user_onlineamount` VALUES ('100', '0', '2011-09-01 12:47:35');
INSERT INTO `t_log_user_onlineamount` VALUES ('101', '0', '2011-09-01 12:48:36');
INSERT INTO `t_log_user_onlineamount` VALUES ('102', '0', '2011-09-01 12:49:37');
INSERT INTO `t_log_user_onlineamount` VALUES ('103', '0', '2011-09-01 12:50:38');
INSERT INTO `t_log_user_onlineamount` VALUES ('104', '0', '2011-09-01 12:51:39');
INSERT INTO `t_log_user_onlineamount` VALUES ('105', '0', '2011-09-01 12:52:40');
INSERT INTO `t_log_user_onlineamount` VALUES ('106', '0', '2011-09-01 12:53:41');
INSERT INTO `t_log_user_onlineamount` VALUES ('107', '0', '2011-09-01 12:54:42');
INSERT INTO `t_log_user_onlineamount` VALUES ('108', '0', '2011-09-01 12:55:43');
INSERT INTO `t_log_user_onlineamount` VALUES ('109', '0', '2011-09-01 12:56:44');
INSERT INTO `t_log_user_onlineamount` VALUES ('110', '0', '2011-09-01 12:57:45');
INSERT INTO `t_log_user_onlineamount` VALUES ('111', '0', '2011-09-01 12:58:46');
INSERT INTO `t_log_user_onlineamount` VALUES ('112', '0', '2011-09-01 12:59:47');
INSERT INTO `t_log_user_onlineamount` VALUES ('113', '0', '2011-09-01 13:00:48');
INSERT INTO `t_log_user_onlineamount` VALUES ('114', '0', '2011-09-01 13:01:49');
INSERT INTO `t_log_user_onlineamount` VALUES ('115', '0', '2011-09-01 13:02:50');
INSERT INTO `t_log_user_onlineamount` VALUES ('116', '0', '2011-09-01 13:03:51');
INSERT INTO `t_log_user_onlineamount` VALUES ('117', '0', '2011-09-01 13:04:52');
INSERT INTO `t_log_user_onlineamount` VALUES ('118', '0', '2011-09-01 13:15:27');
INSERT INTO `t_log_user_onlineamount` VALUES ('119', '0', '2011-09-01 13:16:28');
INSERT INTO `t_log_user_onlineamount` VALUES ('120', '0', '2011-09-01 13:17:29');
INSERT INTO `t_log_user_onlineamount` VALUES ('121', '0', '2011-09-01 13:18:30');
INSERT INTO `t_log_user_onlineamount` VALUES ('122', '0', '2011-09-01 13:19:31');
INSERT INTO `t_log_user_onlineamount` VALUES ('123', '0', '2011-09-01 13:20:32');
INSERT INTO `t_log_user_onlineamount` VALUES ('124', '1', '2011-09-01 13:21:33');
INSERT INTO `t_log_user_onlineamount` VALUES ('125', '1', '2011-09-01 13:22:34');
INSERT INTO `t_log_user_onlineamount` VALUES ('126', '1', '2011-09-01 13:23:35');
INSERT INTO `t_log_user_onlineamount` VALUES ('127', '1', '2011-09-01 13:24:36');
INSERT INTO `t_log_user_onlineamount` VALUES ('128', '1', '2011-09-01 13:25:37');
INSERT INTO `t_log_user_onlineamount` VALUES ('129', '1', '2011-09-01 13:26:38');
INSERT INTO `t_log_user_onlineamount` VALUES ('130', '1', '2011-09-01 13:27:39');
INSERT INTO `t_log_user_onlineamount` VALUES ('131', '1', '2011-09-01 13:28:40');
INSERT INTO `t_log_user_onlineamount` VALUES ('132', '0', '2011-09-01 13:29:41');
INSERT INTO `t_log_user_onlineamount` VALUES ('133', '0', '2011-09-01 13:30:42');
INSERT INTO `t_log_user_onlineamount` VALUES ('134', '0', '2011-09-01 13:31:43');
INSERT INTO `t_log_user_onlineamount` VALUES ('135', '0', '2011-09-01 13:32:44');
INSERT INTO `t_log_user_onlineamount` VALUES ('136', '0', '2011-09-01 13:33:45');
INSERT INTO `t_log_user_onlineamount` VALUES ('137', '0', '2011-09-01 13:34:46');
INSERT INTO `t_log_user_onlineamount` VALUES ('138', '0', '2011-09-01 13:35:47');
INSERT INTO `t_log_user_onlineamount` VALUES ('139', '0', '2011-09-01 13:36:48');
INSERT INTO `t_log_user_onlineamount` VALUES ('140', '1', '2011-09-01 13:37:49');
INSERT INTO `t_log_user_onlineamount` VALUES ('141', '1', '2011-09-01 13:38:50');
INSERT INTO `t_log_user_onlineamount` VALUES ('142', '1', '2011-09-01 13:39:51');
INSERT INTO `t_log_user_onlineamount` VALUES ('143', '1', '2011-09-01 13:40:52');
INSERT INTO `t_log_user_onlineamount` VALUES ('144', '0', '2011-09-01 13:41:53');
INSERT INTO `t_log_user_onlineamount` VALUES ('145', '1', '2011-09-01 13:42:54');
INSERT INTO `t_log_user_onlineamount` VALUES ('146', '1', '2011-09-01 13:43:55');
INSERT INTO `t_log_user_onlineamount` VALUES ('147', '1', '2011-09-01 13:44:56');
INSERT INTO `t_log_user_onlineamount` VALUES ('148', '0', '2011-09-01 13:45:57');
INSERT INTO `t_log_user_onlineamount` VALUES ('149', '0', '2011-09-01 13:46:58');
INSERT INTO `t_log_user_onlineamount` VALUES ('150', '1', '2011-09-01 13:47:59');
INSERT INTO `t_log_user_onlineamount` VALUES ('151', '1', '2011-09-01 13:49:00');
INSERT INTO `t_log_user_onlineamount` VALUES ('152', '1', '2011-09-01 13:50:01');
INSERT INTO `t_log_user_onlineamount` VALUES ('153', '1', '2011-09-01 13:51:02');
INSERT INTO `t_log_user_onlineamount` VALUES ('154', '1', '2011-09-01 13:52:03');
INSERT INTO `t_log_user_onlineamount` VALUES ('155', '0', '2011-09-01 13:53:10');
INSERT INTO `t_log_user_onlineamount` VALUES ('156', '1', '2011-09-01 13:55:33');
INSERT INTO `t_log_user_onlineamount` VALUES ('157', '0', '2011-09-01 13:56:34');
INSERT INTO `t_log_user_onlineamount` VALUES ('158', '0', '2011-09-01 13:57:35');
INSERT INTO `t_log_user_onlineamount` VALUES ('159', '0', '2011-09-01 14:02:19');
INSERT INTO `t_log_user_onlineamount` VALUES ('160', '0', '2011-09-01 14:03:20');
INSERT INTO `t_log_user_onlineamount` VALUES ('161', '0', '2011-09-01 14:04:21');
INSERT INTO `t_log_user_onlineamount` VALUES ('162', '1', '2011-09-01 14:05:23');
INSERT INTO `t_log_user_onlineamount` VALUES ('163', '1', '2011-09-01 14:06:24');
INSERT INTO `t_log_user_onlineamount` VALUES ('164', '1', '2011-09-01 14:07:25');
INSERT INTO `t_log_user_onlineamount` VALUES ('165', '0', '2011-09-01 14:12:26');
INSERT INTO `t_log_user_onlineamount` VALUES ('166', '1', '2011-09-01 14:14:03');
INSERT INTO `t_log_user_onlineamount` VALUES ('167', '1', '2011-09-01 14:24:35');
INSERT INTO `t_log_user_onlineamount` VALUES ('168', '0', '2011-09-01 14:33:38');
INSERT INTO `t_log_user_onlineamount` VALUES ('169', '0', '2011-09-01 14:42:49');
INSERT INTO `t_log_user_onlineamount` VALUES ('170', '0', '2011-09-01 14:47:55');
INSERT INTO `t_log_user_onlineamount` VALUES ('171', '0', '2011-09-01 14:48:56');
INSERT INTO `t_log_user_onlineamount` VALUES ('172', '1', '2011-09-01 14:49:57');
INSERT INTO `t_log_user_onlineamount` VALUES ('173', '1', '2011-09-01 14:50:58');
INSERT INTO `t_log_user_onlineamount` VALUES ('174', '1', '2011-09-01 14:51:59');
INSERT INTO `t_log_user_onlineamount` VALUES ('175', '1', '2011-09-01 14:55:15');
INSERT INTO `t_log_user_onlineamount` VALUES ('176', '1', '2011-09-01 14:56:16');
INSERT INTO `t_log_user_onlineamount` VALUES ('177', '1', '2011-09-01 14:57:17');
INSERT INTO `t_log_user_onlineamount` VALUES ('178', '1', '2011-09-01 14:58:18');
INSERT INTO `t_log_user_onlineamount` VALUES ('179', '1', '2011-09-01 14:59:19');
INSERT INTO `t_log_user_onlineamount` VALUES ('180', '1', '2011-09-01 15:00:33');
INSERT INTO `t_log_user_onlineamount` VALUES ('181', '1', '2011-09-01 15:01:35');
INSERT INTO `t_log_user_onlineamount` VALUES ('182', '0', '2011-09-01 15:02:36');
INSERT INTO `t_log_user_onlineamount` VALUES ('183', '0', '2011-09-01 15:04:07');
INSERT INTO `t_log_user_onlineamount` VALUES ('184', '0', '2011-09-01 15:05:08');
INSERT INTO `t_log_user_onlineamount` VALUES ('185', '0', '2011-09-01 15:06:09');
INSERT INTO `t_log_user_onlineamount` VALUES ('186', '0', '2011-09-01 15:07:10');
INSERT INTO `t_log_user_onlineamount` VALUES ('187', '0', '2011-09-01 15:08:11');
INSERT INTO `t_log_user_onlineamount` VALUES ('188', '0', '2011-09-01 15:09:12');
INSERT INTO `t_log_user_onlineamount` VALUES ('189', '0', '2011-09-01 15:10:13');
INSERT INTO `t_log_user_onlineamount` VALUES ('190', '0', '2011-09-01 15:11:14');
INSERT INTO `t_log_user_onlineamount` VALUES ('191', '0', '2011-09-01 15:12:15');
INSERT INTO `t_log_user_onlineamount` VALUES ('192', '0', '2011-09-01 15:13:16');
INSERT INTO `t_log_user_onlineamount` VALUES ('193', '0', '2011-09-01 15:14:17');
INSERT INTO `t_log_user_onlineamount` VALUES ('194', '0', '2011-09-01 15:15:18');
INSERT INTO `t_log_user_onlineamount` VALUES ('195', '0', '2011-09-01 15:16:19');
INSERT INTO `t_log_user_onlineamount` VALUES ('196', '0', '2011-09-01 15:17:20');
INSERT INTO `t_log_user_onlineamount` VALUES ('197', '0', '2011-09-01 15:18:21');
INSERT INTO `t_log_user_onlineamount` VALUES ('198', '0', '2011-09-01 15:19:22');
INSERT INTO `t_log_user_onlineamount` VALUES ('199', '0', '2011-09-01 15:20:23');
INSERT INTO `t_log_user_onlineamount` VALUES ('200', '0', '2011-09-01 15:21:24');
INSERT INTO `t_log_user_onlineamount` VALUES ('201', '0', '2011-09-01 15:22:25');
INSERT INTO `t_log_user_onlineamount` VALUES ('202', '0', '2011-09-01 15:23:26');
INSERT INTO `t_log_user_onlineamount` VALUES ('203', '0', '2011-09-01 15:24:27');
INSERT INTO `t_log_user_onlineamount` VALUES ('204', '0', '2011-09-01 15:25:28');
INSERT INTO `t_log_user_onlineamount` VALUES ('205', '0', '2011-09-01 15:26:29');
INSERT INTO `t_log_user_onlineamount` VALUES ('206', '0', '2011-09-01 15:27:30');
INSERT INTO `t_log_user_onlineamount` VALUES ('207', '0', '2011-09-01 15:28:31');
INSERT INTO `t_log_user_onlineamount` VALUES ('208', '0', '2011-09-01 15:29:32');
INSERT INTO `t_log_user_onlineamount` VALUES ('209', '0', '2011-09-01 15:30:33');
INSERT INTO `t_log_user_onlineamount` VALUES ('210', '0', '2011-09-01 15:31:34');
INSERT INTO `t_log_user_onlineamount` VALUES ('211', '0', '2011-09-01 15:32:35');
INSERT INTO `t_log_user_onlineamount` VALUES ('212', '0', '2011-09-01 15:33:36');
INSERT INTO `t_log_user_onlineamount` VALUES ('213', '0', '2011-09-01 15:34:37');
INSERT INTO `t_log_user_onlineamount` VALUES ('214', '0', '2011-09-01 15:35:38');
INSERT INTO `t_log_user_onlineamount` VALUES ('215', '0', '2011-09-01 15:36:39');
INSERT INTO `t_log_user_onlineamount` VALUES ('216', '0', '2011-09-01 15:37:40');
INSERT INTO `t_log_user_onlineamount` VALUES ('217', '0', '2011-09-01 22:40:39');
INSERT INTO `t_log_user_onlineamount` VALUES ('218', '0', '2011-09-01 22:41:40');
INSERT INTO `t_log_user_onlineamount` VALUES ('219', '0', '2011-09-01 22:42:41');
INSERT INTO `t_log_user_onlineamount` VALUES ('220', '0', '2011-09-01 22:43:42');
INSERT INTO `t_log_user_onlineamount` VALUES ('221', '0', '2011-09-01 22:44:43');
INSERT INTO `t_log_user_onlineamount` VALUES ('222', '1', '2011-09-01 22:45:44');
INSERT INTO `t_log_user_onlineamount` VALUES ('223', '0', '2011-09-01 22:46:45');
INSERT INTO `t_log_user_onlineamount` VALUES ('224', '0', '2011-09-01 22:47:46');
INSERT INTO `t_log_user_onlineamount` VALUES ('225', '0', '2011-09-01 22:48:47');
INSERT INTO `t_log_user_onlineamount` VALUES ('226', '0', '2011-09-01 22:49:48');
INSERT INTO `t_log_user_onlineamount` VALUES ('227', '0', '2011-09-01 22:50:49');
INSERT INTO `t_log_user_onlineamount` VALUES ('228', '0', '2011-09-01 22:51:50');
INSERT INTO `t_log_user_onlineamount` VALUES ('229', '0', '2011-09-01 22:52:51');
INSERT INTO `t_log_user_onlineamount` VALUES ('230', '0', '2011-09-01 22:53:52');
INSERT INTO `t_log_user_onlineamount` VALUES ('231', '0', '2011-09-01 22:54:53');
INSERT INTO `t_log_user_onlineamount` VALUES ('232', '0', '2011-09-01 22:55:54');
INSERT INTO `t_log_user_onlineamount` VALUES ('233', '0', '2011-09-01 22:56:55');
INSERT INTO `t_log_user_onlineamount` VALUES ('234', '0', '2011-09-01 22:57:56');
INSERT INTO `t_log_user_onlineamount` VALUES ('235', '0', '2011-09-01 22:58:57');
INSERT INTO `t_log_user_onlineamount` VALUES ('236', '0', '2011-09-01 22:59:58');
INSERT INTO `t_log_user_onlineamount` VALUES ('237', '0', '2011-09-01 23:00:59');
INSERT INTO `t_log_user_onlineamount` VALUES ('238', '0', '2011-09-01 23:02:00');
INSERT INTO `t_log_user_onlineamount` VALUES ('239', '0', '2011-09-01 23:03:01');
INSERT INTO `t_log_user_onlineamount` VALUES ('240', '0', '2011-09-01 23:04:02');
INSERT INTO `t_log_user_onlineamount` VALUES ('241', '0', '2011-09-01 23:05:03');
INSERT INTO `t_log_user_onlineamount` VALUES ('242', '0', '2011-09-01 23:06:04');
INSERT INTO `t_log_user_onlineamount` VALUES ('243', '0', '2011-09-01 23:07:05');
INSERT INTO `t_log_user_onlineamount` VALUES ('244', '0', '2011-09-01 23:08:06');
INSERT INTO `t_log_user_onlineamount` VALUES ('245', '0', '2011-09-01 23:09:07');
INSERT INTO `t_log_user_onlineamount` VALUES ('246', '0', '2011-09-01 23:10:08');
INSERT INTO `t_log_user_onlineamount` VALUES ('247', '0', '2011-09-01 23:11:09');
INSERT INTO `t_log_user_onlineamount` VALUES ('248', '0', '2011-09-01 23:12:10');
INSERT INTO `t_log_user_onlineamount` VALUES ('249', '0', '2011-09-01 23:13:11');
INSERT INTO `t_log_user_onlineamount` VALUES ('250', '0', '2011-09-01 23:14:12');
INSERT INTO `t_log_user_onlineamount` VALUES ('251', '0', '2011-09-01 23:15:13');
INSERT INTO `t_log_user_onlineamount` VALUES ('252', '0', '2011-09-01 23:16:14');
INSERT INTO `t_log_user_onlineamount` VALUES ('253', '0', '2011-09-01 23:17:15');
INSERT INTO `t_log_user_onlineamount` VALUES ('254', '0', '2011-09-01 23:18:16');
INSERT INTO `t_log_user_onlineamount` VALUES ('255', '0', '2011-09-01 23:19:17');
INSERT INTO `t_log_user_onlineamount` VALUES ('256', '0', '2011-09-01 23:20:18');
INSERT INTO `t_log_user_onlineamount` VALUES ('257', '0', '2011-09-01 23:21:19');
INSERT INTO `t_log_user_onlineamount` VALUES ('258', '0', '2011-09-01 23:22:20');
INSERT INTO `t_log_user_onlineamount` VALUES ('259', '0', '2011-09-01 23:23:21');
INSERT INTO `t_log_user_onlineamount` VALUES ('260', '0', '2011-09-01 23:24:22');
INSERT INTO `t_log_user_onlineamount` VALUES ('261', '0', '2011-09-01 23:25:23');
INSERT INTO `t_log_user_onlineamount` VALUES ('262', '0', '2011-09-01 23:26:24');
INSERT INTO `t_log_user_onlineamount` VALUES ('263', '0', '2011-09-01 23:27:25');
INSERT INTO `t_log_user_onlineamount` VALUES ('264', '0', '2011-09-01 23:28:26');
INSERT INTO `t_log_user_onlineamount` VALUES ('265', '0', '2011-09-01 23:29:27');
INSERT INTO `t_log_user_onlineamount` VALUES ('266', '0', '2011-09-01 23:30:28');
INSERT INTO `t_log_user_onlineamount` VALUES ('267', '0', '2011-09-01 23:31:29');
INSERT INTO `t_log_user_onlineamount` VALUES ('268', '0', '2011-09-01 23:32:30');
INSERT INTO `t_log_user_onlineamount` VALUES ('269', '0', '2011-09-01 23:33:31');
INSERT INTO `t_log_user_onlineamount` VALUES ('270', '0', '2011-09-01 23:34:32');
INSERT INTO `t_log_user_onlineamount` VALUES ('271', '0', '2011-09-01 23:35:33');
INSERT INTO `t_log_user_onlineamount` VALUES ('272', '0', '2011-09-01 23:36:34');
INSERT INTO `t_log_user_onlineamount` VALUES ('273', '0', '2011-09-01 23:37:35');
INSERT INTO `t_log_user_onlineamount` VALUES ('274', '0', '2011-09-01 23:38:36');
INSERT INTO `t_log_user_onlineamount` VALUES ('275', '0', '2011-09-01 23:39:37');
INSERT INTO `t_log_user_onlineamount` VALUES ('276', '0', '2011-09-01 23:40:38');
INSERT INTO `t_log_user_onlineamount` VALUES ('277', '0', '2011-09-01 23:43:50');
INSERT INTO `t_log_user_onlineamount` VALUES ('278', '0', '2011-09-01 23:44:51');
INSERT INTO `t_log_user_onlineamount` VALUES ('279', '1', '2011-09-01 23:45:52');
INSERT INTO `t_log_user_onlineamount` VALUES ('280', '1', '2011-09-01 23:46:53');
INSERT INTO `t_log_user_onlineamount` VALUES ('281', '1', '2011-09-01 23:47:54');
INSERT INTO `t_log_user_onlineamount` VALUES ('282', '1', '2011-09-01 23:48:55');
INSERT INTO `t_log_user_onlineamount` VALUES ('283', '1', '2011-09-01 23:49:56');
INSERT INTO `t_log_user_onlineamount` VALUES ('284', '1', '2011-09-01 23:50:57');
INSERT INTO `t_log_user_onlineamount` VALUES ('285', '1', '2011-09-01 23:51:58');
INSERT INTO `t_log_user_onlineamount` VALUES ('286', '1', '2011-09-01 23:52:59');
INSERT INTO `t_log_user_onlineamount` VALUES ('287', '1', '2011-09-01 23:54:00');
INSERT INTO `t_log_user_onlineamount` VALUES ('288', '1', '2011-09-01 23:55:01');
INSERT INTO `t_log_user_onlineamount` VALUES ('289', '1', '2011-09-01 23:56:02');
INSERT INTO `t_log_user_onlineamount` VALUES ('290', '1', '2011-09-01 23:57:03');
INSERT INTO `t_log_user_onlineamount` VALUES ('291', '1', '2011-09-01 23:58:04');
INSERT INTO `t_log_user_onlineamount` VALUES ('292', '1', '2011-09-01 23:59:05');
INSERT INTO `t_log_user_onlineamount` VALUES ('293', '1', '2011-09-02 00:00:06');
INSERT INTO `t_log_user_onlineamount` VALUES ('294', '1', '2011-09-02 00:01:07');
INSERT INTO `t_log_user_onlineamount` VALUES ('295', '1', '2011-09-02 00:02:08');
INSERT INTO `t_log_user_onlineamount` VALUES ('296', '1', '2011-09-02 00:03:09');
INSERT INTO `t_log_user_onlineamount` VALUES ('297', '1', '2011-09-02 00:04:10');
INSERT INTO `t_log_user_onlineamount` VALUES ('298', '1', '2011-09-02 00:05:11');
INSERT INTO `t_log_user_onlineamount` VALUES ('299', '1', '2011-09-02 00:06:12');
INSERT INTO `t_log_user_onlineamount` VALUES ('300', '1', '2011-09-02 00:07:13');
INSERT INTO `t_log_user_onlineamount` VALUES ('301', '1', '2011-09-02 00:08:14');
INSERT INTO `t_log_user_onlineamount` VALUES ('302', '1', '2011-09-02 00:09:15');
INSERT INTO `t_log_user_onlineamount` VALUES ('303', '1', '2011-09-02 00:10:16');
INSERT INTO `t_log_user_onlineamount` VALUES ('304', '1', '2011-09-02 00:11:17');
INSERT INTO `t_log_user_onlineamount` VALUES ('305', '1', '2011-09-02 00:12:18');
INSERT INTO `t_log_user_onlineamount` VALUES ('306', '1', '2011-09-02 00:13:19');
INSERT INTO `t_log_user_onlineamount` VALUES ('307', '1', '2011-09-02 00:14:20');
INSERT INTO `t_log_user_onlineamount` VALUES ('308', '0', '2011-09-02 03:43:01');
INSERT INTO `t_log_user_onlineamount` VALUES ('309', '1', '2011-09-02 03:44:02');
INSERT INTO `t_log_user_onlineamount` VALUES ('310', '1', '2011-09-02 03:45:03');
INSERT INTO `t_log_user_onlineamount` VALUES ('311', '1', '2011-09-02 03:46:04');
INSERT INTO `t_log_user_onlineamount` VALUES ('312', '0', '2011-09-02 03:47:05');
INSERT INTO `t_log_user_onlineamount` VALUES ('313', '0', '2011-09-02 03:48:06');
INSERT INTO `t_log_user_onlineamount` VALUES ('314', '1', '2011-09-02 03:49:07');
INSERT INTO `t_log_user_onlineamount` VALUES ('315', '1', '2011-09-02 03:50:08');
INSERT INTO `t_log_user_onlineamount` VALUES ('316', '1', '2011-09-02 03:51:09');
INSERT INTO `t_log_user_onlineamount` VALUES ('317', '1', '2011-09-02 03:52:10');
INSERT INTO `t_log_user_onlineamount` VALUES ('318', '0', '2011-09-02 03:53:11');
INSERT INTO `t_log_user_onlineamount` VALUES ('319', '0', '2011-09-02 03:54:12');
INSERT INTO `t_log_user_onlineamount` VALUES ('320', '1', '2011-09-02 03:55:13');
INSERT INTO `t_log_user_onlineamount` VALUES ('321', '1', '2011-09-02 03:56:14');
INSERT INTO `t_log_user_onlineamount` VALUES ('322', '1', '2011-09-02 03:57:15');
INSERT INTO `t_log_user_onlineamount` VALUES ('323', '1', '2011-09-02 03:58:16');
INSERT INTO `t_log_user_onlineamount` VALUES ('324', '1', '2011-09-02 03:59:17');
INSERT INTO `t_log_user_onlineamount` VALUES ('325', '1', '2011-09-02 04:00:18');
INSERT INTO `t_log_user_onlineamount` VALUES ('326', '0', '2011-09-02 04:01:19');
INSERT INTO `t_log_user_onlineamount` VALUES ('327', '1', '2011-09-02 04:02:20');
INSERT INTO `t_log_user_onlineamount` VALUES ('328', '1', '2011-09-02 04:03:21');
INSERT INTO `t_log_user_onlineamount` VALUES ('329', '0', '2011-09-02 04:04:22');
INSERT INTO `t_log_user_onlineamount` VALUES ('330', '0', '2011-09-02 04:05:23');
INSERT INTO `t_log_user_onlineamount` VALUES ('331', '0', '2011-09-02 04:06:24');
INSERT INTO `t_log_user_onlineamount` VALUES ('332', '0', '2011-09-02 04:07:25');
INSERT INTO `t_log_user_onlineamount` VALUES ('333', '0', '2011-09-02 04:08:26');
INSERT INTO `t_log_user_onlineamount` VALUES ('334', '0', '2011-09-02 04:09:27');
INSERT INTO `t_log_user_onlineamount` VALUES ('335', '1', '2011-09-02 04:10:28');
INSERT INTO `t_log_user_onlineamount` VALUES ('336', '0', '2011-09-02 04:11:29');
INSERT INTO `t_log_user_onlineamount` VALUES ('337', '0', '2011-09-02 04:12:30');
INSERT INTO `t_log_user_onlineamount` VALUES ('338', '0', '2011-09-02 04:13:31');
INSERT INTO `t_log_user_onlineamount` VALUES ('339', '0', '2011-09-02 04:14:32');
INSERT INTO `t_log_user_onlineamount` VALUES ('340', '0', '2011-09-02 04:15:33');
INSERT INTO `t_log_user_onlineamount` VALUES ('341', '0', '2011-09-02 04:16:34');
INSERT INTO `t_log_user_onlineamount` VALUES ('342', '0', '2011-09-02 04:17:35');
INSERT INTO `t_log_user_onlineamount` VALUES ('343', '0', '2011-09-02 04:18:36');
INSERT INTO `t_log_user_onlineamount` VALUES ('344', '0', '2011-09-02 04:19:37');
INSERT INTO `t_log_user_onlineamount` VALUES ('345', '0', '2011-09-02 04:20:38');
INSERT INTO `t_log_user_onlineamount` VALUES ('346', '0', '2011-09-02 04:21:39');
INSERT INTO `t_log_user_onlineamount` VALUES ('347', '0', '2011-09-02 04:22:40');
INSERT INTO `t_log_user_onlineamount` VALUES ('348', '0', '2011-09-02 04:23:41');
INSERT INTO `t_log_user_onlineamount` VALUES ('349', '0', '2011-09-02 04:24:42');
INSERT INTO `t_log_user_onlineamount` VALUES ('350', '0', '2011-09-02 04:25:43');
INSERT INTO `t_log_user_onlineamount` VALUES ('351', '0', '2011-09-02 04:26:44');
INSERT INTO `t_log_user_onlineamount` VALUES ('352', '0', '2011-09-02 04:27:45');
INSERT INTO `t_log_user_onlineamount` VALUES ('353', '0', '2011-09-02 04:28:46');
INSERT INTO `t_log_user_onlineamount` VALUES ('354', '0', '2011-09-02 04:29:47');
INSERT INTO `t_log_user_onlineamount` VALUES ('355', '1', '2011-09-02 04:30:48');
INSERT INTO `t_log_user_onlineamount` VALUES ('356', '0', '2011-09-02 04:31:49');
INSERT INTO `t_log_user_onlineamount` VALUES ('357', '0', '2011-09-02 04:32:50');
INSERT INTO `t_log_user_onlineamount` VALUES ('358', '0', '2011-09-02 04:34:07');
INSERT INTO `t_log_user_onlineamount` VALUES ('359', '1', '2011-09-02 04:35:08');
INSERT INTO `t_log_user_onlineamount` VALUES ('360', '0', '2011-09-02 04:36:09');
INSERT INTO `t_log_user_onlineamount` VALUES ('361', '0', '2011-09-02 04:38:25');
INSERT INTO `t_log_user_onlineamount` VALUES ('362', '1', '2011-09-02 04:39:26');
INSERT INTO `t_log_user_onlineamount` VALUES ('363', '0', '2011-09-02 04:40:27');
INSERT INTO `t_log_user_onlineamount` VALUES ('364', '0', '2011-09-02 04:41:28');
INSERT INTO `t_log_user_onlineamount` VALUES ('365', '1', '2011-09-02 04:42:29');
INSERT INTO `t_log_user_onlineamount` VALUES ('366', '0', '2011-09-02 04:43:30');
INSERT INTO `t_log_user_onlineamount` VALUES ('367', '0', '2011-09-02 04:44:31');
INSERT INTO `t_log_user_onlineamount` VALUES ('368', '0', '2011-09-02 04:45:32');
INSERT INTO `t_log_user_onlineamount` VALUES ('369', '0', '2011-09-02 04:46:33');
INSERT INTO `t_log_user_onlineamount` VALUES ('370', '0', '2011-09-02 04:47:34');
INSERT INTO `t_log_user_onlineamount` VALUES ('371', '0', '2011-09-02 04:48:35');
INSERT INTO `t_log_user_onlineamount` VALUES ('372', '0', '2011-09-02 04:49:36');
INSERT INTO `t_log_user_onlineamount` VALUES ('373', '0', '2011-09-02 04:50:37');
INSERT INTO `t_log_user_onlineamount` VALUES ('374', '0', '2011-09-02 04:51:38');
INSERT INTO `t_log_user_onlineamount` VALUES ('375', '0', '2011-09-02 04:52:39');
INSERT INTO `t_log_user_onlineamount` VALUES ('376', '0', '2011-09-02 04:53:40');
INSERT INTO `t_log_user_onlineamount` VALUES ('377', '0', '2011-09-02 04:54:41');
INSERT INTO `t_log_user_onlineamount` VALUES ('378', '0', '2011-09-02 04:55:42');
INSERT INTO `t_log_user_onlineamount` VALUES ('379', '1', '2011-09-02 04:56:43');
INSERT INTO `t_log_user_onlineamount` VALUES ('380', '1', '2011-09-02 04:57:44');
INSERT INTO `t_log_user_onlineamount` VALUES ('381', '0', '2011-09-02 04:58:45');
INSERT INTO `t_log_user_onlineamount` VALUES ('382', '0', '2011-09-02 04:59:46');
INSERT INTO `t_log_user_onlineamount` VALUES ('383', '0', '2011-09-02 05:00:47');
INSERT INTO `t_log_user_onlineamount` VALUES ('384', '1', '2011-09-02 05:01:48');
INSERT INTO `t_log_user_onlineamount` VALUES ('385', '1', '2011-09-02 05:02:49');
INSERT INTO `t_log_user_onlineamount` VALUES ('386', '1', '2011-09-02 05:03:50');
INSERT INTO `t_log_user_onlineamount` VALUES ('387', '0', '2011-09-02 05:04:51');
INSERT INTO `t_log_user_onlineamount` VALUES ('388', '1', '2011-09-02 05:05:52');
INSERT INTO `t_log_user_onlineamount` VALUES ('389', '0', '2011-09-02 05:06:53');
INSERT INTO `t_log_user_onlineamount` VALUES ('390', '0', '2011-09-02 05:07:54');
INSERT INTO `t_log_user_onlineamount` VALUES ('391', '0', '2011-09-02 05:08:55');
INSERT INTO `t_log_user_onlineamount` VALUES ('392', '0', '2011-09-02 05:09:56');
INSERT INTO `t_log_user_onlineamount` VALUES ('393', '0', '2011-09-02 05:10:57');
INSERT INTO `t_log_user_onlineamount` VALUES ('394', '0', '2011-09-02 05:11:58');
INSERT INTO `t_log_user_onlineamount` VALUES ('395', '0', '2011-09-02 05:12:59');
INSERT INTO `t_log_user_onlineamount` VALUES ('396', '0', '2011-09-02 05:14:00');
INSERT INTO `t_log_user_onlineamount` VALUES ('397', '1', '2011-09-02 05:15:01');
INSERT INTO `t_log_user_onlineamount` VALUES ('398', '0', '2011-09-02 05:16:02');
INSERT INTO `t_log_user_onlineamount` VALUES ('399', '0', '2011-09-02 05:17:03');
INSERT INTO `t_log_user_onlineamount` VALUES ('400', '0', '2011-09-02 05:18:04');
INSERT INTO `t_log_user_onlineamount` VALUES ('401', '0', '2011-09-02 05:19:05');
INSERT INTO `t_log_user_onlineamount` VALUES ('402', '0', '2011-09-02 05:20:06');
INSERT INTO `t_log_user_onlineamount` VALUES ('403', '0', '2011-09-02 05:21:07');
INSERT INTO `t_log_user_onlineamount` VALUES ('404', '0', '2011-09-02 05:22:08');
INSERT INTO `t_log_user_onlineamount` VALUES ('405', '0', '2011-09-02 05:23:09');
INSERT INTO `t_log_user_onlineamount` VALUES ('406', '0', '2011-09-02 05:24:10');
INSERT INTO `t_log_user_onlineamount` VALUES ('407', '0', '2011-09-02 05:25:11');
INSERT INTO `t_log_user_onlineamount` VALUES ('408', '0', '2011-09-02 05:26:12');
INSERT INTO `t_log_user_onlineamount` VALUES ('409', '0', '2011-09-02 05:27:13');
INSERT INTO `t_log_user_onlineamount` VALUES ('410', '0', '2011-09-02 05:28:14');
INSERT INTO `t_log_user_onlineamount` VALUES ('411', '0', '2011-09-02 05:29:15');
INSERT INTO `t_log_user_onlineamount` VALUES ('412', '0', '2011-09-02 05:30:16');
INSERT INTO `t_log_user_onlineamount` VALUES ('413', '0', '2011-09-02 05:31:17');
INSERT INTO `t_log_user_onlineamount` VALUES ('414', '0', '2011-09-02 05:32:18');
INSERT INTO `t_log_user_onlineamount` VALUES ('415', '0', '2011-09-02 05:33:19');
INSERT INTO `t_log_user_onlineamount` VALUES ('416', '0', '2011-09-02 05:34:20');
INSERT INTO `t_log_user_onlineamount` VALUES ('417', '0', '2011-09-02 05:35:21');
INSERT INTO `t_log_user_onlineamount` VALUES ('418', '0', '2011-09-02 05:36:22');
INSERT INTO `t_log_user_onlineamount` VALUES ('419', '0', '2011-09-02 05:37:23');
INSERT INTO `t_log_user_onlineamount` VALUES ('420', '0', '2011-09-02 05:38:24');
INSERT INTO `t_log_user_onlineamount` VALUES ('421', '1', '2011-09-02 05:39:25');
INSERT INTO `t_log_user_onlineamount` VALUES ('422', '0', '2011-09-02 05:40:26');
INSERT INTO `t_log_user_onlineamount` VALUES ('423', '0', '2011-09-02 05:41:27');
INSERT INTO `t_log_user_onlineamount` VALUES ('424', '0', '2011-09-02 05:42:28');
INSERT INTO `t_log_user_onlineamount` VALUES ('425', '0', '2011-09-02 05:43:29');
INSERT INTO `t_log_user_onlineamount` VALUES ('426', '0', '2011-09-02 05:44:30');
INSERT INTO `t_log_user_onlineamount` VALUES ('427', '1', '2011-09-02 05:45:31');
INSERT INTO `t_log_user_onlineamount` VALUES ('428', '0', '2011-09-02 05:46:32');
INSERT INTO `t_log_user_onlineamount` VALUES ('429', '0', '2011-09-02 05:47:33');
INSERT INTO `t_log_user_onlineamount` VALUES ('430', '0', '2011-09-02 05:48:34');
INSERT INTO `t_log_user_onlineamount` VALUES ('431', '0', '2011-09-02 05:49:35');
INSERT INTO `t_log_user_onlineamount` VALUES ('432', '0', '2011-09-02 05:50:36');
INSERT INTO `t_log_user_onlineamount` VALUES ('433', '0', '2011-09-02 05:51:37');
INSERT INTO `t_log_user_onlineamount` VALUES ('434', '0', '2011-09-02 05:52:38');
INSERT INTO `t_log_user_onlineamount` VALUES ('435', '0', '2011-09-02 05:53:39');
INSERT INTO `t_log_user_onlineamount` VALUES ('436', '0', '2011-09-02 05:54:40');
INSERT INTO `t_log_user_onlineamount` VALUES ('437', '0', '2011-09-02 05:55:41');
INSERT INTO `t_log_user_onlineamount` VALUES ('438', '0', '2011-09-02 05:56:42');
INSERT INTO `t_log_user_onlineamount` VALUES ('439', '0', '2011-09-02 05:57:43');
INSERT INTO `t_log_user_onlineamount` VALUES ('440', '0', '2011-09-02 05:58:44');
INSERT INTO `t_log_user_onlineamount` VALUES ('441', '0', '2011-09-02 05:59:45');
INSERT INTO `t_log_user_onlineamount` VALUES ('442', '0', '2011-09-02 06:00:46');
INSERT INTO `t_log_user_onlineamount` VALUES ('443', '0', '2011-09-02 06:01:47');
INSERT INTO `t_log_user_onlineamount` VALUES ('444', '0', '2011-09-02 06:02:48');
INSERT INTO `t_log_user_onlineamount` VALUES ('445', '0', '2011-09-02 06:03:49');
INSERT INTO `t_log_user_onlineamount` VALUES ('446', '0', '2011-09-02 06:04:50');
INSERT INTO `t_log_user_onlineamount` VALUES ('447', '0', '2011-09-02 06:05:51');
INSERT INTO `t_log_user_onlineamount` VALUES ('448', '0', '2011-09-02 06:06:52');
INSERT INTO `t_log_user_onlineamount` VALUES ('449', '0', '2011-09-02 06:07:53');
INSERT INTO `t_log_user_onlineamount` VALUES ('450', '0', '2011-09-02 06:08:54');
INSERT INTO `t_log_user_onlineamount` VALUES ('451', '0', '2011-09-02 06:09:55');
INSERT INTO `t_log_user_onlineamount` VALUES ('452', '0', '2011-09-02 06:10:56');
INSERT INTO `t_log_user_onlineamount` VALUES ('453', '1', '2011-09-02 06:11:57');
INSERT INTO `t_log_user_onlineamount` VALUES ('454', '0', '2011-09-02 06:12:58');
INSERT INTO `t_log_user_onlineamount` VALUES ('455', '0', '2011-09-02 06:13:59');
INSERT INTO `t_log_user_onlineamount` VALUES ('456', '0', '2011-09-02 06:15:00');
INSERT INTO `t_log_user_onlineamount` VALUES ('457', '0', '2011-09-02 06:16:01');
INSERT INTO `t_log_user_onlineamount` VALUES ('458', '0', '2011-09-02 06:17:02');
INSERT INTO `t_log_user_onlineamount` VALUES ('459', '0', '2011-09-02 06:18:03');
INSERT INTO `t_log_user_onlineamount` VALUES ('460', '0', '2011-09-02 06:19:04');
INSERT INTO `t_log_user_onlineamount` VALUES ('461', '0', '2011-09-02 06:20:05');
INSERT INTO `t_log_user_onlineamount` VALUES ('462', '0', '2011-09-02 06:21:06');
INSERT INTO `t_log_user_onlineamount` VALUES ('463', '0', '2011-09-02 06:22:07');
INSERT INTO `t_log_user_onlineamount` VALUES ('464', '1', '2011-09-02 06:23:08');
INSERT INTO `t_log_user_onlineamount` VALUES ('465', '1', '2011-09-02 06:24:09');
INSERT INTO `t_log_user_onlineamount` VALUES ('466', '0', '2011-09-02 06:25:10');
INSERT INTO `t_log_user_onlineamount` VALUES ('467', '0', '2011-09-02 06:26:11');
INSERT INTO `t_log_user_onlineamount` VALUES ('468', '0', '2011-09-02 06:27:12');
INSERT INTO `t_log_user_onlineamount` VALUES ('469', '0', '2011-09-02 06:28:13');
INSERT INTO `t_log_user_onlineamount` VALUES ('470', '0', '2011-09-02 06:29:14');
INSERT INTO `t_log_user_onlineamount` VALUES ('471', '0', '2011-09-02 06:30:15');
INSERT INTO `t_log_user_onlineamount` VALUES ('472', '0', '2011-09-02 06:31:16');
INSERT INTO `t_log_user_onlineamount` VALUES ('473', '0', '2011-09-02 06:32:17');
INSERT INTO `t_log_user_onlineamount` VALUES ('474', '0', '2011-09-02 06:33:18');
INSERT INTO `t_log_user_onlineamount` VALUES ('475', '0', '2011-09-02 06:34:19');
INSERT INTO `t_log_user_onlineamount` VALUES ('476', '0', '2011-09-02 06:35:20');
INSERT INTO `t_log_user_onlineamount` VALUES ('477', '0', '2011-09-02 06:36:21');
INSERT INTO `t_log_user_onlineamount` VALUES ('478', '0', '2011-09-02 06:37:22');
INSERT INTO `t_log_user_onlineamount` VALUES ('479', '0', '2011-09-02 06:38:23');
INSERT INTO `t_log_user_onlineamount` VALUES ('480', '0', '2011-09-02 06:41:02');
INSERT INTO `t_log_user_onlineamount` VALUES ('481', '1', '2011-09-02 06:42:03');
INSERT INTO `t_log_user_onlineamount` VALUES ('482', '1', '2011-09-02 06:43:04');
INSERT INTO `t_log_user_onlineamount` VALUES ('483', '0', '2011-09-02 06:44:05');
INSERT INTO `t_log_user_onlineamount` VALUES ('484', '0', '2011-09-02 06:45:06');
INSERT INTO `t_log_user_onlineamount` VALUES ('485', '0', '2011-09-02 06:46:07');
INSERT INTO `t_log_user_onlineamount` VALUES ('486', '0', '2011-09-02 06:47:08');
INSERT INTO `t_log_user_onlineamount` VALUES ('487', '0', '2011-09-02 06:48:09');
INSERT INTO `t_log_user_onlineamount` VALUES ('488', '0', '2011-09-02 06:49:10');
INSERT INTO `t_log_user_onlineamount` VALUES ('489', '1', '2011-09-02 06:50:11');
INSERT INTO `t_log_user_onlineamount` VALUES ('490', '0', '2011-09-02 06:51:12');
INSERT INTO `t_log_user_onlineamount` VALUES ('491', '0', '2011-09-02 06:52:13');
INSERT INTO `t_log_user_onlineamount` VALUES ('492', '0', '2011-09-02 06:53:14');
INSERT INTO `t_log_user_onlineamount` VALUES ('493', '0', '2011-09-02 06:54:15');
INSERT INTO `t_log_user_onlineamount` VALUES ('494', '0', '2011-09-02 06:55:16');
INSERT INTO `t_log_user_onlineamount` VALUES ('495', '0', '2011-09-02 06:56:17');
INSERT INTO `t_log_user_onlineamount` VALUES ('496', '0', '2011-09-02 06:57:18');
INSERT INTO `t_log_user_onlineamount` VALUES ('497', '0', '2011-09-02 06:58:19');
INSERT INTO `t_log_user_onlineamount` VALUES ('498', '0', '2011-09-02 06:59:20');
INSERT INTO `t_log_user_onlineamount` VALUES ('499', '0', '2011-09-02 07:00:21');
INSERT INTO `t_log_user_onlineamount` VALUES ('500', '0', '2011-09-02 07:01:22');
INSERT INTO `t_log_user_onlineamount` VALUES ('501', '0', '2011-09-02 07:02:23');
INSERT INTO `t_log_user_onlineamount` VALUES ('502', '0', '2011-09-02 07:03:24');
INSERT INTO `t_log_user_onlineamount` VALUES ('503', '1', '2011-09-02 07:04:25');
INSERT INTO `t_log_user_onlineamount` VALUES ('504', '0', '2011-09-02 07:05:26');
INSERT INTO `t_log_user_onlineamount` VALUES ('505', '0', '2011-09-02 07:06:27');
INSERT INTO `t_log_user_onlineamount` VALUES ('506', '0', '2011-09-02 07:07:28');
INSERT INTO `t_log_user_onlineamount` VALUES ('507', '0', '2011-09-02 07:08:29');
INSERT INTO `t_log_user_onlineamount` VALUES ('508', '0', '2011-09-02 07:09:30');
INSERT INTO `t_log_user_onlineamount` VALUES ('509', '0', '2011-09-02 07:10:31');
INSERT INTO `t_log_user_onlineamount` VALUES ('510', '0', '2011-09-02 07:11:32');
INSERT INTO `t_log_user_onlineamount` VALUES ('511', '0', '2011-09-02 07:12:33');
INSERT INTO `t_log_user_onlineamount` VALUES ('512', '0', '2011-09-02 07:13:34');
INSERT INTO `t_log_user_onlineamount` VALUES ('513', '1', '2011-09-02 07:14:35');
INSERT INTO `t_log_user_onlineamount` VALUES ('514', '1', '2011-09-02 07:15:36');
INSERT INTO `t_log_user_onlineamount` VALUES ('515', '0', '2011-09-02 07:16:37');
INSERT INTO `t_log_user_onlineamount` VALUES ('516', '0', '2011-09-02 07:17:38');
INSERT INTO `t_log_user_onlineamount` VALUES ('517', '0', '2011-09-02 07:18:39');
INSERT INTO `t_log_user_onlineamount` VALUES ('518', '0', '2011-09-02 07:19:40');
INSERT INTO `t_log_user_onlineamount` VALUES ('519', '1', '2011-09-02 07:20:41');
INSERT INTO `t_log_user_onlineamount` VALUES ('520', '0', '2011-09-02 07:21:42');
INSERT INTO `t_log_user_onlineamount` VALUES ('521', '1', '2011-09-02 07:22:43');
INSERT INTO `t_log_user_onlineamount` VALUES ('522', '0', '2011-09-02 07:23:44');
INSERT INTO `t_log_user_onlineamount` VALUES ('523', '0', '2011-09-02 07:24:45');
INSERT INTO `t_log_user_onlineamount` VALUES ('524', '0', '2011-09-02 07:25:46');
INSERT INTO `t_log_user_onlineamount` VALUES ('525', '0', '2011-09-02 07:26:47');
INSERT INTO `t_log_user_onlineamount` VALUES ('526', '0', '2011-09-02 07:27:48');
INSERT INTO `t_log_user_onlineamount` VALUES ('527', '0', '2011-09-02 07:28:49');
INSERT INTO `t_log_user_onlineamount` VALUES ('528', '0', '2011-09-02 07:29:50');
INSERT INTO `t_log_user_onlineamount` VALUES ('529', '0', '2011-09-02 07:30:51');
INSERT INTO `t_log_user_onlineamount` VALUES ('530', '0', '2011-09-02 07:31:52');
INSERT INTO `t_log_user_onlineamount` VALUES ('531', '0', '2011-09-02 07:32:53');
INSERT INTO `t_log_user_onlineamount` VALUES ('532', '0', '2011-09-02 07:33:54');
INSERT INTO `t_log_user_onlineamount` VALUES ('533', '0', '2011-09-02 07:34:55');
INSERT INTO `t_log_user_onlineamount` VALUES ('534', '0', '2011-09-02 07:35:56');
INSERT INTO `t_log_user_onlineamount` VALUES ('535', '0', '2011-09-02 07:36:57');
INSERT INTO `t_log_user_onlineamount` VALUES ('536', '0', '2011-09-02 07:37:58');
INSERT INTO `t_log_user_onlineamount` VALUES ('537', '0', '2011-09-02 07:38:59');
INSERT INTO `t_log_user_onlineamount` VALUES ('538', '0', '2011-09-02 07:40:00');
INSERT INTO `t_log_user_onlineamount` VALUES ('539', '0', '2011-09-02 07:41:01');
INSERT INTO `t_log_user_onlineamount` VALUES ('540', '0', '2011-09-02 07:42:02');
INSERT INTO `t_log_user_onlineamount` VALUES ('541', '0', '2011-09-02 07:43:03');
INSERT INTO `t_log_user_onlineamount` VALUES ('542', '0', '2011-09-02 07:44:04');
INSERT INTO `t_log_user_onlineamount` VALUES ('543', '0', '2011-09-02 07:45:05');
INSERT INTO `t_log_user_onlineamount` VALUES ('544', '1', '2011-09-02 07:46:06');
INSERT INTO `t_log_user_onlineamount` VALUES ('545', '1', '2011-09-02 07:47:07');
INSERT INTO `t_log_user_onlineamount` VALUES ('546', '1', '2011-09-02 07:48:08');
INSERT INTO `t_log_user_onlineamount` VALUES ('547', '0', '2011-09-02 07:49:09');
INSERT INTO `t_log_user_onlineamount` VALUES ('548', '0', '2011-09-02 07:50:10');
INSERT INTO `t_log_user_onlineamount` VALUES ('549', '0', '2011-09-02 07:51:11');
INSERT INTO `t_log_user_onlineamount` VALUES ('550', '0', '2011-09-02 07:52:12');
INSERT INTO `t_log_user_onlineamount` VALUES ('551', '0', '2011-09-02 07:53:13');
INSERT INTO `t_log_user_onlineamount` VALUES ('552', '0', '2011-09-02 07:54:14');
INSERT INTO `t_log_user_onlineamount` VALUES ('553', '0', '2011-09-02 07:55:15');
INSERT INTO `t_log_user_onlineamount` VALUES ('554', '0', '2011-09-02 07:56:16');
INSERT INTO `t_log_user_onlineamount` VALUES ('555', '0', '2011-09-02 07:57:17');
INSERT INTO `t_log_user_onlineamount` VALUES ('556', '0', '2011-09-02 07:58:18');
INSERT INTO `t_log_user_onlineamount` VALUES ('557', '0', '2011-09-02 07:59:19');
INSERT INTO `t_log_user_onlineamount` VALUES ('558', '0', '2011-09-02 08:00:20');
INSERT INTO `t_log_user_onlineamount` VALUES ('559', '0', '2011-09-02 08:01:21');
INSERT INTO `t_log_user_onlineamount` VALUES ('560', '0', '2011-09-02 08:02:22');
INSERT INTO `t_log_user_onlineamount` VALUES ('561', '0', '2011-09-02 08:03:23');
INSERT INTO `t_log_user_onlineamount` VALUES ('562', '0', '2011-09-02 08:04:24');
INSERT INTO `t_log_user_onlineamount` VALUES ('563', '0', '2011-09-02 08:05:25');
INSERT INTO `t_log_user_onlineamount` VALUES ('564', '0', '2011-09-02 08:06:26');
INSERT INTO `t_log_user_onlineamount` VALUES ('565', '0', '2011-09-02 08:07:27');
INSERT INTO `t_log_user_onlineamount` VALUES ('566', '0', '2011-09-02 08:08:28');
INSERT INTO `t_log_user_onlineamount` VALUES ('567', '0', '2011-09-02 08:09:29');
INSERT INTO `t_log_user_onlineamount` VALUES ('568', '0', '2011-09-02 08:10:30');
INSERT INTO `t_log_user_onlineamount` VALUES ('569', '0', '2011-09-02 08:11:31');
INSERT INTO `t_log_user_onlineamount` VALUES ('570', '0', '2011-09-02 08:12:32');
INSERT INTO `t_log_user_onlineamount` VALUES ('571', '0', '2011-09-02 08:13:33');
INSERT INTO `t_log_user_onlineamount` VALUES ('572', '0', '2011-09-02 08:14:34');
INSERT INTO `t_log_user_onlineamount` VALUES ('573', '0', '2011-09-02 08:15:35');
INSERT INTO `t_log_user_onlineamount` VALUES ('574', '0', '2011-09-02 08:16:36');
INSERT INTO `t_log_user_onlineamount` VALUES ('575', '0', '2011-09-02 08:17:37');
INSERT INTO `t_log_user_onlineamount` VALUES ('576', '0', '2011-09-02 08:18:38');
INSERT INTO `t_log_user_onlineamount` VALUES ('577', '0', '2011-09-02 08:19:39');
INSERT INTO `t_log_user_onlineamount` VALUES ('578', '0', '2011-09-02 08:20:40');
INSERT INTO `t_log_user_onlineamount` VALUES ('579', '0', '2011-09-02 08:21:41');
INSERT INTO `t_log_user_onlineamount` VALUES ('580', '0', '2011-09-02 08:22:42');
INSERT INTO `t_log_user_onlineamount` VALUES ('581', '0', '2011-09-02 08:23:43');
INSERT INTO `t_log_user_onlineamount` VALUES ('582', '0', '2011-09-02 08:24:44');
INSERT INTO `t_log_user_onlineamount` VALUES ('583', '0', '2011-09-02 08:25:45');
INSERT INTO `t_log_user_onlineamount` VALUES ('584', '0', '2011-09-02 08:26:46');
INSERT INTO `t_log_user_onlineamount` VALUES ('585', '0', '2011-09-02 08:27:47');
INSERT INTO `t_log_user_onlineamount` VALUES ('586', '0', '2011-09-02 08:28:48');
INSERT INTO `t_log_user_onlineamount` VALUES ('587', '0', '2011-09-02 08:29:49');
INSERT INTO `t_log_user_onlineamount` VALUES ('588', '0', '2011-09-02 08:30:50');
INSERT INTO `t_log_user_onlineamount` VALUES ('589', '0', '2011-09-02 08:31:51');
INSERT INTO `t_log_user_onlineamount` VALUES ('590', '0', '2011-09-02 08:32:52');
INSERT INTO `t_log_user_onlineamount` VALUES ('591', '0', '2011-09-02 08:33:53');
INSERT INTO `t_log_user_onlineamount` VALUES ('592', '0', '2011-09-02 08:34:54');
INSERT INTO `t_log_user_onlineamount` VALUES ('593', '0', '2011-09-02 08:35:55');
INSERT INTO `t_log_user_onlineamount` VALUES ('594', '0', '2011-09-02 08:36:56');
INSERT INTO `t_log_user_onlineamount` VALUES ('595', '0', '2011-09-02 08:37:57');
INSERT INTO `t_log_user_onlineamount` VALUES ('596', '0', '2011-09-02 08:38:58');
INSERT INTO `t_log_user_onlineamount` VALUES ('597', '0', '2011-09-02 08:39:59');
INSERT INTO `t_log_user_onlineamount` VALUES ('598', '0', '2011-09-02 08:41:00');
INSERT INTO `t_log_user_onlineamount` VALUES ('599', '0', '2011-09-02 08:42:01');
INSERT INTO `t_log_user_onlineamount` VALUES ('600', '0', '2011-09-02 08:43:02');
INSERT INTO `t_log_user_onlineamount` VALUES ('601', '0', '2011-09-02 08:44:03');
INSERT INTO `t_log_user_onlineamount` VALUES ('602', '0', '2011-09-02 08:45:04');
INSERT INTO `t_log_user_onlineamount` VALUES ('603', '0', '2011-09-02 08:46:05');
INSERT INTO `t_log_user_onlineamount` VALUES ('604', '0', '2011-09-02 08:47:06');
INSERT INTO `t_log_user_onlineamount` VALUES ('605', '0', '2011-09-02 08:48:07');
INSERT INTO `t_log_user_onlineamount` VALUES ('606', '0', '2011-09-02 08:49:08');
INSERT INTO `t_log_user_onlineamount` VALUES ('607', '0', '2011-09-02 08:50:09');
INSERT INTO `t_log_user_onlineamount` VALUES ('608', '0', '2011-09-02 08:51:10');
INSERT INTO `t_log_user_onlineamount` VALUES ('609', '0', '2011-09-02 08:52:11');
INSERT INTO `t_log_user_onlineamount` VALUES ('610', '0', '2011-09-02 08:53:12');
INSERT INTO `t_log_user_onlineamount` VALUES ('611', '0', '2011-09-02 08:54:13');
INSERT INTO `t_log_user_onlineamount` VALUES ('612', '0', '2011-09-02 08:55:14');
INSERT INTO `t_log_user_onlineamount` VALUES ('613', '0', '2011-09-02 08:56:15');
INSERT INTO `t_log_user_onlineamount` VALUES ('614', '0', '2011-09-02 08:57:16');
INSERT INTO `t_log_user_onlineamount` VALUES ('615', '0', '2011-09-02 08:58:17');
INSERT INTO `t_log_user_onlineamount` VALUES ('616', '0', '2011-09-02 08:59:18');
INSERT INTO `t_log_user_onlineamount` VALUES ('617', '0', '2011-09-02 09:00:19');
INSERT INTO `t_log_user_onlineamount` VALUES ('618', '0', '2011-09-02 09:01:20');
INSERT INTO `t_log_user_onlineamount` VALUES ('619', '0', '2011-09-02 09:02:21');
INSERT INTO `t_log_user_onlineamount` VALUES ('620', '0', '2011-09-02 09:03:22');
INSERT INTO `t_log_user_onlineamount` VALUES ('621', '0', '2011-09-02 09:04:23');
INSERT INTO `t_log_user_onlineamount` VALUES ('622', '0', '2011-09-02 09:05:24');
INSERT INTO `t_log_user_onlineamount` VALUES ('623', '0', '2011-09-02 09:06:25');
INSERT INTO `t_log_user_onlineamount` VALUES ('624', '0', '2011-09-02 09:07:26');
INSERT INTO `t_log_user_onlineamount` VALUES ('625', '0', '2011-09-02 09:08:27');
INSERT INTO `t_log_user_onlineamount` VALUES ('626', '0', '2011-09-02 09:09:28');
INSERT INTO `t_log_user_onlineamount` VALUES ('627', '0', '2011-09-02 09:10:29');
INSERT INTO `t_log_user_onlineamount` VALUES ('628', '0', '2011-09-02 09:11:30');
INSERT INTO `t_log_user_onlineamount` VALUES ('629', '0', '2011-09-02 09:12:31');
INSERT INTO `t_log_user_onlineamount` VALUES ('630', '0', '2011-09-02 09:13:32');
INSERT INTO `t_log_user_onlineamount` VALUES ('631', '0', '2011-09-02 09:14:33');
INSERT INTO `t_log_user_onlineamount` VALUES ('632', '0', '2011-09-02 09:15:34');
INSERT INTO `t_log_user_onlineamount` VALUES ('633', '0', '2011-09-02 09:16:35');
INSERT INTO `t_log_user_onlineamount` VALUES ('634', '0', '2011-09-02 09:17:36');
INSERT INTO `t_log_user_onlineamount` VALUES ('635', '0', '2011-09-02 09:18:37');
INSERT INTO `t_log_user_onlineamount` VALUES ('636', '0', '2011-09-02 09:19:38');
INSERT INTO `t_log_user_onlineamount` VALUES ('637', '0', '2011-09-02 09:20:39');
INSERT INTO `t_log_user_onlineamount` VALUES ('638', '0', '2011-09-02 09:21:40');
INSERT INTO `t_log_user_onlineamount` VALUES ('639', '0', '2011-09-02 09:22:41');
INSERT INTO `t_log_user_onlineamount` VALUES ('640', '0', '2011-09-02 09:23:42');
INSERT INTO `t_log_user_onlineamount` VALUES ('641', '0', '2011-09-02 09:24:43');
INSERT INTO `t_log_user_onlineamount` VALUES ('642', '0', '2011-09-02 09:25:44');
INSERT INTO `t_log_user_onlineamount` VALUES ('643', '0', '2011-09-02 09:26:45');
INSERT INTO `t_log_user_onlineamount` VALUES ('644', '0', '2011-09-02 09:27:46');
INSERT INTO `t_log_user_onlineamount` VALUES ('645', '1', '2011-09-02 09:28:47');
INSERT INTO `t_log_user_onlineamount` VALUES ('646', '1', '2011-09-02 09:29:48');
INSERT INTO `t_log_user_onlineamount` VALUES ('647', '1', '2011-09-02 09:30:49');
INSERT INTO `t_log_user_onlineamount` VALUES ('648', '0', '2011-09-02 09:31:50');
INSERT INTO `t_log_user_onlineamount` VALUES ('649', '0', '2011-09-02 09:32:51');
INSERT INTO `t_log_user_onlineamount` VALUES ('650', '0', '2011-09-02 09:33:52');
INSERT INTO `t_log_user_onlineamount` VALUES ('651', '0', '2011-09-02 09:34:53');
INSERT INTO `t_log_user_onlineamount` VALUES ('652', '0', '2011-09-02 09:35:54');
INSERT INTO `t_log_user_onlineamount` VALUES ('653', '0', '2011-09-02 09:36:55');
INSERT INTO `t_log_user_onlineamount` VALUES ('654', '0', '2011-09-02 09:37:56');
INSERT INTO `t_log_user_onlineamount` VALUES ('655', '0', '2011-09-02 09:38:57');
INSERT INTO `t_log_user_onlineamount` VALUES ('656', '0', '2011-09-02 09:39:58');
INSERT INTO `t_log_user_onlineamount` VALUES ('657', '0', '2011-09-02 09:40:59');
INSERT INTO `t_log_user_onlineamount` VALUES ('658', '0', '2011-09-02 09:42:00');
INSERT INTO `t_log_user_onlineamount` VALUES ('659', '0', '2011-09-02 09:43:01');
INSERT INTO `t_log_user_onlineamount` VALUES ('660', '0', '2011-09-02 09:44:02');
INSERT INTO `t_log_user_onlineamount` VALUES ('661', '0', '2011-09-02 09:45:03');
INSERT INTO `t_log_user_onlineamount` VALUES ('662', '0', '2011-09-02 09:46:04');
INSERT INTO `t_log_user_onlineamount` VALUES ('663', '0', '2011-09-02 09:47:05');
INSERT INTO `t_log_user_onlineamount` VALUES ('664', '0', '2011-09-02 09:48:06');
INSERT INTO `t_log_user_onlineamount` VALUES ('665', '0', '2011-09-02 09:49:07');
INSERT INTO `t_log_user_onlineamount` VALUES ('666', '1', '2011-09-02 09:50:08');
INSERT INTO `t_log_user_onlineamount` VALUES ('667', '1', '2011-09-02 09:51:09');
INSERT INTO `t_log_user_onlineamount` VALUES ('668', '1', '2011-09-02 09:52:10');
INSERT INTO `t_log_user_onlineamount` VALUES ('669', '1', '2011-09-02 09:53:11');
INSERT INTO `t_log_user_onlineamount` VALUES ('670', '0', '2011-09-02 09:54:12');
INSERT INTO `t_log_user_onlineamount` VALUES ('671', '0', '2011-09-02 09:55:13');
INSERT INTO `t_log_user_onlineamount` VALUES ('672', '0', '2011-09-02 09:56:14');
INSERT INTO `t_log_user_onlineamount` VALUES ('673', '0', '2011-09-02 09:57:15');
INSERT INTO `t_log_user_onlineamount` VALUES ('674', '0', '2011-09-02 09:58:16');
INSERT INTO `t_log_user_onlineamount` VALUES ('675', '0', '2011-09-02 09:59:17');
INSERT INTO `t_log_user_onlineamount` VALUES ('676', '0', '2011-09-02 10:00:18');
INSERT INTO `t_log_user_onlineamount` VALUES ('677', '0', '2011-09-02 10:01:19');
INSERT INTO `t_log_user_onlineamount` VALUES ('678', '0', '2011-09-02 10:02:20');
INSERT INTO `t_log_user_onlineamount` VALUES ('679', '0', '2011-09-02 10:03:21');
INSERT INTO `t_log_user_onlineamount` VALUES ('680', '0', '2011-09-02 10:04:22');
INSERT INTO `t_log_user_onlineamount` VALUES ('681', '0', '2011-09-02 10:05:23');
INSERT INTO `t_log_user_onlineamount` VALUES ('682', '0', '2011-09-02 10:06:24');
INSERT INTO `t_log_user_onlineamount` VALUES ('683', '0', '2011-09-02 10:07:25');
INSERT INTO `t_log_user_onlineamount` VALUES ('684', '0', '2011-09-02 10:08:26');
INSERT INTO `t_log_user_onlineamount` VALUES ('685', '0', '2011-09-02 10:09:27');
INSERT INTO `t_log_user_onlineamount` VALUES ('686', '0', '2011-09-02 10:10:28');
INSERT INTO `t_log_user_onlineamount` VALUES ('687', '0', '2011-09-02 10:11:29');
INSERT INTO `t_log_user_onlineamount` VALUES ('688', '0', '2011-09-02 10:12:30');
INSERT INTO `t_log_user_onlineamount` VALUES ('689', '0', '2011-09-02 10:13:31');
INSERT INTO `t_log_user_onlineamount` VALUES ('690', '0', '2011-09-02 10:14:32');
INSERT INTO `t_log_user_onlineamount` VALUES ('691', '0', '2011-09-02 10:15:33');
INSERT INTO `t_log_user_onlineamount` VALUES ('692', '0', '2011-09-02 10:16:34');
INSERT INTO `t_log_user_onlineamount` VALUES ('693', '0', '2011-09-02 10:17:17');
INSERT INTO `t_log_user_onlineamount` VALUES ('694', '0', '2011-09-02 10:18:18');
INSERT INTO `t_log_user_onlineamount` VALUES ('695', '0', '2011-09-02 10:19:19');
INSERT INTO `t_log_user_onlineamount` VALUES ('696', '0', '2011-09-02 10:21:21');
INSERT INTO `t_log_user_onlineamount` VALUES ('697', '0', '2011-09-02 10:22:22');
INSERT INTO `t_log_user_onlineamount` VALUES ('698', '0', '2011-09-02 10:29:54');
INSERT INTO `t_log_user_onlineamount` VALUES ('699', '1', '2011-09-02 10:30:55');
INSERT INTO `t_log_user_onlineamount` VALUES ('700', '1', '2011-09-02 10:35:16');
INSERT INTO `t_log_user_onlineamount` VALUES ('701', '0', '2011-09-02 13:38:10');
INSERT INTO `t_log_user_onlineamount` VALUES ('702', '0', '2011-09-02 13:39:11');
INSERT INTO `t_log_user_onlineamount` VALUES ('703', '0', '2011-09-02 13:40:12');
INSERT INTO `t_log_user_onlineamount` VALUES ('704', '0', '2011-09-02 13:41:13');
INSERT INTO `t_log_user_onlineamount` VALUES ('705', '0', '2011-09-02 13:42:14');
INSERT INTO `t_log_user_onlineamount` VALUES ('706', '0', '2011-09-02 13:43:15');
INSERT INTO `t_log_user_onlineamount` VALUES ('707', '0', '2011-09-02 13:44:16');
INSERT INTO `t_log_user_onlineamount` VALUES ('708', '0', '2011-09-02 13:45:17');
INSERT INTO `t_log_user_onlineamount` VALUES ('709', '0', '2011-09-02 13:46:18');
INSERT INTO `t_log_user_onlineamount` VALUES ('710', '0', '2011-09-02 13:47:19');
INSERT INTO `t_log_user_onlineamount` VALUES ('711', '0', '2011-09-02 13:48:20');
INSERT INTO `t_log_user_onlineamount` VALUES ('712', '0', '2011-09-02 13:49:21');
INSERT INTO `t_log_user_onlineamount` VALUES ('713', '0', '2011-09-02 13:50:22');
INSERT INTO `t_log_user_onlineamount` VALUES ('714', '0', '2011-09-02 13:51:23');
INSERT INTO `t_log_user_onlineamount` VALUES ('715', '0', '2011-09-02 13:52:24');
INSERT INTO `t_log_user_onlineamount` VALUES ('716', '0', '2011-09-02 13:53:25');
INSERT INTO `t_log_user_onlineamount` VALUES ('717', '1', '2011-09-02 13:54:26');
INSERT INTO `t_log_user_onlineamount` VALUES ('718', '1', '2011-09-02 13:55:27');
INSERT INTO `t_log_user_onlineamount` VALUES ('719', '0', '2011-09-02 13:56:28');
INSERT INTO `t_log_user_onlineamount` VALUES ('720', '0', '2011-09-02 13:57:29');
INSERT INTO `t_log_user_onlineamount` VALUES ('721', '0', '2011-09-02 13:58:30');
INSERT INTO `t_log_user_onlineamount` VALUES ('722', '0', '2011-09-02 13:59:31');
INSERT INTO `t_log_user_onlineamount` VALUES ('723', '0', '2011-09-02 14:00:32');
INSERT INTO `t_log_user_onlineamount` VALUES ('724', '0', '2011-09-02 14:24:46');
INSERT INTO `t_log_user_onlineamount` VALUES ('725', '1', '2011-09-02 14:25:47');
INSERT INTO `t_log_user_onlineamount` VALUES ('726', '0', '2011-09-02 14:29:16');
INSERT INTO `t_log_user_onlineamount` VALUES ('727', '0', '2011-09-02 14:30:17');
INSERT INTO `t_log_user_onlineamount` VALUES ('728', '1', '2011-09-02 14:31:27');
INSERT INTO `t_log_user_onlineamount` VALUES ('729', '0', '2011-09-02 14:35:50');
INSERT INTO `t_log_user_onlineamount` VALUES ('730', '1', '2011-09-02 14:36:51');
INSERT INTO `t_log_user_onlineamount` VALUES ('731', '1', '2011-09-02 14:37:52');
INSERT INTO `t_log_user_onlineamount` VALUES ('732', '1', '2011-09-02 14:38:53');
INSERT INTO `t_log_user_onlineamount` VALUES ('733', '1', '2011-09-02 14:39:54');
INSERT INTO `t_log_user_onlineamount` VALUES ('734', '1', '2011-09-02 14:40:55');
INSERT INTO `t_log_user_onlineamount` VALUES ('735', '1', '2011-09-02 14:41:56');
INSERT INTO `t_log_user_onlineamount` VALUES ('736', '1', '2011-09-02 14:42:57');
INSERT INTO `t_log_user_onlineamount` VALUES ('737', '1', '2011-09-02 14:43:58');
INSERT INTO `t_log_user_onlineamount` VALUES ('738', '1', '2011-09-02 14:44:59');
INSERT INTO `t_log_user_onlineamount` VALUES ('739', '1', '2011-09-02 14:46:00');
INSERT INTO `t_log_user_onlineamount` VALUES ('740', '1', '2011-09-02 14:47:01');
INSERT INTO `t_log_user_onlineamount` VALUES ('741', '1', '2011-09-02 14:48:02');
INSERT INTO `t_log_user_onlineamount` VALUES ('742', '1', '2011-09-02 14:49:03');
INSERT INTO `t_log_user_onlineamount` VALUES ('743', '1', '2011-09-02 14:50:04');
INSERT INTO `t_log_user_onlineamount` VALUES ('744', '0', '2011-09-02 14:51:05');
INSERT INTO `t_log_user_onlineamount` VALUES ('745', '0', '2011-09-02 14:52:06');
INSERT INTO `t_log_user_onlineamount` VALUES ('746', '0', '2011-09-02 14:53:07');
INSERT INTO `t_log_user_onlineamount` VALUES ('747', '0', '2011-09-02 14:54:08');
INSERT INTO `t_log_user_onlineamount` VALUES ('748', '0', '2011-09-02 14:55:09');
INSERT INTO `t_log_user_onlineamount` VALUES ('749', '0', '2011-09-02 14:56:10');
INSERT INTO `t_log_user_onlineamount` VALUES ('750', '0', '2011-09-02 14:57:11');
INSERT INTO `t_log_user_onlineamount` VALUES ('751', '0', '2011-09-02 14:58:12');
INSERT INTO `t_log_user_onlineamount` VALUES ('752', '0', '2011-09-02 14:59:13');
INSERT INTO `t_log_user_onlineamount` VALUES ('753', '0', '2011-09-02 15:00:14');
INSERT INTO `t_log_user_onlineamount` VALUES ('754', '0', '2011-09-02 15:01:15');
INSERT INTO `t_log_user_onlineamount` VALUES ('755', '0', '2011-09-02 15:02:16');
INSERT INTO `t_log_user_onlineamount` VALUES ('756', '0', '2011-09-02 15:03:17');
INSERT INTO `t_log_user_onlineamount` VALUES ('757', '0', '2011-09-02 15:04:18');
INSERT INTO `t_log_user_onlineamount` VALUES ('758', '0', '2011-09-02 15:05:19');
INSERT INTO `t_log_user_onlineamount` VALUES ('759', '0', '2011-09-02 15:06:20');
INSERT INTO `t_log_user_onlineamount` VALUES ('760', '0', '2011-09-02 15:07:21');
INSERT INTO `t_log_user_onlineamount` VALUES ('761', '0', '2011-09-02 15:08:22');
INSERT INTO `t_log_user_onlineamount` VALUES ('762', '0', '2011-09-02 15:09:23');
INSERT INTO `t_log_user_onlineamount` VALUES ('763', '0', '2011-09-02 15:10:24');
INSERT INTO `t_log_user_onlineamount` VALUES ('764', '0', '2011-09-02 15:11:25');
INSERT INTO `t_log_user_onlineamount` VALUES ('765', '0', '2011-09-02 15:12:26');
INSERT INTO `t_log_user_onlineamount` VALUES ('766', '0', '2011-09-02 15:13:27');
INSERT INTO `t_log_user_onlineamount` VALUES ('767', '0', '2011-09-02 15:14:28');
INSERT INTO `t_log_user_onlineamount` VALUES ('768', '0', '2011-09-02 15:15:29');
INSERT INTO `t_log_user_onlineamount` VALUES ('769', '0', '2011-09-02 15:16:30');
INSERT INTO `t_log_user_onlineamount` VALUES ('770', '0', '2011-09-02 15:17:31');
INSERT INTO `t_log_user_onlineamount` VALUES ('771', '0', '2011-09-02 16:03:49');
INSERT INTO `t_log_user_onlineamount` VALUES ('772', '0', '2011-09-02 16:04:55');
INSERT INTO `t_log_user_onlineamount` VALUES ('773', '0', '2011-09-02 16:05:56');
INSERT INTO `t_log_user_onlineamount` VALUES ('774', '1', '2011-09-02 16:06:57');
INSERT INTO `t_log_user_onlineamount` VALUES ('775', '1', '2011-09-02 16:07:58');
INSERT INTO `t_log_user_onlineamount` VALUES ('776', '1', '2011-09-02 16:08:59');
INSERT INTO `t_log_user_onlineamount` VALUES ('777', '1', '2011-09-02 16:10:00');
INSERT INTO `t_log_user_onlineamount` VALUES ('778', '1', '2011-09-02 16:11:01');
INSERT INTO `t_log_user_onlineamount` VALUES ('779', '1', '2011-09-02 16:12:02');
INSERT INTO `t_log_user_onlineamount` VALUES ('780', '0', '2011-09-02 16:13:03');
INSERT INTO `t_log_user_onlineamount` VALUES ('781', '0', '2011-09-02 16:14:04');
INSERT INTO `t_log_user_onlineamount` VALUES ('782', '1', '2011-09-02 16:15:05');
INSERT INTO `t_log_user_onlineamount` VALUES ('783', '1', '2011-09-02 16:16:06');
INSERT INTO `t_log_user_onlineamount` VALUES ('784', '1', '2011-09-02 16:17:07');
INSERT INTO `t_log_user_onlineamount` VALUES ('785', '1', '2011-09-02 16:18:08');
INSERT INTO `t_log_user_onlineamount` VALUES ('786', '1', '2011-09-02 16:19:09');
INSERT INTO `t_log_user_onlineamount` VALUES ('787', '1', '2011-09-02 16:20:10');
INSERT INTO `t_log_user_onlineamount` VALUES ('788', '1', '2011-09-02 16:21:11');
INSERT INTO `t_log_user_onlineamount` VALUES ('789', '1', '2011-09-02 16:22:12');
INSERT INTO `t_log_user_onlineamount` VALUES ('790', '0', '2011-09-02 16:23:13');
INSERT INTO `t_log_user_onlineamount` VALUES ('791', '0', '2011-09-02 16:24:33');
INSERT INTO `t_log_user_onlineamount` VALUES ('792', '1', '2011-09-02 16:25:34');
INSERT INTO `t_log_user_onlineamount` VALUES ('793', '1', '2011-09-02 16:26:35');
INSERT INTO `t_log_user_onlineamount` VALUES ('794', '1', '2011-09-02 16:27:36');
INSERT INTO `t_log_user_onlineamount` VALUES ('795', '1', '2011-09-02 16:28:37');
INSERT INTO `t_log_user_onlineamount` VALUES ('796', '1', '2011-09-02 16:29:38');
INSERT INTO `t_log_user_onlineamount` VALUES ('797', '1', '2011-09-02 16:30:39');
INSERT INTO `t_log_user_onlineamount` VALUES ('798', '1', '2011-09-02 16:31:40');
INSERT INTO `t_log_user_onlineamount` VALUES ('799', '1', '2011-09-02 16:32:41');
INSERT INTO `t_log_user_onlineamount` VALUES ('800', '0', '2011-09-02 16:33:42');
INSERT INTO `t_log_user_onlineamount` VALUES ('801', '1', '2011-09-02 16:34:43');
INSERT INTO `t_log_user_onlineamount` VALUES ('802', '1', '2011-09-02 16:35:44');
INSERT INTO `t_log_user_onlineamount` VALUES ('803', '1', '2011-09-02 16:36:45');
INSERT INTO `t_log_user_onlineamount` VALUES ('804', '1', '2011-09-02 16:37:46');
INSERT INTO `t_log_user_onlineamount` VALUES ('805', '1', '2011-09-02 16:38:47');
INSERT INTO `t_log_user_onlineamount` VALUES ('806', '1', '2011-09-02 16:39:48');
INSERT INTO `t_log_user_onlineamount` VALUES ('807', '1', '2011-09-02 16:40:49');
INSERT INTO `t_log_user_onlineamount` VALUES ('808', '1', '2011-09-02 16:41:50');
INSERT INTO `t_log_user_onlineamount` VALUES ('809', '1', '2011-09-02 16:42:51');
INSERT INTO `t_log_user_onlineamount` VALUES ('810', '1', '2011-09-02 16:43:52');
INSERT INTO `t_log_user_onlineamount` VALUES ('811', '1', '2011-09-02 16:44:53');
INSERT INTO `t_log_user_onlineamount` VALUES ('812', '1', '2011-09-02 16:45:54');
INSERT INTO `t_log_user_onlineamount` VALUES ('813', '0', '2011-09-02 16:46:55');
