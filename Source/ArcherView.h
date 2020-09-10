#pragma once
#include <ApplicationLayer.h>
#include <View.h>
#include <Process.h>
#include <Events.h>

class AudioSource;
class TextureComponent;
class AnimationComponent;
class TransformComponent;
class BoxCollider2D;
class RaderCollider;

class ArcherView : public Bel::IView
{
private:
    Bel::IGameLayer* m_pGameLayer;

    Bel::ProcessManager m_processManager;
    TransformComponent* m_pActorTransform;
    AudioSource* m_pAudioSource;
    TextureComponent* m_pTexture;
    AnimationComponent* m_pActorAnim;
    BoxCollider2D* m_pCollider;
    RaderCollider* m_pRader;

    std::size_t m_listenerId;
    bool m_canFire;
    bool m_collidingWithPlayer;

public:
    ArcherView()
        : m_canFire(true)
        , m_collidingWithPlayer(false)
    {
    }

    virtual ~ArcherView() override 
    {
        m_processManager.AbortAllProcesses();
    }
    virtual bool Initialize() override;
    virtual void UpdateInput(float delta) override;
    virtual void ViewScene() override;
    virtual void OnCollisionEnter(Bel::Actor* pOther, bool isSensor = false) override;
    virtual void OnCollisionExit(Bel::Actor* pOther, bool isSensor = false) override;

    void FireArrow(Bel::IEvent* pEvent);
    void RemoveAll(Bel::IEvent* pEvent);
};
