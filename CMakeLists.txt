project(codal-microbit)

include("${CODAL_UTILS_LOCATION}")

# find sources and headers
RECURSIVE_FIND_DIR(INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/inc" "*.h")
RECURSIVE_FIND_FILE(SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/source" "*.c??")

RECURSIVE_FIND_DIR(INCLUDE_DIRS2 "${CMAKE_CURRENT_SOURCE_DIR}/model" "*.h")
RECURSIVE_FIND_FILE(SOURCE_FILES2 "${CMAKE_CURRENT_SOURCE_DIR}/model" "*.c??")

list(APPEND INCLUDE_DIRS "${INCLUDE_DIRS2}")
list(APPEND SOURCE_FILES "${SOURCE_FILES2}")
list(APPEND SOURCE_FILES "./asm/nrf51822.s")

list(FIND CODAL_FIELDS "NO_BLE" TMP_RES)

message("${CODAL_FIELDS} ${TMP_RES}")

if ("${TMP_RES}" EQUAL "-1")
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -T\"${CMAKE_CURRENT_LIST_DIR}/ld/nrf51822.ld\"" PARENT_SCOPE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T\"${CMAKE_CURRENT_LIST_DIR}/ld/nrf51822.ld\"" PARENT_SCOPE)
else()
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -T\"${CMAKE_CURRENT_LIST_DIR}/ld/nrf51822-no-softdevice.ld\"" PARENT_SCOPE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T\"${CMAKE_CURRENT_LIST_DIR}/ld/nrf51822-no-softdevice.ld\"" PARENT_SCOPE)
    set(device.post_process "" PARENT_SCOPE)
    message("Ignoring post process")
endif()

# exit()
set(CMAKE_SYSTEM_PROCESSOR "armv6-m" PARENT_SCOPE)

# add them
include_directories(${INCLUDE_DIRS})

# create our target
add_library(codal-microbit ${SOURCE_FILES})

target_link_libraries(
    codal-microbit
    codal-mbed
    ble-nrf51822
    nrf51-sdk
    ${LIB_OBJECT_FILES}
    ${LIB_ARCHIVE_FILES}
)

# expose it to parent cmake.
target_include_directories(codal-microbit PUBLIC ${INCLUDE_DIRS})
