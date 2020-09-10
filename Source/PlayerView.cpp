#include <Core/Layers/ApplicationLayer.h>

#include "Game.h"
#include "PlayerView.h"
#include "ComponentIDs.h"
#include "AnimationComponent.h"
#include "TextureComponent.h"
#include "MarioStates.h"
#include "AudioSource.h"
#include "GameProcess.h"
#include "GameEvents.h"
#include "ButtonComponent.h"
#include "UIImage.h"
#include "HealthComponent.h"
#include "TagComponent.h"
#include "Attackable.h"
#include "DamageComponent.h"
#include "FireBallView.h"
#include "CrateComponent.h"

using namespace Bel;

namespace Lua
{
    static int PlayerViewSetGrounded(lua_State* pState)
    {
        auto pView = reinterpret_cast<PlayerView*>(lua_touserdata(pState, 1));
        bool isGround = static_cast<bool>(lua_toboolean(pState, 2));
        lua_pop(pState, 2);

        pView->SetGrounded(true);
        return 0;
    }

    static int PlayerViewIsGround(lua_State* pState)
    {
        auto pView = reinterpret_cast<PlayerView*>(lua_touserdata(pState, 1));
        lua_pop(pState, 1);
        lua_pushboolean(pState, pView->IsGround());
        return 1;
    }

    static int PlayerSpawnFireBall(lua_State* pState)
    {
        Actor::Id actorId = static_cast<Actor::Id>(lua_tonumber(pState, 1));
        float mouseX = static_cast<float>(lua_tonumber(pState, 2));
        float mouseY = static_cast<float>(lua_tonumber(pState, 3));
        lua_pop(pState, 3);

        auto pGameLayer = ApplicationLayer::GetInstance()->GetGameLayer();
        pGameLayer->GetEventManager().QueueEvent
        (
            std::make_unique<CreateActorAtLocationEvent>
            (
                actorId,
                Resource("Actors/FireBall.xml"),
                "PlayerFireball",
                Vector2<float>(mouseX, mouseY)
            )
        );

        return 0;
    }

    static int PlayerGlueCamera(lua_State* pState)
    {
        auto pView = reinterpret_cast<PlayerView*>(lua_touserdata(pState, 1));
        lua_pop(pState, 1);
        pView->GlueCameraToPlayer();
        return 0;
    }

    static int PlayerSetControlable(lua_State* pState)
    {
        auto pView = reinterpret_cast<PlayerView*>(lua_touserdata(pState, 1));
        bool controlable = static_cast<bool>(lua_toboolean(pState, 2));
        lua_pop(pState, 2);
        pView->SetControlable(controlable);
        return 0;
    }
}

bool PlayerView::Initialize()
{
    m_pGraphics     = ApplicationLayer::GetInstance()->GetGraphics();
    m_pKeyboard     = ApplicationLayer::GetInstance()->GetKeyboardInput();
    m_pController   = ApplicationLayer::GetInstance()->GetControllerInput();
    m_pMouse        = ApplicationLayer::GetInstance()->GetMouseInput();
    m_pAudio        = ApplicationLayer::GetInstance()->GetAudio();
    m_pGameLayer    = ApplicationLayer::GetInstance()->GetGameLayer();

    //m_pAudio->PlayMusic("Sounds/MarioMainTheme.mp3", 128);

    // --- Actor Components ---
    {
        auto pActor = GetActor();
       
        m_pPlayerTransform = static_cast<Bel::TransformComponent*>(pActor->GetComponent(kTransformId));
        if (m_pPlayerTransform == nullptr)
        {
            LOG_FATAL("Unable to finc TransformComponent for player");
            return false;
        }

        m_pPlayerComponent = static_cast<Bel::IDynamicBodyComponent*>(pActor->GetComponent(kDynamicBodyId));
        if (m_pPlayerComponent == nullptr)
        {
            LOG_FATAL("Unable to finc IDynamicBodyComponent for player");
            return false;
        }

        m_pPlayerTexture = static_cast<TextureComponent*>(pActor->GetComponent(kTextureId));
        if (m_pPlayerTexture == nullptr)
        {
            LOG_FATAL("Unable to acquire AnimationComponent");
            return false;
        }

        m_pPlayerAnim = static_cast<AnimationComponent*>(pActor->GetComponent(kAnimationId));
        if (m_pPlayerAnim == nullptr)
        {
            LOG_FATAL("Unable to acquire AnimationComponent");
            return false;
        }

        m_pStateMachine = static_cast<MarioStateMachine*>(pActor->GetComponent(kMarioState));
        if (m_pStateMachine == nullptr)
        {
            LOG_FATAL("Unable to acquire MarioStateMachine");
            return false;
        }

        m_pAudioSource = static_cast<AudioSource*>(pActor->GetComponent(kAudioSourceId));
        if(m_pAudioSource == nullptr)
        {
            LOG_FATAL("Unable to acquire AudioSource: PlayerView");
            return false;
        }

        m_pAttack = static_cast<Attackable*>(pActor->GetComponent(kAttackableId));
        if (m_pAttack == nullptr)
        {
            LOG_FATAL("Unable to finc Attackable from Mario");
            return false;
        }

        m_pHp = static_cast<HealthComponent*>(pActor->GetComponent(kHealthId));
        if (m_pHp == nullptr)
        {
            LOG_FATAL("Unable to finc HealthComponent from Mario");
            return false;
        }

        Camera.SetPosition((m_pPlayerTransform->GetPosition().m_x), (m_pPlayerTransform->GetPosition().m_y));
    }
   
    m_listenerIds[CreateActorAtLocationEvent::kEventType] 
        = m_pGameLayer->GetEventManager().AddEventListener(CreateActorAtLocationEvent::kEventType, std::bind(&PlayerView::SpwanFireBall, this, std::placeholders::_1));
    m_listenerIds[PickUpItemEvent::kEventType] 
        = m_pGameLayer->GetEventManager().AddEventListener(PickUpItemEvent::kEventType, std::bind(&PlayerView::PickUpItem, this, std::placeholders::_1));
    m_listenerIds[TeleportActorEvent::kEventType] 
        = m_pGameLayer->GetEventManager().AddEventListener(TeleportActorEvent::kEventType, std::bind(&PlayerView::TeleportTo, this, std::placeholders::_1));
    m_listenerIds[SetCameraToPositionEvent::kEventType] 
        = m_pGameLayer->GetEventManager().AddEventListener(SetCameraToPositionEvent::kEventType, std::bind(&PlayerView::SetCameraToLocation, this, std::placeholders::_1));
    m_listenerIds[ReachGoalEvent::kEventType] 
        = m_pGameLayer->GetEventManager().AddEventListener(ReachGoalEvent::kEventType, std::bind(&PlayerView::ReachGoal, this, std::placeholders::_1));

    auto& scripting = m_pGameLayer->GetScriptingManager();
    scripting.GetGlobal("g_logic");
    scripting.GetFromTable("actors");
    scripting.GetFromTable(GetActor()->GetId());
    scripting.CreateTable();
    scripting.AddToTable("this", this);
    scripting.AddToTable("IsGround", Lua::PlayerViewIsGround);
    scripting.AddToTable("SetGrounded", Lua::PlayerViewSetGrounded);
    scripting.AddToTable("SpawnFireBall", Lua::PlayerSpawnFireBall);
    scripting.AddToTable("GlueCamera", Lua::PlayerGlueCamera);
    scripting.AddToTable("SetControlable", Lua::PlayerSetControlable);
    scripting.AddToTable("View");
    scripting.PopAll();

    m_pGameOverTexture = m_pGraphics->LoadTextureFromCache("Images/UI/GameOver.png");
    m_pGameClearTexture = m_pGraphics->LoadTextureFromCache("Images/UI/GameClear.png");

    return true;
}

void PlayerView::UpdateInput(float delta)
{
    auto pGameLayer = static_cast<GameLogic*>(ApplicationLayer::GetInstance()->GetGameLayer());
    auto& scripting = pGameLayer->GetScriptingManager();
    
    if (m_pKeyboard->IsKeyPressed(IKeyboard::kR))
    {
        m_pStateMachine->ChangeState("Idle");
        m_pStateMachine->ChangeState("SmallState");
        m_pStateMachine->HandleInput();
        m_pPlayerComponent->ReDimension(20, 20, "Body", Vector2<float>(0, 0));
        m_pPlayerComponent->ReDimension(15, 15, "Bottom", Vector2<float>(0, 18));
        m_pPlayerComponent->ReDimension(15, 7, "Head", Vector2<float>(0, -18));

        scripting.StartFunction("StartNewGame");
        scripting.CallFunction(0);
        m_pAudio->ResumeMusic();
    }
    
    if (m_pKeyboard->IsKeyPressed(IKeyboard::kQ))
    {
        scripting.StartFunction("ResetLevel");
        scripting.CallFunction(0);
    }

    if (m_pHp->GetHP() == 0)
    {
        if (m_controlable)
        {
            m_pAudio->PauseMusic();
            m_pAudio->PlaySoundEffect("Sounds/Env/GameOver.wav", 128);
            m_controlable = false;
        }
    }

    if (m_controlable)
    {
        scripting.StartFunction("UpdateCamera");
        scripting.CallFunction(0);

        //=====================================================================================================================
        //                                              Character Movement
        //=====================================================================================================================
        {
            if (m_pKeyboard->IsKeyDown(IKeyboard::kArrowRight) ||
                m_pKeyboard->IsKeyDown(IKeyboard::kArrowLeft) ||
                m_pKeyboard->IsKeyDown(IKeyboard::kArrowUp) ||
                m_pKeyboard->IsKeyDown(IKeyboard::kArrowDown) ||
                m_pKeyboard->IsKeyDown(IKeyboard::kSpace))
            {
                if (!m_pKeyboard->IsKeyDown(IKeyboard::kCTRL_L))
                {
                    m_pStateMachine->ChangeState("Walk");
                    m_pStateMachine->HandleInput();
                }
            }

            if (m_pKeyboard->IsKeyReleased(IKeyboard::kArrowRight) ||
                m_pKeyboard->IsKeyReleased(IKeyboard::kArrowLeft) ||
                m_pKeyboard->IsKeyReleased(IKeyboard::kArrowUp) ||
                m_pKeyboard->IsKeyReleased(IKeyboard::kArrowDown))
            {
                m_pStateMachine->ChangeState("Idle");
                m_pStateMachine->HandleInput();
            }

            if (m_pKeyboard->IsKeyPressed(IKeyboard::kE))
            {
                m_pPlayerComponent->SetPosition(0, 0);
            }

            // --- Jump ---
            if (m_pKeyboard->IsKeyPressed(IKeyboard::kSpace))
            {

                if (m_isGround)
                {
                    if (m_pStateMachine->GetCurrentStateName() == "SmallState")
                    {
                        m_pAudioSource->PlayClip("Mario_Jump_Small");
                    }
                    else
                    {
                        m_pAudioSource->PlayClip("Mario_Jump_Super");
                    }
                    m_keepPressingSpace = true;
                }
                m_isGround = false;
            }

            if (m_pKeyboard->IsKeyDown(IKeyboard::kSpace))
            {
                if (m_keepPressingSpace == true)
                {
                    m_jumpingTimeCount += delta;

                    scripting.StartFunction("JumpPlayer");
                    scripting.PushNumber(GetActor()->GetId());
                    scripting.PushBoolean(m_isGround);
                    scripting.PushNumber(m_jumpingTimeCount);
                    scripting.CallFunction(0);
                }
            }

            if (m_pKeyboard->IsKeyReleased(IKeyboard::kSpace))
            {
                m_jumpingTimeCount = 0;
                m_keepPressingSpace = false;
            }
        }
        //=====================================================================================================================
        //                                             Change Level
        //=====================================================================================================================
#if defined(DEBUG)
        {
            //if (m_pKeyboard->IsKeyDown(IKeyboard::kN))
            {
                if (m_pKeyboard->IsKeyPressed(IKeyboard::kKeypad1))
                {
                    scripting.StartFunction("ChangeLevel");
                    scripting.PushNumber(1);
                    scripting.CallFunction(0);
                }
                else if (m_pKeyboard->IsKeyPressed(IKeyboard::kKeypad2))
                {
                    scripting.StartFunction("ChangeLevel");
                    scripting.PushNumber(2);
                    scripting.CallFunction(0);
                }
                else if (m_pKeyboard->IsKeyPressed(IKeyboard::kKeypad3))
                {
                    scripting.StartFunction("ChangeLevel");
                    scripting.PushNumber(3);
                    scripting.CallFunction(0);
                }
                else if (m_pKeyboard->IsKeyPressed(IKeyboard::kKeypad4))
                {
                    scripting.StartFunction("ChangeLevel");
                    scripting.PushNumber(4);
                    scripting.CallFunction(0);
                }
                else if (m_pKeyboard->IsKeyPressed(IKeyboard::kKeypad5))
                {
                    scripting.StartFunction("ChangeLevel");
                    scripting.PushNumber(5);
                    scripting.CallFunction(0);
                }
                else if (m_pKeyboard->IsKeyPressed(IKeyboard::kKeypad8))
                {
                    m_pStateMachine->ChangeState("Idle");
                    m_pStateMachine->ChangeState("SmallState");
                    m_pStateMachine->HandleInput();
                    m_pPlayerComponent->ReDimension(20, 20, "Body", Vector2<float>(0, 0));
                    m_pPlayerComponent->ReDimension(15, 15, "Bottom", Vector2<float>(0, 18));
                    m_pPlayerComponent->ReDimension(15, 7, "Head", Vector2<float>(0, -18));
                }
                else if (m_pKeyboard->IsKeyPressed(IKeyboard::kKeypad9))
                {
                    m_pStateMachine->ChangeState("Idle");
                    m_pStateMachine->ChangeState("SuperState");
                    m_pStateMachine->HandleInput();
                    m_pPlayerComponent->ReDimension(20, 40, "Body", Vector2<float>(0, 0));
                    m_pPlayerComponent->ReDimension(15, 30, "Bottom", Vector2<float>(0, 18));
                    m_pPlayerComponent->ReDimension(15, 14, "Head", Vector2<float>(0, -30));
                }
            }
        }

        //=====================================================================================================================
        //                                             Camera Movement
        //=====================================================================================================================
        {
            if (m_pKeyboard->IsKeyDown(IKeyboard::kCTRL_L))
            {
                if (m_pKeyboard->IsKeyDown(IKeyboard::kArrowRight))
                {
                    scripting.StartFunction("MoveCamera");
                    scripting.PushNumber(10);
                    scripting.PushNumber(0);
                    scripting.CallFunction(0);
                }

                if (m_pKeyboard->IsKeyDown(IKeyboard::kArrowLeft))
                {
                    scripting.StartFunction("MoveCamera");
                    scripting.PushNumber(-10);
                    scripting.PushNumber(0);
                    scripting.CallFunction(0);
                }

                if (m_pKeyboard->IsKeyDown(IKeyboard::kArrowUp))
                {
                    scripting.StartFunction("MoveCamera");
                    scripting.PushNumber(0);
                    scripting.PushNumber(-10);
                    scripting.CallFunction(0);
                }

                if (m_pKeyboard->IsKeyDown(IKeyboard::kArrowDown))
                {
                    scripting.StartFunction("MoveCamera");
                    scripting.PushNumber(0);
                    scripting.PushNumber(10);
                    scripting.CallFunction(0);
                }

                if (m_pKeyboard->IsKeyDown(IKeyboard::kL))
                {
                    scripting.StartFunction("SetCameraPos");
                    scripting.PushNumber(0);
                    scripting.PushNumber(0);
                    scripting.CallFunction(0);
                }
            }
        }
#endif

        //=====================================================================================================================
        //                                             Creating Objects
        //=====================================================================================================================
        {
            if (m_pKeyboard->IsKeyPressed(IKeyboard::kZ))
            {
                if (m_canFire)
                {
                    m_pGameLayer->GetEventManager().QueueEvent
                    (
                        std::make_unique<CreateActorAtLocationEvent>
                        (
                            GetActor()->GetId(),
                            Resource("Actors/FireBall.xml"),
                            "MarioFireBall",
                            m_pPlayerTransform->GetPosition()
                        )
                    );
                }
            }
        }

        //=====================================================================================================================
        //                                                  Audio
        //=====================================================================================================================
        {
            // ----- Pressed   -----
            {
                if (m_pKeyboard->IsKeyPressed(IKeyboard::kM))
                {
                    m_pAudio->Mute();
                }
                else if (m_pKeyboard->IsKeyPressed(IKeyboard::kS))
                {
                    m_pAudio->PauseMusic();
                }
                else if (m_pKeyboard->IsKeyPressed(IKeyboard::kR))
                {
                    m_pAudio->ResumeMusic();
                }
            }

            // ----- Motion -----
            {
                if (m_pMouse->IsWheelDown())
                {
                    m_pAudio->DecVolume(3);
                }
                else if (m_pMouse->IsWheelUp())
                {
                    m_pAudio->IncVolume(3);
                }
            }
        }
    m_processManager.UpdateProcesses(delta);
    }
}

void PlayerView::ViewScene()
{
    m_pGraphics->EndDrawing();
    m_pGraphics->StartDrawing();

    if (m_controlable)
    {
        for (auto& actorPair : m_pGameLayer->GetActors())
        {
            actorPair.second->Render(m_pGraphics);
        }

        for (auto& actorPair : m_pGameLayer->GetGUIs())
        {
            actorPair.second->Render(m_pGraphics);
        }
    }
    else
    {
        if (m_pHp->GetHP() > 0)
        {
            m_pGraphics->DrawTexture(m_pGameClearTexture.get(), RenderMode::kRenderFull);
        }
        else
        {
            m_pGraphics->DrawTexture(m_pGameOverTexture.get(), RenderMode::kRenderFull);
        }
    }
}

void PlayerView::Delete()
{
    m_processManager.AbortAllProcesses();
}

void PlayerView::OnCollisionEnter(Collision& col)
{
    //Actor* pOther = pRhs->GetOwner();
    Actor* pOther = col.m_pActor;
    TagComponent* pTag = static_cast<TagComponent*>(pOther->GetComponent(kTagId));

    if(pTag)
    {
        const std::string_view tagName = pTag->GetTag();
        if (tagName == "Projectile")
        {
            if (pOther->IsName("PlayerFireball"))
                return;

            const std::string& stateName = m_pStateMachine->GetCurrentStateName();
            if (stateName.find("Super") != std::string::npos)
            {
                m_pStateMachine->ChangeState("SmallState");
                m_pStateMachine->HandleInput();
                m_pAudioSource->PlayClip("PipeAndPowerDown");
            }
            else
            {
                m_pGameLayer->GetEventManager().QueueEvent(std::make_unique<TeleportActorEvent>(GetActor()->GetId(), 0, 250));
            }

            m_pGameLayer->GetEventManager().TriggerEvent(std::make_unique<DestroyActorEvent>(pOther->GetId()));
        }
        else if (tagName == "Item")
        {
            m_pGameLayer->GetEventManager().QueueEvent(std::make_unique<PickUpItemEvent>(GetActor()->GetId(), pOther->GetId()));
        }
    }

    auto& scripting = ApplicationLayer::GetInstance()->GetGameLayer()->GetScriptingManager();
    scripting.StartFunction("PlayerOnCollisionEnter");
    scripting.PushNumber(GetActor()->GetId());
    scripting.PushNumber(pOther->GetId());
    scripting.PushString(pOther->GetName().c_str());
    scripting.PushString(col.m_userData.c_str());
    scripting.CallFunction(0);
}

void PlayerView::OnCollisionExit(Collision& col)
{
} 

void PlayerView::OnTriggerEnter(Bel::Collision& col)
{
    auto& scripting = ApplicationLayer::GetInstance()->GetGameLayer()->GetScriptingManager();

    Actor* pOther = col.m_pActor;
    TagComponent* pTag = static_cast<TagComponent*>(pOther->GetComponent(kTagId));

    if (pTag)
    {
        const std::string_view tagName = pTag->GetTag();
        
        if (col.m_userData == "Head")
        {
            if (tagName == "Tile")
            {
                auto pDelay = std::make_shared<DelayProcess>(0.15f);
                Actor::Id otherId = pOther->GetId();

                pDelay->SetSuccessCallback
                (
                    [this, otherId]()
                    {
                        m_pGameLayer->GetEventManager().QueueEvent(std::make_unique<DestroyActorEvent>(otherId));
                        m_pAudioSource->PlayClip("BrickSmash");
                    }
                );
                m_processManager.AttachProcess(pDelay);
            }
            else if (tagName == "Crate")
            {
                CrateComponent* pCrateComp = static_cast<CrateComponent*>(pOther->GetComponent(kCrateId));
                if (pCrateComp->IsEmpty())
                {
                    m_pAudio->PlaySoundEffect("Sounds/Env/Bump.wav");
                    return;
                }

                pCrateComp->Empty();
                std::string xmlFile = "Actors/" + pCrateComp->GetItemName() + ".xml";

                AnimationComponent* pCrateAnim = static_cast<AnimationComponent*>(pOther->GetComponent(kAnimationId));
                if (pCrateAnim)
                {
                    pCrateAnim->SetAnimation("Empty");
                }

                TransformComponent* pCrateTransform = static_cast<TransformComponent*>(pOther->GetComponent(kTransformId));
                if (pCrateTransform)
                {
                    scripting.StartFunction("SpawnItem");
                    scripting.PushString(xmlFile.c_str());
                    scripting.PushString(pCrateComp->GetItemName().c_str());
                    scripting.PushNumber(pCrateTransform->GetPosition().m_x);
                    scripting.PushNumber(pCrateTransform->GetPosition().m_y);
                    scripting.CallFunction(0);
                }

                m_pAudio->PlaySoundEffect("Sounds/Env/PowerUpAppear.wav");
            }
        }
    }
    
    if (col.m_userData == "Bottom")
    {
        if (pOther->IsName("Wall") || pOther->IsName("Ground") || pOther->IsName("Crate") || pOther->IsName("Block") || pOther->IsName("Boundary"))
        {
            m_jumpingTimeCount = 0;
            m_isGround = true;
        }
        else if (pOther->IsName("GameClear"))
        {
            m_pGameLayer->GetEventManager().QueueEvent(std::make_unique<ReachGoalEvent>());
        }
    }

    scripting.StartFunction("PlayerOnTriggerEnter");
    scripting.PushNumber(GetActor()->GetId());
    scripting.PushNumber(pOther->GetId());
    scripting.PushString(pOther->GetName().c_str());
    scripting.PushString(col.m_userData.c_str());
    scripting.CallFunction(0);
}

bool PlayerView::IsMoving() const
{
    return m_movingDir.m_right || m_movingDir.m_left || m_movingDir.m_down || m_movingDir.m_up;
}

bool PlayerView::IsGround() const
{
    if (this == nullptr)
        return false;

    return m_isGround;
}

void PlayerView::CreateCameraMoveProcess()
{
    float x = m_pPlayerTransform->GetPosition().m_x;
    float y = m_pPlayerTransform->GetPosition().m_y;
    
    //m_processManager.AbortAllProcesses();
    //m_pCameraMoveProcess = std::make_shared<CameraProcess>(xDest, yDest, time);
}

void PlayerView::GlueCameraToPlayer()
{
    auto pActor = GetActor();
    if (pActor != nullptr)
    {
        auto& scripting = ApplicationLayer::GetInstance()->GetGameLayer()->GetScriptingManager();
        m_pGlueCameraProcess = std::make_shared<GlueCameraToActorProcess>(pActor, &scripting);
        m_processManager.AttachProcess(m_pGlueCameraProcess);
    }
}

void PlayerView::PickUpItem(Bel::IEvent* pEvent)
{
    auto pPickUp = static_cast<PickUpItemEvent*>(pEvent);
    if (GetActor()->GetId() != pPickUp->GetListenerId())
        return;

    auto pItem = m_pGameLayer->GetActor(pPickUp->GetItemId());
    if (!pItem)
        return;


    if (pItem->IsName("Crate"))
    {
        m_pAudioSource->PlayClip("BrickSmash");
    }
    else if (pItem->IsName("Mushroom"))
    {
        auto& scripting = ApplicationLayer::GetInstance()->GetGameLayer()->GetScriptingManager();
        scripting.StartFunction("PickUpItem");
        scripting.PushString(pItem->GetName().c_str());
        scripting.PushNumber(GetActor()->GetId());
        scripting.TraceStack();
        scripting.CallFunction(0);
    }
    else if (pItem->IsName("Flower"))
    {
        m_canFire = true;
    }

    auto pView = m_pGameLayer->FindView(pItem->GetId());
    if (pView)
    {
        pView->Delete();
        m_pGameLayer->RemoveView(pView);
    }

    auto pDelay = std::make_shared<DelayProcess>(0.15f);
    Actor::Id otherId = pItem->GetId();

    pDelay->SetSuccessCallback
    (
        [this, otherId]()
        {
            m_pGameLayer->GetEventManager().QueueEvent(std::make_unique<DestroyActorEvent>(otherId));
        }
    );
    m_processManager.AttachProcess(pDelay);
}

void PlayerView::TeleportTo(Bel::IEvent* pEvent)
{
    auto pTeleportTo = static_cast<TeleportActorEvent*>(pEvent);

    if (GetActor()->GetId() != pTeleportTo->GetId())
        return;

    m_pPlayerTransform->SetPosition(static_cast<float>(pTeleportTo->GetTargetPosition().m_x), static_cast<float>(pTeleportTo->GetTargetPosition().m_y));
}

void PlayerView::ReachGoal(Bel::IEvent* pEvent)
{
    auto actors = m_pGameLayer->GetActors();

    for (auto actor : actors)
    {
        if (actor.second->GetId() == GetActor()->GetId())
            continue;
        m_pGameLayer->GetEventManager().QueueEvent(std::make_unique<DeleteViewAndActorEvent>(actor.second->GetId()));
    }
    SetControlable(false);
    m_pAudio->PauseMusic();
    m_pAudio->PlaySoundEffect("Sounds/Env/GameClear.mp3", 64);
}

void PlayerView::SetCameraToLocation(Bel::IEvent* pEvent)
{
    auto pSetCamera = static_cast<SetCameraToPositionEvent*>(pEvent);

    if ((GetActor()->GetId() != pSetCamera->GetActorId()))
        return;

    const float& kXPos = pSetCamera->GetPosX();
    const float& kYPos = pSetCamera->GetPosX();
    Camera.SetPosition(kXPos, kYPos);
}

void PlayerView::SpwanFireBall(Bel::IEvent* pEvent)
{
    m_canFire = false;
    auto pSpawnFireBall = static_cast<CreateActorAtLocationEvent*>(pEvent);

    const Actor::Id kActorId    = pSpawnFireBall->GetActorId();

    if (this->GetActor() != nullptr && (GetActor()->GetId() != kActorId))
        return;

    const std::string_view kActorName   = pSpawnFireBall->GetActorName();
    const Vector2<float>&  kPosToSpawn  = pSpawnFireBall->GetPositionToSpawn();
    auto pResource = m_pGameLayer->GetResourceCache()->GetHandle(&pSpawnFireBall->GetResource());

    auto pProjectile = m_pGameLayer->GetActorFactory().CreateActorByResource(pResource);
    pProjectile->SetName(kActorName);

    auto& scripting = m_pGameLayer->GetScriptingManager();
    scripting.StartFunction("AddActorIdToTable");
    scripting.PushNumber(pProjectile->GetId());
    scripting.CallFunction(0);


    m_pGameLayer->AddActor(pProjectile->GetId(), pProjectile);

    auto playerFlip = m_pPlayerTexture->GetRenderFlip();

    std::unique_ptr<IView> pFireBallView = std::unique_ptr<IView>(new FireBallView(playerFlip == RenderFlip::FLIP_HORIZONTAL ? false : true));
    
    if (pFireBallView != nullptr)
    {
        pFireBallView->SetActor(pProjectile);
        pProjectile->RegisterWithScript();
    }

    auto pProjectileDynamic = static_cast<Bel::IDynamicBodyComponent*>(pProjectile->GetComponent(kDynamicBodyId));
    if (!pProjectileDynamic)
    {
        LOG_FATAL("Failed to find IDynamicBodyComponent fromt " + pProjectile->GetName(), false);
        LOG_FATAL(". Message from 'SpwanFireBall() in PlayerView'");
        return;
    }

    m_pAudioSource->PlayClip("Fire");

    pProjectileDynamic->SetPosition(kPosToSpawn.m_x, kPosToSpawn.m_y);

    if (pFireBallView && pFireBallView->Initialize())
    {
        m_pGameLayer->AddView(std::move(pFireBallView));
    }

    auto pCooldown = std::make_shared<DelayProcess>(3.0f);
    pCooldown->SetSuccessCallback
    (
        [this]()
        {
            m_canFire = true;
        }
    );

    m_processManager.AttachProcess(pCooldown);
}

void PlayerView::SetGrounded(bool isGround)
{
    m_isGround = isGround;
}
