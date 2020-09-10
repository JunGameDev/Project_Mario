#pragma once

#include <memory>
#include <Core/Layers/ApplicationLayer.h>
#include <string>
#include <Events/Events.h>
#include <Resources/Resource.h>

#include "ActorType.h"

class GameLogic : public Bel::IGameLayer
{
private:
    Bel::ResourceZlibFile m_resource;
    std::string m_curLevel;

public:
    GameLogic(size_t screenWidth, size_t screenHeight);
    virtual ~GameLogic() {}
    virtual const char* GetGameName() const override { return "Project_Mario"; }
    virtual void LoadLevel(Bel::IEvent* pEvent) override;

    virtual bool Initialize() override;
    virtual void RegisterWithLua() override;

    void CreateActor(const ActorType& type);
    void CreateActorByEvent(Bel::IEvent* pEvent);
    void DestroyActorByEvent(Bel::IEvent* pEvent);
    void DeleteView(Bel::IEvent* pEvent);
    void DeleteViewAndActorByEvent(Bel::IEvent* pEvent);
};

class GameApp : public Bel::ApplicationLayer
{
public:
    GameApp() {}
    virtual ~GameApp() {}
    virtual std::unique_ptr<Bel::IGameLayer> CreateGameLayer(size_t width, size_t height) override;

    virtual bool Initialize() override;
};

/****************************************************************
 *                      GameContactListener
 ***************************************************************/
 //-----------------------------------------------------------------------------------------
 // GameContactListener 
 // 
 // [ Description ] 
 //     Stores all the contacts that started and ended
 //
 // TODO: 
 //-----------------------------------------------------------------------------------------
class GameContactListener : public Bel::IContactListener
{
public:
    virtual void BeginActorCollision(Bel::CollisionGroup& group) override;
    virtual void EndActorCollision(Bel::CollisionGroup& group) override;

    virtual void BeginActorTrigger(Bel::CollisionGroup& group) override;
    virtual void EndActorTrigger(Bel::CollisionGroup& group) override;
};