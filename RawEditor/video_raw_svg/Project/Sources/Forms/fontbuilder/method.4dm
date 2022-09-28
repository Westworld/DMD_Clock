Case of 
	: (FORM Event:C1606.code=On Load:K2:1)
		C_PICTURE:C286($pict)
		mypicture:=$pict
		picturepreview:=$pict
		
		ARRAY TEXT:C222(fontlist; 0)
		FONT LIST:C460(fontlist)
		
		Form:C1466.time:="12:34:56"
		Form:C1466.offset:=-1
		
		
	: (FORM Event:C1606.code=On Timer:K2:25)
		Case of 
			: (Form:C1466.timer.job="DrawClockTopDown")
				$y:=Form:C1466.timer.cur
				$max:=Form:C1466.timer.max
				
				DrawClockTopDown(Form:C1466.time; Form:C1466.timer.bitmap; Form:C1466.timer.glyphs; Form:C1466.timer.height; $y)
				
				$y+=1
				If ($y>$max)
					//SET TIMER(0)
					$y:=1
				End if 
				Form:C1466.timer.cur:=$y
				
			: (Form:C1466.timer.job="Blink")
				DrawClockBlink(Form:C1466.time; Form:C1466.timer.bitmap; Form:C1466.timer.glyphs; Form:C1466.timer.height)
				
				
			Else 
				// nothing
		End case 
		