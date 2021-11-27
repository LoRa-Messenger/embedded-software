#define DEF__PINGACK_BODY

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation <PingACK.h>
#endif

#include <PingACK.h>
#include <AbstractACK.h>

using namespace std;

PingACK::PingACK(const byte recipientId, const byte senderId, const uint32_t messageId, const uint32_t timestamp) : AbstractACK(recipientId, senderId, messageId, timestamp) {}

int PingACK::sendPacket() {
    // TODO
}
