#define DEF__REGULARMESSAGE_BODY

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation <RegularMessage.h>
#endif

#include <RegularMessage.h>
#include <AbstractMessage.h>

using namespace std;

RegularMessage::RegularMessage(const byte recipientId, const byte senderId, const uint32_t messageId, const uint32_t timestamp, const std::string message) : AbstractMessage(recipientId, senderId, messageId, timestamp) {
  this->message = message;  
}

int RegularMessage::sendPacket() {
  LoRa.beginPacket();

  // ~~~~~~~~~~~~~~
  // MESSAGE HEADER
  // ~~~~~~~~~~~~~~

  byte header[8];

  // first byte contains recipientId, senderId, and messageType
  header[0] = (byte) (0x00 | (recipientId << 0x05) | (senderId << 0x02) | REGULAR_MESSAGE);

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

  LoRa.print(message.c_str());

  return LoRa.endPacket();
}

std::string RegularMessage::getMessage() {
  return this->message;
}