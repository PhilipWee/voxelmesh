#include "voxelmesh.h"
#include "scene/resources/primitive_meshes.h"

VoxelMesh::VoxelMesh() {
	// defaults
	size = Size2(2.0, 2.0);
	subdivide_w = 0;
	subdivide_d = 0;
};

//Linear Interpolation function
mpVector LinearInterp(mp4Vector p1, mp4Vector p2, float threshold) {
	mpVector p;
	if (p1.val != p2.val)
		//What is going on here!?!?!??
		p = (mpVector)p1 + ((mpVector)p2 - (mpVector)p1) / (p2.val - p1.val) * (threshold - p1.val);
	else
		p = (mpVector)p1;
	
	return p;
}

void VoxelMesh::_create_mesh_array(Array &p_arr) const {
	//This is temporarily a plane mesh
	// int i, j, prevrow, thisrow, point;
	// float x, z;

	Size2 start_pos = size * -0.5;

	PoolVector<Vector3> points;
	PoolVector<Vector3> normals;
	PoolVector<float> tangents; //TODO
	PoolVector<Vector2> uvs; //TODO
	PoolVector<int> indices;
	// point = 0;

#define ADD_TANGENT(m_x, m_y, m_z, m_d) \
	tangents.push_back(m_x);            \
	tangents.push_back(m_y);            \
	tangents.push_back(m_z);            \
	tangents.push_back(m_d);

	//Define a single box
	int cubeIndex = int(0);
	mp4Vector verts[8];

	//Hopefully these bounding box vertices are correct
	verts[0] = mp4Vector(0, 0, 0, 1);
	verts[1] = mp4Vector(1, 0, 0, 1);
	verts[2] = mp4Vector(1, 1, 0, 1);
	verts[3] = mp4Vector(0, 1, 0, 2);
	verts[4] = mp4Vector(0, 0, 1, 3);
	verts[5] = mp4Vector(1, 0, 1, 0);
	verts[6] = mp4Vector(1, 1, 1, 0);
	verts[7] = mp4Vector(0, 1, 1, 0);

	for (int n = 0; n < 8; n++) {
		if (verts[n].val == 0) {
			cubeIndex |= (1 << n);
		}
	}

	print_line(String::num_int64(cubeIndex));

	//Check if completely inside or outside the cube
	if (!edgeTable[cubeIndex]) {
		print_line("WHATS GOING ON!?!?!??!");

	} else {

		//Get linearly interpolated vertices on edges and save into array
		mpVector intVerts[12];

		int minValue = 0.5;

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

		//Build the triangles
		for (int n = 0; triTable[cubeIndex][n] != -1; n += 3) {
			//Get the points of the triangles
			mpVector p0 = intVerts[triTable[cubeIndex][n+2]];
			mpVector p1 = intVerts[triTable[cubeIndex][n+1]];
			mpVector p2 = intVerts[triTable[cubeIndex][n]];
			points.push_back(Vector3(p0.x, p0.y, p0.z));
			points.push_back(Vector3(p1.x, p1.y, p1.z));
			points.push_back(Vector3(p2.x, p2.y, p2.z));
			//Calculate normals
			mpVector mp_norm = ((p1-p0).Cross(p2-p0)).Normalize();
			Vector3 norm = Vector3(mp_norm.x, mp_norm.y, mp_norm.z);
			// normals.push_back(Vector3(norm.x, norm.y, norm.z));
		}

	};

	/*Making a traingle*/
	points.push_back(Vector3(-1,-1,1));
	points.push_back(Vector3(-1,1,0));
	points.push_back(Vector3(1,1,0));

	points.push_back(Vector3(-1,-1,-1));
	points.push_back(Vector3(-1,1,0));
	points.push_back(Vector3(1,1,0));

	/* top + bottom */
	// z = start_pos.y;
	// thisrow = point;
	// prevrow = 0;
	// for (j = 0; j <= 1; j++) {
	// 	x = start_pos.x;
	// 	for (i = 0; i <= 1; i++) {
	// 		float u = i;
	// 		float v = j;
	// 		u /= (subdivide_w + 1.0);
	// 		v /= (subdivide_d + 1.0);

	// 		points.push_back(Vector3(-x, 0.0, -z));
	// 		normals.push_back(Vector3(0.0, 1.0, 0.0));
	// 		ADD_TANGENT(1.0, 0.0, 0.0, 1.0);
	// 		uvs.push_back(Vector2(1.0 - u, 1.0 - v)); /* 1.0 - uv to match orientation with Quad */
	// 		point++;

	// 		if (i > 0 && j > 0) {
	// 			indices.push_back(prevrow + i - 1);
	// 			indices.push_back(prevrow + i);
	// 			indices.push_back(thisrow + i - 1);
	// 			indices.push_back(prevrow + i);
	// 			indices.push_back(thisrow + i);
	// 			indices.push_back(thisrow + i - 1);
	// 		};

	// 		x += size.x;
	// 	};

	// 	z += size.y;
	// 	prevrow = thisrow;
	// 	thisrow = point;
	// };

	p_arr[VS::ARRAY_VERTEX] = points;
	// p_arr[VS::ARRAY_NORMAL] = normals;
	// p_arr[VS::ARRAY_TANGENT] = tangents;
	// p_arr[VS::ARRAY_TEX_UV] = uvs;
	// p_arr[VS::ARRAY_INDEX] = indices;
};

void VoxelMesh::_bind_methods(){
	//Remember to bind methods for callbacks here!
};

VoxelMeshInstance::VoxelMeshInstance() {

	//Set the mesh to a cube for testing
	Ref<VoxelMesh> voxel_mesh;
	voxel_mesh.instance();
	set_mesh(voxel_mesh);
};

void VoxelMeshInstance::_bind_methods(){
	//Remember to bind methods for callbacks here!
};
