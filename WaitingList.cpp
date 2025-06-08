#include "WaitingList.h"

namespace trainsys {
    void WaitingList::addToWaitingList(const PurchaseInfo &purchaseInfo) {
        /* Question */
        purchaseQueue.enQueue(purchaseInfo);
        listSize++;
    }

    void WaitingList::removeHeadFromWaitingList() {
        /* Question */
        if (!purchaseQueue.isEmpty()) {
            purchaseQueue.deQueue();
            listSize--;
        }
    }

    const PurchaseInfo WaitingList::getFrontPurchaseInfo() const {
        /* Question */
        return purchaseQueue.getHead();
    }

    bool WaitingList::isEmpty() const {
        /* Question */
        return purchaseQueue.isEmpty();
    }

    bool WaitingList::isBusy() const {
        /* Question */
        return listSize >= BUSY_STATE_TRESHOLD;
    }

    void PrioritizedWaitingList::addToWaitingList(const PurchaseInfo &purchaseInfo) {
        /* Question */
        purchaseQueue.enQueue(purchaseInfo);
        listSize++;
    }

    void PrioritizedWaitingList::removeHeadFromWaitingList() {
        /* Question */
        if (!purchaseQueue.isEmpty()) {
            purchaseQueue.deQueue();
            listSize--;
        }
    }

    const PurchaseInfo PrioritizedWaitingList::getFrontPurchaseInfo() const {
        /* Question */
        return purchaseQueue.getHead();
    }

    bool PrioritizedWaitingList::isEmpty() const {
        /* Question */
        return purchaseQueue.isEmpty();
    }

    bool PrioritizedWaitingList::isBusy() const {
        /* Question */
        return listSize >= BUSY_STATE_TRESHOLD;
    }
} // namespace trainsys
