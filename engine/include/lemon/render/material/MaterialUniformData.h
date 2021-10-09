#pragma once

#include <cstdint>
#include <lemon/shared/DataBuffer.h>
#include <lemon/shared/AtomicCache.h>
#include <lemon/render/material/common.h>
#include <lemon/render/material/ShaderProgram.h>
#include <lemon/render/material/MaterialLayout.h>

namespace lemon::render {
    // TODO: Figure out proper constraints for uniform data types. `std::is_copy_assignable` is not enough
    // here.
    template<typename T>
    concept CopyableUniform = std::is_integral_v<T> || std::is_copy_assignable_v<T>;

    class MaterialUniformData {
        KeepAlive<MaterialLayout> layout;
        uint32_t offsetCount = 0;
        uint32_t offsets[kMaterialMaxUniforms]{0};
        HeapBuffer data;

    public:
        MaterialUniformData(const KeepAlive<MaterialLayout>& inLayout);

        template<CopyableUniform TData>
        void
        setData(StringID id, const TData& val)
        {
            auto& dataLayout = layout->uniformLayout;

            for (uint8_t u = 0; u < dataLayout.uniformCount; u++) {
                auto& uniform = dataLayout.uniforms[u];

                for (uint8_t m = 0; m < uniform.memberCount; m++) {
                    auto& member = uniform.members[m];

                    if (member.id == id) {
                        if (sizeof(val) != member.size) {
                            logger::warn(
                                "failed to set uniform value: member size mismatch. expected: ", member.size,
                                " actual: ", sizeof(val), " type: ", typeid(TData).name(), " id: ", id);
                            return;
                        }

                        auto* pData = *data;
                        auto* pDest = reinterpret_cast<TData*>(pData + (ptrdiff_t)member.offset);
                        *pDest = val;

                        return;
                    }
                }
            }

            logger::warn("failed to set uniform value: member not found. id: ", id);
        }
    };
} // namespace lemon::render
