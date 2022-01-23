#pragma once

#include <lemon/render/common.h>
#include <lemon/render/material/common.h>
#include <lemon/render/material/ShaderProgram.h>
#include <lemon/render/material/MaterialLayout.h>

namespace lemon::render {
    // TODO: Figure out proper constraints for uniform data types. `std::semiregular` is not enough here.
    template<typename T>
    concept MaterialUniformDataType = std::semiregular<T>;

    class MaterialUniformData {
        static constexpr size_t kUniformBufferMemberAlignment = 4;

        KeepAlive<MaterialLayout> kaLayout{};
        uint32_t offsetCount = 0;
        uint32_t offsets[kMaterialMaxUniforms]{0};
        AlignedMemory<kUniformBufferMemberAlignment> data;

    public:
        MaterialUniformData() {}

        MaterialUniformData(const KeepAlive<MaterialLayout>& kaLayout);

        void
        copyFrom(const MaterialUniformData& other)
        {
            if (kaLayout == other.kaLayout) {
                data = other.data;
            } else {
                logger::warn("failed to copy material uniform data: layout mismatch");
            }
        }

        template<MaterialUniformDataType TData>
        void
        setData(StringID id, const TData& val)
        {
            auto& dataLayout = kaLayout->uniformLayout;

            for (uint8_t u = 0; u < dataLayout.uniformCount; u++) {
                auto& uniform = dataLayout.uniforms[u];

                if (uniform.id == id) {
                    const uint32_t sz = sizeof(val);

                    // Uniform data is padded to 16-byte alignment, so it's possible that our actual data is a
                    // bit smaller.
                    if (math::alignUp(sz, kUniformBufferDataAlignment) == uniform.size) {
                        auto offset = uniform.members[0].offset;
                        auto* pDest = data.get<TData>(offset);
                        *pDest = val;
                    } else {
                        logger::warn("failed to set uniform value: size mismatch. expected: ", uniform.size,
                                     " actual: ", sizeof(val), " type: ", typeid(TData).name(), " id: ", id);
                    }

                    return;
                }

                for (uint8_t m = 0; m < uniform.memberCount; m++) {
                    auto& member = uniform.members[m];

                    if (member.id == id) {
                        if (sizeof(val) == member.size) {
                            // TODO: Check pointer alignment?
                            auto* pDest = data.get<TData>(member.offset);
                            *pDest = val;
                        } else {
                            logger::warn(
                                "failed to set uniform value: member size mismatch. expected: ", member.size,
                                " actual: ", sizeof(val), " type: ", typeid(TData).name(), " id: ", id);
                        }

                        return;
                    }
                }
            }

            logger::warn("failed to set uniform value: member not found. id: ", id);
        }

        void
        merge(class ConstantBuffer& buffer);

        void
        setLayout(const KeepAlive<MaterialLayout>& inLayout);

        inline uint32_t
        getOffsetCount() const
        {
            return offsetCount;
        }

        inline const uint32_t*
        getOffsets() const
        {
            return offsets;
        }
    };
} // namespace lemon::render
