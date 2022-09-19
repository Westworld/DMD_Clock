Form:C1466.theclip.till:=Form:C1466.frame
If (Form:C1466.theclip.from#0)
	Form:C1466.theclip.frames:=Form:C1466.theclip.till-Form:C1466.theclip.from
	C_PICTURE:C286($pict)
	$old:=Form:C1466.frame
	Form:C1466.frame:=Form:C1466.theclip.from-1
	LoadFrame(->$pict)
	Form:C1466.frame:=$old
	Form:C1466.theclip.pict:=$pict  //$pict
	SET PICTURE TO PASTEBOARD:C521($pict)
	Form:C1466.theclip:=Form:C1466.theclip
End if 