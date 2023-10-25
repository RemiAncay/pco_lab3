#include "wholesale.h"
#include "factory.h"
#include "costs.h"
#include <iostream>
#include <pcosynchro/pcothread.h>

WindowInterface* Wholesale::interface = nullptr;

Wholesale::Wholesale(int uniqueId, int fund)
    : Seller(fund, uniqueId)
{
    interface->updateFund(uniqueId, fund);
    interface->consoleAppendText(uniqueId, "Wholesaler Created");

}

void Wholesale::setSellers(std::vector<Seller*> sellers) {
    this->sellers = sellers;

    for(Seller* seller: sellers){
        interface->setLink(uniqueId, seller->getUniqueId());
    }
}

void Wholesale::buyResources() {
    auto seller = Seller::chooseRandomSeller(sellers);
    auto itemsForSale = seller->getItemsForSale();
    auto itemToBuy = Seller::chooseRandomItem(itemsForSale);

    if (itemToBuy == ItemType::Nothing)
        return;

    // note : la stratégie de choisir aléatoirement le nombre d'objets à acheter va faire que les grossistes n'achèteront
    // quasi jamais de robots car il faut beaucoup de robots donc peu de chances qu'une usine aie déjà 3 ou 4 robots à
    // disposition d'un coup.
    int qty = rand() % 5 + 1;
    int price = qty * getCostPerUnit(itemToBuy);

    interface->consoleAppendText(uniqueId, QString("I would like to buy %1 of ").arg(qty) %
                                 getItemName(itemToBuy) % QString(" which would cost me %1").arg(price));

    startTransaction();

    bool transactionSuccessful = false;
    // si on a assez d'argent et que le vendeur peut nous vendre l'objet...
    if(money >= price) {
        if(seller->trade(itemToBuy, qty) != NO_TRADE) {
            // on effectue la transaction
            money -= price;
            stocks[itemToBuy] += qty;
            transactionSuccessful = true;
        }
    }

    finishTransaction();

    if(transactionSuccessful) {
        interface->consoleAppendText(uniqueId, QString("I bought %1 of ").arg(qty) %
                                     getItemName(itemToBuy) % QString(" which cost me %1 ! :)").arg(price));
    }
}

void Wholesale::run() {

    if (sellers.empty()) {
        std::cerr << "You have to give factories and mines to a wholesaler before launching is routine" << std::endl;
        return;
    }

    interface->consoleAppendText(uniqueId, "[START] Wholesaler routine");
    while (!needsToStop()) {
        buyResources();
        interface->updateFund(uniqueId, money);
        interface->updateStock(uniqueId, &stocks);
        //Temps de pause pour espacer les demandes de ressources
        PcoThread::usleep((rand() % 10 + 1) * 10 * TIME_MULTIPLIER);
    }
    interface->consoleAppendText(uniqueId, "[STOP] Wholesaler routine");


}

std::map<ItemType, int> Wholesale::getItemsForSale() {
    return stocks;
}

void Wholesale::setInterface(WindowInterface *windowInterface) {
    interface = windowInterface;
}
