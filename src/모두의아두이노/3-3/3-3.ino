// 봄나들이 연주하기
int myNode[] = {784,659,784,659,784,880,784}; // 음 높이
int myDu[] = {250,250,250,250,250,250,500}; // 음 길이
void setup() {
  for(int i = 0; i < 7; i++) {
    tone(3, myNode[i], myDu[i]);
    delay(myDu[i]*1.3);
    noTone(3);
  }
}

void loop() {

}
