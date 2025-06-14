#pragma once
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
    // void deleteRecord();
    // void insertStockItem();
    // void sortRecord();
    // void logout();
    const vector<StockItem> &getItems() const { return items; }
    vector<StockItem>& getItems() { return items; }
};
