# platform

검증된, 완전히 추상화된 최소 단위 기능/데이터 라이브러리입니다.
주요 키워드: 최소단위 기능 / 추상화 / 검증.
이 저장소를 소비하는 상위 프로젝트에는 git submodule 형태로 연결하는 것을 전제로 합니다.

## 역할

- `graphics`, 그리고 이 저장소를 사용하는 상위 프로젝트 어느 쪽도 몰라야 합니다(어떤 상위 계층의 헤더도 include하지 않습니다).
- 타입이 primitive해 보인다는 것만으로는 platform 소속 근거가 되지 않습니다 — 그 타입이 특정 소비자(예: `IRenderer`)의 계약을 만족시키기 위해 존재한다면 그 소비자 쪽 소속입니다.
- 대상(`T`)에 대해 완전히 무지한 제네릭 메커니즘은 `T`가 나중에 무엇으로 쓰이든 항상 platform입니다.
- 같은 폴더에 "범용 메커니즘"과 "그 메커니즘이 다루는 구체적 데이터"가 섞여 있으면, 메커니즘만 platform으로 가고 데이터는 상위 계층에 남습니다.

## 구성

- `collision/` — 충돌 감지 알고리즘(GJK/EPA, BVH, Octree, R-Tree, Sweep and Prune 등).
- `engine/` — 스레드 간 프레임 데이터 발행 메커니즘(`IFrameDataPublisher`, `DoubleBufferPublisher`).
- `entity_component/` — Entity/Component 패턴 기본 구조.
- `geometry/` — 기하 도형(AABB, OBB, Sphere, Capsule 등)과 교차 판정.
- `logging/` — 로그 포맷/싱크/로거.
- `math/` — 벡터, 행렬, 쿼터니언 등 수학 라이브러리.
- `model_import/` — OBJ/FBX/GLTF/GLB 모델 임포터, 메시 캐시 정책, 파일 변경 감지.
- `persistence/` — 저장/복원 메커니즘(`ISaveable`, `SaveLoadManager`).
- `serialization/` — 데이터 스토어 추상화(JSON/SQLite) 및 레코드 접근.
- `Win32Window` — Win32 창 생성 래퍼.

## 개발 환경

- 언어: C++
- 빌드 시스템: 상위 프로젝트의 CMake 빌드에 편입되어 빌드됩니다. 이와 별개로 `platform/CMakeLists.txt`가 `platform_lib`를 자체적으로 정의해, platform 레포만 단독으로도 빌드·검증할 수 있습니다.

## 테스트

- `platform`은 Google Test 기반의 자체 검증용 단위 테스트를 갖고 있습니다(10개 영역 전체, 247개 케이스).
- `PLATFORM_BUILD_TESTS` 옵션(기본 `OFF`)으로 켜면 `platform_tests` 타겟이 빌드됩니다 — platform 레포 자체 검증용이며, 상위(소비) 프로젝트가 이 빌드를 켤 의무는 없습니다.
- 빌드/실행 방법과 신규 기능 추가 시 테스트 작성 규칙은 [`docs/testing_방법_단위테스트_도입_20260811.md`](./docs/testing_방법_단위테스트_도입_20260811.md)를 참고하세요.

협업 방식과 워크플로우는 [`agent_harness/CLAUDE.md`](./agent_harness/CLAUDE.md)를 따릅니다.
