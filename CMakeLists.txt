cmake_minimum_required(VERSION 3.24)
project(infinity_compiler C)

set(CMAKE_C_STANDARD 23)

add_executable(infinity_compiler main.c config/globals.c config/globals.h lexer/lexer.c lexer/lexer.h token/token.c token/token.h list/list.c list/list.h compiler/compiler.c compiler/compiler.h io/io.c io/io.h types/types.c types/types.h ast/ast.c ast/ast.h variable/variable.c variable/variable.h logging/logging.c logging/logging.h parser/parser.c parser/parser.h)
