#define DEF__PINGMESSAGE_BODY

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation <PingMessage.h>
#endif

#include <PingMessage.h>
#include <AbstractMessage.h>

using namespace std;

PingMessage::PingMessage(const byte recipientId, const byte senderId, const uint32_t messageId, const uint32_t timestamp, const float latitude, const float longitude) : AbstractMessage(recipientId, senderId, messageId, timestamp) {
  this->latitude = latitude;
  this->longitude = longitude;  
}

int PingMessage::sendPacket() {
  LoRa.beginPacket();

  // ~~~~~~~~~~~~~~
  // MESSAGE HEADER
  // ~~~~~~~~~~~~~~

  byte header[8];

  // first byte contains recipientId, senderId, and messageType
  header[0] = (byte) (0x00 | (recipientId << 0x05) | (senderId << 0x02) | PING_MESSAGE);

  // second, third, and fourth bytes come from messageId
  header[1] = (byte) (0x00 | ((messageId & 0x00FF0000) >> 0x10));
  header[2] = (byte) (0x00 | ((messageId & 0x0000FF00) >> 0x08));
  header[3] = (byte) (0x00 | ((messageId & 0x000000FF)));

  // fifth through eigth bytes come from timestamp
  header[4] = (byte) (0x00 | ((timestamp & 0xFF000000) >> 0x18));
  header[5] = (byte) (0x00 | ((timestamp & 0x00FF0000) >> 0x10));
  header[6] = (byte) (0x00 | ((timestamp & 0x0000FF00) >> 0x08));
  header[7] = (byte) (0x00 | ((timestamp & 0x000000FF)));

  LoRa.write(header, 8);

  // ~~~~~~~~~~~~
  // MESSAGE BODY
  // ~~~~~~~~~~~~

  byte body[8];
  
  // TODO: fix the casting as per https://stackoverflow.com/questions/1723575/how-to-perform-a-bitwise-operation-on-floating-point-numbers
  // first four bytes of body come from latitude
  // body[0] = (byte) (0x00 | ((latitude & 0xFF000000) >> 0x18));
  // body[1] = (byte) (0x00 | ((latitude & 0x00FF0000) >> 0x10));
  // body[2] = (byte) (0x00 | ((latitude & 0x0000FF00) >> 0x08));
  // body[3] = (byte) (0x00 | ((latitude & 0x000000FF)));

  // // next four bytes of body come from longitude
  // body[4] = (byte) (0x00 | ((longitude & 0xFF000000) >> 0x18));
  // body[5] = (byte) (0x00 | ((longitude & 0x00FF0000) >> 0x10));
  // body[6] = (byte) (0x00 | ((longitude & 0x0000FF00) >> 0x08));
  // body[7] = (byte) (0x00 | ((longitude & 0x000000FF)));

  // LoRa.write(body, 8);

  return LoRa.endPacket();
}

float PingMessage::getLatitude() {
  return this->latitude;
}

float PingMessage::getLongitude() {
  return this->longitude;
}