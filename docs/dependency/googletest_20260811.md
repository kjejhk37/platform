Brainstorming: [`../brainstorming/단위테스트_도입_20260811_2033.md`](../brainstorming/단위테스트_도입_20260811_2033.md)

이 문서는 `World-of-Tank-imitation-Refactoring/docs/dependency/googletest_20260715.md`에서 이미 수행한 조사를 인용한다.
관리 체계는 platform → WOT 방향이 원칙이므로(브레인스토밍 Q&A Log 참고), platform이 의존하는 GoogleTest도 platform 레포 자체 문서로 남긴다.
Step 1의 조사 내용(Alternatives/Maintenance/License)은 WOT에서 이미 검증된 동일 라이브러리·동일 버전 결정을 재사용하므로 재조사하지 않는다.

---

# Step 1 — Evaluation

## Alternatives

| | GoogleTest | Catch2 (v3) | doctest |
|---|---|---|---|
| 기능 범위 | xUnit 스타일 + `gmock` 내장 | BDD 스타일(`SECTION`), mock 없음 | 최소 기능, mock 없음 |
| 커뮤니티/인지도 | 업계 사실상 표준 | 게임/오픈소스권 인기 | 상대적으로 마이너 |
| 유지보수 | 활발 (Google 공식 관리) | 활발 | 활발하지만 상대적으로 소규모 |

## Maintenance Status

- 최신 릴리스: 1.17.0. 1.17.x부터 C++17 이상 요구 — platform도 C++ 기반이며 WOT(`CMAKE_CXX_STANDARD 17`)와 동일 기준을 따른다.
- 이슈 트래커에 2026-01까지도 신규 이슈가 등록되고 있어 활발히 유지보수 중임을 확인(WOT 조사 시점 기준, 재확인 불필요).
- Google이 공식으로 관리하는 저장소로, 저장소 자체가 아카이브되거나 방치된 상태 아님.
- WOT 프로젝트의 `build/_deps/googletest-src`에 1.17.0 소스가 이미 `FetchContent`로 정상 수신·빌드된 이력이 있어, 동일 버전 고정 시 재현성이 검증되어 있다.

## License

- BSD-3-Clause. Permissive, 카피레프트 없음.
- platform은 개인 학습/포트폴리오 목적의 서브모듈 라이브러리로, 상업적 재배포가 없어 라이선스 충돌 없음(WOT과 동일 판단).

## Risks

- 컴파일 시간이 Catch2/doctest 대비 느리다 — platform은 10개 영역·다수 클래스를 대상으로 하므로 WOT(단일 함수 테스트)보다 테스트 코드 양이 많아 체감 영향이 더 클 수 있다. 다만 `PLATFORM_BUILD_TESTS` 옵션이 기본 OFF이므로 일반 빌드에는 영향 없다.
- CMake `FetchContent`로 소스를 받으므로 최초 빌드 시 네트워크가 필요하다 — 오프라인 빌드가 필요해지면 별도 vendoring 검토 필요(현재 범위 아님).
- 알려진 보안 취약점 없음. platform_lib(프로덕션 라이브러리 타겟)에는 링크되지 않으므로, platform을 소비하는 상위 프로젝트의 배포 바이너리에도 포함되지 않는다.

---

# Step 2 — Wrapper Design Draft

**일반 원칙과의 차이 — WOT 선례를 준용하며, platform 관점으로 다시 정리함.**

`agent_harness/CLAUDE.md`의 핵심 원칙은 "외부 라이브러리는 반드시 내부 wrapper 클래스로 감싸고, 다른 모듈이 외부 라이브러리를 직접 참조하지 않게 한다"이다.
이 원칙은 platform이 이미 실제로 적용 중인 것 — `model_import/`의 `TinyObjWrapper`/`UfbxWrapper`/`CgltfWrapper`처럼, **프로덕션 코드가 의존하는** 외부 라이브러리에는 그대로 적용된다.

GoogleTest는 성격이 다르다.

- 테스트 코드의 본질적 목적 자체가 `TEST(...)`, `EXPECT_EQ(...)` 같은 프레임워크 매크로를 **직접** 표현하는 것 — 클래스 래퍼로 감싸면 표준 관용구를 잃고 가독성만 떨어진다.
- 실제로 격리해야 할 지점은 "platform_lib(프로덕션 라이브러리 타겟)가 GoogleTest를 참조하지 않는 것"이며, 이는 클래스 래퍼가 아니라 **빌드 타겟 경계**로 달성 가능하다 — WOT이 이미 확정한 방식과 동일하다.

**제안하는 격리 방식 (platform 관점 — 라이브러리이지 실행 파일이 아님)**

- platform은 현재 자체 `CMakeLists.txt`가 없다(WOT의 최상위 `CMakeLists.txt`가 `platform_lib` 소스 목록을 직접 하드코딩해서 컴파일하는 구조). 이번 사이클에서 platform 최상위에 `CMakeLists.txt`를 신설해 `platform_lib` 정의를 platform 레포 안으로 옮긴다.
- 신규 CMake 실행 타겟 `platform_tests`를 만들어 `gtest`/`gtest_main`을 여기에만 링크한다.
- `platform_lib` 타겟은 GoogleTest에 대한 의존성이 전혀 없다 — `platform_tests`만 `platform_lib`과 `gtest_main`을 함께 링크해서 검증한다.
- `PLATFORM_BUILD_TESTS` 옵션 플래그(기본 OFF)로 감싸, platform을 서브모듈로 가져가는 상위 프로젝트의 빌드에는 영향이 없다. WOT은 현재 platform의 CMakeLists.txt를 전혀 참조하지 않는 구조(소스 하드코딩)이므로 이 신설 자체가 WOT 빌드에 미치는 영향은 없음을 확인했다.

이 방식으로 진행하는 것에 동의하는지 확인 필요 — 만약 클래스 래퍼 형태를 원한다면 별도로 논의.

---

# Step 3 — Verification Status

- `[x] Verified` — CMake `FetchContent`로 GoogleTest 1.17.0 소스를 받아 platform 신설 빌드에 통합 (`platform_tests` 타겟에만 링크, `platform_lib`은 무의존 확인 — `platform_lib.lib`는 gtest 없이도 독립적으로 빌드됨을 확인)
- `[x] Verified` — `enable_testing()` + `gtest_discover_tests`로 CTest 연동, `ctest -C Debug` 실행으로 247개 케이스 전부(100%) 통과 확인
- `[x] Verified` — 기본 assertion 매크로(`TEST`, `EXPECT_EQ`, `EXPECT_THROW` 등) 컴파일 및 통과 (247개 케이스 전체에서 사용)
- `[x] Verified` — platform 단독 빌드(상위 프로젝트 없이 `platform/`을 최상위로 구성, VS 2022/MSVC 19.37, x64)에서 `PLATFORM_BUILD_TESTS=ON`으로 정상 빌드 및 `platform_tests.exe` 생성 확인
- `[x] Verified` — `PLATFORM_BUILD_TESTS=OFF`(기본값)로 별도 빌드 디렉터리에서 재구성 시 `platform_lib`만 빌드되고 `_deps/`에 `googletest-*`가 전혀 받아지지 않음을 확인. platform은 WOT/graphics 등 상위 프로젝트를 모른다는 전제이므로 WOT 쪽 빌드 확인은 platform의 검증 범위에 포함하지 않는다.

검증은 `../strategy/단위테스트_도입_20260811_2210.md`의 Phase 0 구현 단계에서 실제 빌드로 수행했다(2026-08-11).
