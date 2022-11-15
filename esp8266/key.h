#ifndef Key_h
#define Key_h


class Key {
  private:
    String user; 
    String id;
    String serialNumber;
    long issuedAt;
    long notBefore;
    long expiresAt;

  public:
    Key(String _id, String _serialNumber, String _user, long _issuedAt, long _notBefore, long _expiresAt);
    bool checkSN(String serialNumberFromCard);
    bool isExpired();
    bool isActivated();
    bool isBlacklisted();
};

#endif

