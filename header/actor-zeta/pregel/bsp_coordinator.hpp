#pragma once

#include <actor-zeta.hpp>
#include <actor-zeta/pregel/pregel_types.hpp>

#include <atomic>
#include <vector>
#include <iostream>

namespace actor_zeta {
    namespace pregel {

        template<typename VertexID, typename VertexValue, typename EdgeValue, typename MessageType>
        class bsp_coordinator : public basic_async_actor {
        public:
            using vertex_id_type = VertexID;
            using vertex_value_type = VertexValue;
            using edge_value_type = EdgeValue;
            using message_type = MessageType;

            bsp_coordinator(actor_zeta::supervisor_abstract &supervisor);

            ~bsp_coordinator() override;

            void register_vertex(const actor_zeta::address_t& vertex_address);

            void vote_to_halt();

            void start_next_superstep();

            std::uint64_t superstep() const;

        protected:
            void initialize() override;

        private:
            std::atomic<std::uint64_t> active_vertices_;
            std::vector<actor_zeta::address_t> vertex_addresses_;
            std::uint64_t superstep_counter_;

            // Типы сообщений для внутренней коммуникации
            struct compute_message final {};
        };

    } // namespace pregel
} // namespace actor_zeta

#include <actor-zeta/impl/pregel/bsp_coordinator.ipp>
