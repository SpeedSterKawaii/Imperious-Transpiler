#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <string>
#include <iostream>
#include <queue>
#include "LuauSerializer.h"
#include "LuauTranspiler.h"
#include "Retcheck.h"
#include "CallCheck Bypass.h"
#include "Rlua.h"
extern "C" {
#include "Lua\lua.h"
#include "Lua\lualib.h"
#include "Lua\lauxlib.h"
#include "Lua\luaconf.h"
#include "Lua\llimits.h"
#include "Lua\lobject.h"
#include "Lua/llimits.h"
}
/*
I've done some stuff to imperious so it can help you on how to use it.
Btw SpeedSterKawaii Did this, Credit me pleaze lol ok bye.

This uses mellon addies which we can update the dll with ez copy paste lol.
*/

const char* ExploitName = "Imperious"; // This is the Exploit Name.

void RunWatermark()
{
	// Shit watermark code written by speedsterkawaii.
	r_lua_getglobal(RL, "warn"); // Use "warn" because we are warning duh.
	r_lua_pushstring(RL, "Imperious has Injected, Enjoy."); // This is the message that is going to print in the console.
	r_lua_pcall(RL, 1, 0, 0);
}

void ConsoleBypass(const char* Name) // Console Bypass (opens console).
{
	DWORD ree;
	VirtualProtect((PVOID)&FreeConsole, 1, PAGE_EXECUTE_READWRITE, &ree);
	*(BYTE*)(&FreeConsole) = 0xC3;
	AllocConsole();
	SetConsoleTitleA(Name); // Set the Console Title via Const Char.
	freopen("CONOUT$", "w", stdout);
	freopen("CONIN$", "r", stdin);
	::SetWindowPos(GetConsoleWindow(), HWND_TOPMOST, 0, 0, 0, 0, SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	::ShowWindow(GetConsoleWindow(), SW_NORMAL);
}

bool ExecuteScript(std::string Script) { // Execute the script.
	DWORD xD = r_lua_newthread(RL);
	std::string Bytecode = LuauTranspiler::compile(lua_open(), Script);
	Script = "spawn(function() script = Instance.new('LocalScript') script.Disabled = true; script.Parent = nil\r\n" + Script + "\r\nend)";
	lua_getglobal(L, "string");
	lua_getfield(L, -1, "dump");
	if (luaL_loadbuffer(L, Script.c_str(), Script.size(), "@ImperiousAPI")) // ImperiousAPI is the Exploit Name (again).
	{
		// Throw an exception if an error occurs.
		const std::string errrr = lua_tostring(L, -1);
		lua_close(L);
		throw std::exception(errrr.c_str());
	}
	r_lua_deserialize(xD, ExploitName, LuauTranspiler::compile(lua_open(), Script).c_str(), LuauTranspiler::compile(lua_open(), Script).size());
	// Sometimes r_lua_deserialize can be Patched But No Big Deal, Arent you gonna fix it you skid?
	lua_pcall(L, 0, 0, 0);
	r_lua_spawn(xD);
	return true; // Because we use "bool" we return.
}

union r_Value
{
	void* gc;
	void* p;
	double n;
	bool b;
};

typedef struct r_TValue
{
	r_Value value;
	byte tt;
	byte shit;
};

int RenderstepHook(DWORD rL)
{
	if (!ScriptQueue.empty()) {
		auto script = ScriptQueue.front();
		ExecuteScript(script);
		ScriptQueue.pop();
	}
	return 1;
}

void Execute(const std::string& Script) { // Execute Script in Simple Form.
	if (Script != "") {
		ScriptQueue.push(Script);
	}
}

int __cdecl exepipelmao() { // Creates the Pipe.
	HANDLE hPipe;
	char buffer[999999]; // Buffer Size (dont mess with it).
	DWORD dwRead;
	hPipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\Transpiler"), // This is the DLL Pipe Name.
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, 
		1,
		9999999,
		9999999,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);
	while (hPipe != INVALID_HANDLE_VALUE)
	{
		if (ConnectNamedPipe(hPipe, NULL) != FALSE) 
		{
			while (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL) != FALSE)
			{
				buffer[dwRead] = '\0';

				if (strlen(buffer) != NULL) {
					ScriptQueue.push(buffer); // Execute the Script to the PipeName.
				}
			}
		}
		DisconnectNamedPipe(hPipe);
	}
}

void SetLevel(DWORD RobloxState, int Level) // Use Identity Address to Create Level.
{
	int v3 = *(DWORD*)(RobloxState + EmptySpace1);
	*(DWORD*)(v3 + EmptySpace2) = Level;
}

DWORD SetThreadIdentity(DWORD rL, int level) // Use Identity Address to Create Identity.
{
	return *(DWORD*)(*(DWORD*)(rL + Identity2) + 24) = level;
}

void RunRenderedStepHook() // Run RenderStepped.
{
	r_lua_getfield(RL, LUA_GLOBALSINDEX, "game");
	r_lua_getfield(RL, -1, "GetService");
	r_lua_pushvalue(RL, -2);
	r_lua_pushstring(RL, "RunService");
	r_lua_pcall(RL, 2, 1, 0);
	r_lua_getfield(RL, -1, "RenderStepped");
	r_lua_getfield(RL, -1, "Connect");
	r_lua_pushvalue(RL, -2);
	r_lua_pushcclosure(RL, (DWORD)RenderstepHook, 0);
	r_lua_pcall(RL, 2, 0, 0);
	r_lua_pop(RL, 2);
}

void Thread() { // Attach this Void to the DLL_PROCESS_ATTACH.
	ConsoleBypass("Imperious Transpiler - Updated by SpeedSterKawaii - April 2021");
	DWORD DataModel = ReturnDataModel(); // Do GetDataModel Stuff.
	DWORD ScriptContext = FindChildOfInstance(DataModel, "ScriptContext");
	std::cout << "GDM: " << getdatamodel_addr << "\n";
	std::cout << "GetDataModel Done.\n";
	RL = r_lua_newthread(RBX_LuaState(ScriptContext));
	DWORD Thread = r_lua_newthread(RL);
	std::cout << "NewThread Done.\n";
	L = luaL_newstate();
	std::cout << "New State Done.\n";
	luaL_openlibs(L);
	SetLevel(RL, 6); // Make the Level 6.
	std::cout << "Level is now 6.\n";
	SetThreadIdentity(RL, 6); // Make Identity 6.
	std::cout << "Identity is now 6.\n";
	BypassCallCheck(); // Bypass the CallCheck.
	std::cout << "CallCheck Bypassed.\n";
	RunRenderedStepHook();
	std::cout << "RenderStepped Done.\n";
	CreateThread(NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(exepipelmao), NULL, NULL, NULL);
	std::cout << "Pipe has Created.\n";
	std::cout << "Imperious has Injected.\n";
	RunWatermark();
}

BOOL APIENTRY DllMain(HMODULE Module, DWORD Reason, void* Reserved) {
	if (Reason == DLL_PROCESS_ATTACH) { // Process has been Attached to the DLL.
		CreateThread(NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(Thread), NULL, NULL, NULL);
	}
	return TRUE;
}