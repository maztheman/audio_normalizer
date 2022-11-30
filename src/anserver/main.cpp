#include <drogon/drogon.h>
#include <fmt/format.h>

#include <queue/queue.h>
#include <processor/processor.h>

#include <thread>

using namespace drogon;
int main()
{
    std::atomic_bool interrupter = false;
    std::thread proc(anworker::processor_run, std::ref(interrupter));

    app()
        .setLogPath("./")
        .setLogLevel(trantor::Logger::kWarn)
        .addListener("0.0.0.0", 9889)
        .setThreadNum(16)
        .registerHandler("/sonarr",
                    [](const HttpRequestPtr& req,
                        std::function<void (const HttpResponsePtr &)> &&callback
                    )
                    {
                        Json::Value json;
                        if (anworker::worker_queue().enqueue(anworker::queue_item{anworker::item_type::sonarr, req->getBody().data()}))
                        {
                            json["result"] = "ok";
                            json["message"] = std::string("added_item");
                        }
                        else
                        {
                            json["result"] = "bad";
                            json["message"] = std::string("failed added_item");
                        }
                        auto resp = HttpResponse::newHttpJsonResponse(json);
                        callback(resp);
                    },
                    {Post})
        .run();

    interrupter = true;

    if (proc.joinable())
    {
        proc.join();
    }

    return 0;
}
