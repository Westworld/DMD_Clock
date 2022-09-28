
$root:=Convert path system to POSIX:C1106(ds:C1482.Settings.all().first().root)
$source:=$root+"/Fonts/Arial Black.font"
$source:=Convert path POSIX to system:C1107($source)


$ref:=Open document:C264($source; ""; Read mode:K24:5)
$count:=0
C_BLOB:C604($byte)
SET BLOB SIZE:C606($byte; 0)
RECEIVE PACKET:C104($ref; $byte; 1)
$count:=$byte{0}
If ((OK=1) && ($count#0))
	$height:=0
	SET BLOB SIZE:C606($byte; 0)
	RECEIVE PACKET:C104($ref; $byte; 1)
	$height:=$byte{0}
	$glyphs:=New collection:C1472
	$width:=0
	SET BLOB SIZE:C606($offset_blob; 0)
	For ($i; 1; $count)
		SET BLOB SIZE:C606($byte; 0)
		RECEIVE PACKET:C104($ref; $byte; 1)
		$width:=$byte{0}
		RECEIVE PACKET:C104($ref; $offset_blob; 2)
		$offset:=BLOB to integer:C549($offset_blob; Macintosh byte ordering:K22:2)
		$glyphs.push(New object:C1471("W"; $width; "offset"; $offset))
	End for 
	C_BLOB:C604($bitmap)
	RECEIVE PACKET:C104($ref; $bitmap; 50000)
Else 
	ALERT:C41("invalid document?")
End if 
CLOSE DOCUMENT:C267($ref)

// now draw in animation. we need to outsource to a timer event
Form:C1466.timer:=New object:C1471("job"; "Blink"; "bitmap"; $bitmap; "glyphs"; $glyphs; "height"; $height)
SET TIMER:C645(1)


