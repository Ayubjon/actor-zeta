#pragma once

#include <cstdint>

namespace actor_zeta {
    namespace pregel {

        template<typename VertexID, typename MessageType>
        struct pregel_message {
            VertexID source_vertex;
            VertexID target_vertex;
            MessageType message;
        };

        struct vote_to_halt_message final {};

        struct compute_message final {};

    } // namespace pregel
} // namespace actor_zeta
