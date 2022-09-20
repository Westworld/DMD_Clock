//%attributes = {}


$whichframe:=Form:C1466.frame

$file:=Form:C1466.rgb
$time:=Milliseconds:C459

$frame:=128*32*3
If (Count parameters:C259>0)
	$pixelsize:=2
Else 
	$pixelsize:=6
End if 
$pixeldistance:=0
C_BLOB:C604($input)
$ref:=Open document:C264($file; ""; Read mode:K24:5)
$maxsize:=Get document size:C479($ref)

SET DOCUMENT POSITION:C482($ref; ($whichframe-1)*$frame; 1)

RECEIVE PACKET:C104($ref; $input; $frame)

CLOSE DOCUMENT:C267($ref)
Form:C1466.totalFrames:=($maxsize/$frame)
Form:C1466.pos:=($whichframe/Form:C1466.totalFrames*100)
C_PICTURE:C286(MyPicture)
$framepos:=Form:C1466.frame
If (Count parameters:C259>0)
	$framepos:=1
End if 

If (BLOB size:C605($input)=$frame)
	
	If ($framepos#1)
		
	Else 
		ARRAY TEXT:C222(picturebuffer; 128*32)
		For ($i; 1; 128*32)
			picturebuffer{$i}:=""
		End for 
		
		svgRef:=SVG_New(128*($pixelsize+$pixeldistance)+100; 32*($pixelsize+$pixeldistance)+100; "DMD")
		objectRef:=SVG_New_rect(svgRef; 0; 0; \
			128*($pixelsize+$pixeldistance)+100; 32*($pixelsize+$pixeldistance)+100; -1; -1; "black"; "black"; 0)
	End if 
	
	$pos:=0
	For ($y; 1; 32)
		For ($x; 1; 128)
			// format bgr
			$blue:=$input{$pos+2}
			$green:=$input{$pos+1}
			$red:=$input{$pos}
			$pos+=3
			$color:="rgb("+String:C10($red)+","+String:C10($green)+","+String:C10($blue)+")"
			If ($framepos#1)
				If (picturebuffer{(($y-1)*128+$x)}#$color)
					SVG SET ATTRIBUTE:C1055(myPicture; String:C10($x)+"_"+String:C10($y); "fill"; $color)
					picturebuffer{(($y-1)*128+$x)}:=$color
				End if 
			Else 
				//If (($blue+$green+$red)#0)
				$objectRef:=SVG_New_rect(svgRef; ($x*($pixelsize+$pixeldistance)); ($y*($pixelsize+$pixeldistance)); \
					$pixelsize; $pixelsize; -1; -1; $color; $color; 0)
				SVG_SET_ID($objectRef; String:C10($x)+"_"+String:C10($y))
				picturebuffer{(($y-1)*128+$x)}:=$color
				//End if 
			End if 
		End for 
	End for 
	
	If ($framepos=1)
		$MyPicture:=SVG_Export_to_picture(svgRef; 2)
		If (Count parameters:C259>0)
			$1->:=$MyPicture
			For ($i; 1; 128*32)
				picturebuffer{$i}:=""
			End for 
		Else 
			MyPicture:=$MyPicture
		End if 
	Else 
		If (Count parameters:C259>0)
			$MyPicture:=MyPicture
			//CONVERT PICTURE($MyPicture; ".png")
			$1->:=$MyPicture
			For ($i; 1; 128*32)
				picturebuffer{$i}:=""
			End for 
			
		End if 
	End if 
	
End if 

Form:C1466.frame:=$whichframe+1
$time2:=Milliseconds:C459
$diff:=$time2-$time
If ($diff<30)
	DELAY PROCESS:C323(Current process:C322; 1)
End if 