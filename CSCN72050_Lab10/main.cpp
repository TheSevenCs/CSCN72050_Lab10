#include "crow_all.h"
#include <fstream>
#include <sstream>
#include <string>

using namespace std;
using namespace crow;

const std::string USERNAME = "user";
const std::string PASSWORD = "password";

bool validateLogin(const std::string& username, const std::string& password) {
    return (username == USERNAME && password == PASSWORD);
}

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")
    ([=](const crow::request &req, crow::response &res) {
        ifstream in("../public/index.html", std::ifstream::in);
        if (in) {
            std::ostringstream contents;
            contents << in.rdbuf();
            res.write(contents.str()); // Write HTML content to response
            in.close();
            
        } else {
            res.code = 404;
            res.write("File not found");
        }
        res.end();
    });

    CROW_ROUTE(app, "/product/<string>")
    ([=](const crow::request &req, crow::response &res, std::string filepath) {
        ifstream in("../public/products/" + filepath + ".html", std::ifstream::in);
        if (in) {
            std::ostringstream contents;
            contents << in.rdbuf();
            res.write(contents.str()); // Write HTML content to response
            in.close();
            
        } else {
            res.code = 404;
            res.write("File not found");
        }
        res.end();
    });

    CROW_ROUTE(app, "/product/image/<string>")
    ([](const crow::request& req, crow::response& res, std::string filename) {
        ifstream in("../public/images/" + filename, ifstream::in);
        if (in) {
            ostringstream contents;
            contents << in.rdbuf();
            in.close();

            res.set_header("Content-Type", "image/jpg");
            res.write(contents.str());
        } else {
            res.code = 404;
            res.write("Image not found");
        }
        res.end();
    });

    CROW_ROUTE(app, "/add/<int>/<string>")
    ([](const crow::request& req, crow::response& res, int quantity, std::string itemID) {
        std::ofstream outFile("../shopping_cart.txt", ios_base::app);
        string item;
        if(itemID == "sour"){
            item = "Sourdough Bread";
        }

        else if(itemID == "white"){
            item = "White Bread";
        }
        else if(itemID == "rye"){
            item = "Rye Bread";
        }
        else if(itemID == "whole")
        {
            item = "Wholewheat Bread";
        }

        if (outFile.is_open()) {
            outFile << "Quantity: " << quantity << ", Item: " << item << std::endl;
            outFile.close();
        } else {
            res.code = 500;
        }
        res.redirect("/");
        res.end();
    }); 

    // Route for the checkout page
    CROW_ROUTE(app, "/checkout")
    ([](const crow::request& req, crow::response& res) {
        // Return HTTP Code 402 (Payment Required)
        res.code = 402;
        // Load the checkout page HTML
        std::ifstream file("../public/checkout.html");
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            res.write(buffer.str());
            file.close();
        } else {
            res.write("Error: Unable to open checkout page.");
        }
        res.end();
    });

    // Route for login authentication
    CROW_ROUTE(app, "/login")
    ([](const crow::request& req, crow::response& res) {
        // Get username and password from query parameters
        std::string username = req.url_params.get("username");
        std::string password = req.url_params.get("password");
        
        // Hardcoded username and password for demonstration
        std::string correctUsername = "admin";
        std::string correctPassword = "password";

        // Check if username and password match
        if (username == correctUsername && password == correctPassword) {
            // Return HTTP Code 202 (Accepted) for successful login
            res.code = 202;
            res.redirect("/cart");
        } else {
            // Return HTTP Code 401 (Unauthorized) for unsuccessful login
            res.code = 401;
            res.redirect("/checkout");
        }
        
        res.end();
    });

    CROW_ROUTE(app, "/cart")
    ([](const crow::request& req, crow::response& res) {
        std::ifstream file("../shopping_cart.txt");
            if (file.is_open()) {
                std::stringstream buffer;
                buffer << file.rdbuf();
                res.write(buffer.str());
                file.close();
                std::ofstream outFile("../shopping_cart.txt", std::ofstream::out | std::ofstream::trunc);
                outFile.close();
            } else {
                res.write("Error: displaying shopping cart");
            }
        res.end();
    });
    app.port(23500).multithreaded().run();

    return 0;
}
