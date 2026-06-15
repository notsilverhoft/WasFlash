#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include "../tags.h"
#include "../../base/rect.h"
#include "../../base/shape.h"
#include "../../utils/trackSWF.h"

SWFTag getDefineMorphShapeTag(rawSWFTag& rawTag, int morphShapeVersion);