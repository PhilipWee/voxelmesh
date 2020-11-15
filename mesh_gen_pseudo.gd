tool
extends CollisionShape

export var cube_width=3
# Declare member variables here. Examples:
# var a = 2
# var b = "text"
var point_arr = []

func raycast_get_hits(initial_pos,dir,space_state):
	var result = {'position':initial_pos}
	var cast_forward = true
	var hits = []
	#Do the casting for one
	while !(result.empty() and !cast_forward):
		if cast_forward:
			result = space_state.intersect_ray(result['position'],result['position'] + dir*1e6)
			if result.empty():
				#No more hits in forward direction
				result = {'position':initial_pos + dir*1e6}
				cast_forward = false
				continue
			else:
				hits.append(result)
				continue
		else:
			result = space_state.intersect_ray(result['position'],initial_pos)
			if result.empty():
				#No more hits in backward direction
				break
			else:
				hits.append(result)
				continue
	return hits

func get_point_arr():
	var space_state =  get_world().direct_space_state
	var state_arr = []
	state_arr.resize(pow(cube_width,3))
	
	for i in range(cube_width):
		for j in range(cube_width):
			for k in range(cube_width):
				var hits = raycast_get_hits(Vector3(i,j,k),Vector3.RIGHT,space_state)
				if hits.size() % 2 == 0:
					#Inside mesh
					state_arr[i*cube_width*cube_width + j*cube_width + k] = 0
				else:
					#Outside mesh
					state_arr[i*cube_width*cube_width + j*cube_width + k] = 1
	return state_arr



# Called when the node enters the scene tree for the first time.
func _ready():
	print(get_point_arr())
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var new_point_arr = get_point_arr()
	if new_point_arr != point_arr:
		print(new_point_arr)
		point_arr=new_point_arr
