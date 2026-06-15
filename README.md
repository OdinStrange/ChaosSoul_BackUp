# &#x20;ChaosSoul 진도 현황 (2026-05-15 기준)

## 주차별 계획 vs 실제 구현 비교

|주차|계획 목표|실제 구현 상태|완성도|
|-|-|-|-|
|**1주차**|C++ 기초 (클래스/함수/포인터/상속) + GDD + Git LFS|클래스 정의, 멤버 변수/함수, 생성자, 상속(`ACharacter`), `UPROPERTY`/`UFUNCTION` 매크로 실제 코드에 적용됨|✅ 완료|
|**2주차**|WASD 이동/점프 + 3인칭 카메라 + 첫 패키징|Enhanced Input으로 이동/시점 구현, SpringArm+Camera 세팅, IMC\_Default + IA\_Move/IA\_Look 에셋 존재. 단, **IA\_Move/Look/Attack 경로 문자열에 공백 버그** 있음|⚠️ 완료|
|**3주차**|idle/walk/run 애니메이션 + 공격 몽타주|AnimInstance(플레이어/적 모두), 몽타주 에셋 다수 존재, ABP\_PlayerCharacterBase 블루프린트, AttackAnimNotify 셸|✅ 완료|
|**4주차**|콜리전 + 트레이스 + 데미지 시스템|구형 SweepTrace 구현, 파티클 이펙트 재생까지 동작. 단, **`TakeDamage` 주석처리** → 실제 HP 감소 없음, 플레이어 HP 변수 자체 없음|⚠️ 완료|
|**5주차**|몬스터 AI (탐지/추격/공격)|BehaviorTree + Blackboard 에셋, AIController에서 플레이어 거리 감지 후 `bIsAttack` 설정까지 구현. 단, **AIControllerClass 연결 주석처리**, **BTTask 공격 로직 미구현** (로그만 출력)|⚠️ 거의 완료(몬스터 공격 모션 없음)|
|**6주차**|체력 UI + 사망/리스폰|BP\_HUDUserWidget에 무기 아이콘/텍스트만 존재. **체력바 없음**, **플레이어 HP 변수 없음**, 사망/리스폰 로직 없음|❌ 미구현|
|**7주차**|회복 아이템 + 던전 맵|테스트 맵(L\_Test.umap) 1개만 존재. 회복 아이템 클래스 없음, 던전 맵 없음|❌ 미구현|
|**8주차**|메인메뉴/클리어/게임오버 + 전체 통합|없음|❌ 미구현|
|**9주차**|버그픽스 + SFX/VFX + 패키징|사운드 에셋(ActionMusicVol1, FantasyPackFREE)은 임포트됨. 코드 연결은 없음|❌ 미구현|
|**10주차**|발표 자료 + 시연 영상|없음|❌ 미구현|

\---

## 계획에 없었는데 추가 구현된 것

|항목|내용|
|-|-|
|무기 4종 전환 시스템|숫자키로 소검/대검/둔기/카타나 전환, UI 텍스트로 데미지 수치 표시|
|카메라 줌 연동|앞으로 달릴 때 카메라가 자동으로 멀어지는 동적 팔 길이 조절|
|카메라 셰이크|공격 시 `BP\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\_WeaponLegacyCameraShake` 재생|
|몬스터 2종 구조|계획은 1종이었으나 Zombie + Juggernaut 두 타입 베이스 구성|

\---

## 지금 당장 우선순위로 수정해야 할 것

2. **플레이어 HP 변수 추가 + TakeDamage 연결** (`PlayerCharacterBase.h/.cpp`)

   * `float PlayerHp` 변수 추가
   * `AttackTrace()`의 주석처리된 `TakeDamage` 활성화
3. **AIController 연결 주석 해제** (`EnemyBase.cpp` InitializeAIController)

   * `AIControllerClass = AEnemyAIController::StaticClass();` 주석 해제
   * `AutoPossessAI` 설정 주석 해제
4. **BTTask 공격 로직 구현** (`BTTask\\\_EnemyAttack.cpp`)

   * `ExecuteTask`에서 실제 데미지 적용 코드 작성

