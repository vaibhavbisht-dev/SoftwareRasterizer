#include "Parser.h"
#include <stdexcept>
#include <sstream>
#include <map>

OBJParser::OBJParser(const std::string& fileLocation) : m_path(fileLocation) {
    m_file.open(fileLocation);
    if (!m_file.is_open()) {
        throw std::runtime_error("Failed to open OBJ file: " + fileLocation);
    }
}

ObjectData OBJParser::ParseOBJ() {
    // Ensure file is still good and reset to beginning if read twice
    if (!m_file.good()) {
        m_file.clear();
        m_file.seekg(0, std::ios::beg);
    }
    // Temporary storage for raw OBJ data streams
    std::vector<Vector3<float>> raw_positions;
    std::vector<Vector2<float>> raw_tex_coords;
    std::vector<Vector3<float>> raw_normals;

    // Final output containers
    std::vector<Vertex> final_vertices;
    std::vector<Triangle> final_triangles;

    // Map to deduplicate complex unique combinations into unique vertex indices
    std::map<VertexIndexKey, int> vertex_cache;

    std::string line;
    
    while (std::getline(m_file, line)) {
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        // skip comments or empty prefixes
		if (prefix.empty() || prefix[0] == '#') continue;

        // parse raw position
        if (prefix == "v") {
			float x, y, z;
            if (iss >> x >> y >> z) {
				raw_positions.push_back(Vector3<float>(x, y, z));
            }
        }
        else if (prefix == "vt") {
			float u, v;
            if (iss >> u >> v) {
				raw_tex_coords.push_back(Vector2<float>(u, v));
            }
        }
        else if (prefix == "vn") {
			float x, y, z;
            if (iss >> x >> y >> z) {
				raw_normals.push_back(Vector3<float>(x, y, z));
            }
        }

        // 4. Parse Face structures (handles v, v/vt, v//vn, v/vt/vn)
        else if (prefix == "f") {
            std::vector<int> face_vertex_indices;
            std::string vertex_token;

            while (iss >> vertex_token) {
                // Initialize default OBJ indices (0 means omitted)
                int v_idx = 0, vt_idx = 0, vn_idx = 0;

                std::replace(vertex_token.begin(), vertex_token.end(), '/', ' ');
                std::istringstream token_stream(vertex_token);

                token_stream >> v_idx;
                if (vertex_token.find("  ") != std::string::npos) {
                    // Handing double slash formatting: v//vn
                    token_stream >> vn_idx;
                }
                else {
                    // Handling standard formatting: v/vt or v/vt/vn
                    if (token_stream >> vt_idx) {
                        token_stream >> vn_idx;
                    }
                }

                // Wavefront OBJ elements are 1-indexed. Convert to 0-indexed.
                // Supports negative relative indexing if present in file.
                v_idx = (v_idx > 0) ? v_idx - 1 : (v_idx < 0) ? static_cast<int>(raw_positions.size()) + v_idx : 0;
                vt_idx = (vt_idx > 0) ? vt_idx - 1 : (vt_idx < 0) ? static_cast<int>(raw_tex_coords.size()) + vt_idx : 0;
                vn_idx = (vn_idx > 0) ? vn_idx - 1 : (vn_idx < 0) ? static_cast<int>(raw_normals.size()) + vn_idx : 0;

                VertexIndexKey key{ v_idx, vt_idx, vn_idx };

                // Unique combination tracking
                auto it = vertex_cache.find(key);
                if (it == vertex_cache.end()) {
                    // Assemble a fresh unified vertex layout
                    Vertex new_vertex;

                    if (v_idx < raw_positions.size())  new_vertex.position = raw_positions[v_idx];
                    if (vt_idx < raw_tex_coords.size()) new_vertex.uv = raw_tex_coords[vt_idx];
                    if (vn_idx < raw_normals.size())    new_vertex.normal = raw_normals[vn_idx];

                    int new_index = static_cast<int>(final_vertices.size());
                    final_vertices.push_back(new_vertex);
                    vertex_cache[key] = new_index;
                    face_vertex_indices.push_back(new_index);
                }
                else {
                    // Reuse existing index if already resolved
                    face_vertex_indices.push_back(it->second);
                }
            }

            // Fan triangulation for polygons with > 3 vertices (N-gons)
            for (size_t i = 1; i < face_vertex_indices.size() - 1; ++i) {
                Triangle tri;
                tri.v0 = face_vertex_indices[0];
                tri.v1 = face_vertex_indices[i];
                tri.v2 = face_vertex_indices[i + 1];
                tri.color = 0xFFFFFFFF; // Pure white texture fallback
                final_triangles.push_back(tri);
            }
        }
    }

    ObjectData data;
    data.vertices = std::move(final_vertices);
    data.triangles = std::move(final_triangles);
    return data;
}
