# platform 자체 검증용 테스트 타겟. PLATFORM_BUILD_TESTS=ON일 때만 최상위 CMakeLists.txt에서 add_subdirectory된다.
# (docs/dependency/googletest_20260811.md Step 2 — Wrapper Design 참고)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(platform_tests
    math/Vec2Test.cpp
    math/Vec3Test.cpp
    math/QuaternionTest.cpp
    math/TransformTest.cpp
    math/Matrix4x4Test.cpp

    geometry/AABBTest.cpp
    geometry/OBBTest.cpp
    geometry/IntersectionsTest.cpp
    geometry/GeometryTest.cpp

    collision/gjk_epa/GjkEpaTest.cpp
    collision/sweep_and_prune/SweepAndPruneTest.cpp
    collision/bvh/BvhTest.cpp
    collision/rtree/RTreeTest.cpp
    collision/voxel_grid/VoxelGridTest.cpp
    collision/spatial_hash/SpatialHashTest.cpp
    collision/octree/OctreeTest.cpp
    collision/kdtree/KdTreeTest.cpp
    collision/mpr/MprTest.cpp
    collision/conservative_advancement/ConservativeAdvancementTest.cpp
    collision/fcl/FclTest.cpp

    entity_component/EntityTest.cpp
    entity_component/EntityManagerTest.cpp

    logging/LogFormatterTest.cpp
    logging/ConsoleLogSinkTest.cpp
    logging/FileLogSinkTest.cpp
    logging/LoggerTest.cpp
    logging/LogTest.cpp

    model_import/TinyObjWrapperTest.cpp
    model_import/UfbxWrapperTest.cpp
    model_import/CgltfWrapperTest.cpp
    model_import/ModelLoaderTest.cpp
    model_import/RefCountingCachePolicyTest.cpp
    model_import/LruCachePolicyTest.cpp
    model_import/Win32FileWatcherTest.cpp
    model_import/HotReloadCachePolicyTest.cpp
    model_import/NaiveSignatureDeduplicatorTest.cpp
    model_import/MeshManagerTest.cpp

    persistence/SaveLoadManagerTest.cpp

    serialization/DataRecordAccessTest.cpp
    serialization/JsonDataStoreTest.cpp
    serialization/SqliteDataStoreTest.cpp

    Win32WindowTest.cpp

    engine/DoubleBufferPublisherTest.cpp
)

# platform_lib은 gtest에 무의존 — platform_tests만 gtest_main을 링크한다.
target_link_libraries(platform_tests PRIVATE platform_lib gtest_main)
target_compile_definitions(platform_tests PRIVATE MODEL_IMPORT_FIXTURES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/fixtures/model_import/")

include(GoogleTest)
gtest_discover_tests(platform_tests WORKING_DIRECTORY "$<TARGET_FILE_DIR:platform_tests>")
