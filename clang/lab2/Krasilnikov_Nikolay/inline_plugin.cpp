#include "clang/AST/AST.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/Tooling.h"

namespace {

    class InliningHandler : public clang::ast_matchers::MatchFinder::MatchCallback {
    public:
    InliningHandler(clang::Rewriter &R) : Rewrite(R) {}

    void run(const clang::ast_matchers::MatchFinder::MatchResult &Result) override {
        const auto *Call = Result.Nodes.getNodeAs<clang::CallExpr>("callExpr");
        if (!Call) {
            return;
        }

        const auto *FuncDecl = Call->getDirectCallee();
        if (
            FuncDecl && 
            FuncDecl->hasBody() && 
            FuncDecl->param_empty() && 
            FuncDecl->getReturnType()->isVoidType()
            ) {
        const auto *FuncBody = FuncDecl->getBody();
        std::string FuncBodyText = clang::Lexer::getSourceText(
                                    clang::CharSourceRange::getTokenRange(FuncBody->getSourceRange()), 
                                    *Result.SourceManager, 
                                    Result.Context->getLangOpts()
                                    ).str();

        if (!FuncBodyText.empty() && FuncBodyText.back() == '\n') {
            FuncBodyText.pop_back();
        }

        Rewrite.InsertText(Call->getBeginLoc(), "{\n" + FuncBodyText + "\n}", true, true);
        Rewrite.ReplaceText(Call->getSourceRange(), "");
        }
    }

    private:
    clang::Rewriter &Rewrite;
    };

    class InliningASTConsumer : public clang::ASTConsumer {
    public:
    InliningASTConsumer(clang::Rewriter &R) : Handler(R) {
        Matcher.addMatcher(
            clang::ast_matchers::callExpr(
                clang::ast_matchers::callee(
                    clang::ast_matchers::functionDecl(
                        clang::ast_matchers::parameterCountIs(0), 
                        clang::ast_matchers::returns(
                                                    clang::ast_matchers::voidType()
                                                    )
                    )
                )
        ).bind("callExpr"), &Handler);
    }

    void HandleTranslationUnit(clang::ASTContext &Ctx) override {
        Matcher.matchAST(Ctx);
    }

    private:
    InliningHandler Handler;
    clang::ast_matchers::MatchFinder Matcher;
    };

    class InliningFrontendAction : public clang::PluginASTAction {
    public:
    std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef) override {
        Rewrite.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
        return std::make_unique<InliningASTConsumer>(Rewrite);
    }

    bool ParseArgs(const clang::CompilerInstance &CI, const std::vector<std::string> &) override {
        return true;
    }

    void EndSourceFileAction() override {
        Rewrite.getEditBuffer(Rewrite.getSourceMgr().getMainFileID()).write(llvm::outs());
    }

    private:
    clang::Rewriter Rewrite;
    };
}


static clang::FrontendPluginRegistry::Add<InliningFrontendAction>
X("inlining-plugin", "Inlines plugin for lab2, variant-2");
