#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include "actor-zeta/pregel/graph_serde.hpp"

namespace actor_zeta {
    namespace pregel {

        template<typename VertexID, typename VertexValue, typename EdgeValue>
        std::string graph_serde<VertexID, VertexValue, EdgeValue>::serialize(const std::map<VertexID, VertexValue>& vertices,
                                                                             const std::map<VertexID, std::map<VertexID, EdgeValue>>& edges) {
            std::stringstream ss;

            // Сериализация вершин
            ss << "Vertices: " << vertices.size() << "\n";
            for (const auto& vertex : vertices) {
                ss << vertex.first << " " << vertex.second << "\n";
            }

            // Сериализация ребер
            ss << "Edges: " << edges.size() << "\n";
            for (const auto& edge_source : edges) {
                for (const auto& edge_target : edge_source.second) {
                    ss << edge_source.first << " " << edge_target.first << " " << edge_target.second << "\n";
                }
            }

            return ss.str();
        }

        template<typename VertexID, typename VertexValue, typename EdgeValue>
        bool graph_serde<VertexID, VertexValue, EdgeValue>::deserialize(const std::string& data,
                                                                        std::map<VertexID, VertexValue>& vertices,
                                                                        std::map< VertexID, std::map<VertexID, EdgeValue> >& edges) {
            std::stringstream ss(data);
            std::string line;

            try {
                // Десериализация вершин
                std::getline(ss, line); // "Vertices: X"
                size_t num_vertices = std::stoul(line.substr(line.find(":") + 2));
                for (size_t i = 0; i < num_vertices; ++i) {
                    VertexID id;
                    VertexValue value;
                    ss >> id >> value;
                    vertices[id] = value;
                    ss.ignore(); // Пропускаем символ новой строки
                }

                // Десериализация ребер
                std::getline(ss, line); // "Edges: Y"
                size_t num_edges = std::stoul(line.substr(line.find(":") + 2));
                for (size_t i = 0; i < num_edges; ++i) {
                    VertexID source, target;
                    EdgeValue value;
                    ss >> source >> target >> value;
                    edges[source][target] = value;
                    ss.ignore(); // Пропускаем символ новой строки
                }
            } catch (const std::exception& e) {
                std::cerr << "Error during deserialization: " << e.what() << std::endl;
                return false; // Indicate failure
            }

            return true; // Indicate success
        }

    } // namespace pregel
} // namespace actor_zeta
