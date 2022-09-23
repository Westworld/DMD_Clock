//%attributes = {}
C_BLOB:C604($1; $bw)
C_OBJECT:C1216($2; $data)
C_COLLECTION:C1488($3; $glyphs)

$bw:=$1
$data:=$2
$glyphs:=$3

$emptyleft:=0
$emptyright:=0

$pos:=0
$bit:=7

// we get a 128x32 image, most will be the empty
// to ease and speed up testing, we transfer to boolean 2d array

ARRAY BOOLEAN:C223($pixels; $data.width; $data.height)
For ($y; 1; $data.height)
	For ($x; 1; $data.width)
		$pixels{$x}{$y}:=$bw{$pos} ?? $bit
		$bit-=1
		If ($bit<0)
			$bit:=7
			$pos:=$pos+1
		End if 
	End for 
End for 

For ($x; 1; $data.width)
	For ($y; 1; $data.height)
		If ($pixels{$x}{$y})
			break
		End if 
	End for 
	$emptyleft+=1
End for 
If ($emptyleft>=$data.width)
	ALERT:C41("totally empty picture?")
End if 
For ($x; $data.width; 1; -1)
	For ($y; 1; $data.height)
		If ($pixels{$x}{$y})
			break
		End if 
	End for 
	$emptyright+=1
End for 

// let's create a new blob, based on Emptyleft/right and $data.ytop and ybottom
