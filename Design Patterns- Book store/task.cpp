#include <iostream>
#include <vector>
#include <memory>

using namespace std;

// Strategy Pattern (Pricing)

class PricingStrategy {
public:
    virtual double calculatePrice(double basePrice) = 0;
    virtual ~PricingStrategy() {}
};

class RegularPricing : public PricingStrategy {
public:
    double calculatePrice(double basePrice) override {
        return basePrice;
    }
};

class DiscountPricing : public PricingStrategy {
public:
    double calculatePrice(double basePrice) override {
        return basePrice * 0.8;  
    }
};

// Book Class

class Book {
private:
    string title;
    double basePrice;
    unique_ptr<PricingStrategy> strategy;

public:
    Book(string t, double p, unique_ptr<PricingStrategy> s)
        : title(t), basePrice(p), strategy(move(s)) {}

    void display() {
        cout << "Book: " << title
             << " | Price: "
             << strategy->calculatePrice(basePrice)
             << endl;
    }

    string getTitle() { return title; }
};

// Factory Pattern

class BookFactory {
public:
    static unique_ptr<Book> createBook(string title, double price, bool discount) {
        if (discount)
            return make_unique<Book>(title, price, make_unique<DiscountPricing>());
        else
            return make_unique<Book>(title, price, make_unique<RegularPricing>());
    }
};

// Observer Pattern

class Observer {
public:
    virtual void update(string message) = 0;
};

class Customer : public Observer {
private:
    string name;

public:
    Customer(string n) : name(n) {}

    void update(string message) override {
        cout << "Notification to " << name << ": " << message << endl;
    }
};

// Singleton BookStore

class BookStore {
private:
    static BookStore* instance;
    vector<unique_ptr<Book>> books;
    vector<Observer*> customers;

    BookStore() {}

public:
    static BookStore* getInstance() {
        if (!instance)
            instance = new BookStore();
        return instance;
    }

    void addCustomer(Observer* customer) {
        customers.push_back(customer);
    }

    void addBook(unique_ptr<Book> book) {
        string title = book->getTitle();
        books.push_back(move(book));

        for (auto c : customers)
            c->update("New book available: " + title);
    }

    void showBooks() {
        for (auto& book : books)
            book->display();
    }
};

BookStore* BookStore::instance = nullptr;

// Main

int main() {

    BookStore* store = BookStore::getInstance();

    Customer c1("Kaushik");
    store->addCustomer(&c1);

    auto book1 = BookFactory::createBook("C++ Basics", 500, false);
    auto book2 = BookFactory::createBook("Design Patterns", 800, true);

    store->addBook(move(book1));
    store->addBook(move(book2));

    cout << "\nAvailable Books:\n";
    store->showBooks();

    return 0;
}
