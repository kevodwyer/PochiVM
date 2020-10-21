#include "fastinterp_ast_helper.hpp"

namespace PochiVM
{

FastInterpSnippet WARN_UNUSED AstVariable::PrepareForFastInterp(FISpillLocation spillLoc)
{
    FINumOpaqueIntegralParams numOIP = thread_pochiVMContext->m_fastInterpStackFrameManager->GetNumNoSpillIntegral();
    FINumOpaqueFloatingParams numOFP = thread_pochiVMContext->m_fastInterpStackFrameManager->GetNumNoSpillFloat();
    FastInterpBoilerplateInstance* inst = thread_pochiVMContext->m_fastInterpEngine->InstantiateBoilerplate(
                FastInterpBoilerplateLibrary<FIVariableImpl>::SelectBoilerplateBluePrint(
                    GetTypeId().GetDefaultFastInterpTypeId(),
                    !spillLoc.IsNoSpill(),
                    numOIP,
                    numOFP));
    spillLoc.PopulatePlaceholderIfSpill(inst, 0);
    inst->PopulateConstantPlaceholder<uint64_t>(1, GetFastInterpOffset());
    return FastInterpSnippet {
        inst, inst
    };
}

FastInterpSnippet WARN_UNUSED AstVariable::GetFastInterpDestructorSnippet()
{
    thread_pochiVMContext->m_fastInterpStackFrameManager->AssertNoTemp();

    if (!GetTypeId().RemovePointer().IsCppClassType())
    {
        return FastInterpSnippet { nullptr, nullptr };
    }

    if (m_fastInterpDtorCallOp.IsEmpty())
    {
        const CppFunctionMetadata* md = GetDestructorMetadata(GetTypeId().RemovePointer());
        TestAssert(md != nullptr);

        FastInterpCppFunctionInfo info = md->m_getFastInterpFn();
        TestAssert(info.m_isNoExcept);
        TestAssert(info.m_returnType.IsVoid());

        thread_pochiVMContext->m_fastInterpStackFrameManager->PushTemp(TypeId::Get<uint64_t>());
        FastInterpSnippet varAddr = PrepareForFastInterp(x_FINoSpill);

        FastInterpBoilerplateInstance* fillParamOp = thread_pochiVMContext->m_fastInterpEngine->InstantiateBoilerplate(
                    FastInterpBoilerplateLibrary<FICallExprStoreParamImpl>::SelectBoilerplateBluePrint(
                        TypeId::Get<void*>().GetDefaultFastInterpTypeId() /*paramType*/,
                        false /*hasMore*/));
        fillParamOp->PopulateConstantPlaceholder<uint64_t>(0, 8);

        FastInterpBoilerplateInstance* switchStackFrame = thread_pochiVMContext->m_fastInterpEngine->InstantiateBoilerplate(
                    FastInterpBoilerplateLibrary<FICallExprEnterCppFnImpl>::SelectBoilerplateBluePrint(
                        TypeId::Get<void>().GetDefaultFastInterpTypeId() /*returnType*/,
                        true /*isNoExcept*/));
        switchStackFrame->PopulateCppFnPtrPlaceholder(0, reinterpret_cast<void(*)(void) noexcept>(info.m_interpImpl));

        m_fastInterpDtorCallOp = varAddr.AddContinuation(fillParamOp).AddContinuation(switchStackFrame);
        TestAssert(!m_fastInterpDtorCallOp.IsEmpty());

        m_fastInterpDtorCallOp.m_entry->SetAlignmentLog2(4);

        {
            FISpillLocation spillLoc = thread_pochiVMContext->m_fastInterpStackFrameManager->PopTemp(TypeId::Get<uint64_t>());
            TestAssert(spillLoc.IsNoSpill());
            std::ignore = spillLoc;
        }
    }

    FastInterpBoilerplateInstance* inst = thread_pochiVMContext->m_fastInterpEngine->InstantiateBoilerplate(
            FastInterpBoilerplateLibrary<FICallExprImpl>::SelectBoilerplateBluePrint(
                TypeId::Get<void>().GetDefaultFastInterpTypeId() /*returnType*/,
                false /*spillReturnValue*/,
                true /*isNoExcept*/,
                FIStackframeSizeCategoryHelper::SelectCategory(24 /*stackFrameSize*/)));
    inst->PopulateBoilerplateFnPtrPlaceholder(1, m_fastInterpDtorCallOp.m_entry);

    return FastInterpSnippet {
        inst, inst
    };
}

}   // namespace PochiVM
