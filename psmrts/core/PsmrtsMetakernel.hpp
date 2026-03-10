#ifndef PSMRTS_METAKERNEL_HPP
#define PSMRTS_METAKERNEL_HPP

#include "PsmrtsTagSearch.hpp"
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

namespace psmrts {

class PsmrtsMetakernel {
public:
    PsmrtsMetakernel(const std::string& filename) {
        psmrts::PsmrtsTagSearch parser;

        parser.add_search_tag("PATH_VALUES", ")");
        parser.add_search_tag("PATH_SYMBOLS", ")");
        parser.add_search_tag("KERNELS_TO_LOAD", ")");

        if (parser.parse_file(filename)) {
            std::vector<std::string> symbols = split_clean(parser.get_value("PATH_SYMBOLS"));
            std::vector<std::string> values = split_clean(parser.get_value("PATH_VALUES"));

            // CONTENT-BASED MAPPING: Ensure key is the LAST part of the path
            for (const auto& symbol : symbols) {
                for (const auto& path : values) {
                    if (ends_with_component(path, symbol)) {
                        m_pathMap[symbol] = path;
                        break; 
                    }
                }
            }

            m_kernels = split_clean(parser.get_value("KERNELS_TO_LOAD"));
        }
    }

    const std::map<std::string, std::string>& getPathMap() const { return m_pathMap; }
    const std::vector<std::string>& getKernels() const { return m_kernels; }

private:
    std::map<std::string, std::string> m_pathMap;
    std::vector<std::string> m_kernels;

    // Helper to check if the path ends with the symbol (ignoring trailing slashes)
    bool ends_with_component(std::string path, const std::string& symbol) {
        // Remove trailing slash if it exists
        if (!path.empty() && (path.back() == '/' || path.back() == '\\')) {
            path.pop_back();
        }

        size_t pos = path.find_last_of("/\\");
        std::string lastComponent;
        
        if (pos == std::string::npos) {
            lastComponent = path;
        } else {
            lastComponent = path.substr(pos + 1);
        }

        return lastComponent == symbol;
    }

    std::vector<std::string> split_clean(std::string raw) {
        std::vector<std::string> results;
        for (char &c : raw) {
            if (c == '\'' || c == '\"' || c == ',' || c == '(' || c == ')' || c == '=') {
                c = ' ';
            }
        }

        std::stringstream ss(raw);
        std::string temp;
        while (ss >> temp) {
            results.push_back(temp);
        }
        return results;
    }
};

} // namespace psmrts

#endif
/** 
#ifndef PSMRTS_METAKERNEL_HPP
#define PSMRTS_METAKERNEL_HPP

#include "PsmrtsTagSearch.hpp"
#include <map>
#include <vector>
#include <string>

namespace psmrts {

class PsmrtsMetakernel {
public:
    PsmrtsMetakernel(const std::string& filename) {
        psmrts::PsmrtsTagSearch parser;

        // Step 1: Configure tags to capture the full parenthetical blocks
        parser.add_search_tag("PATH_VALUES", ")");
        parser.add_search_tag("PATH_SYMBOLS", ")");
        parser.add_search_tag("KERNELS_TO_LOAD", ")");

        if (parser.parse_file(filename)) {
            // Step 2: Extract and clean the path symbols and values
            std::vector<std::string> symbols = split_clean(parser.get_value("PATH_SYMBOLS"));
            std::vector<std::string> values = split_clean(parser.get_value("PATH_VALUES"));

            // Step 3: Map symbols to values
            // We assume the file is well-formed and both lists are the same length
            size_t path_count = std::min(symbols.size(), values.size());
            for (size_t i = 0; i < path_count; ++i) {
                m_pathMap[symbols[i]] = values[i];
            }

            // Step 4: Create the kernels vector
            m_kernels = split_clean(parser.get_value("KERNELS_TO_LOAD"));
        }
    }

    // Accessors
    const std::map<std::string, std::string>& getPathMap() const { return m_pathMap; }
    const std::vector<std::string>& getKernels() const { return m_kernels; }

private:
    std::map<std::string, std::string> m_pathMap;
    std::vector<std::string> m_kernels;

    // Helper to turn the raw string block into a clean vector of individual values
    std::vector<std::string> split_clean(std::string raw) {
        std::vector<std::string> results;
        
        // Remove PVL/SPICE syntax characters
        for (char &c : raw) {
            if (c == '\'' || c == '\"' || c == ',' || c == '(' || c == ')' || c == '=') {
                c = ' ';
            }
        }

        std::stringstream ss(raw);
        std::string temp;
        while (ss >> temp) {
            results.push_back(temp);
        }
        return results;
    }
};

} // namespace psmrts

#endif
*/