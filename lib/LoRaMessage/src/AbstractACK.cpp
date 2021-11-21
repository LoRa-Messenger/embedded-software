#define DEF__ABSTRACTACK_BODY

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation <AbstractACK.h>
#endif

#include <AbstractACK.h>

using namespace std;

AbstractACK::AbstractACK(const int recipientId, const int senderId, const int messageId, const int timestamp) : AbstractMessage(recipientId, senderId, messageId, timestamp) {}