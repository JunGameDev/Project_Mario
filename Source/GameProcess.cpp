#include <Core/Layers/ApplicationLayer.h>
#include "GameProcess.h"

#include "ComponentIDs.h"
#include "TextureComponent.h"
#include "AnimationComponent.h"
#include "ParticleComponent.h"

#include "GameEvents.h"

using namespace Bel;

/****************************************************************
 *                      MoveProcess
 ***************************************************************/
bool MoveProcess::Initialize()
{
    auto pActor = m_pActor.lock();
    if (pActor == nullptr)
    {
        LOG_FATAL("Unable to acquire actor");
        return false;
    }

    m_pTransform = static_cast<TransformComponent*>(pActor->GetComponent(kTransformId));
    if (m_pTransform == nullptr)
    {
        LOG_FATAL("Unable to get TransformComponent from actor id: ", false);
        LOG_FATAL(std::to_string(pActor->GetId()));
        return false;
    }

    m_xSpeed = (m_xDest - m_pTransform->GetPosition().m_x) / m_time;
    m_ySpeed = (m_yDest - m_pTransform->GetPosition().m_y) / m_time;
    return true;
}

void MoveProcess::Update(float delta)
{
    if (m_pActor.expired())
    {
        Failed();
        return;
    }
    if (m_time <= delta)
    {
        m_pTransform->SetPosition(m_xDest, m_yDest);
        Succeeded();
    }
    else
    {
        m_pTransform->MoveConstantly(m_xSpeed * delta, m_ySpeed * delta);
        m_time -= delta;
    }
}

bool MoveWithAnimProcess::Initialize()
{
    auto pActor = m_pActor.lock();
    if (pActor == nullptr)
    {
        LOG_FATAL("Unable to acquire actor");
        return false;
    }

    m_pTransform = static_cast<TransformComponent*>(pActor->GetComponent(kTransformId));
    if (m_pTransform == nullptr)
    {
        LOG_FATAL("Unable to get TransformComponent from actor id: ", false);
        LOG_FATAL(std::to_string(pActor->GetId()));
        return false;
    }

    m_pAnimation = static_cast<AnimationComponent*>(pActor->GetComponent(kAnimationId));
    if (!m_pAnimation)
    {
        LOG_FATAL("Unable to get AnimationComponent from actor id: ", false);
        LOG_FATAL(std::to_string(pActor->GetId()));
        return false;
    }

    m_xSpeed = (m_xDest - m_pTransform->GetPosition().m_x) / m_time;
    m_ySpeed = (m_yDest - m_pTransform->GetPosition().m_y) / m_time;
    return true;
}

void MoveWithAnimProcess::Update(float delta)
{
    if (m_pActor.expired())
    {
        Failed();
        return;
    }
    if (m_time <= delta)
    {
        m_pTransform->GetPosition().m_x = m_xDest;
        m_pTransform->GetPosition().m_y = m_yDest;
        int temp = rand() % 2;
        
        if (temp == 0)
        {
            m_pAnimation->SetAnimation("Walk");
        }
        else
        {
            m_pAnimation->SetAnimation("Jump");
        }

        Succeeded();
    }
    else
    {

        m_pTransform->MoveConstantly(m_xSpeed * delta, m_ySpeed * delta);
        m_time -= delta;
    }
}

/****************************************************************
 *                      PatrolProcess
 ***************************************************************/
bool PatrolProcess::Initialize()
{
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError error = doc.LoadFile(m_kPath.data());

    if (error != tinyxml2::XML_SUCCESS)
    {
        LOG_FATAL("Unable to load data file: ", false);
        return false;
    }

    auto pRoot = doc.FirstChildElement();
    if (!pRoot)
    {
        LOG_FATAL("Unable to find root");
        return false;
    }

    auto pElement = pRoot->FirstChildElement("Data");
    if (pElement)
    {
        m_times = pElement->IntAttribute("times");
        m_distance = pElement->IntAttribute("distance");
        m_currentDuration = m_duration = pElement->FloatAttribute("duration");
    }

    auto pActor = m_pActor.lock();
    if (pActor == nullptr)
    {
        LOG_FATAL("Unable to acquire actor");
        return false;
    }

    m_pTransform = static_cast<TransformComponent*>(pActor->GetComponent(kTransformId));
    if (!m_pTransform)
    {
        LOG_FATAL("Failed to get TransformComponent : PatrolProcess");
        return false;
    }

    m_pAnimation = static_cast<AnimationComponent*>(pActor->GetComponent(kAnimationId));
    if (m_pAnimation)
    {
        m_pAnimation->SetAnimation("Walk");
    }

    m_pTexture = static_cast<TextureComponent*>(pActor->GetComponent(kTextureId));
    if (!m_pTexture)
    {
        LOG_FATAL("Failed to get TextureComponent: PatrolProcess");
        return false;
    }

    switch (m_pTexture->GetRenderFlip())
    {
    case RenderFlip::FLIP_HORIZONTAL:
        m_xSpeed = static_cast<float>(m_pTransform->GetSpeed());
        break;
    case RenderFlip::FLIP_NONE:
        m_xSpeed = static_cast<float>(-m_pTransform->GetSpeed());
        break;
    }

    CalcDestination();
    CalcMovingSpeed();

    return true;
}

void PatrolProcess::Update(float delta)
{
    if (m_pActor.expired() || !m_pAnimation)
    {
        Failed();
        return;
    }

    if (m_currentDuration <= delta)
    {
        m_pTransform->GetPosition().m_x = m_xDest;
        
        m_movingRight       = !m_movingRight;
        m_currentDuration   = m_duration;
        
        --m_times;

        if (m_pTexture->GetRenderFlip() == RenderFlip::FLIP_NONE)
        {
            m_pTexture->SetRenderFlip(RenderFlip::FLIP_HORIZONTAL);
        }
        else if (m_pTexture->GetRenderFlip() == RenderFlip::FLIP_HORIZONTAL)
        {
            m_pTexture->SetRenderFlip(RenderFlip::FLIP_NONE);
        }

        CalcDestination();
        CalcMovingSpeed();
    }

    if (m_times <= 0)
    {
        m_pAnimation->SetAnimation("Idle");
        Succeeded();
    }
    else
    {
        m_pAnimation->SetAnimation("DanceTypeA");
        m_pTransform->Move(m_xSpeed * delta, 0);
        m_currentDuration -= delta;
    }
}

void PatrolProcess::CalcDestination()
{
    const int adjust = m_movingRight ? 1 : -1;

    m_xDest = m_pTransform->GetPosition().m_x + (m_distance * adjust);
}

void PatrolProcess::CalcMovingSpeed()
{
    m_xSpeed = ((m_xDest - m_pTransform->GetPosition().m_x) / m_duration) / m_pTransform->GetSpeed();
}

/****************************************************************
 *                      JumpProcess
 ***************************************************************/
bool JumpProcess::Initialize()
{
    auto pActor = m_pActor.lock();
    if (pActor == nullptr)
    {
        LOG_FATAL("Unable to acquire actor");
        return false;
    }

    m_pTransform = static_cast<TransformComponent*>(pActor->GetComponent(kTransformId));
    if (!m_pTransform)
    {
        LOG_FATAL("Unable to acquire TransformComponent from actor");
        return false;
    }

    m_posY = static_cast<int>(m_pTransform->GetPosition().m_y);

    return true;
}

void JumpProcess::Update(float delta)
{
    if (m_pActor.expired())
    {
        Failed();
        return;
    }
    
    m_jumpHeight += (m_jumpTime * m_jumpTime * (-m_gravity)) + (m_jumpPower * m_jumpTime);
    m_jumpTime += delta;

    float jumpDelta = -m_jumpHeight * delta;

    //if (m_pTransform->GetPosition().m_y >= m_posY)

    if (m_jumpTime >= 1.0f)
    {
        m_pTransform->GetPosition().m_y = 3;
        //m_pTransform->GetPosition().m_y = static_cast<float>(m_posY);
        Succeeded();
    }
}

/****************************************************************
 *                      CameraProcess
 ***************************************************************/
bool CameraProcess::Initialize()
{
    m_pCamera = &Camera;
    if (!m_pCamera)
    {
        LOG_ERROR("Failed to get camera: Camera Process");
        return false;
    }
    m_xSpeed = (m_xDest - Camera.GetPosition().m_x) / m_time;
    m_ySpeed = (m_yDest - Camera.GetPosition().m_y) / m_time;

    return true;
}

void CameraProcess::Update(float delta)
{
    if (m_time <= delta)
    {
        Camera.SetPosition(m_xDest, m_yDest);
        Succeeded();
    }
    else
    {
        Camera.MovePos(m_xSpeed * delta, m_ySpeed * delta);
        m_time -= delta;
    }
}

/****************************************************************
 *                      ChasingProcess
 ***************************************************************/
bool ChasingProcess::Initialize()
{
    return true;
}

void ChasingProcess::Update(float delta)
{
    if (m_pTarget)
    {
        CalcRadian();
        if (m_distance < 20.0f)
            m_pManipulated->MoveConstantly(cos(m_radian) * -1 * delta, sin(m_radian) * -1 * delta);
        else
        {
            Failed();
        }
    }

    if (m_pTarget->GetPosition().m_x == m_pManipulated->GetPosition().m_x && m_pTarget->GetPosition().m_y == m_pManipulated->GetPosition().m_y)
    {
        Succeeded();
        return;
    }
}

void ChasingProcess::CalcRadian()
{
    //constexpr float kPi = 3.1415927f;

    // Find the angle θ between point A and point B
    float deltaX = static_cast<float>((m_pManipulated->GetPosition().m_x - m_pTarget->GetPosition().m_x));
    float deltaY = static_cast<float>((m_pManipulated->GetPosition().m_y - m_pTarget->GetPosition().m_y));

    m_distance = Bel::Distance(m_pManipulated->GetPosition(), m_pTarget->GetPosition());
        //std::sqrt((std::fabsf(deltaX) + std::fabsf(deltaY)));
    m_radian = atan2f(deltaY, deltaX);
}

/****************************************************************
 *                      SpreyConfettiProcess
 ***************************************************************/
bool SpreyConfettiProcess::Initialize()
{
    if (!m_pParticle)
        return false;

    m_pParticle->Create(m_size);
    return true;
}

void SpreyConfettiProcess::Update(float delta)
{
    Particle* pParticles = m_pParticle->GetParticles();
    const float kGravity = 9.8f;

    for (int i = 0; i < m_size; ++i)
    {
        Particle* pParticle = (pParticles + i);
        if (pParticle->m_inUse)
        {
            pParticle->m_lifetime -= delta;

            if (pParticle->m_lifetime < 0)
            {
                pParticle->m_inUse = false;
                m_pParticle->DecreaseFreeIndex();
                continue;
            }

            if (pParticle->m_useGravity)
            {
                pParticle->m_yVel += kGravity * delta;
            }

            pParticle->m_rotation += pParticle->m_rotVel * delta;

            pParticle->m_x += pParticle->m_xVel * delta;
            pParticle->m_y += pParticle->m_yVel * delta;
        }
    }

    if (!m_pParticle->IsAnyParticleInUse())
    {
        m_pParticle->SetAllInUse(false);
        Succeeded();
    }
}

/****************************************************************
 *                      GlueCameraToActorProcess
 ***************************************************************/
bool GlueCameraToActorProcess::Initialize()
{
    auto pActor = m_pActor.lock();
    if (pActor == nullptr)
    {
        LOG_FATAL("Failed to lock actor");
        return false;
    }

    m_pTransform = static_cast<TransformComponent*>(pActor->GetComponent(kTransformId));
    if (m_pTransform == nullptr)
    {
        LOG_FATAL("Faild to get TransformComponent from" + pActor->GetName() + ". Message from: GlueCameraToActorProcess, Initialize()");
        return false;
    }

    return true;
}

void GlueCameraToActorProcess::Update(float delta)
{
    if (m_pActor.expired())
    {
        Failed();
        return;
    }

    int temp = static_cast<int>(m_pTransform->GetPosition().m_x - Camera.GetRenderingPoint().m_x);
    if (temp > Camera.GetCenter().m_x)
    {
        m_pScripting->StartFunction("SetCameraPos");
        m_pScripting->PushNumber(m_pTransform->GetPosition().m_x);
        m_pScripting->PushNumber(Camera.GetPosition().m_y);
        m_pScripting->CallFunction(0);
    }
    else
    {
        return;
    }
    
    //m_camera.SetPosition(static_cast<int>(m_pTransform->GetPosition().m_x), static_cast<int>(m_pTransform->GetPosition().m_y));
}

/****************************************************************
 *                      GlueCameraToActorProcess
 ***************************************************************/
bool MoveConstantlyForSecondsProcess::Initialize()
{
    auto pActor = m_pActor.lock();
    if (pActor == nullptr)
    {
        LOG_FATAL("Failed to lock actor");
        return false;
    }

    m_pTransform = static_cast<Bel::TransformComponent*>(pActor->GetComponent(kTransformId));
    if (m_pTransform == nullptr)
    {
        LOG_FATAL("Faild to get TransformComponent from" + pActor->GetName() + ". Message from: MoveConstantlyForSecondsProcess, Initialize()");
        return false;
    }

    m_pBody = static_cast<Bel::IDynamicBodyComponent*>(pActor->GetComponent(kDynamicBodyId));
    if (m_pBody == nullptr)
    {
        LOG_FATAL("Faild to get IDynamicBodyComponent from" + pActor->GetName() + ". Message from: MoveConstantlyForSecondsProcess, Initialize()");
        return false;
    }

    SetSuccessCallback
    (
        [this]()
        {
            auto pActor = m_pActor.lock();
            if (pActor == nullptr)
            {
                LOG_FATAL("Failed to lock actor");
                return;
            }

            ApplicationLayer::GetInstance()->GetGameLayer()->GetEventManager().QueueEvent(std::make_unique<DestroyActorEvent>(pActor->GetId()));
        }
    );

    return true;
}

void MoveConstantlyForSecondsProcess::Update(float delta)
{
    m_time -= delta;
    if (m_time <= 0.0f)
    {
        Succeeded();
    }

    if (m_pActor.expired())
    {
        Failed();
        return;
    }

    m_pBody->SetLinearVelocityX(m_kXSpeed);
}

/****************************************************************
 *                      NewPatrolProcess
 ***************************************************************/
class CollisionPatrol : public NewPatrolProcess::IPatrolImpl
{
private:
    IDynamicBodyComponent* m_pBody;
    float m_xSpeed;
    float m_ySpeed;

public:
    CollisionPatrol()
        : m_pBody(nullptr)
        , m_xSpeed(0)
        , m_ySpeed(0)
    {
    }
    CollisionPatrol(IDynamicBodyComponent* pBody, const float& xSpeed, const float& ySpeed, bool movingRight)
        : m_pBody(pBody)
        , m_xSpeed(xSpeed)
        , m_ySpeed(ySpeed)
    {
        m_movingRight = movingRight;
    }
    virtual ~CollisionPatrol() override
    {
    }

    // Inherited via IPatrolType
    virtual IProcess::State Update(float delta) override
    {
        if (m_pBody)
        {
            if (m_movingRight)
                m_pBody->SetLinearVelocityX(m_xSpeed);
            else
                m_pBody->SetLinearVelocityX(-m_xSpeed);
        }
        else
        {
            return IProcess::State::ABORTED;
        }

        return IProcess::State::RUNNING;
    }
};

class RangePatrol : public NewPatrolProcess::IPatrolImpl
{
private:
    IDynamicBodyComponent* m_pBody;
    TransformComponent* m_pTransform;

    Vector2<float> m_speed;
    Vector2<float> m_from;
    Vector2<float> m_to;

public:
    RangePatrol()
    {
    }
    virtual ~RangePatrol() override
    {
    }

    // Inherited via IPatrolType
    virtual IProcess::State Update(float delta) override
    {
        return IProcess::State::RUNNING;
    }
};

class PointPatrol : public NewPatrolProcess::IPatrolImpl
{
private:
    IDynamicBodyComponent* m_pBody;
    TransformComponent* m_pTransform;

    Vector2<float> m_speed;
    Vector2<float> m_from;
    Vector2<float> m_to;

public:
    PointPatrol()
        : m_pBody(nullptr)
        , m_speed(0, 0)
    {
        m_movingRight = false;
        m_haveReturnPoint = false;
    }

    PointPatrol(IDynamicBodyComponent* pBody, TransformComponent* pTransform, 
            Vector2<float>& speed, Vector2<float>& from, 
            Vector2<float>& to, bool movingRight, bool haveReturnPoint)
        : m_pBody(pBody)
        , m_pTransform(pTransform)
        , m_speed(speed)
        , m_from(from)
        , m_to(to)
    {
        m_movingRight = movingRight;
        m_haveReturnPoint = haveReturnPoint;
    }

    virtual ~PointPatrol() override
    {
    }

    // Inherited via IPatrolType
    virtual IProcess::State Update(float delta) override
    {
        if (m_pBody != nullptr)
        {
            m_pBody->SetLinearVelocity(m_speed.m_x, m_speed.m_y);
        }
        else
        {
            m_pTransform->MoveConstantly(m_speed.m_x, m_speed.m_y);
        }

        if(m_haveReturnPoint)
        {
            
            if (static_cast<int>(m_pTransform->GetPosition().m_x) == static_cast<int>(m_to.m_x)&&
                static_cast<int>(m_pTransform->GetPosition().m_y) == static_cast<int>(m_to.m_y))
            {
                m_pTransform->SetPosition(m_from.m_x, m_from.m_y);
            }
        }

        return IProcess::State::RUNNING;
    }
};

bool NewPatrolProcess::Initialize()
{
    tinyxml2::XMLDocument doc;
    auto pCache = ApplicationLayer::GetInstance()->GetGameLayer()->GetResourceCache();
    auto pResource = pCache->GetHandle(&Resource(m_pFileName));
    tinyxml2::XMLError error = doc.Parse(pResource->GetData().data(), pResource->GetData().size());

    if (error != tinyxml2::XML_SUCCESS)
    {
        LOG_FATAL("Unable to load data file: NewPatrolProcess", false);
        return false;
    }

    auto pRoot = doc.FirstChildElement();
    if (!pRoot)
    {
        LOG_FATAL("Unable to find root");
        return false;
    }

    auto pElement = pRoot->FirstChildElement("Speed");
    if (pElement)
    {
        m_speed.Set(pElement->FloatAttribute("x"), pElement->FloatAttribute("y"));
    }

    pElement = pRoot->FirstChildElement("Type");
    if (pElement)
    {
        const char* pTypeTxt = pElement->GetText();

        if (std::strcmp(pTypeTxt, "Collision") == 0)
        {
            m_type = PatrolType::kCollision;
        }
        else if(std::strcmp(pTypeTxt, "Range") == 0)
        {
            m_type = PatrolType::kRange;
        }
        else if (std::strcmp(pTypeTxt, "Point") == 0)
        {
            m_type = PatrolType::kPoint;
        }
    }

    pElement = pRoot->FirstChildElement("StartingRight");
    if (pElement)
    {
        m_movingRight = pElement->BoolText();
    }

    pElement = pRoot->FirstChildElement("HasReturnPoint");
    if (pElement)
    {
        m_haveReturnPoint = pElement->BoolText();

        pElement = pRoot->FirstChildElement("From");
        if (pElement)
        {
            float x = pElement->FloatAttribute("x");
            float y = pElement->FloatAttribute("y");

            m_from.Set(x, y);
        }

        pElement = pRoot->FirstChildElement("To");
        if (pElement)
        {
            float x = pElement->FloatAttribute("x");
            float y = pElement->FloatAttribute("y");

            m_to.Set(x, y);
        }
    }

    auto pActor = m_pActor.lock();
    if (pActor == nullptr)
    {
        LOG_FATAL("Failed to lock actor");
        return false;
    }

    m_pActorBody = static_cast<IDynamicBodyComponent*>(pActor->GetComponent(kDynamicBodyId));
    if (m_pActorBody == nullptr)
    {
        LOG_FATAL("Faild to get IDynamicBodyComponent from" + pActor->GetName() + ". Message from: NewPatrolProcess, Initialize()");
    }

    m_pActorTransform = static_cast<TransformComponent*>(pActor->GetComponent(kTransformId));
    if (m_pActorTransform == nullptr)
    {
        LOG_FATAL("Faild to get m_pActorTransform from" + pActor->GetName() + ". Message from: NewPatrolProcess, Initialize()");
    }

    switch (m_type)
    {   
    case PatrolType::kRange:
    {
        m_pPatrolImpl = std::make_unique<RangePatrol>();
    }
        break;
    case PatrolType::kCollision:
    {
        m_pPatrolImpl = std::make_unique<CollisionPatrol>(m_pActorBody, m_speed.m_x, m_speed.m_y, m_movingRight);
    }
        break;
    case PatrolType::kPoint:
    {
        m_pPatrolImpl = std::make_unique<PointPatrol>
            (
                m_pActorBody, m_pActorTransform, 
                m_speed, m_from, m_to,
                m_movingRight, m_haveReturnPoint
            );
    }
        break;
    default:
        break;
    }

    return true;
}

void NewPatrolProcess::Update(float delta)
{
    if (m_pActor.expired())
    {
        Aborted();
        return;
    }

    switch (m_type)
    {
    case NewPatrolProcess::PatrolType::kRange:
        break;
    case NewPatrolProcess::PatrolType::kCollision:
        if (!m_pActorBody || !m_pPatrolImpl)
        {
            Failed();
            return;
        }
        break;
    case NewPatrolProcess::PatrolType::kPoint:
        if (!m_pPatrolImpl)
        {
            Failed();
            return;
        }
        break;
    default:
        break;
    }


    auto state = m_pPatrolImpl->Update(delta);
    switch (state)
    {
    case IProcess::State::ABORTED:
        Aborted();
        break;
    default:
        break;
    }
}

bool RotatingProcess::Initialize()
{
    auto pActor = m_pActor.lock();
    if (pActor == nullptr)
    {
        LOG_FATAL("Failed to lock actor");
        return false;
    }

    m_pDynamicBody = static_cast<Bel::IDynamicBodyComponent*>(pActor->GetComponent(kDynamicBodyId));
    if (m_pDynamicBody == nullptr)
    {
        LOG_FATAL("Faild to get IDynamicBodyComponent from" + pActor->GetName() + ". Message from: RotatingProcess, Initialize()");
        return false;
    }

    return true;
}
 
void RotatingProcess::Update(float delta)
{
    if (m_pActor.expired())
    {
        Aborted();
        return;
    }

    float newAngle = m_pDynamicBody->GetAngle() + m_kSpeed * delta;
    m_pDynamicBody->SetRadian(newAngle);
}
