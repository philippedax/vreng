<?php
  header('Content-type: image/gif');
  //$ladate="      ".date("d/m/Y");

  $image = imagecreate(125,15);
  $colorblue = imagecolorallocate($image,0,255,255);
  $colorwhite = imagecolorallocate($image,0,0,0);
  imagefill($image, 0, 0, $colorwhite);
 
  exec("ls -l /infres/ftp/pub/vreng/vreng.tar.gz",$result,$return);
  for ($index=0; $index<count($result); $index++)
  { 
    $nom1 = $result[$index];
    $items1 = explode(" ", $nom1);
    $n1 = count($items1)-1;
    $nom2 = $items1[$n1];
    $items2 = explode("-", $nom2);
    $n2 = count($items2)-1;

    // bord
    //$h = imagesx($image)-1;
    //$l = imagesy($image)-1;

    $nom3 = $items2[1];
    $items3 = explode(".", $nom3);
    $n = 1;
    $version = "vreng-".$items3[0];
    while ($items3[$n] != "tar") {
      $version.=".".$items3[$n];
      $n++;
    }
  }
  $targz = ".tar.gz";
  $version = $version.$targz;
  echo "version = $version";

  //imagestring($image, 3, 0, 3, $version, $colorblue);
  //imagegif($image);
?>
