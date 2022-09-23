If (Picture size:C356(mypicture)>0)
	
/*
for 0..9 and : 
char for char
create bitmap, remove left/right empty columns
calc max width, max 16, else error
code char
add to blob
// font format following adafruit fonts
https://learn.adafruit.com/creating-custom-symbol-font-for-adafruit-gfx-library/understanding-the-font-specification
	
*/
	
	C_BLOB:C604($bitmap)
	C_COLLECTION:C1488($glyphs)
	$glyphs:=New collection:C1472
	
	$chars:="0123456789:"
	BuildClockDisplay($chars)
	$blob:=ConvertPictToRGB(mypicture)
	// find empty lines and remove then.
	$data:=New object:C1471("width"; 128; "height"; 32; "ytop"; 0; "ybottom"; 0)
	$bw:=ConvertRGBtoBW($blob)
	IdentifyEmpyTopBottomLines($bw; $data)
	
	
	For ($i; 1; Length:C16($chars))
		$char:=$chars[[$i]]
		BuildClockDisplay($char)
		$blob:=ConvertPictToRGB(mypicture)
		//$blob in format 128x32
		$bw:=ConvertRGBtoBW($blob)
		
		RemoveEmptyLeftRightColumns($bw; $data; $glyphs)
		
		$pict:=LoadFrameBW($bw)
		conv_picture:=$pict
		break
	End for 
	
	
End if 
