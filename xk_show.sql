-- DROP TABLE IF EXISTS xk_shows;
DROP TABLE IF EXISTS xk_user_like;
DROP TABLE IF EXISTS xk_user;

CREATE TABLE xk_shows (
  show_id int(100) unsigned NOT NULL AUTO_INCREMENT,
  show_title varchar(50) NOT NULL COMMENT '标题 %NOREPEAT%',
  show_describe varchar(100) NOT NULL COMMENT '描述',
  show_author varchar(50) COMMENT '作者',
  show_content longtext NOT NULL COMMENT '内容',
  show_img varchar(1024) NOT NULL COMMENT '图片',
  show_position varchar(200) COMMENT '文件存放位置 %NOREPEAT%',
  show_total_type varchar(200) COMMENT '大方向类型',
  show_type varchar(50) NOT NULL COMMENT '文件类型',
  update_time varchar(50)  NOT NULL COMMENT '最后更新时间',
  show_for_vip varchar(10) NOT NULL COMMENT '只对VIP可见',
  xk_videoLink varchar(200) COMMENT '视频地址',
  PRIMARY KEY(show_id)
)ENGINE=InnoDB AUTO_INCREMENT=30 DEFAULT CHARSET=utf8;

CREATE TABLE xk_user (
  user_id int(100) unsigned NOT NULL AUTO_INCREMENT, 
  user_account varchar(50) NOT NULL COMMENT '用户账户 %NOREPEAT%',
  user_password varchar(50) NOT NULL COMMENT '用户密码',
  user_email_addr varchar(100) NOT NULL COMMENT '邮箱地址 %NOREPEAT%',
  user_phone_number  int(11) NOT NULL COMMENT '手机号 %NOREPEAT%',
  PRIMARY KEY(user_id)
) ENGINE=InnoDB AUTO_INCREMENT=22 DEFAULT CHARSET=utf8;

CREATE TABLE xk_user_like(
  user_id int(100) unsigned NOT NULL ,
  like_id int(100) unsigned NOT NULL AUTO_INCREMENT,
  target_id int(100) unsigned COMMENT '目标id',
  PRIMARY KEY(like_id),
  FOREIGN KEY(user_id) REFERENCES xk_user(user_id)
)ENGINE=InnoDB AUTO_INCREMENT=22 DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS xk_welcome_user;
CREATE TABLE xk_welcome_user(
  user_id int(100) unsigned NOT NULL AUTO_INCREMENT, 
  user_welcome_user_name varchar(100) NOT NULL COMMENT '来宾姓名',
  user_company_name varchar(200) NOT NULL COMMENT '公司名 %NOREPEAT%',
  user_phone_number varchar(12) NOT NULL COMMENT '手机号 %NOREPEAT%',
  user_email_addr varchar(100) NOT NULL COMMENT '邮箱 %NOREPEAT%',
  user_partner_nums varchar(100)  NOT NULL COMMENT '同行人数',
  user_leave_message longtext NOT NULL COMMENT '留言',
  PRIMARY KEY(user_id)
)ENGINE=InnoDB AUTO_INCREMENT=22 DEFAULT CHARSET=utf8;
