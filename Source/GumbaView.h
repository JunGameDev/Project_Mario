#pragma once

#include <Core/Layers/View.h>
#include <Graphics/Graphics.h>
#include <Input/Input.h>
#include <Audio/Audio.h>
#include <vector>
#include <GUI.h>
#include <Physics/Physics.h>

class AnimationComponent;

class GumbaView : public Bel::IView
{
private:
    Bel::TransformComponent* m_pTransform;
    AnimationComponent* m_pAnim;
    Bel::ProcessManager m_processManager;
    Bel::IDynamicBodyComponent* m_pBody;
    std::shared_ptr<Bel::IProcess> m_pPatrolProcess;
    bool m_movingRight;

public:
    GumbaView()
        : m_pAnim(nullptr)
        , m_pTransform(nullptr)
        , m_pBody(nullptr)
        , m_pPatrolProcess(nullptr)
        , m_movingRight(true)
    {
    }
    virtual ~GumbaView() override
    {
    }
    GumbaView& operator=(const GumbaView& rhs) = delete;


    // Inherited via IView
    virtual bool Initialize() override;
    virtual void UpdateInput(float delta) override;
    virtual void ViewScene() override;
    virtual void Delete() override;

    virtual void OnCollisionEnter(Bel::Collision& col) override;
    virtual void OnTriggerEnter(Bel::Collision& col) override;

};

