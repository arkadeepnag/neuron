#include "httplib.h"

int main() {
    httplib::Server srv;

    srv.Get("/repos", [](auto&, auto& res) {
        res.set_content("[]", "application/json");
    });

    srv.listen("0.0.0.0", 8080);
}
