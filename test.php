<?php
    include_once("../common.php");
    include_once("../const.php");
    //This File is Generate Auto By UnitTest.cpp.

    //TableName: xk_shows
    Class xk_shows{
        public $show_id;
        public $show_title;
        public $show_describe;
        public $show_author;
        public $show_content;
        public $show_img;
        public $show_position;
        public $show_total_type;
        public $show_type;
        public $update_time;
        public $show_for_vip;
        public $xk_videolink;

        public function __construct($show_id,$show_title,$show_describe,$show_author,$show_content,$show_img,$show_position,$show_total_type,$show_type,$update_time,$show_for_vip,$xk_videolink){
            $this->show_id = $show_id;
            $this->show_title = $show_title;
            $this->show_describe = $show_describe;
            $this->show_author = $show_author;
            $this->show_content = $show_content;
            $this->show_img = $show_img;
            $this->show_position = $show_position;
            $this->show_total_type = $show_total_type;
            $this->show_type = $show_type;
            $this->update_time = $update_time;
            $this->show_for_vip = $show_for_vip;
            $this->xk_videolink = $xk_videolink;
        }
       public function Check(){
            return new ReturnObject(0,"");
        }
       public function Add(){
            $ret = $this->Check();
            if($ret->resultcode!=0) return $ret;
            MysqlHelper::$UseUTF8 =true;

            $insertSql = "INSERT INTO xk_shows(
                show_id,
                show_title,
                show_describe,
                show_author,
                show_content,
                show_img,
                show_position,
                show_total_type,
                show_type,
                update_time,
                show_for_vip,
                xk_videolink)VALUES(?,?,?,?,?,?,?,?,?,?,?,?)";

            return MysqlHelper::SafeQuery($insertSql,"isssssssssss",
                $this->show_id,
                $this->show_title,
                $this->show_describe,
                $this->show_author,
                $this->show_content,
                $this->show_img,
                $this->show_position,
                $this->show_total_type,
                $this->show_type,
                $this->update_time,
                $this->show_for_vip,
                $this->xk_videolink);
        }

        public static function GetTotalCount(){
            return MysqlHelper::SafeQueryResult("SELECT count(*) AS 'count' FROM xk_shows WHERE 1 = ?",'i',1);
        }

        public static function GetList($page,$num=10000){
            $start = $num*($page-1);
            MysqlHelper::$UseUTF8 = true;
            return MysqlHelper::SafeQueryResult("SELECT * FROM xk_shows LIMIT ?,?","dd",$start,$num);
        }

        //FIND FUNCTIONS START----------------------------------------------------
        public static function FindBySHOW_ID($value,$page=1,$num=10000){
            $start = $num*($page-1);
            MysqlHelper::$UseUTF8 = true;
            $count = MysqlHepler::SafeQueryResult("SELECT count(*) AS 'count' FROM xk_shows WHERE show_id = ?",$value);
            $ret   = MysqlHelper::SafeQueryResult("SELECT * FROM xk_shows WHERE show_id = ? LIMIT ?,? ",'idd',$value,$start,$num);
            array_push($ret,$count);
            return $ret;
        }
        public static function FindBySHOW_TITLE($value,$page=1,$num=10000){
            $start = $num*($page-1);
            MysqlHelper::$UseUTF8 = true;
            $count = MysqlHepler::SafeQueryResult("SELECT count(*) AS 'count' FROM xk_shows WHERE show_title = ?",$value);
            $ret   = MysqlHelper::SafeQueryResult("SELECT * FROM xk_shows WHERE show_title = ? LIMIT ?,? ",'sdd',$value,$start,$num);
            array_push($ret,$count);
            return $ret;
        }
        public static function FindBySHOW_DESCRIBE($value,$page=1,$num=10000){
            $start = $num*($page-1);
            MysqlHelper::$UseUTF8 = true;
            $count = MysqlHepler::SafeQueryResult("SELECT count(*) AS 'count' FROM xk_shows WHERE show_describe = ?",$value);
            $ret   = MysqlHelper::SafeQueryResult("SELECT * FROM xk_shows WHERE show_describe = ? LIMIT ?,? ",'sdd',$value,$start,$num);
            array_push($ret,$count);
            return $ret;
        }
        public static function FindBySHOW_AUTHOR($value,$page=1,$num=10000){
            $start = $num*($page-1);
            MysqlHelper::$UseUTF8 = true;
            $count = MysqlHepler::SafeQueryResult("SELECT count(*) AS 'count' FROM xk_shows WHERE show_author = ?",$value);
            $ret   = MysqlHelper::SafeQueryResult("SELECT * FROM xk_shows WHERE show_author = ? LIMIT ?,? ",'sdd',$value,$start,$num);
            array_push($ret,$count);
            return $ret;
        }
        public static function FindBySHOW_CONTENT($value,$page=1,$num=10000){
            $start = $num*($page-1);
            MysqlHelper::$UseUTF8 = true;
            $count = MysqlHepler::SafeQueryResult("SELECT count(*) AS 'count' FROM xk_shows WHERE show_content = ?",$value);
            $ret   = MysqlHelper::SafeQueryResult("SELECT * FROM xk_shows WHERE show_content = ? LIMIT ?,? ",'sdd',$value,$start,$num);
            array_push($ret,$count);
            return $ret;
        }
        public static function FindBySHOW_IMG($value,$page=1,$num=10000){
            $start = $num*($page-1);
            MysqlHelper::$UseUTF8 = true;
            $count = MysqlHepler::SafeQueryResult("SELECT count(*) AS 'count' FROM xk_shows WHERE show_img = ?",$value);
            $ret   = MysqlHelper::SafeQueryResult("SELECT * FROM xk_shows WHERE show_img = ? LIMIT ?,? ",'sdd',$value,$start,$num);
            array_push($ret,$count);
            return $ret;
        }
        public static function FindBySHOW_POSITION($value,$page=1,$num=10000){
            $start = $num*($page-1);
            MysqlHelper::$UseUTF8 = true;
            $count = MysqlHepler::SafeQueryResult("SELECT count(*) AS 'count' FROM xk_shows WHERE show_position = ?",$value);
            $ret   = MysqlHelper::SafeQueryResult("SELECT * FROM xk_shows WHERE show_position = ? LIMIT ?,? ",'sdd',$value,$start,$num);
            array_push($ret,$count);
            return $ret;
        }
        public static function FindBySHOW_TOTAL_TYPE($value,$page=1,$num=10000){
            $start = $num*($page-1);
            MysqlHelper::$UseUTF8 = true;
            $count = MysqlHepler::SafeQueryResult("SELECT count(*) AS 'count' FROM xk_shows WHERE show_total_type = ?",$value);
            $ret   = MysqlHelper::SafeQueryResult("SELECT * FROM xk_shows WHERE show_total_type = ? LIMIT ?,? ",'sdd',$value,$start,$num);
            array_push($ret,$count);
            return $ret;
        }
        public static function FindBySHOW_TYPE($value,$page=1,$num=10000){
            $start = $num*($page-1);
            MysqlHelper::$UseUTF8 = true;
            $count = MysqlHepler::SafeQueryResult("SELECT count(*) AS 'count' FROM xk_shows WHERE show_type = ?",$value);
            $ret   = MysqlHelper::SafeQueryResult("SELECT * FROM xk_shows WHERE show_type = ? LIMIT ?,? ",'sdd',$value,$start,$num);
            array_push($ret,$count);
            return $ret;
        }
        public static function FindByUPDATE_TIME($value,$page=1,$num=10000){
            $start = $num*($page-1);
            MysqlHelper::$UseUTF8 = true;
            $count = MysqlHepler::SafeQueryResult("SELECT count(*) AS 'count' FROM xk_shows WHERE update_time = ?",$value);
            $ret   = MysqlHelper::SafeQueryResult("SELECT * FROM xk_shows WHERE update_time = ? LIMIT ?,? ",'sdd',$value,$start,$num);
            array_push($ret,$count);
            return $ret;
        }
        public static function FindBySHOW_FOR_VIP($value,$page=1,$num=10000){
            $start = $num*($page-1);
            MysqlHelper::$UseUTF8 = true;
            $count = MysqlHepler::SafeQueryResult("SELECT count(*) AS 'count' FROM xk_shows WHERE show_for_vip = ?",$value);
            $ret   = MysqlHelper::SafeQueryResult("SELECT * FROM xk_shows WHERE show_for_vip = ? LIMIT ?,? ",'sdd',$value,$start,$num);
            array_push($ret,$count);
            return $ret;
        }
        public static function FindByXK_VIDEOLINK($value,$page=1,$num=10000){
            $start = $num*($page-1);
            MysqlHelper::$UseUTF8 = true;
            $count = MysqlHepler::SafeQueryResult("SELECT count(*) AS 'count' FROM xk_shows WHERE xk_videolink = ?",$value);
            $ret   = MysqlHelper::SafeQueryResult("SELECT * FROM xk_shows WHERE xk_videolink = ? LIMIT ?,? ",'sdd',$value,$start,$num);
            array_push($ret,$count);
            return $ret;
        }
        //FIND FUNCTIONS END----------------------------------------------------
    }
