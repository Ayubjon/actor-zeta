#include <iostream>
#include <iomanip>
#include <numeric>
#include <random>

#include <actor-zeta.hpp>
#include <actor-zeta/pregel/pregel_actor.hpp>
#include <actor-zeta/pregel/bsp_coordinator.hpp>
#include <actor-zeta/pregel/message_combiner.hpp>

using namespace actor_zeta;
using namespace actor_zeta::pregel;

// Типы данных для PageRank
using vertex_id_type = std::uint64_t;
using vertex_value_type = double;
using edge_value_type = double;
using message_type = double;

class pagerank_vertex : public pregel_actor<vertex_id_type, vertex_value_type, edge_value_type, message_type> {
public:
    pagerank_vertex(actor_zeta::supervisor_abstract &supervisor,
                    const vertex_id_type& vertex_id,
                    const vertex_value_type& initial_value,
                    double damping_factor = 0.85)
        : pregel_actor(supervisor, vertex_id, initial_value),
        damping_factor_(damping_factor),
        messages_received_(0),
        total_received_(0.0) {}

    void compute(actor_zeta::message_ptr msg) override {
        if (get_context().superstep() >= 1) {
            msg->unpack([&](double value) {
                total_received_ += value;
                messages_received_++;
            });
        }

        if (messages_received_ == expected_messages_) {
            vertex_value_type new_value = (1.0 - damping_factor_) + damping_factor_ * total_received_;
            get_context().set_new_value(new_value);
            messages_received_ = 0;
            total_received_ = 0.0;
        }

        // Отправляем PageRank всем соседям
        if (get_context().superstep() < 30) { // Ограничение на количество супершагов
            const auto& edges = get_edges();
            for (const auto& edge : edges) {
                get_context().send_message(edge.first, value() / edges.size());
            }
            get_context().vote_to_halt();
        } else {
            std::cout << "Vertex " << id() << ": Halting after max iterations" << std::endl;
            get_context().vote_to_halt();
        }
    }

    void set_expected_messages(size_t count) {
        expected_messages_ = count;
    }

private:
    double damping_factor_;
    size_t expected_messages_;
    size_t messages_received_;
    double total_received_;
};

int main() {
    // Настройка actor-zeta
    actor_zeta::environment_t env;
    actor_zeta::supervisor_t supervisor(env);

    // Создание графа
    std::map<vertex_id_type, vertex_value_type> vertices = {
        {1, 1.0},
        {2, 1.0},
        {3, 1.0},
        {4, 1.0}
    };

    std::map<vertex_id_type, std::map<vertex_id_type, edge_value_type>> edges = {
        {1, {{2, 1.0}, {3, 1.0}, {4, 1.0}}},
        {2, {{1, 1.0}, {3, 1.0}}},
        {3, {{4, 1.0}}},
        {4, {{1, 1.0}}}
    };

    // Создание координатора
    auto coordinator = supervisor.make_actor<bsp_coordinator<vertex_id_type, vertex_value_type, edge_value_type, message_type>>();
    auto coordinator_addr = coordinator->address();

    // Создание вершин
    std::vector<actor_zeta::address_t> vertex_actors;
    std::vector<pagerank_vertex*> vertex_pointers;
    for (const auto& vertex : vertices) {
        auto vertex_actor = supervisor.make_actor<pagerank_vertex>(vertex.first, vertex.second);
        vertex_actors.push_back(vertex_actor->address());
        vertex_pointers.push_back(vertex_actor.get());

        // Регистрация вершины у координатора
        actor_zeta::send(coordinator_addr, vertex_actor->address(), "register");

        // Добавление ребер
        size_t edge_count = 0;
        for (const auto& edge : edges[vertex.first]) {
            vertex_actor->add_edge(edge.first, edge.second);
            edge_count++;
        }

        vertex_actor->set_expected_messages(edge_count);
    }

    // Установка адреса координатора в контексте каждой вершины
    for (auto vertex : vertex_pointers) {
        vertex->get_context().set_coordinator_address(coordinator_addr);
    }

    // Запуск Pregel
    actor_zeta::send(coordinator_addr, coordinator_addr, "start");

    // Ожидание завершения
    env.await_actors_completion();

    // Вывод результатов
    for (const auto& addr : vertex_actors) {
        auto vertex = actor_zeta::actor_cast<pagerank_vertex>(addr);
        std::cout << "Vertex " << vertex->id() << ": PageRank = " << std::fixed << std::setprecision(4) << vertex->value() << std::endl;
    }

    return 0;
}
