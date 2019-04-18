<?php
    require_once(dirname(__FILE__).'/'."../common.php");
    require_once(dirname(__FILE__).'/'."../const.php");

    class externTool{
        public static function tAddCanModify(){
            xkSession::addSession("modify","YES");
        }

        public static function tCheckCanModify(){
            if(xkSession::getsession("modify")=='YES')  return true;
            header("Location: ../error.php");
            return false;
        }

        public static function empty($str){
            if($str='none' || $str= '') return false;
            return true;
        }

        public static function specifyStringToArray($subjectStr,$pattern,$subGroup=1){
            $retArray=array();
            preg_match_all($pattern,$subjectStr,$matches,PREG_SET_ORDER);
            foreach($matches as $item){
                array_push($retArray,$item[$subGroup]);
            }
            return $retArray;
        }

        public static function arrayToStr($subjectArray,$splitC=' '){
            $retStr='';
            $length=count($subjectArray);
            for($index=0;$index<$length;$index++){
                if($index==0) {$retStr=$subjectArray[$index];continue;}
                $retStr=$retStr.$splitC.$subjectArray[$index];
            }
            unset($subjectArray);
            return $retStr;
        }

        public static function split($subjectStr,$splitC=","){
            //remove more "
            $subjectStr=str_replace('"','',$subjectStr);
            $retArray= array(); $item='';
            for($index=0;$index<strlen($subjectStr);$index++)
            {
                if($subjectStr[$index]==$splitC)
                {
                    array_push($retArray,$item);
                    $item='';
                    $index++;
                }
                $item=$item.$subjectStr[$index];
            }
            if($item!='') array_push($retArray,$item);
            return $retArray;
        }

        //temp str='none'
        public static function deWeight($subjectArray){
            if(is_array($subjectArray)==false || ($length=count($subjectArray))==0) return array();
            //repeat index set to none
            for($indexX=0;$indexX<$length;$indexX++){
                if($subjectArray[$indexX]=='none') continue;
                for($indexY=$indexX+1;$indexY<$length;$indexY++){
                    if($subjectArray[$indexY]!='none' && $subjectArray[$indexX]==$subjectArray[$indexY]) $subjectArray[$indexY]='none';
                }
            }

            //remove none
            $retArray=array();
            for($indexX=0;$indexX<$length;$indexX++){if($subjectArray[$indexX]!='none') array_push($retArray,$subjectArray[$indexX]);}
            return $retArray;
        }
        //deWeight array element item
        public static function deWeightEx($subjectArray,$index){
            if(is_array($subjectArray)==false || ($length=count($subjectArray))==0) return array();
            //repeat index set to none
            for($indexX=0;$indexX<$length;$indexX++){
                if($subjectArray[$indexX][$index]=='none') continue;
                for($indexY=$indexX+1;$indexY<$length;$indexY++){
                    if($subjectArray[$indexY][$index]!='none' && $subjectArray[$indexX][$index]==$subjectArray[$indexY][$index]) $subjectArray[$indexY][$index]='none';
                }
            }

            //remove none
            $retArray=array();
            for($indexX=0;$indexX<$length;$indexX++){if($subjectArray[$indexX][$index]!='none') array_push($retArray,$subjectArray[$indexX]);}
            return $retArray;
        }

        public static function tCheckIfHaveUnLagelChar($testString){
            global $XK_UNLEGAL_ARRAY;
            for($index=0;$index<strlen($testString);$index++)
                foreach($XK_UNLEGAL_ARRAY as $value){
                if($testString[$index]===$value)
                    return new ReturnObject(-14,array('find not legal char!',$value));
                }
            return new ReturnObject(0,"check ok!");
        }

        public static function tCheckAccount($testString,$minLength=3){
            $length=strlen($testString);
            if($length>20) return new ReturnObject(-17,"the account length is too long!");
            if($length<$minLength) return new ReturnObject(-16,"account must have at least {$minLength} length");
            $matchRet = preg_match('/^([1-9a-zA-Z])([0-9a-zA-Z]+)$/',$testString);
            if($matchRet!=1) return new ReturnObject(-18,"account can only contain pure number or alphabet");
            return new ReturnObject(0,"check account ok!");
        }

        public static function tCheckMaxLength($testString,$maxLength=30){
            if(strlen($testString)>$maxLength) return new ReturnObject(-19,"can not more than {$maxLength}");
            return new ReturnObject(0,"ok");
        }

        public static function tCheckRecordLegal($testString,$legalReacordArray){
            foreach($legalReacordArray as $item)
                if($testString===$item) return new ReturnObject(-21,"not legal record");
            return new ReturnObject(0,"ok");
        }

        //replace html base64 file to local file
        //return all-img-array、replace-html
        //save http img to local
        public static function htmlReplace($strHtmlContent){
            //limit not http://img count
            preg_match_all('/<img src="((?!http).+)"/',
            $strHtmlContent,$match);
            if(count($match[1])>MAX_UPLOAD_FILE_COUNT) {
                return new ReturnObject(-81,"image count can not more than ".MAX_UPLOAD_FILE_COUNT);
            }
            unset($match);

            $localImg=array();
            //conver base64
            $replaceHtml = preg_replace_callback('/<img[^\/]+src="(data)(.*?)"/',function($match)use($localImg){
                //print_r($match);
                if(($ret=StringConver::Base64ToFile($match[2]))->resultcode<0) {
                    return $ret;
                }
                $imgPath=$ret->data;
                unset($ret);
                return '<img src="'.$imgPath.'"';
            },$strHtmlContent);

            if($replaceHtml==null) {
                $replaceHtml=$strHtmlContent;
            }

            //conver http: img
            $replaceHtml2=null;
            if(!SAVE_ABSOULT_PATH ){
                $replaceHtml2 = preg_replace_callback('/<img[^\/]+src="(http.*?)"/',function($match)use($localImg){
                    // if(($ret=StringConver::httpImgToFile(html_entity_decode($match[1])))->resultcode<0){
                    //     return $ret;
                    // }
                    // $imgPath=$ret->data;
                    // unset($ret);
                    // return '<img class="httpImage" src="'.$imgPath.'"';
                    return '<img class="httpImage" src="'.$match[1].'"';
                },$replaceHtml);
            }

            if($replaceHtml2==null) {
                $replaceHtml2=$replaceHtml;
            }

            //get all img path
            preg_match_all('/<img src="(.*?)"/',
            $replaceHtml2,$match);
            foreach($match[1] as $item){
                array_push($localImg,$item);
            }
            //get all html src img path
            preg_match_all('/<img class="httpImage" src="(.*?)"/',
            $replaceHtml2,$match);
            foreach($match[1] as $item){
                array_push($localImg,$item);
            }

            return new ReturnObject(0,array('images'=>$localImg,'content'=>base64_encode($replaceHtml2)));
        }

        //find not exist element
        public static function getNotExistElements($oldArray,$newArray){
            $oLength=count($oldArray);
            $nLength=count($newArray);
            $retArray=array();
            for($indexX=0;$indexX<$oLength;$indexX++){
                for($indexY=0;$indexY<$nLength;$indexY++){
                    if($oldArray[$indexX]==$newArray[$indexY]) goto NEXT;
                }
                //not in new array
                array_push($retArray,$oldArray[$indexX]);
                NEXT: ;
            }

            if(count($retArray)==0) return null;
            return $retArray;
        }

        //get brief info
        public static function getBriefStr($subjectStr,$maxLength=50){
            if(($length=strlen($subjectStr))==0) return '';
            if($length<$maxLength) return substr($subjectStr,0,$length/2).'...';
            return substr($subjectStr,0,$maxLength).'...';
        }
    }

    class Map{
        public function get($key){
            return isset($this->$key)?$this->$key:null;
        }
        public function put($key,$value){
            $this->$key=$value;
        }
    }

    class DFA
    {
        private $arrHashMap = [];

        public function getHashMap() {
            print_r($this->arrHashMap);
        }

        public function addKeyWord($strWord) {
            $len = mb_strlen($strWord, 'UTF-8');

            // 传址
            $arrHashMap = &$this->arrHashMap;
            for ($i=0; $i < $len; $i++) {
                $word = mb_substr($strWord, $i, 1, 'UTF-8');
                // 已存在
                if (isset($arrHashMap[$word])) {
                    if ($i == ($len - 1)) {
                        $arrHashMap[$word]['end'] = 1;
                    }
                } else {
                    // 不存在
                    if ($i == ($len - 1)) {
                        $arrHashMap[$word] = [];
                        $arrHashMap[$word]['end'] = 1;
                    } else {
                        $arrHashMap[$word] = [];
                        ($arrHashMap[$word])["end"] = 0;
                    }
                }
                // 传址
                $arrHashMap = &$arrHashMap[$word];
            }
        }

        public function searchKey($strWord) {
            $len = mb_strlen($strWord, 'UTF-8');
            $arrHashMap = $this->arrHashMap;
            for ($i=0; $i < $len; $i++) {
                $word = mb_substr($strWord, $i, 1, 'UTF-8');
                if (!isset($arrHashMap[$word])) {
                    // reset hashmap
                    $arrHashMap = $this->arrHashMap;
                    continue;
                }
                if ($arrHashMap[$word]['end']) {
                    return true;
                }
                $arrHashMap = $arrHashMap[$word];
            }
            return false;
        }

        public function replaceKey(&$strWord,$strChar='*') {
            $arrHashMap = $this->arrHashMap;
            $splitArray=DFA::unicodeSplit($strWord);
            $len = count($splitArray);
            $startPos=-1;
            for($index=0;$index<$len;$index++){
                $word=$splitArray[$index];
                if(!isset($arrHashMap[$word])){
                    //reset
                    $arrHashMap=$this->arrHashMap;
                    $startPos=-1;
                    continue;
                }
                if($startPos==-1) $startPos=$index;

                if($arrHashMap[$word]['end']){
                    for($jndex=$startPos;$jndex<=$index;$jndex++)
                        $splitArray[$jndex]=$strChar;
                    $startPos=0;
                }

                $arrHashMap = $arrHashMap[$word];
            }

            $ret='';
            foreach($splitArray as $splitItem){
                $ret.=$splitItem;
            }
            $strWord=$ret;
            return $ret;

        }

        // 将字符串分割成数组
        static public  function unicodeSplit($str, $caseword = true)
        {
            if ($caseword)
                $str = strtolower($str);
            $ret = array();
            $len = strlen($str);
            for ($i = 0; $i < $len; $i++) {
                $c = ord($str[$i]);

                if ($c & 0x80) {
                    if (($c & 0xf8) == 0xf0 && $len - $i >= 4) {
                        if ((ord($str[$i + 1]) & 0xc0) == 0x80 && (ord($str[$i + 2]) & 0xc0) == 0x80 && (ord($str[$i + 3]) & 0xc0) == 0x80) {
                            $uc = substr($str, $i, 4);
                            $ret[] = $uc;
                            $i += 3;
                        }
                    } else if (($c & 0xf0) == 0xe0 && $len - $i >= 3) {
                        if ((ord($str[$i + 1]) & 0xc0) == 0x80 && (ord($str[$i + 2]) & 0xc0) == 0x80) {
                            $uc = substr($str, $i, 3);
                            $ret[] = $uc;
                            $i += 2;
                        }
                    } else if (($c & 0xe0) == 0xc0 && $len - $i >= 2) {
                        if ((ord($str[$i + 1]) & 0xc0) == 0x80) {
                            $uc = substr($str, $i, 2);
                            $ret[] = $uc;
                            $i += 1;
                        }
                    }
                } else {
                    $ret[] = $str[$i];
                }
            }

            return $ret;
        }



    }

    class Filter{
        //XSS
        public static function filterHtml($subjectHtml){
            return htmlentities(html_entity_decode($subjectHtml,ENT_QUOTES,'UTF-8'),ENT_QUOTES,"UTF-8");
        }
        //badWord
        public static $badWordMap;
        public static function initBadWordMap($badWord){
            if(!isset(Filter::$badWordMap)) Filter::$badWordMap=new Map();
            $wordMap=Filter::$badWordMap;

            $length=mb_strlen($badWord);
            $tempMap=new Map();
            for($index=0;$index<$length;$index++)
            {
                $currentWord=mb_substr($badWord,$index,1);
                $current=$wordMap->get($currentWord);
                if(is_null($current))
                {
                    $tempMap->put($badWord[$index],0);

                    $wordMap=$tempMap;

                }else{
                    $tempMap = $wordMap[$current];
                }

                if($index==$length-1) {$wordMap->put('end',1);break;}
            }
            Filter::$badWordMap = $wordMap;
        }
    }

    class VerifyCode{
        public static function genVerifyCode(){
            session_start();
            $image=imagecreatetruecolor(150, 60);//imagecreatetruecolor函数建一个真彩色图像

            //白色背景
            $bgColor=imagecolorallocate($image, 255,255,255);
            imagefill($image, 0, 0, $bgColor);

            $captchCode=""; //verifyCode

            $data='abcdefghijkmnpqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ23456789';
            $fontSize=20;
            $dataLegalIndex=strlen($data)-1;

            $fontPath=dirname(__FILE__).'/'."/../font/SourceHanSansCN-Regular.otf";

            //干扰点
            for ($i=0;$i<20;$i++) {
                $color = imagecolorallocate($image,mt_rand(200,255),mt_rand(200,255),mt_rand(200,255));
                imagestring($image,mt_rand(1,5),mt_rand(0,150),mt_rand(0,60),'*',$color);
            }

            //干扰线
            for ($i=0;$i<=4;$i++){
                $x1=rand(0,149);
                $y1=rand(0,59);
                $x2=rand(0,149);
                $y2=rand(0,59);
                $linecolor=imagecolorallocate($image,rand(0,156),rand(0,156),rand(0,156));
                imageline($image,$x1,$y1,$x2,$y2,$linecolor);
            }

            $_x=intval(150/4);
            $_y=intval(60*0.7);
            //该循环,循环取数
            for($i=0;$i<4;$i++){

                $fontContent=substr($data,rand(0,$dataLegalIndex),1);

                $fontColor=imagecolorallocate($image,rand(0,100),rand(0,100),rand(0,100));
                imagettftext($image,$fontSize,mt_rand(-30,30),$i*$_x+3,$_y,$fontColor,$fontPath,$fontContent);
                $captchCode.=$fontContent;
            }

            $_SESSION['verifyCode']=strtoupper($captchCode);//save verifyCode

            header('content-type:image/png');
            imagepng($image);
            //销毁
            imagedestroy($image);
        }

        public static function checkIsVerify($inputVerifyCode){
            session_start();
            if(!isset($_SESSION['verifyCode'])) return new ReturnObject(-60,"null");

            $code= $_SESSION['verifyCode'];
            if(strtoupper($inputVerifyCode)!=$code) return new ReturnObject(-61,"check failed!");
            return new ReturnObject(0,"check ok!");
        }
    }

?>
