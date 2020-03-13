set(GLOG_SEARCH_PATHS
    /usr
    /usr/local
  )

find_path(GLOG_INCLUDE_DIR glog
    PATH_SUFFIXES include
    PATHS ${GLOG_SEARCH_PATHS}
  )

find_library(GLOG_LIBRARY_TEMP glog
    PATH_SUFFIXES lib64 lib
    PATHS ${GLOG_SEARCH_PATHS}
  )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    glog
    FOUND_VAR GLOG_FOUND
    REQUIRED_VARS GLOG_INCLUDE_DIR GLOG_LIBRARY_TEMP
    VERSION_VAR GLOG_VERSION
  )
