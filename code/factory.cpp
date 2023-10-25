#include "factory.h"
#include "extractor.h"
#include "costs.h"
#include "wholesale.h"
#include <pcosynchro/pcothread.h>
#include <iostream>

WindowInterface* Factory::interface = nullptr;


Factory::Factory(int uniqueId, int fund, ItemType builtItem, std::vector<ItemType> resourcesNeeded)
    : Seller(fund, uniqueId), resourcesNeeded(resourcesNeeded), itemBuilt(builtItem), nbItemsBuilt(0)
{
    assert(builtItem == ItemType::Chip ||
           builtItem == ItemType::Plastic ||
           builtItem == ItemType::Robot);

    interface->updateFund(uniqueId, fund);
    interface->consoleAppendText(uniqueId, "Factory created");
}

void Factory::setWholesalers(std::vector<Wholesale *> wholesalers) {
    Factory::wholesalers = wholesalers;

    for(Seller* seller: wholesalers){
        interface->setLink(uniqueId, seller->getUniqueId());
    }
}

ItemType Factory::getItemBuilt() {
    return itemBuilt;
}

int Factory::getMaterialCost() {
    return getCostPerUnit(itemBuilt);
}

bool Factory::verifyResources() {
    for (auto item : resourcesNeeded) {
        if (stocks[item] == 0) {
            return false;
        }
    }
    return true;
}

void Factory::buildItem() {
    auto employee = getEmployeeThatProduces(itemBuilt);
    auto cost = getEmployeeSalary(employee);

    bool transactionSuccessful = false;

    startTransaction();

    if(money >= cost) {
        money -= cost;
        ++stocks[itemBuilt];
        ++nbItemsBuilt;

        for(auto ingredient : resourcesNeeded) {
            --stocks[ingredient];
        }

        transactionSuccessful = true;
    }

    finishTransaction();

    //Temps simulant l'assemblage d'un objet.
    if(transactionSuccessful)
        PcoThread::usleep((rand() % 100) * 10 * TIME_MULTIPLIER);
}

bool Factory::tryToBuildItem() {
    if(verifyResources()) {
        buildItem();
        return true;
    }
    return false;
}

static bool compareResources(const std::pair<ItemType, int>& res1, const std::pair<ItemType, int>& res2) {
    // tri par disponibilité, les ressources les moins abondantes d'abord
    return res1.second < res2.second;
}

void Factory::orderResources() {
    std::vector<std::pair<ItemType, int>> ingredientsPriority;
    for(auto ingredient : resourcesNeeded)
        ingredientsPriority.push_back({ingredient, stocks[ingredient]});
    std::sort(ingredientsPriority.begin(), ingredientsPriority.end(), compareResources);

    auto item = ingredientsPriority.front().first;

    startTransaction();

    if(money >= getCostPerUnit(item)) {
        for(auto seller : wholesalers) {
            int totalPrice = seller->trade(item,1);
            if(totalPrice != NO_TRADE) {
                money -= totalPrice;
                ++stocks[item];
                break;
            }
        }
    }

    finishTransaction();

    //Temps de pause pour éviter trop de demande
    PcoThread::usleep(10 * 10 * TIME_MULTIPLIER);
}

void Factory::run() {
    if (wholesalers.empty()) {
        std::cerr << "You have to give factories wholesalers to sell their resources" << std::endl;
        return;
    }
    interface->consoleAppendText(uniqueId, "[START] Factory routine");

    while (!needsToStop()) {
        if(tryToBuildItem()) {
            interface->consoleAppendText(uniqueId, "Factory has built a new object");
        }
        else {
            orderResources();
        }

        interface->updateFund(uniqueId, money);
        interface->updateStock(uniqueId, &stocks);
    }
    interface->consoleAppendText(uniqueId, "[STOP] Factory routine");
}

std::map<ItemType, int> Factory::getItemsForSale() {
    return std::map<ItemType, int>({{itemBuilt, stocks[itemBuilt]}});
}

int Factory::getAmountPaidToWorkers() {
    return Factory::nbItemsBuilt * getEmployeeSalary(getEmployeeThatProduces(itemBuilt));
}

void Factory::setInterface(WindowInterface *windowInterface) {
    interface = windowInterface;
}

PlasticFactory::PlasticFactory(int uniqueId, int fund) :
    Factory::Factory(uniqueId, fund, ItemType::Plastic, {ItemType::Petrol}) {}

ChipFactory::ChipFactory(int uniqueId, int fund) :
    Factory::Factory(uniqueId, fund, ItemType::Chip, {ItemType::Sand, ItemType::Copper}) {}

RobotFactory::RobotFactory(int uniqueId, int fund) :
    Factory::Factory(uniqueId, fund, ItemType::Robot, {ItemType::Chip, ItemType::Plastic}) {}
