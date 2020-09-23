#pragma once

#include "pochivm/common.h"

// Define 'placeholders' that will be replaced by constant values at runtime.
//
// Usage:
// (1) DEFINE_BOILERPLATE_FNPTR_PLACEHOLDER_[n](noexcept-function-pointer-type)
//         Define BOILERPLATE_FNPTR_PLACEHOLDER_[n] to hold a function pointer that will point to a boilerplate function of type 'type'.
//         WARNING: It is not allowed to hold a null function pointer, or a function pointer pointing to a C++ function.
//
// (2) DEFINE_CONSTANT_PLACEHOLDER_[n]
//         Define CONSTANT_PLACEHOLDER_[n] to hold a value of a primitive type.
//         WARNING: It is illegal to later cast it to a function pointer and invoke it.
//         WARNING: If the type is 8-byte long, the value must not be zero.
//                  This implies that to represent a nullable-pointer, or any 8-byte data type that may be zero,
//                  you must use a constant placeholder TOGETHER WITH a boolean to special case if the data is 0.
//                  For simplicity, it is recommended that you unconditionally always pair
//                  a CONSTANT_PLACEHOLDER with a is-zero boolean template parameter.
//
// (3) DEFINE_CPP_FNTPR_PLACEHOLDER_[n](noexcept-function-pointer-type)
//         Define CPP_FNPTR_PLACEHOLDER_[n] to hold a function pointer that will point to a C++ function in host process of type 'type'.
//         WARNING: It is not allowed to hold a null function pointer.
//
// Finally, since placeholders are internally just external symbols, it is illegal to compare for equality, >=, <=
// of two placeholders, if the placehoder is 64bit since the compiler may assume that the equality case never holds.
//
// This definition is scoped (since the macro just expands to a normal C++ variable declaration).
// However, it is ok to simply re-define it in another scope.
//
// Example:
//    DEFINE_BOILERPLATE_FNPTR_PLACEHOLDER_1(int(*)(int) noexcept);
//    int result = BOILERPLATE_FNPTR_PLACEHOLDER_1(123);
//

// At implementation level, a placeholder is just a dummy symbol. Then later we can discover those symbols in the object file,
// and replace those symbols by absolute values according to the relocation records specified in the object file.
//
// This is a completely safe approach, since technically, this is the identical process as what happens in the dynamic library
// load-time relocation mechanism. We are just repurposing the mechanism for another usage, but we did not modify
// anything in the mechanism itself, so nothing shall break.
//
// The dummy symbols are just dummy declarations: its prototype does not matter, and no definition need to exist
// (the definition must not exist actually, to make sure all accesses to them become relocation records)
//
// The reason that we need 3 different kinds of dummy declarations is a bit complex:
//    In x86-64, a direct 'call' instruction can only branch to within +/- 2GB from the current address.
//    To access addresses further away, one must use indirect calls [ e.g. mov imm64, %rax; call *%rax ]
//    gcc/clang use 'code models' to model assumptions on code and data size.
//    The default 'mcmodel=small' model (with no PIE) assumes all code and data will be placed at the beginning 2GB of RAM,
//    so one may assume that any function or data is accessible with signed 32-bit offsets from current %rip.
//
//    This means that under 'mcmodel=small', the compiler is free to assume that all symbols have value < 2^31.
//    This does not fit our purpose, however.
//    (1) Sometimes we use placeholders to represent a 64-bit raw data.
//        This breaks down, since the compiler is free to assume that the higher-33 bits are 0.
//        The solution is to compile this file using 'mcmodel=medium', which makes the assumption that data symbols
//        may not fall in first 2GB, but code will. Now just let the placeholder reference to a data symbol and we are good.
//    (2) When we call a CPP function in host process, the assumption that it is less than 2GB away from us may not hold,
//        since the generated program could be placed anywhere in the memory.
//        Therefore, a CPP function pointer must not be represented by a code symbol.
//        We represent it by a data symbol as well. We need to use another symbol name to distinguish with case (1).
//    (3) When we call another boilerplate function, we are certain that it is within +/- 2GB, since all the boilerplate
//        functions will be copied into a sequential chunk of memory.
//        Also it is crucial that the compiler *will* emit the direct-call instruction, since that is where one of the
//        most important perf gains come from. Fortunately 'mcmodel=medium' assumes that code falls in first 2GB,
//        so in that case we can just let our placeholder point to a code symbol, then the compiler will have the correct assumption.
//
// To summarize, we must compile this file using "-mcmodel=medium -fno-pic -fno-pie" and we need 3 kinds of dummy symbols:
// (1) A dummy function declaration. It is used to represent a placeholder which is a function pointer to another boilerplate function.
// (2) A large dummy data array declaration. It is used to represent a placeholder which is a 64-bit raw value.
// (3) A dummy uint64_t variable. It is used to represent a function pointer pointing to a C++ function in host process.
//
// Related link:  https://stackoverflow.com/questions/54947302/handling-calls-to-potentially-far-away-ahead-of-time-compiled-functions-from-j
//
// The following macro creates necessary dummy symbol declarations for PLACEHOLDER_ordinal
//
#define INTERNAL_GEN_DECLARATION_FOR_PLACEHOLDER(ordinal)                                                                             \
    extern "C" void __pochivm_fast_interp_dynamic_specialization_boilerplate_function_placeholder_ ## ordinal();                      \
    extern char __pochivm_fast_interp_dynamic_specialization_data_placeholder_ ## ordinal [1048576] __attribute__ ((__aligned__(1))); \
    extern uint64_t __pochivm_fast_interp_dynamic_specialization_aotc_cpp_function_placeholder_ ## ordinal;

// Make more of them if necessary, up to 63
//
INTERNAL_GEN_DECLARATION_FOR_PLACEHOLDER(0)
INTERNAL_GEN_DECLARATION_FOR_PLACEHOLDER(1)
INTERNAL_GEN_DECLARATION_FOR_PLACEHOLDER(2)
INTERNAL_GEN_DECLARATION_FOR_PLACEHOLDER(3)
INTERNAL_GEN_DECLARATION_FOR_PLACEHOLDER(4)
INTERNAL_GEN_DECLARATION_FOR_PLACEHOLDER(5)
INTERNAL_GEN_DECLARATION_FOR_PLACEHOLDER(6)
INTERNAL_GEN_DECLARATION_FOR_PLACEHOLDER(7)
INTERNAL_GEN_DECLARATION_FOR_PLACEHOLDER(8)
INTERNAL_GEN_DECLARATION_FOR_PLACEHOLDER(9)
INTERNAL_GEN_DECLARATION_FOR_PLACEHOLDER(10)
INTERNAL_GEN_DECLARATION_FOR_PLACEHOLDER(11)
INTERNAL_GEN_DECLARATION_FOR_PLACEHOLDER(12)

template<typename T> struct __pochivm_is_noexcept_fnptr_helper : std::false_type {};
template<typename R, typename... Args> struct __pochivm_is_noexcept_fnptr_helper<R(*)(Args...) noexcept> : std::true_type {};

#define INTERNAL_DEFINE_BOILERPLATE_FNPTR_PLACEHOLDER(ordinal, ...)                                                     \
    using _BOILERPLATE_FNPTR_PLACEHOLDER_TYPE_ ## ordinal = __VA_ARGS__;                                                \
    static_assert(std::is_pointer<_BOILERPLATE_FNPTR_PLACEHOLDER_TYPE_ ## ordinal>::value &&                            \
        std::is_function<typename std::remove_pointer<_BOILERPLATE_FNPTR_PLACEHOLDER_TYPE_ ## ordinal>::type>::value && \
        __pochivm_is_noexcept_fnptr_helper<_BOILERPLATE_FNPTR_PLACEHOLDER_TYPE_ ## ordinal>::value,                     \
        "not a noexcept function pointer");                                                                             \
    const _BOILERPLATE_FNPTR_PLACEHOLDER_TYPE_ ## ordinal BOILERPLATE_FNPTR_PLACEHOLDER_ ## ordinal =                   \
        reinterpret_cast<_BOILERPLATE_FNPTR_PLACEHOLDER_TYPE_ ## ordinal>(                                              \
            __pochivm_fast_interp_dynamic_specialization_boilerplate_function_placeholder_ ## ordinal)

#define INTERNAL_DEFINE_CONSTANT_PLACEHOLDER(ordinal, ...)                                                      \
    using _CONSTANT_PLACEHOLDER_TYPE_ ## ordinal = __VA_ARGS__;                                                 \
    static_assert(sizeof(_CONSTANT_PLACEHOLDER_TYPE_ ## ordinal) <= 8 &&                                        \
        (std::is_fundamental<_CONSTANT_PLACEHOLDER_TYPE_ ## ordinal>::value ||                                  \
        std::is_pointer<_CONSTANT_PLACEHOLDER_TYPE_ ## ordinal>::value) &&                                      \
        !std::is_function<typename std::remove_pointer<_CONSTANT_PLACEHOLDER_TYPE_ ## ordinal>::type>::value,   \
        "must be a primitive data type");                                                                       \
    union _UNION_CONSTANT_PLACEHOLDER_TYPE_ ## ordinal {                                                        \
        uint64_t __pochivm_dummy; _CONSTANT_PLACEHOLDER_TYPE_ ## ordinal __pochivm_actual_value; };             \
    const uint64_t _DONOTUSE_INTERNAL_CONSTANT_PLACEHOLDER_ ## ordinal = reinterpret_cast<uint64_t>(            \
        __pochivm_fast_interp_dynamic_specialization_data_placeholder_ ## ordinal);                             \
    const _CONSTANT_PLACEHOLDER_TYPE_ ## ordinal CONSTANT_PLACEHOLDER_ ## ordinal =                             \
        reinterpret_cast<const _UNION_CONSTANT_PLACEHOLDER_TYPE_ ## ordinal *>(                                 \
            &_DONOTUSE_INTERNAL_CONSTANT_PLACEHOLDER_ ## ordinal)->__pochivm_actual_value

#define INTERNAL_DEFINE_CPP_FNTPR_PLACEHOLDER(ordinal, ...)                                                     \
    using _CPP_FNTPR_PLACEHOLDER_TYPE_ ## ordinal = __VA_ARGS__;                                                \
    static_assert(std::is_pointer<_CPP_FNTPR_PLACEHOLDER_TYPE_ ## ordinal>::value &&                            \
        std::is_function<typename std::remove_pointer<_CPP_FNTPR_PLACEHOLDER_TYPE_ ## ordinal>::type>::value && \
        __pochivm_is_noexcept_fnptr_helper<_CPP_FNTPR_PLACEHOLDER_TYPE_ ## ordinal>::value,                     \
        "not a noexcept function pointer");                                                                     \
    const _CPP_FNTPR_PLACEHOLDER_TYPE_ ## ordinal CPP_FNTPR_PLACEHOLDER_ ## ordinal =                           \
        reinterpret_cast<_CPP_FNTPR_PLACEHOLDER_TYPE_ ## ordinal>(                                              \
            __pochivm_fast_interp_dynamic_specialization_aotc_cpp_function_placeholder_ ## ordinal)

#define DEFINE_BOILERPLATE_FNPTR_PLACEHOLDER_0(...) INTERNAL_DEFINE_BOILERPLATE_FNPTR_PLACEHOLDER(0, __VA_ARGS__)
#define DEFINE_BOILERPLATE_FNPTR_PLACEHOLDER_1(...) INTERNAL_DEFINE_BOILERPLATE_FNPTR_PLACEHOLDER(1, __VA_ARGS__)
#define DEFINE_BOILERPLATE_FNPTR_PLACEHOLDER_2(...) INTERNAL_DEFINE_BOILERPLATE_FNPTR_PLACEHOLDER(2, __VA_ARGS__)
#define DEFINE_BOILERPLATE_FNPTR_PLACEHOLDER_3(...) INTERNAL_DEFINE_BOILERPLATE_FNPTR_PLACEHOLDER(3, __VA_ARGS__)
#define DEFINE_BOILERPLATE_FNPTR_PLACEHOLDER_4(...) INTERNAL_DEFINE_BOILERPLATE_FNPTR_PLACEHOLDER(4, __VA_ARGS__)
#define DEFINE_BOILERPLATE_FNPTR_PLACEHOLDER_5(...) INTERNAL_DEFINE_BOILERPLATE_FNPTR_PLACEHOLDER(5, __VA_ARGS__)
#define DEFINE_BOILERPLATE_FNPTR_PLACEHOLDER_6(...) INTERNAL_DEFINE_BOILERPLATE_FNPTR_PLACEHOLDER(6, __VA_ARGS__)
#define DEFINE_BOILERPLATE_FNPTR_PLACEHOLDER_7(...) INTERNAL_DEFINE_BOILERPLATE_FNPTR_PLACEHOLDER(7, __VA_ARGS__)
#define DEFINE_BOILERPLATE_FNPTR_PLACEHOLDER_8(...) INTERNAL_DEFINE_BOILERPLATE_FNPTR_PLACEHOLDER(8, __VA_ARGS__)
#define DEFINE_BOILERPLATE_FNPTR_PLACEHOLDER_9(...) INTERNAL_DEFINE_BOILERPLATE_FNPTR_PLACEHOLDER(9, __VA_ARGS__)
#define DEFINE_BOILERPLATE_FNPTR_PLACEHOLDER_10(...) INTERNAL_DEFINE_BOILERPLATE_FNPTR_PLACEHOLDER(10, __VA_ARGS__)
#define DEFINE_BOILERPLATE_FNPTR_PLACEHOLDER_11(...) INTERNAL_DEFINE_BOILERPLATE_FNPTR_PLACEHOLDER(11, __VA_ARGS__)
#define DEFINE_BOILERPLATE_FNPTR_PLACEHOLDER_12(...) INTERNAL_DEFINE_BOILERPLATE_FNPTR_PLACEHOLDER(12, __VA_ARGS__)

#define DEFINE_CONSTANT_PLACEHOLDER_0(...) INTERNAL_DEFINE_CONSTANT_PLACEHOLDER(0, __VA_ARGS__)
#define DEFINE_CONSTANT_PLACEHOLDER_1(...) INTERNAL_DEFINE_CONSTANT_PLACEHOLDER(1, __VA_ARGS__)
#define DEFINE_CONSTANT_PLACEHOLDER_2(...) INTERNAL_DEFINE_CONSTANT_PLACEHOLDER(2, __VA_ARGS__)
#define DEFINE_CONSTANT_PLACEHOLDER_3(...) INTERNAL_DEFINE_CONSTANT_PLACEHOLDER(3, __VA_ARGS__)
#define DEFINE_CONSTANT_PLACEHOLDER_4(...) INTERNAL_DEFINE_CONSTANT_PLACEHOLDER(4, __VA_ARGS__)
#define DEFINE_CONSTANT_PLACEHOLDER_5(...) INTERNAL_DEFINE_CONSTANT_PLACEHOLDER(5, __VA_ARGS__)
#define DEFINE_CONSTANT_PLACEHOLDER_6(...) INTERNAL_DEFINE_CONSTANT_PLACEHOLDER(6, __VA_ARGS__)
#define DEFINE_CONSTANT_PLACEHOLDER_7(...) INTERNAL_DEFINE_CONSTANT_PLACEHOLDER(7, __VA_ARGS__)
#define DEFINE_CONSTANT_PLACEHOLDER_8(...) INTERNAL_DEFINE_CONSTANT_PLACEHOLDER(8, __VA_ARGS__)
#define DEFINE_CONSTANT_PLACEHOLDER_9(...) INTERNAL_DEFINE_CONSTANT_PLACEHOLDER(9, __VA_ARGS__)
#define DEFINE_CONSTANT_PLACEHOLDER_10(...) INTERNAL_DEFINE_CONSTANT_PLACEHOLDER(10, __VA_ARGS__)
#define DEFINE_CONSTANT_PLACEHOLDER_11(...) INTERNAL_DEFINE_CONSTANT_PLACEHOLDER(11, __VA_ARGS__)
#define DEFINE_CONSTANT_PLACEHOLDER_12(...) INTERNAL_DEFINE_CONSTANT_PLACEHOLDER(12, __VA_ARGS__)

#define DEFINE_CPP_FNTPR_PLACEHOLDER_0(...) INTERNAL_DEFINE_CPP_FNTPR_PLACEHOLDER(0, __VA_ARGS__)
#define DEFINE_CPP_FNTPR_PLACEHOLDER_1(...) INTERNAL_DEFINE_CPP_FNTPR_PLACEHOLDER(1, __VA_ARGS__)
#define DEFINE_CPP_FNTPR_PLACEHOLDER_2(...) INTERNAL_DEFINE_CPP_FNTPR_PLACEHOLDER(2, __VA_ARGS__)
#define DEFINE_CPP_FNTPR_PLACEHOLDER_3(...) INTERNAL_DEFINE_CPP_FNTPR_PLACEHOLDER(3, __VA_ARGS__)
#define DEFINE_CPP_FNTPR_PLACEHOLDER_4(...) INTERNAL_DEFINE_CPP_FNTPR_PLACEHOLDER(4, __VA_ARGS__)
#define DEFINE_CPP_FNTPR_PLACEHOLDER_5(...) INTERNAL_DEFINE_CPP_FNTPR_PLACEHOLDER(5, __VA_ARGS__)
#define DEFINE_CPP_FNTPR_PLACEHOLDER_6(...) INTERNAL_DEFINE_CPP_FNTPR_PLACEHOLDER(6, __VA_ARGS__)
#define DEFINE_CPP_FNTPR_PLACEHOLDER_7(...) INTERNAL_DEFINE_CPP_FNTPR_PLACEHOLDER(7, __VA_ARGS__)
#define DEFINE_CPP_FNTPR_PLACEHOLDER_8(...) INTERNAL_DEFINE_CPP_FNTPR_PLACEHOLDER(8, __VA_ARGS__)
#define DEFINE_CPP_FNTPR_PLACEHOLDER_9(...) INTERNAL_DEFINE_CPP_FNTPR_PLACEHOLDER(9, __VA_ARGS__)
#define DEFINE_CPP_FNTPR_PLACEHOLDER_10(...) INTERNAL_DEFINE_CPP_FNTPR_PLACEHOLDER(10, __VA_ARGS__)
#define DEFINE_CPP_FNTPR_PLACEHOLDER_11(...) INTERNAL_DEFINE_CPP_FNTPR_PLACEHOLDER(11, __VA_ARGS__)
#define DEFINE_CPP_FNTPR_PLACEHOLDER_12(...) INTERNAL_DEFINE_CPP_FNTPR_PLACEHOLDER(12, __VA_ARGS__)
