#include <iostream>
#include "Database.h"
#include "Manager.h"
#include "translation/Lexer.h"

#define WIN32_LEAN_AND_MEAN
#include <httplib.h>

using namespace std;

int main() {
    using namespace httplib;

    // Create a server instance
    Server svr;

    // Initialize the database manager
    Manager manager;
    manager.initDatabases();

    // Define a route for POST requests on "/query"
    svr.Post("/query", [&manager](const Request& req, Response& res) {
        try {
            res.set_header("Access-Control-Allow-Origin", "*");  // Allows all domains
            res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
            res.set_header("Access-Control-Allow-Headers", "Content-Type");

            std::string query = req.body; // Get the SQL query from the POST request body
            for (auto& ch : query) {
                if (ch == '\n') {
                    ch = ' ';
                }
            }

            void* responseData = manager.executeQuery(query);

            if (Util::splitByDelimiter(query, ' ').size() > 1
                && Util::parseKeyWord(Util::splitByDelimiter(query, ' ')[0]) == KeyWords::SELECT) {
                auto* rows = reinterpret_cast<std::vector<Row>*>(responseData);
                res.set_content(Util::convertRowsToString(*rows), "text/plain");
            } else {
                auto* response = reinterpret_cast<std::string*>(responseData);
                res.set_content(*response, "text/plain");
            }
        } catch (std::invalid_argument& ex) {
            res.status = 400; // Bad Request
            res.set_content("ERROR: " + std::string(ex.what()), "text/plain");
        }
    });

    // Start the server
    std::cout << "Server started at http://localhost:5431\n";
    svr.listen("localhost", 5431);

    return 0;
}
