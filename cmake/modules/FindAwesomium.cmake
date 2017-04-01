include(FindPackageHandleStandardArgs)

find_path(AWESOMIUM_INCLUDE_DIR Awesomium/WebCore.h
    PATH_SUFFIXES include
    PATHS
    ${AWESOMIUM_ROOT}
    $ENV{AWE_DIR}
    $ENV{AWESOMIUM_ROOT}
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/
    /usr/
    /sw          # Fink
    /opt/local/  # DarwinPorts
    /opt/csw/    # Blastwave
    /opt/)

find_library(AWESOMIUM_LIBRARY
    NAMES libawesomium.so awesomium.lib
    PATH_SUFFIXES lib64 lib build/lib
    PATHS
    ${AWESOMIUM_ROOT}
    $ENV{AWE_DIR}
    $ENV{AWESOMIUM_ROOT}
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/
    /usr/lib/
    /usr/
    /sw          # Fink
    /opt/local/  # DarwinPorts
    /opt/csw/    # Blastwave
    /opt/)

find_package_handle_standard_args(Awesomium DEFAULT_MSG AWESOMIUM_INCLUDE_DIR AWESOMIUM_LIBRARY)
mark_as_advanced(AWESOMIUM_INCLUDE_DIR AWESOMIUM_LIBRARY)

if (AWESOMIUM_FOUND)
    set(AWESOMIUM_LIBRARIES ${AWESOMIUM_LIBRARY})
    set(AWESOMIUM_INCLUDE_DIRS ${AWESOMIUM_INCLUDE_DIR})
endif()