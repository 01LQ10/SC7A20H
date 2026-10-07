cmake_minimum_required(VERSION 3.15)

# 注意：如果使用toolchain.cmake，toolchain必须在project()之前加载
# example: cmake -DCMAKE_TOOLCHAIN_FILE=./toolchain-arm-none-eabi.cmake ...

project(SC7A20H LANGUAGES C)

# ===================== 可配置编译开关 =====================
option(SC7A20H_BUILD_TEST "Build test source code" ON)
option(SC7A20H_BUILD_EXAMPLE "Build example source code" ON)

# ===================== 核心静态库目标 =====================
add_library(sc7a20h STATIC
    src/driver_sc7a20h.c
    src/driver_sc7a20h_fifo.c
    src/driver_sc7a20h_interrupt.c
    src/driver_sc7a20h_self_test.c
)

# 别名，上层项目推荐使用 SC7A20H::SC7A20H 链接
add_library(SC7A20H::SC7A20H ALIAS sc7a20h)

target_compile_features(sc7a20h PUBLIC c_std_99)

# 裸机静态库，关闭PIE，不要生成位置无关代码（单片机不需要）
set_target_properties(sc7a20h PROPERTIES
    POSITION_INDEPENDENT_CODE OFF
)

# 对外公开头文件路径：src驱动头、interface硬件接口层
target_include_directories(sc7a20h
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/interface
)

# ===================== test 测试代码（可选编译，默认OFF） =====================
# if(SC7A20H_BUILD_TEST)
#     add_library(sc7a20h_test STATIC
#         test/driver_sc7a20h_interrupt_test.c
#         test/driver_sc7a20h_read_test.c
#         test/driver_sc7a20h_register_test.c
#     )
#     target_link_libraries(sc7a20h_test PUBLIC SC7A20H::SC7A20H)
#     target_include_directories(sc7a20h_test
#         PUBLIC
#             ${CMAKE_CURRENT_SOURCE_DIR}/test
#     )
#     add_library(SC7A20H::test ALIAS sc7a20h_test)
# endif()

if(C8724Q_BUILD_TEST)
    target_sources(sc7a20h PRIVATE
        test/driver_sc7a20h_interrupt_test.c
        test/driver_sc7a20h_read_test.c
        test/driver_sc7a20h_register_test.c
    )
    target_include_directories(sc7a20h PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/test
    )
endif()
# ===================== example 示例代码（可选编译，默认OFF） =====================
# if(SC7A20H_BUILD_EXAMPLE)
#     add_library(sc7a20h_example STATIC
#         example/driver_sc7a20h_advance.c
#         example/driver_sc7a20h_basic.c
#         example/driver_sc7a20h_interrupt_example.c
#     )
#     target_link_libraries(sc7a20h_example PUBLIC SC7A20H::SC7A20H)
#     target_include_directories(sc7a20h_example
#         PUBLIC
#             ${CMAKE_CURRENT_SOURCE_DIR}/example
#     )
#     add_library(SC7A20H::example ALIAS sc7a20h_example)
# endif()

if(C8724Q_BUILD_EXAMPLE)
    target_sources(sc7a20h PRIVATE
        example/driver_sc7a20h_advance.c
        example/driver_sc7a20h_basic.c
        example/driver_sc7a20h_interrupt_example.c
    )
    target_include_directories(sc7a20h PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/example
    )
endif()