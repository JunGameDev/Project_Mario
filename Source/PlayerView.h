#pragma once

#include <Core/Layers/View.h>
#include <Graphics/Graphics.h>
#include <Input/Input.h>
#include <Audio/Audio.h>
#include <vector>
#include <Events/Processes.h>
#include <Events/Events.h>
#include <GUI.h>
#include <Physics/Physics.h>

class AnimationComponent;
class TextureComponent;
class MarioStateMachine;
class AudioSource;
class ButtonComponent;
class ParticleComponent;
class HealthComponent;
class Attackable;

class PlayerView : public Bel::IView
{
private:
    // --- Inputs ---
    Bel::IGraphics*         m_pGraphics;
    Bel::IKeyboard*         m_pKeyboard;
    Bel::IGameController*   m_pController;
    
    // --- Systems ---
    Bel::IMouse*        m_pMouse;
    Bel::IAudio*        m_pAudio;
    Bel::IGameLayer*    m_pGameLayer;
    Bel::ProcessManager m_processManager;

    // --- Components ---
    Bel::TransformComponent* m_pPlayerTransform;
    Bel::IDynamicBodyComponent* m_pPlayerComponent;
    AnimationComponent* m_pPlayerAnim;
    TextureComponent*   m_pPlayerTexture;
    MarioStateMachine*  m_pStateMachine;
    AudioSource*        m_pAudioSource;
    Attackable*         m_pAttack;
    HealthComponent*    m_pHp;

    std::shared_ptr<Bel::IProcess> m_pCameraMoveProcess;
    std::shared_ptr<Bel::IProcess> m_pGlueCameraProcess;

    float m_jumpingTimeCount;
    
    bool m_canFire;
    bool m_glueCamera;
    bool m_isGround;
    bool m_keepPressingSpace;
    bool m_controlable;

    std::shared_ptr<Bel::ITexture2D> m_pGameOverTexture;
    std::shared_ptr<Bel::ITexture2D> m_pGameClearTexture;

    struct MovingDir // 4 bytes
    {
        bool m_left;
        bool m_right;
        bool m_up;
        bool m_down;

        MovingDir()
            : m_left(false)
            , m_right(false)
            , m_up(false)
            , m_down(false)
        {
        }
    } m_movingDir;

public:
    PlayerView()
        : m_pGraphics(nullptr)
        , m_pKeyboard(nullptr)
        , m_pMouse(nullptr)
        , m_pAudio(nullptr)
        , m_canFire(false)
        , m_glueCamera(false)
        , m_isGround(true)
        , m_controlable(true)
        , m_keepPressingSpace(false)
        , m_jumpingTimeCount(0)
    {
    }
    virtual ~PlayerView()
    {
        for (auto [guid, id] : m_listenerIds)
        {
            m_pGameLayer->GetEventManager().RemoveEventListener(guid, id);
        }

        Delete();
    }
    PlayerView& operator=(const PlayerView& rhs) = delete;

    virtual bool Initialize()               override;
    virtual void UpdateInput(float delta)   override;
    virtual void ViewScene()                override;
    virtual void Delete()                   override;

    virtual void OnCollisionEnter(Bel::Collision& col) override;
    virtual void OnCollisionExit(Bel::Collision& col) override;
    virtual void OnTriggerEnter(Bel::Collision& col) override;

    bool IsMoving() const;
    bool IsGround() const;
    
    void SetGrounded(bool isGround);
    void SpwanFireBall(Bel::IEvent* pEvent);
    void CreateCameraMoveProcess();
    void GlueCameraToPlayer();
    void PickUpItem(Bel::IEvent* pEvent);
    void TeleportTo(Bel::IEvent* pEvent);
    void ReachGoal(Bel::IEvent* pEvent);
    void SetCameraToLocation(Bel::IEvent* pEvent);

    void SetControlable(bool controllable) { m_controlable = controllable; }
};