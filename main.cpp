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
    FieldDescription field2("name", FieldTypes::TEXT);
    field2.IS_UNIQUE = true;
    std::vector<FieldDescription> studentFields;
    studentFields.push_back(field1);
    studentFields.push_back(field2);
    studentScheme.name = "students";
    studentScheme.fields = studentFields;

    TableScheme animalsScheme;
    FieldDescription field3("id", FieldTypes::INT);
    field3.IS_PRIMARY_KEY = true;
    FieldDescription field4("name", FieldTypes::TEXT);
    field4.IS_UNIQUE = true;
    std::vector<FieldDescription> animalFields;
    animalFields.push_back(field3);
    animalFields.push_back(field4);
    animalsScheme.name = "animals";
    animalsScheme.fields = animalFields;

    TableScheme testScheme;
    vector<FieldDescription> testFields;

    FieldDescription id_description("id", FieldTypes::INT);
    id_description.IS_PRIMARY_KEY = true;
    testFields.push_back(id_description);
    testFields.emplace_back("1", FieldTypes::INT);
    testFields.emplace_back("2", FieldTypes::INT);
    testFields.emplace_back("3", FieldTypes::INT);
    testFields.emplace_back("4", FieldTypes::FLOAT);

    FieldDescription test_5_description = FieldDescription("5", FieldTypes::VARCHAR);
    test_5_description.varcharSize = 10;
    testFields.push_back(test_5_description);

    testFields.emplace_back("6", FieldTypes::TEXT);

    testScheme.name = "test";
    testScheme.fields = testFields;

    try {
        Database db;
        db.name = "example";
        db.init();
        db.readAllTables();

        db.createTable(studentScheme);
        db.createTable(animalsScheme);

        db.saveAllTables();

        db.createTable(testScheme);
        int integer1 = 0xbbbbbbbb;
        int integer2 = 0xcccccccc;
        int integer3 = 0xdddddddd;
        float float1 = 0.14;
        char* test_varchar_data = string("1234567890").data();  // Get the size of the string without the null terminator
        void* start_of_string_text = (void *) string("THISISTEXT!!\0").data();
        vector<string> testColumns = {"id", "1", "2", "3"};
        vector<Value> testValues;

        int id1 = 111;
        int id2 = 222;
        int id3 = 333;

        testValues.emplace_back(FieldTypes::INT, &id1);
        testValues.emplace_back(FieldTypes::INT, &integer1);
        testValues.emplace_back(FieldTypes::INT, &integer2);
        testValues.emplace_back(FieldTypes::INT, &integer3);
//        testValues.emplace_back(FieldTypes::FLOAT, &float1);
//        testValues.emplace_back(FieldTypes::VARCHAR, test_varchar_data);
//        testValues.emplace_back(FieldTypes::TEXT, start_of_string_text);

        db.insert("test", testColumns, testValues);
//        testValues.begin()->data = reinterpret_cast<void *>(&id2);
        db.insert("test",  testColumns, testValues);
//        testValues.begin()->data = reinterpret_cast<void *>(&id3);
//        db.insert("test",  testValues);

        cout << "BEFORE:" << endl;
        vector<vector<Value>> values = db.readAllValuesFromTable(*(db.getTableByName("test")));
        cout << values.size() << " rows!" << endl;
        for(auto &row : values) {
            for(auto &value: row) {
                cout << Util::GET_FIELD_TYPE_NAME(value.type) << endl;
            }
            cout << endl;
        }

        cout << "*erasing id = 111*" << endl;
        db.removeById("test", 111);

        cout << "AFTER:" << endl;
        values = db.readAllValuesFromTable(*(db.getTableByName("test")));
        cout << values.size() << " rows!" << endl;
        for(auto &row : values) {
            for(auto &value: row) {
                cout << Util::GET_FIELD_TYPE_NAME(value.type) << endl;
            }
            cout << endl;
        }
    } catch (std::invalid_argument &e) {
        string message = e.what();
        cout << "Error: " + message;
    }

    return 0;
}
