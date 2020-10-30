#include "voxelmesh.h"
#include "scene/resources/primitive_meshes.h"

VoxelMesh::VoxelMesh() {

    //Set the mesh to a cube for testing
    Ref<CubeMesh> cube_mesh;
    cube_mesh.instance();
    set_mesh(cube_mesh);
}

void VoxelMesh::_bind_methods(){
    //Remember to bind methods for callbacks here!
}