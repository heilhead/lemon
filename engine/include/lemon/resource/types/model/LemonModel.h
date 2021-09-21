#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <lemon/shared/DataBuffer.h>
#include <lemon/serialization.h>
#include <lemon/serialization/glm.h>
#include <lemon/serialization/DataBuffer.h>

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
        glm::i8vec3 normal;
        glm::i8vec3 tangent;
        glm::u16vec2 uv0;
        glm::u16vec2 uv1;
        glm::u8vec4 jointIndex;
        glm::u8vec4 jointWeight;

        static constexpr size_t
        getSize(MeshComponents components) {
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
        serialize(Archive& ar, MeshComponents components) const {
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
        uint8_t material;

        MeshComponents components;
        MeshIndexFormat indexFormat;

        HeapBuffer vertexBuffer;
        HeapBuffer indexBuffer;

        std::optional<std::vector<glm::mat4>> joints{};

        template<class Archive>
        inline void
        serialize(Archive& ar) {
            LEMON_SERIALIZE(ar, material);
            LEMON_SERIALIZE_FLAGS(ar, components);
            LEMON_SERIALIZE(ar, indexFormat);
            LEMON_SERIALIZE(ar, vertexBuffer);
            LEMON_SERIALIZE(ar, indexBuffer);
            LEMON_SERIALIZE(ar, joints);
        }
    };

    struct ModelNode {
        std::string name;
        std::vector<uint8_t> meshes;
        glm::mat4 transform;

        template<class Archive>
        inline void
        serialize(Archive& ar) {
            LEMON_SERIALIZE(ar, name);
            LEMON_SERIALIZE(ar, meshes);
            LEMON_SERIALIZE(ar, transform);
        }
    };

    struct LemonModel {
        std::vector<ModelMesh> meshes;
        std::vector<ModelNode> nodes;

        LemonModel() {
            lemon::utils::log(__FUNCTION__);
        }

        ~LemonModel() {
            lemon::utils::log(__FUNCTION__);
        }

        template<class Archive>
        inline void
        serialize(Archive& ar) {
            LEMON_SERIALIZE(ar, meshes);
            LEMON_SERIALIZE(ar, nodes);
        }
    };
} // namespace lemon::res::model