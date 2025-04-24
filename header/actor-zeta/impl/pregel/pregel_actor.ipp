#include <iostream>
#include <actor-zeta/scheduler/scheduler.hpp> // Include for task creation

#include "actor-zeta/pregel/pregel_actor.hpp"

namespace actor_zeta {
    namespace pregel {

        template<typename VertexID, typename VertexValue, typename EdgeValue, typename MessageType>
        pregel_actor<VertexID, VertexValue, EdgeValue, MessageType>::pregel_actor(actor_zeta::supervisor_abstract &supervisor,
                                                                                  const VertexID& vertex_id,
                                                                                  const VertexValue& initial_value)
            : basic_async_actor(supervisor, "pregel_vertex_" + std::to_string(static_cast<uint64_t>(vertex_id))),
            vertex_id_(vertex_id),
            vertex_value_(initial_value),
            context_(this) {
            std::cout << "pregel_actor created with id: " << vertex_id_ << std::endl;
        }

        template<typename VertexID, typename VertexValue, typename EdgeValue, typename MessageType>
        pregel_actor<VertexID, VertexValue, EdgeValue, MessageType>::~pregel_actor() {
            std::cout << "pregel_actor destroyed with id: " << vertex_id_ << std::endl;
        }

        template<typename VertexID, typename VertexValue, typename EdgeValue, typename MessageType>
        void pregel_actor<VertexID, VertexValue, EdgeValue, MessageType>::set_value(const VertexValue& new_value) {
            std::cout << "Vertex " << vertex_id_ << ": Value updated from " << vertex_value_ << " to " << new_value << std::endl;
            vertex_value_ = new_value;
        }

        template<typename VertexID, typename VertexValue, typename EdgeValue, typename MessageType>
        void pregel_actor<VertexID, VertexValue, EdgeValue, MessageType>::add_edge(const VertexID& target_vertex, const EdgeValue& edge_value) {
            std::cout << "Vertex " << vertex_id_ << ": Added edge to " << target_vertex << " with value " << edge_value << std::endl;
            edges_[target_vertex] = edge_value;
        }

        template<typename VertexID, typename VertexValue, typename EdgeValue, typename MessageType>
        void pregel_actor<VertexID, VertexValue, EdgeValue, MessageType>::pregel_init() {
            std::cout << "Vertex " << vertex_id_ << ": pregel_init called" << std::endl;
        }

        template<typename VertexID, typename VertexValue, typename EdgeValue, typename MessageType>
        void pregel_actor<VertexID, VertexValue, EdgeValue, MessageType>::pregel_finalize() {
            std::cout << "Vertex " << vertex_id_ << ": pregel_finalize called" << std::endl;
        }

    } // namespace pregel
} // namespace actor_zeta
