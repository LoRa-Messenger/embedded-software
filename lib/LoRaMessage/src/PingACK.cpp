#define DEF__PINGACK_BODY

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation <PingACK.h>
#endif

#include <PingACK.h>
#include <AbstractACK.h>

using namespace std;

PingACK::PingACK(const int recipientId, const int senderId, const int messageId, const int timestamp) : AbstractACK(recipientId, senderId, messageId, timestamp) {}

int PingACK::sendPacket() {
    // TODO
}
