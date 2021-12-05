//
// Created by tangchao on 2021/11/19.
//

#include "features_negotiation.h"

#include "rrdb/rrdb.code.definition.h"

namespace pegasus{
namespace server {
    void features_negotiation::open_service() {
        register_rpc_handler_with_rpc_holder(
            dsn::apps::RPC_CM_FEATURES_NEGOTIATE, "negotiate_features", &features_negotiation::negotiate_features);
    }

    void features_negotiation::negotiate_features(negotiate_features_rpc rpc) {
        auto &response = rpc.response();
        response.supported_features.push_back(dsn::apps::negotiate_feature_flag::BATCH_GET);
    }
}
}

