//
// Modifié par Rémi Ançay et Lucas Charbonnier
//

#ifndef SELLER_H
#define SELLER_H

#include <QString>
#include <QStringBuilder>
#include <map>
#include <vector>
#include "costs.h"

#include <pcosynchro/pcomutex.h>

enum class ItemType { Sand, Copper, Petrol, Chip, Plastic, Robot, Nothing};

int getCostPerUnit(ItemType item);
QString getItemName(ItemType item);

enum class EmployeeType {Extractor, Electrician, Plasturgist, Engineer};

EmployeeType getEmployeeThatProduces(ItemType item);
int getEmployeeSalary(EmployeeType employee);

class Seller {
public:
    /**
     * @brief Seller
     * @param money money money !
     */
    Seller(int money, int uniqueId) : money(money), uniqueId(uniqueId), stopRequested(false) {}

    /**
     * @brief getItemsForSale
     * @return The list of items for sale
     */
    virtual std::map<ItemType, int> getItemsForSale() = 0;

    /**
     * @brief Fonction permettant d'acheter des ressources au vendeur
     * @param Le type de ressource à acheter
     * @param Nombre de ressources voulant être achetées
     * @return La facture : côut de la ressource * le nombre, 0 si indisponible
     */
    virtual int trade(ItemType what, int qty);

    /**
     * @brief Indique au Seller qu'il doit arrêter de travailler
     */
    void requestStop();

    /**
     * @return true si l'arrêt a été demandé, sinon false
     */
    bool needsToStop() const;

    /**
     * @brief chooseRandomSeller
     * @param sellers
     * @return Returns a random seller from the sellers vector
     */
    static Seller* chooseRandomSeller(std::vector<Seller*>& sellers);

    /**
     * @brief Chooses a random item type from an items for sale map
     * @param itemsForSale
     * @return Returns the item type
     */
    static ItemType chooseRandomItem(std::map<ItemType, int>& itemsForSale);

    int getFund() { return money; }

    int getUniqueId() { return uniqueId; }

protected:
    /**
     * @brief stocks : Type, Quantité
     */
    std::map<ItemType, int> stocks;
    int money;
    int uniqueId;

    static constexpr unsigned TIME_MULTIPLIER = 10000;
    static constexpr int NO_TRADE = 0;

    /**
     * @brief Indique qu'une transaction est en cours et que personne d'autre ne doit accéder aux resources
     * Si une transaction est déjà en cours, cette méthode attendra la fin de l'autre transaction avant de se terminer.
     */
    void startTransaction();

    /**
     * @brief Indique que la transaction est terminée
     */
    void finishTransaction();

private:
    PcoMutex resourcesMutex;
    bool stopRequested; // privé car les classes dérivées ne devraient pas avoir le droit d'annuler l'arrêt
};

#endif // SELLER_H
