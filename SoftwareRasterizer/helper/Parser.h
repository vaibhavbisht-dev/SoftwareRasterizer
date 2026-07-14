#pragma once
#include <vector>
#include "../DataStructures.h"
#include <string>
#include <fstream>
#include <map>

struct VertexIndexKey {
    int v_idx, vt_idx, vn_idx; // Stores the separate raw OBJ indices for a single vertex corner

    // Less-than operator overload so this custom struct can be safely sorted and used as a std::map key
    bool operator<(const VertexIndexKey& other) const {
        if (v_idx != other.v_idx) return v_idx < other.v_idx; // First, compare geometric position indices
        if (vt_idx != other.vt_idx) return vt_idx < other.vt_idx; // If positions match, compare texture coordinate indices
        return vn_idx < other.vn_idx; // If both match, use the normal index as the final tie-breaker
    }
};

class OBJParser
{
public:
    // Accept std::string or std::string_view for safety
    explicit OBJParser(const std::string& fileLocation);

    // Prevent accidental copying of file resources
    OBJParser(const OBJParser&) = delete;
    OBJParser& operator=(const OBJParser&) = delete;

    // Default destructor handles closing m_file automatically
    ~OBJParser() = default;

    ObjectData ParseOBJ();



private:

	std::ifstream m_file;
	std::string m_path;

};