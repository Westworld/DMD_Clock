//%attributes = {}

ARRAY TEXT:C222($CodecArray; 0)
PICTURE CODEC LIST:C992($CodecArray; $ArrayName)
If (Find in array:C230($CodecArray; ".pbm")<0)
	ALERT:C41("pbm export codec missing")
Else 
	
	
	If (Count parameters:C259>0)
		$text:=$1
	Else 
		$text:=Form:C1466.time
	End if 
	$pixelsize:=1
	$pixeldistance:=0
	
	C_PICTURE:C286(MyPicture)
	
	svgRef:=SVG_New(128*($pixelsize+$pixeldistance); 32*($pixelsize+$pixeldistance); "DMD")
	objectRef:=SVG_New_rect(svgRef; 0; 0; \
		128*($pixelsize+$pixeldistance); 32*($pixelsize+$pixeldistance); -1; -1; "black"; "black"; 0)
	
	If (fontlist=0)
		$font:="Helvetica"
	Else 
		$font:=fontlist{fontlist}
	End if 
	
	$bold:=Num:C11(fontstyle)
	$textID:=SVG_New_text(svgRef; $text; 1; Form:C1466.offset; $font; Num:C11(fontsize); $bold; Align left:K42:2; "white")
	SVG_SET_TEXT_RENDERING($textID; "optimizeSpeed")
	
	MyPicture:=SVG_Export_to_picture(svgRef; 2)
	Picturepreview:=MyPicture
End if 
//
CONVERT PICTURE:C1002(Picturepreview; ".png")
TRANSFORM PICTURE:C988(Picturepreview; Scale:K61:2; 3; 3)

