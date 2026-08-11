cmake_minimum_required(VERSION 3.20)

project(platform LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    # 소스 내 한글 주석이 로컬 코드페이지로 오인되어 C4819가 나는 것을 방지 — 소스/실행 문자셋을 UTF-8로 고정.
    add_compile_options(/utf-8)
endif()

# platform 레포 자체 검증용 옵션 — 기본 OFF. 상위(소비) 프로젝트가 켤 의무는 없다.
# (docs/brainstorming/단위테스트_도입_20260811_2033.md, docs/strategy/단위테스트_도입_20260811_2210.md 참고)
option(PLATFORM_BUILD_TESTS "Build platform's own unit tests (self-verification only)" OFF)

include(FetchContent)

# nlohmann::json — JsonDataStore 구현에서만 쓰인다. (World-of-Tank-imitation-Refactoring/docs/dependency/nlohmann_json_20260719.md와 동일 결정)
FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(nlohmann_json)

# tinyobjloader — TinyObjWrapper 구현에서만 쓰인다. v1.0.7의 CMakeLists.txt가 낮은 cmake_minimum_required를
# 선언해 최신 CMake와 충돌하므로 이 서브프로젝트에 한해 정책 최소 버전을 낮춘다.
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
FetchContent_Declare(
    tinyobjloader
    URL https://github.com/tinyobjloader/tinyobjloader/archive/refs/tags/v1.0.7.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(tinyobjloader)
unset(CMAKE_POLICY_VERSION_MINIMUM)

# ufbx — 소비자용 CMakeLists.txt를 제공하지 않는 단일 소스 라이브러리. 소스만 받아오고 ufbx.c를 직접 추가한다.
FetchContent_Declare(
    ufbx
    URL https://github.com/ufbx/ufbx/archive/refs/tags/v0.23.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(ufbx)

# cgltf — 단일 헤더 라이브러리. CgltfWrapper.cpp가 CGLTF_IMPLEMENTATION을 정의해 구현부를 생성하므로
# 여기서는 include 경로만 추가한다.
FetchContent_Declare(
    cgltf
    URL https://github.com/jkuhlmann/cgltf/archive/refs/tags/v1.15.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(cgltf)

# 기존 소스가 전부 "platform/xxx/Yyy.h" 형태로 스스로를 include하는 레포 전체 관례를 따른다
# (예: math/Matrix4x4.cpp -> #include "platform/math/Matrix4x4.h").
# 이 관례를 그대로 만족시키기 위해 include root를 이 레포의 부모 디렉터리로 잡는다 —
# 이 레포가 GitHub 저장소명과 동일한 "platform" 폴더명으로 체크아웃되어 있다고 전제한다
# (WOT 쪽 서브모듈 경로 src/platform과 동일 전제).
get_filename_component(_platform_repo_parent_dir "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)

add_library(platform_lib STATIC
    logging/LogFormatter.cpp
    logging/ConsoleLogSink.cpp
    logging/FileLogSink.cpp
    logging/Logger.cpp
    logging/Log.cpp
    Win32Window.cpp
    serialization/DataRecordAccess.cpp
    serialization/JsonDataStore.cpp
    serialization/SqliteDataStore.cpp
    persistence/SaveLoadManager.cpp
    math/Matrix4x4.cpp
    geometry/Intersections.cpp
    geometry/Geometry.cpp
    collision/gjk_epa/Gjk.cpp
    collision/gjk_epa/Epa.cpp
    collision/gjk_epa/GjkEpa.cpp
    collision/sweep_and_prune/SweepAndPrune.cpp
    collision/bvh/Bvh.cpp
    collision/rtree/RTree.cpp
    collision/voxel_grid/VoxelGrid.cpp
    collision/spatial_hash/SpatialHash.cpp
    collision/octree/Octree.cpp
    collision/kdtree/KdTree.cpp
    collision/mpr/Mpr.cpp
    collision/conservative_advancement/ConservativeAdvancement.cpp
    collision/fcl/Fcl.cpp
    entity_component/Entity.cpp
    entity_component/EntityManager.cpp
    model_import/TinyObjWrapper.cpp
    model_import/UfbxWrapper.cpp
    model_import/CgltfWrapper.cpp
    model_import/ModelLoader.cpp
    model_import/RefCountingCachePolicy.cpp
    model_import/LruCachePolicy.cpp
    model_import/Win32FileWatcher.cpp
    model_import/HotReloadCachePolicy.cpp
    model_import/NaiveSignatureDeduplicator.cpp
    model_import/MeshManager.cpp
    ${ufbx_SOURCE_DIR}/ufbx.c
)
target_include_directories(platform_lib PUBLIC "${_platform_repo_parent_dir}")
target_include_directories(platform_lib PRIVATE ${ufbx_SOURCE_DIR} ${cgltf_SOURCE_DIR})
target_link_libraries(platform_lib PRIVATE nlohmann_json::nlohmann_json tinyobjloader)

if(PLATFORM_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
