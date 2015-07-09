#!/usr/bin/php -f
<?php
/** \example test_gvgampclient.php
* \brief A short example of how to use the GvgAmpClient class in PHP.
*/


  include("pGvgAmpClient.php");
  $folders=array();
  $clips=array();
  $result=0;
  $cwd="";   //The working directory
  $afd=0;    //The AFD value
  $ws=0;     //The widescreen value
  try {  
      $client = new GvgAmpClient("192.168.10.25");
     
      echo "Listing folders...\n";
      $result=$client->ListFolders($folders);
      foreach ($folders as $folder) {
         echo "   ".$folder."\n";
      }
      echo "Total folders:".$result."\n";

      echo "Current working folder: ";

      $client->GetWorkingFolder($cwd);
      echo $cwd."\n";

      echo "Set working folder to TESTBE: ";
      if ($client->SetWorkingFolder("TESTBE")==0) 
         echo "SUCCESS\n";
      else {
         echo "FAILED. Exiting...\n";
         goto DISCONNECT;
      }

      echo "Current working folder: ";
      $client->GetWorkingFolder($cwd);
      echo $cwd."\n";

      echo "Set working folder to EDIUS: ";
      if ($client->SetWorkingFolder("EDIUS")==0) 
         echo "SUCCESS\n";
      else {
         echo "FAILED. Exiting...\n";
         goto DISCONNECT;
      }

      echo "Current working folder: ";
      $client->GetWorkingFolder($cwd);
      echo $cwd."\n";

      echo "Listing clips...\n";
      echo "Clipname                               AFD Flag     Widescreen Flag";
      echo "-------------------------------------------------------------------";
      $client->ListIDs($clips,1);
      foreach($clips as $clip) {
          $client->GetAFDSetting($clip,$afd,$ws);
          echo $clip."                         ".$afd."      ".$ws."\n";
      }
  } catch (SocketException $e) {
      echo "Exception:".$e.describe()."\n";
  }      
  DISCONNECT:
  unset($client);

?>
