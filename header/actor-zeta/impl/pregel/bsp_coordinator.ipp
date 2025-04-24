#include <iostream>

#include "actor-zeta/pregel/bsp_coordinator.hpp"

namespace actor_zeta {
    namespace pregel {

        template<typename VertexID, typename VertexValue, typename EdgeValue, typename MessageType>
        bsp_coordinator<VertexID, VertexValue, EdgeValue, MessageType>::bsp_coordinator(actor_zeta::supervisor_abstract &supervisor)
            : basic_async_actor(supervisor, "bsp_coordinator"),
            active_vertices_(0),
            superstep_counter_(0) {
            std::cout << "bsp_coordinator created" << std::endl;
        }

        template<typename VertexID, typename VertexValue, typename EdgeValue, typename MessageType>
        bsp_coordinator<VertexID, VertexValue, EdgeValue, MessageType>::~bsp_coordinator() {
            std::cout << "bsp_coordinator destroyed" << std::endl;
        }

        template<typename VertexID, typename VertexValue, typename EdgeValue, typename MessageType>
        void bsp_coordinator<VertexID, VertexValue, EdgeValue, MessageType>::register_vertex(const actor_zeta::address_t& vertex_address) {
            std::cout << "bsp_coordinator: registering vertex " << vertex_address.get() << std::endl;
            vertex_addresses_.push_back(vertex_address);
            active_vertices_++;
        }

        template<typename VertexID, typename VertexValue, typename EdgeValue, typename MessageType>
        void bsp_coordinator<VertexID, VertexValue, EdgeValue, MessageType>::vote_to_halt() {
            active_vertices_--;
            std::cout << "bsp_coordinator: vertex voted to halt, active vertices = " << active_vertices_ << std::endl;
            if (active_vertices_ == 0) {
                std::cout << "bsp_coordinator: all vertices halted, starting next superstep" << std::endl;
                start_next_superstep();
            }
        }

        template<typename VertexID, typename VertexValue, typename EdgeValue, typename MessageType>
        void bsp_coordinator<VertexID, VertexValue, EdgeValue, MessageType>::start_next_superstep() {
            superstep_counter_++;
            std::cout << "bsp_coordinator: starting superstep " << superstep_counter_ << std::endl;
            active_vertices_ = vertex_addresses_.size();
            for (const auto& vertex_address : vertex_addresses_) {
                std::cout << "bsp_coordinator: sending compute message to vertex " << vertex_address.get() << std::endl;
                actor_zeta::send(address(), vertex_address, compute_message{});
            }
        }

        template<typename VertexID, typename VertexValue, typename EdgeValue, typename MessageType>
        std::uint64_t bsp_coordinator<VertexID, VertexValue, EdgeValue, MessageType>::superstep() const {
            return superstep_counter_;
        }

        template<typename VertexID, typename VertexValue, typename EdgeValue, typename MessageType>
        void bsp_coordinator<VertexID, VertexValue, EdgeValue, MessageType>::initialize() {
            // Добавление handler для регистрации вершин
            attach(
                actor_zeta::make_handler([this](const actor_zeta::address_t& address) { register_vertex(address); }),
                "register"
            );

            // Добавление handler для голосвания закончить процесс
            attach(
                actor_zeta::make_handler([this]() { vote_to_halt(); }),
                "vote_to_halt"
            );

            // Добавление handler для начала
            attach(
                actor_zeta::make_handler([this]() { start_next_superstep(); }),
                "start"
            );
        }

    } // namespace pregel
} // namespace actor_zeta
