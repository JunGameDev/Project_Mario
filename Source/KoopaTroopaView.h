#pragma once


#include <Core/Layers/View.h>
#include <Graphics/Graphics.h>
#include <Input/Input.h>
#include <Audio/Audio.h>
#include <vector>
#include <GUI.h>
#include <Events/Processes.h>
#include <Physics/Physics.h>

class AnimationComponent;
class TextureComponent;

class KoopaTroopaView : public Bel::IView
{
private:
    Bel::TransformComponent* m_pTransform;
    TextureComponent*   m_pTexture;
    AnimationComponent* m_pAnim;
    Bel::ProcessManager m_processManager;

    bool m_canAttack;

public:
    KoopaTroopaView();
    virtual ~KoopaTroopaView() {}
    KoopaTroopaView& operator=(const KoopaTroopaView& rhs) = delete;

    // Inherited via IView
    virtual bool Initialize() override;
    virtual void UpdateInput(float delta) override;
    virtual void ViewScene() override;
    virtual void Delete() override;

    //virtual void OnCollisionEnter(Bel::ICollider2D* pLhs, Bel::ICollider2D* pRhs) override;
    //virtual void OnCollisionExit(Bel::ICollider2D* pLhs, Bel::ICollider2D* pRhs) override;

    virtual void OnCollisionEnter(Bel::Collision& col) override;
    virtual void OnCollisionExit(Bel::Collision& col) override;
    virtual void OnTriggerEnter(Bel::Collision& col) override;

    virtual void ChaseTarget(Bel::IEvent* pEvent);

    void DeleteActorAndView();
    void ReachGoal(Bel::IEvent* pEvent);
};