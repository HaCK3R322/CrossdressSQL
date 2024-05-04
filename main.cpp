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

//    std::string expression = "NOT    (   NOT((   aANDbc <= 1  AND a = '123' OR a = 1) AND (a = 1 OR NOT a = 1)) AND a = 1 ) OR a = 1 AND a = 1   OR a = 1";
//    std::string expression = "NOT (a = 1 OR b = 2) AND c = 3";
    std::string expression = "id = 10";
    cout << "Expression:\n" << expression << endl << endl;

    expression = Factor::replaceLogicOperators(expression);
    cout << "Replaced operations:\n" << expression << endl << endl;

    expression = Factor::removeSpaces(expression);
    cout << "Trimmed:\n" << expression << endl << endl;

    cout << "Tokenized:\n";
    vector<string> tokens = Lexer::tokenize(expression);
    cout << '[';
    for(int i = 0; i < tokens.size(); i++) {
        cout << tokens[i];

        if(i < tokens.size() - 1) {
            cout << ", ";
        }
    }
    cout << ']' << endl << endl;

    Factor* factor = Factor::construct(tokens);

    vector<string> columns = {"id"};
    int id_data1 = 10;
    Value id1(FieldTypes::INT, &id_data1, 4);
    vector<Value> values;
    values.push_back(id1);

    int id_data2 = 100;
    Value id2(FieldTypes::INT, &id_data2, 4);
    vector<Value> values2;
    values2.push_back(id2);
    Pointer pointer(0, 0);

    Row row1(pointer, columns, values);
    Row row2(pointer, columns, values2);

    map<string, Value> variables1 = Translator::createVariables(columns, row1);
    map<string, Value> variables2 = Translator::createVariables(columns, row2);

    cout << "Evaluation:\n";
    cout << factor->evalualte(variables1) << endl;
    cout << factor->evalualte(variables2) << endl << endl;

    return 0;



//    Term x1(a, five, Operators::LESS);
//    Term x2(b, ten, Operators::EQUAL);
//
//    Factor factor;
//    factor.op = LogicalOperators::OR;
//    factor.left_term = &x1;
//    factor.right_term = &x2;
//
//    Term x3(c, five, Operators::EQUAL);
//
//    Factor factor2;
//    factor2.op = LogicalOperators::AND;
//    factor2.left_factor = &factor;
//    factor2.right_term = &x3;
//
//    cout << factor2.evalualte() << endl;

    return 0;

//    clearData();
//    Database db;
//    db.name = "example";
//    db.init();
//    TableScheme testScheme = getTestScheme();
//    db.createTable(testScheme);

//    clearData();
//
//    TableScheme studentScheme = getStudentScheme();
//    TableScheme animalsScheme = getAnimalScheme();
//    TableScheme testScheme = getTestScheme();
//
//    try {
//        Database db;
//        db.name = "example";
//        db.init();
//        db.readAllTables();
//
////        db.createTable(studentScheme);
////        db.createTable(animalsScheme);
//        db.createTable(testScheme);
//
//        fillTestScheme(&db);
//
//        Table* testTable = db.getTableByName("test");
//
//        cout << "BEFORE:" << endl;
//        vector<Row> rows = db.selectAll(*testTable);
//        for(auto &row : rows) {
//            cout << Util::convertRowToString(row) << endl;
//        }
//
//        cout << "*erasing id = 222*" << endl;
//        vector<Row> rowsToDelete;
//        for(auto const& row : rows) {
//            int valuePos = (*testTable).scheme.getFieldIndexByName("id");
//            if(Util::readInt(row.values.at(valuePos).data) == 222) {
//                rowsToDelete.push_back(row);
//                break;
//            }
//        }
//        db.deleteRows(testTable, rowsToDelete);
//
//        cout << "AFTER:" << endl;
//        vector<Row> rows2 = db.selectAll(*db.getTableByName("test"));
//        for(auto &row : rows2) {
//            cout << Util::convertRowToString(row) << endl;
//        }
//
//        cout << "\n\nIDS:" << endl;
//        vector<Row> allRows = db.selectAll("test");
//        vector<string> columnsToSelect = {"4", "id"};
//        vector<Row> ids = db.selectColumns("test", allRows, columnsToSelect);
//        for(auto &row : ids) {
//            cout << Util::convertRowToString(row) << endl;
//        }
//
//        cout << Util::convertRowsToString(ids);
//        cout << Util::convertRowsToString(allRows);
//
//
//        db.LOG = false;
//        auto start = std::chrono::high_resolution_clock::now();
//        insertRowsToTestV2(&db, 100);
//        auto stop = std::chrono::high_resolution_clock::now();
//        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
//        cout << "values was inserted. (" << duration.count() << " ms." << ")\n";
//        db.LOG = true;
//
//        start = std::chrono::high_resolution_clock::now();
//        allRows = db.selectAll("test");
//        stop = std::chrono::high_resolution_clock::now();
//        auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
//        cout << "values was selected. (" << duration2.count() << " ms." << ")\n";
//
//        vector<Row> halfOfTheRows(allRows.begin(), allRows.begin() + (allRows.size() / 2));
//        cout << db.getTableByName("test")->header.dataStartShift << endl;
//        db.deleteRows("test", halfOfTheRows);
//        db.vacuum(db.getTableByName("test"));
//        cout << db.getTableByName("test")->header.dataStartShift << endl;
//    } catch (std::invalid_argument &e) {
//        string message = e.what();
//        cout << "Error: " + message;
//    }

    Manager manager;
    manager.createDatabase("example");
    manager.databases.at(0).createTable(getTestScheme());
    manager.databases.at(0).createTable(getStudentScheme());

    // SELECT test.id, students.name FROM test, students WHERE test.id > 1 OR test.id <= 5 AND students.name == 'IVAN' ORDER BY students.name DESCENDING;

    // SELECT (column1, column2, column3, ...)|* FROM tables [WHERE conditions] [ORDER BY columns DESCENDING|ASCENDING] [LIMIT number]
    // INSERT INTO table_name (column1, column2, column3, ...) VALUES (value1, value2, value3, ...);
    // DELETE FROM table_name WHERE conditions;

    // 1) choose tables
    // 2) choose columns in those tables
    // 3) select and combine all columns
    // 4) parse filter conditions
    // 5) filter values
    // 6) sort using ORDER BY
    // 7) return values
    string query = "SELECT id FROM test, students;";

    manager.executeQuery(query, "example");
    manager.extractColumnNames(query);

    return 0;
}
