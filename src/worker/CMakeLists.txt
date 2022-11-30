find_package(concurrentqueue)
find_package(fmt)
find_package(jsoncpp)
find_package(ffmpeg)
find_package(SQLite3)
find_package(matroska)

add_library(anworker STATIC)

SET(CPPS
    queue/queue_item.cpp
    queue/queue.cpp
    processor/processor.cpp
    processor/process_sonarr.cpp
    task/sonarr_type.cpp
    task/define_container.cpp
    task/scan_file_for_container.cpp
    task/mkv_scan_contents.cpp
    task/context.cpp
)

target_sources(anworker PRIVATE ${CPPS})

target_include_directories(anworker PUBLIC "." PRIVATE ${matroska_BINARY_DIR})

target_link_libraries(anworker PUBLIC concurrentqueue::concurrentqueue PRIVATE fmt::fmt jsoncpp::jsoncpp ffmpeg::ffmpeg SQLite::SQLite3 matroska)