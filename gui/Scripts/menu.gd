extends Control

func go_to_networking() -> void:
	get_tree().change_scene_to_file("res://Scenes/Main.tscn")


func go_to_captive_portal() -> void:
	get_tree().change_scene_to_file("res://Scenes/captive_portal.tscn")
