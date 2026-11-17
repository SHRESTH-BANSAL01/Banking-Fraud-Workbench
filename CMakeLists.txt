
cmake_minimum_required(VERSION 3.16)

project(BankingFraudWorkbench C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(BankingFraudWorkbench

    src/main.c

    src/core/transaction.c

    src/core/dataset.c

    src/core/fraud.c

)
