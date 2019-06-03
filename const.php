<?php
    //类别
    define("XK_SHOW_TYPE_VIDEO",0);
    define("XK_SHOW_TYPE_ACTICLE",1);  
    define("XK_SHOW_TYPE_ANNOCE",2);
    define("XK_SHOW_TYPE_DOCUMENT",3);
    define("XK_SHOW_TYPE_SOFTWARE",4);
    define("XK_SHOW_TYPE_PDF",5);
    define("XK_SHOW_TYPE_NEWS",6);

    //登陆账户密码
    define("XK_LOGIN_USERNAME",'');
    define("XK_LOGIN_SIMPLE_PASSWORD",'');

    $XK_TYPE_ARRAY = array(
        "原理",
        "应用",
        "NASDIY",
        "视频链接",
        "使用手册",
        "应用程序",
        "套件",
        "移动端程序",
        "新闻");

    //must have post value or get vaule
    function GetRealValue($value){
        if(!isset($_POST[$value])){
            return $_GET[$value];
        }
        return $_POST[$value];
    }

    function GetRealValueSafe($valueName,$defaultValue=''){
        if(!isset($_POST[$valueName]))
            if(!isset($_GET[$valueName])) return $defaultValue;

        if(!isset($_POST[$valueName])){
            return $_GET[$valueName];
        }
        return $_POST[$valueName];
    }

    function ConverTypeEx($kind){
        global $XK_TYPE_ARRAY ;
        $valueName = GetRealValue($kind);
        $converValue = (int)$valueName;
        if($converValue == 0 || $converValue > count($XK_TYPE_ARRAY))
            return $valueName;
        return $XK_TYPE_ARRAY[($converValue-1)];
    }

    function ConverType($valueName){
        global $XK_TYPE_ARRAY ;
        $converValue = (int)$valueName;
        if($converValue == 0 || $converValue > count($XK_TYPE_ARRAY))
            return $valueName;
        return $XK_TYPE_ARRAY[($converValue-1)];
    }

    function ConverTypeToString($_array,$valueName){
        $converValue = (int)$valueName;
        if($converValue <= 0 || $converValue > count($_array))
            return $valueName;
        return $_array[($converValue-1)];
    }

    //a = {1,2,3} f(a,1) = 1 f(a,4,0) = 0;
    function SafeGatArrayValue($_array,$valueName,$noMatchValue=""){
        foreach($_array as $item) if($item == $valueName) return $valueName;
        return $noMatchValue;
    }

