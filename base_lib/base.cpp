#include "base.hpp"

#include "../errors.hpp"
#include "../definitions.hpp"

#include "io.hpp"
#include "alg.hpp"
#include "operators.hpp"

#include <thread>

bool BaseLib::CheckTypeExistence(const std::string& typeName, const Def::ClassTable& classTable) {
    return IsDefaultType(typeName) ||
        std::any_of(classTable.begin(), classTable.end(),
        [&typeName](const Class& cl) {
            return cl.name == typeName;
        });
}

bool BaseLib::IsDefaultType(const std::string& typeName) {
    return BASE_TYPES.contains(typeName);
}

bool BaseLib::IsAbstractType(const std::string& src) {
    return std::all_of(src.begin(), src.end(), [](char c) {
        return 'a' <= c && c <= 'z';
    });
}

std::optional<BaseLib::NativeDesc> BaseLib::GetNative(const std::string& name) {
    std::optional<BaseLib::NativeDesc> result;
    if ((result = BaseLib::Io::GetNative(name)).has_value() ||
    (result = BaseLib::Ops::GetNative(name)).has_value()) {
        return result;
    }
    return BaseLib::Alg::GetNative(name);
}

void BaseLib::LoadBaseTypes(Def::TypeTable& typeTable) {
    typeTable["main"] = std::make_shared<Pod>("Void");
    BaseLib::LoadStandardTypes(typeTable);
    BaseLib::Io::LoadBaseTypes(typeTable);
    BaseLib::Ops::LoadBaseTypes(typeTable);
    BaseLib::Alg::LoadBaseTypes(typeTable);
}

void BaseLib::LoadStandardTypes(Def::TypeTable& typeTable) {
    for (const auto& [typeName, _] : BASE_TYPES) {
        typeTable[typeName] = std::make_shared<Pod>(typeName);
    }
}

std::string BaseLib::GetBaseCode(std::unordered_set<std::string>& includes) {
    includes.insert("#include <tuple>\n");
    includes.insert("#include <memory>\n");
    return "template <typename R, typename... Args>\n"
           "struct Data {\n"
           "    void* ptr;\n"
           "    R(*fun)(std::tuple<Args...>&);\n"
           "    std::tuple<Args...> args;\n"
           "    bool heapAllocated{false};\n"
           "    size_t refCount{0};\n"
           "};\n"
           "\n"
           "\n"
           "template <typename T, typename U>\n"
           "struct Func {\n"
           "    struct ControlBlock {\n"
           "        virtual U Call(T&&) = 0;\n"
           "\n"
           "        virtual void Replicate() = 0;\n"
           "    };\n"
           "\n"
           "\n"
           "    using type = U;\n"
           "\n"
           "    template <typename R, size_t idx, typename... Args>\n"
           "    struct FunctionBlock : public ControlBlock {\n"
           "        FunctionBlock() {\n"
           "            auto* data = GetData();\n"
           "            if (data->heapAllocated) {\n"
           "                ++data->refCount;\n"
           "            }\n"
           "        }\n"
           "\n"
           "        ~FunctionBlock() {\n"
           "            auto* data = GetData();\n"
           "            if (data->heapAllocated) {\n"
           "                --data->refCount;\n"
           "                if (data->refCount == 0) {\n"
           "                    delete data;\n"
           "                }\n"
           "            }\n"
           "        }\n"
           "\n"
           "        U Call(T&& arg) {\n"
           "            auto* data = GetData();\n"
           "            std::get<idx>(data->args) = std::move(arg);\n"
           "            if constexpr(std::is_same_v<U, R>) {\n"
           "                return data->fun(data->args);\n"
           "            } else {\n"
           "                return U{}.template Set<R, idx + 1, Args...>(data);\n"
           "            }\n"
           "        }\n"
           "\n"
           "        Data<R, Args...>* GetData() {\n"
           "            return (Data<R, Args...>*)((void*)this);\n"
           "        }\n"
           "\n"
           "        void Replicate() {\n"
           "            auto* data = GetData();\n"
           "            auto* newData = new Data<R, Args...>(*data);\n"
           "            newData->heapAllocated = true;\n"
           "            newData->refCount = 1;\n"
           "        }\n"
           "    };\n"
           "\n"
           "    Func() = default;\n"
           "\n"
           "    template <typename R, typename... Args>\n"
           "    Func(Data<R, Args...>* data) : fn(new(&data->ptr) FunctionBlock<R, 0, Args...>()) {}\n"
           "\n"
           "    U Eval(T&& arg) {\n"
           "        return fn->Call(std::move(arg));\n"
           "    }\n"
           "\n"
           "    auto&& Eval() {\n"
           "        return std::move(*this);\n"
           "    }\n"
           "\n"
           "    template <typename F, size_t idx, typename... Args>\n"
           "    auto Set(Data<F, Args...>* data) && {\n"
           "        fn = new(&data->ptr) FunctionBlock<F, idx, Args...>();\n"
           "        return *this;\n"
           "    }\n"
           "\n"
           "    auto& Replicate() {\n"
           "        fn->Replicate();\n"
           "        return *this;\n"
           "    }\n"
           "\n"
           "    ControlBlock* fn;\n"
           "};\n"
           "\n"
           "\n"
           "template <typename U>\n"
           "struct Func<void, U> {\n"
           "    struct ControlBlock {\n"
           "        virtual U Call() = 0;\n"
           "    };\n"
           "\n"
           "\n"
           "    using type = U;\n"
           "\n"
           "    template <typename F, size_t idx, typename... Args>\n"
           "    struct FunctionBlock : public ControlBlock {\n"
           "        U Call() {\n"
           "            auto* data = (Data<F, Args...>*)((void*)this);\n"
           "            return data->fun(data->args);\n"
           "        }\n"
           "    };\n"
           "\n"
           "    template <typename F, size_t idx, typename... Args>\n"
           "    auto Set(Data<F, Args...>* data) {\n"
           "        fn = new(&data->ptr) FunctionBlock<F, idx, Args...>();\n"
           "        return *this;\n"
           "    }\n"
           "\n"
           "    U Eval() {\n"
           "        return fn->Call();\n"
           "    }\n"
           "\n"
           "    ControlBlock* fn;\n"
           "};\n"
           "\n"
           "struct Unit {};\n"
           "\n"
           "template <typename T>\n"
           "struct SimpleContainer {\n"
           "    T value;\n"
           "\n"
           "    auto& Eval() {\n"
           "        return *this;\n"
           "    }\n"
           "\n"
           "    auto& Replicate() {\n"
           "        return *this;\n"
           "    }\n"
           "\n"
           "};\n"
           "\n"
           "template <typename T>\n"
           "struct PtrContainer {\n"
           "    T* ptr{nullptr};\n"
           "\n"
           "    auto& Eval() {\n"
           "        return *this;\n"
           "    }\n"
           "\n"
           "    PtrContainer<T> Copy(void* newStorage) {\n"
           "        if (!ptr) {\n"
           "            return *this;\n"
           "        }\n"
           "        new(newStorage)T(Get());\n"
           "        return PtrContainer<T>{(T*)newStorage};\n"
           "    }\n"
           "\n"
           "    auto& Get() {\n"
           "        if (ptr == nullptr) {\n"
           "            ptr = new T();\n"
           "        }\n"
           "        return *ptr;\n"
           "    }\n"
           "\n"
           "    auto& Replicate() {\n"
           "        return *this;\n"
           "    }\n"
           "\n"
           "    constexpr size_t Size() {\n"
           "        return sizeof(T);\n"
           "    }\n"
           "};\n"
           "\n";
}

std::string BaseLib::GetMTCode(std::unordered_set<std::string>& includes) {
    includes.insert("#include <thread>\n");
    includes.insert("#include <atomic>\n");
    includes.insert("#include <vector>\n");
    includes.insert("#include <condition_variable>\n");
    includes.insert("#include <mutex>\n");
    includes.insert("#include <optional>\n");
    includes.insert("#ifdef __linux__\n"
                    "#include <sched.h>\n"
                    "#elif _WIN32\n"
                    "#include <processthreadsapi.h>\n"
                    "#include <windows.h>\n"
                    "#endif\n");
    size_t concNum = std::thread::hardware_concurrency();
    std::string code = "const size_t SPIN_COUNT{1 << 13};\n"
                       "\n"
                       "void SpinAndWait() {\n"
                       "    for (size_t i = 0; i < SPIN_COUNT; ++i) {}\n"
                       "    std::this_thread::yield();\n"
                       "}\n\n"
                       "struct Functor {\n"
                       "    struct ControlBlock {\n"
                       "        virtual void Call() = 0;\n"
                       "    };\n"
                       "\n"
                       "    template <typename R, typename C>\n"
                       "    struct FunctionBlock : public ControlBlock {\n"
                       "        R* ret_;\n"
                       "        R(C::*fun_)();\n"
                       "        C* caller;\n"
                       "        std::atomic<uint64_t>* cntr;\n"
                       "\n"
                       "        FunctionBlock(R* ret, R(C::*fun)(), C* c, std::atomic<uint64_t>* cnt)\n"
                       "                : ret_(ret), fun_(fun), caller(c), cntr(cnt) {}\n"
                       "\n"
                       "        void Call() {\n"
                       "            *ret_ = (*caller.*fun_)();\n"
                       "            cntr->fetch_add(1, std::memory_order_release);\n"
                       "        }\n"
                       "    };\n"
                       "\n"
                       "    template <typename R, typename C>\n"
                       "    void Load(void* ptr, R* ret, R(C::*fun)(), C* c, std::atomic<size_t>* cnt) {\n"
                       "        cb = new(ptr) FunctionBlock<R, C>(ret, fun, c, cnt);\n"
                       "    }\n"
                       "\n"
                       "    ControlBlock* cb;\n"
                       "};\n"
                       "\n"
                       "thread_local size_t currentThreadNo;\n"
                       "std::pair<size_t, size_t> threadBounds[%i];\n"
                       "thread_local std::vector<std::pair<size_t, size_t>> prevBoundsStack;\n"
                       "thread_local size_t threadsQueue[%i] = {(size_t)-1};\n"
                       "thread_local size_t currentQueueIdx{0};\n"
                       "\n"
                       "struct PLThread {\n"
                       "    bool exit{false};"
                       "    std::mutex mt;\n"
                       "    std::condition_variable cv;\n"
                       "    Functor f;\n"
                       "};\n"
                       "\n"
                       "template <size_t N>\n"
                       "class ThreadPool {\n"
                       "public:\n"
                       "    ThreadPool() {\n"
                       "        currentThreadNo = 0;\n"
                       "        threadBounds[0] = {0, N - 1};\n"
                       "#ifdef __linux__\n"
                       "        cpu_set_t mask;\n"
                       "        CPU_ZERO(&mask);\n"
                       "        CPU_SET(1, &mask);\n"
                       "        sched_setaffinity(0, sizeof(mask), &mask);\n"
                       "#elif _WIN32\n"
                       "        SetThreadAffinityMask(GetCurrentThread(),(1));\n"
                       "#endif\n"
                       "        for (size_t i = 1; i < N; ++i) {\n"
                       "            workers[i].emplace([pt = &infos[i], this, i]() {\n"
                       "                currentThreadNo = i;\n"
                       "                pt->f.cb = nullptr;\n"
                       "                pt->exit = false;\n"
                       "#ifdef __linux__\n"
                       "                cpu_set_t mask;\n"
                       "                CPU_ZERO(&mask);\n"
                       "                CPU_SET(1 << i, &mask);\n"
                       "                sched_setaffinity(0, sizeof(mask), &mask);\n"
                       "#elif _WIN32\n"
                       "                SetThreadAffinityMask(GetCurrentThread(),(1<<i));\n"
                       "#endif\n"
                       "                Process(*pt);\n"
                       "            });\n"
                       "        }\n"
                       "    }\n"
                       "    void CutTheBounds(size_t pieces) {\n"
                       "        auto& curThreadBounds = threadBounds[currentThreadNo];\n"
                       "        prevBoundsStack.push_back(curThreadBounds);\n"
                       "\n"
                       "        size_t baseValue = (curThreadBounds.second - curThreadBounds.first + 1) / pieces;\n"
                       "        size_t additionalValue = (curThreadBounds.second - curThreadBounds.first + 1) % pieces;\n"
                       "        size_t start = threadBounds[currentThreadNo].first;\n"
                       "\n"
                       "        size_t j = 0;\n"
                       "        size_t end = 0;\n"
                       "\n"
                       "        size_t fin = threadBounds[currentThreadNo].second;\n"
                       "        while (end < fin) {\n"
                       "            end = start + baseValue + (additionalValue > 0 ? 1 : 0) - 1;\n"
                       "            threadBounds[start] = {start, end};\n"
                       "            additionalValue = (additionalValue > 0 ? additionalValue - 1 : 0);\n"
                       "\n"
                       "            threadsQueue[pieces - j - 1] = start;\n"
                       "            start = end + 1;\n"
                       "            ++j;\n"
                       "        }\n"
                       "    }\n"
                       "\n"
                       "    void PopTheBounds() {\n"
                       "        threadBounds[currentThreadNo] = prevBoundsStack.back();\n"
                       "        prevBoundsStack.pop_back();\n"
                       "    }\n"
                       "\n"
                       "    template <typename R, typename C>\n"
                       "    void Reset(void* ptr, R* ret, R(C::*fun)(), C* caller, std::atomic<size_t>* cnt) {\n"
                       "        size_t ctn = currentThreadNo;\n"
                       "        size_t nextThread = threadsQueue[currentQueueIdx];\n"
                       "\n"
                       "        if (nextThread == -1 || nextThread == currentThreadNo) {\n"
                       "            *ret = (*caller.*fun)();\n"
                       "            cnt->fetch_add(1, std::memory_order_release);\n"
                       "            return;\n"
                       "        }\n"
                       "\n"
                       "        std::unique_lock<std::mutex> lk(infos[nextThread].mt);\n"
                       "        infos[nextThread].f.Load(ptr, ret, fun, caller, cnt);\n"
                       "        infos[nextThread].cv.notify_one();\n"
                       "        ++currentQueueIdx;\n"
                       "    }\n"
                       "\n"
                       "    void Process(PLThread& pt) {\n"
                       "        std::unique_lock<std::mutex> lk(pt.mt);\n"
                       "        for (;;) {\n"
                       "            pt.cv.wait(lk, [&pt]() {\n"
                       "                return pt.exit || pt.f.cb != nullptr;\n"
                       "            });\n"
                       "            if (pt.exit) {\n"
                       "                break;\n"
                       "            }\n"
                       "            pt.f.cb->Call();\n"
                       "            pt.f.cb = nullptr;\n"
                       "        }\n"
                       "    }\n"
                       "\n"
                       "    void Stop() {\n"
                       "        for (size_t i = 1; i < N; ++i) {\n"
                       "            {\n"
                       "                std::unique_lock<std::mutex> lk(infos[i].mt);\n"
                       "                infos[i].exit = true;\n"
                       "                infos[i].cv.notify_one();\n"
                       "            }\n"
                       "            workers[i]->join();\n"
                       "        }\n"
                       "        for (size_t i = 1; i < N; ++i) {\n"
                       "            workers[i]->join();\n"
                       "        }\n"
                       "    }\n"
                       "\n"
                       "\n"
                       "private:\n"
                       "    std::optional<std::thread> workers[N];\n"
                       "    PLThread infos[N];\n"
                       "};\n"
                       "\n"
                       "ThreadPool<%i> tp;\n";
    size_t pos = 0;
    while ((pos = code.find("%i", pos)) != std::string::npos) {
        code.replace(pos, 2, std::to_string(concNum));
    }
    return code;
}

std::string BaseLib::GetMTPreludeCode(const std::string& counterName, const std::string& pieces) {
    std::string result = "    std::atomic<uint64_t> %s1{0};\n"
                         "\n"
                         "    tp.CutTheBounds(%s2);\n";
    result.replace(result.find("%s1"), 3, counterName);
    result.replace(result.find("%s2"), 3, pieces);
    return result;
}

std::string BaseLib::GetDataCode(const std::string& sign, const std::string& argNo,
const std::string& arg) {
    std::string result = "    Data<%s1> data%s2{.fun = &%s3};\n";
    result.replace(result.find("%s1"), 3, sign);
    result.replace(result.find("%s2"), 3, argNo);
    result.replace(result.find("%s3"), 3, arg);
    return result;
}

std::string BaseLib::GetFuncSTCode(const std::string& argNo, const std::string& expr) {
    std::string result = "    auto arg%s2ret = %s1.Eval();\n";
    result.replace(result.find("%s2"), 3, argNo);
    result.replace(result.find("%s1"), 3, expr);
    return result;
}


std::string BaseLib::GetFuncMTCode(const std::string& argNo, const std::string& expr) {
    std::string result = "    auto arg%s2 = %s1";
    result.replace(result.find("%s1"), 3, expr);
    result.replace(result.find("%s2"), 3, argNo);
    return result;
}

std::string BaseLib::GetEvalMTCode(const std::string& argNo,
const std::string& counterName) {
    std::string result = "    void* arg%s1storage[6];\n"
                         "    decltype(arg%s1)::type arg%s1ret;\n"
                         "    tp.Reset(arg%s1storage, &arg%s1ret, &decltype(arg%s1)::Eval, &arg%s1, &%s3);\n\n";
    size_t pos = 0;
    while ((pos = result.find("%s1", pos)) != std::string::npos) {
        result.replace(pos, 3, argNo);
    }
    result.replace(result.find("%s3"), 3, counterName);
    return result;
}

std::string BaseLib::GetMTCodaCode(const std::string& counterName, const std::string& pieces) {
    std::string result = "    while (%s1.load() < %s2) {\n"
                         "        SpinAndWait();\n"
                         "    }\n"
                         "\n"
                         "    tp.PopTheBounds();\n";
    result.replace(result.find("%s1"), 3, counterName);
    result.replace(result.find("%s2"), 3, pieces);
    return result;
}

std::string BaseLib::GenerateClassCode(const std::string& className,
const std::vector<std::pair<std::string, std::string>>& classVars) {
    std::string result = "struct %s1Gen {\n"
                         "    auto& Eval() {\n"
                         "        return *this;\n"
                         "    }\n"
                         "\n";
    result.replace(result.find("%s1"), 3, className);

    std::string genTemplate =   "    %s1 %s2;\n"
                                "\n"
                                "    auto& Get%s2() {\n"
                                "        return %s2;\n"
                                "    }\n"
                                "\n";
    std::string getTemplateRecursive = "    PtrContainer<%s1Gen> %s2;\n"
                                       "\n"
                                       "    auto& Get%s2() {\n"
                                       "        return %s2;\n"
                                       "    }\n\n";

    for (const auto& nextField : classVars) {
        std::string templateCopy;
        if (nextField.first == className) {
            templateCopy = getTemplateRecursive;
        } else {
            templateCopy = genTemplate;
        }
        size_t pos = 0;
        while ((pos = templateCopy.find("%s1")) != std::string::npos) {
            templateCopy.replace(pos, 3, nextField.first);
        }
        while ((pos = templateCopy.find("%s2")) != std::string::npos) {
            templateCopy.replace(pos, 3, nextField.second);
        }
        result += templateCopy;
    }
    std::string coda = "};\n\ntypedef PtrContainer<%s1Gen> %s1;\n\n";
    coda.replace(coda.find("%s1"), 3, className);
    coda.replace(coda.find("%s1"), 3, className);
    result += coda;
    return result;
}

std::string BaseLib::GenerateConstCode(const std::string& constant, const std::string& type) {
    std::string result = "%s1{%s2}";
    result.replace(result.find("%s1"), 3, type);
    if (type != "Void") {
        result.replace(result.find("%s2"), 3, constant);
    } else {
        result.replace(result.find("%s2"), 3, "");
    }
    return result;
}

std::string BaseLib::GenerateTemplateFromTemplate(const std::unordered_set<std::string>& absTypes) {
    if (absTypes.empty()) {
        return "";
    }
    std::string templateStr = "template <typename %s1>\n";
    size_t i = 0;
    for (const auto& absType : absTypes) {
        auto replacePos = templateStr.find("%s1");
        if (i < absTypes.size() - 1) {
            auto leftSubstr = templateStr.substr(0, replacePos);
            auto rightSubstr = templateStr.substr(replacePos);
            templateStr = leftSubstr + absType + ", " + rightSubstr;
        } else {
            templateStr.replace(replacePos, 3, absType);
        }
        ++i;
    }
    return templateStr;
}

std::string BaseLib::GenerateFbipCode(const std::string& num, const std::string& src, bool first) {
    if (first) {
        std::string templateCode =  "        auto copy%s1 = %s2;\n";
        templateCode.replace(templateCode.find("%s1"), 3, num);
        templateCode.replace(templateCode.find("%s2"), 3, src);
        return templateCode;
    }
    std::string templateCode = "        char storage%s1[%s2.Size()];\n"
                               "        auto copy%s1 = %s2.Copy(&storage%s1[0]);\n";
    templateCode.replace(templateCode.find("%s1"), 3, num);
    templateCode.replace(templateCode.find("%s1"), 3, num);
    templateCode.replace(templateCode.find("%s1"), 3, num);
    templateCode.replace(templateCode.find("%s2"), 3, src);
    templateCode.replace(templateCode.find("%s2"), 3, src);
    return templateCode;
}

std::string BaseLib::GeneratePackedClassCode(const std::vector<std::string>& exprsCode,
const std::vector<std::pair<std::string, std::string>>& values,
const std::string& type, const std::string& name) {
    std::string result;

    std::string baseCode = "    %s1 %s2;\n";
    baseCode.replace(baseCode.find("%s1"), 3, type);
    baseCode.replace(baseCode.find("%s2"), 3, name);
    result += baseCode;

    std::string templateCode = "    %s2.Get().%s3 = %s4;\n";
    templateCode.replace(templateCode.find("%s2"), 3, name);
    for (size_t i = 0; i < exprsCode.size(); ++i) {
        auto copyCode = templateCode;
        copyCode.replace(copyCode.find("%s3"), 3, values[i].second);
        copyCode.replace(copyCode.find("%s4"), 3, exprsCode[i]);
        result += copyCode;
    }
    return result;
}
