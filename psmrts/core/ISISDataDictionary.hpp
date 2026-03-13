#ifndef PSMRTS_ISIS_DATA_DICTIONARY_HPP
#define PSMRTS_ISIS_DATA_DICTIONARY_HPP

/**
 * @file ISISDataDictionary.hpp
 * @brief Parses an ISIS preferences file into named Group/Object blocks,
 *        each exposed as a key→value dictionary.
 *
 * Internally uses PsmrtsTagSearch to locate every Group…EndGroup and
 * Object…End_Object region, then further parses each captured body into
 * key = value pairs stored in a std::map (KVMap).
 *
 * Every key/value pair is also pushed into a PsmrtsTranslations instance
 * so that values containing $-variables (e.g. "$ISISDATA/lro") can be
 * resolved immediately via translations().translate_path().
 *
 * Quick-start:
 *
 *   ISISDataDictionary dict("IsisPreferences");
 *
 *   // Iterate every DataDirectory entry
 *   for (auto& [k, v] : dict.group("DataDirectory"))
 *       std::cout << k << " = " << v << "\n";
 *
 *   // Single-value lookup (both group name and key are case-insensitive)
 *   auto lro = dict.value("DataDirectory", "Lro"); // "$ISISDATA/lro"
 *
 *   // Resolve $-variables through the translation table
 *   std::string resolved = dict.translations().translate_path(*lro);
 *
 *   // With a fallback default
 *   std::string style = dict.value_or("UserInterface", "GuiStyle", "default");
 *
 *   // Serialize back to ISIS PVL format
 *   std::string pvl = dict.to_string();           // full ISIS PVL string
 *   std::string flat = dict.to_string_flat();     // simple Key = Value (no wrappers)
 *   dict.to_file("out.prefs");                    // write ISIS PVL to file
 *   dict.to_file_flat("out.flat");                // write flat format to file
 */

#include <algorithm>
#include <cctype>
#include <fstream>
#include <map>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "PsmrtsTagSearch.hpp"
#include "PsmrtsTranslations.hpp"

namespace psmrts {

class ISISDataDictionary {
public:

    // ---------------------------------------------------------------
    // Public types
    // ---------------------------------------------------------------

    /**
     * Case-insensitive map of key → value for one parsed block.
     * The comparator folds keys to lower-case at lookup time, matching
     * the behaviour of PsmrtsTranslations::ParameterInventory.
     */
    struct IcaseCompare {
        bool operator()(const std::string& a, const std::string& b) const {
            return std::lexicographical_compare(
                a.begin(), a.end(), b.begin(), b.end(),
                [](unsigned char x, unsigned char y) {
                    return std::tolower(x) < std::tolower(y);
                });
        }
    };
    using KVMap = std::map<std::string, std::string, IcaseCompare>;

    /** One parsed block: its group/object name + its key=value translation map. */
    struct Block {
        std::string group_name;   ///< e.g. "DataDirectory", "UserInterface"
        std::string block_type;   ///< "Group" or "Object" — preserved for round-trip serialization
        KVMap       entries;      ///< key → value, keys compared case-insensitively
    };

    // ---------------------------------------------------------------
    // Constructors
    // ---------------------------------------------------------------

    /**
     * Default constructor. An empty PsmrtsTranslations (no environment
     * variables loaded) is created internally. Call parse_file() or
     * parse_string() to populate it.
     */
    ISISDataDictionary() : m_translations("ISISDataDictionary") {}

    /**
     * Construct with an existing PsmrtsTranslations (e.g. one that already
     * has environment variables loaded via PsmrtsTranslations::create()).
     * Parsed key/value pairs are merged INTO that translations object so
     * existing $-variable definitions are available immediately.
     */
    explicit ISISDataDictionary(PsmrtsTranslations translations)
        : m_translations(std::move(translations)) {}

    /** Parse a file immediately on construction. */
    explicit ISISDataDictionary(const std::string& filename)
        : m_translations("ISISDataDictionary") {
        parse_file(filename);
    }

    /**
     * Parse a file and seed with an existing PsmrtsTranslations.
     * Useful when you want environment variables pre-loaded before
     * any $-references in the file are encountered.
     */
    ISISDataDictionary(const std::string& filename,
                       PsmrtsTranslations translations)
        : m_translations(std::move(translations)) {
        parse_file(filename);
    }

    // ---------------------------------------------------------------
    // Parsing
    // ---------------------------------------------------------------

    /**
     * Parse an ISIS preferences file.
     * May be called multiple times; results are appended.
     * @returns the number of Group/Object blocks found.
     * @throws std::runtime_error if the file cannot be opened.
     */
    size_t parse_file(const std::string& filename) {
        PsmrtsTagSearch ts( build_tags() );
        if (!ts.parse_file(filename))
            throw std::runtime_error(
                "ISISDataDictionary: cannot open file: " + filename);
        return ingest(ts);
    }

    /**
     * Parse from a raw string (e.g. already-read file contents).
     * @returns the number of Group/Object blocks found.
     */
    size_t parse_string(const std::string& content) {
        PsmrtsTagSearch ts( build_tags() );
        ts.parse_string(content);
        return ingest(ts);
    }

    // ---------------------------------------------------------------
    // Result access
    // ---------------------------------------------------------------

    /** Number of parsed blocks. */
    size_t size() const { return m_blocks.size(); }

    /** Access a block by index (0-based). */
    const Block& operator[](size_t index) const {
        return m_blocks.at(index);
    }

    /**
     * Return the KVMap for the FIRST block whose group name matches
     * (case-insensitive). Returns a reference to a static empty map
     * if not found, so range-for loops are always safe.
     */
    const KVMap& group(const std::string& name) const {
        static const KVMap empty;
        for (const auto& b : m_blocks)
            if (icase_equal(b.group_name, name))
                return b.entries;
        return empty;
    }

    /**
     * Return ALL blocks whose group name matches (case-insensitive).
     * Useful when the same group name appears more than once in a file.
     */
    std::vector<Block> groups(const std::string& name) const {
        std::vector<Block> out;
        for (const auto& b : m_blocks)
            if (icase_equal(b.group_name, name))
                out.push_back(b);
        return out;
    }

    /**
     * Look up a single value by group name + key (both case-insensitive).
     * Searches the FIRST matching block.
     * @returns std::nullopt if the group or key is absent.
     */
    std::optional<std::string> value(const std::string& group_name,
                                     const std::string& key) const {
        for (const auto& b : m_blocks) {
            if (!icase_equal(b.group_name, group_name)) continue;
            auto it = b.entries.find(key);   // IcaseCompare does the folding
            if (it != b.entries.end())
                return it->second;
        }
        return std::nullopt;
    }

    /**
     * Look up a single value with a fallback default.
     */
    std::string value_or(const std::string& group_name,
                         const std::string& key,
                         const std::string& default_val) const {
        return value(group_name, key).value_or(default_val);
    }

    /** Returns true if a block with the given group name exists. */
    bool has_group(const std::string& name) const {
        for (const auto& b : m_blocks)
            if (icase_equal(b.group_name, name))
                return true;
        return false;
    }

    /** All parsed blocks in file order. */
    const std::vector<Block>& all_blocks() const { return m_blocks; }

    /**
     * The translation table populated from all parsed key/value pairs.
     * Use it to resolve $-variable references in any value, e.g.:
     *
     *   auto raw = dict.value("DataDirectory", "Lro"); // "$ISISDATA/lro"
     *   std::string path = dict.translations().translate_path(*raw);
     */
    const PsmrtsTranslations& translations() const { return m_translations; }

    /** Non-const overload — allows callers to add_parameter() or merge(). */
    PsmrtsTranslations& translations() { return m_translations; }

    /** Clear all parsed blocks. The translation table is also cleared. */
    void clear() {
        m_blocks.clear();
        m_translations = PsmrtsTranslations("ISISDataDictionary");
    }

    // ---------------------------------------------------------------
    // Serialization
    // ---------------------------------------------------------------

    /**
     * Serialize all parsed blocks back to ISIS PVL format.
     *
     * Each block is emitted as:
     *
     *   Group = DataDirectory
     *     Lro  = $ISISDATA/lro
     *     Mro  = $ISISDATA/mro
     *   EndGroup
     *
     * Object blocks use "Object = Name" / "End_Object" accordingly.
     * Blocks are written in the order they were parsed (file order).
     *
     * @returns A std::string containing the full ISIS PVL representation.
     */
    std::string to_string() const {
        std::ostringstream oss;
        for (const auto& block : m_blocks) {
            const std::string& type = block.block_type.empty() ? "Group" : block.block_type;
            const std::string  end  = end_tag_for(type);

            oss << type << " = " << block.group_name << "\n";
            for (const auto& [k, v] : block.entries)
                oss << "  " << k << " = " << v << "\n";
            oss << end << "\n\n";
        }
        return oss.str();
    }

    /**
     * Serialize all parsed blocks to a flat "Key = Value" format,
     * with a comment header identifying each block.
     *
     * Example output:
     *
     *   # Group: DataDirectory
     *   Lro     = $ISISDATA/lro
     *   Mro     = $ISISDATA/mro
     *
     *   # Group: UserInterface
     *   GuiStyle = Default
     *
     * @returns A std::string containing the flat representation.
     */
    std::string to_string_flat() const {
        std::ostringstream oss;
        for (const auto& block : m_blocks) {
            const std::string& type = block.block_type.empty() ? "Group" : block.block_type;
            oss << "# " << type << ": " << block.group_name << "\n";
            for (const auto& [k, v] : block.entries)
                oss << k << " = " << v << "\n";
            oss << "\n";
        }
        return oss.str();
    }

    /**
     * Write the ISIS PVL serialization to a file.
     *
     * @param filename  Path to the output file (created or overwritten).
     * @throws std::runtime_error if the file cannot be opened for writing.
     */
    void to_file(const std::string& filename) const {
        std::ofstream ofs(filename);
        if (!ofs)
            throw std::runtime_error(
                "ISISDataDictionary::to_file: cannot open for writing: " + filename);
        ofs << to_string();
    }

    /**
     * Write the flat "Key = Value" serialization to a file.
     *
     * @param filename  Path to the output file (created or overwritten).
     * @throws std::runtime_error if the file cannot be opened for writing.
     */
    void to_file_flat(const std::string& filename) const {
        std::ofstream ofs(filename);
        if (!ofs)
            throw std::runtime_error(
                "ISISDataDictionary::to_file_flat: cannot open for writing: " + filename);
        ofs << to_string_flat();
    }

private:

    // ---------------------------------------------------------------
    // Tag configuration
    // ---------------------------------------------------------------

    /** The two ISIS tag pairs we register with PsmrtsTagSearch. */
    static std::vector<PsmrtsTagSearch::Tag> build_tags() {
        return {
            {"Group",  "EndGroup"},
            {"Object", "End_Object"}
        };
    }

    /**
     * Return the correct end tag for a given block type.
     * "Group" → "EndGroup", "Object" → "End_Object", anything else → "End"
     */
    static std::string end_tag_for(const std::string& type) {
        if (icase_equal(type, "Group"))  return "EndGroup";
        if (icase_equal(type, "Object")) return "End_Object";
        return "End";
    }

    // ---------------------------------------------------------------
    // Ingest PsmrtsTagSearch results → Block list
    // ---------------------------------------------------------------

    /**
     * Walk every SearchResult in `ts` and convert it to a Block.
     * PsmrtsTagSearch gives us:
     *   tag.first  = "Group" or "Object"
     *   body       = everything between the start and end tags,
     *                beginning with the remainder of the start-tag
     *                line (e.g. " = DataDirectory\nISIS3DATA = ...\n")
     *
     * Each parsed key/value pair is stored in Block::entries (KVMap)
     * AND pushed into m_translations via add_parameter() so that
     * $-variable references are immediately resolvable.
     *
     * block_type is recorded from tag.first so to_string() can emit
     * the correct Group/Object wrapper and matching end tag.
     *
     * @returns number of new blocks added.
     */
    size_t ingest(const PsmrtsTagSearch& ts) {
        const size_t before = m_blocks.size();

        for (size_t i = 0; i < ts.size(); ++i) {
            const auto& [tag, body] = ts(i);

            Block block;
            block.block_type = tag.first;                  // "Group" or "Object"
            block.group_name = extract_group_name(body);
            block.entries    = parse_kv_body(body);

            // Mirror every entry into the translation table
            for (const auto& [k, v] : block.entries)
                m_translations.add_parameter(k, v);

            m_blocks.push_back(std::move(block));
        }

        return m_blocks.size() - before;
    }

    // ---------------------------------------------------------------
    // Body parsing helpers
    // ---------------------------------------------------------------

    /**
     * Extract the group/object name from the captured body.
     *
     * PsmrtsTagSearch strips whitespace and skips blank/comment lines,
     * so `body` starts with the rest of the line that held the start
     * tag. For ISIS files that line looks like:
     *
     *   "= DataDirectory\n..."   (Group = DataDirectory)
     *   "= IsisCube\n..."        (Object = IsisCube)
     *
     * We take the value after the first '=' on the first non-blank line.
     */
    static std::string extract_group_name(const std::string& body) {
        std::istringstream ss(body);
        std::string line;
        while (std::getline(ss, line)) {
            trim(line);
            if (line.empty() || line[0] == '#') continue;

            size_t eq = line.find('=');
            if (eq != std::string::npos) {
                std::string name = line.substr(eq + 1);
                trim(name);
                if (!name.empty()) return name;
            } else if (!line.empty()) {
                // Name without '=' — unlikely in ISIS but handle gracefully
                return line;
            }
        }
        return "(unknown)";
    }

    /**
     * Parse key = value pairs from a captured body into a KVMap.
     *
     * Skips the first content line (the "= GroupName" line) and handles:
     *   - blank lines and '#' comments  → skipped
     *   - ISIS multi-line continuation  → trailing '-' means value
     *                                     continues on the next line
     *   - normal "Key = Value" lines    → inserted into the map
     *
     * If the same key appears more than once in a block, the last
     * value wins (consistent with std::map insert-or-assign behaviour).
     */
    static KVMap parse_kv_body(const std::string& body) {
        KVMap entries;
        std::istringstream ss(body);
        std::string line;
        bool first_content_line = true;

        std::string pending_key;
        std::string pending_val;
        bool        continuing = false;

        // Commit the in-progress key/value pair into the map
        auto flush = [&]() {
            if (!pending_key.empty()) {
                entries[pending_key] = pending_val;   // insert or overwrite
                pending_key.clear();
                pending_val.clear();
                continuing = false;
            }
        };

        while (std::getline(ss, line)) {
            trim(line);

            if (line.empty() || line[0] == '#') continue;

            // First non-blank, non-comment line is "= GroupName" — skip it
            if (first_content_line) {
                first_content_line = false;
                continue;
            }

            // A bare "-" is an ISIS continuation marker — skip the line itself
            if (line == "-") continue;

            // Detect and strip a trailing continuation dash
            bool is_continuation = (!line.empty() && line.back() == '-');
            if (is_continuation) {
                line.pop_back();
                trim(line);
            }

            size_t eq = line.find('=');
            if (eq != std::string::npos && !continuing) {
                // New key=value pair
                flush();

                pending_key = line.substr(0, eq);
                pending_val = line.substr(eq + 1);
                trim(pending_key);
                trim(pending_val);
            } else if (continuing || eq == std::string::npos) {
                // Continuation of the previous value (no '=' on this line)
                if (!pending_val.empty()) pending_val += ' ';
                pending_val += line;
            }

            continuing = is_continuation;

            // If this line is not a continuation, close the pair now
            if (!is_continuation) flush();
        }

        flush();  // catch any trailing unflushed entry
        return entries;
    }

    // ---------------------------------------------------------------
    // String helpers (duplicated locally so this header is self-contained)
    // ---------------------------------------------------------------

    static bool icase_equal(const std::string& a, const std::string& b) {
        if (a.size() != b.size()) return false;
        return std::equal(a.begin(), a.end(), b.begin(),
            [](unsigned char x, unsigned char y) {
                return std::tolower(x) == std::tolower(y);
            });
    }

    static void trim(std::string& s) {
        s.erase(s.begin(),
            std::find_if(s.begin(), s.end(),
                [](unsigned char c){ return !std::isspace(c); }));
        s.erase(
            std::find_if(s.rbegin(), s.rend(),
                [](unsigned char c){ return !std::isspace(c); }).base(),
            s.end());
    }

    // ---------------------------------------------------------------
    // Data
    // ---------------------------------------------------------------

    std::vector<Block>  m_blocks;
    PsmrtsTranslations  m_translations;  ///< flat key→value translation table
};

} // namespace psmrts

#endif // PSMRTS_ISIS_DATA_DICTIONARY_HPP



/** 
#ifndef PSMRTS_ISIS_DATA_DICTIONARY_HPP
#define PSMRTS_ISIS_DATA_DICTIONARY_HPP

/**
 * @file ISISDataDictionary.hpp
 * @brief Parses an ISIS preferences file into named Group/Object blocks,
 *        each exposed as a key→value dictionary.
 *
 * Internally uses PsmrtsTagSearch to locate every Group…EndGroup and
 * Object…End_Object region, then further parses each captured body into
 * key = value pairs stored in a std::map (KVMap).
 *
 * Every key/value pair is also pushed into a PsmrtsTranslations instance
 * so that values containing $-variables (e.g. "$ISISDATA/lro") can be
 * resolved immediately via translations().translate_path().
 *
 * Quick-start:
 *
 *   ISISDataDictionary dict("IsisPreferences");
 *
 *   // Iterate every DataDirectory entry
 *   for (auto& [k, v] : dict.group("DataDirectory"))
 *       std::cout << k << " = " << v << "\n";
 *
 *   // Single-value lookup (both group name and key are case-insensitive)
 *   auto lro = dict.value("DataDirectory", "Lro"); // "$ISISDATA/lro"
 *
 *   // Resolve $-variables through the translation table
 *   std::string resolved = dict.translations().translate_path(*lro);
 *
 *   // With a fallback default
 *   std::string style = dict.value_or("UserInterface", "GuiStyle", "default");
 *

#include <algorithm>
#include <cctype>
#include <map>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "PsmrtsTagSearch.hpp"
#include "PsmrtsTranslations.hpp"

namespace psmrts {

class ISISDataDictionary {
public:

    // ---------------------------------------------------------------
    // Public types
    // ---------------------------------------------------------------

    /**
     * Case-insensitive map of key → value for one parsed block.
     * The comparator folds keys to lower-case at lookup time, matching
     * the behaviour of PsmrtsTranslations::ParameterInventory.
     *
    struct IcaseCompare {
        bool operator()(const std::string& a, const std::string& b) const {
            return std::lexicographical_compare(
                a.begin(), a.end(), b.begin(), b.end(),
                [](unsigned char x, unsigned char y) {
                    return std::tolower(x) < std::tolower(y);
                });
        }
    };
    using KVMap = std::map<std::string, std::string, IcaseCompare>;

    /** One parsed block: its group/object name + its key=value translation map. *
    struct Block {
        std::string group_name;   ///< e.g. "DataDirectory", "UserInterface"
        KVMap       entries;      ///< key → value, keys compared case-insensitively
    };

    // ---------------------------------------------------------------
    // Constructors
    // ---------------------------------------------------------------

    /**
     * Default constructor. An empty PsmrtsTranslations (no environment
     * variables loaded) is created internally. Call parse_file() or
     * parse_string() to populate it.
     *
    ISISDataDictionary() : m_translations("ISISDataDictionary") {}

    /**
     * Construct with an existing PsmrtsTranslations (e.g. one that already
     * has environment variables loaded via PsmrtsTranslations::create()).
     * Parsed key/value pairs are merged INTO that translations object so
     * existing $-variable definitions are available immediately.
     *
    explicit ISISDataDictionary(PsmrtsTranslations translations)
        : m_translations(std::move(translations)) {}

    /** Parse a file immediately on construction. *
    explicit ISISDataDictionary(const std::string& filename)
        : m_translations("ISISDataDictionary") {
        parse_file(filename);
    }

    /**
     * Parse a file and seed with an existing PsmrtsTranslations.
     * Useful when you want environment variables pre-loaded before
     * any $-references in the file are encountered.
     *
    ISISDataDictionary(const std::string& filename,
                       PsmrtsTranslations translations)
        : m_translations(std::move(translations)) {
        parse_file(filename);
    }

    // ---------------------------------------------------------------
    // Parsing
    // ---------------------------------------------------------------

    /**
     * Parse an ISIS preferences file.
     * May be called multiple times; results are appended.
     * @returns the number of Group/Object blocks found.
     * @throws std::runtime_error if the file cannot be opened.
     *
    size_t parse_file(const std::string& filename) {
        PsmrtsTagSearch ts( build_tags() );
        if (!ts.parse_file(filename))
            throw std::runtime_error(
                "ISISDataDictionary: cannot open file: " + filename);
        return ingest(ts);
    }

    /**
     * Parse from a raw string (e.g. already-read file contents).
     * @returns the number of Group/Object blocks found.
     *
    size_t parse_string(const std::string& content) {
        PsmrtsTagSearch ts( build_tags() );
        ts.parse_string(content);
        return ingest(ts);
    }

    // ---------------------------------------------------------------
    // Result access
    // ---------------------------------------------------------------

    /** Number of parsed blocks. *
    size_t size() const { return m_blocks.size(); }

    /** Access a block by index (0-based). *
    const Block& operator[](size_t index) const {
        return m_blocks.at(index);
    }

    /**
     * Return the KVMap for the FIRST block whose group name matches
     * (case-insensitive). Returns a reference to a static empty map
     * if not found, so range-for loops are always safe.
     *
    const KVMap& group(const std::string& name) const {
        static const KVMap empty;
        for (const auto& b : m_blocks)
            if (icase_equal(b.group_name, name))
                return b.entries;
        return empty;
    }

    /**
     * Return ALL blocks whose group name matches (case-insensitive).
     * Useful when the same group name appears more than once in a file.
     *
    std::vector<Block> groups(const std::string& name) const {
        std::vector<Block> out;
        for (const auto& b : m_blocks)
            if (icase_equal(b.group_name, name))
                out.push_back(b);
        return out;
    }

    /**
     * Look up a single value by group name + key (both case-insensitive).
     * Searches the FIRST matching block.
     * @returns std::nullopt if the group or key is absent.
     *
    std::optional<std::string> value(const std::string& group_name,
                                     const std::string& key) const {
        for (const auto& b : m_blocks) {
            if (!icase_equal(b.group_name, group_name)) continue;
            auto it = b.entries.find(key);   // IcaseCompare does the folding
            if (it != b.entries.end())
                return it->second;
        }
        return std::nullopt;
    }

    /**
     * Look up a single value with a fallback default.
     *
    std::string value_or(const std::string& group_name,
                         const std::string& key,
                         const std::string& default_val) const {
        return value(group_name, key).value_or(default_val);
    }

    /** Returns true if a block with the given group name exists. *
    bool has_group(const std::string& name) const {
        for (const auto& b : m_blocks)
            if (icase_equal(b.group_name, name))
                return true;
        return false;
    }

    /** All parsed blocks in file order. *
    const std::vector<Block>& all_blocks() const { return m_blocks; }

    /**
     * The translation table populated from all parsed key/value pairs.
     * Use it to resolve $-variable references in any value, e.g.:
     *
     *   auto raw = dict.value("DataDirectory", "Lro"); // "$ISISDATA/lro"
     *   std::string path = dict.translations().translate_path(*raw);
     *
    const PsmrtsTranslations& translations() const { return m_translations; }

    /** Non-const overload — allows callers to add_parameter() or merge(). *
    PsmrtsTranslations& translations() { return m_translations; }

    /** Clear all parsed blocks. The translation table is also cleared. *
    void clear() {
        m_blocks.clear();
        m_translations = PsmrtsTranslations("ISISDataDictionary");
    }

private:

    // ---------------------------------------------------------------
    // Tag configuration
    // ---------------------------------------------------------------

    /** The two ISIS tag pairs we register with PsmrtsTagSearch. *
    static std::vector<PsmrtsTagSearch::Tag> build_tags() {
        return {
            {"Group",  "EndGroup"},
            {"Object", "End_Object"}
        };
    }

    // ---------------------------------------------------------------
    // Ingest PsmrtsTagSearch results → Block list
    // ---------------------------------------------------------------

    /**
     * Walk every SearchResult in `ts` and convert it to a Block.
     * PsmrtsTagSearch gives us:
     *   tag.first  = "Group" or "Object"
     *   body       = everything between the start and end tags,
     *                beginning with the remainder of the start-tag
     *                line (e.g. " = DataDirectory\nISIS3DATA = ...\n")
     *
     * Each parsed key/value pair is stored in Block::entries (KVMap)
     * AND pushed into m_translations via add_parameter() so that
     * $-variable references are immediately resolvable.
     *
     * @returns number of new blocks added.
     *
    size_t ingest(const PsmrtsTagSearch& ts) {
        const size_t before = m_blocks.size();

        for (size_t i = 0; i < ts.size(); ++i) {
            const auto& [tag, body] = ts(i);

            Block block;
            block.group_name = extract_group_name(body);
            block.entries    = parse_kv_body(body);

            // Mirror every entry into the translation table
            for (const auto& [k, v] : block.entries)
                m_translations.add_parameter(k, v);

            m_blocks.push_back(std::move(block));
        }

        return m_blocks.size() - before;
    }

    // ---------------------------------------------------------------
    // Body parsing helpers
    // ---------------------------------------------------------------

    /**
     * Extract the group/object name from the captured body.
     *
     * PsmrtsTagSearch strips whitespace and skips blank/comment lines,
     * so `body` starts with the rest of the line that held the start
     * tag. For ISIS files that line looks like:
     *
     *   "= DataDirectory\n..."   (Group = DataDirectory)
     *   "= IsisCube\n..."        (Object = IsisCube)
     *
     * We take the value after the first '=' on the first non-blank line.
     *
    static std::string extract_group_name(const std::string& body) {
        std::istringstream ss(body);
        std::string line;
        while (std::getline(ss, line)) {
            trim(line);
            if (line.empty() || line[0] == '#') continue;

            size_t eq = line.find('=');
            if (eq != std::string::npos) {
                std::string name = line.substr(eq + 1);
                trim(name);
                if (!name.empty()) return name;
            } else if (!line.empty()) {
                // Name without '=' — unlikely in ISIS but handle gracefully
                return line;
            }
        }
        return "(unknown)";
    }

    /**
     * Parse key = value pairs from a captured body into a KVMap.
     *
     * Skips the first content line (the "= GroupName" line) and handles:
     *   - blank lines and '#' comments  → skipped
     *   - ISIS multi-line continuation  → trailing '-' means value
     *                                     continues on the next line
     *   - normal "Key = Value" lines    → inserted into the map
     *
     * If the same key appears more than once in a block, the last
     * value wins (consistent with std::map insert-or-assign behaviour).
     *
    static KVMap parse_kv_body(const std::string& body) {
        KVMap entries;
        std::istringstream ss(body);
        std::string line;
        bool first_content_line = true;

        std::string pending_key;
        std::string pending_val;
        bool        continuing = false;

        // Commit the in-progress key/value pair into the map
        auto flush = [&]() {
            if (!pending_key.empty()) {
                entries[pending_key] = pending_val;   // insert or overwrite
                pending_key.clear();
                pending_val.clear();
                continuing = false;
            }
        };

        while (std::getline(ss, line)) {
            trim(line);

            if (line.empty() || line[0] == '#') continue;

            // First non-blank, non-comment line is "= GroupName" — skip it
            if (first_content_line) {
                first_content_line = false;
                continue;
            }

            // A bare "-" is an ISIS continuation marker — skip the line itself
            if (line == "-") continue;

            // Detect and strip a trailing continuation dash
            bool is_continuation = (!line.empty() && line.back() == '-');
            if (is_continuation) {
                line.pop_back();
                trim(line);
            }

            size_t eq = line.find('=');
            if (eq != std::string::npos && !continuing) {
                // New key=value pair
                flush();

                pending_key = line.substr(0, eq);
                pending_val = line.substr(eq + 1);
                trim(pending_key);
                trim(pending_val);
            } else if (continuing || eq == std::string::npos) {
                // Continuation of the previous value (no '=' on this line)
                if (!pending_val.empty()) pending_val += ' ';
                pending_val += line;
            }

            continuing = is_continuation;

            // If this line is not a continuation, close the pair now
            if (!is_continuation) flush();
        }

        flush();  // catch any trailing unflushed entry
        return entries;
    }

    // ---------------------------------------------------------------
    // String helpers (duplicated locally so this header is self-contained)
    // ---------------------------------------------------------------

    static bool icase_equal(const std::string& a, const std::string& b) {
        if (a.size() != b.size()) return false;
        return std::equal(a.begin(), a.end(), b.begin(),
            [](unsigned char x, unsigned char y) {
                return std::tolower(x) == std::tolower(y);
            });
    }

    static void trim(std::string& s) {
        s.erase(s.begin(),
            std::find_if(s.begin(), s.end(),
                [](unsigned char c){ return !std::isspace(c); }));
        s.erase(
            std::find_if(s.rbegin(), s.rend(),
                [](unsigned char c){ return !std::isspace(c); }).base(),
            s.end());
    }

    // ---------------------------------------------------------------
    // Data
    // ---------------------------------------------------------------

    std::vector<Block>  m_blocks;
    PsmrtsTranslations  m_translations;  ///< flat key→value translation table
};

} // namespace psmrts

#endif // PSMRTS_ISIS_DATA_DICTIONARY_HPP
*/