#include <lemon/render/material/MaterialUniformData.h>
#include <lemon/render/ConstantBuffer.h>

using namespace lemon::render;

MaterialUniformData::MaterialUniformData(const KeepAlive<MaterialLayout>& inLayout)
{
    setLayout(inLayout);
}

void
MaterialUniformData::merge(ConstantBuffer& buffer)
{
    auto& uniformLayout = kaLayout->uniformLayout;
    auto dataOffset = 0u;

    for (int i = 0; i < uniformLayout.uniformCount; i++) {
        auto& uniform = uniformLayout.uniforms[i];
        offsets[i] = buffer.write(data.get(dataOffset), uniform.size);
        dataOffset += uniform.size;
    }
}

void
MaterialUniformData::setLayout(const KeepAlive<MaterialLayout>& inLayout)
{
    kaLayout = inLayout;
    offsetCount = kaLayout->uniformLayout.uniformCount;
    data.allocate(kaLayout->uniformLayout.totalSize);
}
