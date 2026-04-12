include(GoogleTest)

set(SCGEOMETRY_CAPABILITY_TEST_SOURCES
    capabilities/test_3d_brep.cpp
    capabilities/test_3d_body_boolean_sdk.cpp
    capabilities/test_3d_conversion.cpp
    capabilities/test_3d_healing.cpp
    capabilities/test_3d_section.cpp
    capabilities/test_box.cpp
    capabilities/test_offset.cpp
    capabilities/test_point_vector.cpp
    capabilities/test_polygon.cpp
    capabilities/test_polyline.cpp
    capabilities/test_relation_boolean.cpp
    capabilities/test_searchpoly_sdk.cpp
    capabilities/test_sdk.cpp
    capabilities/test_sdk_algorithms.cpp
    capabilities/test_sdk_multigeometry.cpp
    capabilities/test_segment.cpp
    capabilities/test_serialize.cpp
    capabilities/test_shapes_pathops.cpp
    capabilities/test_topology_indexing.cpp
    capabilities/test_transform_sampling.cpp)

set(SCGEOMETRY_GAP_TEST_SOURCES
    gaps/test_3d_section_gaps.cpp
    gaps/test_3d_brep_gaps.cpp
    gaps/test_3d_body_boolean_gaps.cpp
    gaps/test_3d_healing_gaps.cpp
    gaps/test_3d_conversion_gaps.cpp
    gaps/test_offset_gaps.cpp
    gaps/test_searchpoly_gaps.cpp
    gaps/test_relation_gaps.cpp)

set(SCGEOMETRY_CAPABILITY_TESTS_TARGET SCGeometryCapabilitiesTests)
set(SCGEOMETRY_GAP_TESTS_TARGET SCGeometryGapTests)

add_executable(${SCGEOMETRY_CAPABILITY_TESTS_TARGET} ${SCGEOMETRY_CAPABILITY_TEST_SOURCES})
target_link_libraries(${SCGEOMETRY_CAPABILITY_TESTS_TARGET} PRIVATE ${SCGEOMETRY_TARGET} GTest::gtest GTest::gtest_main)
target_include_directories(${SCGEOMETRY_CAPABILITY_TESTS_TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
SCGeometryConfigureTarget(${SCGEOMETRY_CAPABILITY_TESTS_TARGET})
SCCopyRuntimeDependenciesToTarget(${SCGEOMETRY_CAPABILITY_TESTS_TARGET} ${SCGEOMETRY_TARGET})
gtest_discover_tests(${SCGEOMETRY_CAPABILITY_TESTS_TARGET})

add_executable(${SCGEOMETRY_GAP_TESTS_TARGET} ${SCGEOMETRY_GAP_TEST_SOURCES})
target_link_libraries(${SCGEOMETRY_GAP_TESTS_TARGET} PRIVATE ${SCGEOMETRY_TARGET} GTest::gtest GTest::gtest_main)
target_include_directories(${SCGEOMETRY_GAP_TESTS_TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
SCGeometryConfigureTarget(${SCGEOMETRY_GAP_TESTS_TARGET})
SCCopyRuntimeDependenciesToTarget(${SCGEOMETRY_GAP_TESTS_TARGET} ${SCGEOMETRY_TARGET})
gtest_discover_tests(${SCGEOMETRY_GAP_TESTS_TARGET})
