CREATE TABLE `top_main` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `title` text,
  `description` varchar(255) default NULL,
  `url` varchar(255) default NULL,
  `cat` smallint(6) default NULL,
  `gin` int(11) default NULL,
  `gout` int(11) default NULL,
  `email` varchar(155) default NULL,
  `type` tinyint(1) unsigned default NULL,
  `broken` tinyint(1) unsigned default '0',
  `insert_date` datetime default NULL,
  `resfield1` text,
  `resfield2` text,
  `resfield3` text,
  `mail_sended` tinyint(4) NOT NULL default '0',
  `hits` int(10) NOT NULL default '0',
  `hits_y` int(10) NOT NULL default '0',
  `hosts` int(10) NOT NULL default '0',
  `hosts_y` int(10) NOT NULL default '0',
  `users` int(10) NOT NULL default '0',
  `users_y` int(10) NOT NULL default '0',
  `hits_all` int(32) NOT NULL default '0',
  `hosts_all` int(32) NOT NULL default '0',
  `users_all` int(32) NOT NULL default '0',
  `num_count` char(2) NOT NULL default '0',
  `color_hits_all` varchar(6) NOT NULL default '0',
  `color_hits` varchar(6) NOT NULL default '0',
  `color_users` varchar(6) NOT NULL default '0',
  `passw` varchar(32) NOT NULL default '',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=cp1251 AUTO_INCREMENT=2 ;

-- 
-- Дамп данных таблицы `top_main`
-- 

INSERT INTO `top_main` (`id`, `title`, `description`, `url`, `cat`, `gin`, `gout`, `email`, `type`, `broken`, `insert_date`, `resfield1`, `resfield2`, `resfield3`, `mail_sended`, `hits`, `hits_y`, `hosts`, `hosts_y`, `users`, `users_y`, `hits_all`, `hosts_all`, `users_all`, `num_count`, `color_hits_all`, `color_hits`, `color_users`, `passw`) VALUES (1, 'Сайт первый', 'Описание сайта.', 'www.site.ru', 61, 0, 2194, 'info@site.ru', 1, 0, '2008-04-26 17:22:39', '', '', '', 0, 1179, 210, 56, 103, 55, 82, 308832, 72719, 59148, '01', '000000', '000000', '000000', '');

-- 
-- Структура таблицы `top_users_today`
-- 

CREATE TABLE `top_users_today` (
  `id` int(10) NOT NULL default '0',
  `us_id` int(10) NOT NULL default '0',
  KEY `id` (`id`,`us_id`)
) ENGINE=MyISAM DEFAULT CHARSET=cp1251;

