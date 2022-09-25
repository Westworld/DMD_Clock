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
	
	SET BLOB SIZE:C606($bitmap; 0)
	For ($i; 1; Length:C16($chars))
		$char:=$chars[[$i]]
		BuildClockDisplay($char)
		$blob:=ConvertPictToRGB(mypicture)
		//$blob in format 128x32
		$bw:=ConvertRGBtoBW($blob)
		
		$charblob:=RemoveEmptyLeftRightColumns($bw; $data; $glyphs)
		$glyphs[$glyphs.length-1].Index:=BLOB size:C605($bitmap)
		$size:=BLOB size:C605($charblob)
		$destination:=BLOB size:C605($bitmap)
		$source:=0
		COPY BLOB:C558($charblob; $bitmap; $source; $destination; $size)
		
	End for 
	
End if 
$root:=Convert path system to POSIX:C1106(ds:C1482.Settings.all().first().root)
$source:=$root+"/Fonts/"+fontlist{fontlist}+".font"
$source:=Convert path POSIX to system:C1107($source)
C_BLOB:C604($indexblob)
If (Test path name:C476($source)=Is a document:K24:1)
	DELETE DOCUMENT:C159($source)
End if 
$ref:=Create document:C266($source)
// count chars - height
// for each char width (1 byte) and index of bitmap begin (2 byte)
SEND PACKET:C103($ref; Char:C90($glyphs.length)+Char:C90($data.height-$data.ytop-$data.ybottom))
For ($i; 0; $glyphs.length-1)
	$offset:=$glyphs[$i].Index
	$destination:=0
	INTEGER TO BLOB:C548($offset; $indexblob; Macintosh byte ordering:K22:2; $destination)
	SEND PACKET:C103($ref; Char:C90($glyphs[$i].W))
	SEND PACKET:C103($ref; $indexblob)
End for 

SEND PACKET:C103($ref; $bitmap)
CLOSE DOCUMENT:C267($ref)
