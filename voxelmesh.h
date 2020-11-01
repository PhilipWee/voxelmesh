/*voxelmesh.h*/

#ifndef VOXELMESH_H
#define VOXELMESH_H

#include "MCTable.h"
#include "core/reference.h"
#include "mpVector.h"
#include "scene/3d/mesh_instance.h"
#include "scene/resources/mesh.h"
#include "scene/resources/primitive_meshes.h"

class VoxelMeshInstance : public MeshInstance {
	GDCLASS(VoxelMeshInstance, MeshInstance);

protected:
	static void _bind_methods();

public:
	VoxelMeshInstance();
};

class VoxelMesh : public PrimitiveMesh {
	GDCLASS(VoxelMesh, PrimitiveMesh);

protected:
	float minValue;
	static void _bind_methods();
	virtual void _create_mesh_array(Array &p_arr) const;

public:
	void set_size(const float &p_size);
	float get_size() const;

	VoxelMesh();
};

#endif
