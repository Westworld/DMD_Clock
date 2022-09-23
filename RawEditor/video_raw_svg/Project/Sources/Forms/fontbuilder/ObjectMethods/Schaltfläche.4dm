If (Picture size:C356(mypicture)>0)
	$blob:=ConvertPictToRGB(mypicture)
	If (BLOB size:C605($blob)>0)
		$pixelsize:=5
		$pixeldistance:=1
		
		svgRef:=SVG_New(128*($pixelsize+$pixeldistance); 32*($pixelsize+$pixeldistance); "DMD")
		objectRef:=SVG_New_rect(svgRef; 0; 0; \
			128*($pixelsize+$pixeldistance)+100; 32*($pixelsize+$pixeldistance)+100; -1; -1; "black"; "black"; 0)
		
		$pos:=0
		For ($y; 1; 32)
			For ($x; 1; 128)
				// format bgr
				$blue:=$blob{$pos+2}
				$green:=$blob{$pos+1}
				$red:=$blob{$pos}
				$pos+=3
				$color:="rgb("+String:C10($red)+","+String:C10($green)+","+String:C10($blue)+")"
				$objectRef:=SVG_New_rect(svgRef; ($x*($pixelsize+$pixeldistance)); ($y*($pixelsize+$pixeldistance)); \
					$pixelsize; $pixelsize; -1; -1; $color; $color; 0)
			End for 
		End for 
		conv_picture:=SVG_Export_to_picture(svgRef; 2)
	End if 
End if 
