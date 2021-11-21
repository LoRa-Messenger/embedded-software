#define DEF__READACK_BODY

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation <ReadACK.h>
#endif

#include <ReadACK.h>
#include <AbstractACK.h>

using namespace std;

ReadACK::ReadACK(const int recipientId, const int senderId, const int messageId, const int timestamp) : AbstractACK(recipientId, senderId, messageId, timestamp) {}

int ReadACK::sendPacket() {
    // TODO
}
