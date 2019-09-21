# OpenViBE2Unity

<img width="1035" alt="Screen Shot 2019-08-11 at 8 39 59 PM" src="https://user-images.githubusercontent.com/30457077/62833356-63531f00-bc78-11e9-8cff-bd336aadf098.png">


### Introduction
뇌-컴퓨터 인터페이스는 많은 관심을 받으며 사람의 의도, 인지, 느낌 등을 정량화하여 활용하고자 하는 연구 및 개발에 사용되고 있다. 하지만 뇌파를 측정하고 분석하여 결과를 활용하는 애플리케이션을 제작하는 것은 상당한 노력이 요구되며, 실제로 연구 및 개발보다는 상대적으로 플랫폼간의 연동에 적지 않은 시간을 투자하게 된다. 이에 따라, 뇌-컴퓨터 인터페이스 어플리케이션 개발을 용이하게 하고자 대표적인 게임 개발 플랫폼인 Unity3D와 뇌 신호의 실시간 수집, 처리, 시각화 기능을 제공하는 OpenViBE를 연결하는 API, OpenViBE2Unity를 개발하였다. 개발된 API는 두 오픈소스 플랫폼간의 연동을 쉽게 할 수 있도록 몇 가지 함수들을 제공하며 주요 코드 및 사용설명서는 Github를 통해 공개되어 있다. 본 논문에서는 개발한 OpenViBE2Unity의 구조 및 사용법 그리고 OpenViBE2Unity를 활용하여 개발 된 애플리케이션을 소개한다.


### Documentation 
https://github.com/Mainea/Petween/files/3489903/O2U.development.guide.pdf


### Members
- 정성준 (Developer)
- 김수용 (Developer)
- 안민규 (Professor)


### 1. 오픈소스 소개 및 활용
OpenViBE, UIVA(O2U), Unity 총 3개의 오픈소스를 사용하는 시스템이다. UIVA는 VRPN(가상현실 주변 장치 네트워크과 Unity3D를 소켓을 통해 통신이 가능하도록 한다. 본 시스템 내에서는 OpenViBE와 Unity3D 간 통신에 사용되고 있는 VRPN은 응용 프로그램의 가상현실 주변 장치에 접근하기 위한 독립적인 네트워크 기반 인터페이스로 Kinect, Wiimote, WiiFit, BPack 과 같은 가상현실 입력 장치들을 가상현실 애플리케이션에 적용하는 데에 목적을 둔다. OpenViBE에서 VRPN을 지원하며, UIVA가 VRPN client와 Unity와의 통신을 지원하기에 VRPN을 뇌파 신호 전달을 포함한 OpenViBE와 Unity3D간의 통신에 이용할 수 있다.





### 2. 뇌파 데이터의 통신 과정 및 양방향 통신
![image](https://user-images.githubusercontent.com/30457077/62833264-c04dd580-bc76-11e9-9384-9f815c5d81de.png)

  OpenViBE-UIVA-Unity3D 시스템으로 뇌파 측정을 위한 뇌전도 장비로부터 뇌파 게임에 이르기까지 뇌파 데이터 통신을 하는 과정을 나타낸다. 뇌전도 장비를 통해 측정된 뇌파 신호는 Aquisition Server, Client 함수 박스를 시작으로 OpenViBE로 들어오게 되며  전 처리 단계로써 OpenViBE 내의 함수 박스를 이용하여 채널 및 주파수 대역 선택을 통한 시공간적 정보로 필터링을 수행 할 수 있다. 이후 연구 방향에 맞게 뇌파 원 신호를 사용자의 의도에 맞게 파이썬 스크립트를 통해 추출하는 것, Unity3D로부터 입력 받은 Stimulation 신호를 통해 파이썬 내 조건문으로 상황에 따른 뇌파 처리 과정 구분 또한 가능하다. 이후 VRPN을 통해  UIVA_Server와 통신하게 된다. 더 나아가 VRPN, UIVA, 그리고 애플리케이션인 Unity3D와의 통신은 정방향, 역방향 및 양방향으로 통신이 가능하다. VRPN에서 입력 장치의 활용을 위해 출력 신호로 제공하는 Analog, Button신호를 활용하여 뇌파 게임에 적용이 가능한 양방향 통신을 지원한다. Analog와 Button 신호는 조이스틱의 레버의 데이터를 Analog신호로 전달 받고, 버튼의 데이터를 Button신호로 전달하는 것을 지원하기 위해 만들어진 신호이다. Analog형 신호는 실시간으로 정수형 데이터를 보낼 수 있으며, Button형 신호는 일시적으로  불린 자료형 데이터를 보낼 수 있는 특징을 가진다. 이 특징을 활용하여 OpenViBE에서 Analog 신호로 실시간으로 뇌파 신호를 OpenViBE에서 Unity3D로 정방향 통신으로 받아오고, Button형 신호를 Unity3D에서 OpenViBE로 역방향으로 보내어 이벤트를 찍어주는 형식으로 쓰이거나, 또는 이 신호들이 파이썬 스크립트 내에서 신호처리 조건 인자로도 쓸 수 있도록 OpenViBE VRPN Server가 지원한다.
