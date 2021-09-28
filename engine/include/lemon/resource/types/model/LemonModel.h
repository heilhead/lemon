#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <webgpu/webgpu_cpp.h>
#include <lemon/shared/DataBuffer.h>
#include <lemon/serialization.h>
#include <lemon/serialization/glm.h>
#include <lemon/serialization/DataBuffer.h>
#include <lemon/render.h>

namespace lemon::res::model {
    enum class MeshIndexFormat { U16 = 2, U32 = 4 };

    enum class MeshComponents {
        Position = 1 << 0,
        Normal = 1 << 1,
        Tangent = 1 << 2,
        UV0 = 1 << 3,
        UV1 = 1 << 4,
        JointInfluence = 1 << 5
    };

    struct MeshPackedVertex {
        glm::f32vec3 position;
        glm::i8vec4 normal;
        glm::i8vec4 tangent;
        glm::u16vec2 uv0;
        glm::u16vec2 uv1;
        glm::u8vec4 jointIndex;
        glm::u8vec4 jointWeight;

        static constexpr size_t
        getSize(MeshComponents components)
        {
            size_t size = 0;

            if ((bool)(components & MeshComponents::Position)) {
                size += sizeof(position);
            }

            if ((bool)(components & MeshComponents::Normal)) {
                size += sizeof(normal);
            }

            if ((bool)(components & MeshComponents::Tangent)) {
                size += sizeof(tangent);
            }

            if ((bool)(components & MeshComponents::UV0)) {
                size += sizeof(uv0);
            }

            if ((bool)(components & MeshComponents::UV1)) {
                size += sizeof(uv1);
            }

            if ((bool)(components & MeshComponents::JointInfluence)) {
                size += sizeof(jointIndex);
                size += sizeof(jointWeight);
            }

            return size;
        }

        template<class Archive>
        inline void
        save(Archive& ar, MeshComponents components) const
        {
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

    struct MeshGPUVertexFormat {
        static constexpr uint8_t kMaxAttributes = 7;

        std::array<wgpu::VertexAttribute, kMaxAttributes> attributes;
        uint64_t stride = 0;
        uint8_t attributeCount = 0;

    private:
        uint8_t locationCount = 0;

    public:
        void
        add(wgpu::VertexFormat format)
        {
            auto idx = (size_t)attributeCount++;
            auto size = lemon::render::getVertexFormatSize(format);
            auto location = (size_t)locationCount++;

            assert(size > 0);

            attributes[idx].format = format;
            attributes[idx].offset = stride;
            attributes[idx].shaderLocation = location;

            stride += size;
        }

        void
        skip()
        {
            locationCount++;
        }

        void
        reset()
        {
            stride = 0;
            attributeCount = 0;
            locationCount = 0;
        }
    };

    struct ModelMesh {
    public:
        uint8_t material;

        MeshComponents components;

        MeshGPUVertexFormat vertexFormat;
        wgpu::IndexFormat indexFormat;
        size_t indexCount;

        HeapBuffer vertexData;
        HeapBuffer indexData;

        std::optional<std::vector<glm::mat4>> joints{};

    public:
        template<class Archive>
        inline void
        load(Archive& ar)
        {
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

            assert(indexData.size() % idxSize == 0);

            updateVertexFormat();
        }

        void
        updateVertexFormat()
        {
            vertexFormat.reset();

            if ((bool)(components & MeshComponents::Position)) {
                vertexFormat.add(wgpu::VertexFormat::Float32x3);
            } else {
                vertexFormat.skip();
            }

            if ((bool)(components & MeshComponents::Normal)) {
                vertexFormat.add(wgpu::VertexFormat::Snorm8x4);
            } else {
                vertexFormat.skip();
            }

            if ((bool)(components & MeshComponents::Tangent)) {
                vertexFormat.add(wgpu::VertexFormat::Snorm8x4);
            } else {
                vertexFormat.skip();
            }

            if ((bool)(components & MeshComponents::UV0)) {
                vertexFormat.add(wgpu::VertexFormat::Unorm16x2);
            } else {
                vertexFormat.skip();
            }

            if ((bool)(components & MeshComponents::UV1)) {
                vertexFormat.add(wgpu::VertexFormat::Unorm16x2);
            } else {
                vertexFormat.skip();
            }

            if ((bool)(components & MeshComponents::JointInfluence)) {
                vertexFormat.add(wgpu::VertexFormat::Uint8x4);
                vertexFormat.add(wgpu::VertexFormat::Unorm8x4);
            } else {
                vertexFormat.skip();
                vertexFormat.skip();
            }
        }
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

        LemonModel()
        {
            lemon::utils::log(__FUNCTION__);
        }

        ~LemonModel()
        {
            lemon::utils::log(__FUNCTION__);
        }

        template<class Archive>
        inline void
        serialize(Archive& ar)
        {
            LEMON_SERIALIZE(ar, meshes);
            LEMON_SERIALIZE(ar, nodes);
        }
    };
} // namespace lemon::res::model
