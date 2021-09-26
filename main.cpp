#include <cstdio>
#include <cstdlib> 

#include "codegen_context.hpp"
#include "pochivm.h"              // For all PochiVM APIs
#include "test_util_helper.h"

using namespace PochiVM;          // All PochiVM APIs live in this namespace

int main(int argc, char **argv)
{

    llvm::InitLLVM X(argc, argv);
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();

    // Initialize PochiVM global contexts for this thread
    // The contexts are automatically destructed on destruction of these variables
    AutoThreadPochiVMContext apv;
    AutoThreadErrorContext arc;
    AutoThreadLLVMCodegenContext alc;

    // code in snippets below are inserted here

    NewModule("test" /*name*/);
    using FnPrototype = uint64_t(*)(int);
    auto [fn, n] = NewFunction<FnPrototype>("fib");

    fn.SetBody(
      If(n <= Literal<int>(2)).Then(
        Return(Literal<uint64_t>(1))
      ).Else(
        Return(Call<FnPrototype>("fib", n - Literal<int>(1))
               + Call<FnPrototype>("fib", n - Literal<int>(2)))
      )
    );

    // Validate that the module does not contain errors.
    // We require doing this in test build before JIT'ing the module
    TestAssert(thread_pochiVMContext->m_curModule->Validate());

    // Unfortunately the API for JIT part is not done yet, so we still need some ugly code
    thread_pochiVMContext->m_curModule->EmitIR();
    thread_pochiVMContext->m_curModule->OptimizeIR(1);
    SimpleJIT jit;
    jit.SetModule(thread_pochiVMContext->m_curModule);

    // Retrive the built function, and execute it.
    FnPrototype jitFn = jit.GetFunction<FnPrototype>("fib");
    std::cout << "The 20th number in fibonacci sequence is " << jitFn(20) << std::endl;

    return 0;
}

