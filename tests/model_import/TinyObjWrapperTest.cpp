#include <gtest/gtest.h>

#include <stdexcept>

#include "FixturePathHelper.h"
#include "platform/model_import/TinyObjWrapper.h"

TEST(TinyObjWrapperTest, LoadProducesTriangulatedCubeMesh)
{
    const TinyObjWrapper wrapper;
    const ModelMesh mesh = wrapper.Load(FixturePath("cube.obj"));

    EXPECT_EQ(mesh.positions.size(), 36u);
    EXPECT_EQ(mesh.normals.size(), 36u);
    ASSERT_EQ(mesh.uvChannels.size(), 1u);
    EXPECT_EQ(mesh.uvChannels[0].size(), 36u);
    EXPECT_EQ(mesh.indices.size(), 36u);
    EXPECT_EQ(mesh.indices.size() % 3, 0u);
}

TEST(TinyObjWrapperTest, LoadThrowsOnMissingFile)
{
    const TinyObjWrapper wrapper;
    EXPECT_THROW(wrapper.Load(FixturePath("does_not_exist.obj")), std::runtime_error);
}
