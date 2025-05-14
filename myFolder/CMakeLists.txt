    cmake_minimum_required(VERSION 3.10)
    project(UnoCheck)
    
    find_package(raylib REQUIRED)
    
    add_executable(UnoCheck 
    main/main.cpp
    source/Game.cpp
    source/Combined_uno.cpp
    )
    
    target_link_libraries(UnoCheck raylib)
    