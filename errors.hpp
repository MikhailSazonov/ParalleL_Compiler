#include <stdexcept>

class ParalleLCompilerError : public std::exception {

};

class UndefinedVariableError : public ParalleLCompilerError {
    public:
        const char *what() const noexcept {
            return "Undefined variable.";
        }
};


class UndefinedTypeError : public ParalleLCompilerError {
public:
    const char *what() const noexcept {
        return "Undefined variable.";
    }
};


class TypeMismatchError : public ParalleLCompilerError {
    public:
        const char *what() const noexcept {
            return "Type mismatch.";
        }
};

class DefaultAlreadyDeclared : public ParalleLCompilerError {
    public:
        const char *what() const noexcept {
            return "Default execution already declared.";
        }
};

class IsNotFuncError : public ParalleLCompilerError {
    public:
        const char *what() const noexcept {
            return "The variable is not func.";
        }
};


class TooManyArgsError : public ParalleLCompilerError {
    public:
        const char *what() const noexcept {
            return "Too many args for fun.";
        }
};


class SyntaxError : public ParalleLCompilerError {
    public:
        const char *what() const noexcept {
            return "Some syntax error.";
        }
};


class ImproperName : public SyntaxError {
    public:
        const char *what() const noexcept {
            return "Improper name.";
        }
};


class UnexpectedEndOfString : public SyntaxError {
    public:
        const char *what() const noexcept {
            return "Unexpected end of string.";
        }
};


class UnexpectedSymbol : public SyntaxError {
public:
    const char *what() const noexcept {
        return "Unexpected symbol.";
    }
};


class DoubleDefinition : public ParalleLCompilerError {
    public:
        const char *what() const noexcept {
            return "Double variable definition.";
        }
};

class FileError : public ParalleLCompilerError {

};

class FileNotFoundError : public FileError {
    public:
        const char *what() const noexcept {
            return "File not found.";
        }
};

class FileWrongFormat : public FileError {
    public:
        const char *what() const noexcept {
            return "Wrong file format.";
        }
};


class FailToCreateFile : public FileError {
    public:
        const char *what() const noexcept {
            return "Failed to create output file.";
        }
};
