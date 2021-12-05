//
// Created by tangchao on 2021/11/19.
//

#ifndef PEGASUS_FEATURES_NEGOTIATION_H
#define PEGASUS_FEATURES_NEGOTIATION_H

#include "rrdb/rrdb_types.h"

#include "dsn/cpp/serverlet.h"
#include "dsn/cpp/rpc_holder.h"

namespace pegasus{
    namespace server {
        typedef dsn::rpc_holder<dsn::apps::negotiate_feature_request, dsn::apps::negotiate_feature_response> negotiate_features_rpc;

        class features_negotiation : public dsn::serverlet<features_negotiation>
        {
        public:
            features_negotiation() : dsn::serverlet<features_negotiation>("features_negotiation") {}
            void open_service();
            void negotiate_features(negotiate_features_rpc rpc);
        };
    }
}


#endif // PEGASUS_FEATURES_NEGOTIATION_H
