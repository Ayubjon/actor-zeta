#include "actor-zeta/pregel/message_combiner.hpp"

namespace actor_zeta {
    namespace pregel {

        template<typename VertexID, typename MessageType>
        template <typename T, typename>
        MessageType message_combiner<VertexID, MessageType>::combine(const std::vector<MessageType>& messages) {
            // Реализация по умолчанию: суммирование числовых сообщений
            static_assert(std::is_arithmetic_v<MessageType>,
                          "Default combine() implementation only supports arithmetic message types. "
                          "Provide a specialized message_combiner for non-arithmetic types.");
            return std::accumulate(messages.begin(), messages.end(), static_cast<MessageType>(0));
        }

        template<typename VertexID, typename MessageType>
        MessageType message_combiner<VertexID, MessageType>::apply(const MessageType& combined_message, const MessageType& current_value) {
            // Реализация по умолчанию: замена текущего значения объединенным сообщением
            return combined_message;
        }

    } // namespace pregel
} // namespace actor_zeta
