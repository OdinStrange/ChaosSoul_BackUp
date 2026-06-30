# S ChaosSoul 진도 현황 (2026-06-30 기준)

## 주차별 계획 vs 실제 구현 비교

|주차|계획 목표|실제 구현 상태|완성도|
|-|-|-|-|
|**1주차**|C++ 기초 (클래스/함수/포인터/상속) + GDD + Git LFS|클래스 정의, 멤버 변수/함수, 생성자, 상속(`ACharacter`), `UPROPERTY`/`UFUNCTION` 매크로 실제 코드에 적용됨|✅ 완료|
|**2주차**|WASD 이동/점프 + 3인칭 카메라 + 첫 패키징|Enhanced Input으로 이동/시점 구현, SpringArm+Camera 세팅, IMC\_Default + IA\_Move/IA\_Look 에셋 존재. `FObjectFinder<UInputAction>` 템플릿 공백 버그 수정 완료|✅ 완료|
|**3주차**|idle/walk/run 애니메이션 + 공격 몽타주|AnimInstance(플레이어/적 모두), 몽타주 에셋 다수 존재, ABP\_PlayerCharacterBase 블루프린트, AttackAnimNotify 셸|✅ 완료|
|**4주차**|콜리전 + 트레이스 + 데미지 시스템|구형 SweepTrace 구현, 파티클 이펙트 재생까지 동작. `CurrentHP`/`MaxHP` 변수 추가됨, `TakeDamage` 활성화 → 실제 HP 감소 및 사망(Destroy) 동작|✅ 완료|
|**5주차**|몬스터 AI (탐지/추격/공격)|BehaviorTree + Blackboard 에셋, AIController 연결(`AIControllerClass`, `AutoPossessAI`) 활성화됨. BTTask에서 `UGameplayStatics::ApplyDamage` 실제 호출 + 쿨타임 관리 구현. 공격 시 `AttackMontage` 재생 연결 완료|✅ 완료|
|**6주차**|체력 UI + 사망/리스폰|`CurrentHP`/`MaxHP` + `TakeDamage` 구현. `HPBar`(`UProgressBar`) BeginPlay 바인딩 및 피격 시 실시간 갱신. 사망 시 `Destroy()` → PC 타이머 3초 → `RestartPlayer(PC)`로 리스폰|✅ 완료|
|**7주차**|회복 아이템 + 던전 맵|**회복 아이템** — 포션 개수(`CurrentPotion`/`MaxPotion=3`) 관리, `UsePotion()`으로 체력(`HealAmount=50`) 회복, HUD `PotionQuantity` 텍스트 연동, 사망 시 초기화, 리스폰 시 HUD 중복 제거 구현. 3D 포션 메시 에셋 존재. **미구현**: 월드 픽업 액터(플레이어가 걸어가면 줍는 포션) 없음. **던전 맵** — `Stage\_Cemetary.umap` 수작업 제작, `CombatEnemySpawner`/`CombatCheckpointVolume` 구역 활성화 체인 존재. **미구현**: 절차적 생성·방 구조·미니맵 없음|⚠️ 부분 구현|
|**8주차**|메인메뉴/클리어/게임오버 + 전체 통합|없음|❌ 미구현|
|**9주차**|버그픽스 + SFX/VFX + 패키징|사운드 에셋(ActionMusicVol1, FantasyPackFREE) 임포트됨. `AttackAnimNotify`에서 무기 4종 공격 사운드 재생 연결됨. 피해/배경음/환경음은 미연결|⚠️ 부분 구현|
|**10주차**|발표 자료 + 시연 영상|없음|❌ 미구현|

\---

## 계획에 없었는데 추가 구현된 것

|항목|내용|
|-|-|
|무기 4종 전환 시스템|숫자키로 소검/대검/둔기/카타나 전환, UI 텍스트로 데미지 수치 표시|
|카메라 줌 연동|앞으로 달릴 때 카메라가 자동으로 멀어지는 동적 팔 길이 조절|
|카메라 셰이크|공격 시 `BP\\\_WeaponLegacyCameraShake` 재생|
|몬스터 2종 구조|계획은 1종이었으나 Zombie + Juggernaut 두 타입 베이스 구성|
|포션 UI 시스템|`PotionQuantity` TextBlock에 현재 포션 개수 실시간 반영, 게임 시작 시 초기값(3) 즉시 표시|
|포션 입력 바인딩|`IA\_DrinkPotion` + `PotionAction`으로 키 입력 시 `UsePotion()` 호출|
|사망·리스폰 HUD 정리|사망 시 기존 HUD 제거 → 리스폰 후 새 HUD 중복 없이 생성|
|무기별 개별 오프셋|`ChangeWeaponTo()` switch에서 무기마다 `SetRelativeLocation`/`SetRelativeRotation` 독립 적용|
|공격 범위 시각화 개선|`AttackTrace()` 에서 항상 구체 표시 (명중 빨강·빗나감 초록)|

\---

## 지금 당장 우선순위로 수정해야 할 것

1. **게임오버 / 메인메뉴 UI 구현** — 플레이어 사망 후 게임오버 화면, 메인메뉴 레벨 전환 구현 (8주차 목표)
2. **월드 포션 픽업 액터** — `ARecoveryItem` 클래스 제작, 맵에 배치해 플레이어가 걸어가면 포션 획득
3. **던전 맵 구성** — `Stage\_Cemetary` 방 구조 정리, 스포너로 구역별 적 배치

<b>~~완료된 항목 (이전 우선순위)~~</b>

* ~~플레이어 HP 변수 추가 + TakeDamage 연결~~ → ✅ `CurrentHP`/`MaxHP` 추가, `TakeDamage` 활성화 완료
* ~~AIController 연결 주석 해제~~ → ✅ `AIControllerClass` + `AutoPossessAI` 활성화 완료
* ~~BTTask 공격 로직 구현~~ → ✅ `ApplyDamage` 실제 호출 + 쿨타임 관리 완료
* ~~몬스터 공격 애니메이션 연결~~ → ✅ `BTTask\\\_EnemyAttack`에서 `Attack()` 호출로 몽타주 재생 완료
* ~~기본 HUD에 체력바 연결~~ — `BP\\\_HUDUserWidget` 블루프린트에 ProgressBar 추가 후 `CurrentHP / MaxHP`로 바인딩 완료



## 추후 고쳐야 할 것들

1\. 플레이어 캐릭터의 애니메이션이 정상적으로 리타게팅 되지 않아서, 애니메이션이 정상적으로 플레이되고 있지 않아요. 전반적인 캐릭터 애니메이션이 어색합니다.

2\. ~~플레어의 마우스 왼쪽 클릭을 통한 공격시 다시 처음부터 몽타주가 재생이 되고 있습니다. 몽타주 재생시에는 공격 이벤트를 return 시키거나 쿨타임을 적용 시키는 것을 고려해 보세요.~~ → ✅ `Montage\\\_IsPlaying` 체크로 재생 중 연타 차단 완료

3\. 플레이어의 무기들의 소켓 위치가 손에 정확히 위치 하지 않습니다. 무기가 떠다니는 컨셉이라면, 컨셉에 맞춘 추가 기능을 정의하고, 자연스러울 수 있도록 구현 해야 합니다.

4\. 플레이어 무기 히트 사운드가, 애니메이션에 설정되어 있어 히트가 되지 않음에도 재생 되고 있습니다. 실제로 타격이 들어간 시점에 사운드가 플레이 되도록 수정해 보세요.

5\. 플레이어와 카메라 사이에 몬스터가 지나갈때 CameraRag이 반응 하고 있어서 카메라의 위치가 갑자기 변경이 되고 있습니다. 플레이어와 카메라의 사이에 몬스터가 걸리더라도 반응 하지 않도록 Collision 설정을 Ignore 하면 더 자연스럽게 게임플레이가 가능합니다.

6\. 코드는 전반적으로 깔끔하게 잘 작성되어 있습니다. 아래의 코드처럼 하드코딩 되어 있는 값들을 데이터 테이블로 관리 하게 되면 수정이 더 용이하니 고려해 보세요.

7\.이 외에도, 완성도가 떨어져 보일만한 요소들 중에 가장 먼저 수정 해야 하는 부분으로 캐릭터의 texture 가 미 적용 상태인 부분도 함께 살펴 보세요!

