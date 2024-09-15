cmake_minimum_required( VERSION 3.10 )
project( homework11 VERSION 0.1.1 )

find_package( SQLite3 )
if ( SQLITE3_FOUND )
    include_directories( ${SQLITE3_INCLUDE_DIRS})
endif (SQLITE3_FOUND)

file( COPY ../data/inserts.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )

add_executable(sql_server sql_server.cpp server_connection.cpp service.cpp sqlite_wrap.cpp )
target_include_directories( sql_server PUBLIC "${SQLITE3_INCLUDE_DIRS}")
target_link_libraries(sql_server sqlite3 )

add_executable( sql_client sql_client.cpp client_connection.cpp )

# package part which enables
# run: cmake --build . --target package
install(TARGETS sql_server RUNTIME DESTINATION bin)
install(TARGETS sql_client RUNTIME DESTINATION bin)
set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PATCH_VERSION}")
set(CPACK_PACKAGE_CONTACT berrendej@gmail.com)
include(CPack)
