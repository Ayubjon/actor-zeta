#include <iostream>

#include "actor-zeta/pregel/pregel_context.hpp"

namespace actor_zeta {
    namespace pregel {

        template<typename VertexID, typename VertexValue, typename EdgeValue, typename MessageType>
        pregel_context<VertexID, VertexValue, EdgeValue, MessageType>::pregel_context(pregel_actor<VertexID, VertexValue, EdgeValue, MessageType>* actor)
            : actor_(actor) {}

        template<typename VertexID, typename VertexValue, typename EdgeValue, typename MessageType>
        void pregel_context<VertexID, VertexValue, EdgeValue, MessageType>::send_message(const VertexID& target_vertex, const MessageType& message) {
            std::cout << "Vertex " << actor_->id() << ": Sending message to " << target_vertex << " with value " << message << std::endl;

            actor_zeta::send(coordinator_address_, actor_->address(),
                             pregel_message<VertexID, MessageType>{actor_->id(), target_vertex, message});
        }

        template<typename VertexID, typename VertexValue, typename EdgeValue, typename MessageType>
        void pregel_context<VertexID, VertexValue, EdgeValue, MessageType>::vote_to_halt() {
            std::cout << "Vertex " << actor_->id() << ": Voting to halt" << std::endl;
            actor_zeta::send(coordinator_address_, actor_->address(), vote_to_halt_message{});
        }

        template<typename VertexID, typename VertexValue, typename EdgeValue, typename MessageType>
        std::uint64_t pregel_context<VertexID, VertexValue, EdgeValue, MessageType>::superstep() const {
            return current_superstep_;
        }

        template<typename VertexID, typename VertexValue, typename EdgeValue, typename MessageType>
        void pregel_context<VertexID, VertexValue, EdgeValue, MessageType>::set_new_value(const VertexValue& new_value) {
            actor_->set_value(new_value);
        }

        template<typename VertexID, typename VertexValue, typename EdgeValue, typename MessageType>
        void pregel_context<VertexID, VertexValue, EdgeValue, MessageType>::set_coordinator_address(const actor_zeta::address_t& coordinator_address) {
            coordinator_address_ = coordinator_address;
        }

        template<typename VertexID, typename VertexValue, typename EdgeValue, typename MessageType>
        void pregel_context<VertexID, VertexValue, EdgeValue, MessageType>::set_current_superstep(std::uint64_t superstep) {
            current_superstep_ = superstep;
        }

    } // namespace pregel
} // namespace actor_zeta
