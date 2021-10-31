#ifndef TOMATO_TEMPLATE_H
#define TOMATO_TEMPLATE_H
#include <string>
namespace tomato {
class TEMPLATE {
public:
#define DEF_TYPE_NAME(TYPE, INIT) static std::string TYPE;
#include "inc/path.def"
#undef DEF_TYPE_NAME
};
#define DEF_TYPE_NAME(TYPE, INIT) std::string TEMPLATE::TYPE = INIT;
#include "inc/path.def"
#undef DEF_TYPE_NAME

class TAG {
public:
#define DEF_TYPE_NAME(TYPE, INIT) static std::string TYPE;
#include "inc/tag.def"
#undef DEF_TYPE_NAME
};

#define DEF_TYPE_NAME(TYPE, INIT) std::string TAG::TYPE = INIT;
#include "inc/tag.def"
#undef DEF_TYPE_NAME

} // end namespace tomato
#endif // TOMATO_TEMPLATE_H