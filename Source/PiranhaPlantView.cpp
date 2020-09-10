#include <Core/Layers/ApplicationLayer.h>

#include "PiranhaPlantView.h"
#include "ComponentIDs.h"
#include "AnimationComponent.h"
 
using namespace Bel;

bool PiranhaPlantView::Initialize()
{
    // --- Actor Components ---
    {
        if (!GetActor()->HasComponent(kTransformId))
        {
            LOG_FATAL("Unable to finc Bel::TransformComponent for player");
            return false;
        }

        m_pTransform = static_cast<Bel::TransformComponent*>(GetActor()->GetComponent(kTransformId));

        if (!GetActor()->HasComponent(kAnimationId))
        {
            LOG_FATAL("Unable to acquire AnimationComponent");
            return false;
        }

        m_pAnim = static_cast<AnimationComponent*>(GetActor()->GetComponent(kAnimationId));
        m_pAnim->SetAnimation("Walk");
    }

    return true;
}

void PiranhaPlantView::UpdateInput(float delta)
{
}

void PiranhaPlantView::ViewScene()
{
}
