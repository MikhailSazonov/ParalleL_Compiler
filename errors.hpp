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

class TypeUndefined : public ParalleLCompilerError {
public:
    const char *what() const noexcept {
        return "Type undefined.";
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

class DoubleWhere : public ParalleLCompilerError {
    public:
        const char *what() const noexcept {
            return "Double where.";
        }
};

class AbstractTypeUnresolved : public ParalleLCompilerError {
    public:
        const char *what() const noexcept {
            return "Type unresolved.";
        }
};

class AbstractTypeMismatch : public ParalleLCompilerError {
public:
    const char *what() const noexcept {
        return "Abstract type mismatch.";
    }
};

class DataRaceError : public ParalleLCompilerError {
protected:
    const std::string err;

    DataRaceError(const std::string& err)
        : err(err) {}

public:
    const char *what() const noexcept {
        return err.data();
    }
};

class DataRaceButNotRacing : public DataRaceError {
    public:
        DataRaceButNotRacing(const std::string& name)
            : DataRaceError("Logical data race detected, but func " + name + " is not \"racing.\"")
            {}
};

class CertainDataRace : public DataRaceError {
public:
    CertainDataRace(const std::string& name)
            : DataRaceError("Data race detected for func " + name + ".")
    {}
};

class ColorDataRace : public ParalleLCompilerError {
    const char *what() const noexcept {
        return "Functions color are ill-formed.";
    }
};
