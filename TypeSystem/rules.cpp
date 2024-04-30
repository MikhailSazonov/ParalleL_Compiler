#include "rules.hpp"


void TypeMismatchError() {
	// TODO
}


Type RuleVar(const Type& var) {
	return var;
}


Type RuleApp(const Arrow& fun, const Type& arg) {
	if (fun.left != arg) {
		TypeMismatchError();
	}
	return Evaluate(fun.right);
}


Type Evaluate(const Term& term) {
	switch (term.type) {
		case TermType.VAR:
			return RuleVar(term);
		case TermType.ARROW:
			return RuleApp(term);
	}
}
