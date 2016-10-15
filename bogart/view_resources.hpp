#ifndef VIEW_RESOURCES_HPP
#define VIEW_RESOURCES_HPP

#include <string>

namespace bogart
{
  //------------------------------------------------------------------------------------------------
  //! Types
  //------------------------------------------------------------------------------------------------
  typedef unsigned int res_id;

  //------------------------------------------------------------------------------------------------
  //! Constants
  //------------------------------------------------------------------------------------------------
  const res_id RESOURCE_FORWARD_PIPELINE                    =  0x000;
  const res_id RESOURCE_FONT_MATERIAL                       =  0x010;
  const res_id RESOURCE_PANEL_MATERIAL                      =  0x020;
  const res_id RESOURCE_LIGHT_MATERIAL                      =  0x030;
  const res_id RESOURCE_SKYBOX                              =  0x040;
  const res_id RESOURCE_SPONZA_MODEL                        =  0x050;

  //------------------------------------------------------------------------------------------------
  //! Free functions
  //------------------------------------------------------------------------------------------------
  bool load_resources(const std::string& path);
  int get_resource(res_id id);
} // namespace bogart

#endif
