#define POCHIVM_INSIDE_FASTINTERP_TPL_CPP

#include "fastinterp_tpl_helper.h"

namespace PochiVM
{

struct FIIfStatementImpl
{
    // TODO: we should support special-casing the 'cond' being a bool variable: this is a very common case
    //
    template<FIIfStmtNumStatements numTrueBranchStmtsEnum,
             FIIfStmtMayCFRMask trueBranchMayCFRMaskEnum>
    static constexpr bool cond()
    {
        constexpr int numStmts = static_cast<int>(numTrueBranchStmtsEnum);
        constexpr int mayCFRMask = static_cast<int>(trueBranchMayCFRMaskEnum);
        if (mayCFRMask >= (1 << numStmts)) { return false; }
        return true;
    }

    template<FIIfStmtNumStatements numTrueBranchStmtsEnum,
             FIIfStmtMayCFRMask trueBranchMayCFRMaskEnum,
             FIIfStmtNumStatements numFalseBranchStmtsEnum,
             FIIfStmtMayCFRMask falseBranchMayCFRMaskEnum>
    static constexpr bool cond()
    {
        constexpr int numStmts = static_cast<int>(numFalseBranchStmtsEnum);
        constexpr int mayCFRMask = static_cast<int>(falseBranchMayCFRMaskEnum);
        if (mayCFRMask >= (1 << numStmts)) { return false; }
        return true;
    }

    template<FIIfStmtMayCFRMask trueBranchMayCFRMaskEnum, FIIfStmtMayCFRMask falseBranchMayCFRMaskEnum>
    using ReturnTypeHelper = typename std::conditional<
        static_cast<int>(trueBranchMayCFRMaskEnum) == 0 && static_cast<int>(falseBranchMayCFRMaskEnum) == 0,
        void, InterpControlSignal>::type;

    // BoilerplateFn placeholder 0: the condition
    // BoilerplateFn placeholder 1-5: the true branch
    // BoilerplateFn placeholder 6-10: the false branch
    //
    template<FIIfStmtNumStatements numTrueBranchStmtsEnum,
             FIIfStmtMayCFRMask trueBranchMayCFRMaskEnum,
             FIIfStmtNumStatements numFalseBranchStmtsEnum,
             FIIfStmtMayCFRMask falseBranchMayCFRMaskEnum>
    static ReturnTypeHelper<trueBranchMayCFRMaskEnum, falseBranchMayCFRMaskEnum> f() noexcept
    {
        using ReturnType = ReturnTypeHelper<trueBranchMayCFRMaskEnum, falseBranchMayCFRMaskEnum>;

#define EXECUTE_STMT(stmtOrd, placeholderOrd)                                                                      \
    if constexpr(numStmts > (stmtOrd))                                                                             \
    {                                                                                                              \
        if constexpr((mayCFRMask & (1 << (stmtOrd))) != 0)                                                         \
        {                                                                                                          \
            INTERNAL_DEFINE_BOILERPLATE_FNPTR_PLACEHOLDER(placeholderOrd, InterpControlSignal(*)() noexcept);      \
            InterpControlSignal ics = BOILERPLATE_FNPTR_PLACEHOLDER_ ## placeholderOrd();                          \
            if (ics != InterpControlSignal::None)                                                                  \
            {                                                                                                      \
                return ics;                                                                                        \
            }                                                                                                      \
        }                                                                                                          \
        else                                                                                                       \
        {                                                                                                          \
            INTERNAL_DEFINE_BOILERPLATE_FNPTR_PLACEHOLDER(placeholderOrd, void(*)() noexcept);                     \
            BOILERPLATE_FNPTR_PLACEHOLDER_ ## placeholderOrd();                                                    \
        }                                                                                                          \
    }

        DEFINE_BOILERPLATE_FNPTR_PLACEHOLDER_0(bool(*)() noexcept);
        if (BOILERPLATE_FNPTR_PLACEHOLDER_0())
        {
            constexpr int numStmts = static_cast<int>(numTrueBranchStmtsEnum);
            constexpr int mayCFRMask = static_cast<int>(trueBranchMayCFRMaskEnum);
            EXECUTE_STMT(0, 1)
            EXECUTE_STMT(1, 2)
            EXECUTE_STMT(2, 3)
            EXECUTE_STMT(3, 4)
            EXECUTE_STMT(4, 5)

            if constexpr(!std::is_same<ReturnType, void>::value)
            {
                return InterpControlSignal::None;
            }
        }
        else
        {
            constexpr int numStmts = static_cast<int>(numFalseBranchStmtsEnum);
            constexpr int mayCFRMask = static_cast<int>(falseBranchMayCFRMaskEnum);
            EXECUTE_STMT(0, 6)
            EXECUTE_STMT(1, 7)
            EXECUTE_STMT(2, 8)
            EXECUTE_STMT(3, 9)
            EXECUTE_STMT(4, 10)

            if constexpr(!std::is_same<ReturnType, void>::value)
            {
                return InterpControlSignal::None;
            }
        }
        static_assert(static_cast<int>(FIIfStmtNumStatements::X_END_OF_ENUM) == 5 + 1);

#undef EXECUTE_STMT
    }

    static auto metavars()
    {
        return CreateMetaVarList(
                    CreateEnumMetaVar<FIIfStmtNumStatements::X_END_OF_ENUM>("trueBranchNumStmts"),
                    CreateEnumMetaVar<FIIfStmtMayCFRMask::X_END_OF_ENUM>("trueBranchMayCFRMask"),
                    CreateEnumMetaVar<FIIfStmtNumStatements::X_END_OF_ENUM>("falseBranchNumStmts"),
                    CreateEnumMetaVar<FIIfStmtMayCFRMask::X_END_OF_ENUM>("falseBranchMayCFRMask")
        );
    }
};

}   // namespace PochiVM

// build_fast_interp_lib.cpp JIT entry point
//
extern "C"
void __pochivm_build_fast_interp_library__()
{
    using namespace PochiVM;
    RegisterBoilerplate<FIIfStatementImpl>();
}
