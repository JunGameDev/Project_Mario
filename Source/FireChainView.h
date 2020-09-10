#pragma once

#include <Core/Layers/ApplicationLayer.h>
#include <Core/Layers/View.h>
#include <Physics/Physics.h>
#include "GameProcess.h"

class IRotation
{
public:
	virtual ~IRotation() = 0 {}
};

class ClockWise : public IRotation
{
public:
	ClockWise() {}
	virtual ~ClockWise() override {}
};

class CounterClockWise : public IRotation
{
public:
	CounterClockWise() {}
	virtual ~CounterClockWise() override {}
};

template<class Type>
class FireChainView : public Bel::IView
{
	static_assert(std::is_base_of<IRotation, Type>::value, "Type must derive from Base");

protected:
	Bel::TransformComponent* m_pTransform;
	Bel::ProcessManager		 m_processManager;
	std::shared_ptr<Bel::IProcess> m_pRotatingProcess;

public:
	FireChainView()
		: m_pTransform(nullptr)
	{
	}
	virtual ~FireChainView() {}

	FireChainView(const FireChainView& other) = default;
	FireChainView& operator=(const FireChainView& other) = default;
	FireChainView(FireChainView&& other) = default;
	FireChainView& operator=(FireChainView&& other) = default;

	// Inherited via IView
	virtual inline bool Initialize() override
	{
		m_pTransform = static_cast<Bel::TransformComponent*>(GetActor()->GetComponent(kTransformId));
		if (m_pTransform == nullptr)
		{
			LOG_FATAL("Failed to acquire TransformComponent from " + GetActor()->GetName());
			return false;
		}

		return true;
	}
	virtual void FireChainView<Type>::UpdateInput(float delta) final
	{
		m_processManager.UpdateProcesses(delta);
	}
	virtual void FireChainView<Type>::ViewScene() final {}
	virtual void FireChainView<Type>::Delete() final
	{
		auto pGameLayer = Bel::ApplicationLayer::GetInstance()->GetGameLayer();
		for (auto [guid, id] : m_listenerIds)
		{
			pGameLayer->GetEventManager().RemoveEventListener(guid, id);
		}

		m_processManager.AbortAllProcesses();
	}
};

class ClockWiseFireChainView : public FireChainView<ClockWise>
{
public:
	ClockWiseFireChainView()
		: FireChainView()
	{
	}
	virtual ~ClockWiseFireChainView() override final
	{
	}

	ClockWiseFireChainView(const ClockWiseFireChainView& other) = default;
	ClockWiseFireChainView(ClockWiseFireChainView&& other) = default;

	ClockWiseFireChainView& operator=(const ClockWiseFireChainView& other) = default;
	ClockWiseFireChainView& operator=(ClockWiseFireChainView&& other) = default;

	// Inherited via IView
	virtual bool Initialize() override final
	{
		if (!FireChainView::Initialize())
			return false;
		
		m_pRotatingProcess = std::make_shared<RotatingProcess>(GetActor(), m_pTransform->GetSpeed());
		m_processManager.AttachProcess(m_pRotatingProcess);
		return true;
	}
};

class CounterClockWiseFireChainView : public FireChainView<CounterClockWise>
{
public:
	CounterClockWiseFireChainView()
		: FireChainView()
	{
	}
	virtual ~CounterClockWiseFireChainView() override final
	{
	}

	CounterClockWiseFireChainView(const CounterClockWiseFireChainView& other) = default;
	CounterClockWiseFireChainView(CounterClockWiseFireChainView&& other) = default;
	
	CounterClockWiseFireChainView& operator=(const CounterClockWiseFireChainView& other) = default;
	CounterClockWiseFireChainView& operator=(CounterClockWiseFireChainView&& other) = default;

	// Inherited via IView
	virtual bool Initialize() override final
	{
		if (!FireChainView::Initialize())
			return false;

		m_pRotatingProcess = std::make_shared<RotatingProcess>(GetActor(), -m_pTransform->GetSpeed());
		m_processManager.AttachProcess(m_pRotatingProcess);
		return true;
	}
};