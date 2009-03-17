<?php
$connectBTG = false;
require("btg.php");

if(!isset($_FILES['torrent_file']))
	die();

$file = $_FILES['torrent_file'];

@header("Content-Type: text/xml");
echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
?>
<response>
<?php
try
{
	echo $btg->contextCreateFromUpload($file);
}catch(Exception $e)
{
	$output.= "<error>".$e->getMessage()."</error>";
	$output.= "<debug>Got Exception. Debug trace:\n";
	$output.= $e->getTraceAsString();
	$output.= "</debug>";
	echo $output;
}
?>
</response>
