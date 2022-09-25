//%attributes = {}
C_TEXT:C284($1; $text)
C_BLOB:C604($2; $bitmap)
C_COLLECTION:C1488($3; $glyphs)
C_LONGINT:C283($4; $height)

$text:=$1
$bitmap:=$2
$glyphs:=$3
$height:=$4

$posx:=0

$pixelsize:=4
$pixeldistance:=0

svgRef:=SVG_New(128*($pixelsize+$pixeldistance)+100; 32*($pixelsize+$pixeldistance)+100; "DMD")
objectRef:=SVG_New_rect(svgRef; 0; 0; \
128*($pixelsize+$pixeldistance); 32*($pixelsize+$pixeldistance); -1; -1; "black"; "black"; 0)

For ($i; 1; Length:C16($text))
	$char:=$text[[$i]]
	If (($char>="0") & ($char<="9"))
		$charcode:=Character code:C91($char)-48
	Else   // :
		$charcode:=10
	End if 
	
	$width:=$glyphs[$charcode].W
	$offset:=$glyphs[$charcode].offset
	
	$helper:=$width\8
	$width_round:=($helper+1)*8
	$pos:=0
	$color:="white"
	For ($y; 1; $height)
		$bit:=7
		$byte:=$bitmap{$offset+$pos}
		For ($x; 1; $width_round)
			If ($byte ?? $bit)
				$objectRef:=SVG_New_rect(svgRef; $posx+($x*($pixelsize+$pixeldistance)); ($y*($pixelsize+$pixeldistance)); \
					$pixelsize; $pixelsize; -1; -1; $color; $color; 0)
			End if 
			$bit-=1
			If ($bit<0)
				$bit:=7
				If (($pos+$offset+1)<BLOB size:C605($bitmap))
					$pos+=1
					$byte:=$bitmap{$offset+$pos}
				End if 
				
			End if 
		End for 
	End for 
	
	$posx+=(($width+1)*$pixelsize)
End for 

$0:=SVG_Export_to_picture(svgRef; 2)