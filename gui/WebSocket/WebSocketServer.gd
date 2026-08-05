extends Node

# The port we will listen to.
const PORT = 54732

# Our TCP Server instance.
var _tcp_server = TCPServer.new()
var err
var Connected : bool
var packet_recieved := false
var target = [0.0,0.0,0.0,0.0]
var TGyaw = target[2]
var TGpitch = target[3]
var TGroll = target[0]
var TGthrust = target[1]
var connectable : bool
var throttle = 0
var throttletemp = 0
var method : bool
var speed = 0

# Our connected peers list.
var _peers: Dictionary[int, WebSocketPeer] = {}

var last_peer_id := 1
#oru last message recieved
var Last_logged : String
func _ready() -> void:
	set_process(false)

func Connect(InputMethod : bool):
	# Start listening on the given port.
	Connected = false
	err = _tcp_server.listen(PORT)
	if err == OK:
		print("Server started.")
		Connected = true
		method = InputMethod
		set_process(true)
	else:
		push_error("Unable to start server.")
		Connected = false
		set_process(false)

func Disconnect():
	if !connectable:
		return
	_peers[last_peer_id].send_text("closing")
	_tcp_server.stop()
	set_process(false)
	Connected = false

func _process(delta):
	Inputs()
	TGyaw = target[2] + 1
	TGpitch = target[3] + 1
	TGroll = target[0] + 1
	TGthrust = target[1] + 1
		
	#where TG means target
	
	throttletemp += target[3] * speed * delta * -1
	throttle = roundi(throttletemp)
	if throttletemp < 1200:
		throttle = 1000
	if throttletemp < 1000:
		throttletemp = 1000
	if throttletemp > 2000:
		throttle = 2000
		throttletemp = 2000
	if packet_recieved:
		packet_recieved = false
	while _tcp_server.is_connection_available():
		last_peer_id += 1
		print("+ Peer %d connected." % last_peer_id)
		var ws = WebSocketPeer.new()
		ws.accept_stream(_tcp_server.take_connection())
		_peers[last_peer_id] = ws

	# Iterate over all connected peers using "keys()" so we can erase in the loop
	for peer_id in _peers.keys():
		var peer = _peers[peer_id]

		peer.poll()

		var peer_state = peer.get_ready_state()
		if peer_state == WebSocketPeer.STATE_OPEN:
			connectable = true
			if $Timer.is_stopped() == true:
				$Timer.start(0.05)
			while peer.get_available_packet_count():
				var packet = peer.get_packet()
				if peer.was_string_packet():
					var packet_text = packet.get_string_from_utf8()
					Last_logged = packet_text
					packet_recieved = true
		elif peer_state == WebSocketPeer.STATE_CLOSED:
			# Remove the disconnected peer.
			connectable = false
			last_peer_id -= 1
			_peers.erase(peer_id)
			var code = peer.get_close_code()
			var reason = peer.get_close_reason()
			print("- Peer %s closed with code: %d, reason %s. Clean: %s" % [peer_id, code, reason, code != -1])


func Send_Rate() -> void:
	if !connectable:
		return
	_peers[last_peer_id].send_text(str(TGthrust, TGroll, TGyaw, throttle))

func Inputs():
	if method == true: #easy part: simply use raw controller inputs
			target = [
				roundf(Input.get_joy_axis(0, JOY_AXIS_LEFT_X) * 10) / 10,
				roundf(Input.get_joy_axis(0, JOY_AXIS_LEFT_Y) * 10) / 10,
				roundf(Input.get_joy_axis(0, JOY_AXIS_RIGHT_X) * 10) / 10,
				roundf(Input.get_joy_axis(0, JOY_AXIS_RIGHT_Y) * 10) / 10
			]
	else: #this is a lot easier than i thoughts
		var steer = Input.get_axis("S", "W")
		var thrust = Input.get_axis("I", "K")
		target[1] = steer
		target[3] = thrust

func Send_Custom(text : String):
	if connectable:
		_peers[last_peer_id].send_text(text)
