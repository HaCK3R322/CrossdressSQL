#include <iostream>
#include "Database.h"
#include "Manager.h"
#include "translation/Factor.h"
#include <filesystem>
#include <stack>
#include "translation/Lexer.h"
#include "translation/Translator.h"

using namespace std;

TableScheme getStudentScheme() {
    TableScheme studentScheme;
    studentScheme.name = "students";

    std::vector<FieldDescription> studentFields;

    FieldDescription field1("id", FieldTypes::INT);
    field1.IS_PRIMARY_KEY = true;
    FieldDescription field2("name", FieldTypes::TEXT);
    FieldDescription field3("age", FieldTypes::INT);
    field3.NULLABLE = true;
    FieldDescription field4("weight", FieldTypes::FLOAT);
    field4.NULLABLE = true;

    studentFields.push_back(field1);
    studentFields.push_back(field2);
    studentFields.push_back(field3);
    studentFields.push_back(field4);

    studentScheme.fields = studentFields;

    return studentScheme;
}

void fillStudentsScheme(Database* db, int numberOfRows) {
    vector<vector<Value>> values;
    vector<string> columns = {"id", "name", "age", "weight"};
    string start_of_string_text("12345");
    int age_data = 21;
    float weight = 86.2;

    db->LOG = false;
    for(int i = 0; i < numberOfRows; i++) {
        if(i % 10 == 0) weight += 10;

        vector<Value> testValues;
        testValues.emplace_back(FieldTypes::INT, &i, sizeof(int));
        testValues.emplace_back(FieldTypes::TEXT, start_of_string_text.data(), start_of_string_text.size() + 1);
        testValues.emplace_back(FieldTypes::INT, &age_data, 4);
        testValues.emplace_back(FieldTypes::FLOAT, &weight, 4);

        values.push_back(testValues);
    }

    // additional
    vector<Value> testValues;
    int additional_id = 1000;
    string additional_start_of_string_text = "1234567890";
    testValues.emplace_back(FieldTypes::INT, &additional_id, sizeof(int));
    testValues.emplace_back(FieldTypes::TEXT, additional_start_of_string_text.data(), additional_start_of_string_text.size() + 1);
    testValues.emplace_back(FieldTypes::INT, &age_data, 4);
    testValues.emplace_back(FieldTypes::FLOAT, &weight, 4);

    values.push_back(testValues);

    db->insert("students", columns, values);
    db->LOG = true;
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
    char* test_varchar_data3 = reinterpret_cast<char*>(malloc(10));
    for(int i = 0; i < 10; i++) {
        *(test_varchar_data1 + i) = '0' + i;
        *(test_varchar_data2 + i) = 'a' + i;
        *(test_varchar_data3 + i) = 'A' + i;
    }

    string start_of_string_text("abc");

    vector<string> testColumns = {"id", "1", "2", "3", "4", "5", "6"};
    vector<Value> testValues;
    vector<Value> testValues2;
    vector<Value> testValues3;

    int id1 = 1;
    int id2 = 2;
    int id3 = 3;

    testValues.emplace_back(FieldTypes::INT, &id1, sizeof(int));
    testValues.emplace_back(FieldTypes::INT, &integer1, 4);
    testValues.emplace_back(FieldTypes::INT, &integer2, 4);
    testValues.emplace_back(FieldTypes::INT, &integer3, 4);
    testValues.emplace_back(FieldTypes::FLOAT, &float1, 4);
    testValues.emplace_back(FieldTypes::VARCHAR, test_varchar_data1, 10);
    testValues.emplace_back(FieldTypes::TEXT, start_of_string_text.data(), 4);

    testValues2.emplace_back(FieldTypes::INT, &id2, sizeof(int));
    testValues2.emplace_back(FieldTypes::INT, &integer1, 4);
    testValues2.emplace_back(FieldTypes::INT, &integer2, 4);
    testValues2.emplace_back(FieldTypes::INT, &integer3, 4);
    testValues2.emplace_back(FieldTypes::FLOAT, &float1, 4);
    testValues2.emplace_back(FieldTypes::VARCHAR, test_varchar_data2, 10);
    testValues2.emplace_back(FieldTypes::TEXT, start_of_string_text.data(), 4);

    testValues3.emplace_back(FieldTypes::INT, &id3, sizeof(int));
    testValues3.emplace_back(FieldTypes::INT, &integer1, 4);
    testValues3.emplace_back(FieldTypes::INT, &integer2, 4);
    testValues3.emplace_back(FieldTypes::INT, &integer3, 4);
    testValues3.emplace_back(FieldTypes::FLOAT, &float1, 4);
    testValues3.emplace_back(FieldTypes::VARCHAR, test_varchar_data3, 10);
    testValues3.emplace_back(FieldTypes::TEXT, start_of_string_text.data(), 4);

    db->insert("test", testColumns, testValues);
    db->insert("test",  testColumns, testValues2);
    db->insert("test",  testColumns, testValues3);
}

void insertRowsToTest(Database* db, int numberOfRows) {
    int integer1 = 123;
    int integer2 = 1234;
    int integer3 = 12345;
    float float1 = 0.14;
    char* test_varchar_data1 = reinterpret_cast<char*>(malloc(10));
    for(int i = 0; i < 10; i++) {
        *(test_varchar_data1 + i) = '0' + i;
    }

    string start_of_string_text("abc");

    vector<string> testColumns = {"id", "1", "2", "3", "4", "5", "6"};

    int start_id = 1000;
    for(int i = 0; i < numberOfRows; i++) {
        start_id += 1;

        vector<Value> testValues;
        testValues.emplace_back(FieldTypes::INT, &start_id, sizeof(int));
        testValues.emplace_back(FieldTypes::INT, &integer1, 4);
        testValues.emplace_back(FieldTypes::INT, &integer2, 4);
        testValues.emplace_back(FieldTypes::INT, &integer3, 4);
        testValues.emplace_back(FieldTypes::FLOAT, &float1, 4);
        testValues.emplace_back(FieldTypes::VARCHAR, test_varchar_data1, 10);
        testValues.emplace_back(FieldTypes::TEXT, start_of_string_text.data(), 4);

        db->insert("test", testColumns, testValues);

        cout << "\r" << i;
    }

}

void insertRowsToTestV2(Database* db, int numberOfRows) {
    int integer1 = 123;
    int integer2 = 1234;
    int integer3 = 12345;
    float float1 = 0.14;
    char* test_varchar_data1 = reinterpret_cast<char*>(malloc(10));
    for(int i = 0; i < 10; i++) {
        *(test_varchar_data1 + i) = '0' + i;
    }

    string start_of_string_text("abc");

    vector<string> testColumns = {"id", "1", "2", "3", "4", "5", "6"};

    int start_id = 1000;
    vector<vector<Value>> rows;
    for(int i = 0; i < numberOfRows; i++) {
        start_id += 1;

        vector<Value> testValues;
        testValues.emplace_back(FieldTypes::INT, &start_id, sizeof(int));
        testValues.emplace_back(FieldTypes::INT, &integer1, 4);
        testValues.emplace_back(FieldTypes::INT, &integer2, 4);
        testValues.emplace_back(FieldTypes::INT, &integer3, 4);
        testValues.emplace_back(FieldTypes::FLOAT, &float1, 4);
        testValues.emplace_back(FieldTypes::VARCHAR, test_varchar_data1, 10);
        testValues.emplace_back(FieldTypes::TEXT, start_of_string_text.data(), 4);

        rows.push_back(testValues);
    }
    cout << "\n";

    db->insert("test", testColumns, rows);

}

void clearData() {
    try {
        std::filesystem::remove_all("example");
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error deleting directory: " << e.what() << std::endl;
    }
}

int main() {
    Manager manager;

    manager.createDatabase("example");
    manager.createTable("example", getStudentScheme());
    fillStudentsScheme(manager.getDatabase("example"), 1);

    while(true) {
        string query;
        cout << ">>";
        getline(cin, query, '\n');

        if(query == "exit") break;

        try {
            void* responseData = reinterpret_cast<vector<Row>*>(manager.executeQuery(query));

            if(Util::splitByDelimiter(query, ' ').size() > 1
            and Util::parseKeyWord(Util::splitByDelimiter(query, ' ')[0]) == KeyWords::SELECT) {
                auto* rows = reinterpret_cast<vector<Row>*>(responseData);
                cout << Util::convertRowsToString(*rows) << endl;
            } else {
                auto* response = reinterpret_cast<string*>(responseData);
                cout << *response << endl;
            }
        } catch (invalid_argument ex) {
            cout << "ERROR: " << ex.what() << endl;
        }
    }

    return 0;
}
