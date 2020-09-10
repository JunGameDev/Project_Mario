#pragma once
#include <Core/Layers/ApplicationLayer.h>
#include <Core/Layers/View.h>
#include <Process.h>
#include <Events/Events.h>
#include <Physics/Physics.h>

class AudioSource;
class TextureComponent;
class AnimationComponent;

class BowserView : public Bel::IView
{
private:
    Bel::IGameLayer* m_pGameLayer;

    Bel::ProcessManager m_processManager;
    Bel::TransformComponent* m_pActorTransform;
    Bel::IDynamicBodyComponent* m_pBody;
    AudioSource* m_pAudioSource;
    TextureComponent* m_pTexture;
    AnimationComponent* m_pActorAnim;

    bool m_canFire;
    bool m_collidingWithPlayer;

public:
    BowserView()
        : m_canFire(true)
        , m_collidingWithPlayer(false)
    {
    }

    virtual ~BowserView() override
    {
        Delete();
    }
    virtual bool Initialize() override;
    virtual void UpdateInput(float delta) override;
    virtual void ViewScene() override;
    virtual void Delete() override;

    virtual void OnCollisionEnter(Bel::Collision& col) override;
    virtual void OnTriggerEnter(Bel::Collision& col) override;
    virtual void OnTriggerExit(Bel::Collision& col) override;

    void SpawnProjectile(Bel::IEvent* pEvent);
    void RemoveAll(Bel::IEvent* pEvent);

    void DeleteActorAndView();
    void ReachGoal(Bel::IEvent* pEvent);
};
