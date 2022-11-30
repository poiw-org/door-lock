class Door {

private:
  String id;

public:
  // Constructor
  Door(String id) { this->id = id; }
  String getId() { return this->id; }

  void open() {
    digitalWrite(BUSY_LED, LOW);
    Serial.println("Door Opened");
    digitalWrite(SUCCESS_LED, HIGH);
    digitalWrite(D0, HIGH);
    digitalWrite(SPEAKER, HIGH);
    delay(300);
    digitalWrite(SPEAKER, LOW);
    delay(1000);
    digitalWrite(D0, LOW);
    digitalWrite(SUCCESS_LED, LOW);
  }

  void error() {
    digitalWrite(BUSY_LED, LOW);
    Serial.println("Authorization failed.");
    digitalWrite(ERROR_LED, HIGH);

    for (int i = 0; i < 3; i++) {
      digitalWrite(SPEAKER, HIGH);
      delay(300);
      digitalWrite(SPEAKER, LOW);
      delay(200);
    }

    delay(3000);
    digitalWrite(ERROR_LED, LOW);
  }
};
