#include <lemon/shared/SlotMap.h>

using namespace lemon;
using namespace lemon::slotmap_detail;

SlotMapKey::SlotMapKey() : dataIndex{0}, generation{0}, next{}
{
    next.init(nullptr, 0);
}

void
SlotMapKey::setNext(SlotMapKey* pNext)
{
    next.set(pNext);
}

SlotMapKey*
SlotMapKey::getNext() const
{
    return next.get();
}

bool
SlotMapKey::isUsed() const
{
    return next.extra() == 1;
}

void
SlotMapKey::setUsed(bool bUsed)
{
    return next.setExtra(bUsed ? 1 : 0);
}

void
SlotMapKey::bump()
{
    generation++;
}
