#include <iostream>
#include "Database.h"
#include <filesystem>

using namespace std;

TableScheme getStudentScheme() {
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

    return studentScheme;
}

TableScheme getAnimalScheme() {
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

    return animalsScheme;
}

TableScheme getTestScheme() {
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

    return testScheme;
}

void fillTestScheme(Database* db) {
    int integer1 = 123;
    int integer2 = 1234;
    int integer3 = 12345;
    float float1 = 0.14;
    char* test_varchar_data1 = reinterpret_cast<char*>(malloc(10));
    char* test_varchar_data2 = reinterpret_cast<char*>(malloc(10));
    for(int i = 0; i < 10; i++) {
        *(test_varchar_data1 + i) = '0' + i;
        *(test_varchar_data2 + i) = 'a' + i;
    }

    string start_of_string_text("abc");

    vector<string> testColumns = {"id", "1", "2", "3", "4", "5", "6"};
    vector<Value> testValues;
    vector<Value> testValues2;

    int id1 = 111;
    int id2 = 222;
    int id3 = 333;

    testValues.emplace_back(FieldTypes::INT, &id1, sizeof(int));
    testValues.emplace_back(FieldTypes::INT, &integer1, 4);
    testValues.emplace_back(FieldTypes::INT, &integer2, 4);
    testValues.emplace_back(FieldTypes::INT, &integer3, 4);
    testValues.emplace_back(FieldTypes::FLOAT, &float1, 4);
    testValues.emplace_back(FieldTypes::VARCHAR, test_varchar_data1, 10);
    testValues.emplace_back(FieldTypes::TEXT, start_of_string_text.data(), 4);

    void* lol = start_of_string_text.data();
    void* kek = malloc(start_of_string_text.size() + 1 + 4);
    for(int i = 0; i < start_of_string_text.size() + 1 + 4; i++) {
        *(reinterpret_cast<char*>(kek) + i) = 0xAA;
    }
    for(int i = 0; i < start_of_string_text.size() + 1; i++) {
        memcpy((reinterpret_cast<char*>(kek)) + i, (reinterpret_cast<char*>(lol)) + i, 1);
    }
    int xd = 10;
    free(kek);

    testValues2.emplace_back(FieldTypes::INT, &id2, sizeof(int));
    testValues2.emplace_back(FieldTypes::INT, &integer1, 4);
    testValues2.emplace_back(FieldTypes::INT, &integer2, 4);
    testValues2.emplace_back(FieldTypes::INT, &integer3, 4);
    testValues2.emplace_back(FieldTypes::FLOAT, &float1, 4);
    testValues2.emplace_back(FieldTypes::VARCHAR, test_varchar_data2, 10);
    testValues2.emplace_back(FieldTypes::TEXT, start_of_string_text.data(), 4);

    db->insert("test",  testColumns, testValues2);
    db->insert("test", testColumns, testValues);
}

int main() {

    try {
        std::filesystem::remove_all("example");
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error deleting directory: " << e.what() << std::endl;
    }

    TableScheme studentScheme = getStudentScheme();
    TableScheme animalsScheme = getAnimalScheme();
    TableScheme testScheme = getTestScheme();

    try {
        Database db;
        db.name = "example";
        db.init();
        db.readAllTables();

        db.createTable(studentScheme);
        db.createTable(animalsScheme);
        db.createTable(testScheme);

        fillTestScheme(&db);

        cout << "BEFORE:" << endl;
        vector<vector<Value>> values = db.readAllValuesFromTable(*(db.getTableByName("test")));
        for(auto &row : values) {
            cout << Util::convertRowToString(testScheme, row) << endl;
        }

        cout << "*erasing id = 111*" << endl;
        db.removeById("test", 111);

        cout << "AFTER:" << endl;
        vector<vector<Value>> values2 = db.readAllValuesFromTable(*(db.getTableByName("test")));
        for(auto &row : values) {
            cout << Util::convertRowToString(testScheme, row) << endl;
        }
    } catch (std::invalid_argument &e) {
        string message = e.what();
        cout << "Error: " + message;
    }

    return 0;
}
