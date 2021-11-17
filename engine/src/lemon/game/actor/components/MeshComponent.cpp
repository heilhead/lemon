#include <lemon/game/actor/components/MeshComponent.h>
#include <lemon/game/actor/GameWorld.h>
#include <lemon/resource/ResourceManager.h>
#include <lemon/resource/types/MaterialResource.h>

using namespace lemon;
using namespace lemon::game;
using namespace lemon::res;
using namespace lemon::render;

MeshComponent::MeshDescriptor::MeshDescriptor(size_t index,
                                              const res::ModelResource::MeshWrapper& meshWrapper,
                                              const res::MaterialResource* pMaterial)
    : index{index}
{
    const auto* pMesh = meshWrapper.pMesh;

    components = pMesh->components;
    format = pMesh->vertexFormat;
    buffer = meshWrapper.buffer;
    renderHandle = MeshRenderHandle();
    bMaterialDirty = true;
    materialIndex = pMesh->material;

    if (pMaterial != nullptr) {
        createMaterialInstance(pMaterial);
    } else {
        material = std::nullopt;
    }
}

void
MeshComponent::MeshDescriptor::createMaterialInstance(const res::MaterialResource* pMaterial)
{
    material = MaterialManager::get()->getSurfaceMaterialInstance(*pMaterial, format);
}

void
MeshComponent::setModel(const ResourceLocation& loc)
{
    auto* pResourceMan = ResourceManager::get();

    switch (pResourceMan->getResourceState(loc.handle, loc.object)) {
    case ResourceState::Loading:
    case ResourceState::NotLoaded:
        Scheduler::get()->block(pResourceMan->loadResource<ModelResource>(loc));
    }

    auto* pResource = pResourceMan->getResource<ModelResource>(loc);
    if (pResource == nullptr) {
        auto currState = pResourceMan->getResourceState(loc.handle, loc.object);
        logger::warn("failed to set mesh: resource not available: state = ",
                     magic_enum::enum_name(currState));
        return;
    }

    auto* pModel = pResource->getObject<ModelResource::Model>(loc.object);

    LEMON_ASSERT(pModel != nullptr);

    updateModel(pModel);
}

void
MeshComponent::setMaterial(uint8_t index, const ResourceLocation& loc)
{
    auto* pResourceMan = ResourceManager::get();

    switch (pResourceMan->getResourceState(loc.handle, loc.object)) {
    case ResourceState::Loading:
    case ResourceState::NotLoaded:
        Scheduler::get()->block(pResourceMan->loadResource<MaterialResource>(loc));
    }

    auto* pMaterial = pResourceMan->getResource<MaterialResource>(loc);
    if (pMaterial == nullptr) {
        auto currState = pResourceMan->getResourceState(loc.handle);
        logger::warn("failed to set material: resource not available: state = ",
                     magic_enum::enum_name(currState));
        return;
    }

    updateMaterial(index, pMaterial);
}

void
MeshComponent::updateRenderProxy(MeshRenderProxy& proxy)
{
    using namespace magic_enum::flags;

    if (proxy.matrixGeneration != updateGlobalTransform()) {
        proxy.matrix = getGlobalTransformMatrix();
    }

    auto& mesh = meshes[proxy.meshIndex];

    if (mesh.bMaterialDirty) {
        mesh.bMaterialDirty = false;
        proxy.material.getUniformData().copyFrom((*mesh.material).getUniformData());
    }
}

void
MeshComponent::updateModel(const res::ModelResource::Model* pModel)
{
    meshes.clear();

    size_t index = 0;
    for (auto& mesh : pModel->getMeshes()) {
        MeshDescriptor desc{index++, mesh, getMaterial(mesh.pMesh->material)};

        if (desc.isValid()) {
            addRenderProxy(desc);
        }

        meshes.emplace_back(std::move(desc));
    }
}

void
MeshComponent::updateMaterial(uint8_t index, const MaterialResource* pMaterial)
{
    LEMON_ASSERT(pMaterial != nullptr);

    if (index >= materials.size()) {
        materials.resize(index + 1);
    }

    if (materials[index] == pMaterial) {
        return;
    }

    materials[index] = pMaterial;

    for (auto& mesh : meshes) {
        if (mesh.materialIndex == index) {
            if (mesh.hasRenderHandle()) {
                removeRenderProxy(mesh);
            }

            mesh.createMaterialInstance(pMaterial);

            addRenderProxy(mesh);
        }
    }
}

void
MeshComponent::addRenderProxy(MeshDescriptor& mesh)
{
    LEMON_ASSERT(!mesh.hasRenderHandle());
    LEMON_ASSERT(mesh.isValid());

    MeshRenderProxy proxy{.pOwner = this,
                          .material = *mesh.material,
                          .vertexBuffer = mesh.buffer.vertexBuffer,
                          .indexBuffer = mesh.buffer.indexBuffer,
                          .indexCount = mesh.buffer.indexCount,
                          .indexFormat = mesh.buffer.indexFormat,
                          .matrix = kMatrixIdentity,
                          .matrixGeneration = 0};

    mesh.renderHandle = GameWorld::get()->getRenderQueue().addMeshProxy(std::move(proxy));
}

void
MeshComponent::removeRenderProxy(MeshDescriptor& mesh)
{
    LEMON_ASSERT(mesh.hasRenderHandle());

    GameWorld::get()->getRenderQueue().removeMeshProxy(mesh.renderHandle);

    mesh.renderHandle = MeshRenderHandle();
}

inline bool
MeshComponent::hasMaterial(uint8_t index) const
{
    return materials.size() > index && materials[index] != nullptr;
}

inline const res::MaterialResource*
MeshComponent::getMaterial(uint8_t index) const
{
    if (hasMaterial(index)) {
        return materials[index];
    }

    return nullptr;
}
//
// void
// StaticMeshComponent::updateModel(const ModelResource::Model* pModel)
//{
//    MeshComponent::updateModel(pModel);
//
//    // component-specific init
//}
//
// void
// StaticMeshComponent::updateMaterial(uint8_t index, const MaterialResource* pMaterial)
//{
//    MeshComponent::updateMaterial(index, pMaterial);
//
//    // component-specific init
//}
//
// void
// SkeletalMeshComponent::updateModel(const ModelResource::Model* pModel)
//{
//    MeshComponent::updateModel(pModel);
//
//    // component-specific init
//}
//
// void
// SkeletalMeshComponent::updateMaterial(uint8_t index, const MaterialResource* pMaterial)
//{
//    MeshComponent::updateMaterial(index, pMaterial);
//
//    // component-specific init
//}
