#include "STStringJson.h"
#include "nlohmann/json.hpp"

NLOHMANN_JSON_NAMESPACE_BEGIN

        void adl_serializer<ST::string>::to_json(json& j, const ST::string& str) {
            j = str.c_str();
        }

        void adl_serializer<ST::string>::from_json(const nlohmann::json& j, ST::string& str) {
            str = j.get<std::string>().c_str();
        }

NLOHMANN_JSON_NAMESPACE_END
