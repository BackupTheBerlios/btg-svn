<?php

/*
* PHP URL encoding/decoding functions for Javascript interaction V3.0
* (C) 2006 www.captain.at - all rights reserved
* License: GPL
*/

/* 
* wojci: removed all other functions than the one used for decoding
* output of javascript encodeURIComponent function.
*/

function decodeURIComponent($string) {
   $result = "";
   for ($i = 0; $i < strlen($string); $i++) {
       $decstr = "";
       for ($p = 0; $p <= 8 && ($i+$p) < strlen($string); $p++) {
          $decstr .= $string[$i+$p];
       } 
       list($decodedstr, $num) = decodeURIComponentbycharacter($decstr);
       $result .= urldecode($decodedstr);
       $i += $num ;
   }
   return $result;
}

function decodeURIComponentbycharacter($str) {

   $char = $str;
   
   if ($char == "%E2%82%AC") { return array("%80", 8); }
   if ($char == "%E2%80%9A") { return array("%82", 8); }
   if ($char == "%E2%80%9E") { return array("%84", 8); }
   if ($char == "%E2%80%A6") { return array("%85", 8); }
   if ($char == "%E2%80%A0") { return array("%86", 8); }
   if ($char == "%E2%80%A1") { return array("%87", 8); }
   if ($char == "%E2%80%B0") { return array("%89", 8); }
   if ($char == "%E2%80%B9") { return array("%8B", 8); }
   if ($char == "%E2%80%98") { return array("%91", 8); }
   if ($char == "%E2%80%99") { return array("%92", 8); }
   if ($char == "%E2%80%9C") { return array("%93", 8); }
   if ($char == "%E2%80%9D") { return array("%94", 8); }
   if ($char == "%E2%80%A2") { return array("%95", 8); }
   if ($char == "%E2%80%93") { return array("%96", 8); }
   if ($char == "%E2%80%94") { return array("%97", 8); }
   if ($char == "%E2%84%A2") { return array("%99", 8); }
   if ($char == "%E2%80%BA") { return array("%9B", 8); }

   $char = substr($str, 0, 6);

   if ($char == "%C2%81") { return array("%81", 5); }
   if ($char == "%C6%92") { return array("%83", 5); }
   if ($char == "%CB%86") { return array("%88", 5); }
   if ($char == "%C5%A0") { return array("%8A", 5); }
   if ($char == "%C5%92") { return array("%8C", 5); }
   if ($char == "%C2%8D") { return array("%8D", 5); }
   if ($char == "%C5%BD") { return array("%8E", 5); }
   if ($char == "%C2%8F") { return array("%8F", 5); }
   if ($char == "%C2%90") { return array("%90", 5); }
   if ($char == "%CB%9C") { return array("%98", 5); }
   if ($char == "%C5%A1") { return array("%9A", 5); }
   if ($char == "%C5%93") { return array("%9C", 5); }
   if ($char == "%C2%9D") { return array("%9D", 5); }
   if ($char == "%C5%BE") { return array("%9E", 5); }
   if ($char == "%C5%B8") { return array("%9F", 5); }
   if ($char == "%C2%A0") { return array("%A0", 5); }
   if ($char == "%C2%A1") { return array("%A1", 5); }
   if ($char == "%C2%A2") { return array("%A2", 5); }
   if ($char == "%C2%A3") { return array("%A3", 5); }
   if ($char == "%C2%A4") { return array("%A4", 5); }
   if ($char == "%C2%A5") { return array("%A5", 5); }
   if ($char == "%C2%A6") { return array("%A6", 5); }
   if ($char == "%C2%A7") { return array("%A7", 5); }
   if ($char == "%C2%A8") { return array("%A8", 5); }
   if ($char == "%C2%A9") { return array("%A9", 5); }
   if ($char == "%C2%AA") { return array("%AA", 5); }
   if ($char == "%C2%AB") { return array("%AB", 5); }
   if ($char == "%C2%AC") { return array("%AC", 5); }
   if ($char == "%C2%AD") { return array("%AD", 5); }
   if ($char == "%C2%AE") { return array("%AE", 5); }
   if ($char == "%C2%AF") { return array("%AF", 5); }
   if ($char == "%C2%B0") { return array("%B0", 5); }
   if ($char == "%C2%B1") { return array("%B1", 5); }
   if ($char == "%C2%B2") { return array("%B2", 5); }
   if ($char == "%C2%B3") { return array("%B3", 5); }
   if ($char == "%C2%B4") { return array("%B4", 5); }
   if ($char == "%C2%B5") { return array("%B5", 5); }
   if ($char == "%C2%B6") { return array("%B6", 5); }
   if ($char == "%C2%B7") { return array("%B7", 5); }
   if ($char == "%C2%B8") { return array("%B8", 5); }
   if ($char == "%C2%B9") { return array("%B9", 5); }
   if ($char == "%C2%BA") { return array("%BA", 5); }
   if ($char == "%C2%BB") { return array("%BB", 5); }
   if ($char == "%C2%BC") { return array("%BC", 5); }
   if ($char == "%C2%BD") { return array("%BD", 5); }
   if ($char == "%C2%BE") { return array("%BE", 5); }
   if ($char == "%C2%BF") { return array("%BF", 5); }
   if ($char == "%C3%80") { return array("%C0", 5); }
   if ($char == "%C3%81") { return array("%C1", 5); }
   if ($char == "%C3%82") { return array("%C2", 5); }
   if ($char == "%C3%83") { return array("%C3", 5); }
   if ($char == "%C3%84") { return array("%C4", 5); }
   if ($char == "%C3%85") { return array("%C5", 5); }
   if ($char == "%C3%86") { return array("%C6", 5); }
   if ($char == "%C3%87") { return array("%C7", 5); }
   if ($char == "%C3%88") { return array("%C8", 5); }
   if ($char == "%C3%89") { return array("%C9", 5); }
   if ($char == "%C3%8A") { return array("%CA", 5); }
   if ($char == "%C3%8B") { return array("%CB", 5); }
   if ($char == "%C3%8C") { return array("%CC", 5); }
   if ($char == "%C3%8D") { return array("%CD", 5); }
   if ($char == "%C3%8E") { return array("%CE", 5); }
   if ($char == "%C3%8F") { return array("%CF", 5); }
   if ($char == "%C3%90") { return array("%D0", 5); }
   if ($char == "%C3%91") { return array("%D1", 5); }
   if ($char == "%C3%92") { return array("%D2", 5); }
   if ($char == "%C3%93") { return array("%D3", 5); }
   if ($char == "%C3%94") { return array("%D4", 5); }
   if ($char == "%C3%95") { return array("%D5", 5); }
   if ($char == "%C3%96") { return array("%D6", 5); }
   if ($char == "%C3%97") { return array("%D7", 5); }
   if ($char == "%C3%98") { return array("%D8", 5); }
   if ($char == "%C3%99") { return array("%D9", 5); }
   if ($char == "%C3%9A") { return array("%DA", 5); }
   if ($char == "%C3%9B") { return array("%DB", 5); }
   if ($char == "%C3%9C") { return array("%DC", 5); }
   if ($char == "%C3%9D") { return array("%DD", 5); }
   if ($char == "%C3%9E") { return array("%DE", 5); }
   if ($char == "%C3%9F") { return array("%DF", 5); }
   if ($char == "%C3%A0") { return array("%E0", 5); }
   if ($char == "%C3%A1") { return array("%E1", 5); }
   if ($char == "%C3%A2") { return array("%E2", 5); }
   if ($char == "%C3%A3") { return array("%E3", 5); }
   if ($char == "%C3%A4") { return array("%E4", 5); }
   if ($char == "%C3%A5") { return array("%E5", 5); }
   if ($char == "%C3%A6") { return array("%E6", 5); }
   if ($char == "%C3%A7") { return array("%E7", 5); }
   if ($char == "%C3%A8") { return array("%E8", 5); }
   if ($char == "%C3%A9") { return array("%E9", 5); }
   if ($char == "%C3%AA") { return array("%EA", 5); }
   if ($char == "%C3%AB") { return array("%EB", 5); }
   if ($char == "%C3%AC") { return array("%EC", 5); }
   if ($char == "%C3%AD") { return array("%ED", 5); }
   if ($char == "%C3%AE") { return array("%EE", 5); }
   if ($char == "%C3%AF") { return array("%EF", 5); }
   if ($char == "%C3%B0") { return array("%F0", 5); }
   if ($char == "%C3%B1") { return array("%F1", 5); }
   if ($char == "%C3%B2") { return array("%F2", 5); }
   if ($char == "%C3%B3") { return array("%F3", 5); }
   if ($char == "%C3%B4") { return array("%F4", 5); }
   if ($char == "%C3%B5") { return array("%F5", 5); }
   if ($char == "%C3%B6") { return array("%F6", 5); }
   if ($char == "%C3%B7") { return array("%F7", 5); }
   if ($char == "%C3%B8") { return array("%F8", 5); }
   if ($char == "%C3%B9") { return array("%F9", 5); }
   if ($char == "%C3%BA") { return array("%FA", 5); }
   if ($char == "%C3%BB") { return array("%FB", 5); }
   if ($char == "%C3%BC") { return array("%FC", 5); }
   if ($char == "%C3%BD") { return array("%FD", 5); }
   if ($char == "%C3%BE") { return array("%FE", 5); }
   if ($char == "%C3%BF") { return array("%FF", 5); }
   
   $char = substr($str, 0, 3);
   if ($char == "%20") { return array("+", 2); }
   
   $char = substr($str, 0, 1);
   
   if ($char == "!") { return array("%21", 0); }
   if ($char == "\"") { return array("%27", 0); }
   if ($char == "(") { return array("%28", 0); }
   if ($char == ")") { return array("%29", 0); }
   if ($char == "*") { return array("%2A", 0); }
   if ($char == "~") { return array("%7E", 0); }

   if ($char == "%") {
      return array(substr($str, 0, 3), 2);
   } else {
      return array($char, 0);
   }
}

?>
