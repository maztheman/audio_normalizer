#include "eval.h"

#include <fmt/format.h>
#include <sqlite3.h>

namespace anworker::task
{

float eval_asfloat(const std::string &value)
{
    sqlite3 *db = nullptr;
    int rc = sqlite3_open(":memory:", &db);

    auto sql = fmt::format("SELECT ROUND(1.0 * ({}), 3)", value);

    float retval = 0.0f;


    rc = sqlite3_exec(
      db,
      sql.c_str(),
      [](void *userData, int cols, char **data, char **column_names) {
          float *outtie = reinterpret_cast<float *>(userData);

          if (cols < 1)
          {
              *outtie = 0.0f;
              return 1;
          }

          *outtie = std::stof(data[0]);
      },
      &retval,
      nullptr);

    return retval;
}

}// namespace anworker::task
