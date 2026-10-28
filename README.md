# PuzzlePlatforms

**움직이는 발판과 트리거를 활용하는 3D 멀티플레이 플랫폼 퍼즐 프로젝트**입니다.

Unreal Engine 5.7과 C++로 서버 권한 기반 플랫폼 이동, 세션 생성·검색·참가, 로비에서 게임으로 이어지는 흐름을 구현했습니다. UMG 메뉴와 C++ 인터페이스를 연결해 플레이어가 서버를 만들고 선택해 참가할 수 있도록 구성했습니다.

> 아래 기능 설명과 실행 방법은 최신 구현이 있는 [`dev` 브랜치](https://github.com/HRI-GD/PuzzlePlatforms/tree/dev)를 기준으로 합니다. `main`에는 이전 단계의 게임 코드가 있습니다.


## 프로젝트 개요

- **장르:** 3D 멀티플레이 플랫폼 퍼즐
- **엔진:** Unreal Engine 5.7
- **구현:** C++ / Blueprint
- **UI:** UMG와 C++ 위젯 클래스
- **네트워크:** 리슨 서버, 액터 이동 복제, Online Subsystem
- **온라인 구성:** Steam Online Subsystem / SteamSockets
- **세션 정원:** 3명
- **주요 맵:** `MainMenu`, `Lobby`, `Game`, `Transition`

## 플레이 흐름

1. 메인 메뉴에서 **Host**를 선택하고 서버 이름을 입력합니다.
2. 세션이 만들어지면 호스트가 로비 맵으로 이동합니다.
3. 다른 플레이어는 **Join** 메뉴에서 서버 목록을 확인하고 참가합니다.
4. 현재 로비 코드는 접속 인원이 **3명 이상**이 되면 **10초 타이머**를 등록합니다.
5. 타이머가 완료되면 세션을 시작하고 게임 맵으로 서버 트래블합니다.
6. 게임에서는 캐릭터를 이동·점프시키며 플랫폼과 트리거를 이용합니다.

기본 조작은 **WASD 이동, 마우스 시점 조작, Space Bar 점프**입니다.

## 주요 구현 요소

### 서버 권한 기반 이동 플랫폼

[`AMovingPlatform`](https://github.com/HRI-GD/PuzzlePlatforms/blob/dev/Source/PuzzlePlatforms/MovingPlatform.cpp)은 서버에서만 위치를 계산하고, 액터 이동 복제로 클라이언트에 전달합니다.

- `HasAuthority()`로 플랫폼 이동 계산 주체를 서버로 제한
- `SetReplicates(true)`와 `SetReplicateMovement(true)`로 이동 복제
- 로컬 목표 위치를 월드 좌표로 변환해 레벨에 배치된 플랫폼의 방향 반영
- `VInterpConstantTo`와 `DeltaTime`으로 일정 속도의 이동 처리
- 목표 도착 시 왕복 이동 또는 정지 선택
- 속도·목표 위치·왕복 여부를 에디터에서 설정

### 여러 트리거를 조합하는 플랫폼 작동 조건

[`APlatformTrigger`](https://github.com/HRI-GD/PuzzlePlatforms/blob/dev/Source/PuzzlePlatforms/PlatformTrigger.cpp)는 박스 영역의 진입·이탈 이벤트를 연결된 플랫폼에 전달합니다.

- 트리거 하나에 여러 플랫폼 연결 가능
- 플랫폼별 `RequiredActiveTriggers`로 필요한 활성 트리거 수 설정
- 조건을 만족하면 Tick을 활성화해 이동 시작
- 조건이 부족해지면 Tick을 비활성화해 이동 중단

플랫폼의 이동 로직과 작동 조건을 나누어 레벨에서 조합할 수 있도록 구성했습니다.

### 비동기 세션 생성·검색·참가

[`UPuzzlePlatformsGameInstance`](https://github.com/HRI-GD/PuzzlePlatforms/blob/dev/Source/PuzzlePlatforms/PuzzlePlatformsGameInstance.cpp)에서 맵 전환과 독립적으로 세션 상태를 관리합니다.

- 세션 생성·파괴·검색·참가 완료 델리게이트 등록
- 기존 세션이 있으면 파괴 완료 후 새 세션 생성
- 사용자 지정 서버 이름을 세션 설정에 저장하고 목록에 표시
- 서버 이름·호스트 이름·현재 인원·최대 인원을 UI 데이터로 변환
- 세션 참가 완료 후 접속 주소를 얻어 `ClientTravel` 실행
- 네트워크 실패 시 메인 메뉴로 복귀

Steam에서는 온라인 로비 검색을, NULL 서브시스템에서는 LAN 검색을 사용하도록 코드가 분기되어 있습니다. 기본 프로젝트 설정은 Steam과 SteamSockets를 사용합니다.

### 로비와 맵 전환

[`ALobbyGameMode`](https://github.com/HRI-GD/PuzzlePlatforms/blob/dev/Source/PuzzlePlatforms/LobbyGameMode.cpp)는 접속·퇴장 시 인원을 관리하고, 대기 시간이 끝나면 게임을 시작합니다.

- `PostLogin` / `Logout`에서 접속 인원 갱신
- `FTimerManager`로 게임 시작 지연 처리
- 세션 시작 후 `ServerTravel`로 게임 맵 이동
- `bUseSeamlessTravel`을 활성화해 서버의 맵 전환 흐름 구성

### 메뉴 UI와 게임 로직 분리

[`MenuSystem`](https://github.com/HRI-GD/PuzzlePlatforms/tree/dev/Source/PuzzlePlatforms/MenuSystem)의 C++ 클래스와 UMG Blueprint를 연결했습니다.

- `IMenuInterface`: 호스팅·참가·서버 목록 갱신·메인 메뉴 복귀 요청
- `UMenuWidget`: 메뉴 표시·해제, 입력 모드와 마우스 커서 전환
- `UMainMenu`: 메인·호스트·참가 화면 전환과 서버 선택
- `UServerRow`: 개별 서버 정보 표시와 선택 이벤트
- `UInGameMenu`: 게임 복귀와 메인 메뉴 이동

## 실행 방법

**Unreal Engine 5.7과 Unreal C++ 빌드 환경**이 필요합니다. Visual Studio 2022의 C++ 게임 개발 도구와 Windows SDK를 준비합니다.

1. 최신 구현이 있는 `dev` 브랜치를 내려받습니다.

   ```bash
   git clone --branch dev https://github.com/HRI-GD/PuzzlePlatforms.git
   ```

2. `PuzzlePlatforms.uproject`에서 프로젝트 파일을 생성하고 `PuzzlePlatformsEditor` 타깃을 빌드합니다.
3. 프로젝트를 Unreal Editor로 엽니다. `OnlineSubsystemSteam`과 `SteamSockets` 플러그인이 활성화되어 있어야 합니다.
4. 기본 맵 `Content/PuzzlePlatforms/Maps/MainMenu`를 실행합니다.
5. 온라인 세션 테스트는 Steam에 로그인한 별도 PC·계정에서 호스팅과 참가를 확인합니다.

**플랫폼 동작만 확인하려면** `Content/PuzzlePlatforms/Maps/Game`을 직접 열어 Play로 실행할 수 있습니다. 여러 플레이어의 이동 복제는 에디터의 멀티플레이 실행 설정에서 리슨 서버와 클라이언트로 확인할 수 있습니다.

## 테스트 환경 참고

- Steam 설정은 테스트용 App ID **480**을 사용합니다. 독립 Steam 출시용 App ID가 설정된 상태는 아닙니다.
- 서버 검색 결과와 접속 여부는 Steam 실행 상태, 계정, 네트워크 환경의 영향을 받습니다.
- 로비의 자동 게임 시작 조건은 현재 코드상 3명입니다. 혼자 게임 레벨을 확인할 때는 `Game` 맵을 직접 실행합니다.

## 프로젝트 구조

```text
PuzzlePlatforms/
├── PuzzlePlatforms.uproject
├── Source/PuzzlePlatforms/
│   ├── MovingPlatform.*                  이동·복제·작동 조건
│   ├── PlatformTrigger.*                 플랫폼 활성 트리거
│   ├── PuzzlePlatformsGameInstance.*      세션과 메뉴 흐름
│   ├── LobbyGameMode.*                    로비 인원·게임 시작
│   ├── PuzzlePlatformsGameMode.*          기본 캐릭터·컨트롤러 설정
│   └── MenuSystem/                       C++ 메뉴·서버 목록 UI
├── Content/
│   ├── PuzzlePlatforms/Maps/             메뉴·로비·게임·전환 맵
│   ├── MenuSystem/                       UMG 위젯과 UI 리소스
│   ├── ThirdPerson/                      캐릭터와 컨트롤러 Blueprint
│   └── Input/                            Enhanced Input 액션·매핑
├── Config/                               엔진·온라인 설정
└── docs/images/                          README 스크린샷
```

## 구현을 통해 다룬 내용

- 서버 권한과 이동 복제를 구분한 멀티플레이 액터 설계
- 비동기 완료 델리게이트를 이용한 세션 생명주기 관리
- `GameInstance`와 `GameMode`의 역할 분리
- `ServerTravel`과 `ClientTravel`을 이용한 연결·맵 전환
- UMG와 C++ 인터페이스를 통한 UI·네트워크 로직 연결
- 에디터 노출 속성과 겹침 이벤트를 활용한 퍼즐 요소 구성
