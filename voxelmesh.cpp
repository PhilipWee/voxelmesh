#include "voxelmesh.h"
#include "scene/resources/primitive_meshes.h"

VoxelMesh::VoxelMesh() {
    // defaults
	size = Size2(2.0, 2.0);
	subdivide_w = 0;
	subdivide_d = 0;
};

void VoxelMesh::_create_mesh_array(Array &p_arr) const {
    //This is temporarily a plane mesh
	int i, j, prevrow, thisrow, point;
	float x, z;

	Size2 start_pos = size * -0.5;

	PoolVector<Vector3> points;
	PoolVector<Vector3> normals;
	PoolVector<float> tangents;
	PoolVector<Vector2> uvs;
	PoolVector<int> indices;
	point = 0;

#define ADD_TANGENT(m_x, m_y, m_z, m_d) \
	tangents.push_back(m_x);            \
	tangents.push_back(m_y);            \
	tangents.push_back(m_z);            \
	tangents.push_back(m_d);

	/* top + bottom */
	z = start_pos.y;
	thisrow = point;
	prevrow = 0;
	for (j = 0; j <= (subdivide_d + 1); j++) {
		x = start_pos.x;
		for (i = 0; i <= (subdivide_w + 1); i++) {
			float u = i;
			float v = j;
			u /= (subdivide_w + 1.0);
			v /= (subdivide_d + 1.0);

			points.push_back(Vector3(-x, 0.0, -z));
			normals.push_back(Vector3(0.0, 1.0, 0.0));
			ADD_TANGENT(1.0, 0.0, 0.0, 1.0);
			uvs.push_back(Vector2(1.0 - u, 1.0 - v)); /* 1.0 - uv to match orientation with Quad */
			point++;

			if (i > 0 && j > 0) {
				indices.push_back(prevrow + i - 1);
				indices.push_back(prevrow + i);
				indices.push_back(thisrow + i - 1);
				indices.push_back(prevrow + i);
				indices.push_back(thisrow + i);
				indices.push_back(thisrow + i - 1);
			};

			x += size.x / (subdivide_w + 1.0);
		};

		z += size.y / (subdivide_d + 1.0);
		prevrow = thisrow;
		thisrow = point;
	};

	p_arr[VS::ARRAY_VERTEX] = points;
	p_arr[VS::ARRAY_NORMAL] = normals;
	p_arr[VS::ARRAY_TANGENT] = tangents;
	p_arr[VS::ARRAY_TEX_UV] = uvs;
	p_arr[VS::ARRAY_INDEX] = indices;
};

void VoxelMesh::_bind_methods() {
	//Remember to bind methods for callbacks here!
};

VoxelMeshInstance::VoxelMeshInstance() {

	//Set the mesh to a cube for testing
	Ref<VoxelMesh> voxel_mesh;
	voxel_mesh.instance();
	set_mesh(voxel_mesh);
};

void VoxelMeshInstance::_bind_methods() {
	//Remember to bind methods for callbacks here!
};
