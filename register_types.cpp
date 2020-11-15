#include "register_types.h"

#include "core/class_db.h"
#include "voxelmesh.h"

void register_voxelmesh_types() {
    ClassDB::register_class<VoxelMesh>();
    // ClassDB::register_class<VoxelMeshInstance>();
};

void unregister_voxelmesh_types() {
    //Nothing yet
}