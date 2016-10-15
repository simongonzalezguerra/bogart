#include "bogart/view_resources.hpp"
#include "Horde3DUtils.h"
#include "Horde3D.h"

#include <vector>
#include <map>

namespace bogart
{
  //------------------------------------------------------------------------------------------------
  //! Internal helper functions and types.
  //------------------------------------------------------------------------------------------------
  namespace
  {
    //----------------------------------------------------------------------------------------------
    //! Types
    //----------------------------------------------------------------------------------------------
    class resource_definition
    {
    public:
      resource_definition(res_id id, int type, const std::string& path) :
        id(id), type(type), path(path)
      {

      }

      res_id id;
      int type;
      std::string path;
    };

    typedef std::vector<resource_definition> resource_definition_vector;
    typedef resource_definition_vector::iterator resource_definition_it;
    typedef std::map<res_id, H3DRes> resource_map;
    typedef resource_map::iterator resource_it;

    //----------------------------------------------------------------------------------------------
    //! Internal variables
    //----------------------------------------------------------------------------------------------
    resource_definition_vector resource_definitions;
    resource_map resources;

    //----------------------------------------------------------------------------------------------
    //! Internal helper functions
    //----------------------------------------------------------------------------------------------
    void add_resource(res_id id, int type, const std::string& path)
    {
      resource_definitions.push_back(resource_definition(id, type, path));
    }

    void fill_resource_definitions()
    {
      if (resource_definitions.empty()) {
        add_resource(RESOURCE_FORWARD_PIPELINE, H3DResTypes::Pipeline,   "pipelines/forward.pipeline.xml");
        add_resource(RESOURCE_FONT_MATERIAL   , H3DResTypes::Material,   "overlays/font.material.xml");
        add_resource(RESOURCE_PANEL_MATERIAL  , H3DResTypes::Material,   "overlays/panel.material.xml");
        add_resource(RESOURCE_LIGHT_MATERIAL  , H3DResTypes::Material,   "materials/light.material.xml");
        add_resource(RESOURCE_SKYBOX          , H3DResTypes::SceneGraph, "models/skybox/skybox.scene.xml");
        add_resource(RESOURCE_SPONZA_MODEL    , H3DResTypes::SceneGraph, "models/sponza/sponza.scene.xml");
      }
    }
  }

  //----------------------------------------------------------------------------------------------
  //! Public functions
  //----------------------------------------------------------------------------------------------
  bool load_resources(const std::string& content_dir)
  {
    fill_resource_definitions();

    resources.clear();
    for (resource_definition_it it = resource_definitions.begin(); it != resource_definitions.end(); it++) {
      resources[it->id] = h3dAddResource(it->type, it->path.c_str(), 0);
    }
    return h3dutLoadResourcesFromDisk(content_dir.c_str());
  }

  int get_resource(res_id id)
  {
    resource_it it = resources.find(id);
    if (it != resources.end()) {
      return it->second;
    }

    return 0;
  }
} // namespace bogart
