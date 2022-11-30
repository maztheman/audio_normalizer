#include <drogon/drogon.h>
#include <fmt/format.h>

using namespace drogon;
int main()
{
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
                        fmt::print("{}\n", req->getBody());
                        Json::Value json;
                        json["result"] = "ok";
                        json["message"] = std::string("hello,");
                        auto resp = HttpResponse::newHttpJsonResponse(json);
                        callback(resp);
                    },
                    {Post})
        .run();
    return 0;
}
