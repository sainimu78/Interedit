#pragma once
#include <cassert>
#include <cstddef>//std::size_t, offsetof
#include "IndexingConst.h"

//����define, typedef, const

#ifndef ASSERT
enum { INDEX_NONE = tq::INDEX_NONE };

//begin, ʵ��׶β�ʹ��PCH, �����ʱ���Ͷ���
typedef unsigned char uint8;
typedef int int32;
typedef unsigned int uint32;
typedef unsigned long long uint64;
//end

#define ASSERT(b) assert(b)
#endif

#ifndef LogError
#define LogError(fmt, ...)\
{\
printf(fmt "\n", ##__VA_ARGS__);\
ASSERT(false);\
} do{} while(0)
#endif

#ifdef NIFLECT_API
#else
    #ifdef WIN32
        #ifdef NIFLECT_EXPORTS
            #define NIFLECT_API __declspec(dllexport)
        #else
            #define NIFLECT_API __declspec(dllimport)
        #endif
    #else
        #ifdef NIFLECT_EXPORTS
		    #define NIFLECT_API __attribute__((visibility("default")))
	    #else
		    #define NIFLECT_API 
	    #endif
    #endif
#endif

#ifdef NIFLECT_C_API
#else
    #ifdef WIN32
        #ifdef NIFLECT_C_EXPORTS
            #define NIFLECT_C_API extern "C" __declspec(dllexport)
        #else
		    #define NIFLECT_C_API
        #endif
    #else
        #ifdef NIFLECT_C_EXPORTS
		    #define NIFLECT_C_API extern "C" __attribute__((visibility("default")))
	    #else
		    #define NIFLECT_C_API 
	    #endif
    #endif
#endif