If (FORM Event:C1606.code=On Clicked:K2:4)
	$newpos:=Trunc:C95(Form:C1466.pos/100*Form:C1466.totalFrames; 0)
	Form:C1466.frame:=$newpos
	LoadFrame
End if 