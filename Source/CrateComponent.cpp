#include <Core/Layers/ApplicationLayer.h>
#include "CrateComponent.h"

using namespace Bel;

namespace Lua
{
    static int CrateComponentGetItemName(lua_State* pState)
    {
        CrateComponent* pCrate = reinterpret_cast<CrateComponent*>(lua_touserdata(pState, 1));
        lua_pop(pState, 1);
        lua_pushstring(pState, pCrate->GetItemName().c_str());
    
        return 1;
    }
}

bool CrateComponent::Initialize(tinyxml2::XMLElement* pData)
{
    auto pElement = pData->FirstChildElement("ItemName");
    if (pElement)
    {
        m_itemName = pElement->GetText();
    }

    return true;
}

bool CrateComponent::PostInit()
{
    return true;
}

void CrateComponent::RegisterWithScript()
{
    auto& scripting = ApplicationLayer::GetInstance()->GetGameLayer()->GetScriptingManager();
    scripting.GetGlobal("g_logic");
    scripting.GetFromTable("actors");
    scripting.GetFromTable(GetOwner()->GetId());

    scripting.CreateTable();
    scripting.AddToTable("this", this);
    scripting.AddToTable("GetItemName", Lua::CrateComponentGetItemName);
    scripting.AddToTable("CrateComponent");

    scripting.PopAll();
}