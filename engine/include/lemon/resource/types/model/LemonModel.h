#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <webgpu/webgpu_cpp.h>
#include <lemon/shared/Memory.h>
#include <lemon/shared/logger.h>
#include <lemon/serialization.h>
#include <lemon/serialization/glm.h>
#include <lemon/serialization/DataBuffer.h>
#include <lemon/render.h>
#include <lemon/render/MeshVertexFormat.h>

namespace lemon::res::model {
    struct MeshPackedVertex {
        glm::f32vec3 position;
        glm::i8vec4 normal;
        glm::i8vec4 tangent;
        glm::u16vec2 uv0;
        glm::u16vec2 uv1;
        glm::u8vec4 jointIndex;
        glm::u8vec4 jointWeight;

        static size_t
        getSize(render::MeshComponents components);

        template<class Archive>
        inline void
        save(Archive& ar, render::MeshComponents components) const
        {
            using namespace lemon::render;

            if ((bool)(components & MeshComponents::Position)) {
                LEMON_SERIALIZE(ar, position);
            }

            if ((bool)(components & MeshComponents::Normal)) {
                LEMON_SERIALIZE(ar, normal);
            }

            if ((bool)(components & MeshComponents::Tangent)) {
                LEMON_SERIALIZE(ar, tangent);
            }

            if ((bool)(components & MeshComponents::UV0)) {
                LEMON_SERIALIZE(ar, uv0);
            }

            if ((bool)(components & MeshComponents::UV1)) {
                LEMON_SERIALIZE(ar, uv1);
            }

            if ((bool)(components & MeshComponents::JointInfluence)) {
                LEMON_SERIALIZE(ar, jointIndex);
                LEMON_SERIALIZE(ar, jointWeight);
            }
        }
    };

    struct ModelMesh {
    public:
        uint8_t material;

        render::MeshComponents components;
        render::MeshVertexFormat vertexFormat;
        wgpu::IndexFormat indexFormat;
        size_t indexCount;

        UnalignedMemory vertexData;
        UnalignedMemory indexData;

        std::optional<std::vector<glm::mat4>> joints{};

    public:
        template<class Archive>
        inline void
        load(Archive& ar)
        {
            using namespace lemon::render;

            MeshIndexFormat lIdxFormat;

            LEMON_SERIALIZE(ar, material);
            LEMON_SERIALIZE_FLAGS(ar, components);
            LEMON_SERIALIZE(ar, lIdxFormat);
            LEMON_SERIALIZE(ar, vertexData);
            LEMON_SERIALIZE(ar, indexData);
            LEMON_SERIALIZE(ar, joints);

            auto idxSize = (int)lIdxFormat;
            indexFormat =
                lIdxFormat == MeshIndexFormat::U32 ? wgpu::IndexFormat::Uint32 : wgpu::IndexFormat::Uint16;
            indexCount = indexData.size() / idxSize;

            LEMON_ASSERT(indexData.size() % idxSize == 0);

            updateVertexFormat();
        }

        void
        updateVertexFormat();
    };

    struct ModelNode {
        std::string name;
        std::vector<uint8_t> meshes;
        glm::mat4 transform;

        template<class Archive>
        inline void
        serialize(Archive& ar)
        {
            LEMON_SERIALIZE(ar, name);
            LEMON_SERIALIZE(ar, meshes);
            LEMON_SERIALIZE(ar, transform);
        }
    };

    struct LemonModel {
        std::vector<ModelMesh> meshes;
        std::vector<ModelNode> nodes;

        LemonModel();
        ~LemonModel();

        template<class Archive>
        inline void
        serialize(Archive& ar)
        {
            LEMON_SERIALIZE(ar, meshes);
            LEMON_SERIALIZE(ar, nodes);
        }
    };
} // namespace lemon::res::model
