// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#ifndef _SPTAG_CORE_COMMONDEFS_H_
#define _SPTAG_CORE_COMMONDEFS_H_

#include <cstdint>
#include <type_traits>
#include <memory>
#include <string>
#include <limits>
#include <vector>
#include <cmath>
#include "inc/Helper/Logging.h"
#include "inc/Helper/DiskIO.h"

#ifndef _MSC_VER
#include <stdio.h>
#include <unistd.h>
#include <cstring>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/types.h>

#if defined(__INTEL_COMPILER)
#include <malloc.h>
#else
#include <mm_malloc.h>
#endif

#define FolderSep '/'

inline bool direxists(const char* path) {
    struct stat info;
    return stat(path, &info) == 0 && (info.st_mode & S_IFDIR);
}
inline bool fileexists(const char* path) {
    struct stat info;
    return stat(path, &info) == 0 && (info.st_mode & S_IFDIR) == 0;
}
template <class T>
inline T min(T a, T b) {
    return a < b ? a : b;
}
template <class T>
inline T max(T a, T b) {
    return a > b ? a : b;
}

#ifndef _rotl
#define _rotl(x, n) (((x) << (n)) | ((x) >> (32-(n))))
#endif

#define mkdir(a) mkdir(a, ACCESSPERMS)
#define InterlockedCompareExchange(a,b,c) __sync_val_compare_and_swap(a, c, b)
#define InterlockedExchange8(a,b) __sync_lock_test_and_set(a, b)
#define Sleep(a) usleep(a * 1000)
#define strtok_s(a, b, c) strtok_r(a, b, c)

#else

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <Psapi.h>
#include <malloc.h>

#define FolderSep '\\'

inline bool direxists(const TCHAR* path) {
    auto dwAttr = GetFileAttributes(path);
    return (dwAttr != INVALID_FILE_ATTRIBUTES) && (dwAttr & FILE_ATTRIBUTE_DIRECTORY);
}
inline bool fileexists(const TCHAR* path) {
    auto dwAttr = GetFileAttributes(path);
    return (dwAttr != INVALID_FILE_ATTRIBUTES) && (dwAttr & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

#define mkdir(a) CreateDirectory(a, NULL)
#endif

namespace SPTAG
{
#define ALIGN_SPTAG 32

typedef std::int32_t SizeType;
typedef std::int32_t DimensionType;

const SizeType MaxSize = (std::numeric_limits<SizeType>::max)();
const float MinDist = (std::numeric_limits<float>::min)();
const float MaxDist = (std::numeric_limits<float>::max)() / 10;
const float Epsilon = 0.000000001f;

extern std::shared_ptr<Helper::DiskPriorityIO>(*f_createIO)();

#define IOBINARY(ptr, func, bytes, ...) if (ptr->func(bytes, __VA_ARGS__) != bytes) return ErrorCode::DiskIOFail
#define IOSTRING(ptr, func, ...) if (ptr->func(__VA_ARGS__) == 0) return ErrorCode::DiskIOFail

extern std::shared_ptr<Helper::Logger> g_pLogger;

#define LOG(l, ...) g_pLogger->Logging("SPTAG", l, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

class MyException : public std::exception 
{
private:
    std::string Exp;
public:
    MyException(std::string e) { Exp = e; }
#ifdef _MSC_VER
    const char* what() const { return Exp.c_str(); }
#else
    const char* what() const noexcept { return Exp.c_str(); }
#endif
};

enum class ErrorCode : std::uint16_t
{
#define DefineErrorCode(Name, Value) Name = Value,
#include "DefinitionList.h"
#undef DefineErrorCode

    Undefined
};
static_assert(static_cast<std::uint16_t>(ErrorCode::Undefined) != 0, "Empty ErrorCode!");


enum class DistCalcMethod : std::uint8_t
{
#define DefineDistCalcMethod(Name) Name,
#include "DefinitionList.h"
#undef DefineDistCalcMethod

    Undefined
};
static_assert(static_cast<std::uint8_t>(DistCalcMethod::Undefined) != 0, "Empty DistCalcMethod!");


enum class VectorValueType : std::uint8_t
{
#define DefineVectorValueType(Name, Type) Name,
#include "DefinitionList.h"
#undef DefineVectorValueType

    Undefined
};
static_assert(static_cast<std::uint8_t>(VectorValueType::Undefined) != 0, "Empty VectorValueType!");


enum class IndexAlgoType : std::uint8_t
{
#define DefineIndexAlgo(Name) Name,
#include "DefinitionList.h"
#undef DefineIndexAlgo

    Undefined
};
static_assert(static_cast<std::uint8_t>(IndexAlgoType::Undefined) != 0, "Empty IndexAlgoType!");

enum class VectorFileType : std::uint8_t
{
#define DefineVectorFileType(Name) Name,
#include "DefinitionList.h"
#undef DefineVectorFileType

    Undefined
};
static_assert(static_cast<std::uint8_t>(VectorFileType::Undefined) != 0, "Empty VectorFileType!");

enum class TruthFileType : std::uint8_t
{
#define DefineTruthFileType(Name) Name,
#include "DefinitionList.h"
#undef DefineTruthFileType

    Undefined
};
static_assert(static_cast<std::uint8_t>(TruthFileType::Undefined) != 0, "Empty TruthFileType!");

template<typename T>
constexpr VectorValueType GetEnumValueType()
{
    return VectorValueType::Undefined;
}


#define DefineVectorValueType(Name, Type) \
template<> \
constexpr VectorValueType GetEnumValueType<Type>() \
{ \
    return VectorValueType::Name; \
} \

#include "DefinitionList.h"
#undef DefineVectorValueType


inline std::size_t GetValueTypeSize(VectorValueType p_valueType)
{
    switch (p_valueType)
    {
#define DefineVectorValueType(Name, Type) \
    case VectorValueType::Name: \
        return sizeof(Type); \

#include "DefinitionList.h"
#undef DefineVectorValueType

    default:
        break;
    }

    return 0;
}

} // namespace SPTAG

#endif // _SPTAG_CORE_COMMONDEFS_H_
