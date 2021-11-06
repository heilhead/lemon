#pragma once

#include <lemon/game.h>

namespace demo {
    class DemoModelActor : public lemon::game::Actor {
        lemon::game::StaticMeshComponent* pMeshComponent{nullptr};
        int wave;
        glm::f32vec3 color;

    public:
        DemoModelActor();

        void
        onStart() override;

        void
        onTick(float dt) override;

        void
        setColor(const glm::f32vec3& inColor)
        {
            constexpr auto tint = lemon::sid("materialParams.tint");
            color = inColor;
            pMeshComponent->setMaterialParameter(0, tint, glm::f32vec4(inColor, 1.f));
        }

        void
        setWave(int inWave)
        {
            wave = inWave;
        }
    };
} // namespace demo
