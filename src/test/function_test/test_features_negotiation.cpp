//
// Created by tangchao on 2021/11/19.
//

#include "rrdb/rrdb.code.definition.h"
#include "rrdb/rrdb_types.h"
#include "base/pegasus_const.h"

#include <dsn/dist/replication/replication_ddl_client.h>

#include <gtest/gtest.h>

using namespace ::dsn;
using namespace ::replication;
using namespace ::pegasus;

extern std::shared_ptr<replication_ddl_client> ddl_client;

class features_negotiation_test : public testing::Test {
public:
   void test_negotiate_call() {
       std::map<dsn::rpc_address, dsn::replication::node_status::type> nodes;
       auto error_code = ddl_client->list_nodes(dsn::replication::node_status::type::NS_ALIVE, nodes);
       ASSERT_EQ(error_code, dsn::ERR_OK);
       std::vector<dsn::rpc_address> negotiate_servers;
       replica_helper::load_meta_servers(negotiate_servers, PEGASUS_CLUSTER_SECTION_NAME.c_str(), "mycluster");
       for (auto iter = nodes.begin(); iter != nodes.end(); ++iter) {
            negotiate_servers.push_back(iter->first);
       }

       std::stringstream ss;
       for (int i = 0; i < negotiate_servers.size(); ++i) {
            ss << negotiate_servers[i].to_std_string() << ",";
       }
       dinfo("%d server need to be negotiated, they are : %s.", negotiate_servers.size(), ss.str().c_str());
       task_spec *sp =  task_spec::get(apps::RPC_CM_FEATURES_NEGOTIATE);
       auto timeout_ms = sp->rpc_timeout_milliseconds;
       if (timeout_ms >= sp->rpc_timeout_milliseconds)
           timeout_ms = 0;
       apps::negotiate_feature_request req;
       std::vector<rpc_response_task_ptr> response_tasks;
       std::atomic<int> success_count(0);
       std::atomic<int> failed_count(0);
       for (auto iter = negotiate_servers.begin(); iter != negotiate_servers.end(); ++iter) {
           auto msg = dsn::message_ex::create_request(apps::RPC_CM_FEATURES_NEGOTIATE, timeout_ms);
           marshall(msg, req);
           response_tasks.push_back(rpc::call(*iter, msg, nullptr, [&success_count, &failed_count, iter](dsn::error_code err, dsn::message_ex *req, dsn::message_ex *resp){
                if (err == dsn::ERR_OK) {
                   ++success_count;
                   apps::negotiate_feature_response response;
                   unmarshall(resp, response);
                   std::stringstream ss;
                   ss << response;
                   ddebug("do negotiate success with one server : %s, response : %s", iter->to_string(), ss.str().c_str());
                } else {
                   ++failed_count;
                   derror("do negotiate failed with one server : %s, error : %s", iter->to_string(), err.to_string());
                }
           }));
       }
       for (auto i = 0; i < response_tasks.size(); ++i) {
            bool wait_ret = response_tasks[i]->wait(1000);
            ASSERT_EQ(wait_ret, true);
       }
       ASSERT_EQ(success_count, response_tasks.size());
   }

protected:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

TEST_F(features_negotiation_test, test_negotiate_call) {
    test_negotiate_call();
}


