#define LUA_TEST false

#if LUA_TEST
    #include <lua.hpp>
    #include <memory>
    #include <Resource.h>
#endif

#if defined(_WIN32)
    #include <Windows.h>
    #include <VersionHelpers.h>
#endif

#include <iostream>
#include <tchar.h>
#include <Core/Math/Vector2.h>
#include "Game.h"

using namespace Bel;

#if LUA_TEST == false
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    srand(static_cast<unsigned int>(time(nullptr)));
    GameApp app;

    if (!app.Initialize())
    {
        return 1;
    }

    app.Run();
    app.Shutdown();

    return 0;
}
#else
using namespace Bel;

template <typename Type>
void PrintValueInVector(const Vector2<Type>& vec)
{
    std::cout << "X: " << vec.m_x << ", Y: " << vec.m_y << std::endl;
}

static int Square(lua_State* pState)
{
    double number = lua_tonumber(pState, 1); // get the parameter
    lua_pop(pState, 1); // pop it off the stack
    double result = number * number;
    lua_pushnumber(pState, result); // push the result
    size_t test;
    return 1; // return the number of return values;
}

static int Normalize(lua_State* pState)
{
    luaL_checktype(pState, 1, LUA_TTABLE); // verify we have a table

    size_t tableSize = lua_rawlen(pState, 1); // get table length

    // compute our length
    double length = 0;
    for (size_t tableIndex = 1; tableIndex <= tableSize; tableIndex++)
    {
        lua_rawgeti(pState, 1, tableIndex); // push value onto the stack
        double value = lua_tonumber(pState, -1);
        length += value * value;
        lua_pop(pState, 1);
    }
    length = sqrt(length);

    // now normalize all values 
    for (size_t tableIndex = 1; tableIndex <= tableSize; tableIndex++)
    {
        lua_rawgeti(pState, 1, tableIndex);
        double normValue = lua_tonumber(pState, -1);
        normValue /= length;
        lua_pushnumber(pState, normValue);
        lua_rawseti(pState, 1, tableIndex); // will pop the value for us
    }

    lua_pop(pState, 1); // pop our table

    return 0;
}

static int GetFullName(lua_State* pState)
{
    luaL_checktype(pState, 1, LUA_TTABLE);

    lua_getfield(pState, 1, "first");
    std::string first = lua_tostring(pState, -1);
    lua_pop(pState, 1);

    lua_getfield(pState, 1, "last");
    std::string last = lua_tostring(pState, -1);
    lua_pop(pState, 1);

    std::string fullName = first + " " + last;
    lua_pushstring(pState, fullName.c_str());
    return 1;
}

static void Inventory(lua_State* pState)
{
    // Get our function onto the stack
    lua_getglobal(pState, "PrintInventory");
    lua_newtable(pState);

    // Add the weapon to the table
    lua_pushstring(pState, "Master Sword");
    lua_setfield(pState, -2, "weapon");

    // Add the rupees to the table
    lua_pushnumber(pState, 100);
    lua_setfield(pState, -2, "rupees");

    // Call the function
    if (lua_pcall(pState, 1, 0, 0) != 0)
    {
        std::cout << "Error: " << lua_tostring(pState, -1) << std::endl;
    }
}

static void Inventory(Bel::ScriptingManager& manager)
{
    manager.GetGlobal("PrintInventory");
    manager.TraceStack();

    manager.CreateTable();
    manager.TraceStack();

    manager.PushString("Master Sword");
    manager.TraceStack();

    manager.AddToTable("weapon");
    manager.TraceStack();

    manager.PushNumber(100);
    manager.TraceStack();

    manager.AddToTable("rupees");
    manager.TraceStack();

    manager.CallFunction(1);
}

static void MultipleReturn(lua_State* pState)
{
    lua_getglobal(pState, "MultipleReturn");
    if (lua_pcall(pState, 0, 3, 0) != 0)
    {
        std::cout << "Error: " << lua_tostring(pState, -1) << std::endl;
    }

    std::cout << lua_tonumber(pState, -1) << std::endl;
    std::cout << lua_tonumber(pState, -2) << std::endl;
    std::cout << lua_tonumber(pState, -3) << std::endl;

    lua_pop(pState, 3);
}

int main()
{
    //auto pLuaState = std::unique_ptr<lua_State, decltype(&lua_close)>(luaL_newstate(), &lua_close);
    //luaL_openlibs(pLuaState.get());

    Bel::ZlibFile file;
    file.Load("resources.bin");


    Bel::ScriptingManager scriptingManager;
    scriptingManager.Initialize();

    scriptingManager.CreateTable();
    scriptingManager.AddToTable("Square", Square);
    scriptingManager.TraceStack();

    scriptingManager.AddToTable("Normalize", Normalize);
    scriptingManager.TraceStack();

    scriptingManager.AddToTable("GetFullName", GetFullName);
    scriptingManager.TraceStack();

    scriptingManager.CreateTable();
    scriptingManager.AddToTable("actors");
    scriptingManager.TraceStack();

    std::cout << "--------------------------------------------------------" << std::endl;

    auto pScript = file.LoadResource("Scripts/script.lua");
    scriptingManager.RunScript(pScript);

    Inventory(scriptingManager);
    scriptingManager.TraceStack();


    //bool error = luaL_loadbuffer(pLuaState.get(),
    //    pScript->GetData().data(), pScript->GetData().size(), "script") ||
    //    lua_pcall(pLuaState.get(), 0, 0, 0);
    //
    //MultipleReturn(pLuaState.get());
    //
    //
    //if (error)
    //{
    //    std::cout << lua_tostring(pLuaState.get(), -1) << std::endl;
    //    lua_pop(pLuaState.get(), 1);
    //}
    
    return 0;
}
#endif