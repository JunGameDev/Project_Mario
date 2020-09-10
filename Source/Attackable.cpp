#include <Core/Layers/ApplicationLayer.h>
#include "Attackable.h"
using namespace Bel;


#include "Attackable.gen"

namespace Lua
{
    //static int AttackableCanAttack(lua_State* pState)
    //{
    //    auto pAttackable = reinterpret_cast<Attackable*>(lua_touserdata(pState, 1));
    //    lua_pop(pState, 1);
    //    lua_pushboolean(pState, pAttackable->CanAttack());

    //    return 1;
    //}

    static int AttackableReadyToAttack(lua_State* pState)
    {
        auto pAttackable = reinterpret_cast<Attackable*>(lua_touserdata(pState, 1));
        lua_pop(pState, 1);
        pAttackable->ReadyToAttack();
        return 0;
    }

    static int AttackableNotReadyToAttack(lua_State* pState)
    {
        auto pAttackable = reinterpret_cast<Attackable*>(lua_touserdata(pState, 1));
        lua_pop(pState, 1);
        pAttackable->NotReadyToAttack();
        return 0;
    }
}

bool Attackable::Initialize(tinyxml2::XMLElement* pData)
{
    auto pElement = pData->FirstChildElement("CanAttack");
    if (pElement == nullptr)
    {
        return false;
    }
    m_canAttack = pElement->BoolText();

    return true;
}

bool Attackable::PostInit()
{
    return true;
}

void Attackable::Render(IGraphics* pGraphics)
{
}

//void Attackable::RegisterWithScript(ScriptingManager& scripting)
//{
//    scripting.GetGlobal("g_logic");
//    scripting.GetFromTable("Actors");
//    scripting.GetFromTable(GetOwner()->GetId());
//
//    scripting.CreateTable();
//    scripting.AddToTable("this", this);
//    scripting.AddToTable("CanAttak", Lua::AttackableCanAttack);
//    scripting.AddToTable("ReadyToAttack", Lua::AttackableReadyToAttack);
//    scripting.AddToTable("NotReadyToAttack", Lua::AttackableNotReadyToAttack);
//    scripting.AddToTable("Attackable");
//
//    scripting.PopAll();
//}