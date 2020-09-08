# Rex AI Kit

## 요구사항 정의

### 환경 및 개발 목표

#### 대상 사용자

* SW 계열 특성화고/특목고 1학년 학생
* C 언어 문법을 이해할 수 있으며, C 언어 사용에 대해 기초적 지식이 있다고 가정

#### HW

* Rex Basic/Pro 보드에 I2C 센서보드가 연결된 상태
* I2C 센서보드
  * APDS-9960: 6 방향 제스처 감지 센서
  * MAX30101: 심박수 및 산소포화도 측정 센서
  * MAX30105: 공기 중 입자 검출 센서
  * RA12P: 압력 센서(Analogue)
    * 센서 출력값은 ADC인 MCP3021을 거쳐 10bit 정수값으로 변환됨
    * MCP3021: I2C ADC
  * SHT35-DIS: 온습도 측정 센서
  * Si1145: 가시광선, 자외선 측정 센서

### 개발 요구사항

#### 코드 정리

* 중복 및 미사용 코드 제거

* 소스코드 주석 추가

#### 코드 통합

* 각 센서 코드를 재사용 가능하도록 라이브러리화

* 센서 6종 데이터를 취합하는 Main Application 개발