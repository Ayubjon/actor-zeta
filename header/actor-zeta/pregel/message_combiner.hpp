#pragma once

#include <vector>
#include <numeric>
#include <type_traits>

namespace actor_zeta {
    namespace pregel {

        template<typename VertexID, typename MessageType>
        class message_combiner {
        public:
            using vertex_id_type = VertexID;
            using message_type = MessageType;

            virtual ~message_combiner() = default;

            // Объединение нескольких сообщений, предназначенных для одной вершины.
            // По умолчанию включено только для арифметических типов.
            template <typename T = MessageType, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
            virtual MessageType combine(const std::vector<MessageType>& messages) {
                static_assert(std::is_arithmetic_v<MessageType>,
                              "Default combine() implementation supports only arithmetic message types. "
                              "Provide a specialized message_combiner for non-arithmetic types.");
                return std::accumulate(messages.begin(), messages.end(), static_cast<MessageType>(0));
            }

            // Применить объединенное сообщение к текущему значению вершины.
            virtual MessageType apply(const MessageType& combined_message, const MessageType& current_value) {
                return combined_message;
            }
        };

    } // namespace pregel
} // namespace actor_zeta
