#pragma once
#pragma warning(disable: 4805)

#include "Misc/AssertionMacros.h"

#define ALS_STRINGIFY_IMPLEMENTATION(Value) #Value

#define ALS_STRINGIFY(Value) ALS_STRINGIFY_IMPLEMENTATION(Value)

#define ALS_GET_TYPE_STRING(Type) \
	((void) sizeof UEAsserts_Private::GetMemberNameCheckedJunk(static_cast<Type*>(nullptr)), TEXTVIEW(#Type))

// A lightweight version of the ensure() macro that doesn't generate a C++ call stack and doesn't send a
// crash report, because it doesn't happen instantly and causes the editor to freeze, which can be annoying.

#if DO_ENSURE && !USING_CODE_ANALYSIS

namespace AlsEnsure
{
	ALS_API bool UE_DEBUG_SECTION VARARGS Execute(bool& bExecuted, bool bEnsureAlways, const ANSICHAR* Expression,
	                                              const TCHAR* StaticMessage, const TCHAR* Format, ...);
}

#define ALS_ENSURE_IMPLEMENTATION(Capture, bEnsureAlways, Expression, Format, ...) \
	(LIKELY(Expression) || [Capture]() UE_DEBUG_SECTION \
	{ \
		static constexpr auto StaticMessage{TEXT("Ensure failed: " #Expression ", File: " __FILE__ ", Line: " ALS_STRINGIFY(__LINE__) ".")}; \
		static auto bExecuted{false}; \
		\
		FValidateArgsInternal(__VA_ARGS__); \
		\
		if (AlsEnsure::Execute(bExecuted, bEnsureAlways, #Expression, StaticMessage, Format, ##__VA_ARGS__)) \
		{ \
			PLATFORM_BREAK(); \
		} \
		\
		return false; \
	}())

#define ALS_ENSURE(Expression) ALS_ENSURE_IMPLEMENTATION( , false, Expression, TEXT(""))
#define ALS_ENSURE_MESSAGE(Expression, Format, ...) ALS_ENSURE_IMPLEMENTATION(&, false, Expression, Format, ##__VA_ARGS__)
#define ALS_ENSURE_ALWAYS(Expression) ALS_ENSURE_IMPLEMENTATION( , true, Expression, TEXT(""))
#define ALS_ENSURE_ALWAYS_MESSAGE(Expression, Format, ...) ALS_ENSURE_IMPLEMENTATION(&, true, Expression, Format, ##__VA_ARGS__)

#else

#define ALS_ENSURE(Expression) (Expression)
#define ALS_ENSURE_MESSAGE(Expression, Format, ...) (Expression)
#define ALS_ENSURE_ALWAYS(Expression) (Expression)
#define ALS_ENSURE_ALWAYS_MESSAGE(Expression, Format, ...) (Expression)

#endif

#define __XTEXT_TOKEN_(token1, token2) TEXT(token1 token2)
#define __XLOG_(Category, Format, ...) UE_LOG(LogTemp, Category, __XTEXT_TOKEN_("[%d:%s:%d] ", Format), clock(), __FILEW__, __LINE__, ##__VA_ARGS__)
#define XINFO(Format, ...) __XLOG_(Display, Format, ##__VA_ARGS__)
#define XWARN(Format, ...) __XLOG_(Warning, Format, ##__VA_ARGS__)
#define XERROR(Format, ...) __XLOG_(Error, Format, ##__VA_ARGS__)
#define XFATAL(Format, ...) __XLOG_(Fatal, Format, ##__VA_ARGS__)

#define XASSERT(expr) if (!(expr)) XFATAL(#expr)
#define XASSERT_MESSAGE(expr, Format, ...) if (!(expr)) XFATAL(Format, ##__VA_ARGS__)
#define XVALID_ERROR(expr, Format, ...) if (!IsValid(expr)) XERROR(Format, ##__VA_ARGS__)
#define XVALID_WARN(expr, Format, ...) if (!IsValid(expr)) XWARN(Format, ##__VA_ARGS__)

#define ECC_LedgeClimbable ECC_GameTraceChannel1
#define ECC_Beam ECC_GameTraceChannel2
#define ECC_Rope ECC_GameTraceChannel3
#define ECC_Bubbles ECC_GameTraceChannel4
#define ECC_Water ECC_GameTraceChannel5
#define ECC_TraceWater ECC_GameTraceChannel6

template<typename T, typename T2 = T> inline void
SetValue(T& dst, const T2& src) {
	if (dst != src) {
		dst = src;
	}
}

template<> inline void
SetValue(FVector& dst, const FVector& src) {
	if (!dst.Equals(src, 0.0001)) {
		dst = src;
	}
}

template<> inline void
SetValue(FRotator& dst, const FRotator& src) {
	if (!dst.Equals(src, 0.0001)) {
		dst = src;
	}
}

template<> inline void
SetValue(FQuat& dst, const FQuat& src) {
	if (!dst.Equals(src, 0.0001)) {
		dst = src;
	}
}

template<> inline void
SetValue(FVector2D& dst, const FVector2D& src) {
	if (!dst.Equals(src, 0.0001)) {
		dst = src;
	}
}

template<> inline void
SetValue(FTransform& dst, const FTransform& src) {
	if (!dst.Equals(src, 0.0001)) {
		dst = src;
	}
}

template<typename T> inline void
SetValue(bool& dst, const T& src) {
	if (dst != src) {
		dst = static_cast<bool>(src);
	}
}