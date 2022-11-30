#pragma once


namespace anworker::task
{

struct sonarr_type;
struct context;

void scan_file_for_container(sonarr_type& type, context& context);



}