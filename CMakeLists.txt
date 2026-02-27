cmake_minimum_required(VERSION 3.14)
project(pdq)
include(FindLibXml2)
include(FindPostgreSQL)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(CMAKE_HOST_APPLE)
    # brew install libarchive
    #
    # libarchive is keg-only, which means it was not symlinked into /opt/homebrew,
    # because macOS already provides this software and installing another version in
    # parallel can cause all kinds of trouble.

    # If you need to have libarchive first in your PATH, run:
    # echo 'export PATH="/opt/homebrew/opt/libarchive/bin:$PATH"' >> ~/.zshrc

    # For compilers to find libarchive you may need to set:
    # export LDFLAGS="-L/opt/homebrew/opt/libarchive/lib"
    # export CPPFLAGS="-I/opt/homebrew/opt/libarchive/include"

    # For pkgconf to find libarchive you may need to set:
    # export PKG_CONFIG_PATH="/opt/homebrew/opt/libarchive/lib/pkgconfig"

	set(LibArchive_INCLUDE_DIRS /opt/homebrew/opt/libarchive/include)
    set(LibArchive_INCLUDE_DIR /opt/homebrew/opt/libarchive/include)
    set(LibArchive_LIBRARIES /opt/homebrew/opt/libarchive/lib)
    # Note: the following line seems necessary in CLion 2022.3.3
    #set(PostgreSQL_INCLUDE_DIR /Applications/Postgres.app/Contents/Versions/latest/include/libpq)
    set(PostgreSQL_INCLUDE_DIR /Applications/Postgres.app/Contents/Versions/latest/include)
    set(PostgreSQL_LIBRARY /Applications/Postgres.app/Contents/Versions/latest/lib/libpq.dylib)
endif()
find_package(PostgreSQL REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(LibXml2 REQUIRED)

# apt-get install libarchive-dev
include_directories(${LibArchive_INCLUDE_DIR})
link_directories(${LibArchive_LIBRARIES})
if(LibXml2_FOUND)
    include_directories(${LIBXML2_INCLUDE_DIR})
    #link_directories(${LIBXML2_LIBRARIES})
endif()
add_executable(pdq pdq.h pdq.c parse.h
        json.h json.c buffer.h buffer.c
        db.c db.h timestamp.c timestamp.h signature.c signature.h
        delete.c delete.h readarcfile.c readarcfile.h)
target_link_libraries(pdq ${LIBXML2_LIBRARIES} -larchive PostgreSQL::PostgreSQL OpenSSL::SSL)
