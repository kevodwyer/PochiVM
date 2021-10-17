// DO NOT INCLUDE ADDITIONAL HEADERS
// Put the includes in 'pochivm_runtime_headers.h', not here.
//
#include "pochivm_runtime_headers.h"
#include "pochivm/pochivm_reflection_helper.h"

__attribute__((__optnone__))
static void RegisterRuntimeLibrary()
{
    using namespace PochiVM;

    // ****************************************
    // Register the list of functions callable from generated code, using the APIs below.
    //
    //    void RegisterFreeFn<function pointer>()
    //    void RegisterMemberFn<member function pointer>()
    //    void RegisterStaticMemberFn<member function pointer>()
    //    void RegisterMemberObject<member object pointer>()
    //    void RegisterConstructor<ClassName, ArgTypeNames...>()
    //    void RegisterExceptionObjectType<Type>()
    //
    // While you may write any logic you like, keep in mind that you will get a segfault if you try to
    // call functions or access global values which implementations reside in other CPP files.
    // E.g. suppose implementation of function 'f' is not in this file.
    //      'auto fnPtr = &f' or 'RegisterFreeFn<&f>()' is OK since it only takes address of the symbol
    //      (which does not require knowledge of its implementation). But 'f()' will give you a segfault.
    //
    // ****************************************


    // Register the constructor, which takes no additional parameters
    RegisterConstructor<Tutorial101>();
    // Register member function Increment(int value)
    RegisterMemberFn<&Tutorial101::Increment>();
    // Register member function Print()
    RegisterMemberFn<&Tutorial101::Print>();

    RegisterMemberFn<&Tutorial101::opJUMP>();
    RegisterMemberFn<&Tutorial101::opBNE>();
    RegisterMemberFn<&Tutorial101::opLOAD_SP>();
    RegisterMemberFn<&Tutorial101::opLOAD_EAX_MEM>();
    RegisterMemberFn<&Tutorial101::opLOAD_EAX_IB>();
    RegisterMemberFn<&Tutorial101::opSTORE_EAX>();
    RegisterMemberFn<&Tutorial101::opLOAD_EBX_MEM>();
    RegisterMemberFn<&Tutorial101::opLOAD_EBX_IB>();
    RegisterMemberFn<&Tutorial101::opSTORE_EBX>();
    RegisterMemberFn<&Tutorial101::opSTORE_1>();
    RegisterMemberFn<&Tutorial101::opINC>();
    RegisterMemberFn<&Tutorial101::opADD>();
    RegisterMemberFn<&Tutorial101::opMUL>();
    RegisterMemberFn<&Tutorial101::opDIV>();
    RegisterMemberFn<&Tutorial101::opXOR>();

    // Register member object 'm_x'
    RegisterMemberObject<&Tutorial101::m_x>();
    RegisterMemberObject<&Tutorial101::opCounter>();

}

// DO NOT MODIFY
// dump_symbols.cpp JIT entry point
//
extern "C" __attribute__((__optnone__, __used__))
void __pochivm_register_runtime_library__()
{
    RegisterRuntimeLibrary();
}
