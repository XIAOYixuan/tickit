#include "xmlparser.h"
using namespace tomato;
using namespace tomato::xml;
int main() {
    Parser parser;
    auto xml = parser.load_path("test.xml");
    print(xml);
}
