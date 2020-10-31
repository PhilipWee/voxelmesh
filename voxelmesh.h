/*voxelmesh.h*/

#ifndef VOXELMESH_H
#define VOXELMESH_H

#include "core/reference.h"
#include "scene/3d/mesh_instance.h"
#include "scene/resources/mesh.h"
#include "scene/resources/primitive_meshes.h"
#include "MCTable.h"
#include "mpVector.h"

class VoxelMeshInstance : public MeshInstance {
	GDCLASS(VoxelMeshInstance, MeshInstance);

protected:
	static void _bind_methods();

public:
	VoxelMeshInstance();
};

class VoxelMesh : public PrimitiveMesh {
	GDCLASS(VoxelMesh, PrimitiveMesh);

private:
	Size2 size;
	int subdivide_w;
	int subdivide_d;

protected:
	static void _bind_methods();
    virtual void _create_mesh_array(Array &p_arr) const;

public:
	VoxelMesh();
};

#endif
