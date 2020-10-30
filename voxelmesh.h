/*voxelmesh.h*/

#ifndef VOXELMESH_H
#define VOXELMESH_H

#include "core/reference.h"
#include "scene/resources/mesh.h"
#include "scene/3d/mesh_instance.h"

class VoxelMesh : public MeshInstance {
    GDCLASS(VoxelMesh, MeshInstance);

    protected:
        
        static void _bind_methods();


    public:
        VoxelMesh();

};

#endif

