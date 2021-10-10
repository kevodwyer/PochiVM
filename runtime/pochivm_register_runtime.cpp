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


    RegisterConstructor<std::vector<int>>();
    RegisterConstructor<std::vector<int>, size_t /*count*/>();
    RegisterConstructor<std::vector<int>, size_t /*count*/, int /*value*/>();
    RegisterConstructor<std::vector<int>, const std::vector<int>& /*other*/>();


    RegisterExceptionObjectType<int>();
    RegisterExceptionObjectType<int*****>();
    RegisterExceptionObjectType<int*>();
    RegisterExceptionObjectType<const int*>();
    RegisterExceptionObjectType<const int* const*>();
    RegisterExceptionObjectType<std::exception>();
    RegisterExceptionObjectType<std::bad_alloc>();
    RegisterExceptionObjectType<std::vector<int>>();

    RegisterConstructor<std::bad_alloc>();

    RegisterMemberObject<&std::pair<int, double>::first>();
    RegisterMemberObject<&std::pair<int, double>::second>();


    RegisterMemberFn<static_cast<std::vector<int>::iterator(std::vector<int>::*)()>(&std::vector<int>::begin)>();
    RegisterMemberFn<static_cast<std::vector<int>::iterator(std::vector<int>::*)()>(&std::vector<int>::end)>();
    RegisterMemberFn<&std::vector<int>::iterator::operator*>();
    RegisterMemberFn<&std::vector<int>::iterator::operator->>();
    // RegisterMemberFn<static_cast<std::vector<int>::iterator(std::vector<int>::iterator::*)(int)>(&std::vector<int>::iterator::operator++)>();
    // RegisterMemberFn<static_cast<std::vector<int>::iterator&(std::vector<int>::iterator::*)()>(&std::vector<int>::iterator::operator++)>();


    RegisterMemberFn<&std::string::c_str>();
    RegisterMemberFn<&std::string::size>();
    RegisterMemberFn<static_cast<std::string::reference(std::string::*)(std::string::size_type)>(&std::string::operator[])>();
    RegisterConstructor<std::string, const char*>();

    RegisterMemberFn<&std::vector<std::string>::size>();
    RegisterMemberFn<static_cast<std::vector<std::string>::reference(std::vector<std::string>::*)(std::vector<std::string>::size_type)>(&std::vector<std::string>::operator[])>();

    RegisterMemberFn<static_cast<std::vector<std::string>::iterator(std::vector<std::string>::*)()>(&std::vector<std::string>::begin)>();
    RegisterMemberFn<static_cast<std::vector<std::string>::iterator(std::vector<std::string>::*)()>(&std::vector<std::string>::end)>();
    RegisterMemberFn<&std::vector<std::string>::iterator::operator*>();
    RegisterMemberFn<&std::vector<std::string>::iterator::operator->>();

    RegisterOutlineDefinedOverloadedOperator<std::vector<std::string>::iterator, std::vector<std::string>::iterator, AstComparisonExprType::EQUAL>();
    RegisterOutlineDefinedOverloadedOperator<std::vector<std::string>::iterator, std::vector<std::string>::iterator, AstComparisonExprType::NOT_EQUAL>();
    RegisterOutlineDefinedOverloadedOperator<std::vector<std::string>::iterator, std::vector<std::string>::iterator, AstComparisonExprType::LESS_THAN>();
    RegisterOutlineDefinedOverloadedOperator<std::vector<std::string>::iterator, std::vector<std::string>::iterator, AstComparisonExprType::LESS_EQUAL>();
    RegisterOutlineDefinedOverloadedOperator<std::vector<std::string>::iterator, std::vector<std::string>::iterator, AstComparisonExprType::GREATER_THAN>();
    RegisterOutlineDefinedOverloadedOperator<std::vector<std::string>::iterator, std::vector<std::string>::iterator, AstComparisonExprType::GREATER_EQUAL>();

    RegisterOutlineIncrementOrDecrementOperator<std::vector<std::string>::iterator, true /*isIncrement*/>();
    RegisterOutlineIncrementOrDecrementOperator<std::vector<std::string>::iterator, false /*isIncrement*/>();


    RegisterConstructor<std::vector<uintptr_t>>();

    RegisterMemberFn<static_cast<std::vector<uintptr_t>::iterator(std::vector<uintptr_t>::*)()>(&std::vector<uintptr_t>::begin)>();
    RegisterMemberFn<static_cast<std::vector<uintptr_t>::iterator(std::vector<uintptr_t>::*)()>(&std::vector<uintptr_t>::end)>();
    RegisterMemberFn<&std::vector<uintptr_t>::iterator::operator*>();
    RegisterMemberFn<&std::vector<uintptr_t>::iterator::operator->>();

    RegisterOutlineDefinedOverloadedOperator<std::vector<uintptr_t>::iterator, std::vector<uintptr_t>::iterator, AstComparisonExprType::EQUAL>();
    RegisterOutlineDefinedOverloadedOperator<std::vector<uintptr_t>::iterator, std::vector<uintptr_t>::iterator, AstComparisonExprType::NOT_EQUAL>();
    RegisterOutlineDefinedOverloadedOperator<std::vector<uintptr_t>::iterator, std::vector<uintptr_t>::iterator, AstComparisonExprType::LESS_THAN>();
    RegisterOutlineDefinedOverloadedOperator<std::vector<uintptr_t>::iterator, std::vector<uintptr_t>::iterator, AstComparisonExprType::LESS_EQUAL>();
    RegisterOutlineDefinedOverloadedOperator<std::vector<uintptr_t>::iterator, std::vector<uintptr_t>::iterator, AstComparisonExprType::GREATER_THAN>();
    RegisterOutlineDefinedOverloadedOperator<std::vector<uintptr_t>::iterator, std::vector<uintptr_t>::iterator, AstComparisonExprType::GREATER_EQUAL>();

    RegisterOutlineIncrementOrDecrementOperator<std::vector<uintptr_t>::iterator, true /*isIncrement*/>();
    RegisterOutlineIncrementOrDecrementOperator<std::vector<uintptr_t>::iterator, false /*isIncrement*/>();

    RegisterMemberFn<static_cast<uintptr_t*(std::vector<uintptr_t>::*)()>(&std::vector<uintptr_t>::data)>();
    RegisterMemberFn<&std::vector<uintptr_t>::size>();
    RegisterMemberFn<static_cast<void(std::vector<uintptr_t>::*)(const uintptr_t&)>(&std::vector<uintptr_t>::push_back)>();

    RegisterMemberObject<&std::pair<const uintptr_t, uintptr_t>::first>();
    RegisterMemberObject<&std::pair<const uintptr_t, uintptr_t>::second>();

    RegisterConstructor<std::vector<uintptr_t>, size_t>();

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
