#include <iostream>
#include <limits>

#include <actor-zeta.hpp>
#include <actor-zeta/pregel/pregel_actor.hpp>
#include <actor-zeta/pregel/bsp_coordinator.hpp>
#include <actor-zeta/pregel/message_combiner.hpp>

#include <gtest/gtest.h>

using namespace actor_zeta;
using namespace actor_zeta::pregel;

// Типы данных для теста
using vertex_id_type = std::uint64_t;
using vertex_value_type = double;
using edge_value_type = double;
using message_type = double;

// Простой vertex для тестирования
class test_vertex : public pregel_actor<vertex_id_type, vertex_value_type, edge_value_type, message_type> {
public:
    test_vertex(actor_zeta::supervisor_abstract &supervisor,
                const vertex_id_type& vertex_id,
                const vertex_value_type& initial_value)
        : pregel_actor(supervisor, vertex_id, initial_value),
        messages_received_(0),
        total_received_(0.0) {}

    void compute(actor_zeta::message_ptr msg) override {
        msg->unpack([&](double value) {
            total_received_ += value;
            messages_received_++;
        });

        if (get_context().superstep() < 3) {
            // Отправляем сообщение всем соседям
            const auto& edges = get_edges();
            for (const auto& edge : edges) {
                get_context().send_message(edge.first, value() + 1.0);
            }
            get_context().vote_to_halt();
        } else {
            get_context().vote_to_halt();
        }
    }

    size_t messages_received() const {
        return messages_received_;
    }

    double total_received() const {
        return total_received_;
    }

private:
    size_t messages_received_;
    double total_received_;
};

TEST(PregelTest, BasicTest) {
    actor_zeta::environment_t env;
    actor_zeta::supervisor_t supervisor(env);

    // Создание графа
    std::map<vertex_id_type, vertex_value_type> vertices = {
        {1, 1.0},
        {2, 2.0}
    };

    std::map<vertex_id_type, std::map<vertex_id_type, edge_value_type>> edges = {
        {1, {{2, 1.0}}},
        {2, {{1, 1.0}}}
    };

    // Создание координатора
    auto coordinator = supervisor.make_actor<bsp_coordinator<vertex_id_type, vertex_value_type, edge_value_type, message_type>>();
    auto coordinator_addr = coordinator->address();

    // Создание вершин
    std::vector<actor_zeta::address_t> vertex_actors;
    std::vector<test_vertex*> vertex_pointers;
    for (const auto& vertex : vertices) {
        auto vertex_actor = supervisor.make_actor<test_vertex>(vertex.first, vertex.second);
        vertex_actors.push_back(vertex_actor->address());
        vertex_pointers.push_back(vertex_actor.get());

        // Регистрация вершины у координатора
        actor_zeta::send(coordinator_addr, vertex_actor->address(), "register");

        // Добавление ребер
        for (const auto& edge : edges[vertex.first]) {
            vertex_actor->add_edge(edge.first, edge.second);
        }
    }

    // Установка адреса координатора в контексте каждой вершины
    for (auto vertex : vertex_pointers) {
        vertex->get_context().set_coordinator_address(coordinator_addr);
    }

    // Запуск Pregel
    actor_zeta::send(coordinator_addr, coordinator_addr, "start");

    // Ожидание завершения
    env.await_actors_completion();

    // Проверка результатов
    for (const auto& addr : vertex_actors) {
        auto vertex = actor_zeta::actor_cast<test_vertex>(addr);
        std::cout << "Vertex " << vertex->id() << ": Total Received = " << vertex->total_received() << std::endl;
    }

    // Проверка, что все вершины получили сообщения
    for (auto vertex : vertex_pointers) {
        ASSERT_NE(vertex->messages_received(), 0);
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
