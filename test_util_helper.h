#pragma once

#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Operator.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ExecutionEngine/Orc/LLJIT.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Mangler.h"
#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/Orc/ExecutionUtils.h"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/IR/DebugInfo.h"

#include "pochivm/common.h"
#include "pochivm/codegen_context.hpp"
#include "pochivm/pochivm.h"

// This class has been so rotten.. Don't use it for anything but test purpose
// WARNING: especially do not use this class for perf benchmark.
// It does not set llvm::CodeGenOpt::Level correctly, so the assembly instructions emitted by LLVM is very poor quality.
//
class SimpleJIT
{
public:
    SimpleJIT()
        : m_jit(nullptr)
        , m_astModule(nullptr)
        , m_allowResolveSymbolInHostProcess(false)
    { }

    // JIT the given module. Transfers ownership of the llvm module.
    // The previous JIT'd module, if exists, is thrown away
    //
    void SetModule(PochiVM::AstModule* module)
    {
        llvm::ExitOnError exitOnErr;
        std::unique_ptr<llvm::orc::LLJIT>&& jit = GetJIT();
        llvm::orc::ThreadSafeModule M = module->GetThreadSafeModule();
        exitOnErr(jit->addIRModule(std::move(M)));
        m_jit.reset(jit.release());
        m_astModule = module;
    }

    void SetNonAstModule(std::unique_ptr<llvm::orc::ThreadSafeModule> module)
    {
        llvm::ExitOnError exitOnErr;
        std::unique_ptr<llvm::orc::LLJIT> jit = GetJIT();
        exitOnErr(jit->addIRModule(std::move(*module.release())));
        m_jit.reset(jit.release());
        m_astModule = nullptr;
    }

    std::unique_ptr<llvm::orc::LLJIT> GetJIT()
    {
        llvm::ExitOnError exitOnErr;
        std::unique_ptr<llvm::orc::LLJIT> jit = exitOnErr(llvm::orc::LLJITBuilder().create());
        if (m_allowResolveSymbolInHostProcess)
        {
            char Prefix = llvm::EngineBuilder().selectTarget()->createDataLayout().getGlobalPrefix();
            std::unique_ptr<llvm::orc::DynamicLibrarySearchGenerator> R =
                    exitOnErr(llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(Prefix));
            ReleaseAssert(R != nullptr);
            jit->getMainJITDylib().addGenerator(std::move(R));
        }
        return jit;
    }

    void SetAllowResolveSymbolInHostProcess(bool value)
    {
        m_allowResolveSymbolInHostProcess = value;
    }

    // Get a callable to the function name
    // FnPrototype may be a C-style pointer or std::function object.
    // Fires an assert if the function prototype does not match the actual type of the function
    //
    template<typename FnPrototype>
    FnPrototype GetFunction(const std::string& fnName)
    {
        ReleaseAssert(m_jit != nullptr && m_astModule != nullptr);
        ReleaseAssert(m_astModule->CheckFunctionExistsAndPrototypeMatches<FnPrototype>(fnName));
        llvm::ExitOnError exitOnErr;
        auto sym = exitOnErr(m_jit->lookup(fnName));
        return PochiVM::AstTypeHelper::function_addr_to_callable<FnPrototype>::get(
                reinterpret_cast<void*>(sym.getAddress()));
    }

    template<typename FnPrototype>
    FnPrototype GetFunctionNonAst(const std::string& fnName)
    {
        ReleaseAssert(m_jit != nullptr);
        llvm::ExitOnError exitOnErr;
        auto sym = exitOnErr(m_jit->lookup(fnName));
        return PochiVM::AstTypeHelper::function_addr_to_callable<FnPrototype>::get(
                reinterpret_cast<void*>(sym.getAddress()));
    }

    std::unique_ptr<llvm::orc::LLJIT> m_jit;
    PochiVM::AstModule* m_astModule;
    bool m_allowResolveSymbolInHostProcess;
};

class TestJitHelper
{
public:
    TestJitHelper() {}

    void Init(int optLevel)
    {
        using namespace PochiVM;
        TestAssert(0 <= optLevel && optLevel <= 3);
        if (optLevel > 0)
        {
            thread_pochiVMContext->m_curModule->OptimizeIR(optLevel);
        }

        llvm::ExitOnError exitOnErr;
        llvm::orc::JITTargetMachineBuilder jtmb((llvm::Triple(llvm::sys::getProcessTriple())));

        // The other part of this project are compiled using default options
        //     "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87"
        // For fair comparison, only enable the same set of CPU features used
        // TODO: consider enable avx2
        //
        jtmb.getFeatures().AddFeature("cx8");
        jtmb.getFeatures().AddFeature("fxsr");
        jtmb.getFeatures().AddFeature("mmx");
        jtmb.getFeatures().AddFeature("sse");
        jtmb.getFeatures().AddFeature("sse2");
        jtmb.getFeatures().AddFeature("x87");

        jtmb.setCPU("x86-64");

        if (optLevel == 0)
        {
            jtmb.setCodeGenOptLevel(llvm::CodeGenOpt::None);
        }
        else if (optLevel == 1)
        {
            jtmb.setCodeGenOptLevel(llvm::CodeGenOpt::Less);
        }
        else if (optLevel == 2)
        {
            jtmb.setCodeGenOptLevel(llvm::CodeGenOpt::Default);
        }
        else if (optLevel == 3)
        {
            jtmb.setCodeGenOptLevel(llvm::CodeGenOpt::Aggressive);
        }
        // We must use CodeModel::Medium, otherwise address of functions/data symbols would break down
        //
        jtmb.setCodeModel(llvm::CodeModel::Medium);

        m_jit = exitOnErr(llvm::orc::LLJITBuilder().setJITTargetMachineBuilder(jtmb).create());

        {
            char Prefix = llvm::EngineBuilder().selectTarget()->createDataLayout().getGlobalPrefix();
            std::unique_ptr<llvm::orc::DynamicLibrarySearchGenerator> R =
                    exitOnErr(llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(Prefix));
            ReleaseAssert(R != nullptr);
            m_jit->getMainJITDylib().addGenerator(std::move(R));
        }

        llvm::orc::ThreadSafeModule M = thread_pochiVMContext->m_curModule->GetThreadSafeModule();
        exitOnErr(m_jit->addIRModule(std::move(M)));
        m_astModule = thread_pochiVMContext->m_curModule;
    }

    template<typename FnPrototype>
    FnPrototype GetFunction(const std::string& fnName)
    {
        ReleaseAssert(m_jit != nullptr && m_astModule != nullptr);
        ReleaseAssert(m_astModule->CheckFunctionExistsAndPrototypeMatches<FnPrototype>(fnName));
        llvm::ExitOnError exitOnErr;
        auto sym = exitOnErr(m_jit->lookup(fnName));
        return PochiVM::AstTypeHelper::function_addr_to_callable<FnPrototype>::get(
                reinterpret_cast<void*>(sym.getAddress()));
    }

    std::unique_ptr<llvm::orc::LLJIT> m_jit;
    PochiVM::AstModule* m_astModule;
};
