/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef PSMRTS_METAKERNEL_HPP
#define PSMRTS_METAKERNEL_HPP

#include "PsmrtsTagSearch.hpp"
#include "PsmrtsUtilities.hpp"
#include <chrono>
#include <ctime>
#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>

namespace psmrts {

  /**
   * @brief PSMRTS Metakernel file extractor 
   * 
   * This class parses SPICE metakernel (.tm) type files and extracts values
   * from PATH_VALUES, PATH_SYMBOLS, and KERNELS_TO_LOAD. It reads a single
   * \begindata block up to EOF or \begintext. It is not designed to read
   * more than one block, and files containing multiple will need to be split
   * into separate files.
   * 
   * Note: This class was experimentally designed with AI assistance. For 
   * more information, please refer to Anthropic's Claude AI model:
   *                        https://claude.ai/
   * 
   * Quick-start:
   *
   * PsmrtsMetakernel mk("my_mission.tm");
   *
   * // Inspect the symbol→path map
   * for (auto& [sym, path] : mk.getPathMap())
   *     std::cout << sym << " = " << path << "\n";
   *
   * // Get all kernel paths
   * for (auto& k : mk.getKernels())
   *      std::cout << k << "\n";
   *
   * // Serialize back to metakernel format
   * std::string tm = mk.to_string();
   * mk.to_file("output.tm");
   * 
   * 
   * @author Kyle Becker, University of Arizona
   * @history 2026-03-14 Kyle Becker  Original Version
   */
class PsmrtsMetakernel {
public:
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

    PsmrtsMetakernel() {
        m_tags = {
            { "PATH_VALUES",     ")" },
            { "PATH_SYMBOLS",    ")" },
            { "KERNELS_TO_LOAD", ")" }
        };
    }

    PsmrtsMetakernel(const std::string& filename) {
        m_tags = {
            { "PATH_VALUES",     ")" },
            { "PATH_SYMBOLS",    ")" },
            { "KERNELS_TO_LOAD", ")" }
        };
        load_file(filename);
    }

    /**
     * @brief Populates the PsmrtsMetakernel from a file.
     * 
     * Can be called on a default-constructed instance, or to reload
     * an existing one. Clears any previously loaded data before parsing.
     *
     * Only a single \begindata ... \begintext section is supported.
     * See above class description.
     *
     * @param filename  Path to the metakernel (.tm) file to parse.
     * @throws std::runtime_error if the file cannot be opened, or if
     *         multiple \begindata or \begintext sections are detected.
     */
    void load_file(const std::string& filename) {
        // Clear any previously loaded state
        m_pathMap.clear();
        m_kernels.clear();
        m_source.clear();

        std::ifstream ifs(filename);
        if (!ifs)
            throw std::runtime_error(
                "PsmrtsMetakernel: cannot open file: " + filename);

        std::string content((std::istreambuf_iterator<char>(ifs)),
                            std::istreambuf_iterator<char>());

        // Warn if multiple \begindata or \begintext sections are detected
        size_t first_begin  = content.find("\\begindata");
        size_t second_begin = content.find("\\begindata", first_begin + 1);
        if (second_begin != std::string::npos)
            throw std::runtime_error(
                "PsmrtsMetakernel: multiple \\begindata sections detected in: "
                + filename + ". Only a single \\begindata block is supported.");

        size_t first_text  = content.find("\\begintext");
        size_t second_text = content.find("\\begintext", first_text + 1);
        if (second_text != std::string::npos)
            throw std::runtime_error(
                "PsmrtsMetakernel: multiple \\begintext sections detected in: "
                + filename + ". Only a single \\begintext block is supported.");

        // Locate \begindata — everything before it is the comment header
        size_t begin_pos = first_begin;
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
        m_source  = filename;
    }

    /** Return SYMBOL to VALUE map pairings */
    const std::map<std::string, std::string>& getPathMap() const { 
        return m_pathMap;
    }

    /** Return KERNELS list */
    const std::vector<std::string>& getKernels() const { 
        return m_kernels; 
    }

    /** Return source file / path */
    const std::string& getSource() const { 
        return m_source; 
    }


    /**
     * @brief String conversion
     * 
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

        oss << "KPL/MK\n\n";
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
        // Add timestamp and source to text section
        std::time_t now = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
        char timebuf[64];
        std::strftime( timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", std::localtime(&now) );
        oss << "Metakernel file generated by PSMRTS: \n";
        oss << timebuf << "\n";
        if (!m_source.empty()) {
            oss << "Source: " << m_source << "\n";
        }

        return oss.str();
    }

    /**
     * @brief Write the metakernel serialization to a file.
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
    std::map<std::string, std::string> m_pathMap;  // symbol -> path (e.g. "GENS" → "/kernels/gens")
    std::vector<std::string>           m_kernels;  // kernel paths in load order
    std::string                        m_source;


    /**
     * @brief Trim function
     * 
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

