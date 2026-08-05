extends Control

@onready var Websocket = WebSockets
@onready var UDP = Udp
var Connected = false

#fucking gyroscope math :sob:
var values_vel = ["0.0", "0.0", "0.0","0.0"] #stores gyro speed calculations
var values = [0.0,0.0,0.0,0.0]
var packet := false
var method : bool


func Connect() -> void:
	Connected = Websocket.Connected
	if Connected == true:
		print("server already started, checking for udp discovery status")
		if UDP.is_processing() == true:
			print("UDP Discovery already running, returning...")
		else:
			print("UDP Discovery is OFF, preparing for reconnection")
			UDP.start()
		return
	Websocket.Connect(method)
	UDP.start()


func Disconnect() -> void:
	Connected = Websocket.Connected
	if Connected == false:
		print("No Server running, returning...")
		return
		
	Websocket.Disconnect()
	
func process_packet():
	$VBoxContainer/LOG_LAST.text = Websocket.Last_logged
	$VBoxContainer/Label.text = "THROTTLE (IN MICRO?P): " + str(roundi(Websocket.throttletemp))
	$VBoxContainer/Label2.text = str(Websocket.TGthrust , Websocket.TGroll , Websocket.TGyaw , Websocket.throttle)
	$VBoxContainer/Label3.text = "throttle += input * speed * delta"
func eed() -> void:
	get_tree().change_scene_to_file("res://Scenes/3d_sim.tscn")


func method_(toggled_on: bool) -> void:
	method = toggled_on

func yes(new_text: String) -> void:
	if int(new_text) == null or !new_text.is_valid_int():
		print("no Speed insterted or Incorrect input, defaulting to 10")
		Websocket.speed = 10
		return
	Websocket.speed = int(new_text)


func Restart_ESP() -> void:
	if Websocket.connectable:
		Websocket._peers[Websocket.last_peer_id].send_text("rest")
		UDP.start()


func quit_to_menu() -> void:
	Disconnect()
	get_tree().change_scene_to_file("res://Scenes/Menu.tscn")
