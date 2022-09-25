//%attributes = {}
C_BLOB:C604($1; $bw; $0; $bw_new)
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
	If ($y<$data.height)
		break
	End if 
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
	If ($y<$data.height)
		break
	End if 
	$emptyright+=1
End for 

// let's create a new blob, based on Emptyleft/right and $data.ytop and ybottom
$width:=$data.width-$emptyleft-$emptyright
$height:=$data.height-$data.ytop-$data.ybottom
// width needs to be multiple of 8
$helper:=$width\8
$width_round:=($helper+1)*8

C_BLOB:C604($bw_new)
SET BLOB SIZE:C606($bw_new; ($width_round*$height/8))
$posout:=0
$bit:=7
$byte:=0
For ($y; 1; $height)
	For ($x; 1; $width)
		If ($pixels{$emptyleft+$x}{$data.ytop+$y})
			$byte:=$byte ?+ $bit
		End if 
		$bit-=1
		If ($bit<0)
			$bw_new{$posout}:=$byte
			$bit:=7
			$byte:=0
			$posout+=1
		End if 
	End for 
	$bw_new{$posout}:=$byte
	$bit:=7
	$byte:=0
	$posout+=1
End for 

// add to glyphs
//Index,  W, H,xAdv,dX, dY
// char, size,
$glyphs.push(New object:C1471("W"; $width; "H"; $height))

$0:=$bw_new
