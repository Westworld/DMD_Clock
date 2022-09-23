//%attributes = {}
// display BW frame, pass as blob
$input:=$1
C_BLOB:C604($1; $input)
C_PICTURE:C286($0)

// x / y as parameter to be flexible


$pixelsize:=4
$pixeldistance:=0

//If (BLOB size($input)=(128*32/8))
svgRef:=SVG_New(128*($pixelsize+$pixeldistance)+100; 32*($pixelsize+$pixeldistance)+100; "DMD")
objectRef:=SVG_New_rect(svgRef; 0; 0; \
128*($pixelsize+$pixeldistance); 32*($pixelsize+$pixeldistance); -1; -1; "black"; "black"; 0)


$pos:=0
$bit:=7
For ($y; 1; 32)
	For ($x; 1; 128)
		$byte:=$input{$pos}
		$pixel:=$byte ?? $bit
		$bit-=1
		If ($bit<0)
			$pos+=1
			$bit:=7
		End if 
		
		If ($pixel)
			$color:="white"
		Else 
			$color:="black"
		End if 
		
		$objectRef:=SVG_New_rect(svgRef; ($x*($pixelsize+$pixeldistance)); ($y*($pixelsize+$pixeldistance)); \
			$pixelsize; $pixelsize; -1; -1; $color; $color; 0)
	End for 
End for 
//End if 

$0:=SVG_Export_to_picture(svgRef; 2)

