<?php
  header('Content-type: image/gif');

  $img = imagecreate(125,15);
  $fgcolor = imagecolorallocate($img,0,255,255); // cyan
  $bgcolor = imagecolorallocate($img,0,0,0);	 // black
  imagefill($img, 0, 0, $bgcolor);
 
  exec("ls -l /infres/ftp/pub/vreng/vreng.tar.gz", $ls, $ret);
  for ($i=0; $i < count($ls); $i++)
  { 
    $tabls = $ls[$i];
    $ils = explode(" ", $tabls);
    $nls = count($ils)-1;
    $file = $ils[$nls];
    $ifile = explode("-", $file);
    $str = $ifile[1];
    $items = explode(".", $str);
    $n = 1;
    $vers = "vreng-".$items[0];
    while ($items[$n] != "tar") {
      $vers.=".".$items[$n];
      $n++;
    }
  }
  $targz = ".tar.gz";
  $vers = $vers.$targz;

  imagestring($img, 3, 0, 3, $vers, $fgcolor);
  imagegif($img);
?>
