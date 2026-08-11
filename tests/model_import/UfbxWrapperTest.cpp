#include <gtest/gtest.h>

#include <stdexcept>

#include "FixturePathHelper.h"
#include "platform/model_import/UfbxWrapper.h"

TEST(UfbxWrapperTest, LoadProducesAtLeastOneTriangulatedMesh)
{
    const UfbxWrapper wrapper;
    const Model model = wrapper.Load(FixturePath("cube.fbx"));

    ASSERT_FALSE(model.meshes.empty());

    bool foundNonEmptyMesh = false;
    for (const std::shared_ptr<const ModelMesh>& mesh : model.meshes)
    {
        if (!mesh->positions.empty())
        {
            foundNonEmptyMesh = true;
            EXPECT_EQ(mesh->indices.size() % 3, 0u);
            EXPECT_EQ(mesh->positions.size(), mesh->normals.size());
        }
    }
    EXPECT_TRUE(foundNonEmptyMesh);
}

TEST(UfbxWrapperTest, LoadThrowsOnMissingFile)
{
    const UfbxWrapper wrapper;
    EXPECT_THROW(wrapper.Load(FixturePath("does_not_exist.fbx")), std::runtime_error);
}
