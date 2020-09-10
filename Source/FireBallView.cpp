#include <Core/Layers/ApplicationLayer.h>
#include "FireBallView.h"
using namespace Bel;

 bool FireBallView::Initialize()
{

    return true;
}

void FireBallView::UpdateInput(float delta)
{
}

void FireBallView::ViewScene()
{
}

void FireBallView::Delete()
{
}

void FireBallView::OnCollisionEnter(Bel::Collision& col)
{
    auto& scripting = ApplicationLayer::GetInstance()->GetGameLayer()->GetScriptingManager();

    scripting.StartFunction("FireBallOnCollisionEnter");
    scripting.PushNumber(GetActor()->GetId());
    scripting.PushNumber(col.m_pActor->GetId());
    scripting.PushString(col.m_pActor->GetName().c_str());
    scripting.PushString(col.m_userData.c_str());
    scripting.PushBoolean(m_headingRight);
    scripting.CallFunction(0);
}
