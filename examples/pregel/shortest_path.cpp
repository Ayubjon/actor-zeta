#include <iostream>
#include <limits>
#include <iomanip>
#include <vector>
#include <algorithm>

#include <actor-zeta.hpp>
#include <actor-zeta/pregel/pregel_actor.hpp>
#include <actor-zeta/pregel/bsp_coordinator.hpp>
#include <actor-zeta/pregel/message_combiner.hpp>
#include <actor-zeta/scheduler/scheduler.hpp> // для создания задачи

using namespace actor_zeta;
using namespace actor_zeta::pregel;

// Типы данных для Shortest Path
using vertex_id_type = std::uint64_t;
using vertex_value_type = double; // Расстояние от источника
using edge_value_type = double;   // Edge weight
using message_type = double;      // Дистанция

class shortest_path_vertex : public pregel_actor<vertex_id_type, vertex_value_type, edge_value_type, message_type> {
public:
    shortest_path_vertex(actor_zeta::supervisor_abstract &supervisor,
                         const vertex_id_type& vertex_id,
                         const vertex_value_type& initial_value,
                         const vertex_id_type& source_vertex)
        : pregel_actor(supervisor, vertex_id, initial_value),
        source_vertex_(source_vertex) {}

    void compute(actor_zeta::message_ptr msg) override {
        if (get_context().superstep() == 0 && id() == source_vertex_) {
            // Исходная вершина отправляет сообщение с расстоянием 0 своим соседям
            const auto& edges = get_edges();
            std::vector<actor_zeta::scheduler::task> tasks;
            for (const auto& edge : edges) {
                tasks.emplace_back([this, edge]() {
                    get_context().send_message(edge.first, 0.0);
                });
            }
            for (auto& task : tasks) {
                actor_zeta::scheduler::make_task(std::move(task));
            }
            get_context().vote_to_halt();
            return;
        }

        double min_distance = std::numeric_limits<double>::max();
        bool message_received = false;

        msg->unpack([&](double value) {
            min_distance = std::min(min_distance, value);
            message_received = true;
        });

        if (message_received) {
            if (min_distance < value()) {
                get_context().set_new_value(min_distance);

                // Отправляем новое расстояние всем соседям
                const auto& edges = get_edges();
                std::vector<actor_zeta::scheduler::task> tasks;
                for (const auto& edge : edges) {
                    tasks.emplace_back([this, edge, min_distance]() {
                        get_context().send_message(edge.first, min_distance + edge.second);
                    });
                }
                for (auto& task : tasks) {
                    actor_zeta::scheduler::make_task(std::move(task));
                }
            }
        }
        get_context().vote_to_halt();

    }

private:
    vertex_id_type source_vertex_;
};

class min_double_combiner : public message_combiner<vertex_id_type, message_type> {
public:
    message_type combine(const std::vector<message_type>& messages) override {
        if (messages.empty()) {
            return std::numeric_limits<double>::max();
        }
        return *std::min_element(messages.begin(), messages.end());
    }

    message_type apply(const message_type& combined_message, const message_type& current_value) override {
        return std::min(combined_message, current_value);
    }
};

int main() {
    // Настройка actor-zeta
    actor_zeta::environment_t env;
    actor_zeta::supervisor_t supervisor(env);

    // Создание графа
    std::map<vertex_id_type, vertex_value_type> vertices; // Расстояние от источника
    std::map<vertex_id_type, std::map<vertex_id_type, edge_value_type>> edges; // Edge weight

    vertex_id_type source_vertex = 1;

    vertices = {
        {1, std::numeric_limits<double>::max()},
        {2, std::numeric_limits<double>::max()},
        {3, std::numeric_limits<double>::max()},
        {4, std::numeric_limits<double>::max()},
        {5, std::numeric_limits<double>::max()}
    };

    edges = {
        {1, {{2, 1.0}, {4, 5.0}}},
        {2, {{1, 1.0}, {3, 2.0}, {5, 7.0}}},
        {3, {{2, 2.0}, {5, 4.0}}},
        {4, {{1, 5.0}, {5, 3.0}}},
        {5, {{2, 7.0}, {3, 4.0}, {4, 3.0}}}
    };


    // Создание координатора
    auto coordinator = supervisor.make_actor<bsp_coordinator<vertex_id_type, vertex_value_type, edge_value_type, message_type>>();
    auto coordinator_addr = coordinator->address();

    // Создание вершин
    std::vector<actor_zeta::address_t> vertex_actors;
    for (const auto& vertex : vertices) {
        auto vertex_actor = supervisor.make_actor<shortest_path_vertex>(vertex.first, vertex.second, source_vertex);
        vertex_actors.push_back(vertex_actor->address());

        // Регистрация вершины у координатора
        actor_zeta::send(coordinator_addr, vertex_actor->address(), "register");

        // Добавление ребер
        for (const auto& edge : edges[vertex.first]) {
            vertex_actor->add_edge(edge.first, edge.second);
        }
    }

    // Установка адреса координатора в контексте каждой вершины
    for (const auto& addr : vertex_actors) {
        auto vertex = actor_zeta::actor_cast<shortest_path_vertex>(addr);
        vertex->get_context().set_coordinator_address(coordinator_addr);
    }


    // Запуск Pregel
    actor_zeta::send(coordinator_addr, coordinator_addr, "start");

    // Ожидание завершения
    env.await_actors_completion();

    // Вывод результатов
    for (const auto& addr : vertex_actors) {
        auto vertex = actor_zeta::actor_cast<shortest_path_vertex>(addr);
        std::cout << "Vertex " << vertex->id() << ": Shortest Path = " << std::fixed << std::setprecision(1) << vertex->value() << std::endl;
    }

    return 0;
}
