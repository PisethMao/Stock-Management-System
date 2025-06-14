#pragma once
#include "User.hpp"
#include <string>
#include <vector>
#include <iostream>
using namespace std;
struct StockItem
{
    int id;
    string type;
    string brand;
    string model;
    int year;
    string origin;
    int quantity;
    double price;
};
class StockManager
{
private:
    vector<StockItem> items;
    vector<User> users;
    static int nextId;

public:
    StockManager();
    void loadSampleData();
    void loadDataFromFile();
    void saveDataToFile();
    void createRecord();
    void displayData();
    void searchById(int id) const;
    void searchByType(const string &type) const;
    void searchByBrand(const string &brand) const;
    void searchByModel(const string &model) const;
    void filterByYear(int year) const;
    void filterByOrigin(const string &origin) const;
    void filterByQuantity(int quatity) const;
    void filterByPrice(double price) const;
    void updateRecord();
    void deleteRecord();
    void sortRecordByIdASC();
    void sortRecordByIdDESC();
    void sortRecordByTypeASC();
    void sortRecordByTypeDESC();
    void sortRecordByBrandASC();
    void sortRecordByBrandDESC();
    void sortRecordByModelASC();
    void sortRecordByModelDESC();
    void sortRecordByYearASC();
    void sortRecordByYearDESC();
    void sortRecordByOriginASC();
    void sortRecordByOriginDESC();
    void sortRecordByQuantityASC();
    void sortRecordByQuantityDESC();
    void sortRecordByPriceASC();
    void sortRecordByPriceDESC();
    void logout();
    void viewAllCustomers(const vector<User> &users);
    void deleteCustomer();
    void setUsers(const vector<User> &newUsers);
    const vector<StockItem> &getItems() const { return items; }
    const vector<User> &getUsers() const { return users; }
    vector<StockItem>& getItems() { return items; }
};
