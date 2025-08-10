int led1 = 5;
int led2 = 11;

void setup() {
  // Q. 이 장에서 시리얼 통신을 사용하지 않는 이유는 출력을 led 로 확인가능하기 때문인가
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

}

void loop() {
  analogWrite(led1, 1);
  analogWrite(led2, 1);
}
