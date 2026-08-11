#include <gtest/gtest.h>

#include <stdexcept>

#include "FixturePathHelper.h"
#include "platform/model_import/CgltfWrapper.h"

TEST(CgltfWrapperTest, LoadProducesSingleTriangleMesh)
{
    const CgltfWrapper wrapper;
    const Model model = wrapper.Load(FixturePath("triangle.gltf"));

    ASSERT_EQ(model.meshes.size(), 1u);
    const ModelMesh& mesh = *model.meshes[0];
    EXPECT_EQ(mesh.positions.size(), 3u);
    EXPECT_EQ(mesh.indices.size(), 3u);
}

TEST(CgltfWrapperTest, LoadPreservesSceneGraphInstancingWithoutFlattening)
{
    const CgltfWrapper wrapper;
    const Model model = wrapper.Load(FixturePath("instancing.gltf"));

    // 같은 메쉬를 두 노드가 참조하므로 고유 지오메트리는 하나만 저장되어야 한다.
    ASSERT_EQ(model.meshes.size(), 1u);

    ASSERT_EQ(model.rootNode.children.size(), 2u);
    const Node& instanceA = model.rootNode.children[0];
    const Node& instanceB = model.rootNode.children[1];

    ASSERT_TRUE(instanceA.meshIndex.has_value());
    ASSERT_TRUE(instanceB.meshIndex.has_value());
    EXPECT_EQ(*instanceA.meshIndex, *instanceB.meshIndex);

    EXPECT_NE(instanceA.localTransform.position.x, instanceB.localTransform.position.x);
}

TEST(CgltfWrapperTest, LoadThrowsOnMissingFile)
{
    const CgltfWrapper wrapper;
    EXPECT_THROW(wrapper.Load(FixturePath("does_not_exist.gltf")), std::runtime_error);
}
