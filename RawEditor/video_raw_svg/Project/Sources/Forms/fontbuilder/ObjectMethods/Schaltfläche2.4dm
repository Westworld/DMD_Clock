$ref:=Open document:C264(""; ".font"; Get pathname:K24:6)
If (OK=1)
	$ref:=Open document:C264(document; ""; Read mode:K24:5)
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
End if 

// now draw the 11 chars - with 1 pixel distance
conv_Picture:=DrawClock("0123456789:"; $bitmap; $glyphs; $height)

