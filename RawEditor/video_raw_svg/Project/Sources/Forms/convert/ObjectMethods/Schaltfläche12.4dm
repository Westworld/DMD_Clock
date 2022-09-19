$name:=Request:C163("clip name"; Form:C1466.game.avi_name+"_")
If (OK=1)
	Form:C1466.clips.push(New object:C1471("name"; $name; "from"; Form:C1466.frame; "till"; 0; "frames"; 0; "pict"; Null:C1517))
	Form:C1466.clips:=Form:C1466.clips
	LISTBOX SELECT ROW:C912(*; "List box"; Form:C1466.clips.length)
End if 