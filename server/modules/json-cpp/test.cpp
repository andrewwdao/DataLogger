#include <iostream>

#include <nlohmann/json.hpp>

int main()
{
    nlohmann::json j = R"(
        {
            "timestamp": "hahahahaha",
            "payload": {
                "NameCode":"TA02",
                "P01":"0.00",
                "P02":"0.00",
                "P03":"0.00",
                "P04":"0.00",
                "Flow01":"772.20",
                "Pulse01":"108422.00",
                "Flow02":"0.00",
                "Pulse02":"0",
                "Flow03":"0.00",
                "Pulse03":"0",
                "UserName":"scadaapi",
                "Password":"scadaapi@123",
                "CheckSum":"CF20F38E"
            }
        }
    )"_json;

    std::cout << j["timestamp"] << std::endl;
    std::cout << j["payload"] << std::endl;

    return 0;
}