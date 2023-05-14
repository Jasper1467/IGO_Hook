#pragma once

#include <cassert>
#include <Windows.h>

#if _WIN32 || _WIN64
#if _WIN64
#define IGO_MODULE (L"IGO64.dll")
#else
#define ENV32BIT
#define IGO_MODULE (L"IGO32.dll")
#endif
#endif

template<typename T>
class CIGO_Hook
{
public:
	explicit CIGO_Hook()
	{
		#if _WIN32 || _WIN64
#if _WIN64
		m_szModule = "IGO64.dll";
#else
		m_szModule = "IGO32.dll";
#endif
#endif
	}

	bool Hook(const char* szModule, const char* szProcName, T* pHook)
	{
		typedef bool(__fastcall* igoHookAPIFn)(const char*, const char*, LPVOID , FARPROC*);

		// 48 8B E9 FF 15 + 0x23
		const auto igoHookAPI = GetProcAddress(GetModuleHandleA(m_szModule), "igoHookAPI");
		assert(igoHookAPI);

		m_pHook = pHook;

		return ((igoHookAPIFn)igoHookAPI)(szModule, szProcName, pHook, m_pOriginal);
	}

	bool Unhook() const
	{
		typedef bool(__fastcall* igoUnhookAPIFn)(LPVOID);

		// 5B C3 48 83 39 00 + 0x18
		const auto igoUnhookAPI = GetProcAddress(GetModuleHandleA(m_szModule), "igoUnhookAPI");
		assert(igoUnhookAPI);

		return ((igoUnhookAPIFn)igoUnhookAPI)(m_pHook);
	}

	T GetOriginal()
	{
		return &m_pOriginal;
	}

private:
	const char* m_szModule = "";
	T* m_pOriginal = nullptr;
	T* m_pHook = nullptr;
};