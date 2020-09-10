#include <functional>
#include <algorithm>
#include <Physics/Physics.h>

#include "Game.h"
#include "PlayerView.h"
#include "ComponentHelper.h"
#include "ComponentIDs.h"
#include "ActorType.h"

// --- Views ---
#include "PlayerView.h"
#include "GumbaView.h"
#include "KoopaTroopaView.h"
#include "PiranhaPlantView.h"
#include "BowserView.h"
#include "ProjectileView.h"
#include "PlatformView.h"
#include "MushroomView.h"
#include "FireChainView.h"

// --- Events ---
#include "GameEvents.h"

// --- Resources ---
#include "SoundResource.h"
#include "TextureResource.h"

using namespace Bel;

namespace Lua
{
    //--------------------------------------------------------------------------------------------
    // This will take the XML file for the actor that we want to create.
    //--------------------------------------------------------------------------------------------
    static int CreateActor(lua_State* pState)
    {
        const char* pXML        = luaL_checkstring(pState, 1);
        const char* pActorName  = luaL_checkstring(pState, 2);

        auto  pGameLayer     = static_cast<GameLogic*>(ApplicationLayer::GetInstance()->GetGameLayer());
        auto& actorFactory   = pGameLayer->GetActorFactory();
        auto  pResource      = pGameLayer->GetResourceCache()->GetHandle(&Resource(pXML));
        auto  pActor         = actorFactory.CreateActorByResource(pResource);

        if (pActor == nullptr)
        {
            return -1;
        }
        pActor->SetName(pActorName);
        pGameLayer->AddActor(pActor->GetId(), pActor);

        lua_pop(pState, 1);

        // Add our actor to our actors table
        lua_getglobal(pState, "g_logic");
        lua_getfield(pState, -1, "actors");
        lua_newtable(pState);
        lua_rawseti(pState, -2, pActor->GetId());
        lua_pop(pState, 2);

        //pActor->RegisterWithScript();

        lua_pushinteger(pState, pActor->GetId());
        return 1;
    }

    static int LoadLevel(lua_State* pState)
    {
        std::string level = luaL_checkstring(pState, 1);
        float startX = static_cast<float>(lua_tonumber(pState, 2));
        float startY = static_cast<float>(lua_tonumber(pState, 3));
        float cameraX = static_cast<float>(lua_tonumber(pState, 4));
        float cameraY = static_cast<float>(lua_tonumber(pState, 5));

        lua_pop(pState, 5);

        auto pGameLayer = static_cast<GameLogic*>(ApplicationLayer::GetInstance()->GetGameLayer());
        auto& eventManager = pGameLayer->GetEventManager();
        eventManager.QueueEvent(std::make_unique<ChangeLevel>(level, Vector2<float>(startX, startY), Vector2<float>(cameraX, cameraY)));
        return 0;
    }

    static int CreatePlayerView(lua_State* pState)
    {
        Actor::Id actorId = static_cast<Actor::Id>(luaL_checkinteger(pState, 1));
        lua_pop(pState, 1);

        auto pGameLayer = static_cast<GameLogic*>(ApplicationLayer::GetInstance()->GetGameLayer());
        auto pActor = pGameLayer->GetActor(actorId);
        pActor->RegisterWithScript();

        auto pView = std::make_unique<PlayerView>();
        pView->SetActor(pActor);
        pView->Initialize();

        pGameLayer->AddView(std::move(pView));
        return 0;
    }

    static int SetActorToScript(lua_State* pState)
    {
        auto pGameLayer = static_cast<GameLogic*>(ApplicationLayer::GetInstance()->GetGameLayer());
        auto& actorFactory = pGameLayer->GetActorFactory();
        auto actors = pGameLayer->GetActors();

        Actor::Id id = static_cast<Actor::Id>(luaL_checkinteger(pState, 1));
        lua_pop(pState, 1);

        lua_getglobal(pState, "g_logic");
        lua_getfield(pState, -1, "actors");
        lua_newtable(pState);
        lua_rawseti(pState, -2, id);
        lua_pop(pState, 2);

        lua_pushinteger(pState, id);
        return 0;
    }

    static int DestroyActor(lua_State* pState)
    {
        Actor::Id actorId = static_cast<Actor::Id>(luaL_checkinteger(pState, 1));
        lua_pop(pState, 1);

        auto pGameLayer = ApplicationLayer::GetInstance()->GetGameLayer();
        auto pActor = pGameLayer->GetActor(actorId);

        if (pActor)
        {
            pGameLayer->DestroyActor(actorId);
        }
        return 0;
    }

    static int DestroyView(lua_State* pState)
    {
        Actor::Id actorId = static_cast<Actor::Id>(luaL_checkinteger(pState, 1));
        lua_pop(pState, 1);

        auto pGameLayer = ApplicationLayer::GetInstance()->GetGameLayer();
        pGameLayer->FindViewToDelete(actorId);

        return 0;
    }

    static int CreateView(lua_State* pState)
    {
        auto pGameLayer = static_cast<GameLogic*>(ApplicationLayer::GetInstance()->GetGameLayer());

        Actor::Id actorId = static_cast<Actor::Id>(luaL_checkinteger(pState, 1));
        std::string actorType = static_cast<std::string>(luaL_checkstring(pState, 2));
        lua_pop(pState, 2);

        IView* pView = nullptr;

        if (actorType == "Mario")
        {
            pView = new PlayerView();
        }
        else if (actorType == "Gumba")
        {
            pView = new GumbaView();
        }
        else if (actorType == "KoopaTroopa")
        {
            pView = new KoopaTroopaView();
        }
        else if (actorType == "Bowser")
        {
            pView = new BowserView();
        }

        std::shared_ptr<Actor> pActor = pGameLayer->GetActor(actorId);

        if (!pView)
            return 0;

        pView->SetActor(pActor);
        pView->Initialize();

        pGameLayer->AddView(std::unique_ptr<IView>(pView));
        return 0;
    }

    static int PlaySoundEffect(lua_State* pState)
    {
        auto pApp = ApplicationLayer::GetInstance();
        auto pAudio = pApp->GetAudio();

        const char* pFileName = static_cast<const char*>(lua_tostring(pState, 1));
        int         volume    = static_cast<int>(lua_tonumber(pState, 2));
        lua_pop(pState, 2);

        pAudio->PlaySoundEffect(pFileName, volume);
        return 0;
    }

    static int PlayMusic(lua_State* pState)
    {
        auto pApp = ApplicationLayer::GetInstance();
        auto pAudio = pApp->GetAudio();

        const char* pFileName = static_cast<const char*>(lua_tostring(pState, 1));
        int         volume = static_cast<int>(lua_tonumber(pState, 2));
        lua_pop(pState, 2);

        pAudio->PlayMusic(pFileName, volume);
        return 0;
    }

    static int StopMusic(lua_State* pState)
    {
        auto pApp = ApplicationLayer::GetInstance();
        auto pAudio = pApp->GetAudio();

        pAudio->PauseMusic();
        return 0;
    }

    static int MuteMusic(lua_State* pState)
    {
        auto pApp = ApplicationLayer::GetInstance();
        auto pAudio = pApp->GetAudio();

        pAudio->Mute();
        return 0;
    }

    static int ResumeMusic(lua_State* pState)
    {
        auto pApp = ApplicationLayer::GetInstance();
        auto pAudio = pApp->GetAudio();

        pAudio->ResumeMusic();
        return 0;
    }


    static int ChangeMusic(lua_State* pState)
    {
        auto pApp = ApplicationLayer::GetInstance();
        auto pAudio = pApp->GetAudio();

        const char* pFileName = static_cast<const char*>(lua_tostring(pState, 1));
        int         volume = static_cast<int>(lua_tonumber(pState, 2));
        lua_pop(pState, 2);

        pAudio->ChangeMusic(pFileName, volume);
        return 0;
    }

    static int SpawnItem(lua_State* pState)
    {
        const char* pXmlFile  = static_cast<const char*>(lua_tostring(pState, 1));
        const char* pItemName = static_cast<const char*>(lua_tostring(pState, 2));
        Vector2<float> spawnPos 
        (
            static_cast<float>(lua_tonumber(pState, 3)),
            static_cast<float>(lua_tonumber(pState, 4))
        );

        lua_pop(pState, 4);

        //ResourceCache* pCache = ApplicationLayer::GetInstance()->GetGameLayer()->GetResourceCache();
        //std::shared_ptr<Actor> pActor = ApplicationLayer::GetInstance()->GetGameLayer()->GetActorFactory().CreateActorByResource(pCache->GetHandle(&Resource(pXmlFile)));
        //ApplicationLayer::GetInstance()->GetGameLayer()->AddActor(pActor->GetId(), pActor);
        ApplicationLayer::GetInstance()->GetGameLayer()->GetEventManager().QueueEvent(std::make_unique<CreateActorEvent>(Resource(pXmlFile), pItemName, spawnPos));

        return 0;
    }

}

std::shared_ptr<IResourceLoader> CreateWAVResourceLoader()
{
    return std::make_shared<WaveResourceLoader>();
}

std::shared_ptr<IResourceLoader> CreateJPGResourceLoader()
{
    return std::make_shared<WaveResourceLoader>();
}

/****************************************************************
 *                      Game Logic
 ***************************************************************/
GameLogic::GameLogic(size_t screenWidth, size_t screenHeight)
    : IGameLayer(0, -10.0f)
    , m_resource("resources.bin")
{
    ApplicationLayer::GetInstance()->GetLogging().AddCategory(Logging::SeverityLevel::kLevelFatal, "GameLogic");
    m_camera = Camera2D(static_cast<uint32_t>(screenWidth), static_cast<uint32_t>(screenHeight));

    m_actorFactory.RegisterComponentCreator("BoxRenderComponent", &CreateBoxRenderComponent);
    m_actorFactory.RegisterComponentCreator("TextureComponent",   &CreateTextureComponent);
    m_actorFactory.RegisterComponentCreator("AnimationComponent", &CreateAnimationComponent);
    m_actorFactory.RegisterComponentCreator("AudioSource",        &CreateAudioSourceComponent);
    m_actorFactory.RegisterComponentCreator("MarioStateMachine",  &CreateMarioStateComponent);
    m_actorFactory.RegisterComponentCreator("ButtonComponent",    &CreateButtonComponent);
    m_actorFactory.RegisterComponentCreator("UIImage",            &CreateUIImage);
    m_actorFactory.RegisterComponentCreator("ParticleComponent",  &CreateParticleComponent);
    m_actorFactory.RegisterComponentCreator("TagComponent",       &CreateTagComponent);
    m_actorFactory.RegisterComponentCreator("HealthComponent",    &CreateHealthComponent);
    m_actorFactory.RegisterComponentCreator("Attackable",         &CreateAttackable);
    m_actorFactory.RegisterComponentCreator("DamageComponent",    &CreateDamageComponent);
    m_actorFactory.RegisterComponentCreator("CrateComponent",     &CreateCrateComponent);
}

bool GameLogic::Initialize()
{
    m_pResCache = std::make_unique<ResourceCache>(50, &m_resource);
    if (!m_pResCache->Initialize())
        return false;

    m_pResCache->RegisterLoader(CreateWAVResourceLoader());

    m_eventManager.AddEventListener(CreateActorEvent::kEventType, std::bind(&GameLogic::CreateActorByEvent, this, std::placeholders::_1));
    m_eventManager.AddEventListener(DestroyActorEvent::kEventType, std::bind(&GameLogic::DestroyActorByEvent, this, std::placeholders::_1));
    m_eventManager.AddEventListener(DeleteViewAndActorEvent::kEventType, std::bind(&GameLogic::DeleteViewAndActorByEvent, this, std::placeholders::_1));
    m_eventManager.AddEventListener(ChangeLevel::kEventType, std::bind(&GameLogic::LoadLevel, this, std::placeholders::_1));

    bool result = IGameLayer::Initialize();

    if (!result)
    {
        return result;
    }

    RegisterWithLua();

    auto pScript = m_pResCache->GetHandle(&Resource("Scripts/script.lua"));
    m_scriptingManager.RunScript(pScript);
    
    m_pContactListener = std::make_shared<GameContactListener>();
    GetPhysicsManager().SetContactListener(m_pContactListener);

    return true;
}

void GameLogic::LoadLevel(IEvent* pEvent)
{
    auto pLoadLevel = static_cast<ChangeLevel*>(pEvent);
    m_curLevel = pLoadLevel->GetLevelName();
    Vector2<float> startPos = pLoadLevel->GetStartPos();
    Vector2<float> camera = pLoadLevel->GetCameraPos();

    std::shared_ptr<Actor> pPlayer = nullptr;
    for (auto actor : m_actors)
    {
        if (actor.second->IsName("Mario"))
        {
            pPlayer = actor.second;
            continue;
        }

        auto components = actor.second->GetComponents();
        for (auto compIter = components->begin(); compIter != components->end(); ++compIter)
        {
            compIter->second.reset();
        }
    }
    m_actors.clear();

    std::unique_ptr<IView> pPlayerView = nullptr;
    for (auto& view : m_views)
    {
        if (view->GetActor()->IsName("Mario"))
        {
            pPlayerView = std::move(view);
            pPlayerView->Delete();
            continue;
        }
        view->Delete();
        view.reset();
    } 
    m_views.clear();

    std::unordered_map<Actor::Id, std::shared_ptr<Actor>> actorMap;

    Level level;

    auto pLevelActor = level.LoadTiles(m_pResCache.get(), m_curLevel);
    actorMap[pLevelActor->GetId()] = pLevelActor;
    auto actors = level.LoadObjects(m_pResCache.get(), m_curLevel);

    for (auto& pActor : actors)
    {
        m_scriptingManager.StartFunction("AddActorIdToTable");
        m_scriptingManager.PushNumber(pActor->GetId());
        m_scriptingManager.CallFunction(0);

        std::unique_ptr<IView> pView;

        //----------------------------------------------------------------------------------------
        // I changed from checking actor type with component to actor name.
        // This is for temporary purpose.
        // I'll switch it to checking component way later.
        //----------------------------------------------------------------------------------------
        if (pActor->IsName("Mario"))
        {
            pView = std::unique_ptr<IView>(new PlayerView());
        }
        else if (pActor->IsName("Gumba"))
        {
            pView = std::unique_ptr<IView>(new GumbaView());
        }
        else if (pActor->IsName("KoopaTroopa"))
        {
            pView = std::unique_ptr<IView>(new KoopaTroopaView());
        }
        else if (pActor->IsName("Bowser"))
        {
            pView = std::unique_ptr<IView>(new BowserView());
        }
        else if (pActor->IsName("MovingUpPlatform"))
        {
            pView = std::unique_ptr<IView>(new UpPlatformView());
        }
        else if (pActor->IsName("MovingDownPlatform"))
        {
            pView = std::unique_ptr<IView>(new DownPlatformView());
        }
        else if (pActor->IsName("ClockWiseFireChain"))
        {
            pView = std::unique_ptr<IView>(new ClockWiseFireChainView());
        }
        else if (pActor->IsName("CounterClockWiseFireChain"))
        {
            pView = std::unique_ptr<IView>(new CounterClockWiseFireChainView());
        }

        if (pView != nullptr)
        {
            pView->SetActor(pActor);
            pActor->RegisterWithScript();
        }

        actorMap[pActor->GetId()] = pActor;

        if (pView && pView->Initialize())
        {
            AddView(std::move(pView));
        }
    }

    auto pPlayerBody = static_cast<IDynamicBodyComponent*>(pPlayer->GetComponent(kDynamicBodyId));
    if (pPlayerBody == nullptr)
    {
        LOG("Failed to get body component from player when loading a level");
        return;
    }
    pPlayerBody->SetPosition(startPos.m_x, startPos.m_y);

    actorMap[pPlayer->GetId()] = pPlayer;
    AddView(std::move(pPlayerView));
    m_actors = actorMap;

    Camera.SetPosition(camera.m_x, camera.m_y);
}

void GameLogic::RegisterWithLua()
{
    IGameLayer::RegisterWithLua();
    m_scriptingManager.GetGlobal("g_logic");
    
    m_scriptingManager.AddToTable("CreateActor",        Lua::CreateActor);
    m_scriptingManager.AddToTable("CreatePlayerView",   Lua::CreatePlayerView);
    m_scriptingManager.AddToTable("CreateView",         Lua::CreateView);
    m_scriptingManager.AddToTable("DestroyActor",       Lua::DestroyActor);
    m_scriptingManager.AddToTable("DestroyView",        Lua::DestroyView);
    m_scriptingManager.AddToTable("LoadLevel",          Lua::LoadLevel);
    m_scriptingManager.AddToTable("SetActorsToScript",  Lua::SetActorToScript);
    m_scriptingManager.AddToTable("PlaySoundEffect",    Lua::PlaySoundEffect);
    m_scriptingManager.AddToTable("SpawnItem",          Lua::SpawnItem);
    m_scriptingManager.AddToTable("ChangeMusic",        Lua::ChangeMusic);
    m_scriptingManager.AddToTable("StopMusic",          Lua::StopMusic);
    m_scriptingManager.AddToTable("PlayMusic",          Lua::PlayMusic);
    m_scriptingManager.AddToTable("MuteMusic",          Lua::MuteMusic);
    m_scriptingManager.AddToTable("ResumeMusic",        Lua::ResumeMusic);

    m_camera.RegisterWithLua();
    auto& pLogging = ApplicationLayer::GetInstance()->GetLogging();
    pLogging.RegisterWithScript();
    ApplicationLayer::GetInstance()->RegisterWithLua();
}

void GameLogic::CreateActor(const ActorType& type)
{
    std::shared_ptr<Actor> pActor = nullptr;
    IView* pView = nullptr;

    switch (type)
    {   
    case ActorType::kMario:
    {
        pActor = m_actorFactory.CreateActorByFileName("Actors/Mario.xml");
        pActor->SetName("Mario");
        pView = new PlayerView();
    }
        break;
    case ActorType::kGumba:
    {
        pActor = m_actorFactory.CreateActorByFileName("Actors/Gumba.xml");
        pActor->SetName("Gumba");
        pView = new GumbaView();
    }
        break;
    case ActorType::kKoopaTroopa:
    {
        pActor = m_actorFactory.CreateActorByFileName("Actors/KoopaTroopa.xml");
        pActor->SetName("KoopaTroopa");
        pView = new KoopaTroopaView();
    }
        break;
    default:
        break;
    }
    
    m_actors[pActor->GetId()] = pActor;
    if(pView)
    {
        pView->SetActor(pActor);
        AddView(std::unique_ptr<IView>(pView));
    }
}

void GameLogic::CreateActorByEvent(IEvent* pEvent)
{
    auto pCreateActor = static_cast<CreateActorEvent*>(pEvent);
    Vector2<float> spawnPos = pCreateActor->GetSpawnPos();
    auto pResource = m_pResCache->GetHandle(&pCreateActor->GetResource());
    
    auto pActor = m_actorFactory.CreateActorByResource(pResource);
    pActor->SetName(pCreateActor->GetActorName());
    std::unique_ptr<IView> pView = nullptr;

    if (pActor->IsName("Mario"))
    {
        pView = std::unique_ptr<IView>(new PlayerView());
    }
    else if (pActor->IsName("Gumba"))
    {
        pView = std::unique_ptr<IView>(new GumbaView());
    }
    else if (pActor->IsName("KoopaTroopa"))
    {
        pView = std::unique_ptr<IView>(new KoopaTroopaView());
    }
    else if (pActor->IsName("Bowser"))
    {
        pView = std::unique_ptr<IView>(new BowserView());
    }
    else if (pActor->IsName("MovingUpPlatform"))
    {
        pView = std::unique_ptr<IView>(new UpPlatformView());
    }
    else if (pActor->IsName("MovingDownPlatform"))
    {
        pView = std::unique_ptr<IView>(new DownPlatformView());
    }
    else if (pActor->IsName("Mushroom"))
    {
        pView = std::make_unique<MushroomView>();
        IDynamicBodyComponent* pBody = static_cast<IDynamicBodyComponent*>(pActor->GetComponent(kDynamicBodyId));
        if (pBody == nullptr)
        {
            LOG("Failed to get IDynamicBodyComponent from mushroom");
        }
        else
        {
            pBody->SetPosition(spawnPos.m_x, spawnPos.m_y);
        }
    }
    else if (pActor->IsName("ExtraLife"))
    {
        IDynamicBodyComponent* pBody = static_cast<IDynamicBodyComponent*>(pActor->GetComponent(kDynamicBodyId));
        if (pBody == nullptr)
        {
            LOG("Failed to get IDynamicBodyComponent from mushroom");
        }
        else
        {
            pBody->SetPosition(spawnPos.m_x, spawnPos.m_y);
        }
    }
    else if (pActor->IsName("Flower"))
    {
        IDynamicBodyComponent* pBody = static_cast<IDynamicBodyComponent*>(pActor->GetComponent(kDynamicBodyId));
        if (pBody == nullptr)
        {
            LOG("Failed to get IDynamicBodyComponent from mushroom");
        }
        else
        {
            pBody->SetPosition(spawnPos.m_x, spawnPos.m_y);
        }
    }

    m_actors[pActor->GetId()] = pActor;
    m_scriptingManager.StartFunction("AddActorIdToTable");
    m_scriptingManager.PushNumber(pActor->GetId());
    m_scriptingManager.CallFunction(0);
    
    if (!pView)
        return;

    pView->SetActor(pActor);
    pActor->RegisterWithScript();
    
    if (pView->Initialize())
    {
        AddView(std::move(pView));
    }
}

void GameLogic::DestroyActorByEvent(IEvent* pEvent)
{
    auto pDestroyActor = static_cast<DestroyActorEvent*>(pEvent);
    auto pActor = GetActor(pDestroyActor->GetId());
    if (pActor == nullptr)
        return;
    DestroyActor(pDestroyActor->GetId());
}

void GameLogic::DeleteView(IEvent* pEvent)
{
    auto pDeleteView = static_cast<RemoveViewEvent*>(pEvent);
    auto pViewToRemove = pDeleteView->GetView();
    if (pViewToRemove == nullptr)
        return;
    RemoveView(pViewToRemove);
}

void GameLogic::DeleteViewAndActorByEvent(Bel::IEvent* pEvent)
{
    auto pDeletion = static_cast<DeleteViewAndActorEvent*>(pEvent);
    auto pActor = GetActor(pDeletion->GetId());
    Bel::Actor::Id actorToDelete = pDeletion->GetId();

    ApplicationLayer::GetInstance()->GetGameLayer()->FindViewToDelete(actorToDelete);
    DestroyActor(actorToDelete);
}

/****************************************************************
 *                      Game App
 ***************************************************************/
std::unique_ptr<IGameLayer> GameApp::CreateGameLayer(size_t width, size_t height)
{
    return std::make_unique<GameLogic>(width, height);
}

bool GameApp::Initialize()
{
    LoadConfig("../Configuration");

    if (!ApplicationLayer::Initialize())
        return false;

    m_logging.SetConfiguration(m_configs);
    /*
    Note:
        Please make any text file in game project folder in order to testing following feature.
    */
    //  m_pSystem->ReadTextFile("TestText.txt");

    return true;
}

/****************************************************************
 *                      GameContactListener
 ***************************************************************/
void GameContactListener::BeginActorCollision(CollisionGroup& group)
{
    auto pGameLayer = ApplicationLayer::GetInstance()->GetGameLayer();
    
    Collision colA = group.m_colA;
    Collision colB = group.m_colB;

    IView* pViewA = pGameLayer->FindView(colA.m_pActor->GetId());
    if (pViewA != nullptr)
    {
        pViewA->OnCollisionEnter(colB);
    }

    IView* pViewB = pGameLayer->FindView(colB.m_pActor->GetId());
    if (pViewB != nullptr)
    {
        pViewB->OnCollisionEnter(colA);
    }
}

void GameContactListener::EndActorCollision(CollisionGroup& group)
{
}

void GameContactListener::BeginActorTrigger(CollisionGroup& group)
{
    auto pGameLayer = ApplicationLayer::GetInstance()->GetGameLayer();

    Collision colA = group.m_colA;
    Collision colB = group.m_colB;

    IView* pViewA = pGameLayer->FindView(colA.m_pActor->GetId());
    if (pViewA)
    {
        pViewA->OnTriggerEnter(colB);
    }

    IView* pViewB = pGameLayer->FindView(colB.m_pActor->GetId());
    if (pViewB)
    {
        pViewB->OnTriggerEnter(colA);
    }
}

void GameContactListener::EndActorTrigger(CollisionGroup& group)
{
    auto pGameLayer = ApplicationLayer::GetInstance()->GetGameLayer();

    Collision colA = group.m_colA;
    Collision colB = group.m_colB;

    IView* pViewA = pGameLayer->FindView(colA.m_pActor->GetId());
    if (pViewA)
    {
        pViewA->OnTriggerExit(colB);
    }

    IView* pViewB = pGameLayer->FindView(colB.m_pActor->GetId());
    if (pViewB)
    {
        pViewB->OnTriggerExit(colA);
    }
}