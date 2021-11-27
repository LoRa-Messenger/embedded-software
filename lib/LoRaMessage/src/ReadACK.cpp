#define DEF__READACK_BODY

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation <ReadACK.h>
#endif

#include <ReadACK.h>
#include <AbstractACK.h>

using namespace std;

ReadACK::ReadACK(const byte recipientId, const byte senderId, const uint32_t messageId, const uint32_t timestamp) : AbstractACK(recipientId, senderId, messageId, timestamp) {}

int ReadACK::sendPacket() {
    // TODO
}
