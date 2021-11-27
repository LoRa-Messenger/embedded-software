#define DEF__ABSTRACTACK_BODY

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation <AbstractACK.h>
#endif

#include <AbstractACK.h>

using namespace std;

AbstractACK::AbstractACK(const byte recipientId, const byte senderId, const uint32_t messageId, const uint32_t timestamp) : AbstractMessage(recipientId, senderId, messageId, timestamp) {}