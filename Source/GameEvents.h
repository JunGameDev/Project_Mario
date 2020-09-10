#pragma once
#include <Events/Events.h>
#include <Actors/Actor.h>
#include <ActorType.h>
#include <Resources/Resource.h>
#include <Core/Math/Vector2.h>

class CreateActorEvent : public Bel::IEvent
{
public:
    static constexpr const Bel::EventType kEventType = Bel::MakeGuid("{58A17010-BE8D-4A9E-AADD-3E64321B61D9}");

private:
    Bel::Resource m_resource;
    const std::string   m_kActorName;
    Bel::Vector2<float>  m_spawnPos;

public:
    CreateActorEvent(const Bel::Resource& resource, const char* pActorName, const Bel::Vector2<float>& spawnPos)
        : m_resource(resource)
        , m_kActorName(pActorName)
        , m_spawnPos(spawnPos)
    {
    }
    virtual ~CreateActorEvent() override { }

    virtual Bel::EventType GetEventType()    const override { return kEventType; }
    virtual const std::string_view GetName() const          { return "CreateActor"; }
     
    Bel::Resource GetResource() const       { return m_resource; }
    const std::string& GetActorName() const { return m_kActorName; }
    Bel::Vector2<float>& GetSpawnPos()      { return m_spawnPos; }
};

class DestroyActorEvent : public Bel::IEvent
{
public:
    static constexpr const Bel::EventType kEventType = Bel::MakeGuid("{64C7F3F8-C035-421B-B4B0-FE775D47EAED}");

private:
    Bel::Actor::Id m_id;

public:
    DestroyActorEvent(Bel::Actor::Id id)
        : m_id(id)
    {
    }
    virtual ~DestroyActorEvent() {}

    virtual Bel::EventType GetEventType()    const override { return kEventType; }
    virtual const std::string_view GetName() const          { return "DestroyActor"; }

    Bel::Actor::Id GetId() const { return m_id; }
};

class RemoveViewEvent : public Bel::IEvent
{
public:
    static constexpr const Bel::EventType kEventType = Bel::MakeGuid("{EBE8A2F3-CD21-40D7-8CF3-696F1BF81E02}");

private:
    Bel::IView* m_pView;

public:
    RemoveViewEvent(Bel::IView* pView)
        : m_pView(pView)
    {
    }
    virtual ~RemoveViewEvent() {}

    virtual Bel::EventType GetEventType()    const override { return kEventType; }
    virtual const std::string_view GetName() const { return "RemoveView"; }

    Bel::IView* GetView() { return m_pView; }
};

class DeleteViewAndActorEvent : public Bel::IEvent 
{
public:
    static constexpr const Bel::EventType kEventType = Bel::MakeGuid("{4669B4BC-D2F6-42E5-B71A-6AA708314FB7}");
    
private:
    Bel::Actor::Id m_id;

public:
    DeleteViewAndActorEvent(const Bel::Actor::Id& id)
        : m_id(id)
    {
    }
    virtual ~DeleteViewAndActorEvent() {}

    virtual Bel::EventType GetEventType()    const override { return kEventType; }
    virtual const std::string_view GetName() const { return "DeleteViewAndActorEvent"; }

    Bel::Actor::Id GetId() const { return m_id; }
};

class ChaseTargetEvent : public Bel::IEvent
{
public:
    static constexpr const Bel::EventType kEventType = Bel::MakeGuid("{EF50641A-F9F5-44A8-A2C4-E32107F55414}");
    
    Bel::Actor::Id m_manipulated;
    Bel::Actor::Id m_target;

public:
    ChaseTargetEvent(const Bel::Actor::Id& manipulated, const Bel::Actor::Id& target)
        : m_manipulated(manipulated)
        , m_target(target)
    {
    }
    virtual ~ChaseTargetEvent() {}

    virtual Bel::EventType GetEventType()    const override { return kEventType; }
    virtual const std::string_view GetName() const          { return "ChaseActorEvent"; }

    const Bel::Actor::Id GetManipualtedId() const { return m_manipulated; }
    const Bel::Actor::Id GetTargetId()      const { return m_target; }
};

class TeleportActorEvent : public Bel::IEvent 
{
public:
    static constexpr const Bel::EventType kEventType = Bel::MakeGuid("{082C2A3C-008D-4EC3-8715-EEC7C64CD9DC}");

private:
    Bel::Actor::Id m_id;
    Bel::Vector2<int> m_position;

public:
    TeleportActorEvent(const Bel::Actor::Id& kId, const int& kXPos, const int& kYPos)
        : m_id(kId)
        , m_position(kXPos, kYPos)
    {
    }
    virtual ~TeleportActorEvent() {}

    virtual Bel::EventType GetEventType()    const override { return kEventType; }
    virtual const std::string_view GetName() const          { return "TeleportActor"; }
    Bel::Actor::Id GetId() const { return m_id; }
    Bel::Vector2<int>& GetTargetPosition() { return m_position; }
};

class PickUpItemEvent : public Bel::IEvent
{
public:
    static constexpr const Bel::EventType kEventType = Bel::MakeGuid("{9ABA9B8D-F8D7-4168-9000-45B0B7BC0823}");

private:
    Bel::Actor::Id m_listenerId;
    Bel::Actor::Id m_itemId;

public:
    PickUpItemEvent(const Bel::Actor::Id& listenrId, const Bel::Actor::Id& itemId)
        : m_listenerId(listenrId)
        , m_itemId(itemId)
    {
    }
    virtual ~PickUpItemEvent() {}

    virtual Bel::EventType GetEventType()    const override { return kEventType; }
    virtual const std::string_view GetName() const          { return "PickUpItem"; }

    Bel::Actor::Id GetListenerId() const { return m_listenerId; }
    Bel::Actor::Id GetItemId()     const { return m_itemId; }
};

class CreateActorAtLocationEvent : public CreateActorEvent
{
public:
    static constexpr const Bel::EventType kEventType = Bel::MakeGuid("{5834FF26-2A60-4BDF-ACBD-4AC99AAB629B}");

private:
    const Bel::Actor::Id m_kActorId;
    const Bel::Vector2<float> m_spawnPos;

public:
    CreateActorAtLocationEvent(const Bel::Actor::Id& kActorId, const Bel::Resource& kResource, const char* pActorName, const Bel::Vector2<float>& spawnPos)
        : CreateActorEvent(kResource, pActorName, spawnPos)
        , m_kActorId(kActorId)
        , m_spawnPos(spawnPos)
    {
    }
    virtual ~CreateActorAtLocationEvent() {}

    virtual Bel::EventType GetEventType()    const override { return kEventType; }
    virtual const std::string_view GetName() const          { return "SpawnActorAtTarget"; }

    const Bel::Actor::Id GetActorId() const { return m_kActorId; }
    const Bel::Vector2<float>& GetPositionToSpawn() { return m_spawnPos; }
}; 

class ReachGoalEvent : public Bel::IEvent
{
public:
    static constexpr const Bel::EventType kEventType = Bel::MakeGuid("{D29F6ECE-D958-40F3-A390-300301F3DF9F}");

public:
    ReachGoalEvent()
    {
    }
    virtual ~ReachGoalEvent() {}

    virtual Bel::EventType GetEventType()    const override { return kEventType; }
    virtual const std::string_view GetName() const { return "ReachGoal"; }
};

class SetCameraToPositionEvent : public Bel::IEvent
{
public:
    static constexpr const Bel::EventType kEventType = Bel::MakeGuid("{90FAF4C0-A777-4A4C-8A3A-1B14EEA8275B}");

private:
    const Bel::Actor::Id m_kActorId;
    const float m_kXPos;
    const float m_kYPos;

public:
    SetCameraToPositionEvent(const Bel::Actor::Id& kActorId, const float& xPos, const float& yPos)
        : m_kActorId(kActorId)
        , m_kXPos(xPos)
        , m_kYPos(yPos)
    {
    }
    virtual ~SetCameraToPositionEvent() {}

    virtual Bel::EventType GetEventType()    const override { return kEventType; }
    virtual const std::string_view GetName() const { return "ReachGoal"; }

    const Bel::Actor::Id GetActorId()& { return m_kActorId; }
    const float GetPosX() const { return m_kXPos; }
    const float GetPosY() const { return m_kYPos; }
};

class ChangeLevel : public Bel::IEvent
{
private:
    std::string m_levelName;
    Bel::Vector2<float> m_startPos;
    Bel::Vector2<float> m_cameraPos;

public:
    static constexpr Bel::EventType kEventType = Bel::MakeGuid("{5908C23C-6BD4-4A55-9E0D-D9F27FEAE90D}");

    ChangeLevel(std::string str, Bel::Vector2<float> startPos, Bel::Vector2<float> cameraPos)
        : m_levelName(str)
        , m_startPos(startPos)
        , m_cameraPos(cameraPos)
    {
    }
    virtual ~ChangeLevel() {}
    virtual const std::string_view GetName() const { return "RemovingViewEvent"; }
    virtual Bel::EventType GetEventType() const override { return kEventType; }

    const char* GetLevelName() { return m_levelName.data(); }
    Bel::Vector2<float> GetStartPos() { return m_startPos; }
    Bel::Vector2<float> GetCameraPos() { return m_cameraPos; }
};

