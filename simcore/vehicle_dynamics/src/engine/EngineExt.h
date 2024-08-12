// Copyright 2024 Tencent Inc. All rights reserved.
//

#ifndef VEHICLEDYNAMICS_ENGINE_ENGINEEXT_H_
#define VEHICLEDYNAMICS_ENGINE_ENGINEEXT_H_
#include "Engine.h"
#include "inc/car_common.h"

namespace tx_car {
namespace power {

class MODULE_API EngineExt : public Engine {
 public:
  EngineExt(/* args */);
  ~EngineExt();

  /* Initial conditions function */
  void initialize();

  /* model step function */
  void step();

  /* model terminate function */
  void terminate();

 protected:
  bool initMDL();

  /* load parameter from json file */
  bool loadParam(const std::string& par_path);

  /* init model by loaded json file */
  bool initParam();

 private:
  bool parsingParameterFromJson(char* errorLog = nullptr);
  // parsing parameter from json file
};
}  // namespace power
}  // namespace tx_car

#endif  // VEHICLEDYNAMICS_ENGINE_ENGINEEXT_H_
