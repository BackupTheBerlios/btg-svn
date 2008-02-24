<?php
header("Cache-Control: no-cache");
header("Expires: Mon, 01 Jan 1980 00:00:00 GMT");
$connectBTG = false;
require("btg.php");
echo $ajax->get_js();
?>
