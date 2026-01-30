#include <iostream>
#include <string>

using namespace std;

/*
 Abstract base class
 This defines the interface for all book types
*/
class Book {
public:
    virtual void read() = 0;   // pure virtual function
    virtual ~Book() {}         // virtual destructor
};

/*
 Concrete class: EBook
*/
class EBook : public Book {
public:
    void read() override {
        cout << "Reading an E-Book on a digital device." << endl;
    }
};

/*
 Concrete class: PrintedBook
*/
class PrintedBook : public Book {
public:
    void read() override {
        cout << "Reading a printed book." << endl;
    }
};

/*
 Factory class
 Responsible for creating Book objects
*/
class BookFactory {
public:
    static Book* createBook(const string& type) {
        if (type == "ebook") {
            return new EBook();
        }
        else if (type == "printed") {
            return new PrintedBook();
        }
        else {
            return nullptr;
        }
    }
};

/*
 Client code
*/
int main() {
    Book* book1 = BookFactory::createBook("ebook");
    Book* book2 = BookFactory::createBook("printed");

    if (book1) {
        book1->read();
    }

    if (book2) {
        book2->read();
    }

    // Clean up memory
    delete book1;
    delete book2;

    return 0;
}