#include "key.h"

Key::Key(String _id, String _serialNumber, String _user, long _issuedAt, long _notBefore, long _expiresAt){
  user = _user; 
  id = _id;
  serialNumber = _serialNumber;
  issuedAt = _issuedAt;
  notBefore = _notBefore;
  expiresAt = _expiresAt;
}

bool Key::checkSN(String serialNumberFromCard){
  return serialNumber == serialNumberFromCard;
}

bool Key::isExpired(){
  if(!connected) return true;
  return epochTime > expiresAt;
}

bool Key::isActivated(){
  return epochTime > issuedAt && epochTime > notBefore;
}

bool Key::isBlacklisted(){
  return false;
}