<?php 
    include_once('xk_shows/test.php');

    echo json_encode((function(){
        header("content-type:application/json");
        return xk_shows::FindByShowId(
            GetRealValueSafe('value'),
            GetRealValueSafe('page',1),
            GetRealValueSafe('num',10000));
    })());