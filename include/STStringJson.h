#ifndef BAKERMAKER_STSTRINGJSON_H
#define BAKERMAKER_STSTRINGJSON_H

#include "nlohmann/json.hpp"
#include "string_theory/string"

NLOHMANN_JSON_NAMESPACE_BEGIN

template<>
struct adl_serializer<ST::string> {
    static void to_json(json& j, const ST::string& str);

    static void from_json(const json& j, ST::string& str);
};



NLOHMANN_JSON_NAMESPACE_END

#endif //BAKERMAKER_STSTRINGJSON_H
