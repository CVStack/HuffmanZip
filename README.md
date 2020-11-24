# HuffmanZip

<h1>System Programming Final Project</h1>

프로젝트 목적 : testfiles 를 압축하고, 압축해제하는 2 개의 프로그램 제작한다.

사용 언어 : C

실행 소스코드: decompress.c, compress.c

사용 알고리즘 : 허프만 코딩
![image](https://user-images.githubusercontent.com/38209962/100064516-f7e58e80-2e75-11eb-94c9-dd985965434b.png)

<h2>실행 방법</h2> 

c파일을 컴파일 후 사용한다.
![image](https://user-images.githubusercontent.com/38209962/100065004-97a31c80-2e76-11eb-9843-2b25aa05b50f.png)

compress를 실행하면 확장자 .cpd인 파일이 생성된다.

decompress를 실행하면 확장자 .dcpd인 파일이 생성된다.

![image](https://user-images.githubusercontent.com/38209962/100065159-cde09c00-2e76-11eb-8a8b-cd8cf84aeeb4.png)

<h2>테스트 방법</h2>

dcpd파일과 원본 파일을 같은 위치에 두고 diff을 실행시킨다.

diff.exe --> dcpd 파일과 원본 파일을 비교한다

![image](https://user-images.githubusercontent.com/38209962/100065516-3760aa80-2e77-11eb-88b5-e0581233934b.png)

아무 것도 뜨지 않으면 정상적으로 압축 및 압축 해제가 된 것이다.

<h2>압축 결과</h2>

![image](https://user-images.githubusercontent.com/38209962/100065613-565f3c80-2e77-11eb-90a9-446d82c4f7c2.png)

![image](https://user-images.githubusercontent.com/38209962/100065639-60813b00-2e77-11eb-8f2c-fb248c5d1dfe.png)










