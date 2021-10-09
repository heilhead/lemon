#include <lemon/render/material/MaterialUniformData.h>

using namespace lemon::render;

MaterialUniformData::MaterialUniformData(const KeepAlive<MaterialLayout>& inLayout)
{
    layout = inLayout;
    offsetCount = layout->uniformLayout.uniformCount;
    data.allocate(layout->uniformLayout.totalSize);
}
