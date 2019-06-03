<?php
define("REMOTE_SERVER",FALSE);

define("host", "");
define("username", "");
define("password", "");
define("db", "");

define("_DEBUG","0");
define("SAVE_ABSOULT_PATH",FALSE);

define("FILE_UPLOAD_PATH","upload/");
define("FILE_DELETE_PATH","");

define("MAX_LOG_FILE_SIZE",200);
define("DEFAULT_MAX_UPLOAD_FILE_SIZE",100);
define("LOG_FILE_NAME","xk_web_log.log");
define("UNLEGAL_VALUE",'XK_UNLEGAL');

define('BASIC_PATH',str_replace('\\','/',dirname(dirname(__FILE__))).'/');

define("MAX_CURL_TIME",10);

date_default_timezone_set("Asia/Hong_Kong");
set_time_limit(0);  //FOR_LONG_TIME_OPERATOR
ini_set('pcre.backtrack_limit',-1); //RegexMaxLimit

class xkSession
{
    public static function safeStart(){
        $status=session_status();
        if($status!=PHP_SESSION_ACTIVE)
            session_start();
        unset($status);
    }

    public static function isLogin($username='none')
    {
        $status=session_status();
        if($status!=PHP_SESSION_ACTIVE)
            session_start();
        if(isset($_SESSION['LOGIN_USER'])) return new ReturnObject(0,$_SESSION['LOGIN_USER']);
        return new ReturnObject(-39,"user not login");
    }

    public static function addSpecifySession($sessionname,$value){
        session_start();
        $_SESSION[$sessionname] = $value;
    }

    public static function getsession($sessionname,$defaultvalue=''){
        if(!isset($_SESSION[$sessionname])) return $defaultvalue;
        return $_SESSION[$sessionname];
    }

    public static function addSession($username)
    {
        session_start();
        // $username="xk_".strval($username);
        // $_SESSION[$username] = "hasLogin";
        $_SESSION['LOGIN_USER']=$username;
    }

    public static function closeSession()
    {
        $status=session_status();
        if($status!=PHP_SESSION_ACTIVE)
            session_start();
        foreach ($_SESSION as $sessionItem) {
            unset($sessionItem);
        }
        session_destroy();
    }
}

class XkPush
{
    public static function baiduPush($postSiteArray, $targetSite = 'www.seekswan.com', $token = 'W6BAmYYx70Cpv7UW')
    {
        $url = $postSiteArray;
        $api = "http://data.zz.baidu.com/urls?site={$targetSite}&token={$token}";
        $ch = curl_init();
        $options = array(
            CURLOPT_URL => $api,
            CURLOPT_POST => true,
            CURLOPT_RETURNTRANSFER => true,
            CURLOPT_POSTFIELDS => implode("\n", $url),
            CURLOPT_HTTPHEADER => array('Content-Type:text/plain'),
        );
        curl_setopt_array($ch, $options);
        $result = curl_exec($ch);
        return new ReturnObject(0, $result . $url);
    }
}

class ReturnObject
{
    public $data;      //error: errordesc / success: data
    public $resultcode; //return result

    public function __construct($resultcode, $data, array $arguments = array())
    {
        $this->resultcode = $resultcode;
        // //only one element array conver object
        // if(is_array($data) && count($data)==1){$this->data=data[0];}
        // else
        $this->data = $data;

        if ($resultcode < 0) {
            Log::LogInfo("Error: {$data}");
        }
    }

    public function value($name){
        return ($this->data[0])[$name];
    }

    public function first(){
        return $this->data[0];
    }

    public function second(){
        if(count($this->data)>1)
            return $this->data[1];
        else return new ReturnObject(-99,"invaild index");
    }
}

//do not save mysqli object
class MysqlHelper{
    public static $UseUTF8 = false;
    public static function SafeQuery($sql, $paramsType, ...$params){
        //static $mysqli=null;
        log::LogInfo($sql);
        //get mysqli object
//        if($mysqli==null)
//        if(($mysqli=new mysqli(host,username,password,db))->connect_errno) return new ReturnObject(-1000,"connect database failed!");
        $mysqli = new mysqli(host,username,password,db);
        if($mysqli->connect_errno) return new ReturnObject(-1000,"connect database failed!");
        //set charset
        if(MysqlHelper::$UseUTF8) $mysqli->set_charset("utf8");
        //get stmt
        if(!($stmt = $mysqli->stmt_init())) return new ReturnObject(-2004, "stmt init error");
        if (!$stmt->prepare($sql)) return new ReturnObject(-2004, "stmt prepare error");
        $stmt->bind_param($paramsType, ...$params);
        $stmt->execute();

        //get result
        foreach($params as $item) Log::LogInfo($item);
        $affect_row = $stmt->affected_rows;
        $error = $stmt->error;
        if ($affect_row < 0) {
            $stmt->close();
            return new ReturnObject(-2004, "bind_param error! {$error}");
        }

        //close stmt and mysqli
        $stmt->close();
        //$mysqli->close();
        return new ReturnObject(0, $affect_row);
    }

    public static function SafeQueryResult($sql, $paramsType, ...$params){
        log::LogInfo($sql);
        $mysqli = new mysqli(host,username,password,db);
        if($mysqli->connect_errno) return new ReturnObject(-1000,"connect database failed!");
        //set charset
        if(MysqlHelper::$UseUTF8) $mysqli->set_charset("utf8");
        //get stmt
        if(!($stmt = $mysqli->stmt_init())) return new ReturnObject(-2005, "stmt init error");
        if (!$stmt->prepare($sql)) return new ReturnObject(-2006, "stmt prepare error");
        $stmt->bind_param($paramsType, ...$params);
        $stmt->execute();
        //store result
        $stmt->store_result();

        $variables=array();
        $data=array();
        $array=array();

        $result = $stmt->result_metadata();
        Log::LogInfo(json_encode($result));
        if($result==false)
            return new ReturnObject(0,"this query can not fetch result if you want to get result ,use try SafeQuery!");

        while($field=$result->fetch_field())
            $variables[] = &$data[$field->name];
        call_user_func_array(array($stmt,'bind_result'),$variables);
        $index=0;
        while($stmt->fetch()){
            $array[$index]=array();
            foreach($data as $k=>$v)
            $array[$index][$k] = $v;
            $index++;
        }

        //close result、stmt and mysqli
        $result->close();
        $stmt->close();
        //$mysqli->close();
        return new ReturnObject(0, $array);
    }

    public static function S_IsEmptySet($sql, $paramsType, ...$params)
    {
        if(($ret=MysqlHelper::SafeQueryResult($sql,$paramsType,...$params))->resultcode<0) return $ret;
        $num=count($ret->data);
        unset($ret);
        return new ReturnObject(0,$num);
    }

}

class MysqlHelperOld
{
    private $mysqli;

    private function open()
    {
        $this->mysqli = new mysqli(host, username, password, db);
        if ($this->mysqli->connect_errno) return new ReturnObject(1000, "MySql connect failed!");
        return new ReturnObject(0, "success");
    }

    private function close()
    {
        if (isset($this->musqli)) {
            $this->mysqli->close();
        }
    }

    private function check()
    {
        if (!isset($this->mysqli))
            return new ReturnObject(-2000, "invaild mysqli");
        return new ReturnObject(0,"check ok");
    }

    private function __construct(array $arguments = array())
    {
        $this->open();
    }

    private function __clone()
    {
    }

    public function __destruct()
    {
        self::close();
    }

    public static function getMysql(){

    }

    public function getStmt($sql, $paramsType, ...$params)
    {
        $this->open();
        $stmt = $this->mysqli->stmt_init();
        if (!$stmt) return new ReturnObject(-2004, "stmt init error");
        log::LogInfo($sql);
        if (!$stmt->prepare($sql)) return new ReturnObject(-2004, "stmt prepare error");
        $stmt->bind_param($paramsType, ...$params);
        $stmt->execute();
        return new ReturnObject(0, $stmt);
    }

    public function SafeQuery($sql, $paramsType, ...$params)
    {
	    // foreach($params as $item) Log::LogInfo($item);
        $stmtRet = $this->getStmt($sql, $paramsType, ...$params);
        if ($stmtRet->resultcode != 0) return $stmtRet;
        $stmt = $stmtRet->data;
        $affect_row = $stmt->affected_rows;
        $error = $stmt->error;
        if ($affect_row < 0) {
            $stmt->close();
            return new ReturnObject(-2004, "bind_param error! {$error}");
        }
        $stmt->close();
        $this->close();
        return new ReturnObject(0, $affect_row);
    }

    public function SafeQueryResult($sql, $paramsType, ...$params)
    {
        $stmtRet = $this->getStmt($sql, $paramsType, ...$params);
        if ($stmtRet->resultcode != 0) return $stmtRet;
        $stmt = $stmtRet->data;
        $stmt->store_result();

        $variables=array();
        $data=array();
        $array=array();

        $result = $stmt->result_metadata();
        if($result==false)
            return new ReturnObject(-2010,"this query can not fetch result try SafeQuery");

        while($field=$result->fetch_field())
            $variables[] = &$data[$field->name];
        call_user_func_array(array($stmt,'bind_result'),$variables);
        $i=0;
        while($stmt->fetch()){
            $array[$i]=array();
            foreach($data as $k=>$v)
            $array[$i][$k] = $v;
            $i++;
        }

        $result->close();
        $stmt->close();
        $this->close();
        return new ReturnObject(0, $array);
    }

    public function S_IsEmptySet($sql, $paramsType, ...$params)
    {
        $ret = $this->SafeQueryResult($sql,$paramsType,...$params);
        if($ret->resultcode!=0) return $ret;
        $num=count($ret->data);
        unset($ret);
        $this->close();
        return new ReturnObject(0,$num);
    }

    public function Query($sql)
    {
        $this->open();
        xk_log_info("ExecuteSql>{$sql}");
        $result = $this->mysqli->query($sql);
        if (!$result) return new ReturnObject(2001, $this->mysqli->error . "error_sql: " . $sql);
        $this->close();
        return new ReturnObject(0, $result);
    }

    public function QueryResult($sql)
    {
        $this->open();
        $ret = $this->Query($sql);
        if ($ret->resultcode != 0) return $ret;

        $data=null;
        if (method_exists('mysqli_result', 'fetch_all')) {
            $data = $ret->data->fetch_all(MYSQLI_ASSOC);
            unset($ret);
            return new ReturnObject(0, 'error_sql:' . $sql);
        }
        for ($res = array(); $tmp = $ret->data->fetch_array(); ) $res[] = $tmp;
        $this->close();
        return new ReturnObject(0, $res);
    }

    public function emptySet($sql)
    {
        $this->open();
        $ret = $this->QueryResult($sql);
        if ($ret->resultcode != 0) return $ret;
        if (method_exists('mysqli_result', 'num_rows')) {
            echo $ret->data->num_rows;
            return new ReturnObject(0, $ret->data->num_rows);
        }
        $this->close();
        return new ReturnObject(0, count($ret->data));
    }

    static private $mInstance;

    static public function getInstance()
    {
        xk_log_info("getInstance...");
        if (!self::$mInstance instanceof self) self::$mInstance = new self();
        return self::$mInstance;
    }
}

class Log
{
    static public function LogInfo($msg)
    {
        if(_DEBUG==0) return ;
        $now=date("Y-m-d H-i-s");
        $logContent="[{$now}]  ".$msg.'  [FILE]'.__FILE__.'   [LINE]  '.__LINE__.PHP_EOL;
        if(filesize(LOG_FILE_NAME)/1024 >200) unlink(LOG_FILE_NAME);
        file_put_contents(LOG_FILE_NAME, $logContent, FILE_APPEND);
        if(_DEBUG=="1") echo "[{$now}]   ".$msg.PHP_EOL;
        unset($now);
        unset($logContent);
    }
}

class StringConver
{
    //converto utf-8
    function toUTF8($data){
        if( !empty($data) ){
          $fileType = mb_detect_encoding($data , array('UTF-8','GBK','LATIN1','BIG5')) ;
          if( $fileType != 'UTF-8'){
            $data = mb_convert_encoding($data ,'utf-8' , $fileType);
          }
        }
        return $data;
      }
    //deal with ' and "
    static public function CoverContent($strIn)
    {
        $ret = str_replace("'", "\\\'", $strIn);
        return str_replace("\"", "\\\"", $ret);
    }

    static public function Base64Encode($strIn)
    {
        return base64_encode($strIn);
    }

    //curl get
    public static function curlGet($url){
        $curl = curl_init();
        curl_setopt($curl, CURLOPT_URL, $url);
        curl_setopt($curl, CURLOPT_HEADER, false);
        curl_setopt($curl, CURLOPT_REFERER,$url);
        curl_setopt($curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/70.0.3538.67 Safari/537.36");
        curl_setopt($curl, CURLOPT_TIMEOUT, MAX_CURL_TIME);
        curl_setopt($curl, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($curl, CURLOPT_FOLLOWLOCATION, 0);
        curl_setopt($curl, CURLOPT_ENCODING, 'gzip, deflate');
        $values = curl_exec($curl);
        curl_close($curl);
        return ($values);
    }

    public static function httpImgToFile($imgUrl,$path=FILE_UPLOAD_PATH){
        if($imgUrl=='' || $imgUrl=='none') return new ReturnObject(-69,"empty image url");

        $direcotryPath=$path.xkTime::getYear().'/'.xkTime::getMonth().'/'.xkTime::getDay().'/';
        if(file_exists($direcotryPath)!=true)
            if(!mkdir($direcotryPath,0777,true)) return new ReturnObject(-71,"can not create direcotry: {$direcotryPath}");

        $imgFile=$direcotryPath.time().rand(1000,2000).".png";
        $data=StringConver::curlGet($imgUrl);

        if(!($r=file_put_contents($imgFile,$data))){
            FileOperator::deldir($imgFile);
            return new ReturnObject(-70,"write image content failed!");
        }

        return new ReturnObject(0, StringConver::getRelativePath($imgFile));
    }

    public static function Base64ToFile($base64Str,$path=FILE_UPLOAD_PATH){
        if($base64Str=='' || $base64Str=='none') return new ReturnObject(-51,"empty base64 str");

        if(strstr($base64Str,',')) {
            $base64Str=explode(',',$base64Str);
            $base64Str=$base64Str[1];
        }
        //create directory
        $direcotryPath=$path.xkTime::getYear().'/'.xkTime::getMonth().'/'.xkTime::getDay().'/';
        if(file_exists($direcotryPath)!=true)
            if(!mkdir($direcotryPath,0777,true)) return new ReturnObject(-40,"can not create direcotry: {$direcotryPath}");

        $imgFile=$direcotryPath.time().rand(1000,2000).'.png';

        if(!($r=file_put_contents($imgFile,base64_decode($base64Str))))
        {
            FileOperator::deldir($imgFile);
            return new ReturnObject(-50,"write image content failed!");
        }

        return new ReturnObject(0, StringConver::getRelativePath($imgFile));
    }

    public static $relateHeadPath='';

    // ../../../...upload/1.jpg to upload/1.jpg
    //and save ../../../... to relateHeadPath
    static public function getRelativePath($strPath){
        $path=''; $index=0; $relateHeadPath='';
        for($index=0;$index<($length=strlen($strPath));$index++){
            while($strPath[$index] =='.' || $strPath[$index]=='/')
            {
                $relateHeadPath=$relateHeadPath.$strPath[$index];
                $index++;
            }
            break;
        }
        for($index2=$index;$index2<$length;$index2++){
            $path=$path.$strPath[$index2];
        }
        StringConver::$relateHeadPath=$relateHeadPath;
        return $path;
    }

    static public function Base64Decode($strBase64)
    {
        return base64_decode($strBase64);
    }

    static public function Base64FileEncode($filePath)
    {
        if (!file_exists($filePath)) return new ReturnObject(-1, $filePath . "is not an exist file!");
        $img_info = getimagesize($filePath);
        $img_data = fread(fopen($filePath, 'r'), filesize($filePath));
        $ret = 'data:' . $img_info['mime'] . ';base64,' . chunk_split(base64_encode($img_data));
            //echo '<img src="'.$ret.'">';
        return new ReturnObject(0, str_replace("\r\n", "", $ret));
    }

    public static function dealWithImgContent($strContent)
    {
        $imgRegx = "/img.*?src=\"(?<imgname>.*?)\"/";
        preg_match_all($imgRegx, $strContent, $matchRet, PREG_PATTERN_ORDER);
        $replaceArray = array();
        $patternArray = array();
        $index = 0;
        foreach ($matchRet['imgname'] as $item) {
            if (strpos($item, "base64") != false || strpos($item, "http") != false) continue;
            $ret = StringConver::Base64FileEncode($item);
            if ($ret->resultcode != 0) return $ret;
            $replaceArray[$index] = $ret->data;
            $patternArray[$index] = '/' . $item . '/';
            unset($ret);
            $index++;
        }
        ksort($replaceArray);
        ksort($patternArray);
        return new ReturnObject(0, preg_replace($patternArray, $replaceArray, $strContent));
    }

    public static function have($subjectStr,$legalSubstrArray){
        $bFind=0;
        $subjectUpper=mb_strtoupper($subjectStr);
        foreach($legalSubstrArray as $item)
            if(strpos($subjectUpper,mb_strtoupper($item))!=FALSE){
                $bFind=1;break;
            }
        return $bFind;
    }
}

function xk_log_info($msg, $show = false)
{
    if ($show)
        echo $msg . PHP_EOL;
}

class xkTime{
    //Internal
    public static function getYearInternal($startTime,$endTime){
        return round(xkTime::getMonthInternal($startTime,$endTime)/12);
    }

    public static function getMonthInternal($startTime,$endTime){
        return round(xkTime::getDayInternal($startTime,$endTime)/30);
    }

    public static function getDayInternal($startTime,$endTime){
        return round(xkTime::getHourInternal($startTime,$endTime)/24);
    }

    public static function getHourInternal($startTime,$endTime){
        return round(xkTime::getMinuteInternal($startTime,$endTime)/60);
    }

    public static function getMinuteInternal($startTime,$endTime){
        return round(xkTime::getSecondInternal($startTime,$endTime)/60);
    }

    public static function getSecondInternal($startTime,$endTime){
        $start_time = strtotime($startTime);
        $end_time = strtotime($endTime);
        return $end_time - $start_time;
    }

    //time_cover
    public static function getTimeStamp($time){
        return strtotime($time);
    }

    public static function getYear(){
        return date("Y");
    }

    public static function getMonth(){
        return date("m");
    }

    public static function getDay(){
        return date("d");
    }
}

class FileUploadConfig{
    public $fileSavePath;
    public $fileFilterArray;
    public $maxFileUploadSize;

    public function __construct($maxFileUploadSize=DEFAULT_MAX_UPLOAD_FILE_SIZE,$fileFilterArray='none',$fileSavePath=FILE_UPLOAD_PATH){
        $this->$fileSavePath=$fileSavePath;
        $this->$fileFilterArray=$fileFilterArray;
        $this->$maxFileUploadSize=$maxFileUploadSize;
    }
}

class FileOperator
{
    public static function getFile($filename,$retValue='',$maxFileUploadSize=DEFAULT_MAX_UPLOAD_FILE_SIZE,$fileFilterArray='none',$fileSavePath=FILE_UPLOAD_PATH){

        if(!isset($_FILES[$filename])) return $retValue;
        $file=$_FILES[$filename];

        $direcotryPath=$fileSavePath.xkTime::getYear().'/'.xkTime::getMonth().'/'.xkTime::getDay().'/';
        if(file_exists($direcotryPath)!=true)
            if(!mkdir($direcotryPath,0777,true)) return $retValue;

        if(($ret=FileOperator::uploadFile($file,$maxFileUploadSize,$fileFilterArray,$direcotryPath))->resultcode<0) {
            unset($ret);
            return 'long';
        }
        $path=$ret->data; unset($ret);
        return $path;
    }

    public static function uploadFile($file,$maxFileUploadSize=DEFAULT_MAX_UPLOAD_FILE_SIZE,$fileFilterArray='none',$fileSavePath=FILE_UPLOAD_PATH)
    {
        if(!isset($file)) return new ReturnObject(0,"EMPTY FILE");
        if ($file == "") return new ReturnObject(0, "EMPTY FILE");

        $real_name = $file['name'];

        //filter
        if($fileFilterArray!='none' && !StringConver::have($real_name,$fileFilterArray)) return new ReturnObject(0,UNLEGAL_VALUE);

        $temp_name = $file['tmp_name'];
        $size = $file['size'] * (1.0/1024);

        if ($file["error"] > 0)
            return new ReturnObject(-2004, $file["error"]);

        // echo "size=".$size;
        if ($size > ($maxFileUploadSize * 1024) || $size == 0)
            return new ReturnObject(-2004, "file size error! size:{$size},max_size:{$maxFileUploadSize}");

        
        if (file_exists(FILE_UPLOAD_PATH . $real_name))
            return new ReturnObject(-2004, "file repeat");

        $path = $fileSavePath . time() . $real_name;

        if (!move_uploaded_file($temp_name, $path))
            return new ReturnObject(-2004, "file upload failed!");

        unlink($temp_name);
        return new ReturnObject(0, StringConver::getRelativePath($path));
    }

    public static function deldir($path,$useRelatePath=true)
    {
        if($useRelatePath) $path=FILE_DELETE_PATH.$path;

        // if (is_dir($path)) {
        //     $p = scandir($path);
        //     foreach ($p as $val) {
        //         if ($val != "." && $val != "..") {
        //             if (is_dir($path . $val)) {
        //                 deldir($path . $val);
        //                 @rmdir($path . $val);
        //             } else
        //                 unlink($path . $val);
        //         }
        //     }
        // } else {
        if($path==FILE_DELETE_PATH) return;
        if(file_exists($path)) unlink($path);

    }

    public static function makedir($dirname, $dirpath)
    {
        $path = $dirname . '/' . $dirpath;
        if (empty($dirpath)) {
            $path = '/' . $dirname;
        }
        mkdir($path);
        chmod($path, 777);
    }
}



