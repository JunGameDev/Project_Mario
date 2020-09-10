#pragma once

#include <Core/Layers/View.h>
#include <Graphics/Graphics.h>
#include <Input/Input.h>
#include <Audio/Audio.h>
#include <vector>
#include <GUI.h>
#include <Physics/Physics.h>

class AnimationComponent;

class MushroomView : public Bel::IView
{
private:
    Bel::TransformComponent* m_pTransform;
    Bel::ProcessManager m_processManager;
    Bel::IDynamicBodyComponent* m_pBody;
    std::shared_ptr<Bel::IProcess> m_pPatrolProcess;
    bool m_movingRight;

public:
    MushroomView()
        : m_pTransform(nullptr)
        , m_pBody(nullptr)
        , m_pPatrolProcess(nullptr)
        , m_movingRight(true)
    {
    }
    virtual ~MushroomView() override
    {
        Delete();
    }
    MushroomView& operator=(const MushroomView& rhs) = delete;


    // Inherited via IView
    virtual bool Initialize() override;
    virtual void UpdateInput(float delta) override;
    virtual void ViewScene() override;
    virtual void Delete() override;

    virtual void OnCollisionEnter(Bel::Collision& col) override;
    virtual void OnTriggerEnter(Bel::Collision& col) override;

};

