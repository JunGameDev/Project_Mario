#pragma once
#include <Events/Processes.h>
#include <Actors/Actor.h>
#include <Core/Camera/Camera.h>
#include <Physics/Physics.h>
#include <Scripting/Scripting.h>

class ParticleComponent;
class AnimationComponent;
class TextureComponent;

// ===== Process =====
class DelayProcess : public Bel::IProcess
{
private:
    float m_delay;

public:
    DelayProcess(const float& timeToDelay)
        : m_delay(timeToDelay)
    {
    }

    virtual ~DelayProcess() {}
    virtual void Update(float delta) override
    {
        m_delay -= delta;
        if (m_delay <= 0.0f)
        {
            Succeeded();
        }
    }
};

class MoveWithAnimProcess : public Bel::IProcess
{
private:
    std::weak_ptr<Bel::Actor> m_pActor;
    Bel::TransformComponent* m_pTransform;
    AnimationComponent* m_pAnimation;

    float m_xDest;
    float m_yDest;
    float m_xSpeed;
    float m_ySpeed;
    float m_time;

public:
    MoveWithAnimProcess(std::weak_ptr<Bel::Actor> pActor, float destX, float destY, float time)
        : m_pActor(pActor)
        , m_pTransform(nullptr)
        , m_xDest(destX)
        , m_yDest(destY)
        , m_xSpeed(0.0f)
        , m_ySpeed(0.0f)
        , m_time(time)
    {

    }

    virtual ~MoveWithAnimProcess() {}
    virtual bool Initialize() override;
    virtual void Update(float delta) override;
};

class MoveProcess : public Bel::IProcess
{
private:
    std::weak_ptr<Bel::Actor> m_pActor;
    Bel::TransformComponent* m_pTransform;

    float m_xDest;
    float m_yDest;
    float m_xSpeed;
    float m_ySpeed;
    float m_time;

public:
    MoveProcess(std::weak_ptr<Bel::Actor> pActor, float destX, float destY, float time)
        : m_pActor(pActor)
        , m_pTransform(nullptr)
        , m_xDest(destX)
        , m_yDest(destY)
        , m_xSpeed(0.0f)
        , m_ySpeed(0.0f)
        , m_time(time)
    {

    }

    virtual ~MoveProcess() {}
    virtual bool Initialize() override;
    virtual void Update(float delta) override;
};

class PatrolProcess : public Bel::IProcess
{
private:
    std::weak_ptr<Bel::Actor> m_pActor;
    const std::string m_kPath;
    Bel::TransformComponent* m_pTransform;
    AnimationComponent* m_pAnimation;
    TextureComponent* m_pTexture;

    int m_times;
    int m_distance;
    
    float m_xDest;
    float m_duration;
    float m_currentDuration;
    float m_xSpeed;

    bool m_movingRight;

public:
    PatrolProcess(std::weak_ptr<Bel::Actor> pActor, const std::string& path)
        : m_pActor(pActor)
        , m_kPath(path)
    {

    }

    virtual ~PatrolProcess() {}
    virtual bool Initialize() override;
    virtual void Update(float delta) override;

private:
    void CalcDestination();
    void CalcMovingSpeed();
};

class NewPatrolProcess : public Bel::IProcess
{
public:
    enum class PatrolType
    {
        kRange,
        kCollision,
        kPoint,
        kNone
    };

    class IPatrolImpl
    {
    protected:
        bool m_movingRight;
        bool m_haveReturnPoint;

    public:
        virtual ~IPatrolImpl() = 0 {};
        virtual Bel::IProcess::State Update(float delta) = 0;
    };

    std::vector<Bel::Vector2<float>> m_points;

private:
    std::weak_ptr<Bel::Actor> m_pActor;
    Bel::IDynamicBodyComponent* m_pActorBody;
    Bel::TransformComponent* m_pActorTransform;
    std::unique_ptr<IPatrolImpl> m_pPatrolImpl;
    PatrolType m_type;
    const char* m_pFileName;
    bool m_movingRight;
    bool m_haveReturnPoint;

    Bel::Vector2<float> m_speed;
    Bel::Vector2<float> m_from;
    Bel::Vector2<float> m_to;

public:
    NewPatrolProcess(std::weak_ptr<Bel::Actor> pActor, const char* pFileName)
        : m_pActor(pActor)
        , m_type(PatrolType::kNone)
        , m_pFileName(pFileName)
        , m_movingRight(false)
        , m_speed({ 0, 0 })
        , m_from({0, 0})
        , m_to({ 0, 0 })
        , m_haveReturnPoint(false)
    {
    }

    NewPatrolProcess(std::weak_ptr<Bel::Actor> pActor, const PatrolType& type, const char* pFileName, bool movingRight)
        : m_pActor(pActor)
        , m_type(type)
        , m_pFileName(pFileName)
        , m_movingRight(movingRight)
        , m_speed({ 0, 0 })
        , m_from({ 0, 0 })
        , m_to({ 0, 0 })
        , m_haveReturnPoint(false)
    {
    }

    virtual ~NewPatrolProcess() override {}
    virtual bool Initialize() override;
    virtual void Update(float delta) override;
};

class JumpProcess : public Bel::IProcess
{
private:
    Bel::TransformComponent* m_pTransform;
    std::weak_ptr<Bel::Actor> m_pActor;

    float m_jumpHeight;
    float m_jumpTime;
    float m_jumpPower;
    float m_gravity;
    int m_posY;

public:
    JumpProcess(std::weak_ptr<Bel::Actor> pActor, float jumpHeight, float jumpPower)
        : m_pActor(pActor)
        , m_jumpHeight(jumpHeight)
        , m_jumpTime(0)
        , m_jumpPower(jumpPower)
        , m_gravity(9.8f)
    {

    }

    virtual void Update(float delta) override;
    virtual bool Initialize() override;
};

class CameraProcess : public Bel::IProcess
{
private:
    float m_xDest;
    float m_yDest;
    float m_time;
    float m_xSpeed;

    float m_ySpeed;
    Bel::Camera2D* m_pCamera;

public:
    CameraProcess(float xDest, float yDest, float time)
        : m_xDest(xDest)
        , m_yDest(yDest)
        , m_time(time)
        , m_xSpeed(0.0f)
        , m_ySpeed(0.0f)
    {
    }
    virtual ~CameraProcess() {}
    virtual bool Initialize() override;
    virtual void Update(float delta) override;
};

class ChasingProcess : public Bel::IProcess
{
private:
    Bel::TransformComponent* m_pManipulated;
    Bel::TransformComponent* m_pTarget;
    TextureComponent*   m_pTexture;

    float m_distance;
    float m_radian;

public:
    ChasingProcess(Bel::TransformComponent* pManipulated, Bel::TransformComponent* pTarget, TextureComponent* pManipulatedTexture)
        : m_pManipulated(pManipulated)
        , m_pTarget(pTarget)
        , m_pTexture(pManipulatedTexture)
        , m_distance(0)
        , m_radian(0)
    {

    }

    virtual bool Initialize() override; 
    virtual void Update(float delta) override;

    void CalcRadian();
};

class SpreyConfettiProcess : public Bel::IProcess
{
private:
    int m_size;
    ParticleComponent* m_pParticle;

public:
    SpreyConfettiProcess(ParticleComponent* pParticle, int size)
        : m_pParticle(pParticle)
        , m_size(size)
    {

    }

    virtual ~SpreyConfettiProcess() {}
    virtual bool Initialize() override;
    virtual void Update(float delta) override;
};

class GlueCameraToActorProcess : public Bel::IProcess
{
private:
    std::weak_ptr<Bel::Actor> m_pActor;
    Bel::TransformComponent* m_pTransform;
    Bel::Camera2D& m_camera;
    Bel::ScriptingManager* m_pScripting;

public:
    GlueCameraToActorProcess(std::weak_ptr<Bel::Actor> pActor, Bel::ScriptingManager* pScripting)
        : m_pActor(pActor)
        , m_pTransform(nullptr)
        , m_camera(Camera)
        , m_pScripting(pScripting)
    {
    }

    virtual ~GlueCameraToActorProcess() {}
    virtual bool Initialize() override;
    virtual void Update(float delta) override;
};

class MoveConstantlyForSecondsProcess : public Bel::IProcess
{
private:
    std::weak_ptr<Bel::Actor> m_pActor;
    Bel::TransformComponent* m_pTransform;
    Bel::IDynamicBodyComponent* m_pBody;
    float m_time;
    const float m_kXSpeed;
    const float m_kYSpeed;

public:
    MoveConstantlyForSecondsProcess(std::weak_ptr<Bel::Actor> pActor, const float& time, const float& xSpeed, const float& ySpeed)
        : m_pActor(pActor)
        , m_pTransform(nullptr)
        , m_pBody(nullptr)
        , m_time(time)
        , m_kXSpeed(xSpeed)
        , m_kYSpeed(ySpeed)
    {
    }

    virtual ~MoveConstantlyForSecondsProcess() override {}
    virtual bool Initialize() override;
    virtual void Update(float delta) override;
};

class RotatingProcess : public Bel::IProcess
{
private:
    std::weak_ptr<Bel::Actor> m_pActor;
    const float m_kSpeed;
    Bel::IDynamicBodyComponent* m_pDynamicBody;

public:
    RotatingProcess(std::weak_ptr<Bel::Actor> pActor, const float& kSpeed)
        : m_pActor(pActor)
        , m_kSpeed(kSpeed)
        , m_pDynamicBody(nullptr)
    {
    }

    virtual ~RotatingProcess() override final {}

    RotatingProcess(const RotatingProcess& other) = default;
    RotatingProcess(RotatingProcess&& other) = default;
    
    RotatingProcess& operator=(const RotatingProcess& other) = default;
    RotatingProcess& operator=(RotatingProcess&& other) = default;

    // Inherited via IProcess
    virtual bool Initialize() override final;
    virtual void Update(float delta) override final;
};