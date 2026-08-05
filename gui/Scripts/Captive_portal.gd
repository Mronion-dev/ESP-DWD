extends Control

func _on_connect_pressed() -> void:
	var value1 = $CenterContainer/LineEdit.text
	var value2 = $CenterContainer2/LineEdit.text
	
	var url = "http://192.168.4.1/send?str1=" + value1.uri_encode() + "&str2=" + value2.uri_encode()
	
	$HTTPRequest.request(url)
