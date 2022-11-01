cmake_minimum_required(VERSION 3.22)

project(digitalcurling3_gui
    VERSION 0.1.1
    LANGUAGES CXX
)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# use C++ 17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# symbol visiblity
set(CMAKE_CXX_VISIBLIRY_PRESET hidden)
set(CMAKE_VISIBLITY_INLINES_HIDDEN TRUE)


# コミットのハッシュ値とその日付を得る

find_package(Git)

# 参考: https://stackoverflow.com/a/21028226
# # the commit's SHA1, and whether the building workspace was dirty or not
execute_process(COMMAND
  "${GIT_EXECUTABLE}" describe --match=NeVeRmAtCh --always --abbrev=40 --dirty
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  OUTPUT_VARIABLE DIGITALCURLING3_GUI_GIT_SHA1
  ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

# the date of the commit
execute_process(COMMAND
  "${GIT_EXECUTABLE}" log -1 --format=%ad --date=local
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  OUTPUT_VARIABLE DIGITALCURLING3_GUI_GIT_DATE
  ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)


# バージョンファイルを生成
configure_file(src/version.cpp.in version.cpp @ONLY)


# ソース
set(TS_FILES digitalcurling3_gui_ja_JP.ts)
set(PROJECT_SOURCES
  src/coord.hpp
  src/game_log.cpp
  src/game_log.hpp
  src/main.cpp
  src/main_window.cpp
  src/main_window.hpp
  src/main_window.ui
  src/model.cpp
  src/model.hpp
  src/preference_dialog.cpp
  src/preference_dialog.hpp
  src/preference_dialog.ui
  src/score_table_model.cpp
  src/score_table_model.hpp
  src/settings.hpp
  src/settings.cpp
  src/sheet_play_area_widget.cpp
  src/sheet_play_area_widget.hpp
  src/sheet_widget.cpp
  src/sheet_widget.hpp
  src/sheet.cpp
  src/sheet.hpp
  src/shot_log.cpp
  src/shot_log.hpp
  src/slider_widget.cpp
  src/slider_widget.hpp
  src/stone_item_delegate.cpp
  src/stone_item_delegate.hpp
  src/version.hpp
  "${CMAKE_CURRENT_BINARY_DIR}/version.cpp"
  ${TS_FILES}
)


# Qtを設定する
find_package(Qt6 COMPONENTS Widgets LinguistTools REQUIRED)
qt_standard_project_setup()

qt_add_executable(digitalcurling3_gui ${PROJECT_SOURCES})

qt_add_translations(digitalcurling3_gui
  TS_FILES ${TS_FILES}
  LUPDATE_OPTIONS -locations none  # .tsファイルにファイル名を含めないようにする．参考: https://stackoverflow.com/a/29964486
)

target_link_libraries(digitalcurling3_gui PRIVATE Qt6::Widgets)

set_target_properties(digitalcurling3_gui PROPERTIES
  MACOSX_BUNDLE_GUI_IDENTIFIER minerva.cs.uec.ac.jp
  MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
  MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)



# Digital Curling Library
add_subdirectory(extern/DigitalCurling3 EXCLUDE_FROM_ALL)
target_link_libraries(digitalcurling3_gui PRIVATE digitalcurling3::digitalcurling3)


# 自動生成されるファイルからのインクルードがうまくいくように src ディレクトリをインクルードディレクトリにする．
target_include_directories(digitalcurling3_gui
  PRIVATE
    src
)


# リソースファイルの設定
qt_add_resources(digitalcurling3_gui "images"
  PREFIX "/"
  FILES
    images/logo.svg
    images/next_end.svg
    images/next_turn.svg
    images/pause.svg
    images/play.svg
    images/prev_end.svg
    images/prev_turn.svg
    images/hammer.svg
    images/no_hammer.svg
)


install(TARGETS digitalcurling3_gui
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET digitalcurling3_gui
    FILENAME_VARIABLE DIGITALCURLING3_GUI_DEPLOY_SCRIPT
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${DIGITALCURLING3_GUI_DEPLOY_SCRIPT})
