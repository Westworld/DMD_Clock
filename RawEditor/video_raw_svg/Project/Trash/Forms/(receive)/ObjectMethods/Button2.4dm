var $url : Text

// url of the websocket server
$url:="ws://127.0.0.1:8080/"

If (Form:C1466.webSocket=Null:C1517)
	// connection of the websocket client
	Form:C1466.webSocket:=4D:C1709.WebSocket.new($url; cs:C1710.WSConnectionHandler.new())
	
	If (Form:C1466.webSocket#Null:C1517)
		InitEnabledObject(True:C214)
	Else 
		// Error if connection cannot be established
		Form:C1466.messages.push({color: "red"; message: "The websocket server is not started!!"; name: "Error"})
	End if 
End if 

