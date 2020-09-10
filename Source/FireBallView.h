#pragma once
#include <Core/Layers/View.h>

class FireBallView : public Bel::IView
{
private:
    bool m_headingRight;
public:
    FireBallView(bool headingRight)
        : m_headingRight(headingRight)
    {
    }
    virtual ~FireBallView() override
    {

    }
    FireBallView& operator=(const FireBallView& rhs) = delete;

    // Inherited via IView
    virtual bool Initialize() override;
    virtual void UpdateInput(float delta) override;
    virtual void ViewScene() override;
    virtual void Delete() override;
    virtual void OnCollisionEnter(Bel::Collision& col) override;

};

