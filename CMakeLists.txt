cmake_minimum_required(VERSION 3.22)
project(testing_compiler)

set(CMAKE_CXX_STANDARD 20)

add_executable(testing_compiler builder.cpp type.cpp utils.cpp testing/testing_type_builder.cpp testing/testing_expr_builder.cpp typecheck.cpp testing/testing_typecheck.cpp parse.cpp main.cpp base_lib/base.cpp base_lib/io.cpp expression.cpp codegen/cpp/codegen.cpp reductor.cpp codegen/cpp/sample.cpp codegen/create_gen.cpp)
