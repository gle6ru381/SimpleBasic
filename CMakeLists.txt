cmake_minimum_required(VERSION 3.5)

project(SimpleBasic LANGUAGES C)

add_executable(SimpleBasic main.c context.c context.h mystring.c mystring.h instruction.c instruction.h mathTree.c mathTree.h)
