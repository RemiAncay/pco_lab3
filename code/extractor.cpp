#include "extractor.h"
#include "costs.h"
#include <pcosynchro/pcothread.h>

#include "constants.h"

WindowInterface* Extractor::interface = nullptr;

Extractor::Extractor(int uniqueId, int fund, ItemType resourceExtracted)
    : Seller(fund, uniqueId), resourceExtracted(resourceExtracted), nbExtracted(0)
{
    assert(resourceExtracted == ItemType::Copper ||
           resourceExtracted == ItemType::Sand ||
           resourceExtracted == ItemType::Petrol);
    interface->consoleAppendText(uniqueId, QString("Mine Created"));
    interface->updateFund(uniqueId, fund);
}

std::map<ItemType, int> Extractor::getItemsForSale() {
    return stocks;
}

void Extractor::run() {
    interface->consoleAppendText(uniqueId, "[START] Mine routine");

    while (!needsToStop()) {
        /* TODO concurrence */

        int minerCost = getEmployeeSalary(getEmployeeThatProduces(resourceExtracted));
        if (money < minerCost) {
            PcoThread::usleep(TIME_MULTIPLIER / 10);
            continue;
        }


        /* Temps aléatoire borné qui simule le mineur qui mine */
        PcoThread::usleep((rand() % 100 + 1) * TIME_MULTIPLIER);

        startTransaction();

        if(money >= minerCost) {
            money -= minerCost;
            nbExtracted++;
            stocks[resourceExtracted] += 1;
        }

        finishTransaction();

        interface->consoleAppendText(uniqueId, QString("1 ") % getItemName(resourceExtracted) %
                                     " has been mined");
        interface->updateFund(uniqueId, money);
        interface->updateStock(uniqueId, &stocks);
    }
    interface->consoleAppendText(uniqueId, "[STOP] Mine routine");
}

int Extractor::getMaterialCost() {
    return getCostPerUnit(resourceExtracted);
}

ItemType Extractor::getResourceMined() {
    return resourceExtracted;
}

int Extractor::getAmountPaidToMiners() {
    return nbExtracted * getEmployeeSalary(getEmployeeThatProduces(resourceExtracted));
}

void Extractor::setInterface(WindowInterface *windowInterface) {
    interface = windowInterface;
}

SandExtractor::SandExtractor(int uniqueId, int fund): Extractor::Extractor(uniqueId, fund, ItemType::Sand) {}

CopperExtractor::CopperExtractor(int uniqueId, int fund): Extractor::Extractor(uniqueId, fund, ItemType::Copper) {}

PetrolExtractor::PetrolExtractor(int uniqueId, int fund): Extractor::Extractor(uniqueId, fund, ItemType::Petrol) {}
