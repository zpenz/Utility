<?php 
    include_once('xk_shows/test.php');

    echo json_encode((function(){
        header("content-type:application/json");
        return xk_shows::GetTotalCount();
    })());