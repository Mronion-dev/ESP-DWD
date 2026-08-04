extends Node3D

var Rotation : Vector3
@onready var Websocket = WebSockets
var Connected = false

#fucking gyroscope math :sob:
var values_vel = ["0.0", "0.0", "0.0","0.0"] #stores gyro speed calculations
var values = [0.0,0.0,0.0,0.0]
var packet := false

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	if Websocket.Last_logged.split(" ").size() < 2:
		return
	var quat = Websocket.Last_logged.split(" ")
	$"aaaa/Tinkercad GLTF Scene".quaternion = Quaternion(quat[1].to_float(), quat[2].to_float(), quat[3].to_float(), quat[0].to_float())
	if Input.is_action_pressed("ui_cancel"):
		get_tree().change_scene_to_file("res://Scenes/Main.tscn")
	pass
	
	
	
	
	
	
