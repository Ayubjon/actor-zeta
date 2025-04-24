#pragma once

#include <actor-zeta.hpp>
#include <actor-zeta/pregel/pregel_types.hpp>

namespace actor_zeta {
    namespace pregel {

        template<typename VertexID, typename VertexValue, typename EdgeValue, typename MessageType>
        class pregel_context {
        public:
            using vertex_id_type = VertexID;
            using vertex_value_type = VertexValue;
            using edge_value_type = EdgeValue;
            using message_type = MessageType;

            pregel_context(pregel_actor<VertexID, VertexValue, EdgeValue, MessageType>* actor);

            void send_message(const VertexID& target_vertex, const MessageType& message);

            void vote_to_halt();

            std::uint64_t superstep() const;

            void set_new_value(const VertexValue& new_value);

            void set_coordinator_address(const actor_zeta::address_t& coordinator_address);

            void set_current_superstep(std::uint64_t superstep);

        private:
            pregel_actor<VertexID, VertexValue, EdgeValue, MessageType>* actor_;
            actor_zeta::address_t coordinator_address_;
            std::uint64_t current_superstep_{0};
        };

    } // namespace pregel
} // namespace actor_zeta

#include <actor-zeta/impl/pregel/pregel_context.ipp>
