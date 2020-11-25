#pragma once
#include "pch.h"
#define D3D_API_API __declspec(dllexport)

VOID Hook();
VOID UnHook();
extern "C" D3D_API_API void A();