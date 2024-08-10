# 이미지 처리 프로젝트

&nbsp;

&nbsp;
## 💪 프로젝트 소개
이 프로젝트는 외부라이브러리에 의존하지 않고 다양한 영상변환 기능을 직접 구현하였습니다.

## 😄 주요 기능
* Digital Image Processing (3rd)의 주요기능들을 직접 구현하였습니다
* BMP파일은 편의를 위해 8bpp로 고정시켰습니다. (pallette 정보는 흑백을 가정하고 제작하였습니다)
* 필터를 사용하는 부분은 직접 사용자가 필터를 설계할 수 있게 하였습니다
* 그외의 사용자가 건드릴 수 있는 부분은 최대한 커스터마이징 할 수 있게 하였습니다
* 나머지 부분은 수학 알고리즘에 의해 최적의 값을 찾습니다

## ℹ️ 개발 정보
* 개발기간 : 23.03.22 ~ 05.23
* 개발인원 : 류성수 (dbtjdtn325@gmail.com)
* 개발환경 : Window 11 (23H2), VisualStudio 2022 (17.9.6 v), C17

## 🙏 설치 및 실행방법

### 1. 먼저 레포지토리를 clone 합니다 (혹은 zip 파일로 다운 가능)
   ```sh
   git clone https://github.com/supergravityy/Temporary_Image_Processing.git
   ```
### 2. 폴더안의 exe 파일을 실행합니다

### 3. 목록에 있는 샘플 사진파일중에 하나를 선택하여, 파일명을 입력합니다

![스크린샷 2024-08-10 174218](https://github.com/user-attachments/assets/88bd027f-5c5b-4f1c-813e-31e1641247e7)

### 4. 원하는 이미지처리 기능을 선택합니다 (1~14번)

그외의 번호는 디버깅과 관련있습니다

![스크린샷 2024-08-10 174333](https://github.com/user-attachments/assets/88bc9df2-cd44-4619-9959-658f3abc53a5)

### 5. 커스터마이징 할 수 있는 부분을 바꿉니다

  ```sh
  블러링 처리에서 가우시안 블러링 처리를 선택
  ```
![스크린샷 2024-08-10 174514](https://github.com/user-attachments/assets/92b7d83c-4fe6-4f28-a115-d57b33a2f3a4)

  ```sh
  필터의 크기를 선택
  ```

![스크린샷 2024-08-10 174525](https://github.com/user-attachments/assets/45f4a591-40bb-44c9-9d31-3761f2cfa639)

  ```sh
  표준편차를 선택 (표준편차는 블러링의 강도와 관련되어 있습니다)
  ```

![스크린샷 2024-08-10 174619](https://github.com/user-attachments/assets/c2c068df-683d-4c36-85da-353354f18da7)

## 👍 결과 예시

오리지널 baboon.bmp

![스크린샷 2024-08-10 175056](https://github.com/user-attachments/assets/8a0ccc30-44e9-4782-9b46-d08a93d89c85)

해당 이미지 처리로 완성된 baboon_blurred.bmp

![스크린샷 2024-08-10 175103](https://github.com/user-attachments/assets/0c57c1f1-f8de-4285-a5b4-3f1bb7515945)


## 🤔 자세한 정보

코드의 주석만으로는 해당 파일이 왜 이렇게 작성되었는지를 알고싶다면???
자세한 정보는 디렉터리 안의 'Project_explaination.pdf' 파일을 참고해주세요~~
