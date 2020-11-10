#include "voxelmesh.h"
#include "scene/resources/primitive_meshes.h"



//Linear Interpolation function
mpVector LinearInterp(mp4Vector p1, mp4Vector p2, float threshold) {
	mpVector p;
	if (p1.val != p2.val)
		//What is going on here!?!?!??
		p = (mpVector)p1 + ((mpVector)p2 - (mpVector)p1) / (p2.val - p1.val) * (threshold - p1.val);
	else
		p = p1;

	return p;
};

float get_sphere_magnitude(float x, float y, float z) {
	return Math::pow(x, 2) + Math::pow(y, 2) + Math::pow(z, 2);
}

void VoxelMesh::set_sphere_mesh(float magnitude_multiplier) {
	print_line("WHATS GOING ON!?!?!??!");
}

float VoxelMesh::magnitude_at_point(int x, int y, int z) const {
	const int chunk_size = get_chunk_size();
	return scalar_field[x*chunk_size*chunk_size+y*chunk_size+z];
}

void VoxelMesh::set_scalar_field(const Array &p_scalar_field) {
	int expected_arr_size = (int) Math::pow(double(chunk_size+1),3.0);
	ERR_FAIL_COND_MSG(p_scalar_field.size() != expected_arr_size,"The array should the size of the chunk cubed");
	scalar_field = p_scalar_field;
	_request_update();
}

Array VoxelMesh::get_scalar_field() const {
	return scalar_field;
}

void VoxelMesh::set_chunk_size(const int &p_chunk_size) {
	chunk_size = p_chunk_size;
	int new_arr_size = (int) Math::pow(double(p_chunk_size+1),3.0);
	scalar_field.resize(new_arr_size);
	for (int i = 0; i <scalar_field.size(); i++) {
		if ((int) scalar_field[i] == NULL) {
			scalar_field[i] = 0;
		}
	}
	set_scalar_field(scalar_field);
	
	_request_update();
}

int VoxelMesh::get_chunk_size() const {
	return chunk_size;
}

void VoxelMesh::set_cube_width(const float &p_cube_width) {
	cube_width = p_cube_width;
	_request_update();
}

float VoxelMesh::get_cube_width() const {
	return cube_width;
}

void VoxelMesh::_create_mesh_array(Array &p_arr) const {

	PoolVector<Vector3> points;
	PoolVector<Vector3> normals;
	PoolVector<float> tangents; //TODO
	PoolVector<Vector2> uvs; //TODO
	PoolVector<int> indices;
	

#define ADD_TANGENT(m_x, m_y, m_z, m_d) \
	tangents.push_back(m_x);            \
	tangents.push_back(m_y);            \
	tangents.push_back(m_z);            \
	tangents.push_back(m_d);

	//We shall perform the operation in a chunk of chunk_size
	//Minus one otherwise we will go outside the bounding box
	for (int i = 0; i < get_chunk_size(); i++) {
		for (int j = 0; j < get_chunk_size(); j++) {
			for (int k = 0; k < get_chunk_size(); k++) {
				// print_line(String::num_int64(i));
				// print_line(String::num_int64(j));
				// print_line(String::num_int64(k));

				// print_line(String::num_real((double) magnitude_at_point(i,j,k)));
				// print_line(String::num_real((double) magnitude_at_point(i+1,j+1,k+1)));

				// Define a single box
				int cubeIndex = int(0);
				mp4Vector verts[8];

				//Hopefully these bounding box vertices are correct
				verts[0] = mp4Vector(i + 0,j + 0,k + 0, magnitude_at_point(i,j,k));
				verts[1] = mp4Vector(i + 1,j + 0,k + 0, magnitude_at_point(i+1,j,k));
				verts[2] = mp4Vector(i + 1,j + 0,k + 1, magnitude_at_point(i+1,j,k+1));
				verts[3] = mp4Vector(i + 0,j + 0,k + 1, magnitude_at_point(i,j,k+1));
				verts[4] = mp4Vector(i + 0,j + 1,k + 0, magnitude_at_point(i,j+1,k));
				verts[5] = mp4Vector(i + 1,j + 1,k + 0, magnitude_at_point(i+1,j+1,k));
				verts[6] = mp4Vector(i + 1,j + 1,k + 1, magnitude_at_point(i+1,j+1,k+1));
				verts[7] = mp4Vector(i + 0,j + 1,k + 1, magnitude_at_point(i,j+1,k+1));

				for (int n = 0; n < 8; n++) {
					if (verts[n].val <= minValue) {
						cubeIndex |= (1 << n);
					}
				}

				//Check if completely inside or outside the cube
				if (!edgeTable[cubeIndex]) {
					// print_line("WHATS GOING ON!?!?!??!");

				} else {

					//Get linearly interpolated vertices on edges and save into array
					mpVector intVerts[12];

					if (edgeTable[cubeIndex] & 1) intVerts[0] = LinearInterp(verts[0], verts[1], minValue);
					if (edgeTable[cubeIndex] & 2) intVerts[1] = LinearInterp(verts[1], verts[2], minValue);
					if (edgeTable[cubeIndex] & 4) intVerts[2] = LinearInterp(verts[2], verts[3], minValue);
					if (edgeTable[cubeIndex] & 8) intVerts[3] = LinearInterp(verts[3], verts[0], minValue);
					if (edgeTable[cubeIndex] & 16) intVerts[4] = LinearInterp(verts[4], verts[5], minValue);
					if (edgeTable[cubeIndex] & 32) intVerts[5] = LinearInterp(verts[5], verts[6], minValue);
					if (edgeTable[cubeIndex] & 64) intVerts[6] = LinearInterp(verts[6], verts[7], minValue);
					if (edgeTable[cubeIndex] & 128) intVerts[7] = LinearInterp(verts[7], verts[4], minValue);
					if (edgeTable[cubeIndex] & 256) intVerts[8] = LinearInterp(verts[0], verts[4], minValue);
					if (edgeTable[cubeIndex] & 512) intVerts[9] = LinearInterp(verts[1], verts[5], minValue);
					if (edgeTable[cubeIndex] & 1024) intVerts[10] = LinearInterp(verts[2], verts[6], minValue);
					if (edgeTable[cubeIndex] & 2048) intVerts[11] = LinearInterp(verts[3], verts[7], minValue);

					// print_line(String::num_int64(cubeIndex));

					//Build the triangles
					for (int n = 0; triTable[cubeIndex][n] != -1; n += 3) {
						//Get the points of the triangles
						mpVector p0 = intVerts[triTable[cubeIndex][n + 2]];
						mpVector p1 = intVerts[triTable[cubeIndex][n + 1]];
						mpVector p2 = intVerts[triTable[cubeIndex][n]];
						Vector3 vp0 = Vector3(p0.x, p0.y, p0.z);
						Vector3 vp1 = Vector3(p1.x, p1.y, p1.z);
						Vector3 vp2 = Vector3(p2.x, p2.y, p2.z);

						points.push_back(vp0);
						points.push_back(vp1);
						points.push_back(vp2);
						//Calculate normals
						mpVector mp_norm = ((p1 - p0).Cross(p2 - p0)).Normalize();

						for (int norm_counter = 0; norm_counter < 3; norm_counter++) {
							Vector3 norm = -Vector3(mp_norm.x, mp_norm.y, mp_norm.z);
							// print_line((String)norm);
							normals.push_back(norm);
						}
					}
				};
			}
		}
	}
	p_arr[VS::ARRAY_VERTEX] = points;
	p_arr[VS::ARRAY_NORMAL] = normals;

	// p_arr[VS::ARRAY_TANGENT] = tangents;
	// p_arr[VS::ARRAY_TEX_UV] = uvs;
	// p_arr[VS::ARRAY_INDEX] = indices;
};

void VoxelMesh::_bind_methods(){
	//Remember to bind methods for callbacks here!
	
	ClassDB::bind_method(D_METHOD("set_sphere_mesh", "magnitude_multiplier"), &VoxelMesh::set_sphere_mesh);

	ClassDB::bind_method(D_METHOD("set_scalar_field", "p_scalar_field"), &VoxelMesh::set_scalar_field);
	ClassDB::bind_method(D_METHOD("get_scalar_field"), &VoxelMesh::get_scalar_field);

	ClassDB::bind_method(D_METHOD("set_chunk_size", "p_chunk_size"), &VoxelMesh::set_chunk_size);
	ClassDB::bind_method(D_METHOD("get_chunk_size"), &VoxelMesh::get_chunk_size);

	ClassDB::bind_method(D_METHOD("set_cube_width", "p_cube_width"), &VoxelMesh::set_cube_width);
	ClassDB::bind_method(D_METHOD("get_cube_width"), &VoxelMesh::get_cube_width);

	ADD_PROPERTY( PropertyInfo(Variant::ARRAY,"scalar_field"),"set_scalar_field","get_scalar_field");
	ADD_PROPERTY( PropertyInfo(Variant::INT,"chunk_size"),"set_chunk_size","get_chunk_size");
	ADD_PROPERTY( PropertyInfo(Variant::REAL,"cube_width", PROPERTY_HINT_RANGE, "0.001,100.0,0.001,or_greater"),"set_cube_width","get_cube_width");
};

VoxelMesh::VoxelMesh(){
	
	//Instantiate defaults
	minValue = 14;
	scalar_field = Array();
	chunk_size = 0;
	cube_width = 1;
};