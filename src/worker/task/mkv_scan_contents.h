#pragma once

namespace anworker::task {

struct sonarr_type;
struct context;

void mkv_scan_contents(sonarr_type &type, context &context);

}// namespace anworker::task