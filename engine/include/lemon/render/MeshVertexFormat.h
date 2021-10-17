#pragma once

#include <lemon/render/material/MaterialConfiguration.h>

namespace lemon::render {
    enum class MeshIndexFormat { U16 = 2, U32 = 4 };

    enum class MeshComponents {
        None = 0,
        Position = 1 << 0,
        Normal = 1 << 1,
        Tangent = 1 << 2,
        UV0 = 1 << 3,
        UV1 = 1 << 4,
        JointInfluence = 1 << 5
    };

    class MeshVertexFormat {
        static constexpr uint8_t kMaxAttributes = 7;

        std::array<wgpu::VertexAttribute, kMaxAttributes> attributes{};
        uint64_t stride = 0;
        uint8_t attributeCount = 0;
        uint8_t locationCount = 0;
        MeshComponents components = MeshComponents::None;

    public:
        MeshVertexFormat() {}

        explicit MeshVertexFormat(MeshComponents inComponents)
        {
            setComponents(inComponents);
        }

        void
        setComponents(MeshComponents components);

        inline MeshComponents
        getComponents() const
        {
            return components;
        }

        inline uint64_t
        getStride() const
        {
            return stride;
        }

        inline uint32_t
        getAttributeCount() const
        {
            return attributeCount;
        }

        inline const wgpu::VertexAttribute*
        getAttributes() const
        {
            return attributes.data();
        }

        bool
        has(MeshComponents components) const;

        MaterialConfiguration
        getMeshConfig() const;

    private:
        void
        add(wgpu::VertexFormat format);

        void
        skip();

        void
        reset();
    };
} // namespace lemon::render
