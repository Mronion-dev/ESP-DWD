extends Node

const DISCOVERY_PORT := 42100

var udp := PacketPeerUDP.new()

# Runs when the script is initialized.
func _ready() -> void:
	set_process(false)

func start():
	var err = udp.bind(DISCOVERY_PORT)
	if err != OK:
		push_error("Couldn't bind discovery socket")
		return
	set_process(true)


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(_delta):
	while udp.get_available_packet_count() > 0:
	
		var packet := udp.get_packet().get_string_from_utf8()
		
		if packet == "FIND_MY_SERVER":
			var ip := udp.get_packet_ip()
			 
			udp.set_dest_address(ip, DISCOVERY_PORT)
			udp.put_packet("IM_HERE".to_utf8_buffer())
			udp.close()
			set_process(false)
