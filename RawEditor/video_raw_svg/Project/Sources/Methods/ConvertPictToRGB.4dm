//%attributes = {}
// converts an image (SVG or PNG) to RGB
C_PICTURE:C286($1)
C_BLOB:C604($0)

$pict:=$1

//WRITE PICTURE FILE(System folder(Desktop)+"convert.pbm"; $pict; ".pbm")
//$text:=Document to text(System folder(Desktop)+"convert.pbm"; "UTF-8"; Document with CR)

C_BLOB:C604($blobin; $blob)
PICTURE TO BLOB:C692($pict; $blobin; ".pbm")
$text:=Convert to text:C1012($blobin; "UTF-8")
$text:=Replace string:C233($text; Char:C90(13)+Char:C90(10); Char:C90(13))
$text:=Replace string:C233($text; Char:C90(10); Char:C90(13))

$lines:=Split string:C1554($text; Char:C90(13))
If ($lines.length<10)
	ALERT:C41("wrong format")
Else 
	If (($lines[0]#"P3") || ($lines[2]#"128 32") || ($lines[3]#"255"))
		ALERT:C41("wrong format")
	Else 
		// we can start
		// convert to RAW image
		$lines.remove(0; 4)
		$text:=$lines.join(Char:C90(13))
		$text:=Replace string:C233($text; Char:C90(13); " ")
		$text:=Replace string:C233($text; "    "; " ")
		$text:=Replace string:C233($text; "   "; " ")
		$text:=Replace string:C233($text; "  "; " ")
		$bytes:=Split string:C1554($text; " ")
		If ($bytes[0]="")
			$bytes.remove(0; 1)
		End if 
		If ($bytes[$bytes.length-1]="")
			$bytes.remove($bytes.length-1; 1)
		End if 
		
		$size:=128*32*3
		If ($bytes.length#$size)
			ALERT:C41("wrong size?")
		Else 
			SET BLOB SIZE:C606($blob; $bytes.length)
			For ($i; 0; $size-1)
				$blob{$i}:=Num:C11($bytes[$i])
			End for 
			
		End if 
	End if 
End if 

$0:=$blob
