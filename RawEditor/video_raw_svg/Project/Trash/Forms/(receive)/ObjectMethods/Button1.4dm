If (Form:C1466.message#"")
	
	// Sends a message through the websocket client connected to the websocket server
	Form:C1466.webSocket.send({message: Form:C1466.message})
	
	Form:C1466.message:=""
	
End if 
