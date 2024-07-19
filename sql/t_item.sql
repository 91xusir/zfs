/*
Navicat MySQL Data Transfer

Source Server         : 127.0.0.1
Source Server Version : 50156
Source Host           : localhost:3306
Source Database       : game

Target Server Type    : MYSQL
Target Server Version : 50156
File Encoding         : 65001

Date: 2011-09-01 14:49:16
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `t_item`
-- ----------------------------
DROP TABLE IF EXISTS `t_item`;
CREATE TABLE `t_item` (
  `index` bigint(20) NOT NULL AUTO_INCREMENT COMMENT '物品唯一ID',
  `item_id` int(11) NOT NULL,
  `creature_id` int(11) NOT NULL DEFAULT '0' COMMENT '角色ID',
  `bag_type` tinyint(4) NOT NULL COMMENT '包裹类型',
  `bag_page` tinyint(4) NOT NULL DEFAULT '0' COMMENT '物品所在包裹页码',
  `bag_line` tinyint(4) NOT NULL DEFAULT '0' COMMENT '物品所在包裹行',
  `bag_column` tinyint(4) NOT NULL DEFAULT '0' COMMENT '物品所在包裹列',
  `item_type` int(11) NOT NULL COMMENT '物品种类',
  `count` int(11) NOT NULL DEFAULT '0' COMMENT '药瓶的剩余数量',
  `watt` int(11) NOT NULL DEFAULT '0',
  `attrib_id1` int(11) NOT NULL DEFAULT '0' COMMENT '有色装备额外增加的属性ID1',
  `attrib_rate1` smallint(6) NOT NULL DEFAULT '0' COMMENT '有色装备属性ID的加成比率1',
  `attrib_id2` int(11) NOT NULL DEFAULT '0' COMMENT '有色装备额外增加的属性ID2',
  `attrib_rate2` smallint(6) NOT NULL DEFAULT '0' COMMENT '有色装备属性ID的加成比率2',
  `pet_name` varchar(32) NOT NULL DEFAULT '',
  `param_count` tinyint(4) NOT NULL DEFAULT '0' COMMENT '使用到的参数个数',
  `params` longtext COMMENT '使用到的参数',
  `enchant_attrib` smallint(6) NOT NULL DEFAULT '0' COMMENT '附魔的属性ID',
  `time_limit` int(11) NOT NULL DEFAULT '0' COMMENT '过期时间',
  `cur_wear` int(11) NOT NULL DEFAULT '-1' COMMENT '当前耐久',
  PRIMARY KEY (`index`),
  UNIQUE KEY `index` (`index`),
  KEY `creature_id` (`creature_id`),
  KEY `item_type` (`item_type`),
  KEY `item_id` (`item_id`)
) ENGINE=MyISAM AUTO_INCREMENT=64 DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of t_item
-- ----------------------------
