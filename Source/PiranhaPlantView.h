#pragma once

#include <Core/Layers/View.h>
#include <Graphics/Graphics.h>
#include <Input/Input.h>
#include <Audio/Audio.h>
#include <vector>
#include <GUI.h>
#include <Physics/Physics.h>

class AnimationComponent;

class PiranhaPlantView : public Bel::IView
{
private:
    Bel::TransformComponent* m_pTransform;
    AnimationComponent* m_pAnim;

public:
    PiranhaPlantView()
    {
    }
    virtual ~PiranhaPlantView() {}
    PiranhaPlantView& operator=(const PiranhaPlantView& rhs) = delete;


    // Inherited via IView
    virtual bool Initialize() override;
    virtual void UpdateInput(float delta) override;
    virtual void ViewScene() override;
};