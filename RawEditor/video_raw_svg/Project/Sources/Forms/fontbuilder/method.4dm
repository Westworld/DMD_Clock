If (FORM Event:C1606.code=On Load:K2:1)
	C_PICTURE:C286($pict)
	mypicture:=$pict
	picturepreview:=$pict
	
	ARRAY TEXT:C222(fontlist; 0)
	FONT LIST:C460(fontlist)
	
	Form:C1466.time:="12:34:56"
	Form:C1466.offset:=-1
End if 
