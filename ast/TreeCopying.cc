#include "ast.h"
#include <type_traits>

using namespace std;

namespace ruby_typer {
namespace ast {

template <class Elem = Expression, class T> T deepCopyVec(const Expression *avoid, const T &origin) {
    T copy;
    for (const auto &memb : origin) {
        copy.emplace_back(cast_tree<Elem>(memb->_deepCopy(avoid).release()));
    };
    return copy;
}
std::unique_ptr<Expression> Expression::deepCopy() const {
    std::unique_ptr<Expression> res;

    try {
        res = this->_deepCopy(this, true);
    } catch (DeepCopyError &e) {
        return nullptr;
    }
    return res;
}

std::unique_ptr<Expression> ClassDef::_deepCopy(const Expression *avoid, bool root) const {
    if (!root && this == avoid) {
        throw DeepCopyError();
    }
    return make_unique<ClassDef>(this->loc, this->symbol, this->name->_deepCopy(avoid),
                                 deepCopyVec(avoid, this->ancestors), deepCopyVec(avoid, this->rhs), this->kind);
}

std::unique_ptr<Expression> MethodDef::_deepCopy(const Expression *avoid, bool root) const {
    if (!root && this == avoid) {
        throw DeepCopyError();
    }
    return make_unique<MethodDef>(this->loc, this->symbol, this->name, deepCopyVec(avoid, this->args),
                                  rhs->_deepCopy(avoid), isSelf);
}

std::unique_ptr<Expression> ConstDef::_deepCopy(const Expression *avoid, bool root) const {
    if (!root && this == avoid) {
        throw DeepCopyError();
    }
    return make_unique<ConstDef>(this->loc, this->symbol, this->rhs->_deepCopy(avoid));
}

std::unique_ptr<Expression> If::_deepCopy(const Expression *avoid, bool root) const {
    if (!root && this == avoid) {
        throw DeepCopyError();
    }
    return make_unique<If>(this->loc, this->cond->_deepCopy(avoid), this->thenp->_deepCopy(avoid),
                           this->elsep->_deepCopy(avoid));
}

std::unique_ptr<Expression> While::_deepCopy(const Expression *avoid, bool root) const {
    if (!root && this == avoid) {
        throw DeepCopyError();
    }
    return make_unique<While>(this->loc, this->cond->_deepCopy(avoid), this->body->_deepCopy(avoid));
}

std::unique_ptr<Expression> Break::_deepCopy(const Expression *avoid, bool root) const {
    if (!root && this == avoid) {
        throw DeepCopyError();
    }
    return make_unique<Break>(this->loc, this->expr->_deepCopy(avoid));
}

std::unique_ptr<Expression> Retry::_deepCopy(const Expression *avoid, bool root) const {
    if (!root && this == avoid) {
        throw DeepCopyError();
    }
    return make_unique<Retry>(this->loc);
}

std::unique_ptr<Expression> Next::_deepCopy(const Expression *avoid, bool root) const {
    if (!root && this == avoid) {
        throw DeepCopyError();
    }
    return make_unique<Next>(this->loc, this->expr->_deepCopy(avoid));
}

std::unique_ptr<Expression> Return::_deepCopy(const Expression *avoid, bool root) const {
    if (!root && this == avoid) {
        throw DeepCopyError();
    }
    return make_unique<Return>(this->loc, this->expr->_deepCopy(avoid));
}

std::unique_ptr<Expression> Yield::_deepCopy(const Expression *avoid, bool root) const {
    if (!root && this == avoid) {
        throw DeepCopyError();
    }
    return make_unique<Yield>(this->loc, this->expr->_deepCopy(avoid));
}

std::unique_ptr<Expression> RescueCase::_deepCopy(const Expression *avoid, bool root) const {
    if (!root && this == avoid) {
        throw DeepCopyError();
    }
    return make_unique<RescueCase>(this->loc, deepCopyVec(avoid, this->exceptions), var->_deepCopy(avoid),
                                   body->_deepCopy(avoid));
}

std::unique_ptr<Expression> Rescue::_deepCopy(const Expression *avoid, bool root) const {
    if (!root && this == avoid) {
        throw DeepCopyError();
    }
    return make_unique<Rescue>(this->loc, this->body->_deepCopy(avoid),
                               deepCopyVec<RescueCase, Rescue::RESCUE_CASE_store>(avoid, rescueCases),
                               else_->_deepCopy(avoid), ensure->_deepCopy(avoid));
}

std::unique_ptr<Expression> Ident::_deepCopy(const Expression *avoid, bool root) const {
    if (!root && this == avoid) {
        throw DeepCopyError();
    }
    return make_unique<Ident>(loc, symbol);
}

std::unique_ptr<Expression> Local::_deepCopy(const Expression *avoid, bool root) const {
    if (!root && this == avoid) {
        throw DeepCopyError();
    }
    return make_unique<Local>(loc, localVariable);
}

std::unique_ptr<Expression> UnresolvedIdent::_deepCopy(const Expression *avoid, bool root) const {
    if (!root && this == avoid) {
        throw DeepCopyError();
    }
    return make_unique<UnresolvedIdent>(loc, kind, name);
}

std::unique_ptr<Expression> RestArg::_deepCopy(const Expression *avoid, bool root) const {
    if (!root && this == avoid) {
        throw DeepCopyError();
    }
    return make_unique<RestArg>(loc, unique_ptr<Reference>(cast_tree<Reference>(expr->_deepCopy(avoid).release())));
}

std::unique_ptr<Expression> KeywordArg::_deepCopy(const Expression *avoid, bool root) const {
    if (!root && this == avoid) {
        throw DeepCopyError();
    }
    return make_unique<KeywordArg>(loc, unique_ptr<Reference>(cast_tree<Reference>(expr->_deepCopy(avoid).release())));
}

std::unique_ptr<Expression> OptionalArg::_deepCopy(const Expression *avoid, bool root) const {
    if (!root && this == avoid) {
        throw DeepCopyError();
    }
    return make_unique<OptionalArg>(loc, unique_ptr<Reference>(cast_tree<Reference>(expr->_deepCopy(avoid).release())),
                                    default_->_deepCopy(avoid));
}

std::unique_ptr<Expression> BlockArg::_deepCopy(const Expression *avoid, bool root) const {
    if (!root && this == avoid) {
        throw DeepCopyError();
    }
    return make_unique<BlockArg>(loc, unique_ptr<Reference>(cast_tree<Reference>(expr->_deepCopy(avoid).release())));
}

std::unique_ptr<Expression> ShadowArg::_deepCopy(const Expression *avoid, bool root) const {
    if (!root && this == avoid) {
        throw DeepCopyError();
    }
    return make_unique<ShadowArg>(loc, unique_ptr<Reference>(cast_tree<Reference>(expr->_deepCopy(avoid).release())));
}

std::unique_ptr<Expression> Assign::_deepCopy(const Expression *avoid, bool root) const {
    if (!root && this == avoid) {
        throw DeepCopyError();
    }
    return make_unique<Assign>(loc, lhs->_deepCopy(avoid), rhs->_deepCopy(avoid));
}

std::unique_ptr<Expression> Send::_deepCopy(const Expression *avoid, bool root) const {
    if (!root && this == avoid) {
        throw DeepCopyError();
    }
    return make_unique<Send>(loc, recv->_deepCopy(avoid), fun, deepCopyVec(avoid, args),
                             block == nullptr ? nullptr
                                              : unique_ptr<Block>(cast_tree<Block>(block->_deepCopy(avoid).release())));
}

std::unique_ptr<Expression> Cast::_deepCopy(const Expression *avoid, bool root) const {
    if (!root && this == avoid) {
        throw DeepCopyError();
    }
    return make_unique<Cast>(loc, type, arg->_deepCopy(avoid), cast);
}

std::unique_ptr<Expression> Hash::_deepCopy(const Expression *avoid, bool root) const {
    if (!root && this == avoid) {
        throw DeepCopyError();
    }
    return make_unique<Hash>(loc, deepCopyVec(avoid, keys), deepCopyVec(avoid, values));
}

std::unique_ptr<Expression> Array::_deepCopy(const Expression *avoid, bool root) const {
    if (!root && this == avoid) {
        throw DeepCopyError();
    }
    return make_unique<Array>(loc, deepCopyVec(avoid, elems));
}

std::unique_ptr<Expression> Literal::_deepCopy(const Expression *avoid, bool root) const {
    if (!root && this == avoid) {
        throw DeepCopyError();
    }
    return make_unique<Literal>(loc, value);
}

std::unique_ptr<Expression> ConstantLit::_deepCopy(const Expression *avoid, bool root) const {
    if (!root && this == avoid) {
        throw DeepCopyError();
    }
    return make_unique<ConstantLit>(loc, scope->_deepCopy(avoid), cnst);
}

std::unique_ptr<Expression> ArraySplat::_deepCopy(const Expression *avoid, bool root) const {
    if (!root && this == avoid) {
        throw DeepCopyError();
    }
    return make_unique<ArraySplat>(loc, arg->_deepCopy(avoid));
}

std::unique_ptr<Expression> HashSplat::_deepCopy(const Expression *avoid, bool root) const {
    if (!root && this == avoid) {
        throw DeepCopyError();
    }
    return make_unique<HashSplat>(loc, arg->_deepCopy(avoid));
}

std::unique_ptr<Expression> ZSuperArgs::_deepCopy(const Expression *avoid, bool root) const {
    if (!root && this == avoid) {
        throw DeepCopyError();
    }
    return make_unique<ZSuperArgs>(loc);
}

std::unique_ptr<Expression> Self::_deepCopy(const Expression *avoid, bool root) const {
    if (!root && this == avoid) {
        throw DeepCopyError();
    }
    return make_unique<Self>(loc, claz);
}

std::unique_ptr<Expression> Block::_deepCopy(const Expression *avoid, bool root) const {
    if (!root && this == avoid) {
        throw DeepCopyError();
    }
    auto res = make_unique<Block>(loc, deepCopyVec(avoid, args), body->_deepCopy(avoid));
    res->symbol = this->symbol;
    return res;
}

std::unique_ptr<Expression> InsSeq::_deepCopy(const Expression *avoid, bool root) const {
    if (!root && this == avoid) {
        throw DeepCopyError();
    }
    return make_unique<InsSeq>(loc, deepCopyVec(avoid, stats), expr->_deepCopy(avoid));
}

std::unique_ptr<Expression> EmptyTree::_deepCopy(const Expression *avoid, bool root) const {
    if (!root && this == avoid) {
        throw DeepCopyError();
    }
    return make_unique<EmptyTree>(loc);
}
std::unique_ptr<Expression> TreeRef::_deepCopy(const Expression *avoid, bool root) const {
    if (!root && this == avoid) {
        throw DeepCopyError();
    }
    if (tree.get() == avoid) {
        throw DeepCopyError();
    }
    if (!tree) {
        throw DeepCopyError();
    }
    return tree->_deepCopy(avoid);
}
} // namespace ast
} // namespace ruby_typer