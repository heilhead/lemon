#pragma once

#include <concepts>
#include <cstdint>
#include <lemon/shared/Memory.h>
#include <lemon/shared/AtomicCache.h>
#include <lemon/render/material/common.h>
#include <lemon/render/material/ShaderProgram.h>
#include <lemon/render/material/MaterialLayout.h>

namespace lemon::render {
    class MaterialUniformData {
        static constexpr size_t kUniformBufferDataAlignment = 4;

        KeepAlive<MaterialLayout> kaLayout{};
        uint32_t offsetCount = 0;
        uint32_t offsets[kMaterialMaxUniforms]{0};
        AlignedMemory<kUniformBufferDataAlignment> data;

    public:
        MaterialUniformData() {}

        MaterialUniformData(const KeepAlive<MaterialLayout>& kaLayout);

        // TODO: Figure out proper constraints for uniform data types. `std::regular` is not enough here.
        template<std::regular TData>
        void
        setData(StringID id, const TData& val)
        {
            auto& dataLayout = kaLayout->uniformLayout;

            for (uint8_t u = 0; u < dataLayout.uniformCount; u++) {
                auto& uniform = dataLayout.uniforms[u];

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
