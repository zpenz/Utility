<?php 
    include_once('xk_shows/test.php');

    echo json_encode((function(){
        header("content-type:application/json");
        return (new xk_shows(
            GetRealValueSafe("show_id"),
            GetRealValueSafe("show_title"),
            GetRealValueSafe("show_describe"),
            GetRealValueSafe("show_author"),
            GetRealValueSafe("show_content"),
            GetRealValueSafe("show_img"),
            GetRealValueSafe("show_position"),
            GetRealValueSafe("show_total_type"),
            GetRealValueSafe("show_type"),
            GetRealValueSafe("update_time"),
            GetRealValueSafe("show_for_vip"),
            GetRealValueSafe("xk_videolink")
            ))->Add();
    })());