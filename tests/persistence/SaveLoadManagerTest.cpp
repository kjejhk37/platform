#include <gtest/gtest.h>

#include "platform/persistence/ISaveable.h"
#include "platform/persistence/SaveLoadManager.h"
#include "platform/serialization/DataRecordAccess.h"
#include "platform/serialization/IDataStore.h"

namespace
{
// Author: Claude
// Description: SaveLoadManagerTest 전용 ISaveable 테스트 더블. platform은 WOT의
//              projects/persistence/PlayerProgress처럼 상위 프로젝트의 구체 ISaveable을 알 수 없으므로
//              (platform은 상위 프로젝트를 몰라야 한다) 순수 platform 범위의 최소 구현을 대신 쓴다.
// Input: (해당 없음 - 필드 score를 테스트 코드가 직접 설정)
// Output: ToRecord/FromRecord - DataRecord와 score 필드를 상호 변환
// Notes: score 필드 하나만 왕복 검증하면 SaveLoadManager의 saveVersion 부여 책임을 충분히 검증할 수 있다.
// Date: 2026-08-11
class FakeSaveable : public ISaveable
{
public:
    int score = 0;

    DataRecord ToRecord() const override
    {
        DataRecord record;
        record["score"] = std::to_string(score);
        return record;
    }

    void FromRecord(const DataRecord& record) override
    {
        score = DataRecordAccess::GetIntOr(record, "score", score);
    }
};

// Author: Claude
// Description: SaveLoadManagerTest 전용 IDataStore 테스트 더블. 실제 파일 시스템 없이 메모리 안에서
//              Save/Load 왕복과 실패 케이스(loadSucceeds=false)를 시뮬레이션한다.
// Input: (해당 없음 - loadSucceeds 필드를 테스트 코드가 직접 설정)
// Output: Save - 항상 true / Load - loadSucceeds가 false면 false, 아니면 마지막 Save 값을 반환
// Notes: key 인자는 사용하지 않는다 - 이 더블은 단일 레코드만 보관하는 최소 구현이다.
// Date: 2026-08-11
class FakeDataStore : public IDataStore
{
public:
    DataRecord saved;
    bool loadSucceeds = true;

    bool Save(const std::string&, const DataRecord& record) override
    {
        saved = record;
        return true;
    }

    bool Load(const std::string&, DataRecord& outRecord) override
    {
        if (!loadSucceeds)
        {
            return false;
        }
        outRecord = saved;
        return true;
    }
};
}  // namespace

TEST(SaveLoadManagerTest, SaveIncludesSaveVersionField)
{
    FakeDataStore store;
    const SaveLoadManager manager(store);

    FakeSaveable progress;
    progress.score = 100;

    ASSERT_TRUE(manager.Save("save.json", progress));
    EXPECT_EQ(store.saved.at("saveVersion"), "1");
    EXPECT_EQ(store.saved.at("score"), "100");
}

TEST(SaveLoadManagerTest, SaveThenLoadRoundTripsData)
{
    FakeDataStore store;
    const SaveLoadManager manager(store);

    FakeSaveable saved;
    saved.score = 250;
    ASSERT_TRUE(manager.Save("save.json", saved));

    FakeSaveable loaded;
    ASSERT_TRUE(manager.Load("save.json", loaded));
    EXPECT_EQ(loaded.score, 250);
}

TEST(SaveLoadManagerTest, LoadReturnsFalseWhenStoreLoadFails)
{
    FakeDataStore store;
    store.loadSucceeds = false;
    const SaveLoadManager manager(store);

    FakeSaveable progress;
    EXPECT_FALSE(manager.Load("missing.json", progress));
}

TEST(SaveLoadManagerTest, SaveAsyncBehavesIdenticallyToSave)
{
    // SaveAsync는 헤더 주석대로 이번 사이클에서는 시그니처만 제공하는 확장 지점이며 내부적으로
    // 동기 Save()를 그대로 호출한다 - 그 위임이 실제로 지켜지는지(saveVersion 포함, 데이터 보존)를
    // 검증한다. 실제 비동기 스레드 구현은 이 테스트의 범위가 아니다(refactor.md 커버리지 갭 해소).
    FakeDataStore store;
    const SaveLoadManager manager(store);

    FakeSaveable saved;
    saved.score = 42;

    ASSERT_TRUE(manager.SaveAsync("save.json", saved));
    EXPECT_EQ(store.saved.at("saveVersion"), "1");
    EXPECT_EQ(store.saved.at("score"), "42");

    FakeSaveable loaded;
    ASSERT_TRUE(manager.Load("save.json", loaded));
    EXPECT_EQ(loaded.score, 42);
}
