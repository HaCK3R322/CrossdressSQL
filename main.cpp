#include <iostream>
#include "Database.h"
#include <filesystem>

using namespace std;

int main() {

    try {
        std::filesystem::remove_all("example");
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error deleting directory: " << e.what() << std::endl;
    }

    TableScheme studentScheme;
    FieldDescription field1("id", FieldTypes::INT);
    field1.IS_PRIMARY_KEY = true;
    FieldDescription field2("name", FieldTypes::STRING);
    field2.IS_UNIQUE = true;
    std::vector<FieldDescription> studentFields;
    studentFields.push_back(field1);
    studentFields.push_back(field2);
    studentScheme.name = "student";
    studentScheme.fields = studentFields;

    TableScheme animalsScheme;
    FieldDescription field3("id", FieldTypes::INT);
    field3.IS_PRIMARY_KEY = true;
    FieldDescription field4("name", FieldTypes::STRING);
    field4.IS_UNIQUE = true;
    std::vector<FieldDescription> animalFields;
    animalFields.push_back(field3);
    animalFields.push_back(field4);
    animalsScheme.name = "animals";
    animalsScheme.fields = animalFields;

    try {
        Database db;
        db.name = "example";
        db.init();
        db.readAllTables();

        db.createTable(studentScheme);
        db.createTable(animalsScheme);

        db.saveAllTables();

        db.dropTable("animals");
    } catch (std::invalid_argument &e) {
        std::string message = e.what();
        cout << "Error: " + message;
    }

    return 0;
}
