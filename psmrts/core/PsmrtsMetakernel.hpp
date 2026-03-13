#ifndef PSMRTS_METAKERNEL_HPP
#define PSMRTS_METAKERNEL_HPP

#include "PsmrtsTagSearch.hpp"
#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>

// Parses a SPICE metakernel (.tm) file.
//
// Reads a single \begindata block up to EOF or \begintext and extracts:
//   PATH_SYMBOLS  — short alias names  (e.g. GENS, LRO)
//   PATH_VALUES   — corresponding paths (e.g. /kernels/gens)
//   KERNELS_TO_LOAD — list of kernel paths, $SYMBOL references resolved
//                     by substituting from the path map
//
// GENS is equal to /kernels/gens, etc.
//
// Quick-start:
//
//   PsmrtsMetakernel mk("my_mission.tm");
//
//   // Inspect the symbol→path map
//   for (auto& [sym, path] : mk.getPathMap())
//       std::cout << sym << " = " << path << "\n";
//
//   // Get all kernel paths
//   for (auto& k : mk.getKernels())
//       std::cout << k << "\n";
//
//   // Serialize back to metakernel format
//   std::string tm = mk.to_string();
//   mk.to_file("output.tm");

namespace psmrts {

class PsmrtsMetakernel {
public:

    // ---------------------------------------------------------------
    // Tag storage
    // ---------------------------------------------------------------

    /**
     * The PsmrtsTagSearch::Tag pairs used internally to locate each
     * valued section within the \begindata block.
     *
     * Stored so callers can inspect which start/end tag pairs were
     * registered, and so to_string() can reconstruct the correct
     * metakernel syntax without hardcoding tag names in multiple places.
     *
     *   m_tags[0]  →  { "PATH_VALUES",    ")" }
     *   m_tags[1]  →  { "PATH_SYMBOLS",   ")" }
     *   m_tags[2]  →  { "KERNELS_TO_LOAD",")" }
     */
    std::vector<PsmrtsTagSearch::Tag> m_tags;

    // ---------------------------------------------------------------
    // Constructor
    // ---------------------------------------------------------------

    PsmrtsMetakernel(const std::string& filename) {
        // Register tag pairs and store them for later introspection/serialization
        m_tags = {
            { "PATH_VALUES",     ")" },
            { "PATH_SYMBOLS",    ")" },
            { "KERNELS_TO_LOAD", ")" }
        };
        // Read the file and extract only the \begindata ... \begintext region
        // before handing off to PsmrtsTagSearch, so that occurrences of tag
        // keywords in the comment header (e.g. "PATH_VALUES keyword") are ignored.
        std::ifstream ifs(filename);
        if (!ifs)
            throw std::runtime_error(
                "PsmrtsMetakernel: cannot open file: " + filename);

        std::string content((std::istreambuf_iterator<char>(ifs)),
                            std::istreambuf_iterator<char>());

        // Locate \begindata — everything before it is the comment header
        size_t begin_pos = content.find("\\begindata");
        if (begin_pos == std::string::npos)
            return;  // no \begindata block found, nothing to parse
        begin_pos += std::strlen("\\begindata");

        // Locate \begintext — if absent, parse to EOF
        size_t end_pos = content.find("\\begintext", begin_pos);
        std::string block = (end_pos == std::string::npos)
            ? content.substr(begin_pos)
            : content.substr(begin_pos, end_pos - begin_pos);

        psmrts::PsmrtsTagSearch parser;
        for (const auto& tag : m_tags)
            parser.add_search_tag(tag.first, tag.second);

        parser.parse_string(block);

        std::vector<std::string> symbols = split_clean(parser.get_value("PATH_SYMBOLS"));
        std::vector<std::string> values  = split_clean(parser.get_value("PATH_VALUES"));

        for (size_t i = 0; i < symbols.size() && i < values.size(); ++i)
            m_pathMap[symbols[i]] = values[i];

        m_kernels = split_clean(parser.get_value("KERNELS_TO_LOAD"));
        /** 
        psmrts::PsmrtsTagSearch parser;
        for (const auto& tag : m_tags)
            parser.add_search_tag(tag.first, tag.second);

        if (parser.parse_file(filename)) {
            std::vector<std::string> symbols = split_clean(parser.get_value("PATH_SYMBOLS"));
            std::vector<std::string> values  = split_clean(parser.get_value("PATH_VALUES"));

            for (size_t i = 0; i < symbols.size() && i < values.size(); ++i)
                m_pathMap[symbols[i]] = values[i];
            /
            // CONTENT-BASED MAPPING: ensure key is the LAST part of the path
            for (const auto& symbol : symbols) {
                for (const auto& path : values) {
                    if (ends_with_component(path, symbol)) {
                        m_pathMap[symbol] = path;
                        break;
                    }
                }
            }
            
            m_kernels = split_clean(parser.get_value("KERNELS_TO_LOAD"));
            
        }*/
    }

    // ---------------------------------------------------------------
    // Result access
    // ---------------------------------------------------------------

    const std::map<std::string, std::string>& getPathMap() const { return m_pathMap; }
    const std::vector<std::string>&           getKernels() const { return m_kernels; }

    // ---------------------------------------------------------------
    // Serialization
    // ---------------------------------------------------------------

    /**
     * Serialize the parsed metakernel data back to the original
     * SPICE metakernel (.tm) format:
     *
     *   \begindata
     *     PATH_VALUES     = ( /kernels/gens,
     *                         /kernels/lro )
     *     PATH_SYMBOLS    = ( 'GENS',
     *                         'LRO' )
     *     KERNELS_TO_LOAD = ( '$GENS/spk/some.bsp',
     *                         '$LRO/ck/other.bc' )
     *   \begintext
     *
     * Symbols are quoted with single quotes as per SPICE convention.
     * Kernel paths are written one per line, comma-separated, with the
     * closing ')' on its own line.
     *
     * @returns A std::string containing the full metakernel representation.
     */
    std::string to_string() const {
        std::ostringstream oss;
        oss << "\\begindata\n\n";

        // PATH_VALUES
        oss << "  PATH_VALUES     = (\n";
        size_t pv_count = 0;
        for (const auto& [sym, path] : m_pathMap) {
            oss << "                      '" << path << "'";
            if (++pv_count < m_pathMap.size()) oss << ",";
            oss << "\n";
        }
        oss << "                    )\n\n";

        // PATH_SYMBOLS
        oss << "  PATH_SYMBOLS    = (\n";
        size_t ps_count = 0;
        for (const auto& [sym, path] : m_pathMap) {
            oss << "                      '" << sym << "'";
            if (++ps_count < m_pathMap.size()) oss << ",";
            oss << "\n";
        }
        oss << "                    )\n\n";

        // KERNELS_TO_LOAD
        oss << "  KERNELS_TO_LOAD = (\n";
        for (size_t i = 0; i < m_kernels.size(); ++i) {
            oss << "                      '" << m_kernels[i] << "'";
            if (i + 1 < m_kernels.size()) oss << ",";
            oss << "\n";
        }
        oss << "                    )\n\n";

        oss << "\\begintext\n";
        return oss.str();
    }

    /**
     * Write the metakernel serialization to a file.
     *
     * @param filename  Path to the output file (created or overwritten).
     * @throws std::runtime_error if the file cannot be opened for writing.
     */
    void to_file(const std::string& filename) const {
        std::ofstream ofs(filename);
        if (!ofs)
            throw std::runtime_error(
                "PsmrtsMetakernel::to_file: cannot open for writing: " + filename);
        ofs << to_string();
    }

private:

    // ---------------------------------------------------------------
    // Data
    // ---------------------------------------------------------------

    std::map<std::string, std::string> m_pathMap;  ///< symbol → path (e.g. "GENS" → "/kernels/gens")
    std::vector<std::string>           m_kernels;  ///< kernel paths in load order

    // ---------------------------------------------------------------
    // Helpers
    // ---------------------------------------------------------------

    /**
     * Check if the last path component of `path` matches `symbol`.
     * Trailing slashes are stripped before comparison.
     * 
     * **May be unnecessary, instead need to use positional matching**
     */
    bool ends_with_component(std::string path, const std::string& symbol) {
        if (!path.empty() && (path.back() == '/' || path.back() == '\\'))
            path.pop_back();

        size_t pos = path.find_last_of("/\\");
        std::string lastComponent = (pos == std::string::npos)
            ? path
            : path.substr(pos + 1);

        return lastComponent == symbol;
    }

    /**
     * Strip metakernel punctuation from a raw captured value string
     * and return the individual tokens.
     * Removes: ' " , ( ) =
     */
    std::vector<std::string> split_clean(std::string raw) {
        std::vector<std::string> results;
        for (char& c : raw) {
            if (c == '\'' || c == '\"' || c == ',' ||
                c == '('  || c == ')'  || c == '=')
                c = ' ';
        }
        std::stringstream ss(raw);
        std::string temp;
        while (ss >> temp)
            results.push_back(temp);
        return results;
    }
};

} // namespace psmrts

#endif // PSMRTS_METAKERNEL_HPP

/** 
#ifndef PSMRTS_METAKERNEL_HPP
#define PSMRTS_METAKERNEL_HPP

#include "PsmrtsTagSearch.hpp"
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>


// add a starttag: \begindata and endtag: EOF or begintext
// Add documentation for only reading in a single begindata to EOF or begintext
// GENS is equal to /kernels/gens, etc

// 
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
*/