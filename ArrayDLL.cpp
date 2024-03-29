// ArrayDLL.cpp : Defines the exported functions for the DLL application.

#include "stdafx.h"
#include <stdio.h>
#include <assert.h>
#include <Math.h>

#define DEBUG

#if defined(DEBUG)
	#define WARN(message)				\
	{									\
		MessageBox(						\
			NULL,						\
			message,					\
			TEXT("Недопустимая ветка"),	\
			MB_OK | MB_ICONERROR		\
		);								\
		exit(EXIT_FAILURE);				\
	}

	#define WARN_IF(condition, message)		\
	{										\
		if (condition)						\
		{									\
			MessageBox(						\
				NULL,						\
				TEXT(message),				\
				TEXT(#condition),			\
				MB_OK | MB_ICONERROR		\
			);								\
			exit(EXIT_FAILURE);				\
		}									\
	}
#else
	#define WARN(message)
	#define WARN_IF(condition, message)
#endif

template<typename DataType>
struct TArray
{
	DataType* Data;

	int Num()
	{
		return ArrayNum;
	}

	void Reallocate(int NewNum, bool bCompact=false)
	{
		ArrayNum = NewNum;
		if (ArrayNum > ArrayMax || bCompact)
		{
			ArrayMax = ArrayNum;
			Data = (DataType*)(*ReallocFunctionPtr)(Data, ArrayMax * sizeof(DataType), 8);
		}
	}
private:
	int ArrayNum;
	int ArrayMax;
};

struct NaviStruct
{
	TArray<int> NaviData;
};

//---------------------------------------------------------------------------------------------------------

const double PI = 3.14159026;

double Gauss(double mu = 0.0, double sigma = 1.0) {
	static bool deviateAvailable = false;
	static double storedDeviate;
	double polar, sq, x, y;
	if (!deviateAvailable)
	{
		do
		{
			x = 2.0 * (double(rand()) / double(RAND_MAX)) - 1.0;
			y = 2.0 * (double(rand()) / double(RAND_MAX)) - 1.0;
			sq=x*x+y*y;
		}
		while (sq >= 1.0 || sq == 0.0);
		polar = sqrt(-2.0 * log(sq) / sq);
		storedDeviate = x * polar;   // два за раз
		deviateAvailable = true;   // при втором запуске вернёт заготовленное
		return y * polar * sigma + mu;  // а сейчас возвращает первое
	}
	else
	{
		deviateAvailable = false;
		return storedDeviate * sigma + mu;
	}
}

void SpiralGalaxy(int* stars, int nStars) { // меньше реального в 100000 раз
	double ang, dist;
	for(int i = 0; i< nStars; i++ )
	{
		ang = (Gauss()+0.5) * PI;
		dist = exp(ang * .247f) * 10000;  // спираль
		if (rand() % 2)  ang += PI;     // второй рукав ( rand(2) возвращает 0 либо 1 )
		stars[i * 3] = (int)floor(sin(ang) * dist + 500 + Gauss() * 2000);
		stars[i * 3 + 1] = (int)floor(cos(ang) * dist + 500 + Gauss() * 2000);
		stars[i * 3 + 2] = (int)floor(Gauss() * 2000);
	}
}

//---------------------------------------------------------------------------------------------------------

extern "C"
{
	typedef void* (*ReallocFunctionPtrType)(void* Original, DWORD Count, DWORD Alignment);

	ReallocFunctionPtrType ReallocFunctionPtr = NULL;

	struct FDLLBindInitData
	{
		INT Version;
		ReallocFunctionPtrType ReallocFunctionPtr;
	};

	__declspec(dllexport) void DLLBindInit(FDLLBindInitData* InitData)
	{
		ReallocFunctionPtr = InitData->ReallocFunctionPtr;
	}

	__declspec(dllexport) void GetNavData(struct NaviStruct* NavData,int nstars)
	{
		NavData->NaviData.Reallocate(nstars * 3);

		SpiralGalaxy(NavData->NaviData.Data, nstars);
	}
}


