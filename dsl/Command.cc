#include "dsl/Command.h"
#include "ast/Helpers.h"
#include "ast/ast.h"
#include "core/Context.h"
#include "core/Names/dsl.h"
#include "core/core.h"
#include "dsl/dsl.h"

using namespace std;

namespace ruby_typer {
namespace dsl {

bool isCommand(core::MutableContext ctx, ast::ClassDef *klass) {
    if (klass->kind != ast::Class || klass->ancestors.empty()) {
        return false;
    }
    auto *cnst = ast::cast_tree<ast::ConstantLit>(klass->ancestors.front().get());
    if (cnst == nullptr) {
        return false;
    }
    if (cnst->cnst != ctx.state.enterNameConstant(core::Names::Command())) {
        return false;
    }
    auto *scope = ast::cast_tree<ast::ConstantLit>(cnst->scope.get());
    if (scope == nullptr) {
        return false;
    }
    if (scope->cnst != ctx.state.enterNameConstant(core::Names::Opus())) {
        return false;
    }
    if (ast::isa_tree<ast::EmptyTree>(scope->scope.get())) {
        return true;
    }
    auto *id = ast::cast_tree<ast::Ident>(scope->scope.get());
    if (id == nullptr) {
        return false;
    }
    return id->symbol == core::Symbols::root();
}

void Command::patchDSL(core::MutableContext ctx, ast::ClassDef *klass) {
    if (!isCommand(ctx, klass)) {
        return;
    }

    int i = 0;
    ast::MethodDef *call;

    for (auto &stat : klass->rhs) {
        auto *mdef = ast::cast_tree<ast::MethodDef>(stat.get());
        if (mdef == nullptr) {
            continue;
        }
        if (mdef->name != core::Names::call()) {
            continue;
        }

        i = &stat - &klass->rhs.front();
        call = mdef;
        break;
    }
    // If we didn't find a `call` method, or if it was the first statement (and
    // thus couldn't have a `sig`)
    if (call == nullptr || i == 0) {
        return;
    }

    // Heuristic: Does the previous node look like a `sig`? Check that it's a
    // Send node and so is its receiver.
    //
    // This could in principle be `resolver::TypeSyntax::isSig`, but we don't
    // want to depend on the internals of the resolver, or accidentally rely on
    // passes that happen between here and the resolver.
    auto *sig = ast::cast_tree<ast::Send>(klass->rhs[i - 1].get());
    if (sig == nullptr || !ast::isa_tree<ast::Send>(sig->recv.get())) {
        return;
    }

    ast::MethodDef::ARGS_store newArgs;
    for (auto &arg : call->args) {
        newArgs.emplace_back(arg->deepCopy());
    }

    auto selfCall = ast::MK::Method(call->loc, call->name, move(newArgs), ast::MK::Untyped(call->loc), true);

    klass->rhs.insert(klass->rhs.begin() + i + 1, sig->deepCopy());
    klass->rhs.insert(klass->rhs.begin() + i + 2, move(selfCall));
}

} // namespace dsl
}; // namespace ruby_typer