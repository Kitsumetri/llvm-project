#include "clang/AST/AST.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Frontend/CompilerInstance.h"
#include "llvm/Support/raw_ostream.h"

namespace {

class AlwaysInlineHandler : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
    explicit AlwaysInlineHandler(clang::Rewriter &Rewrite) : Rewrite(Rewrite) {}

    void run(const clang::ast_matchers::MatchFinder::MatchResult &Result) override {
        const auto *Func = Result.Nodes.getNodeAs<clang::FunctionDecl>("func");
        if (Func && Func->hasBody() && !hasCondition(Func)) {
            clang::SourceLocation StartLoc = Func->getBeginLoc();
            Rewrite.InsertText(StartLoc, "__attribute__((always_inline))", true, true);
        }
    }

private:
    bool hasCondition(const clang::FunctionDecl *Func) const {
        const clang::Stmt *Body = Func->getBody();
        return containsCondition(Body);
    }

    bool containsCondition(const clang::Stmt *S) const {
        if (!S) return false;

        if (llvm::isa<clang::IfStmt>(S)) return true;

        for (const clang::Stmt *Child : S->children()) {
            if (containsCondition(Child)) {
                return true;
            }
        }
        return false;
    }

    clang::Rewriter &Rewrite;
};

class AlwaysInlineAction : public clang::PluginASTAction {
protected:
    std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef) override {
        RewriterForCI.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
        Finder.addMatcher(clang::ast_matchers::functionDecl(clang::ast_matchers::isDefinition()).bind("func"), &Handler);
        return Finder.newASTConsumer();
    }

    bool ParseArgs(const clang::CompilerInstance &, const std::vector<std::string> &) override {
        return true;
    }

    void EndSourceFileAction() override {
        RewriterForCI.getEditBuffer(RewriterForCI.getSourceMgr().getMainFileID()).write(llvm::outs());
    }

private:
    clang::Rewriter RewriterForCI;
    AlwaysInlineHandler Handler{RewriterForCI};
    clang::ast_matchers::MatchFinder Finder;
};

} // namespace

static clang::FrontendPluginRegistry::Add<AlwaysInlineAction>
    X("always-inline-plugin", "Add always_inline attribute to functions without conditions");