//%attributes = {}
// Converts blob in RGB to Black/White (1 Pixel)
// for format 128x32 only

C_BLOB:C604($in; $out; $1; $0)
$in:=$1
$size:=BLOB size:C605($in)
SET BLOB SIZE:C606($out; $size/24)
$posin:=0
$posout:=0
$byte:=0
$bit:=7
While ($posin<$size)
	If ($in{$posin}=255)  // for white pixel it is enough to check Red
		$byte:=$byte ?+ $bit
	End if 
	$bit-=1
	If ($bit<0)
		$out{$posout}:=$byte
		$Byte:=0
		$posout+=1
		$bit:=7
	End if 
	$posin+=3
End while 

$0:=$out
