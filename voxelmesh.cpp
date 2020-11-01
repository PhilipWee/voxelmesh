#include "voxelmesh.h"
#include "scene/resources/primitive_meshes.h"

VoxelMesh::VoxelMesh(){
	minValue = 14;
};

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

float MagnitudeAtPoint(float x, float y, float z) {
	return Math::pow(x, 2) + Math::pow(y, 2) + Math::pow(z, 2);
}

void VoxelMesh::set_size(const float &p_size) {
	minValue = p_size;
	_request_update();
}

float VoxelMesh::get_size() const {
	return minValue;
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

	//We shall perform the operation in a 16x16x16 chunk
	for (int i = -8; i < 8; i++) {
		for (int j = -8; j < 8; j++) {
			for (int k = -8; k < 8; k++) {
				//Define a single box
				int cubeIndex = int(0);
				mp4Vector verts[8];

				//Hopefully these bounding box vertices are correct
				verts[0] = mp4Vector(i + 0,j + 0,k + 0, MagnitudeAtPoint(i,j,k));
				verts[1] = mp4Vector(i + 1,j + 0,k + 0, MagnitudeAtPoint(i+1,j,k));
				verts[2] = mp4Vector(i + 1,j + 0,k + 1, MagnitudeAtPoint(i+1,j,k+1));
				verts[3] = mp4Vector(i + 0,j + 0,k + 1, MagnitudeAtPoint(i,j,k+1));
				verts[4] = mp4Vector(i + 0,j + 1,k + 0, MagnitudeAtPoint(i,j+1,k));
				verts[5] = mp4Vector(i + 1,j + 1,k + 0, MagnitudeAtPoint(i+1,j+1,k));
				verts[6] = mp4Vector(i + 1,j + 1,k + 1, MagnitudeAtPoint(i+1,j+1,k+1));
				verts[7] = mp4Vector(i + 0,j + 1,k + 1, MagnitudeAtPoint(i,j+1,k+1));

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
	ClassDB::bind_method(D_METHOD("set_size", "size"), &VoxelMesh::set_size);
	ClassDB::bind_method(D_METHOD("get_size"), &VoxelMesh::get_size);

	ADD_PROPERTY( PropertyInfo(Variant::REAL,"size"),"set_size","get_size");
};

VoxelMeshInstance::VoxelMeshInstance() {

	//Set the mesh to a cube for testing
	Ref<VoxelMesh> voxel_mesh;
	voxel_mesh.instance();
	set_mesh(voxel_mesh);
	// Ref<CustomPlaneMesh> plane_mesh;
	// plane_mesh.instance();
	// set_mesh(plane_mesh);
};

void VoxelMeshInstance::_bind_methods(){
	//Remember to bind methods for callbacks here!
};
