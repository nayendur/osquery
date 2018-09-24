/**
 *  Copyright (c) 2014-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under both the Apache 2.0 license (found in the
 *  LICENSE file in the root directory of this source tree) and the GPLv2 (found
 *  in the COPYING file in the root directory of this source tree).
 *  You may select, at your option, one of the above-listed licenses.
 */

#include <ctime>

#include <boost/algorithm/string/trim.hpp>

#include <osquery/core.h>
#include <osquery/flags.h>
#include <osquery/system.h>
#include <osquery/tables.h>

namespace osquery {

namespace tables {

QueryData gensearchtext(QueryContext& context) {
  Row r;  

  r["test"] = SQL_TEXT("sample");
  r["data"] = SQL_TEXT("Found the value");
  QueryData results;
  results.push_back(r);
  return results;
}
}
}
