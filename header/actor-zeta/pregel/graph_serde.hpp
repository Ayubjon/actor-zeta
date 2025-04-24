#pragma once

#include <string>
#include <map>

namespace actor_zeta {
    namespace pregel {

        template<typename VertexID, typename VertexValue, typename EdgeValue>
        class graph_serde {
        public:
            using vertex_id_type = VertexID;
            using vertex_value_type = VertexValue;
            using edge_value_type = EdgeValue;

            // Сериализация графа в строку
            static std::string serialize(const std::map<VertexID, VertexValue>& vertices,
                                         const std::map<VertexID, std::map<VertexID, EdgeValue>>& edges);

            // Десериализация графа из строки
            static bool deserialize(const std::string& data,
                                    std::map<VertexID, VertexValue>& vertices,
                                    std::map<VertexID, std::map<VertexID, EdgeValue>>& edges);
        };

    } // namespace pregel
} // namespace actor_zeta
