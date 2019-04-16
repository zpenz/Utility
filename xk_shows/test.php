<?php
    include_once("../common.php");
    include_once("../const.php");
    //This File is Generate Auto By Test.

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
            if(!empty($this->show_title)){
                $ret=MysqlHelper::S_IsEmptySet("SELECT * FROM xk_shows WHERE show_title = ?",'s',$this->show_title);
                if($ret->resultcode<0) return $ret;
                if($ret->data !=0) return new ReturnObject(-233,"show_title can not repeat");
            }

            if(!empty($this->show_position)){
                $ret=MysqlHelper::S_IsEmptySet("SELECT * FROM xk_shows WHERE show_position = ?",'s',$this->show_position);
                if($ret->resultcode<0) return $ret;
                if($ret->data !=0) return new ReturnObject(-233,"show_position can not repeat");
            }

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
        public static function FindByShowId($value,$page=1,$num=10000){
            $start = $num*($page-1);
            MysqlHelper::$UseUTF8 = true;
            $count = MysqlHepler::SafeQueryResult("SELECT count(*) AS 'count' FROM xk_shows WHERE show_id = ?",$value);
            $ret   = MysqlHelper::SafeQueryResult("SELECT * FROM xk_shows WHERE show_id = ? LIMIT ?,? ",'idd',$value,$start,$num);
            array_push($ret,$count);
            return $ret;
        }
        public static function FindByShowTitle($value,$page=1,$num=10000){
            $start = $num*($page-1);
            MysqlHelper::$UseUTF8 = true;
            $count = MysqlHepler::SafeQueryResult("SELECT count(*) AS 'count' FROM xk_shows WHERE show_title = ?",$value);
            $ret   = MysqlHelper::SafeQueryResult("SELECT * FROM xk_shows WHERE show_title = ? LIMIT ?,? ",'sdd',$value,$start,$num);
            array_push($ret,$count);
            return $ret;
        }
        public static function FindByShowDescribe($value,$page=1,$num=10000){
            $start = $num*($page-1);
            MysqlHelper::$UseUTF8 = true;
            $count = MysqlHepler::SafeQueryResult("SELECT count(*) AS 'count' FROM xk_shows WHERE show_describe = ?",$value);
            $ret   = MysqlHelper::SafeQueryResult("SELECT * FROM xk_shows WHERE show_describe = ? LIMIT ?,? ",'sdd',$value,$start,$num);
            array_push($ret,$count);
            return $ret;
        }
        public static function FindByShowAuthor($value,$page=1,$num=10000){
            $start = $num*($page-1);
            MysqlHelper::$UseUTF8 = true;
            $count = MysqlHepler::SafeQueryResult("SELECT count(*) AS 'count' FROM xk_shows WHERE show_author = ?",$value);
            $ret   = MysqlHelper::SafeQueryResult("SELECT * FROM xk_shows WHERE show_author = ? LIMIT ?,? ",'sdd',$value,$start,$num);
            array_push($ret,$count);
            return $ret;
        }
        public static function FindByShowContent($value,$page=1,$num=10000){
            $start = $num*($page-1);
            MysqlHelper::$UseUTF8 = true;
            $count = MysqlHepler::SafeQueryResult("SELECT count(*) AS 'count' FROM xk_shows WHERE show_content = ?",$value);
            $ret   = MysqlHelper::SafeQueryResult("SELECT * FROM xk_shows WHERE show_content = ? LIMIT ?,? ",'sdd',$value,$start,$num);
            array_push($ret,$count);
            return $ret;
        }
        public static function FindByShowImg($value,$page=1,$num=10000){
            $start = $num*($page-1);
            MysqlHelper::$UseUTF8 = true;
            $count = MysqlHepler::SafeQueryResult("SELECT count(*) AS 'count' FROM xk_shows WHERE show_img = ?",$value);
            $ret   = MysqlHelper::SafeQueryResult("SELECT * FROM xk_shows WHERE show_img = ? LIMIT ?,? ",'sdd',$value,$start,$num);
            array_push($ret,$count);
            return $ret;
        }
        public static function FindByShowPosition($value,$page=1,$num=10000){
            $start = $num*($page-1);
            MysqlHelper::$UseUTF8 = true;
            $count = MysqlHepler::SafeQueryResult("SELECT count(*) AS 'count' FROM xk_shows WHERE show_position = ?",$value);
            $ret   = MysqlHelper::SafeQueryResult("SELECT * FROM xk_shows WHERE show_position = ? LIMIT ?,? ",'sdd',$value,$start,$num);
            array_push($ret,$count);
            return $ret;
        }
        public static function FindByShowTotalType($value,$page=1,$num=10000){
            $start = $num*($page-1);
            MysqlHelper::$UseUTF8 = true;
            $count = MysqlHepler::SafeQueryResult("SELECT count(*) AS 'count' FROM xk_shows WHERE show_total_type = ?",$value);
            $ret   = MysqlHelper::SafeQueryResult("SELECT * FROM xk_shows WHERE show_total_type = ? LIMIT ?,? ",'sdd',$value,$start,$num);
            array_push($ret,$count);
            return $ret;
        }
        public static function FindByShowType($value,$page=1,$num=10000){
            $start = $num*($page-1);
            MysqlHelper::$UseUTF8 = true;
            $count = MysqlHepler::SafeQueryResult("SELECT count(*) AS 'count' FROM xk_shows WHERE show_type = ?",$value);
            $ret   = MysqlHelper::SafeQueryResult("SELECT * FROM xk_shows WHERE show_type = ? LIMIT ?,? ",'sdd',$value,$start,$num);
            array_push($ret,$count);
            return $ret;
        }
        public static function FindByUpdateTime($value,$page=1,$num=10000){
            $start = $num*($page-1);
            MysqlHelper::$UseUTF8 = true;
            $count = MysqlHepler::SafeQueryResult("SELECT count(*) AS 'count' FROM xk_shows WHERE update_time = ?",$value);
            $ret   = MysqlHelper::SafeQueryResult("SELECT * FROM xk_shows WHERE update_time = ? LIMIT ?,? ",'sdd',$value,$start,$num);
            array_push($ret,$count);
            return $ret;
        }
        public static function FindByShowForVip($value,$page=1,$num=10000){
            $start = $num*($page-1);
            MysqlHelper::$UseUTF8 = true;
            $count = MysqlHepler::SafeQueryResult("SELECT count(*) AS 'count' FROM xk_shows WHERE show_for_vip = ?",$value);
            $ret   = MysqlHelper::SafeQueryResult("SELECT * FROM xk_shows WHERE show_for_vip = ? LIMIT ?,? ",'sdd',$value,$start,$num);
            array_push($ret,$count);
            return $ret;
        }
        public static function FindByXkVideolink($value,$page=1,$num=10000){
            $start = $num*($page-1);
            MysqlHelper::$UseUTF8 = true;
            $count = MysqlHepler::SafeQueryResult("SELECT count(*) AS 'count' FROM xk_shows WHERE xk_videolink = ?",$value);
            $ret   = MysqlHelper::SafeQueryResult("SELECT * FROM xk_shows WHERE xk_videolink = ? LIMIT ?,? ",'sdd',$value,$start,$num);
            array_push($ret,$count);
            return $ret;
        }
        //FIND FUNCTIONS END----------------------------------------------------

        public static function Delete($unique){
            $ret = MysqlHelper::S_IsEmptySet("SELECT * FROM xk_shows WHERE show_id = ?",'i',$unique);
            if($ret->resultcode<0) return new ReturnObject(-2,"delete a no exist record");
            //TODO: if have file link . delete link before delete record!
            $ret = MysqlHelper::SafeQueryResult("SELECT * FROM xk_shows WHERE show_id= ? ",'i',$unique);
            if($ret->resultcode<0) return $ret;
            
            return MysqlHelper::SafeQuery("DELETE FROM xk_shows WHERE show_id = ?",'i',$unique);
        }

    }
