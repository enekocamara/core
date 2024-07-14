#pragma once
#include "EntityComponentSystem.h"
#include "Components.h"
namespace ecs {
    using SApplyVelocity = ecs::Signature<CPosition, CVelocity>;
    using SApplyAcceleration = ecs::Signature<CVelocity, CAcceleration>;
    using SPlayer = ecs::Signature<SApplyAcceleration, SApplyVelocity>;

    using MySignatures = SignatureList<SApplyVelocity, SApplyAcceleration>;
}
