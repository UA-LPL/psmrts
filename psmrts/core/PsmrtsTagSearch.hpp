#ifndef PSMRTS_TAG_SEARCH_HPP
#define PSMRTS_TAG_SEARCH_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <tuple>
#include <algorithm>
#include <sstream>
#include <cctype>

namespace psmrts {
/**
 * @brief Searches strings or files for tagged regions delimited by
 *        start/end token pairs, capturing the content between them.
 *
 * Internally maintains a list of start/end Tag pairs and scans input
 * line-by-line, accumulating text between each matched pair into a
 * ResultSet of (Tag, captured_content) tuples.
 *
 * Blank lines and comment lines beginning with '#' are ignored during
 * parsing. Matching is case-insensitive. Multiple tag pairs can be
 * registered and searched simultaneously; the earliest match on any
 * given line wins. A newline ("\n") may be used as an end tag to
 * capture single-line values.
 *
 * Note: This class was experimentally designed with AI assistance. For
 * more information, please refer to Anthropic's Claude AI model:
 *                        https://claude.ai/
 *
 * Quick-start:
 *
 *   // Construct with tag pairs up front
 *   PsmrtsTagSearch searcher({ {"Group", "EndGroup"}, {"Object", "End_Object"} });
 *
 *   // Or add tags incrementally
 *   PsmrtsTagSearch searcher;
 *   searcher.add_search_tag("Group",  "EndGroup");
 *   searcher.add_search_tag("Object", "End_Object");
 *
 *   // Parse a file or a string
 *   searcher.parse_file("IsisPreferences");
 *   searcher.parse_string("Group = Foo\n  Key = Value\nEndGroup");
 *
 *   // How many regions were captured?
 *   size_t n = searcher.size();
 *
 *   // Access a result by index — returns a (Tag, content) tuple
 *   const auto& result = searcher(0);
 *   std::string content = std::get<1>(result);
 *
 *   // Get all results that share a specific start tag
 *   auto groups = searcher.get_by_start_tag("Group");
 *   for (const auto& r : groups)
 *       std::cout << std::get<1>(r) << "\n";
 *
 *   // Check whether any result with a given start tag exists
 *   if (searcher.contains("Object"))
 *       std::cout << "Found at least one Object block\n";
 *
 *   // Retrieve the captured content of the first match for a start tag
 *   std::string body = searcher.get_value("Group");
 *
 *   // Use newline as end tag to capture single-line values
 *   searcher.add_search_tag("KeyName", "\n");
 *   searcher.parse_string("KeyName = SomeValue");
 *   std::string val = searcher.get_value("KeyName"); // "= SomeValue"
 *
 * @author  Kyle Becker, University of Arizona
 * @history 2026-03-14 Kyle Becker  Original Version
 */
class PsmrtsTagSearch {
public:
    using Tag = std::pair<std::string, std::string>;
    using SearchResult = std::tuple<Tag, std::string>;
    using ResultSet = std::vector<SearchResult>;

    PsmrtsTagSearch() = default;
    PsmrtsTagSearch(const std::vector<Tag>& tags) : m_tags(tags) {}

    void add_search_tag(const Tag& tag) { m_tags.push_back(tag); }
    void add_search_tag(const std::string& start, const std::string& end) { m_tags.push_back({start, end}); }

    size_t size() const { return m_search_sets.size(); }
    const SearchResult& operator()(size_t index) const { return m_search_sets.at(index); }
    const ResultSet& get_results() const { return m_search_sets; }

    // Search within a file
    inline bool parse_file(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) return false;
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        parse_string(buffer.str());
        return true;
    }

    // Search within a string
    inline void parse_string(const std::string& input) {
        std::istringstream stream(input);
        std::string line;
        
        bool in_capture = false;
        Tag active_tag;
        std::string sbuf;
        const std::string terminator = "\n";

        while (std::getline(stream, line)) {
            std::string processed_line = trim(line);

            // Ignore blank lines and comments
            if (processed_line.empty() || processed_line[0] == '#') {
                continue;
            }

            process_segment(processed_line + "\n", in_capture, active_tag, sbuf, terminator);
        }

        // Handle EOF without end tag: Save what we have
        if (in_capture) {
            m_search_sets.emplace_back(active_tag, sbuf);
        }
    }

    // Get all results matching a specific start tag
    ResultSet get_by_start_tag(const std::string& start_tag) const {
        ResultSet filtered;
        for (const auto& res : m_search_sets) {
            if (std::get<0>(res).first == start_tag) {
                filtered.push_back(res);
            }
        }
        return filtered;
    }

    // Returns true if the start tag exiss in the results
    bool contains(const std::string& start) const {
        for (const auto& result : m_search_sets) {
            if( std::get<0>(result).first == start ) {
                return true;
            }
        }
        return false;
    }

    std::string get_value(const std::string& start) const {
        for (const auto& result : m_search_sets) {
            if ( std::get<0>(result).first == start ) {
                return std::get<1>(result);
            }
        }
        return "";
    }



private:
    std::vector<Tag> m_tags;
    ResultSet m_search_sets;

    // Core logic to handle line segments recursively/iteratively
    void process_segment(std::string segment, bool& in_capture, Tag& active_tag, std::string& sbuf, const std::string& term) {
        if (segment.empty()) return;

        if (!in_capture) {
            // Looking for any start tag
            size_t best_pos = std::string::npos;
            Tag found_tag;

            for (const auto& tag : m_tags) {
                size_t pos = find_case_insensitive(segment, tag.first);
                if (pos != std::string::npos && (best_pos == std::string::npos || pos < best_pos)) {
                    best_pos = pos;
                    found_tag = tag;
                }
            }

            if (best_pos != std::string::npos) {
                in_capture = true;
                active_tag = found_tag;
                sbuf.clear();
                
                // Remainder of line after the start tag
                std::string remainder = trim(segment.substr(best_pos + found_tag.first.length()));
                process_segment(remainder, in_capture, active_tag, sbuf, term);
            }
        } else {
            // Looking for the specific end tag
            size_t end_pos = find_case_insensitive(segment, active_tag.second);

            if (active_tag.second == "\n") {
                // newline end tag — the newline appended in parse_string IS the terminator
                sbuf += segment;
                m_search_sets.emplace_back(active_tag, sbuf);
                in_capture = false;
                sbuf.clear();
            } else if (end_pos != std::string::npos) {
                // Append content up to end tag
                sbuf += segment.substr(0, end_pos);
                m_search_sets.emplace_back(active_tag, sbuf);
                in_capture = false;
                // Process the rest of the line for new start tags
                std::string remainder = segment.substr(end_pos + active_tag.second.length());
                process_segment(remainder, in_capture, active_tag, sbuf, term);
            } else {
                // End tag not found in this segment, accumulate and move to next line
                sbuf += segment + term;
            }
        }
    }

    // Helper: Case-insensitive find
    size_t find_case_insensitive(const std::string& haystack, const std::string& needle) {
        auto it = std::search(
            haystack.begin(), haystack.end(),
            needle.begin(), needle.end(),
            [](char ch1, char ch2) { return std::tolower(ch1) == std::tolower(ch2); }
        );
        return (it != haystack.end()) ? std::distance(haystack.begin(), it) : std::string::npos;
    }

    // Helper: Trim whitespace
    std::string trim(const std::string& s) {
        size_t first = s.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = s.find_last_not_of(" \t\r\n");
        return s.substr(first, (last - first + 1));
    }
};

} // namespace psmrts

#endif
