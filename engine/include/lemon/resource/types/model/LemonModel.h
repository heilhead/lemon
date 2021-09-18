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

            if (enum_integer(components & MeshComponents::Position)) {
                size += sizeof(position);
            }

            if (enum_integer(components & MeshComponents::Normal)) {
                size += sizeof(normal);
            }

            if (enum_integer(components & MeshComponents::Tangent)) {
                size += sizeof(tangent);
            }

            if (enum_integer(components & MeshComponents::UV0)) {
                size += sizeof(uv0);
            }

            if (enum_integer(components & MeshComponents::UV1)) {
                size += sizeof(uv1);
            }

            if (enum_integer(components & MeshComponents::JointInfluence)) {
                size += sizeof(jointIndex);
                size += sizeof(jointWeight);
            }

            return size;
        }

        template<class Archive>
        inline void
        serialize(Archive& ar, MeshComponents components) const {
            if (enum_integer(components & MeshComponents::Position)) {
                LEMON_SERIALIZE(ar, position);
            }

            if (enum_integer(components & MeshComponents::Normal)) {
                LEMON_SERIALIZE(ar, normal);
            }

            if (enum_integer(components & MeshComponents::Tangent)) {
                LEMON_SERIALIZE(ar, tangent);
            }

            if (enum_integer(components & MeshComponents::UV0)) {
                LEMON_SERIALIZE(ar, uv0);
            }

            if (enum_integer(components & MeshComponents::UV1)) {
                LEMON_SERIALIZE(ar, uv1);
            }

            if (enum_integer(components & MeshComponents::JointInfluence)) {
                LEMON_SERIALIZE(ar, jointIndex);
                LEMON_SERIALIZE(ar, jointWeight);
            }
        }
    };

    struct ModelMesh {
        MeshComponents components;
        MeshIndexFormat indexFormat;

        HeapBuffer vertexBuffer;
        HeapBuffer indexBuffer;

        std::optional<std::vector<glm::mat4>> joints;

        template<class Archive>
        inline void
        serialize(Archive& ar) {
            LEMON_SERIALIZE(ar, components);
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

        template<class Archive>
        inline void
        serialize(Archive& ar) {
            LEMON_SERIALIZE(ar, meshes);
            LEMON_SERIALIZE(ar, nodes);
        }
    };
} // namespace lemon::res::model