#pragma once

#include <string>

// Author: Claude
// Description: model_import/ 테스트가 공유하는 픽스처 파일 경로 조합 헬퍼. MODEL_IMPORT_FIXTURES_DIR(빌드
//              시스템이 정의하는 컴파일 정의)와 파일명을 이어 붙인다.
// Input: fileName - tests/fixtures/model_import/ 아래의 파일명 (예: "cube.obj")
// Output: 픽스처 파일의 절대 경로
// Notes: ModelLoaderTest/TinyObjWrapperTest/UfbxWrapperTest/CgltfWrapperTest가 각자 동일한 함수를
//        중복 정의하던 것을 이 헤더 하나로 통합했다(code_review Stage 1 finding).
// Date: 2026-08-11
inline std::string FixturePath(const std::string& fileName)
{
    return std::string(MODEL_IMPORT_FIXTURES_DIR) + fileName;
}
