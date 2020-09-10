#pragma once
#include <Actor.h>

class TransformComponent : public Bel::IActorComponent
{
private:
    float   m_x;
    float   m_y;
    float   m_angle;
    int     m_speed;

public:
    TransformComponent(Bel::Actor* pOwner, const char* pName)
        : Bel::IActorComponent(pOwner, pName)
        , m_x(0)
        , m_y(0)
        , m_speed(1)
    {
    }
    virtual ~TransformComponent() {}

    virtual bool Initialize(tinyxml2::XMLElement* pData) override;

    void Move(float x, float y)
    {
        m_x += x * m_speed;
        m_y += y * m_speed;
    }

    void MoveConstant(float x, float y)
    {
        m_x += x;
        m_y += y;
    }

    void SetPosition(float x, float y)
    {
        m_x = x;
        m_y = y;
    }

    float GetX() const { return m_x; }
    float GetY() const { return m_y; }
    
    void SetX(float x) { m_x = x; }
    void SetY(float y) { m_y = y; }

    float GetAngle() const      { return m_angle; }
    void SetAngle(float angle)  { m_angle = angle; }

    const int GetSpeed() { return m_speed; }
    void SetSpeed(int speed) { m_speed = speed; }
};

