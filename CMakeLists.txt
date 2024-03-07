cmake_minimum_required(VERSION 3.14)
project(pdq)
include(FindLibXml2)
if(CMAKE_HOST_APPLE)
	set(LibArchive_INCLUDE_DIRS /opt/local/include)
    set(LibArchive_INCLUDE_DIR /opt/local/include)
    set(LibArchive_LIBRARIES /opt/local/lib)
    # Note: the following line seems necessary in CLion 2022.3.3
    # set(PostgreSQL_INCLUDE_DIR /Applications/Postgres.app/Contents/Versions/latest/include/libpq)
    set(PostgreSQL_INCLUDE_DIR /Applications/Postgres.app/Contents/Versions/latest/include)
    set(PostgreSQL_LIBRARY /Applications/Postgres.app/Contents/Versions/latest/lib/libpq.dylib)
endif()
include(FindPostgreSQL)
find_package(PostgreSQL REQUIRED)
find_package(OpenSSL REQUIRED)

# apt-get install libarchive-dev
include_directories(${LibArchive_INCLUDE_DIR})
link_directories(${LibArchive_LIBRARIES})
find_package(LibXml2 REQUIRED)
if(LibXml2_FOUND)
    include_directories(${LIBXML2_INCLUDE_DIR})
    #link_directories(${LIBXML2_LIBRARIES})
endif()
add_executable(pdq pdq.h pdq.c uri.h uri.c parse.h
        jorflegi.h jorflegi.c json.h json.c buffer.h buffer.c
        db.c db.h timestamp.c timestamp.h signature.c signature.h
        delete.c delete.h readarcfile.c readarcfile.h)
target_link_libraries(pdq ${LIBXML2_LIBRARIES} -larchive PostgreSQL::PostgreSQL OpenSSL::SSL)

