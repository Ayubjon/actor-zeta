#pragma once

#include <actor-zeta.hpp>
#include <actor-zeta/pregel/pregel_types.hpp>
#include <map>

namespace actor_zeta {
    namespace pregel {

        template<typename VertexID, typename VertexValue, typename EdgeValue, typename MessageType>
        class pregel_actor : public basic_async_actor {
        public:
            using vertex_id_type = VertexID;
            using vertex_value_type = VertexValue;
            using edge_value_type = EdgeValue;
            using message_type = MessageType;

            pregel_actor(actor_zeta::supervisor_abstract &supervisor,
                         const VertexID& vertex_id,
                         const VertexValue& initial_value);

            ~pregel_actor() override;

            VertexValue value() const { return vertex_value_; }

            VertexID id() const { return vertex_id_; }

            void set_value(const VertexValue& new_value);

            void add_edge(const VertexID& target_vertex, const EdgeValue& edge_value);

            const std::map<VertexID, EdgeValue>& get_edges() const { return edges_; }

            pregel_context<VertexID, VertexValue, EdgeValue, MessageType>& get_context() { return context_; }

            virtual void compute(actor_zeta::message_ptr msg) = 0;

            virtual void pregel_init();

            virtual void pregel_finalize();

        protected:
            VertexID vertex_id_;
            VertexValue vertex_value_;
            std::map<VertexID, EdgeValue> edges_;
            pregel_context<VertexID, VertexValue, EdgeValue, MessageType> context_;
        };

    } // namespace pregel
} // namespace actor_zeta
