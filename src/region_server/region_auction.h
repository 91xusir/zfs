
#ifndef REGION_AUCTION_H
#define REGION_AUCTION_H

#include "region.h"
#include "auction_base.h"

extern CSalesroom	g_SalesroomBackUp;

extern bool	RemoveCharge(CAuctionItem* item);
extern bool	RemoveStorageCharge(CAuctionItem* item);
extern bool RemoveItemFromBag(CAuctionItem* item);

#endif //REGION_AUCTION_H

