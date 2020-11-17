/*voxelmesh.h*/

#ifndef VOXELMESH_H
#define VOXELMESH_H

#include "MCTable.h"
#include "core/reference.h"
#include "mpVector.h"
#include "scene/3d/mesh_instance.h"
#include "scene/resources/mesh.h"
#include "scene/resources/primitive_meshes.h"

class VoxelMesh : public PrimitiveMesh {
	GDCLASS(VoxelMesh, PrimitiveMesh);

private:
	Array scalar_field;
	float cube_width;
	int chunk_size;

protected:
	float threshold;
	static void _bind_methods();

	virtual void _create_mesh_array(Array &p_arr) const;

public:
	void set_scalar_field(const Array &p_scalar_field);
	Array get_scalar_field() const;

	int get_chunk_size() const;

	void set_cube_width(const float &p_cube_width);
	float get_cube_width() const;

	void set_sphere_mesh(const float magnitude_multiplier);
	float magnitude_at_point(int x, int y, int z) const;

	void set_threshold(const float &p_threshold);
	float get_threshold() const;

	VoxelMesh();
};

// class VoxelMeshInstance : public MeshInstance {
// 	GDCLASS(VoxelMeshInstance, MeshInstance);

// protected:
// 	static void _bind_methods();

// public:
// 	void set_collision_shape() const;
// 	VoxelMeshInstance();
// };

#endif
